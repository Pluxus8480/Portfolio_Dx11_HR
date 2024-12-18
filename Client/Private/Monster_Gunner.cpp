#include "Monster_Gunner.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Shader.h"
#include "Model.h"
#include "CharacterController.h"

#include "GameManager.h"
#include "Beat_Manager.h"
#include "Effect_Manager.h"

#include "CombatManager.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "GunnerState_Intro.h"
#include "GunnerState_Idle.h"
#include "GunnerState_Move.h"
#include "GunnerState_Attack.h"
#include "GunnerState_Down.h"
#include "GunnerState_Damaged.h"
#include "GunnerState_Dead.h"

#include "PickUpItem.h"

typedef CGunnerState::State GunnerState;


wstring CMonster_Gunner::ObjID = TEXT("Gunner");

CMonster_Gunner::CMonster_Gunner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonsterBase{ pDevice, pContext }
{
}

CMonster_Gunner::CMonster_Gunner(const CMonster_Gunner& rhs)
    : CMonsterBase{ rhs }
{
}

CMonster_Gunner::~CMonster_Gunner()
{
    Free();
}

HRESULT CMonster_Gunner::Initialize_Prototype()
{
    _matrix		LocalTransformMatrix = XMMatrixIdentity();
    LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Monster_Gunner"), TEXT("../Bin/Resources/Models/Monsters/Gunner/Gunner.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );

    return S_OK;
}

HRESULT CMonster_Gunner::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC transformDesc{};
    if (FAILED(__super::Initialize(&transformDesc)))
        return E_FAIL;

    auto pDesc = static_cast<Monster_DESC*>(pArg);
    _float4x4 matTrans = pDesc->matTransform;
    string strDesc = pDesc->strDesc;
    _matrix mMat = XMLoadFloat4x4(&matTrans);
    _bool isNan = XMMatrixIsNaN(mMat);
    mMat.r[0] = XMVector4Normalize(mMat.r[0]);
    mMat.r[1] = XMVector4Normalize(mMat.r[1]);
    mMat.r[2] = XMVector4Normalize(mMat.r[2]);
    m_pTransformCom->Set_WorldMatrix(mMat);

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
    m_pCurState = new CGunnerState_Idle(dynamic_pointer_cast<CMonster_Gunner>(shared_from_this()), GunnerState::IDLE);
    m_pCurState->ChangeState(nullptr, m_pCurState);

    // For tool setted effects
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fBeatSpeed, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fBeatSpeed, isBillBoard);
        }
    );

    if (isNan)
        Dead();
    else
        CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());
    return S_OK;
}

void CMonster_Gunner::PriorityTick(_float fTimeDelta)
{
    if (m_isAirState)
    {
        PxControllerState pCS;
        m_pCCT->Get_Controller()->getState(pCS);
        if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
        {
            auto pDamagedState = dynamic_cast<CGunnerState_Damaged*>(m_pCurState);
            if (pDamagedState != nullptr && !pDamagedState->DoingAirBone())
            {
                AirStateOFF();

                auto pStateDown = new CGunnerState_Down(dynamic_pointer_cast<CMonster_Gunner>(shared_from_this()), m_pCurState->GetState());
                m_pCurState->ChangeState(m_pCurState, pStateDown);
                m_pCurState = pStateDown;
            }
        }
    }
    auto pIntroState = dynamic_cast<CGunnerState_Intro*>(m_pCurState);
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

    // Update AttackTimer
    if (m_pCurState->GetState() != GunnerState::ATTACK && m_pCurState->GetState() != GunnerState::DAMAGED)
        m_fTimer_Attack -= fTimeDelta;
}

void CMonster_Gunner::Tick(_float fTimeDelta)
{
    // Update state
    CGunnerState* pNextState = m_pCurState->StepState(fTimeDelta);
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
        m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
    }

    // Update Physics
    if (m_isCCTWork)
        m_pCCT->Update(fTimeDelta);

    // Update Colliders
    _matrix matrixSpine = NormalizeMatrix(m_pMat_Spine);
    m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());
    _matrix matrixGun = XMMatrixTranslationFromVector({ 0.f, 1.f, 6.8f, 1.f});
    m_pCollider_Attack->Update(matrixGun * m_pTransformCom->Get_WorldMatrix_XMMat());

    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();
}

