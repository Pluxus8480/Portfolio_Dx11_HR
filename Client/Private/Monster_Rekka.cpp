#include "Monster_Rekka.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Effect_Manager.h"
#include "Beat_Manager.h"

#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "Bounding_OBB.h"
#include "CharacterController.h"

#include "Cine_Manager.h"
#include "MapManager.h"
#include "CombatManager.h"
#include "GameManager.h"

#include "UI_Event_Cut.h"

#include "Barrier.h"

#include "Rekka_Pipe_R.h"
#include "Rekka_Pipe_L.h"
#include "Rekka_Long_Pipe.h"
#include "Rekka_Pipe_Door.h"

#include "Rekka_Gauntlet.h"

#include "CUi_Mgr.h"
#include "CUi_RekkaSystem.h"

#include "RekkaState_Idle.h"
#include "RekkaState_Move.h"
#include "RekkaState_Attack.h"
#include "RekkaState_PipeAtt_Short.h"
#include "RekkaState_Pase_2.h"
#include "RekkaState_Pase_3.h"
#include "RekkaState_Pase_4.h"
#include "RekkaState_Pase_Dead.h"
#include "RekkaState_Groggy.h"
#include "RekkaState_Event.h"
typedef CRekkaState::State RekkaState;

typedef CBoss_InteractObj::REKKA_PATTERN  RekkaPattern;

wstring CMonster_Rekka::ObjID = TEXT("Rekka");

CMonster_Rekka::CMonster_Rekka(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonsterBase{ pDevice, pContext }
{
}

CMonster_Rekka::CMonster_Rekka(const CMonster_Rekka& rhs)
	: CMonsterBase{ rhs }
{
}

CMonster_Rekka::~CMonster_Rekka()
{
	Free();
}

HRESULT CMonster_Rekka::Initialize_Prototype()
{
	_matrix	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Monster_Rekka"), TEXT("../Bin/Resources/Models/Monsters/Rekka/Rekka.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Rekka_Attacked", L"../Bin/Resources/Models/Monsters/Rekka/Sound/Rekka_Attacked.ogg",
		SOUND_CHANNEL::SOUND_EFFECT));

	// Load weapons
	m_List_Owning_ObjectID.push_back(CRekka_Pipe_L::ObjID);
	m_List_Owning_ObjectID.push_back(CRekka_Pipe_R::ObjID);
	m_List_Owning_ObjectID.push_back(CRekka_Long_Pipe::ObjID);
	m_List_Owning_ObjectID.push_back(CRekka_Gauntlet::ObjID);

	m_List_Owning_ObjectID.push_back(CUi_RekkaSystem::ObjID);

	// Load Gimmicks
	m_List_Owning_ObjectID.push_back(CRekka_Pipe_Door::ObjID);

	return S_OK;
}

