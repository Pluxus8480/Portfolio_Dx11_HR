#include "Smoke_Explode_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h" 
#include "VIBuffer_Rect.h"

wstring CSmoke_Explode_Effect::ObjID = TEXT("CSmoke_Explode_Effect");

CSmoke_Explode_Effect::CSmoke_Explode_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CSmoke_Explode_Effect::ObjID);
}

CSmoke_Explode_Effect::CSmoke_Explode_Effect(const CSmoke_Explode_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CSmoke_Explode_Effect::~CSmoke_Explode_Effect()
{
	Free();
}

HRESULT CSmoke_Explode_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSmoke_Explode_Effect::Initialize(void* pArg)
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

	m_vEffectColorPresets.push_back(_float4{ 1.0f,0.8f,0.4f,1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.7f, 0.7f,0.7f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.5f,0.5f,1.0f,1.f });
	m_vEffectBaseColor = _float4{ 1.0f,1.0f,1.0f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CSmoke_Explode_Effect::PriorityTick(_float fTimeDelta)
{


}

void CSmoke_Explode_Effect::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CSmoke_Explode_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CSmoke_Explode_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = 1.f;
	_float fScale2 = 1.f;

	if (GetUseGlow()) {
		if (m_fEffectTimePercent < 0.3f)
			fScale = m_fEffectTimePercent * 3.3f;
		else
			fScale = 1.f;

		if (m_fEffectTimePercent > 0.3f)
			fScale2 = 1.f + (2.f * (m_fEffectTimePercent - 0.3f));
	}

	XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale * fScale2, 1.f * fScale2, 1.f) * m_pTransformCom->Get_WorldMatrix_XMMat());


	if (GetUseGlow()) {
		_float4 vTempRight;
		XMStoreFloat4(&vTempRight, m_pTransformCom->Get_NormRight());

		_float4 vTempUp;
		XMStoreFloat4(&vTempUp, m_pTransformCom->Get_NormUp());

		if (m_fEffectTimePercent < 0.3f) {
			Temp._41 += vTempUp.x * 0.5f * m_fEffectTimePercent * 10.f;
			Temp._42 += vTempUp.y * 0.5f * m_fEffectTimePercent * 10.f;
			Temp._43 += vTempUp.z * 0.5f * m_fEffectTimePercent * 10.f;
		}
		else {
			Temp._41 += vTempUp.x * 0.5f * 3.f;
			Temp._42 += vTempUp.y * 0.5f * 3.f;
			Temp._43 += vTempUp.z * 0.5f * 3.f;
		}

		Temp._41 += vTempRight.x * 0.5f * m_fEffectTimePercent * 50.f;
		Temp._42 += vTempRight.y * 0.5f * m_fEffectTimePercent * 50.f;
		Temp._43 += vTempRight.z * 0.5f * m_fEffectTimePercent * 50.f;

	}


	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(&Temp), XMLoadFloat4x4(m_pParentWorld)));
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

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pAlphaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Alpha_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pEmissiveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Emissive_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::Explosion)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSmoke_Explode_Effect::EffectStartCallBack()
{
}

void CSmoke_Explode_Effect::EffectEndCallBack()
{

}

void CSmoke_Explode_Effect::EffectSoftEnd()
{
}

void CSmoke_Explode_Effect::EffectHardEnd()
{
}

HRESULT CSmoke_Explode_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion_Scratch"), TEXT("Com_Noise_Texture")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion"), TEXT("Com_Texture")));

	m_pEmissiveTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion_Emissive"), TEXT("Com_EmissiveTexture")));
	m_pAlphaTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Explosion_Alpha"), TEXT("Com_AlphaTexture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CSmoke_Explode_Effect> CSmoke_Explode_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSmoke_Explode_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSmoke_Explode_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CSmoke_Explode_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSmoke_Explode_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CSmoke_Explode_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSmoke_Explode_Effect
	{
		MakeSharedEnabler(const CSmoke_Explode_Effect& rhs) : CSmoke_Explode_Effect(rhs) {}
	};

	shared_ptr<CSmoke_Explode_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSmoke_Explode_Effect"));
		assert(false);
	}

	return pInstance;
}

void CSmoke_Explode_Effect::Free()
{
}