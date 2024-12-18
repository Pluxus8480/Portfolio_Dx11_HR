#include "Blaze_Fire.h"
#include "GameInstance.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "Random_Laser.h"
#include "Beat_Manager.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CBlaze_Fire::ObjID = TEXT("CBlaze_Fire");

CBlaze_Fire::CBlaze_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBlaze_Fire::CBlaze_Fire(const CBlaze_Fire& rhs)
	: CGameObject(rhs)
{
}

CBlaze_Fire::~CBlaze_Fire()
{
	Free();
}

HRESULT CBlaze_Fire::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Set_PoolSize(50);

	return S_OK;
}

HRESULT CBlaze_Fire::Initialize(void* pArg)
{

	//CTransform::TRANSFORM_DESC			TransformDesc{};
	//TransformDesc.fSpeedPerSec = 1.f;
	//TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fBlazeFireTime = 0.f;
	m_pBlaze_Oil_Effect = nullptr;
	m_pBlaze_Fire_Effect = nullptr;
	m_pBlaze_Fire_Effect2 = nullptr;
	m_pBlaze_Fire_End_Effect = nullptr;
	m_bActive = true;

	m_pBlaze_Oil_Effect = EFFECTMANAGER->Call_Fixed_Union_Effect_Return("Blaze_Oil_Start", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
	
	m_eBlazeFireState = BLAZE_FIRE_STATE::BLAZE_FIRE_START;

	if(m_pBlaze_Oil_Effect == nullptr)
		m_eBlazeFireState = BLAZE_FIRE_STATE::BLAZE_FIRE_END;

	return S_OK;
}

void CBlaze_Fire::PriorityTick(_float fTimeDelta)
{

}

void CBlaze_Fire::Tick(_float fTimeDelta)
{

	State_Machine(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

}

void CBlaze_Fire::LateTick(_float fTimeDelta)
{

}

HRESULT CBlaze_Fire::Render()
{
	return S_OK;
}

void CBlaze_Fire::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == CHAI_COLLIDER_HIT) {
		DamageInfo damageInfo = {};
		damageInfo.eAttackType = EAttackType::Fire;
		damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
		damageInfo.fAmountDamage = 5.f;
		damageInfo.pObjectOwner = shared_from_this();
		pOther->Get_Owner()->TakeDamage(damageInfo);
	}
}

void CBlaze_Fire::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == CHAI_COLLIDER_HIT) {
		DamageInfo damageInfo = {};
		damageInfo.eAttackType = EAttackType::Fire;
		damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
		damageInfo.fAmountDamage = 5.f;
		damageInfo.pObjectOwner = shared_from_this();
		pOther->Get_Owner()->TakeDamage(damageInfo);
	}
}

void CBlaze_Fire::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CBlaze_Fire::TakeDamage(const DamageInfo& damageInfo)
{
	if (damageInfo.eAttackType == EAttackType::Korsica) {
		m_pBlaze_Fire_Effect->UnionEffectStop();
		m_pBlaze_Fire_Effect2->UnionEffectStop();
		m_pBlaze_Fire_End_Effect = EFFECTMANAGER->Call_Fixed_Union_Effect_Return("Blaze_Fire_End", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
		m_eBlazeFireState = BLAZE_FIRE_STATE::BLAZE_FIRE_END;
	}
}

HRESULT CBlaze_Fire::Ready_Components()
{
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vExtents = { 0.5f, 2.f, 0.5f };
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };

	m_pColliderCom = dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));

	return S_OK;
}

void CBlaze_Fire::State_Machine(_float fTimeDelta)
{

	switch (m_eBlazeFireState)
	{
	case BLAZE_FIRE_STATE::BLAZE_FIRE_START:
		State_Start(fTimeDelta);
		break;
	case BLAZE_FIRE_STATE::BLAZE_FIRE_LOOP:
		State_Loop(fTimeDelta);
		break;
	case BLAZE_FIRE_STATE::BLAZE_FIRE_END:
		State_End(fTimeDelta);
		break;
	}

}

void CBlaze_Fire::State_Start(_float fTimeDelta)
{
	if (m_pBlaze_Oil_Effect->GetUnionTimePer() > 0.95f) 
	{
		m_pBlaze_Oil_Effect = nullptr;
		m_pBlaze_Fire_Effect = EFFECTMANAGER->Call_Fixed_Union_Effect_Return("Blaze_Fire_Loop_1", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		m_pBlaze_Fire_Effect2 = EFFECTMANAGER->Call_Fixed_Union_Effect_Return("Blaze_Fire_Loop", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
		m_eBlazeFireState = BLAZE_FIRE_STATE::BLAZE_FIRE_LOOP;
	}
}

void CBlaze_Fire::State_Loop(_float fTimeDelta)
{
	m_fBlazeFireTime += fTimeDelta;

	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_MONSTER, m_pColliderCom);

	if (m_fBlazeFireTime > m_fBlazeFireTimeMax)
	{

		m_pBlaze_Fire_Effect->UnionEffectStop();
		m_pBlaze_Fire_Effect2->UnionEffectStop();
		m_pBlaze_Fire_End_Effect = EFFECTMANAGER->Call_Fixed_Union_Effect_Return("Blaze_Fire_End", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
		m_eBlazeFireState = BLAZE_FIRE_STATE::BLAZE_FIRE_END;
	}
}

void CBlaze_Fire::State_End(_float fTimeDelta)
{
	
	m_bActive = false;
	Dead();

}

shared_ptr<CBlaze_Fire> CBlaze_Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBlaze_Fire
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBlaze_Fire(pDevice, pContext)
		{}
	};

	shared_ptr<CBlaze_Fire> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBlaze_Fire"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CBlaze_Fire::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBlaze_Fire
	{
		MakeSharedEnabler(const CBlaze_Fire& rhs) : CBlaze_Fire(rhs)
		{}
	};


	shared_ptr<CBlaze_Fire> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBlaze_Fire"));
		assert(false);
	}

	return pInstance;
}

void CBlaze_Fire::Free()
{
}