HRESULT CMonster_Rekka::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	if (FAILED(Ready_PartObjects()))
		return E_FAIL;
	if (FAILED(Ready_GimmickObjects()))
		return E_FAIL;
	PrepareCallbacks();

	// Init HP
	m_fCurHp = m_fMaxHp_C;

	// Physix setting
	m_pCCT->Enable_Pseudo_Physics();
	m_pCCT->Set_Gravity(40.f);

	// Initialize state
	m_pCurState = new CRekkaState_Idle(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), RekkaState::END_STATE);
	m_pCurState->ChangeState(nullptr, m_pCurState);

	// Add to combat manager
	CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());

	// Add to map manager
	CMapManager::Get_Instance()->Boss_ChangePhase(_uint(CBoss_InteractObj::REKKA_PHASE::PHASE_1));

	// For effect handling
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach)
			CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matCurrentPos, m_fBeatSpeed);
		else
			CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matCurrentPos, m_fBeatSpeed);
		}
	);


	auto pHpUI = dynamic_pointer_cast<CUi_RekkaSystem>(CUi_Mgr::Get_Instance()->Find_UI(CUi_RekkaSystem::ObjID));
	pHpUI->SetRekka(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()));

	// Set cinematic callback(Scene)
	CCine_Manager::Get_Instance()->AddLastCallback("CS0230",
		[&]() {
			CCombat_Manager::Get_Instance()->Set_Combat_True();
			_float4x4 matResult;
			CCine_Manager::Get_Instance()->GetMatTrans("em2000", matResult);
			auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
			vNewPos += {0.f, 0.3f, 0.f, 0.f};

			m_pTransformCom->Set_Position(vNewPos);
			m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());

			vNewPos -= XMVectorSetW(XMLoadFloat4x4(&matResult).r[2], 0.f);
			m_pTransformCom->LookAt_Horizontal(vNewPos);

			CUi_Mgr::Get_Instance()->Ui_AllActive(true);
		}
	);
	CCine_Manager::Get_Instance()->AddLastCallback("CS0235",
		[&]() {
			m_isElectric = true;

			_float4x4 matResult;
			CCine_Manager::Get_Instance()->GetMatTrans("em2000", matResult);
			auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
			vNewPos += {0.f, 0.3f, 0.f, 0.f};

			m_pTransformCom->Set_Position(vNewPos);
			m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());

			vNewPos -= XMVectorSetW(XMLoadFloat4x4(&matResult).r[2], 0.f);
			m_pTransformCom->LookAt_Horizontal(vNewPos);

			CUi_Mgr::Get_Instance()->Ui_AllActive(true);
		}
	);
	CCine_Manager::Get_Instance()->AddLastCallback("CS0240",
		[&]() {
			_float4x4 matResult;
			CCine_Manager::Get_Instance()->GetMatTrans("em2000", matResult);
			auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
			vNewPos += {0.f, 0.3f, 0.f, 0.f};

			m_pTransformCom->Set_Position(vNewPos);
			m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());

			vNewPos -= XMVectorSetW(XMLoadFloat4x4(&matResult).r[2], 0.f);
			m_pTransformCom->LookAt_Horizontal(vNewPos);

			CUi_Mgr::Get_Instance()->Ui_AllActive(true);

			// 통나무 상태에서 시작
			m_pPipeL->AttachOn();
			m_pPipeR->AttachOn();

			auto pStateShortAttack = new CRekkaState_PipeAtt_Short(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), RekkaState::IDLE);
			m_pCurState->ChangeState(m_pCurState, pStateShortAttack);
			m_pCurState = pStateShortAttack;
		}
	);
	CCine_Manager::Get_Instance()->AddLastCallback("CS0245",
		[&]() {
			CUi_Mgr::Get_Instance()->Ui_AllActive(true);

			//CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
			//Dead();
		}
	);

	// Set Cinematic callback()
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0230", 3, 0,
		[&](_float4, _float4, _float3) {
			CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::GRAB));
		}
	);
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0240", 1, 0,
		[&](_float4, _float4, _float3) {
			m_pDoorL->OpenDoor();
			m_pDoorR->OpenDoor();
		}
	);

	// Set cinematic callback(Seq)
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0235", 19, 0,
		[&](_float4, _float4, _float3) {
			CUI_Event_Cut::Desc Desc;
			Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::RekkaPowerUp;
			CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
		}
	);
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0235", 19, 1,
		[&](_float4, _float4, _float3) {
			CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
		}
	);
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0245", 13, 0,
		[&](_float4, _float4, _float3) {
			CUI_Event_Cut::Desc Desc;
			Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::RekkaFinal;
			CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
		}
	);
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0245", 13, 1,
		[&](_float4, _float4, _float3) {
			CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
		}
	);
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0245", 15, 1,
		[&](_float4, _float4, _float3) {
			// TODO: 상현아!!!@!@@! 폭발~~~
			CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
			Dead();
		}
	);

	return S_OK;
}

void CMonster_Rekka::PriorityTick(_float fTimeDelta)
{
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
}

