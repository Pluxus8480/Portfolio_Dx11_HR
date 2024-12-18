#include "Monster_SwordMachine.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Shader.h"
#include "Model.h"
#include "CharacterController.h"

#include "GameManager.h"
#include "Beat_Manager.h"
#include "Effect_Manager.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"


#include "CombatManager.h"
#include "PickUpItem.h"
#include "GimmickBase.h"

#include "PartObject.h"
#include "SwordMachine_Sword.h"

#include "Chai.h"
#include "Player_Camera.h"
#include "CUi_StunGaugeSystem.h"
wstring CMonster_SwordMachine::ObjID = TEXT("SwordMachine");

CMonster_SwordMachine::CMonster_SwordMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonsterBase{ pDevice, pContext }
{
}

CMonster_SwordMachine::CMonster_SwordMachine(const CMonster_SwordMachine& rhs)
    : CMonsterBase(rhs)
{
}

CMonster_SwordMachine::~CMonster_SwordMachine()
{
    Free();
}

HRESULT CMonster_SwordMachine::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SwordMachine"), TEXT("../Bin/Resources/Models/Monsters/SwordMachine/SwordMachine.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_Owning_ObjectID.push_back(CSwordMachine_Sword::ObjID);
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("SwordMachine_Shield_Break"), TEXT("../Bin/Resources/Models/Monsters/SwordMachine/Sound/Shield_Break.ogg")));
    return S_OK;
}

HRESULT CMonster_SwordMachine::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    //TransformDesc.vPosition = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vPosition;
    TransformDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    auto pDesc = static_cast<Monster_DESC*>(pArg);
    _float4x4 matTrans = pDesc->matTransform;
    _matrix mMat = XMLoadFloat4x4(&matTrans);
    _bool isNan = XMMatrixIsNaN(mMat);
    mMat.r[0] = XMVector4Normalize(mMat.r[0]);
    mMat.r[1] = XMVector4Normalize(mMat.r[1]);
    mMat.r[2] = XMVector4Normalize(mMat.r[2]);
    m_pTransformCom->Set_WorldMatrix(mMat);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    vector<shared_ptr<CAnimation>> animList = m_pModelCom->Get_Animation_List();
    for (auto& iter : animList)
    {
        m_vecAnimList.push_back(iter->Get_AnimName());
    }

    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matCurrentPos, 1);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matCurrentPos, 1);
        }
    );
    //m_pTransformCom->Set_Position(XMVectorSet(5.7f, 0.f, 3.f, 1.f));

    if (isNan)
        Dead();
    else
        CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());
    PrepareCallbacks();
    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Enable_Gravity();
    m_pCCT->Set_Gravity(40.f);

    m_pModelCom->Set_AnimIndex("em0700_idle_100", false);
    m_strCurrent_Pattern = "em0700_sword-a_100";
    m_strNext_Pattern = "em0700_sword-a_100";
    m_bPlayerLook = true;

    m_fCurHp = 100.f;
    m_fBarrier = 60;

    CUi_StunGaugeSystem::Desc Desc;
    Desc.fMaxStunGauge = m_fBarrier;
    Desc.pCurStunGauge = &m_fBarrier;
    Desc.iStar = 1;
    Desc.pTargetTransform = m_pTransformCom;
    Desc.RelativeDistance = { 0.f, 1.4f, 0 };
    m_pBarrier_UI = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StunGaugeSystem::ObjID, m_iLevelIndex, L"CUi", CUi_StunGaugeSystem::ObjID, &Desc);


    _float4x4 xMat = {};
    m_pModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, xMat);
    return S_OK;
}

void CMonster_SwordMachine::PriorityTick(_float fTimeDelta)
{
    XMStoreFloat4(&m_vPlayerPos, PLAYERMANAGER->Get_PlayerPos());
}

