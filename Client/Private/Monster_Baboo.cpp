#include "Monster_Baboo.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Beat_Manager.h"

#include "GameManager.h"
#include "Effect_Manager.h"

#include "Shader.h"
#include "Model.h"

#include "CharacterController.h"

#include "CombatManager.h"

#include "Collider.h"
#include "Bounding_Sphere.h"

#include "BabooState_Intro.h"
#include "BabooState_Idle.h"
#include "BabooState_Move.h"
#include "BabooState_Attack.h"
#include "BabooState_Damaged.h"
#include "BabooState_Dead.h"
#include "BabooState_Down.h"

#include "Barrier.h"

typedef CBabooState::State BabooState;


wstring CMonster_Baboo::ObjID = TEXT("Baboo");

CMonster_Baboo::CMonster_Baboo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonsterBase{ pDevice, pContext }
{
}

CMonster_Baboo::CMonster_Baboo(const CMonster_Baboo& rhs)
	: CMonsterBase{ rhs }
{
}

CMonster_Baboo::~CMonster_Baboo()
{
	Free();
}

HRESULT CMonster_Baboo::Initialize_Prototype()
{
	_matrix		LocalTransformMatrix = XMMatrixIdentity();
	LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Monster_Baboo"), TEXT("../Bin/Resources/Models/Monsters/Baboo/Baboo.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CMonster_Baboo::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC transformDesc{};
	if (FAILED(__super::Initialize(&transformDesc)))
		return E_FAIL;

	auto pDesc = static_cast<Monster_DESC*>(pArg);
	_float4x4 matTrans = pDesc->matTransform;
	_matrix mMat = XMLoadFloat4x4(&matTrans);
	_bool isNan = XMMatrixIsNaN(mMat);
	mMat.r[0] = XMVector4Normalize(mMat.r[0]);
	mMat.r[1] = XMVector4Normalize(mMat.r[1]);
	mMat.r[2] = XMVector4Normalize(mMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(mMat);
	string strDesc = pDesc->strDesc;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	PrepareCallbacks();

	// Init HP
	m_fCurHp = m_fMaxHp;

	// Physix setting
	if (strDesc == "1")
		CCT_OFF();
	else
		CCT_ON();

	// Initialize state
	m_pCurState = new CBabooState_Idle(dynamic_pointer_cast<CMonster_Baboo>(shared_from_this()), BabooState::END_STATE);
	m_pCurState->ChangeState(nullptr, m_pCurState);

	// Initialize Barrier
	m_pBarrier = static_pointer_cast<CBarrier>(m_pGameInstance.lock()->Clone_Object(CBarrier::ObjID, nullptr));
	m_pBarrier->Get_Transform()->Set_Scale(_float3(3.f, 3.f, 3.f));
	m_pBarrier->SetRootMatrix(m_pBarrier->Get_Transform()->Get_WorldMatrix());
	m_pBarrier->SetOwner(dynamic_pointer_cast<CMonster_Baboo>(shared_from_this()));

	// For tool setted effects
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach)
		{
			if(strName == "em0400_shield-a_020")
				CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matSizeUp, m_fBeatSpeed, isBillBoard);
			else
				CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matCurrentPos, m_fBeatSpeed, isBillBoard);
		}
		else
		{
			if (strName == "em0400_shield-a_020")
				CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matSizeUp, 1.f);
			else
				CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matCurrentPos, 1.f);
		}
		}
	);

	if (isNan)
		Dead();
	else
		CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());
	return S_OK;
}

void CMonster_Baboo::PriorityTick(_float fTimeDelta)
{
	if (m_isAirState)
	{
		PxControllerState pCS;
		m_pCCT->Get_Controller()->getState(pCS);
		if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			auto pDamagedState = dynamic_cast<CBabooState_Damaged*>(m_pCurState);
			if (pDamagedState != nullptr && !pDamagedState->DoingAirBone())
			{
				AirStateOFF();

				auto pStateDown = new CBabooState_Down(dynamic_pointer_cast<CMonster_Baboo>(shared_from_this()), m_pCurState->GetState());
				m_pCurState->ChangeState(m_pCurState, pStateDown);
				m_pCurState = pStateDown;
			}
		}
	}
	auto pIntroState = dynamic_cast<CBabooState_Intro*>(m_pCurState);
	if (pIntroState != nullptr)
	{
		PxControllerState pCS;
		m_pCCT->Get_Controller()->getState(pCS);
		if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			pIntroState->FinishLifting();
		}
	}

	_vector vPlayerPos = PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position();
	_vector vCurPos = m_pTransformCom->Get_Position();
	_vector vCurLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_vector vDirToPlayer = XMVectorSetY(vPlayerPos - vCurPos, 0.f);
	m_fDistance = XMVectorGetX(XMVector4Length(vDirToPlayer));
	vDirToPlayer = XMVector3Normalize(vDirToPlayer);
	XMStoreFloat4(&m_vfDirToPlayer, vDirToPlayer);

	_float fCrossY = XMVectorGetY(XMVector3Cross(vCurLook, vDirToPlayer));
	_float fDot = XMVectorGetX(XMVector3Dot(vCurLook, vDirToPlayer));

	m_fAngleDegree = XMConvertToDegrees(acosf(fDot));
	if (fCrossY < 0.f)
		m_fAngleDegree *= -1.f;

	// Update Barrier
	m_pBarrier->PriorityTick(fTimeDelta);
}

