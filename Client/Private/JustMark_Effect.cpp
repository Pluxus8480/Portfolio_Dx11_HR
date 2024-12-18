#include "JustMark_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CJustMark_Effect::ObjID = TEXT("CJustMark_Effect");

CJustMark_Effect::CJustMark_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CJustMark_Effect::ObjID);
}

CJustMark_Effect::CJustMark_Effect(const CJustMark_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CJustMark_Effect::~CJustMark_Effect()
{
	Free();
}

HRESULT CJustMark_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_JustMark_Scratch"), TEXT("../Bin/Resources/Effect/JustMark/T_VFX_tk_scratch_21a.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_JustMark"), TEXT("../Bin/Resources/Effect/JustMark/T_VFX_tk_PL_JustMark_0%d.dds"), 4));




	return S_OK;
}

HRESULT CJustMark_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(1.f, 1.f, 1.f));

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

void CJustMark_Effect::PriorityTick(_float fTimeDelta)
{


}

void CJustMark_Effect::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CJustMark_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CJustMark_Effect::Render()
{
	_float fTimeRatio = min(m_fEffectTimePercent * 8.f, 1.f);
	_float4x4 ViewMat;
	auto pGameInstance = m_pGameInstance.lock();
	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	if (m_pParentWorld)
	{
		_float4x4 Temp;
		XMStoreFloat4x4(&Temp, XMMatrixScaling(fTimeRatio, fTimeRatio, fTimeRatio) * m_pTransformCom->Get_WorldMatrix_XMMat() * XMLoadFloat4x4(m_pParentWorld));
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	}
	else
	{
		_float4x4 Temp;
		XMStoreFloat4x4(&Temp, XMMatrixScaling(fTimeRatio, fTimeRatio, fTimeRatio) * m_pTransformCom->Get_WorldMatrix_XMMat());
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.8f) * 5.f, 0.f));

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iStreak)))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::TextureColor)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CJustMark_Effect::Set_BeatStreak(_uint iStreak)
{
	m_iStreak = iStreak;
	if (m_iStreak > 3)
	{
		m_iStreak = 3;
	}
}

void CJustMark_Effect::EffectStartCallBack()
{
}

void CJustMark_Effect::EffectEndCallBack()
{

}

void CJustMark_Effect::EffectSoftEnd()
{
}

void CJustMark_Effect::EffectHardEnd()
{
}

HRESULT CJustMark_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_JustMark_Scratch"), TEXT("Com_Noise_Texture")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_JustMark"), TEXT("Com_Texture")));


	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CJustMark_Effect> CJustMark_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CJustMark_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CJustMark_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CJustMark_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CJustMark_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CJustMark_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CJustMark_Effect
	{
		MakeSharedEnabler(const CJustMark_Effect& rhs) : CJustMark_Effect(rhs) {}
	};

	shared_ptr<CJustMark_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CJustMark_Effect"));
		assert(false);
	}

	return pInstance;
}

void CJustMark_Effect::Free()
{
}