void CMonster_SwordMachine::Tick(_float fTimeDelta)
{
    m_fShakeTime -= fTimeDelta;
    SelectBeat();

    Animation_End();

    Animation(fTimeDelta);

    for (auto& pPartObj : m_pWeapon)
        pPartObj->Tick(fTimeDelta);

    _float4 fPos = {};
    XMStoreFloat4(&fPos,m_pTransformCom->Get_Position());

    //if(GAMEINSTANCE->Get_KeyDown(DIK_B))
    //    m_bPlayerLook = !m_bPlayerLook;
    //m_pTransformCom->LookAt_Horizontal(XMLoadFloat4(&m_vPlayerPos));

    Collision_Down(fTimeDelta);

    DownState(fTimeDelta);
    if(m_eState != SWORDMACHINE_STATE::DOWN)
        Pattern_Change(fTimeDelta);

    // Update Colliders
    auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
    m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());

    auto matrixRightHand = XMLoadFloat4x4(m_pMat_RightHand);
    matrixRightHand.r[0] = XMVector3Normalize(matrixRightHand.r[0]);
    matrixRightHand.r[1] = XMVector3Normalize(matrixRightHand.r[1]);
    matrixRightHand.r[2] = XMVector3Normalize(matrixRightHand.r[2]);
    auto matAttackResult = matrixRightHand * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pCollider_Attack->Update(matAttackResult);

    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

    m_pCCT->Update(fTimeDelta);
    LookPlayerPos(fTimeDelta);
}

void CMonster_SwordMachine::LateTick(_float fTimeDelta)
{

   if(m_bHitOn)
       GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
   if (m_isAttackOn)
       GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);

   for (auto& pPartObj : m_pWeapon)
       pPartObj->LateTick(fTimeDelta);

   m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
   m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
    
}

HRESULT CMonster_SwordMachine::Render()
{
    _float4x4 matShakingWorld = m_pTransformCom->Get_WorldMatrix();
    if (m_fShakeTime > 0.f)
    {
        _float3 vResult = GAMEINSTANCE->Random_Float3({ -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f });
        _matrix matWorld = XMLoadFloat4x4(&matShakingWorld);
        matWorld.r[3] += XMVectorSetW(XMLoadFloat3(&vResult), 0) * m_fShakeMagnitude;
        XMStoreFloat4x4(&matShakingWorld, matWorld);
    }

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matShakingWorld)))
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
#ifdef _DEBUG
    m_pCollider_Hit->Render();
    m_pCollider_Attack->Render();
#endif
    return S_OK;
}

HRESULT CMonster_SwordMachine::Render_Shadow()
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

HRESULT CMonster_SwordMachine::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SwordMachine"), TEXT("Com_Model")));

    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
    m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_hand_attach_00");

    CSwordMachine_Sword::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("r_hand_attach_00");
    WeaponDesc.pParentTransform = m_pTransformCom;
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;
    WeaponDesc.iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();
    shared_ptr <CPartObject> pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CSwordMachine_Sword"), &WeaponDesc));
    if (nullptr == pWeapon)
        return E_FAIL;
    m_pWeapon.push_back(pWeapon);

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);


    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 0.7f;
    cctDesc.radius = 0.1f;
    cctDesc.stepOffset = 0.2f;
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));


    {
        CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
        sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        sphereDesc.fRadius = 1.f;
        m_pCollider_Hit =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
        m_pCollider_Hit->Set_Owner(shared_from_this());
        m_pCollider_Hit->Set_ColliderTag(TEXT("Monster_Hit"));
    }
    {
        CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
        obbDesc.vCenter = _float3{ 0.f, 0.f, -0.9f };
        obbDesc.vExtents = _float3{ 0.28f, 0.2f, 1.f };
        obbDesc.vRotation = _float3{ 0.f, 0.f, 0.f };
        m_pCollider_Attack =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));
        m_pCollider_Attack->Set_Owner(shared_from_this());
        m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));
    }

    return S_OK;
}

