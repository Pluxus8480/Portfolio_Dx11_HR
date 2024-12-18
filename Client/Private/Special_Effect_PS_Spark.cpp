#include "Special_Effect_PS_Spark.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"

wstring CSpecial_Effect_PS_Spark::ObjID = TEXT("CSpecial_Effect_PS_Spark");

CSpecial_Effect_PS_Spark::CSpecial_Effect_PS_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CSpecial_Effect_PS_Spark::ObjID);
}

CSpecial_Effect_PS_Spark::CSpecial_Effect_PS_Spark(const CSpecial_Effect_PS_Spark& rhs)
	: CEffect_Base(rhs)
{
}

CSpecial_Effect_PS_Spark::~CSpecial_Effect_PS_Spark()
{
	Free();
}

HRESULT CSpecial_Effect_PS_Spark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_T_VFX_tk_symbol_Lightning_02"), TEXT("../Bin/Resources/Effect/Special/PickSlide/Spark/T_VFX_tk_symbol_Lightning_02.png"), 1));

	return S_OK;
}

HRESULT CSpecial_Effect_PS_Spark::Initialize(void* pArg)
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

	SetColorPresetRGB(_float4{ 1.f,0.1f,0.1f,1.f }, _float4{ 1.f,0.8f,0.1f,1.f }, _float4{ 0.8f,0.5f,1.0f,1.f });

	EffectInit();
	return S_OK;
}

void CSpecial_Effect_PS_Spark::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CSpecial_Effect_PS_Spark::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CSpecial_Effect_PS_Spark::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		}
}

HRESULT CSpecial_Effect_PS_Spark::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pTransformCom->Go_Right(0.033f * GAMEINSTANCE->Random_Float(-0.3f, 0.3f));

	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()), XMLoadFloat4x4(m_pParentWorld)));
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

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 1.f - sin(m_fEffectTimePercent * XM_PI));

	/*if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;*/

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::BlendOriginColor)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;*/

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSpecial_Effect_PS_Spark::EffectStartCallBack()
{

}

void CSpecial_Effect_PS_Spark::EffectEndCallBack()
{

}

void CSpecial_Effect_PS_Spark::EffectSoftEnd()
{

}

void CSpecial_Effect_PS_Spark::EffectHardEnd()
{

}

HRESULT CSpecial_Effect_PS_Spark::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_T_VFX_tk_symbol_Lightning_02"), TEXT("Com_Texture")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Smack_Scratch"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CSpecial_Effect_PS_Spark> CSpecial_Effect_PS_Spark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSpecial_Effect_PS_Spark
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSpecial_Effect_PS_Spark(pDevice, pContext)
		{}
	};


	shared_ptr<CSpecial_Effect_PS_Spark> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpecial_Effect_PS_Spark"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CSpecial_Effect_PS_Spark::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSpecial_Effect_PS_Spark
	{
		MakeSharedEnabler(const CSpecial_Effect_PS_Spark& rhs) : CSpecial_Effect_PS_Spark(rhs) {}
	};

	shared_ptr<CSpecial_Effect_PS_Spark> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpecial_Effect_PS_Spark"));
		assert(false);
	}

	return pInstance;
}

void CSpecial_Effect_PS_Spark::Free()
{
}