void CMonster_Rekka::Tick(_float fTimeDelta)
{
	if (!CCine_Manager::Get_Instance()->IsInGame())
		return;

	// Update state
	CRekkaState* pNextState = m_pCurState->StepState(fTimeDelta);
	if (nullptr != pNextState)
	{
		m_pCurState->ChangeState(m_pCurState, pNextState);
		m_pCurState = pNextState;
	}
	m_pCurState->ActiveState(fTimeDelta);

	// Play Animation
	_float4x4 fmatResult;
	XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
	if (m_isAnimPlay)
	{
		SelectBeat();
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fBeatSpeed * m_fTimeMultiflier, 0.1f, fmatResult);

		if (m_pModelCom->IsRootAnim() && !m_pModelCom->IsForcedNonRoot())
		{
			_matrix matResult = XMLoadFloat4x4(&fmatResult);
			auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);

			_float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
			auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
			if (!m_isCCT_Disable)
				m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
			else
				m_pTransformCom->Set_Position(vCurPos);
		}
	}

	// Update Physics
	if (!m_isCCT_Disable)
		m_pCCT->Update(fTimeDelta);

	// Update Bones
	_matrix matrixSpine = NormalizeMatrix(m_pMat_Spine);
	_matrix matrixRHand = NormalizeMatrix(m_pMat_RightHand);
	_matrix matrixLHand = NormalizeMatrix(m_pMat_LeftHand);
	_matrix matrixAttach_R = NormalizeMatrix(m_pMat_Attach_R);
	_matrix matrixAttach_L = NormalizeMatrix(m_pMat_Attach_L);
	_matrix matrixForeArm_R = NormalizeMatrix(m_pMat_ForeArm_R);
	_matrix matrixForeArm_L = NormalizeMatrix(m_pMat_ForeArm_L);

	// Update Colliders
	m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());
	if (m_isAttackOn)
	{
		_matrix matAttack = XMMatrixScaling(m_fAttackScale, m_fAttackScale, m_fAttackScale);
		switch (m_curAttackBone)
		{
		case EAttackBone::RHand:
			matAttack = matAttack * matrixRHand;
			break;
		case EAttackBone::LHand:
			matAttack = matAttack * matrixLHand;
			break;
		case EAttackBone::Body:
			matAttack = matAttack * matrixSpine;
			break;
		}
		m_pCollider_Attack->Update(matAttack * m_pTransformCom->Get_WorldMatrix_XMMat());
	}


	// Update AttackTimer
	if (m_pCurState->GetState() != RekkaState::ATTACK && m_pCurState->GetState() != RekkaState::DAMAGED)
		m_fTimer_Attack -= fTimeDelta;
	if (m_pCurState->GetState() == RekkaState::INTRO)
		m_fTimer_Attack = m_fTimer_Attack_C;

	// Electric effect
	if (m_isElectric)
	{
		if (!m_isAttackOn)
		{
			_matrix matAttack = XMMatrixScaling(1.5f, 1.5f, 1.5f);
			matAttack = matAttack * matrixSpine;
			m_pCollider_Attack->Update(matAttack * m_pTransformCom->Get_WorldMatrix_XMMat());
		}

		m_fTimerElect -= fTimeDelta;
		if (m_fTimerElect <= 0.f)
		{
			m_fTimerElect = 0.2f;
			PlayUnionEffect("Rekka_Body_Spark", 1.f, true);
		}
	}

	// Update PartObjects
	m_pPipeL->Tick(fTimeDelta);
	m_pPipeR->Tick(fTimeDelta);
	m_pPipe_Long->Tick(fTimeDelta);
	m_pGauntlet->Tick(fTimeDelta);

	// Rotation
	_matrix matRot = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixRotationX(XM_PIDIV2);

	// Call all reserved effects
	XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
	XMStoreFloat4x4(&m_matRekka_Rasengan, matrixRHand * m_pTransformCom->Get_WorldMatrix_XMMat());
	XMStoreFloat4x4(&m_matRekka_Groggy_R, matRot * matrixForeArm_R * m_pTransformCom->Get_WorldMatrix_XMMat());
	XMStoreFloat4x4(&m_matRekka_Groggy_L, matRot * matrixForeArm_L * m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pModelCom->PlayReservedCallbacks();
}

void CMonster_Rekka::LateTick(_float fTimeDelta)
{
	if (!CCine_Manager::Get_Instance()->IsInGame())
		return;

	if (RekkaState::DEAD != m_pCurState->GetState())
		GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
	if (m_isAttackOn || m_isElectric)
		GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);

	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

	// Update PartObjects
	m_pPipeL->LateTick(fTimeDelta);
	m_pPipeR->LateTick(fTimeDelta);
	m_pPipe_Long->LateTick(fTimeDelta);
	m_pGauntlet->LateTick(fTimeDelta);
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
}