shared_ptr<CMonster_SwordMachine> CMonster_SwordMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_SwordMachine
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_SwordMachine(pDevice, pContext)
        {}
    };


    shared_ptr<CMonster_SwordMachine> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_SwordMachine"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_SwordMachine::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_SwordMachine
    {
        MakeSharedEnabler(const CMonster_SwordMachine& rhs) : CMonster_SwordMachine(rhs)
        {}
    };


    shared_ptr<CMonster_SwordMachine> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_SwordMachine"));
        assert(false);
    }

    return pInstance;
}

void CMonster_SwordMachine::Free()
{
}

void CMonster_SwordMachine::Dying()
{

}

void CMonster_SwordMachine::Animation_End()
{
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_sword-a_100")
        && m_pModelCom->Get_Finished())
    {
        Pattern_Wait();
        m_bTurn = true;
    } 
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_sword-a_101")
        && m_pModelCom->Get_Finished())
    {
        Pattern_Wait();
        m_bTurn = true;
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_atk_020")
        && m_pModelCom->Get_Finished())
    {
        Pattern_Wait();
        m_bTurn = true;
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_040")
        && m_pModelCom->Get_Finished())
    {
        m_isAirState = false;
        m_eState = SWORDMACHINE_STATE::DOWN;
        GravityOn();
        m_pModelCom->Set_AnimIndex("em0700_dmg-down_000", true);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-high_500")
        && m_pModelCom->Get_Finished())
    {
        m_eState = SWORDMACHINE_STATE::DOWN;
        GravityOn();
        m_pModelCom->Set_AnimIndex("em0700_dmg-down_100", true);
    }
    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_250") )
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0700_dmg-down_000", true);
    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_210"))
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0700_dmg-down_100", true);

    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_010") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_110"))
        && m_pModelCom->Get_Finished())
    {
        m_bTurn = true;
        m_eState = SWORDMACHINE_STATE::IDLE;
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_000")
        && m_pModelCom->Get_Finished())
    {
        m_bAirLunched = true;
        m_isAirState = false;
        GravityOn();
    }
    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_200") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_210")  ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_220"))
        && m_pModelCom->Get_Finished())
    {
       
        m_pModelCom->Set_AnimIndex("em0700_dmg-air_050", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_050")
        && m_pModelCom->Get_Finished())
    {
        GravityOn();
        m_pModelCom->Set_AnimIndex("em0700_dmg-air_050", true);
    }

    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_die_100"))
        && m_pModelCom->Get_Finished())
    {
        Make_PickUp_Object(5, CPickUpItem::PICKUP::GEAR);
        Make_PickUp_Object(5, CPickUpItem::PICKUP::HP);
        _float4x4 matResult{};
        XMStoreFloat4x4(&matResult, XMMatrixIdentity());
        PlayEffect("Enemy_Boom", matResult, 1.f, true);
        Dead();
        CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
        m_pBarrier_UI->Dead();


    }
    else if (m_pModelCom->Get_Finished() && m_eState != SWORDMACHINE_STATE::DOWN)
    {
        if (m_bPlayerLook == false)
            m_bTurn = true;
        Pattern_Wait();
    }

}

void CMonster_SwordMachine::Animation(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    /* 반박자에 끝나는 모션 ex) 단타 공격모션 */
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * m_fTimeMultiflier;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f * m_fTimeMultiflier;
    _float Three_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.3333f) * m_fTimeMultiflier;
    _float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f * m_fTimeMultiflier;
    _float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f) * m_fTimeMultiflier;
    _float Six_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f) * m_fTimeMultiflier;
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f) * m_fTimeMultiflier;
    _float Nine_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.111f) * m_fTimeMultiflier;

    _uint index = m_pModelCom->Get_CurrentAnimIndex();
    _float fTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();


    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_sword-a_100"))
    {
        if (m_pModelCom->Get_Current_Track_Position() > 58.f && m_pModelCom->Get_Current_Track_Position() < 63.f)
            m_bHitOn = false;
        else
            m_bHitOn = true;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f, fmatResult);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_sword-a_101"))
    {
        if (m_pModelCom->Get_Current_Track_Position() > 43.f && m_pModelCom->Get_Current_Track_Position() < 47.f)
            m_bHitOn = false;
        else
            m_bHitOn = true;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_atk_020"))
    {
        if (m_pModelCom->Get_Current_Track_Position() > 73.f && m_pModelCom->Get_Current_Track_Position() < 77.f)
            m_bHitOn = false;
        else if (m_pModelCom->Get_Current_Track_Position() > 88.f && m_pModelCom->Get_Current_Track_Position() < 92.f)
            m_bHitOn = false;
        else
            m_bHitOn = true;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Eight_Beat, 0.1f, fmatResult);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_other_000"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat , 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_other_001"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat , 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_000"))
    {
        m_bPlayerLook = false;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_die_100"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-high_500"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat , 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_100"))
    {
        m_bPlayerLook = false;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    }
    else
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat , 0.1f, fmatResult);

    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);

    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
}

