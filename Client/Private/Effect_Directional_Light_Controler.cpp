#include "Effect_Directional_Light_Controler.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "CUi_SuccessSystem.h"
#include "Light.h"

wstring CEffect_Directional_Light_Controler::ObjID = TEXT("CEffect_Directional_Light_Controler");

CEffect_Directional_Light_Controler::CEffect_Directional_Light_Controler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_Directional_Light_Controler::ObjID);
}

CEffect_Directional_Light_Controler::CEffect_Directional_Light_Controler(const CEffect_Directional_Light_Controler& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_Directional_Light_Controler::~CEffect_Directional_Light_Controler()
{
	Free();
}

HRESULT CEffect_Directional_Light_Controler::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Directional_Light_Controler::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	SetColorPresetRGB(_float4{ 1.f,0.1f,0.1f,1.f }, _float4{ 1.f,0.8f,0.1f,1.f }, _float4{ 0.8f,0.5f,1.0f,1.f });

	EffectInit();
	return S_OK;
}

void CEffect_Directional_Light_Controler::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CEffect_Directional_Light_Controler::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CEffect_Directional_Light_Controler::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
			//GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PLAYER, m_pColliderCom);
			//	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CEffect_Directional_Light_Controler::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	return S_OK;
}

void CEffect_Directional_Light_Controler::EffectStartCallBack()
{
	auto pLight = GAMEINSTANCE->Return_Light("Light_Directional_Main");
	pLight->Set_LightAmbient(m_vEffectColorPresets[0]);
	pLight->Set_LightDiffuse(m_vEffectColorPresets[1]);
	pLight->Set_LightSpecular(m_vEffectColorPresets[2]);
	pLight->Set_LightIntensity(m_fEffectAlpha);
}

void CEffect_Directional_Light_Controler::EffectEndCallBack()
{

}

void CEffect_Directional_Light_Controler::EffectSoftEnd()
{

}

void CEffect_Directional_Light_Controler::EffectHardEnd()
{

}

HRESULT CEffect_Directional_Light_Controler::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_Directional_Light_Controler> CEffect_Directional_Light_Controler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Directional_Light_Controler
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Directional_Light_Controler(pDevice, pContext)
		{}
	};


	shared_ptr<CEffect_Directional_Light_Controler> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Directional_Light_Controler"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEffect_Directional_Light_Controler::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Directional_Light_Controler
	{
		MakeSharedEnabler(const CEffect_Directional_Light_Controler& rhs) : CEffect_Directional_Light_Controler(rhs) {}
	};

	shared_ptr<CEffect_Directional_Light_Controler> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Directional_Light_Controler"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Directional_Light_Controler::Free()
{
}