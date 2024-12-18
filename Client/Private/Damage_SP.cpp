#include "Damage_SP.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CDamage_SP::ObjID = TEXT("CDamage_SP");

CDamage_SP::CDamage_SP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CDamage_SP::ObjID);
}

CDamage_SP::CDamage_SP(const CDamage_SP& rhs)
	: CEffect_Base(rhs)
{
}

CDamage_SP::~CDamage_SP()
{
	Free();
}

HRESULT CDamage_SP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamage_SP::Initialize(void* pArg)
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

void CDamage_SP::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CDamage_SP::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);

	if (m_pParentWorld != nullptr) {
		m_pColliderCom->Update(XMMatrixMultiply(m_pTransformCom->Get_WorldMatrix_XMMat(), XMLoadFloat4x4(m_pParentWorld)));
	}
	else {
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
	}
}

void CDamage_SP::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
			GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PLAYER, m_pColliderCom);
			//	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CDamage_SP::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
	//#endif

	return S_OK;
}

void CDamage_SP::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() != CHAI_COLLIDER_HIT) {
		DamageInfo damageInfo = {};
		damageInfo.eAttackType = EAttackType::SP;
		damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
		damageInfo.fAmountDamage = 20.f;
		pOther->Get_Owner()->TakeDamage(damageInfo);
	}
}

void CDamage_SP::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{

}

void CDamage_SP::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CDamage_SP::EffectStartCallBack()
{

}

void CDamage_SP::EffectEndCallBack()
{

}

void CDamage_SP::EffectSoftEnd()
{

}

void CDamage_SP::EffectHardEnd()
{

}

HRESULT CDamage_SP::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vExtents = { 0.5f, 0.5f, 0.5f };
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };

	m_pColliderCom = dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CDamage_SP> CDamage_SP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CDamage_SP
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDamage_SP(pDevice, pContext)
		{}
	};


	shared_ptr<CDamage_SP> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDamage_SP"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CDamage_SP::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CDamage_SP
	{
		MakeSharedEnabler(const CDamage_SP& rhs) : CDamage_SP(rhs) {}
	};

	shared_ptr<CDamage_SP> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDamage_SP"));
		assert(false);
	}

	return pInstance;
}

void CDamage_SP::Free()
{
}