void CMonster_SwordMachine::Move(_float fTimeDelta)
{
}

void CMonster_SwordMachine::Attack(_float fTimeDelta)
{
}

void CMonster_SwordMachine::LookPlayerPos(_float fTimeDelta)
{
    if (m_bPlayerLook && !m_bTurn)
        m_pTransformCom->LookAt_Horizontal(PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position());

    else if (m_bTurn)
    {
        m_fTurnTimer -= fTimeDelta * 5.f;
        m_pTransformCom->LookAt_Horizontal_With_Speed(XMLoadFloat4(&m_vPlayerPos), fTimeDelta * 5.f);
        if (m_fTurnTimer < 0.f)
        {
            m_fTurnTimer = 1.f;
            m_bTurn = false;
            m_bPlayerLook = true;
        }
    }
}

void CMonster_SwordMachine::DownState(_float fTimeDelta)
{
    _float fBeat = BEATMANAGER->Get_Beat();
    _float fBeatRate = BEATMANAGER->Get_BeatRate();
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_000") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_100") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_010") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_110") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_210") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_250")
        )
        m_fDownTimer += fTimeDelta;

    if (m_fDownTimer > fBeat * 5.f && fBeatRate > 0.92f)
    {
        m_bTurn = true;
        m_fDownTimer = 0.f;
        m_eState = SWORDMACHINE_STATE::IDLE;
        if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_000"))
            m_pModelCom->Set_AnimIndexNonCancle("em0700_dmg-down_010", false);
        else if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_100"))
            m_pModelCom->Set_AnimIndexNonCancle("em0700_dmg-down_110", false);

        GravityOn();
    }
}

void CMonster_SwordMachine::Pattern_Change(_float fTimeDelta)
{
    _float fBeat = BEATMANAGER->Get_Beat();
    _float fBeatRate = BEATMANAGER->Get_BeatRate();

    if (m_bPattern_Timer_On)
        m_fPatternTimer += fTimeDelta;
    if (m_fPatternTimer > fBeat * 7.f && fBeatRate > 0.96f)
    {
        m_fPatternTimer = 0.f;
        m_bPattern_Timer_On = false;

        m_strCurrent_Pattern = m_strNext_Pattern;
        m_pModelCom->Set_AnimIndex(m_strCurrent_Pattern, false);
      
        if (m_strCurrent_Pattern == "em0700_sword-a_100")
        {
            m_bPlayerLook = false;
            m_strNext_Pattern = "em0700_sword-a_101";
        }
        else if (m_strCurrent_Pattern == "em0700_sword-a_101")
        {
            m_bPlayerLook = false;
            m_strNext_Pattern = "em0700_atk_020";
        }
        else if (m_strCurrent_Pattern == "em0700_atk_020")
        {
            m_bPlayerLook = false;
            m_strNext_Pattern = "em0700_sword-a_100";
        }
    }
}

void CMonster_SwordMachine::Play_Pattern(_float fTimeDelta)
{
}