HRESULT CMonster_Rekka::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			continue;

		m_pShaderCom->Begin(0);

		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			continue;

		m_pModelCom->Render((_uint)i);

		m_pShaderCom->Bind_Float("g_OutlineWidth", 1.1f);
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

		m_pModelCom->Draw((_uint)i);

	}

#ifdef _DEBUG
	if (RekkaState::DEAD != m_pCurState->GetState())
		m_pCollider_Hit->Render();
	if (m_isAttackOn || m_isElectric)
		m_pCollider_Attack->Render();
#endif

	return S_OK;
}

HRESULT CMonster_Rekka::Render_Shadow()
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

void CMonster_Rekka::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pCaller == m_pCollider_Attack)
	{
		if (m_isGrapOn)
		{
			auto pStateEvent = new CRekkaState_Event(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), m_pCurState->GetState());
			m_pCurState->ChangeState(m_pCurState, pStateEvent);
			m_pCurState = pStateEvent;
			m_pModelCom->Set_AnimFrame(0.01f);

			DamageInfo damageInfo{};
			damageInfo.eAttackType = EAttackType::Special;
			damageInfo.pObjectOwner = shared_from_this();
			damageInfo.strAttackName = "Rekka_WWE";
			
			CCT_Off();

			pOther->Get_Owner()->TakeDamage(damageInfo);
		}
		else
		{
			DamageInfo damageInfo{};
			damageInfo.fAmountDamage = 20.f;
			if (m_isElectric)
				damageInfo.eAttackType = EAttackType::ELECTRIC;
			else
				damageInfo.eAttackType = EAttackType::Hard;
			damageInfo.pObjectOwner = shared_from_this();
			pOther->Get_Owner()->TakeDamage(damageInfo);
		}
	}
}

void CMonster_Rekka::TakeDamage(const DamageInfo& damageInfo)
{
	if (RekkaState::INTRO == m_pCurState->GetState())
		return;

	m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Rekka_Attacked", m_pTransformCom->Get_Position());

	_float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
	_vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
	_float4x4 matResult{};
	XMStoreFloat4x4(&matResult, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));
	PlayEffect("Big_HitEffect", matResult, 1.f, true);

	_float fTargetHp = m_fCurHp - damageInfo.fAmountDamage;
	PLAYERMANAGER->Add_PlayerRankScore(damageInfo.fAmountDamage / 2.f);
	PLAYERMANAGER->Add_PlayerScore((_int)damageInfo.fAmountDamage / 2.f);
	if (fTargetHp <= 0.f)
	{
		// Dead Scene
		auto pStatePase = new CRekkaState_Pase_Dead(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), m_pCurState->GetState());
		m_pCurState->ChangeState(m_pCurState, pStatePase);
		m_pCurState = pStatePase;
	}
	else if (fTargetHp <= m_fMaxHp_C * 0.25f && m_fCurHp > m_fMaxHp_C * 0.25f)
	{
		m_iCurPase = 4;
		CMapManager::Get_Instance()->Boss_ChangePhase(_uint(CBoss_InteractObj::REKKA_PHASE::PHASE_4));
		m_isElectric = false;

		auto pStatePase = new CRekkaState_Pase_4(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), m_pCurState->GetState());
		m_pCurState->ChangeState(m_pCurState, pStatePase);
		m_pCurState = pStatePase;
	}
	else if (fTargetHp <= m_fMaxHp_C * 0.5f && m_fCurHp > m_fMaxHp_C * 0.5f)
	{
		m_isElectric = false;

		auto pStatePase = new CRekkaState_Pase_3(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), m_pCurState->GetState());
		m_pCurState->ChangeState(m_pCurState, pStatePase);
		m_pCurState = pStatePase;
	}
	else if (fTargetHp <= m_fMaxHp_C * 0.75f && m_fCurHp > m_fMaxHp_C * 0.75f)
	{
		m_isElectric = false;

		auto pStatePase = new CRekkaState_Pase_2(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), m_pCurState->GetState());
		m_pCurState->ChangeState(m_pCurState, pStatePase);
		m_pCurState = pStatePase;
	}
	else if (m_fTimerGroggy > 0.f)
	{
		m_isElectric = false;

		auto pStateGroggy = new CRekkaState_Groggy(dynamic_pointer_cast<CMonster_Rekka>(shared_from_this()), m_pCurState->GetState(), damageInfo.eAttack_Direction);
		m_pCurState->ChangeState(m_pCurState, pStateGroggy);
		m_pCurState = pStateGroggy;
	}
	m_fCurHp = fTargetHp;
}

