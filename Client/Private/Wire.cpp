#include "Wire.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CWire::ObjID = TEXT("CWire");

CWire::CWire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CWire::ObjID);
}

CWire::CWire(const CWire& rhs)
	: CEffect_Base(rhs)
{
}

CWire::~CWire()
{
	Free();
}

HRESULT CWire::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Wire"), TEXT("../Bin/Resources/Effect/Wire/Wire.png"), 1));


	return S_OK;
}

HRESULT CWire::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(4.f, 2.f, 1.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));


	m_vEffectColorPresets.push_back({ 0.3f,0.3f,1.f,1.f });
	m_vEffectColorPresets.push_back({ 1.f,1.f,1.f,1.f });
	m_vEffectColorPresets.push_back({ 0.f,0.f,0.f,0.f });
	m_vEffectBaseColor = { 1.f,0.2f,0.f,4.f };
	m_vStartPos = { 0.f,0.f,0.f,0.f };
	m_vEndPos = { 4.f,4.f,4.f,0.f };
	m_bEffectLoop = true;

	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CWire::PriorityTick(_float fTimeDelta)
{

}

void CWire::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
	
	_vector vStart, vEnd, vDiff, vMid;
	vStart = XMLoadFloat4(&m_vStartPos);
	vEnd = XMLoadFloat4(&m_vEndPos);
	vDiff = vEnd - vStart;
	vMid = vStart + vDiff * .5f;
	_vector vLook = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(vDiff));
	m_pTransformCom->Set_Position(vMid);
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vDiff);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(XMVector3Cross(XMVector3Normalize(vDiff), vLook)) * vScale.y);
	m_fEffectUvSizeXMultiplier = XMVectorGetX(XMVector3Length(vDiff)) / vScale.y;
}

void CWire::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CWire::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//_float4x4 matTemp;
	//XMStoreFloat4x4(&matTemp, XMLoadFloat4x4(&m_matRotation) * m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matTemp);
	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));


	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorR", &m_vEffectColorPresets[0], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[1], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[2], sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));
	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowXSpeed, m_fEffectUvFlowYSpeed };
	m_pShaderCom->Bind_RawValue("g_vUVFlowSpeed", &vFlowSpeed, sizeof(_float2));

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);

	_float2 vUvMultiplier = _float2{ m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier };

	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUvMultiplier, sizeof(_float2));


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::WeightBlendMonoColor)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CWire::EffectStartCallBack()
{
}

void CWire::EffectEndCallBack()
{

}

void CWire::EffectSoftEnd()
{
}

void CWire::EffectHardEnd()
{
}

HRESULT CWire::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wire"), TEXT("Com_Texture")));


	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CWire> CWire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CWire
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CWire(pDevice, pContext)
		{}
	};


	shared_ptr<CWire> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWire"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CWire::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CWire
	{
		MakeSharedEnabler(const CWire& rhs) : CWire(rhs) {}
	};

	shared_ptr<CWire> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWire"));
		assert(false);
	}

	return pInstance;
}

void CWire::Free()
{
}