void CMonster_Baboo::Tick(_float fTimeDelta)
{
	// Update state
	CBabooState* pNextState = m_pCurState->StepState(fTimeDelta);
	if (nullptr != pNextState)
	{
		m_pCurState->ChangeState(m_pCurState, pNextState);
		m_pCurState = pNextState;
	}
	m_pCurState->ActiveState(fTimeDelta);

	// Play Animation
	_float4x4 fmatResult;
	XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
	SelectBeat();
	if (m_pCurState->IsSlowAnim())
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * 0.2f * m_fBeatSpeed * m_fTimeMultiflier, 0.1f, fmatResult);
	else
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fBeatSpeed * m_fTimeMultiflier, 0.1f, fmatResult);

	if (m_pModelCom->IsRootAnim() && !m_pModelCom->IsForcedNonRoot())
	{
		_matrix matResult = XMLoadFloat4x4(&fmatResult);
		auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);

		_float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
		auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
		if (m_isCCTWork)
			m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
		else
			m_pTransformCom->Set_Position(vCurPos);
	}

	// Update Physics
	if (m_isCCTWork)
		m_pCCT->Update(fTimeDelta);

	// Update Colliders
	auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
	matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
	matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
	matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
	m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());

	if (m_isAttackOn)
	{
		auto matrixRHand = XMLoadFloat4x4(m_pMat_RHand);
		matrixRHand.r[0] = XMVector3Normalize(matrixRHand.r[0]);
		matrixRHand.r[1] = XMVector3Normalize(matrixRHand.r[1]);
		matrixRHand.r[2] = XMVector3Normalize(matrixRHand.r[2]);
		matrixRHand *= m_pTransformCom->Get_WorldMatrix_XMMat();

		auto matrixLHand = XMLoadFloat4x4(m_pMat_LHand);
		matrixLHand.r[0] = XMVector3Normalize(matrixLHand.r[0]);
		matrixLHand.r[1] = XMVector3Normalize(matrixLHand.r[1]);
		matrixLHand.r[2] = XMVector3Normalize(matrixLHand.r[2]);
		matrixLHand *= m_pTransformCom->Get_WorldMatrix_XMMat();

		if (m_eHandAttack == EHand::LEFT)
			m_pCollider_Attack->Update(matrixLHand);
		else if (m_eHandAttack == EHand::RIGHT)
			m_pCollider_Attack->Update(matrixRHand);
		else if (m_eHandAttack == EHand::BOTH)
		{
			_vector vPosCenter = XMVectorSetW((matrixLHand.r[3] + matrixRHand.r[3]) * 0.5f, 1.f);
			matrixLHand.r[3] = vPosCenter;
			m_pCollider_Attack->Update(matrixLHand);
		}
	}

	// Update shaking timer
	m_fShakeTime -= fTimeDelta;

	// Update AttackTimer
	if (m_pCurState->GetState() != BabooState::ATTACK && m_pCurState->GetState() != BabooState::DAMAGED)
		m_fTimer_Attack -= fTimeDelta;

	// Update Barrier
	if (m_isBarrierOn)
		m_pBarrier->Tick(fTimeDelta);

	// Call all reserved effects
	XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
	XMStoreFloat4x4(&m_matSizeUp, XMMatrixScaling(2.5f, 2.5f, 2.5f) * XMLoadFloat4x4(&m_matCurrentPos));
	m_pModelCom->PlayReservedCallbacks();
}