HRESULT CMonster_Rekka::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Monster_Rekka"), TEXT("Com_Model")));
	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
	m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_hand_sub");
	m_pMat_LeftHand = m_pModelCom->Get_BoneMatrixPtr("l_hand_sub");
	m_pMat_Attach_R = m_pModelCom->Get_BoneMatrixPtr("r_attach_00");
	m_pMat_Attach_L = m_pModelCom->Get_BoneMatrixPtr("l_attach_00");
	m_pMat_ForeArm_R = m_pModelCom->Get_BoneMatrixPtr("r_forearm");
	m_pMat_ForeArm_L = m_pModelCom->Get_BoneMatrixPtr("l_forearm");
	m_pMat_Gauntlet = m_pModelCom->Get_BoneMatrixPtr("spine_00");

	m_pMat_Camera = m_pModelCom->Get_BoneMatrixPtr("prop_02");
	m_pMat_Attach_Chai = m_pModelCom->Get_BoneMatrixPtr("prop_01");

	/* For. Com_CCT*/
	CCharacterController::CCT_DESC cctDesc;
	cctDesc.pTransform = m_pTransformCom;
	cctDesc.height = 3.0f;
	cctDesc.radius = 1.0f;
	cctDesc.stepOffset = 0.1f;
	cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
	cctDesc.userData = this;
	m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));

	/* For. Com Colliders */
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
		sphereDesc.fRadius = 1.f;
		m_pCollider_Attack =
			dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Attack"), &sphereDesc));
		m_pCollider_Attack->Set_Owner(shared_from_this());
		m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));
	}

	return S_OK;
}

