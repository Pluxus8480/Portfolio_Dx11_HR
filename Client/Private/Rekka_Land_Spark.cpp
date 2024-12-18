#include "Rekka_Land_Spark.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CRekka_Land_Spark::ObjID = TEXT("CRekka_Land_Spark");

CRekka_Land_Spark::CRekka_Land_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CRekka_Land_Spark::ObjID);
}

CRekka_Land_Spark::CRekka_Land_Spark(const CRekka_Land_Spark& rhs)
	: CEffect_Base(rhs)
{
}

CRekka_Land_Spark::~CRekka_Land_Spark()
{
	Free();
}

HRESULT CRekka_Land_Spark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	//m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_StarSymbol_Scratch"), TEXT("../Bin/Resources/Effect/Symbol/T_VFX_tk_scratch_symbol_Star_04.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Zap"), TEXT("../Bin/Resources/Effect/Zap/T_VFX_yg_lightning_04_%d.png"), 8));

	return S_OK;
}

HRESULT CRekka_Land_Spark::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(48.f, 48.f, 1.f));

	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	m_vEffectColorPresets.push_back(_float4{ 1.f,1.f,0.0f,5.f });
	m_vEffectColorPresets.push_back(_float4{ 1.0f, 0.5f,0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.0f,0.0f,0.0f,1.f });
	m_vEffectBaseColor = _float4{ 0.0f,0.9f,0.9f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CRekka_Land_Spark::PriorityTick(_float fTimeDelta)
{


}

void CRekka_Land_Spark::Tick(_float fTimeDelta)
{
	//m_Local.m[3][0] += m_vDir.x * fTimeDelta;
	//m_Local.m[3][1] += m_vDir.y * fTimeDelta;
	//m_Local.m[3][2] += m_vDir.z * fTimeDelta;
	m_pTextureCom->Set_Speed(m_fEffectTimeAccMultiplier * 2.f);
	m_pTextureCom->MoveFrame(fTimeDelta);
	//m_pTransformCom->LookAt_Horizontal(m_pGameInstance.lock()->Get_CamPosition_Vector());
	EffectTick(fTimeDelta);
}

void CRekka_Land_Spark::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CRekka_Land_Spark::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()), XMLoadFloat4x4(m_pParentWorld)));

		if (m_pParentUnion.lock()->GetBillboard()) {
			ParentMultipleWorld = GAMEINSTANCE->Matrix_BillBoard(ParentMultipleWorld);
		}
		XMStoreFloat4x4(&ParentMultipleWorld, XMLoadFloat4x4(&m_Local) * XMLoadFloat4x4(&ParentMultipleWorld));
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
			return E_FAIL;
	}
	else
	{
		_float4x4 Temp = m_pTransformCom->Get_WorldMatrix();
		XMStoreFloat4x4(&Temp, XMLoadFloat4x4(&m_Local) * XMLoadFloat4x4(&Temp));
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));


	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));
	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowXSpeed, m_fEffectUvFlowYSpeed };
	m_pShaderCom->Bind_RawValue("g_vUVFlowSpeed", &vFlowSpeed, sizeof(_float2));

	_float2 vUvMultiplier = _float2{ m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier };
	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUvMultiplier, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_fEffectAlpha", m_bEffectAlphaLinkTimePer ? &m_fEffectTimePercentReverse : &m_fEffectAlpha, sizeof(_float));

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_Frame())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CRekka_Land_Spark::EffectStartCallBack()
{
	_float fDegree = GAMEINSTANCE->Random_Float(0.f, XM_2PI);
	XMStoreFloat3(&m_vDir, XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMQuaternionRotationRollPitchYaw(0.f, 0.f, fDegree)));

	m_fSize = GAMEINSTANCE->Random_Float(1.f, 2.f);
	XMStoreFloat4x4(&m_Local, XMMatrixAffineTransformation(XMVectorSet(m_fSize * 0.125f, m_fSize, m_fSize, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
		XMQuaternionRotationRollPitchYaw(0.f, 0.f, fDegree), XMVectorSetW(XMLoadFloat3(&m_vDir) * 0.7f, 1.f)));
}

void CRekka_Land_Spark::EffectEndCallBack()
{

}

void CRekka_Land_Spark::EffectSoftEnd()
{
}

void CRekka_Land_Spark::EffectHardEnd()
{
}

HRESULT CRekka_Land_Spark::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Zap"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CRekka_Land_Spark> CRekka_Land_Spark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRekka_Land_Spark
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Land_Spark(pDevice, pContext)
		{}
	};


	shared_ptr<CRekka_Land_Spark> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_Land_Spark"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CRekka_Land_Spark::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_Land_Spark
	{
		MakeSharedEnabler(const CRekka_Land_Spark& rhs) : CRekka_Land_Spark(rhs) {}
	};

	shared_ptr<CRekka_Land_Spark> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_Land_Spark"));
		assert(false);
	}

	return pInstance;
}

void CRekka_Land_Spark::Free()
{
}