void CMonster_SwordMachine::Pattern_Wait()
{
    _float distance = Calculate_Distance_To_Player();
    m_bPattern_Timer_On = true;


    
    if (distance < 3.f)
    {
        _int iRandNum = GAMEINSTANCE->Random_Int(1, 2);
        if (iRandNum == 1)
            m_pModelCom->Set_AnimIndex("em0700_other_000", false);
        else
            m_pModelCom->Set_AnimIndex("em0700_other_001", false);
    }
    else if (distance < 7.f)
        m_pModelCom->Set_AnimIndex("em0700_mvnt-walk-st_010", false);
    else if (distance < 10.f)
    {
        _int iRandNum = GAMEINSTANCE->Random_Int(1, 2);
        if (iRandNum == 1)
            m_pModelCom->Set_AnimIndex("em0700_mvnt-escape_101", false);
        else
            m_pModelCom->Set_AnimIndex("em0700_mvnt-escape_100", false);
    }
    else
        m_pModelCom->Set_AnimIndex("em0700_mvnt-walk-st_020", false);

  /* if (distance > 10.f)
       m_pModelCom->Set_AnimIndex("em0700_mvnt-walk-st_020", false);
   else
       m_pModelCom->Set_AnimIndex("em0700_mvnt-walk-st_010", false);*/
   
}

_float CMonster_SwordMachine::Calculate_Distance_To_Player()
{
    _float resDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position()));
    return resDistance;
}

void CMonster_SwordMachine::GravityOn()
{
    m_pCCT->Enable_Gravity();
}

void CMonster_SwordMachine::GravityOff()
{
    m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    m_pCCT->Disable_Gravity();
}

void CMonster_SwordMachine::AirStateON()
{
    m_isAirState = true;
    GravityOff();
}

void CMonster_SwordMachine::AirStateOFF()
{
    m_isAirState = false;
    GravityOn();
}

