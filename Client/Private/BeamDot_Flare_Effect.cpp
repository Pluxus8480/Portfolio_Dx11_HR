#include "BeamDot_Flare_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"

wstring CBeamDot_Flare_Effect::ObjID = TEXT("CBeamDot_Flare_Effect");

CBeamDot_Flare_Effect::CBeamDot_Flare_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CBeamDot_Flare_Effect::ObjID);
}

CBeamDot_Flare_Effect::CBeamDot_Flare_Effect(const CBeamDot_Flare_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CBeamDot_Flare_Effect::~CBeamDot_Flare_Effect()
{
	Free();
}

HRESULT CBeamDot_Flare_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	//m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_T_VFX_tk_flare_13b"), TEXT("../Bin/Resources/Effect/Flare/T_VFX_tk_flare_13b.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_T_VFX_tk_flare_tc_03"), TEXT("../Bin/Resources/Effect/Flare/T_VFX_tk_flare_tc_03.dds"), 1));

	return S_OK;
}

HRESULT CBeamDot_Flare_Effect::Initialize(void* pArg)
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

void CBeamDot_Flare_Effect::PriorityTick(_float fTimeDelta)
{
	auto pGameInstance = m_pGameInstance.lock();
	if (pGameInstance->Get_KeyPressing(DIK_RETURN))
	{
		_float3 vScale =
			m_pTransformCom->Get_Scale();
		vScale.x += fTimeDelta * 10.f;
		vScale.y += fTimeDelta * 10.f;
		vScale.z += fTimeDelta * 10.f;
		m_pTransformCom->Set_Scale(vScale);
	}
	if (pGameInstance->Get_KeyPressing(DIK_RSHIFT))
	{
		_float3 vScale =
			m_pTransformCom->Get_Scale();
		vScale.x -= fTimeDelta * 10.f;
		vScale.y -= fTimeDelta * 10.f;
		vScale.z -= fTimeDelta * 10.f;
		m_pTransformCom->Set_Scale(vScale);
	}

}

void CBeamDot_Flare_Effect::Tick(_float fTimeDelta)
{
	//SetEffectLoop(true);
	//SetEffectTimeFreeze(false);
	//SetEffectColorBlendRate(1.f);


	EffectTick(fTimeDelta);
}

void CBeamDot_Flare_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CBeamDot_Flare_Effect::Render()
{
	auto pGameInstance = m_pGameInstance.lock();
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fTimeRatio = 1.f;

	if(GetUseGlow()) 
		fTimeRatio = max(m_fEffectTimePercent * 2.f, 0.5f);

	XMStoreFloat4x4(&Temp, XMMatrixScaling(fTimeRatio, fTimeRatio, fTimeRatio) * m_pTransformCom->Get_WorldMatrix_XMMat());

	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(&Temp), XMLoadFloat4x4(m_pParentWorld)));
		if (m_pParentUnion.lock()->GetBillboard()) {
			ParentMultipleWorld = GAMEINSTANCE->Matrix_BillBoard(ParentMultipleWorld);
		}
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorR", &m_vEffectColorPresets[0], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[1], sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[2], sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));


	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float));
	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowXSpeed, m_fEffectUvFlowYSpeed };
	m_pShaderCom->Bind_RawValue("g_vUVFlowSpeed", &vFlowSpeed, sizeof(_float2));
	_float2 vUvMultiplier = _float2{ m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier };

	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUvMultiplier, sizeof(_float2));
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", powf(cos(m_fEffectTimePercent * XM_PI), 2.f));

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::MaskFlare)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CBeamDot_Flare_Effect::EffectStartCallBack()
{
}

void CBeamDot_Flare_Effect::EffectEndCallBack()
{

}

void CBeamDot_Flare_Effect::EffectSoftEnd()
{
}

void CBeamDot_Flare_Effect::EffectHardEnd()
{
}

HRESULT CBeamDot_Flare_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_tk_flare_tc_03"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_tk_flare_13b"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CBeamDot_Flare_Effect> CBeamDot_Flare_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBeamDot_Flare_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBeamDot_Flare_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CBeamDot_Flare_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBeamDot_Flare_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CBeamDot_Flare_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBeamDot_Flare_Effect
	{
		MakeSharedEnabler(const CBeamDot_Flare_Effect& rhs) : CBeamDot_Flare_Effect(rhs) {}
	};

	shared_ptr<CBeamDot_Flare_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBeamDot_Flare_Effect"));
		assert(false);
	}

	return pInstance;
}

void CBeamDot_Flare_Effect::Free()
{
}