void CMonster_Gunner::LateTick(_float fTimeDelta)
{
    if (m_pCurState->IsColliderOn())
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
    if (m_isAttackOn)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);

    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
}

HRESULT CMonster_Gunner::Render()
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
    if (m_pCurState->IsColliderOn())
        m_pCollider_Hit->Render();
    if (m_isAttackOn)
        m_pCollider_Attack->Render();
#endif

    return S_OK;
}

HRESULT CMonster_Gunner::Render_Shadow()
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

void CMonster_Gunner::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack)
    {
        DamageInfo damageInfo{};
        damageInfo.fAmountDamage = 15.f;
        damageInfo.eAttackType = EAttackType::Hard;
        damageInfo.pObjectOwner = shared_from_this();
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

void CMonster_Gunner::TakeDamage(const DamageInfo& damageInfo)
{
    if (GunnerState::DEAD == m_pCurState->GetState())
        return;

    _float fTargetHp = m_fCurHp - damageInfo.fAmountDamage;
    /* Play electric effect
    if (m_fCurHp > 0.f && fTargetHp <= 0.f)
        PlayUnionEffect("", 1.f, false);
    */
    m_fCurHp = fTargetHp;
    if (m_fCurHp <= 0.f)
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
    auto pStateDamaged = new CGunnerState_Damaged(dynamic_pointer_cast<CMonster_Gunner>(shared_from_this()), m_pCurState->GetState(), damageInfo);
    m_pCurState->ChangeState(m_pCurState, pStateDamaged);
    m_pCurState = pStateDamaged;
}

void CMonster_Gunner::Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType)
{
    CGimmickBase::COMMONOBJ_DESC desc{};
    desc.fRotationPerSec = 0.f;
    desc.fSpeedPerSec = 0.f;

    for (size_t i = 0; i < iNumber; i++)
    {
        _float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
        _vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
        _float4x4 matResults{};
        XMStoreFloat4x4(&matResults, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));

        _float4x4 matResult{};
        XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matResults) * XMLoadFloat4x4(&m_matCurrentPos));

        XMStoreFloat4(&desc.vPosition, XMLoadFloat4x4(&matResult).r[3]);
        XMStoreFloat4x4(&desc.worldMatrix, XMLoadFloat4x4(&matResult));

        auto pPickup = static_pointer_cast<CPickUpItem>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"PickUp", CPickUpItem::ObjID, &desc));
        CPickUpItem::PICKUP_DESC PUDESC;
        PUDESC.eSize = (CPickUpItem::SIZE)(rand() % 2);
        PUDESC.eType = pickupType;
        pPickup->Model_Change(PUDESC);
        pPickup->Execute();
    }
}

HRESULT CMonster_Gunner::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    // Init Model and Matrices
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(CGame_Manager::Get_Instance()->Get_Current_Level(), TEXT("Model_Monster_Gunner"), TEXT("Com_Model")));
    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");

    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 2.0f;
    cctDesc.radius = 0.5f;
    cctDesc.position = PxExtendedVec3(0.0f, 1.0f, 0.0f);
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));

    {
        CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
        sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        sphereDesc.fRadius = 0.6f;
        m_pCollider_Hit =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
        m_pCollider_Hit->Set_Owner(shared_from_this());
        m_pCollider_Hit->Set_ColliderTag(TEXT("Monster_Hit"));
    }
    {
        CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
        obbDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        obbDesc.vExtents = { 1.f, 1.f, 12.f };
        obbDesc.vRotation = { 0.f, 0.f, 0.f };
        m_pCollider_Attack =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));
        m_pCollider_Attack->Set_Owner(shared_from_this());
        m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));
    }

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
    return S_OK;
}

