#include "Effect_Line.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CEffect_Line::ObjID = TEXT("CEffect_Line");

CEffect_Line::CEffect_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_Line::ObjID);
}

CEffect_Line::CEffect_Line(const CEffect_Line& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_Line::~CEffect_Line()
{
	Free();
}

HRESULT CEffect_Line::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_T_VFX_tk_flare_03"), TEXT("../Bin/Resources/Effect/Flare/T_VFX_tk_flare_03.png"), 1));
	//m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_T_VFX_tk_flare_tc_03"), TEXT("../Bin/Resources/Effect/Flare/T_VFX_tk_flare_tc_03.dds"), 1));

	return S_OK;
}

HRESULT CEffect_Line::Initialize(void* pArg)
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

	m_vEffectColorPresets.push_back(_float4{ 1.f,1.0f,1.0f,5.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.5f,1.0f, 5.f });
	m_vEffectColorPresets.push_back(_float4{ 0.5f,0.5f,1.0f,5.f });
	m_vEffectBaseColor = _float4{ 0.5f,0.5f,1.5f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CEffect_Line::PriorityTick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_RETURN))
	{
		_float3 vScale =
			m_pTransformCom->Get_Scale();
		vScale.x += fTimeDelta * 10.f;
		vScale.y += fTimeDelta * 10.f;
		vScale.z += fTimeDelta * 10.f;
		m_pTransformCom->Set_Scale(vScale);
	}
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_RSHIFT))
	{
		_float3 vScale =
			m_pTransformCom->Get_Scale();
		vScale.x -= fTimeDelta * 10.f;
		vScale.y -= fTimeDelta * 10.f;
		vScale.z -= fTimeDelta * 10.f;
		m_pTransformCom->Set_Scale(vScale);
	}

}

void CEffect_Line::Tick(_float fTimeDelta)
{
	//SetEffectLoop(true);
	//SetEffectTimeFreeze(false);
	//SetEffectColorBlendRate(1.f);


	EffectTick(fTimeDelta);
}

void CEffect_Line::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		if (GetUseGlow())
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		else
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CEffect_Line::Render()
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

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	//m_pShaderCom->Bind_RawValue("g_vEffectBaseColorR", &m_vEffectColorPresets[0], sizeof(_float4));
	//m_pShaderCom->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[1], sizeof(_float4));
	//m_pShaderCom->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[2], sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));


	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));
	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowXSpeed, m_fEffectUvFlowYSpeed };
	m_pShaderCom->Bind_RawValue("g_vUVFlowSpeed", &vFlowSpeed, sizeof(_float2));
	_float2 vUvMultiplier = _float2{ m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier };

	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUvMultiplier, sizeof(_float2));

	_float fDissolve = 0.f;

	if(m_fEffectTimePercent < 0.2f)
		fDissolve = m_fEffectTimePercent * 5.f;
	else if (m_fEffectTimePercent > 0.8f)
		fDissolve = (1.f - m_fEffectTimePercent) * 5.f;

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", fDissolve);

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::Flare)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Line::EffectStartCallBack()
{
}

void CEffect_Line::EffectEndCallBack()
{

}

void CEffect_Line::EffectSoftEnd()
{
}

void CEffect_Line::EffectHardEnd()
{
}

HRESULT CEffect_Line::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Line"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Line"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_Line> CEffect_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Line
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Line(pDevice, pContext)
		{}
	};


	shared_ptr<CEffect_Line> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Line"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEffect_Line::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Line
	{
		MakeSharedEnabler(const CEffect_Line& rhs) : CEffect_Line(rhs) {}
	};

	shared_ptr<CEffect_Line> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Line"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Line::Free()
{
}