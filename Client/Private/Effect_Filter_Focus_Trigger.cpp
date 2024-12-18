#include "Effect_Filter_Focus_Trigger.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "CUi_SuccessSystem.h"

wstring CEffect_Filter_Focus_Trigger::ObjID = TEXT("CEffect_Filter_Focus_Trigger");

CEffect_Filter_Focus_Trigger::CEffect_Filter_Focus_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_Filter_Focus_Trigger::ObjID);
}

CEffect_Filter_Focus_Trigger::CEffect_Filter_Focus_Trigger(const CEffect_Filter_Focus_Trigger& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_Filter_Focus_Trigger::~CEffect_Filter_Focus_Trigger()
{
	Free();
}

HRESULT CEffect_Filter_Focus_Trigger::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Filter_Focus_Trigger::Initialize(void* pArg)
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

void CEffect_Filter_Focus_Trigger::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CEffect_Filter_Focus_Trigger::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CEffect_Filter_Focus_Trigger::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
			//GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PLAYER, m_pColliderCom);
			//	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CEffect_Filter_Focus_Trigger::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	return S_OK;
}

void CEffect_Filter_Focus_Trigger::EffectStartCallBack()
{
	CUi_SuccessSystem::Desc successDesc;
	successDesc.CenterPos = { 0,0 };
	successDesc.bEffectMode = { true };
	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SuccessSystem::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi_Success_System", CUi_SuccessSystem::ObjID, &successDesc);
}

void CEffect_Filter_Focus_Trigger::EffectEndCallBack()
{

}

void CEffect_Filter_Focus_Trigger::EffectSoftEnd()
{

}

void CEffect_Filter_Focus_Trigger::EffectHardEnd()
{

}

HRESULT CEffect_Filter_Focus_Trigger::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_Filter_Focus_Trigger> CEffect_Filter_Focus_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Filter_Focus_Trigger
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Filter_Focus_Trigger(pDevice, pContext)
		{}
	};


	shared_ptr<CEffect_Filter_Focus_Trigger> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Filter_Focus_Trigger"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEffect_Filter_Focus_Trigger::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Filter_Focus_Trigger
	{
		MakeSharedEnabler(const CEffect_Filter_Focus_Trigger& rhs) : CEffect_Filter_Focus_Trigger(rhs) {}
	};

	shared_ptr<CEffect_Filter_Focus_Trigger> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Filter_Focus_Trigger"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Filter_Focus_Trigger::Free()
{
}