void CMonster_Gunner::PrepareCallbacks()
{
    // Attack
    m_pModelCom->SetAnimCallback("em0200_atk-gun_002", 0, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("em0200_atk-gun_002", [&]() { m_isAttackOn = false; });

    // Dead
    m_pModelCom->SetAnimCallback("em0200_dmg-die_100", 0, [&]() { MonsterDead(); });
    m_pModelCom->SetAnimCallback("em0200_dmg-die_010", 0, [&]() { MonsterDead(); });

    m_pModelCom->SetAnimCallback("em0200_dmg-mid_000", 0, [&]() { if (IsDeadState()) MonsterDead(); });
    m_pModelCom->SetAnimCallback("em0200_dmg-mid_010", 0, [&]() { if (IsDeadState()) MonsterDead(); });
    m_pModelCom->SetAnimCallback("em0200_dmg-mid_020", 0, [&]() { if (IsDeadState()) MonsterDead(); });
    m_pModelCom->SetAnimCallback("em0200_dmg-mid_600", 0, [&]() { if (IsDeadState()) MonsterDead(); });
}

void CMonster_Gunner::SetPos_Hard(_fvector vPos)
{
    m_pTransformCom->Set_Position(vPos);
}

void CMonster_Gunner::MoveToPos(_fvector vTargetPos)
{
    auto pStateIntro = new CGunnerState_Intro(dynamic_pointer_cast<CMonster_Gunner>(shared_from_this()), m_pCurState->GetState(), vTargetPos);
    m_pCurState->ChangeState(m_pCurState, pStateIntro);
    m_pCurState = pStateIntro;
}

void CMonster_Gunner::MonsterDead()
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMMatrixIdentity());
    PlayEffect("Enemy_Boom", matResult, 1.f, true);

    Dead();
    Make_PickUp_Object(5, CPickUpItem::PICKUP::GEAR);
    Make_PickUp_Object(5, CPickUpItem::PICKUP::HP);
    CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
}

_bool CMonster_Gunner::IsAnimFinished()
{
    return m_pModelCom->Get_Finished();
}

void CMonster_Gunner::SetAnim(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndexNonCancle(strAnimName, isLoop);
}

void CMonster_Gunner::SetAnimHard(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndex_NoInterpole(strAnimName, isLoop);
}

void CMonster_Gunner::GravityOn()
{
    m_pCCT->Enable_Gravity();
}

void CMonster_Gunner::GravityOff()
{
    m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    m_pCCT->Disable_Gravity();
}

void CMonster_Gunner::AirStateON()
{
    m_isAirState = true;
    GravityOff();
}

void CMonster_Gunner::AirStateOFF()
{
    m_isAirState = false;
    GravityOn();
}

void CMonster_Gunner::MoveStraight(_float fSpeed)
{
    _vector vCurLook = m_pTransformCom->Get_NormLook();
    m_pCCT->Add_Displacement(vCurLook * fSpeed);
}

void CMonster_Gunner::MoveUp(_float fSpeed)
{
    _vector vUpVector = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    m_pCCT->Add_Displacement(vUpVector * fSpeed);
}

void CMonster_Gunner::Set_Position(_matrix mMat)
{
    m_pCCT->Set_Position(mMat);
}

void CMonster_Gunner::CCT_ON()
{
    m_isCCTWork = true;
    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Enable_Gravity();
    m_pCCT->Set_Gravity(40.f);
}

void CMonster_Gunner::CCT_OFF()
{
    m_isCCTWork = false;
    m_pCCT->Disable_Pseudo_Physics();
    m_pCCT->Disable_Gravity();
}

void CMonster_Gunner::SelectBeat()
{
    _float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

    m_fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
}

void CMonster_Gunner::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(&m_matCurrentPos));
    CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier * m_fBeatSpeed, isBillBoard);
}
void CMonster_Gunner::PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard)
{
    CEffect_Manager::Get_Instance()->Call_Union_Effect(strEffect, &m_matCurrentPos, fmultiplier * m_fBeatSpeed, isBillBoard);
}

_bool CMonster_Gunner::IsNeedToMove()
{
    return m_fDistance > m_fDistance_Far_C;
}

shared_ptr<CMonster_Gunner> CMonster_Gunner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_Gunner
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Gunner(pDevice, pContext) {}
    };

    shared_ptr<CMonster_Gunner> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Gunner"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_Gunner::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_Gunner
    {
        MakeSharedEnabler(const CMonster_Gunner& rhs) : CMonster_Gunner(rhs) {}
    };

    shared_ptr<CMonster_Gunner> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Gunner"));
        assert(false);
    }

    return pInstance;
}

void CMonster_Gunner::Free()
{
    Safe_Delete(m_pCurState);
}