void CMonster_Baboo::LateTick(_float fTimeDelta)
{
	if (m_pCurState->IsColliderOn())
		GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
	if (m_isAttackOn)
		GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);

	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

	// Update Barrier
	if (m_isBarrierOn)
	{
		m_pBarrier->Get_Transform()->Set_Position(m_pTransformCom->Get_Position() + XMVectorSet(0.f, m_pTransformCom->Get_Scale().y * 0.75f, 0.f, 0.f));
		m_pBarrier->LateTick(fTimeDelta);
	}
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
}

HRESULT CMonster_Baboo::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	// Shaking
	_float4x4 matShakingWorld = m_pTransformCom->Get_WorldMatrix();
	if (m_fShakeTime > 0.f)
	{
		_float3 vResult = GAMEINSTANCE->Random_Float3({ -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f });
		_matrix matWorld = XMLoadFloat4x4(&matShakingWorld);
		matWorld.r[3] += XMVectorSetW(XMLoadFloat3(&vResult), 0) * m_fShakeMagnitude;
		XMStoreFloat4x4(&matShakingWorld, matWorld);
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matShakingWorld)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			continue;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			continue;

		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}


		if (bNoEmit)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);


		m_pModelCom->Draw((_uint)i);

		m_pShaderCom->Bind_Float("g_OutlineWidth", 0.005f);
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

		m_pModelCom->Draw((_uint)i);

	}

	// For Colliders
#ifdef _DEBUG
	if (BabooState::DEAD != m_pCurState->GetState())
		m_pCollider_Hit->Render();
	if (m_isAttackOn)
		m_pCollider_Attack->Render();
#endif

	return S_OK;
}

HRESULT CMonster_Baboo::Render_Shadow()
{
	auto pGameInstance = m_pGameInstance.lock();
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_LightView_Matrix());
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_LightProj_Matrix());
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
	//   return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Shadow);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CMonster_Baboo::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pCaller == m_pCollider_Attack)
	{
		DamageInfo damageInfo{};
		damageInfo.fAmountDamage = 10.f;
		damageInfo.eAttackType = EAttackType::Hard;
		damageInfo.pObjectOwner = shared_from_this();
		pOther->Get_Owner()->TakeDamage(damageInfo);
	}
}

void CMonster_Baboo::TakeDamage(const DamageInfo& damageInfo)
{
	if (m_fArmourGauge > 0.f)
	{
		m_fArmourGauge -= 1.f;
		if (m_fArmourGauge <= FLT_MIN)
		{
			// Groggy
			auto pStateDamaged = new CBabooState_Damaged(dynamic_pointer_cast<CMonster_Baboo>(shared_from_this()), m_pCurState->GetState(), damageInfo, true);
			m_pCurState->ChangeState(m_pCurState, pStateDamaged);
			m_pCurState = pStateDamaged;
		}
		else
		{
			m_fShakeTime = m_fShakeTime_C;
		}
	}
	else
	{
		_float fTargetHp = m_fCurHp - damageInfo.fAmountDamage;
		PLAYERMANAGER->Add_PlayerRankScore(damageInfo.fAmountDamage / 2.f);
		PLAYERMANAGER->Add_PlayerScore((_int)damageInfo.fAmountDamage / 2.f);

		m_fCurHp = fTargetHp;
		if (m_fCurHp < 0.f)
		{
			_int waveNum = CCombat_Manager::Get_Instance()->Get_Wave_Num();
			list <shared_ptr<CGameObject>> enemyList = CCombat_Manager::Get_Instance()->Get_Enemy_List();
			if (enemyList.size() == 1 && waveNum == 0)
			{
				m_bFinishBlow = true;
				PLAYERMANAGER->Bullet_Time(2.f, 0.2f, shared_from_this());
				PLAYERMANAGER->Bullet_Time(2.f, 0.2f, PLAYERMANAGER->Get_Player());
				static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Zoom_In(2.f, 0.5f);
			}
		}
		auto pStateDamaged = new CBabooState_Damaged(dynamic_pointer_cast<CMonster_Baboo>(shared_from_this()), m_pCurState->GetState(), damageInfo);
		m_pCurState->ChangeState(m_pCurState, pStateDamaged);
		m_pCurState = pStateDamaged;
	}
}