HRESULT CMonster_Rekka::Ready_PartObjects()
{
	{
		CRekka_Pipe_R::DESC desc{};
		desc.pBoneMatrix = m_pMat_Attach_R;
		desc.pParentTransform = m_pTransformCom;
		desc.iLevelIndex = m_iLevelIndex;

		m_pPipeR = dynamic_pointer_cast<CRekka_Pipe_R>(GAMEINSTANCE->Clone_Object(CRekka_Pipe_R::ObjID, &desc));
		if (nullptr == m_pPipeR)
			return E_FAIL;
	}
	{
		CRekka_Pipe_L::DESC desc{};
		desc.pBoneMatrix = m_pMat_Attach_L;
		desc.pParentTransform = m_pTransformCom;
		desc.iLevelIndex = m_iLevelIndex;

		m_pPipeL = dynamic_pointer_cast<CRekka_Pipe_L>(GAMEINSTANCE->Clone_Object(CRekka_Pipe_L::ObjID, &desc));
		if (nullptr == m_pPipeL)
			return E_FAIL;
	}
	{
		CRekka_Long_Pipe::DESC desc{};
		desc.pBoneMatrix = m_pMat_Attach_R;
		desc.pParentTransform = m_pTransformCom;
		desc.iLevelIndex = m_iLevelIndex;

		m_pPipe_Long = dynamic_pointer_cast<CRekka_Long_Pipe>(GAMEINSTANCE->Clone_Object(CRekka_Long_Pipe::ObjID, &desc));
		if (nullptr == m_pPipe_Long)
			return E_FAIL;
	}
	{
		CRekka_Gauntlet::DESC desc{};
		desc.pBoneMatrix = m_pMat_Gauntlet;
		desc.pParentTransform = m_pTransformCom;
		desc.iLevelIndex = m_iLevelIndex;

		m_pGauntlet = dynamic_pointer_cast<CRekka_Gauntlet>(GAMEINSTANCE->Clone_Object(CRekka_Gauntlet::ObjID, &desc));
		if (nullptr == m_pGauntlet)
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Rekka::Ready_GimmickObjects()
{
	{
		MISCOBJ_DESC desc{};
		_matrix matResult = XMMatrixRotationY(XM_PI) * XMMatrixTranslationFromVector({ 1181.41f, 485.76f, 131.97f, 1.f });
		XMStoreFloat4x4(&desc.worldMatrix, matResult);
		m_pDoorL = static_pointer_cast<CRekka_Pipe_Door>(
			GAMEINSTANCE->Add_Clone_Return(m_iLevelIndex, TEXT("Layer_Map"), CRekka_Pipe_Door::ObjID, &desc)
		);
	}
	{
		MISCOBJ_DESC desc{};
		XMStoreFloat4x4(&desc.worldMatrix, XMMatrixTranslationFromVector({ 1181.41f, 485.76f, 90.83f, 1.f }));
		m_pDoorR = static_pointer_cast<CRekka_Pipe_Door>(
			GAMEINSTANCE->Add_Clone_Return(m_iLevelIndex, TEXT("Layer_Map"), CRekka_Pipe_Door::ObjID, &desc)
		);
	}

	return S_OK;
}

void CMonster_Rekka::PrepareCallbacks()
{
	/* Attack Callbacks */
	m_pModelCom->SetAnimCallback("em2000_atk_056", 0, [&]() {
		m_isAttackOn = true;
		m_curAttackBone = EAttackBone::LHand;
		m_fAttackScale = 1.2f;
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk_056", 1, [&]() { m_isAttackOn = false; });
	m_pModelCom->SetLastAnimCallback("em2000_atk_056", [&]() { m_isAttackOn = false; });

	// Dash Attack
	m_pModelCom->SetAnimCallback("em2000_atk_110", 0, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->DashOn();
		m_isAttackOn = true;

		m_curAttackBone = EAttackBone::Body;
		m_fAttackScale = 2.f;
		});
	m_pModelCom->SetAnimCallback("em2000_atk_110", 1, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->DashOff();
		m_isAttackOn = false;
		});
	m_pModelCom->SetLastAnimCallback("em2000_atk_110", [&]() { m_isAttackOn = false; });

	// Sequence Dash Attack
	m_pModelCom->SetAnimCallback("em2000_atk_100", 0, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->DashOn();
		m_isAttackOn = true;

		m_curAttackBone = EAttackBone::Body;
		m_fAttackScale = 2.f;
		});
	m_pModelCom->SetAnimCallback("em2000_atk_100", 1, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->DashOff();
		m_isAttackOn = false;
		});
	m_pModelCom->SetLastAnimCallback("em2000_atk_100", [&]() { m_isAttackOn = false; });

	m_pModelCom->SetAnimCallback("em2000_atk_104", 1, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->DashOn();
		m_isAttackOn = true;

		m_curAttackBone = EAttackBone::Body;
		m_fAttackScale = 2.f;
		});
	m_pModelCom->SetAnimCallback("em2000_atk_104", 2, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->DashOff();
		m_isAttackOn = false;
		});
	m_pModelCom->SetLastAnimCallback("em2000_atk_104", [&]() { m_isAttackOn = false; });

	// Range Attack
	m_pModelCom->SetAnimCallback("em2000_atk_330", 0, [&]() {
		m_isAttackOn = true;
		m_curAttackBone = EAttackBone::Body;
		m_fAttackScale = 5.f;

		CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::STOMP));
		});
	m_pModelCom->SetAnimCallback("em2000_atk_330", 1, [&]() {m_isAttackOn = false;});
	m_pModelCom->SetLastAnimCallback("em2000_atk_330", [&]() {
		m_isAttackOn = false;
		CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::SOUNDWAVE));
		}
	);

	// Fist Down
	m_pModelCom->SetAnimCallback("em2000_atk_400", 0, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->FistJumpOn();
		});
	m_pModelCom->SetAnimCallback("em2000_atk_400", 1, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->FistJumpOff();
		});
	m_pModelCom->SetAnimCallback("em2000_atk_400", 2, [&]() {
		});
	m_pModelCom->SetAnimCallback("em2000_atk_400", 3, [&]() {
		auto pAttackState = dynamic_cast<CRekkaState_Attack*>(m_pCurState);
		pAttackState->FistDown();
		m_isAttackOn = true;

		m_curAttackBone = EAttackBone::Body;
		m_fAttackScale = 3.5f;
		});
	m_pModelCom->SetAnimCallback("em2000_atk_400", 4, [&]() {
		m_isAttackOn = false;
		});
	m_pModelCom->SetLastAnimCallback("em2000_atk_400", [&]() { m_isAttackOn = false; });

	// Elect
	m_pModelCom->SetAnimCallback("em2000_atk_150", 0, [&]() {
		m_isElectric = true;
		CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::CHARGE));
		}
	);
	m_pModelCom->SetLastAnimCallback("em2000_atk_150", [&]() {
		CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::SOUNDWAVE));
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk_250", 0, [&]() {
		CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::ELECTRIC));
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk_250", 1, [&]() {
		CEffect_Manager::Get_Instance()->Call_Union_Effect("Rekka_Rasengan", &m_matRekka_Rasengan, m_fBeatSpeed, true);
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk_250", 2, [&]() { m_isElectric = false; });
	m_pModelCom->SetLastAnimCallback("em2000_atk_250", [&]() {
		m_isElectric = false;
		CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::SOUNDWAVE));
		}
	);

	m_pModelCom->SetAnimCallback("em2000_atk_251", 0, [&]() {
		CEffect_Manager::Get_Instance()->Call_Union_Effect("Rekka_Groggy_1", &m_matRekka_Groggy_L, m_fBeatSpeed, false);
		CEffect_Manager::Get_Instance()->Call_Union_Effect("Rekka_Groggy_2", &m_matRekka_Groggy_L, m_fBeatSpeed, true);

		CEffect_Manager::Get_Instance()->Call_Union_Effect("Rekka_Groggy_1", &m_matRekka_Groggy_R, m_fBeatSpeed, false);
		CEffect_Manager::Get_Instance()->Call_Union_Effect("Rekka_Groggy_2", &m_matRekka_Groggy_R, m_fBeatSpeed, true);
		}
	);

	// About Event
	m_pModelCom->SetAnimCallback("em2000_atk_123", 0, [&]() {
		m_curAttackBone = EAttackBone::Body;
		m_fAttackScale = 4.f;

		m_isAttackOn = true;
		m_isGrapOn = true;
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk_123", 1, [&]() {
		m_isAttackOn = false;
		m_isGrapOn = false;
		}
	);
	m_pModelCom->SetLastAnimCallback("em2000_atk_123", [&]() {
		m_isAttackOn = false;
		m_isGrapOn = false;

		m_pGauntlet->FinishAttack();
		CMapManager::Get_Instance()->Boss_ChangePattern(_uint(RekkaPattern::SOUNDWAVE));
		}
	);

	// About Pipe Long
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_020", 0, [&]() {
		m_pPipe_Long->AttachOn();
		}
	);
	m_pModelCom->SetLastAnimCallback("em2000_atk-pipe_101", [&]() {
		m_isAttackOn = false;
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_102", 0, [&]() {
		m_pPipe_Long->AttachOff();
		}
	);

	// About Pipe Short
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_030", 0, [&]() {
		m_pPipeL->AttachOn();
		m_pPipeR->AttachOn();
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_201", 0, [&]() {
		auto pState = dynamic_cast<CRekkaState_PipeAtt_Short*>(m_pCurState);
		if (pState == nullptr) return;
		pState->DashOn();
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_201", 1, [&]() {
		auto pState = dynamic_cast<CRekkaState_PipeAtt_Short*>(m_pCurState);
		if (pState == nullptr) return;
		pState->AttackOn();
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_201", 2, [&]() {
		m_curAttackBone = EAttackBone::RHand;
		m_fAttackScale = 4.f;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetLastAnimCallback("em2000_atk-pipe_201", [&]() {
		m_isAttackOn = false;
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_210", 0, [&]() {
		auto pState = dynamic_cast<CRekkaState_PipeAtt_Short*>(m_pCurState);
		if (pState == nullptr) return;
		pState->DashOn();
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_210", 1, [&]() {
		auto pState = dynamic_cast<CRekkaState_PipeAtt_Short*>(m_pCurState);
		if (pState == nullptr) return;
		pState->AttackOn();
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_210", 2, [&]() {
		m_curAttackBone = EAttackBone::RHand;
		m_fAttackScale = 4.f;
		m_isAttackOn = true;
		}
	);
	m_pModelCom->SetLastAnimCallback("em2000_atk-pipe_210", [&]() {
		m_isAttackOn = false;
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_050", 0, [&]() {
		m_pPipeL->PrepareAttack();
		m_pPipeR->PrepareAttack();
		}
	);
	m_pModelCom->SetAnimCallback("em2000_atk-pipe_050", 1, [&]() {
		m_pPipeL->AttackSpecial();
		m_pPipeR->AttackSpecial();
		}
	);
}