void CMonster_SwordMachine::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack)
    {
        DamageInfo damageInfo{};
        damageInfo.fAmountDamage = 10.f;
        damageInfo.eAttackType = EAttackType::Base;
        damageInfo.pObjectOwner = shared_from_this();
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

void CMonster_SwordMachine::TakeDamage(const DamageInfo& damageInfo)
{
    if (IsDeadState())
        return;

    _float fTargetHp = m_fCurHp - damageInfo.fAmountDamage;
    PLAYERMANAGER->Add_PlayerRankScore(damageInfo.fAmountDamage / 1.5f);
    PLAYERMANAGER->Add_PlayerScore((_int)damageInfo.fAmountDamage * 15.f);

    m_fPatternTimer = 0.f;
    m_bPattern_Timer_On = false;
    m_bPlayerLook = false;

    CCombat_Manager::Get_Instance()->Add_Combo();
    if (m_fBarrier <= 0.f)
    {    
        m_fCurHp = fTargetHp;
    }
    else
    {
        _float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
        _vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
        _float4x4 matResult{};
        XMStoreFloat4x4(&matResult, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));
        PlayEffect("Big_HitEffect", matResult, 1.f, true);

        m_fShakeTime = 0.3f;
        m_fShakeMagnitude = 0.08f;
        m_fBarrier -= damageInfo.fAmountDamage;
        if(m_fBarrier <= 0.f)
            GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"SwordMachine_Shield_Break");
        if (damageInfo.eAttackType == EAttackType::Parry)
            m_fBarrier -= 10.f;
        return;
    }
    if (m_fCurHp <= 0.f)
    {
        m_bPlayerLook = false;
        GravityOn();
        m_pModelCom->Set_AnimIndex_NoInterpole("em0700_die_100", false);
        _int waveNum = CCombat_Manager::Get_Instance()->Get_Wave_Num();
        list <shared_ptr<CGameObject>> enemyList = CCombat_Manager::Get_Instance()->Get_Enemy_List();
        if (enemyList.size() == 1 && waveNum == 0)
        {
            m_bFinishBlow = true;
            PLAYERMANAGER->Bullet_Time(2.f, 0.2f, shared_from_this());
            PLAYERMANAGER->Bullet_Time(2.f, 0.2f, PLAYERMANAGER->Get_Player());
            static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Zoom_In(2.f, 0.5f);
        }
        return;
    }

    _float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
    _vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

    _vector vToPlayerDir = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_Position());


    // Hardly look player
    _vector vTargetPos = m_pTransformCom->Get_Position() + vToPlayerDir;
    m_pTransformCom->LookAt_Horizontal(vTargetPos);

    if (m_isAirState)
    {
        GravityOff();
        if (EAttackDirection::DOWN == damageInfo.eAttack_Direction)
        {
            m_pCCT->Set_Velocity({ 0,-30.f,0,0 });
        }
        else
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            switch (damageInfo.eAttackType)
            {
            case EAttackType::Base:
            {
                shared_ptr<CTransform> playerTransform = PLAYERMANAGER->Get_Player()->Get_Transform();
                _matrix playerMat = playerTransform->Get_WorldMatrix_XMMat();
                _vector playerPos = playerMat.r[3];

                _vector backMovePos = {};
                //backMovePos = pOwner->Get_Transform()->Get_Position() + -pOwner->Get_Transform()->Get_NormLook() * 0.5f;
                backMovePos = (playerPos + playerTransform->Get_NormLook() * 2.f);
                backMovePos = XMVectorSetY(backMovePos, XMVectorGetY(backMovePos) + 1.5f);
                playerMat.r[3] = backMovePos;

                Set_Position(playerMat);

                GravityOff();
                switch (damageInfo.eAttack_Direction)
                {
                case EAttackDirection::DEFAULT: case EAttackDirection::UP:
                {
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-air_200",false);
                    break;
                }
                case EAttackDirection::LEFT:
                {
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-air_210", false);
                    break;
                }
                case EAttackDirection::RIGHT:
                {
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-air_220", false);
                    break;
                }
                }
            }
            }
        }
    }
    else // On the ground
    {
        if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_100") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_210"))
        {
            m_pModelCom->Set_AnimIndex("em0700_dmg-down_210", false);
            return;
        }
        else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_000") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0700_dmg-down_250"))
        {
            m_pModelCom->Set_AnimIndex("em0700_dmg-down_250", false);
            return;
        }

        switch (damageInfo.eAttackType)
        {
        case EAttackType::Base:
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            switch (damageInfo.eAttack_Direction)
            {
            case EAttackDirection::DEFAULT: case EAttackDirection::UP:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-low_000", false);
                break;
            case EAttackDirection::LEFT:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-low_010", false);
                break;
            case EAttackDirection::RIGHT:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-low_020", false);
                break;
            case EAttackDirection::DOWN:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-low_030", false);
                break;
            }

            break;
        }
        case EAttackType::Hard:
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            switch (damageInfo.eAttack_Direction)
            {
            case EAttackDirection::DEFAULT: case EAttackDirection::UP:
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-mid_000", false);
                break;
            case EAttackDirection::LEFT:
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-mid_010", false);
                break;
            case EAttackDirection::RIGHT:
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-mid_020", false);
                break;
            case EAttackDirection::DOWN:
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-mid_600", false);
                break;
            }
            if (damageInfo.strAttackName == "R")
                PLAYERMANAGER->Callback_Event("R");

            if (damageInfo.strAttackName == "RR")
                PLAYERMANAGER->Callback_Event("RR");

            break;
        }
        case EAttackType::SP:
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            if (IsDeadState())
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_010", false);
            else
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-high_500", false);

            break;
        }
        case EAttackType::Special:
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            if (IsDeadState())
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_010", false);
            else
                m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-high_500", false);


            //Make_PickUp_Object(5, CPickUpItem::PICKUP::BATTERY);

            if (damageInfo.strAttackName == "RRR")
            {
                if (!m_bFinishBlow)
                {
                    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, shared_from_this());
                    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, PLAYERMANAGER->Get_Player());
                }
                PLAYERMANAGER->Callback_Event("RRR");
            }
            else if (damageInfo.strAttackName == "LL_LLL")
            {
                if (!m_bFinishBlow)
                {
                    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, shared_from_this());
                    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, PLAYERMANAGER->Get_Player());
                }
                PLAYERMANAGER->Callback_Event("LL_LLL");
            }
            break;
        }
        case EAttackType::Parry:
        {
            m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-mid_000", false);
            
            m_fBarrier -= 10.f;
            m_isAttackOn = false;
            break;
        }
        case EAttackType::Air:
        {
            shared_ptr<CTransform> playerTransform = PLAYERMANAGER->Get_Player()->Get_Transform();
            _matrix playerMat = playerTransform->Get_WorldMatrix_XMMat();
            _vector playerPos = playerMat.r[3];

            _vector backMovePos = {};
            //backMovePos = pOwner->Get_Transform()->Get_Position() + -pOwner->Get_Transform()->Get_NormLook() * 0.5f;
            backMovePos = (playerPos + playerTransform->Get_NormLook() * 2.f);
            backMovePos = XMVectorSetY(backMovePos, XMVectorGetY(backMovePos) + 1.5f);
            playerMat.r[3] = backMovePos;

            Set_Position(playerMat);

            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            AirStateON();
            m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-air_000", false);

            m_doing_AirBone = true;
            break;
        }
        case EAttackType::Peppermint:
        {
            PlayEffect("PepperMint_HitEffect", matResult, 1.f, true);
            switch (damageInfo.eAttack_Direction)
            {
            case EAttackDirection::DEFAULT:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-low_000", false);
                break;
            default:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-low_000", false);
                break;
            }

            break;
        }
        case EAttackType::Macaron:
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            switch (damageInfo.eAttack_Direction)
            {
            case EAttackDirection::DEFAULT:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-high_500", false);
                break;
            default:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-high_500", false);
                break;
            }

            break;
        }
        case EAttackType::Korsica:
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            switch (damageInfo.eAttack_Direction)
            {
            case EAttackDirection::DEFAULT:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-mid_000", false);
                break;
            default:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0700_dmg-mid_000", false);
                break;
            }

            break;
        }
        }
    }
}