HRESULT CMonster_Baboo::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

	// Init Model and Matrices
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(CGame_Manager::Get_Instance()->Get_Current_Level(), TEXT("Model_Monster_Baboo"), TEXT("Com_Model")));
	m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
	m_pMat_LHand = m_pModelCom->Get_BoneMatrixPtr("l_hand");
	m_pMat_RHand = m_pModelCom->Get_BoneMatrixPtr("r_hand");

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);


	CCharacterController::CCT_DESC cctDesc;
	cctDesc.pTransform = m_pTransformCom;
	cctDesc.height = 1.0f;
	cctDesc.radius = 0.3f;
	cctDesc.stepOffset = 0.2f;
	cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
	cctDesc.userData = this;

	/* For. Com_CCT*/
	m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));

	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
		sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
		sphereDesc.fRadius = 1.2f;
		m_pCollider_Hit =
			dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
		m_pCollider_Hit->Set_Owner(shared_from_this());
		m_pCollider_Hit->Set_ColliderTag(TEXT("Monster_Hit"));
	}
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
		sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
		sphereDesc.fRadius = 1.2f;
		m_pCollider_Attack =
			dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Attack"), &sphereDesc));
		m_pCollider_Attack->Set_Owner(shared_from_this());
		m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));
	}

	return S_OK;
}