_bool CMonster_Rekka::IsAnimFinished()
{
	return m_pModelCom->Get_Finished();
}

void CMonster_Rekka::SetAnim(const string& strAnimName, _bool isLoop)
{
	m_pModelCom->Set_AnimIndexNonCancle(strAnimName, isLoop);
}

void CMonster_Rekka::SetAnimHard(const string& strAnimName, _bool isLoop)
{
	m_pModelCom->Set_AnimIndex_NoInterpole(strAnimName, isLoop);
}

_bool CMonster_Rekka::GravityCheck()
{
	PxVec3 pos = { (_float)m_pCCT->Get_Controller()->getFootPosition().x,(_float)m_pCCT->Get_Controller()->getFootPosition().y,(_float)m_pCCT->Get_Controller()->getFootPosition().z };
	PxRaycastBuffer hit;
	PxVec3 origin = pos;
	PxVec3 direction = PxVec3(0, -1, 0); // 아래 방향으로 레이캐스팅
	PxReal distance = 0.5f; // 계단의 높이보다 살짝 큰 값
	direction.normalize();

	return GAMEINSTANCE->Get_Scene()->raycast(origin, direction, distance, hit);
}

void CMonster_Rekka::CCT_ON()
{
	m_isCCT_Disable = false;
	m_pCCT->Disable(false);
}