void CMonster_SwordMachine::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(&m_matCurrentPos));
    CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier * m_fBeatSpeed, isBillBoard);
}

void CMonster_SwordMachine::PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard)
{
    CEffect_Manager::Get_Instance()->Call_Union_Effect(strEffect, &m_matCurrentPos, fmultiplier * m_fBeatSpeed, isBillBoard);
}

void CMonster_SwordMachine::PrepareCallbacks()
{
    m_pModelCom->SetAnimCallback("em0700_atk_020", 0, [&]() {m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("em0700_atk_020", 1, [&]() {m_isAttackOn = false; });
    m_pModelCom->SetAnimCallback("em0700_atk_020", 2, [&]() {m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("em0700_atk_020", 3, [&]() {m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("em0700_atk_020", [&]() {m_isAttackOn = false; });

    m_pModelCom->SetAnimCallback("em0700_sword-a_100", 0, [&]() {m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("em0700_sword-a_100", 1, [&]() {m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("em0700_sword-a_100", [&]() {m_isAttackOn = false; });

    m_pModelCom->SetAnimCallback("em0700_sword-a_101", 0, [&]() {m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("em0700_sword-a_101", 1, [&]() {m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("em0700_sword-a_101", [&]() {m_isAttackOn = false; });
}

void CMonster_SwordMachine::SelectBeat()
{
    _float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

    m_fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
}

void CMonster_SwordMachine::Set_Position(_matrix mMat)
{
    m_pCCT->Set_Position(mMat);
}

void CMonster_SwordMachine::Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType)
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

void CMonster_SwordMachine::Collision_Down(_float fTimeDelta)
{
    PxControllerState pCS;
    m_pCCT->Get_Controller()->getState(pCS);
    if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
        if (
            (m_bAirLunched && currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_000")) ||
            currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_200") || 
            currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0700_dmg-air_050") 
            )
        {
            m_bAirLunched = false;
            m_pModelCom->Set_AnimIndexNonCancle("em0700_dmg-air_040", false);
        }
    }
}