void CMonster_Baboo::PrepareCallbacks()
{
	// Default Attack
	m_pModelCom->SetAnimCallback("em0400_shield-a_040", 0, [&]() {
		m_eHandAttack = EHand::RIGHT;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_040", 1, [&]() { m_isAttackOn = false; });
	m_pModelCom->SetLastAnimCallback("em0400_shield-a_040", [&]() { m_isAttackOn = false; });
	m_pModelCom->SetAnimCallback("em0400_shield-a_041", 0, [&]() {
		m_eHandAttack = EHand::LEFT;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_041", 1, [&]() { m_isAttackOn = false; });
	m_pModelCom->SetLastAnimCallback("em0400_shield-a_041", [&]() { m_isAttackOn = false; });

	// Hard Attack
	m_pModelCom->SetAnimCallback("em0400_shield-a_000", 0, [&]() {
		m_eHandAttack = EHand::RIGHT;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_000", 1, [&]() { m_isAttackOn = false; });
	m_pModelCom->SetLastAnimCallback("em0400_shield-a_000", [&]() { m_isAttackOn = false; });

	// Range Attack
	m_pModelCom->SetAnimCallback("em0400_shield-a_030", 0, [&]() {
		m_eHandAttack = EHand::BOTH;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_030", 1, [&]() { m_isAttackOn = false; });
	m_pModelCom->SetLastAnimCallback("em0400_shield-a_030", [&]() { m_isAttackOn = false; });

	// Range Attack
	m_pModelCom->SetAnimCallback("em0400_shield-a_030", 0, [&]() {
		m_eHandAttack = EHand::BOTH;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_030", 1, [&]() { m_isAttackOn = false; });
	m_pModelCom->SetLastAnimCallback("em0400_shield-a_030", [&]() { m_isAttackOn = false; });

	// Move Attack
	m_pModelCom->SetAnimCallback("em0400_shield-a_050", 0, [&]() {
		m_eHandAttack = EHand::RIGHT;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_050", 1, [&]() { m_isAttackOn = false; });
	m_pModelCom->SetLastAnimCallback("em0400_shield-a_050", [&]() { m_isAttackOn = false; });

	// Jump Attack
	m_pModelCom->SetAnimCallback("em0400_shield-a_020", 0, [&]() {
		auto pAttackState = dynamic_cast<CBabooState_Attack*>(m_pCurState);
		pAttackState->JumpOn();
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_020", 1, [&]() {
		m_eHandAttack = EHand::BOTH;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em0400_shield-a_020", 2, [&]() {
		auto pAttackState = dynamic_cast<CBabooState_Attack*>(m_pCurState);
		pAttackState->JumpOff();

		m_isAttackOn = false;
		}
	);
	m_pModelCom->SetLastAnimCallback("em0400_shield-a_020", [&]() { m_isAttackOn = false; });


	// Shield ON
	m_pModelCom->SetAnimCallback("em0400_shield-a_010", 0, [&]() {
		m_isBarrierOn = true;
		m_pCCT->Resize(2.f, 0.5f);
		}
	);


	// Dead
	m_pModelCom->SetAnimCallback("em0400_dmg-die_100", 0, [&]() { MonsterDead(); });
	m_pModelCom->SetAnimCallback("em0400_dmg-die_010", 0, [&]() { MonsterDead(); });

	m_pModelCom->SetAnimCallback("em0400_dmg-mid_010", 0, [&]() { if (IsDeadState()) MonsterDead(); });
	m_pModelCom->SetAnimCallback("em0400_dmg-mid_020", 0, [&]() { if (IsDeadState()) MonsterDead(); });
	m_pModelCom->SetAnimCallback("em0400_dmg-mid_600", 0, [&]() { if (IsDeadState()) MonsterDead(); });
}

void CMonster_Baboo::SetPos_Hard(_fvector vPos)
{
	m_pTransformCom->Set_Position(vPos);
}

void CMonster_Baboo::MoveToPos(_fvector vTargetPos)
{
	auto pStateIntro = new CBabooState_Intro(dynamic_pointer_cast<CMonster_Baboo>(shared_from_this()), m_pCurState->GetState(), vTargetPos);
	m_pCurState->ChangeState(m_pCurState, pStateIntro);
	m_pCurState = pStateIntro;
}

void CMonster_Baboo::MonsterDead()
{
	_float4x4 matResult{};
	XMStoreFloat4x4(&matResult, XMMatrixIdentity());
	PlayEffect("Enemy_Boom", matResult, 1.f, true);

	Dead();

	CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
}

void CMonster_Baboo::GravityOn()
{
	m_pCCT->Enable_Gravity();
}

void CMonster_Baboo::GravityOff()
{
	m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pCCT->Disable_Gravity();
}

void CMonster_Baboo::AirStateON()
{
	m_isAirState = true;
	GravityOff();
}

void CMonster_Baboo::AirStateOFF()
{
	m_isAirState = false;
	GravityOn();
}

void CMonster_Baboo::CCT_ON()
{
	m_isCCTWork = true;
	m_pCCT->Enable_Pseudo_Physics();
	m_pCCT->Enable_Gravity();
	m_pCCT->Set_Gravity(40.f);
}

void CMonster_Baboo::CCT_OFF()
{
	m_isCCTWork = false;
	m_pCCT->Disable_Pseudo_Physics();
	m_pCCT->Disable_Gravity();
}

void CMonster_Baboo::SelectBeat()
{
	_float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

	_float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

	m_fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
}

void CMonster_Baboo::MoveStraight(_float fSpeed)
{
	_vector vCurLook = m_pTransformCom->Get_NormLook();
	m_pCCT->Add_Displacement(vCurLook * fSpeed);
}

void CMonster_Baboo::MoveRight(_float fSpeed)
{
	_vector vCurRight = m_pTransformCom->Get_NormRight();
	m_pCCT->Add_Displacement(vCurRight * fSpeed);
}

void CMonster_Baboo::Set_Position(_matrix mMat)
{
	if (m_isCCTWork)
		m_pCCT->Set_Position(mMat);
	else
		m_pTransformCom->Set_Position(mMat.r[3]);
}

void CMonster_Baboo::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
	_float4x4 matResult{};
	XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(&m_matCurrentPos));
	CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier * m_fBeatSpeed, isBillBoard);
}

void CMonster_Baboo::PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard)
{
	CEffect_Manager::Get_Instance()->Call_Union_Effect(strEffect, &m_matCurrentPos, fmultiplier * m_fBeatSpeed, isBillBoard);
}

_bool CMonster_Baboo::IsNeedToMove()
{
	return true;
}

void CMonster_Baboo::SetBarrierOff()
{
	__super::SetBarrierOff();
	m_pCCT->Resize(0.3f, 1.f);
}

_bool CMonster_Baboo::IsAnimFinished()
{
	return m_pModelCom->Get_Finished();
}

void CMonster_Baboo::SetAnim(const string& strAnimName, _bool isLoop)
{
	m_pModelCom->Set_AnimIndexNonCancle(strAnimName, isLoop);
}

void CMonster_Baboo::SetAnimHard(const string& strAnimName, _bool isLoop)
{
	m_pModelCom->Set_AnimIndex_NoInterpole(strAnimName, isLoop);
}

shared_ptr<CMonster_Baboo> CMonster_Baboo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMonster_Baboo
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Baboo(pDevice, pContext) {}
	};

	shared_ptr<CMonster_Baboo> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Baboo"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMonster_Baboo::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMonster_Baboo
	{
		MakeSharedEnabler(const CMonster_Baboo& rhs) : CMonster_Baboo(rhs) {}
	};

	shared_ptr<CMonster_Baboo> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Baboo"));
		assert(false);
	}

	return pInstance;
}

void CMonster_Baboo::Free()
{
	Safe_Delete(m_pCurState);
	m_pBarrier = nullptr;
}