void CMonster_Rekka::CCT_Off()
{
	m_isCCT_Disable = true;
	m_pCCT->Disable();
}

void CMonster_Rekka::GravityOn()
{
	m_pCCT->Enable_Gravity();
}

void CMonster_Rekka::AnimationOff()
{
	m_isAnimPlay = false;
}

void CMonster_Rekka::AnimationOn(_float fFrameBegin)
{
	m_pModelCom->Set_AnimFrame(fFrameBegin);
	m_isAnimPlay = true;
}

void CMonster_Rekka::GravityOff()
{
	m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pCCT->Disable_Gravity();
}

void CMonster_Rekka::SelectBeat()
{
	if (RekkaState::GIMMICK == m_pCurState->GetState())
	{
		m_fBeatSpeed = 1.f;
		return;
	}

	_float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

	_float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

	m_fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
}

void CMonster_Rekka::MoveStraight(_float fSpeed)
{
	_vector vCurLook = m_pTransformCom->Get_NormLook();
	m_pCCT->Add_Displacement(vCurLook * fSpeed);
}

void CMonster_Rekka::MoveRight(_float fSpeed)
{
	_vector vCurRight = m_pTransformCom->Get_NormRight();
	m_pCCT->Add_Displacement(vCurRight * fSpeed);
}

void CMonster_Rekka::MoveUp(_float fSpeed)
{
	_vector vUpVector = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_pCCT->Add_Displacement(vUpVector * fSpeed);
}

void CMonster_Rekka::SetPosition_Hard(_vector vTargetPos)
{
	m_pTransformCom->Set_Position(vTargetPos);
	m_pCCT->Set_Position(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CMonster_Rekka::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
	_float4x4 matResult{};
	XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(&m_matCurrentPos));
	CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier * m_fBeatSpeed, isBillBoard);
}

void CMonster_Rekka::PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard)
{
	CEffect_Manager::Get_Instance()->Call_Union_Effect(strEffect, &m_matCurrentPos, fmultiplier * m_fBeatSpeed, isBillBoard);
}

shared_ptr<CMonster_Rekka> CMonster_Rekka::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMonster_Rekka
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Rekka(pDevice, pContext) {}
	};

	shared_ptr<CMonster_Rekka> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Rekka"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMonster_Rekka::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMonster_Rekka
	{
		MakeSharedEnabler(const CMonster_Rekka& rhs) : CMonster_Rekka(rhs) {}
	};

	shared_ptr<CMonster_Rekka> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Rekka"));
		assert(false);
	}

	return pInstance;
}

void CMonster_Rekka::Free()
{
	Safe_Delete(m_pCurState);
}
