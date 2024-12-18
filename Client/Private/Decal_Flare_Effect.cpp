#include "Decal_Flare_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"

wstring CDecal_Flare_Effect::ObjID = TEXT("CDecal_Flare_Effect");

CDecal_Flare_Effect::CDecal_Flare_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CDecal_Flare_Effect::ObjID);
}

CDecal_Flare_Effect::CDecal_Flare_Effect(const CDecal_Flare_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CDecal_Flare_Effect::~CDecal_Flare_Effect()
{
	Free();
}

HRESULT CDecal_Flare_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecal_Flare_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(4.f, 0.25f, 4.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	SetColorPresetRGB(_float4{ 2.f,1.0f,0.0f,1.f }, _float4{ 0.f,0.0f,0.0f,1.f }, _float4{ 0.0f,0.0f,0.0f,1.f });
	m_vEffectBaseColor = _float4{ 7.f,0.5f,0.f,1.f };
	EffectInit();
	return S_OK;
}

void CDecal_Flare_Effect::PriorityTick(_float fTimeDelta)
{
}

void CDecal_Flare_Effect::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CDecal_Flare_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_DECAL, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CDecal_Flare_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	_float fDissolveThreshold = max(m_fEffectTimePercent * 2.f - 1.f, 0.f);

	if (GetUseDistortion()) fDissolveThreshold = 0.f;

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", fDissolveThreshold);

	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_Position", m_pShaderCom, "g_PositionTexture");

	_float4x4 Temp;
	_float fTimeRatio = 1.f;

	if (GetUseGlow())
		fTimeRatio = max(m_fEffectTimePercentReverse * 2.f, 1.f);

	if (m_pParentWorld)
	{
		fTimeRatio = max(m_fEffectTimePercent * 2.f, 1.f);
	}

	XMStoreFloat4x4(&Temp, XMMatrixScaling(fTimeRatio, 1.f, fTimeRatio) * m_pTransformCom->Get_WorldMatrix_XMMat());

	_float4x4 WorldInv = m_pTransformCom->Get_WorldMatrix_Inversed();

	if (GetUseGlow())
		XMStoreFloat4x4(&WorldInv, XMMatrixScaling(fTimeRatio, 1.f, fTimeRatio) * XMLoadFloat4x4(&WorldInv));

	if (m_pParentWorld)
	{
		_matrix Parent = XMLoadFloat4x4(&Temp) * XMLoadFloat4x4(m_pParentWorld);
		_float4x4 Temp;
		XMStoreFloat4x4(&Temp, Parent);
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);

		XMStoreFloat4x4(&WorldInv, XMMatrixInverse(nullptr, Parent));
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp)))
			return E_FAIL;
	}
	m_pShaderCom->Bind_Matrix("g_WorldInvMatrix", &WorldInv);

	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));


	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2D", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)CUBE_PASS::SSD)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;*/

		//if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		//	return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CDecal_Flare_Effect::EffectStartCallBack()
{

}

void CDecal_Flare_Effect::EffectEndCallBack()
{

}

void CDecal_Flare_Effect::EffectSoftEnd()
{

}

void CDecal_Flare_Effect::EffectHardEnd()
{

}

HRESULT CDecal_Flare_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Cube>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_tk_flare_tc_03"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_tk_flare_13b"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CDecal_Flare_Effect> CDecal_Flare_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CDecal_Flare_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDecal_Flare_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CDecal_Flare_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDecal_Flare_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CDecal_Flare_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CDecal_Flare_Effect
	{
		MakeSharedEnabler(const CDecal_Flare_Effect& rhs) : CDecal_Flare_Effect(rhs) {}
	};

	shared_ptr<CDecal_Flare_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDecal_Flare_Effect"));
		assert(false);
	}

	return pInstance;
}

void CDecal_Flare_Effect::Free()
{
}