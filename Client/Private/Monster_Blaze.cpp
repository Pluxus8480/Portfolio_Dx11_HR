#include "Monster_Blaze.h"
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
#include "CUi_EnemySpecialAttack.h"
#include "Blaze_FireStarter.h"

wstring CMonster_Blaze::ObjID = TEXT("Blaze");

CMonster_Blaze::CMonster_Blaze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonsterBase{ pDevice, pContext }
{
}

CMonster_Blaze::CMonster_Blaze(const CMonster_Blaze& rhs)
    : CMonsterBase(rhs)
{
}

CMonster_Blaze::~CMonster_Blaze()
{
    Free();
}

HRESULT CMonster_Blaze::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Blaze"), TEXT("../Bin/Resources/Models/Monsters/Blaze/Blaze.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Blaze_Shield_Break"), TEXT("../Bin/Resources/Models/Monsters/Blaze/Sound/Shield_Break.ogg")));
    return S_OK;
}

HRESULT CMonster_Blaze::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    //TransformDesc.vPosition = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vPosition;
    TransformDesc.vPosition = { -0.f, 1.f, 5.f, 1.f };
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

    m_pModelCom->Set_AnimIndex("em0740_idle_000", false);
    m_strCurrent_Pattern = "em0740_atk_000";
    m_strNext_Pattern = "em0740_atk_000";
    m_bPlayerLook = true;

    m_pChai_Attach = m_pModelCom->Get_BoneMatrixPtr("prop_01");
    m_fCurHp = 150.f;
    m_fBarrier = 80.f;
    _float4x4 xMat = {};
    m_pModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, xMat);

    CUi_StunGaugeSystem::Desc Desc;
    Desc.fMaxStunGauge = m_fBarrier;
    Desc.pCurStunGauge = &m_fBarrier;
    Desc.iStar = 2;
    Desc.pTargetTransform = m_pTransformCom;
    Desc.RelativeDistance = { 0.f, 1.4f, 0 };
    m_pBarrier_UI = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StunGaugeSystem::ObjID, m_iLevelIndex, L"CUi", CUi_StunGaugeSystem::ObjID, &Desc);

  
    return S_OK;
}

void CMonster_Blaze::PriorityTick(_float fTimeDelta)
{
    XMStoreFloat4(&m_vPlayerPos, PLAYERMANAGER->Get_PlayerPos());
}

void CMonster_Blaze::Tick(_float fTimeDelta)
{
   XMStoreFloat4x4(&m_pL_Attach , XMMatrixScaling(3.f, 3.f, 3.f) * XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("l_attach_hand_00")) * m_pTransformCom->Get_WorldMatrix_XMMat() );
   XMStoreFloat4x4(&m_pR_Attach , XMMatrixScaling(3.f, 3.f, 3.f) * XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00")) * m_pTransformCom->Get_WorldMatrix_XMMat() );
    if(m_bFirePunch)
    {
        m_fFirePunch_Timer -= fTimeDelta;
        if (m_fFirePunch_Timer < 0.f)
        {
            m_fFirePunch_Timer = 0.5f;
            CEffect_Manager::Get_Instance()->Call_Union_Effect("FirePunch", &m_pL_Attach, 1.f, false);
            CEffect_Manager::Get_Instance()->Call_Union_Effect("FirePunch", &m_pR_Attach, 1.f, false);
        }
    }
    m_fShakeTime -= fTimeDelta;
    SelectBeat();

    Animation_End();

    Animation(fTimeDelta);

    _float4 fPos = {};
    XMStoreFloat4(&fPos, m_pTransformCom->Get_Position());

    Collision_Down(fTimeDelta);
    LookPlayerPos(fTimeDelta);
    DownState(fTimeDelta);
    if (m_eState != BlAZE_STATE::DOWN && !m_bRPMode)
        Pattern_Change(fTimeDelta);

    // Update Colliders
    auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
    m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());

 
    m_pCollider_RP->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());

    auto matrixRightHand = XMLoadFloat4x4(m_pMat_RightHand);
    matrixRightHand.r[0] = XMVector3Normalize(matrixRightHand.r[0]);
    matrixRightHand.r[1] = XMVector3Normalize(matrixRightHand.r[1]);
    matrixRightHand.r[2] = XMVector3Normalize(matrixRightHand.r[2]);
    auto matAttackResult = matrixRightHand * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pCollider_Attack->Update(matAttackResult);

    auto matrixLeftHand = XMLoadFloat4x4(m_pMat_LeftHand);
    matrixLeftHand.r[0] = XMVector3Normalize(matrixLeftHand.r[0]);
    matrixLeftHand.r[1] = XMVector3Normalize(matrixLeftHand.r[1]);
    matrixLeftHand.r[2] = XMVector3Normalize(matrixLeftHand.r[2]);
    matAttackResult = matrixLeftHand * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pCollider_Attack_Left->Update(matAttackResult);

    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

    m_pCCT->Update(fTimeDelta);
}

void CMonster_Blaze::LateTick(_float fTimeDelta)
{
    if (m_bRPMode)
        Judge_Attack(fTimeDelta);
    if(m_bHitOn)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
    if (m_isRPOn)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_RP);
    if (m_isAttackOn_Right)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);
    if (m_isAttackOn_Left)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack_Left);
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
}

HRESULT CMonster_Blaze::Render()
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
    m_pCollider_RP->Render();
    m_pCollider_Attack->Render();
    m_pCollider_Attack_Left->Render();
#endif
    return S_OK;
}

HRESULT CMonster_Blaze::Render_Shadow()
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

HRESULT CMonster_Blaze::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Blaze"), TEXT("Com_Model")));

    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
    m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
    m_pMat_LeftHand = m_pModelCom->Get_BoneMatrixPtr("l_attach_hand_00");

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
        CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
        sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        sphereDesc.fRadius = 1.f;
        m_pCollider_Attack =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Attack"), &sphereDesc));
        m_pCollider_Attack->Set_Owner(shared_from_this());
        m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));

        m_pCollider_Attack_Left = 
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Attack_Left"), &sphereDesc));
        m_pCollider_Attack_Left->Set_Owner(shared_from_this());
        m_pCollider_Attack_Left->Set_ColliderTag(TEXT("Monster_Attack_Left"));


        sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        sphereDesc.fRadius = 10.f;

        m_pCollider_RP =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Attack_RP"), &sphereDesc));
        m_pCollider_RP->Set_Owner(shared_from_this());
        m_pCollider_RP->Set_ColliderTag(TEXT("Monster_Attack_Left"));
    }

    return S_OK;
}

shared_ptr<CMonster_Blaze> CMonster_Blaze::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_Blaze
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Blaze(pDevice, pContext)
        {}
    };


    shared_ptr<CMonster_Blaze> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Blaze"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_Blaze::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_Blaze
    {
        MakeSharedEnabler(const CMonster_Blaze& rhs) : CMonster_Blaze(rhs)
        {}
    };


    shared_ptr<CMonster_Blaze> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Blaze"));
        assert(false);
    }

    return pInstance;
}

void CMonster_Blaze::Free()
{
}

void CMonster_Blaze::Dying()
{

}

void CMonster_Blaze::Animation_End()
{
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();


    if(m_bRPMode && m_pModelCom->Get_Finished() && !m_bRPMode_Start)
    {
        m_bRPMode_Start = true;
        m_pModelCom->Set_AnimIndex("em0740_atk-rp-cp_010", false);
        return;
    }
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_000")
        && m_pModelCom->Get_Finished())
    {
        Pattern_Wait();
        m_bTurn = true;
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_010")
        && m_pModelCom->Get_Current_Track_Position() > 60.f)
    {
        if (m_pModelCom->Get_Finished())
        {
            m_bFireLand = false;
            Pattern_Wait();
            m_bTurn = true;
        }
        else
        {
            _int iCTP = m_pModelCom->Get_Current_Track_Position();
            if (iCTP % 2 == 0 && !m_bFireLand)
            {
                m_bFireLand = true;
                _vector vPos = m_pTransformCom->Get_Position();
                CTransform::TRANSFORM_DESC            TransformDesc{};
                TransformDesc.fSpeedPerSec = 1.f;
                TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
                XMStoreFloat4(&TransformDesc.vPosition, vPos);

                GAMEINSTANCE->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("CBlaze_Fire"), &TransformDesc);
            }
            else
                m_bFireLand = false;
        }
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_060")
        && m_pModelCom->Get_Finished())
    {
        m_strNext_Pattern = "em0740_atk_070";
        m_bFirePunch = true;
        Pattern_Wait();
        m_bTurn = true;
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_070")
        && m_pModelCom->Get_Current_Track_Position() > 45.f)
    {
        if(m_pModelCom->Get_Finished())
        {
            m_bFireLand = false;
            Pattern_Wait();
            m_bTurn = true;
        }
        else if(!m_bFireLand)
        {
            m_bFireLand = true;
            CBlaze_FireStarter::Blaze_FireStarter eFireStarterDesc = {};
            eFireStarterDesc.eFireStarterType = CBlaze_FireStarter::FIRESTARTER_LIST::FIRESTARTER_STRAIGHT;
            eFireStarterDesc.matWorld = m_pTransformCom->Get_WorldMatrix();

            GAMEINSTANCE->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("CBlaze_FireStarter"), &eFireStarterDesc);
        }
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_040")
        && m_pModelCom->Get_Finished())
    {
        m_isAirState = false;
        m_eState = BlAZE_STATE::DOWN;
        GravityOn();
        m_pModelCom->Set_AnimIndex("em0740_dmg-down_000", true);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_reaction_000")
        && m_pModelCom->Get_Finished())
    {
        m_bTurn = true;
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-high_500")
        && m_pModelCom->Get_Finished())
    {
        m_eState = BlAZE_STATE::DOWN;
        GravityOn();
        m_pModelCom->Set_AnimIndex("em0740_dmg-down_100", true);
    }
    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_250"))
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0740_dmg-down_000", true);
    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_210"))
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0740_dmg-down_100", true);

    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_010") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_110"))
        && m_pModelCom->Get_Finished())
    {
        m_bTurn = true;
        m_eState = BlAZE_STATE::IDLE;
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_000")
        && m_pModelCom->Get_Finished())
    {
        m_bAirLunched = true;
        m_isAirState = false;
        GravityOn();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_idle_000")
        && m_pModelCom->Get_Finished())
    {
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_010")
        && m_pModelCom->Get_Current_Track_Position() > 59.f)
    {
      
        PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));

        shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());

        _matrix matBone = XMLoadFloat4x4(m_pChai_Attach);
        matBone.r[0] = XMVector3Normalize(matBone.r[0]);
        matBone.r[1] = XMVector3Normalize(matBone.r[1]);
        matBone.r[2] = XMVector3Normalize(matBone.r[2]);
        _matrix vMat = matBone * m_pTransformCom->Get_WorldMatrix_XMMat();

        //_vector vVec = vMat.r[3];

        pPlayer->Set_Player_Position(vMat.r[3]);

        pPlayer->Get_Transform()->LookAt_Horizontal(pPlayer->Get_Transform()->Get_Position() - m_pTransformCom->Get_NormLook());

        m_pModelCom->Set_AnimIndex("em0740_atk-rp-cp_052", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_052")
        && m_pModelCom->Get_Current_Track_Position() > 59.f)
    {

        if(m_pModelCom->Get_Finished())
        {

            m_bCCTOn = false;
            m_bRPMode = false;
            m_pModelCom->Set_AnimIndex("em0740_atk-rp-cp_002", false);
            PLAYERMANAGER->Callback_Event("Rhythm_Parry_End");
        }
        else if (m_pModelCom->Get_Current_Track_Position() > 140.f)
        {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
        }
        else if(!m_bRPUI)
        {
            BEATMANAGER->Reset();
            m_bRPUI = true;
            m_iRP_Success_Count = 0;
            Set_Ui(0);
        }
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_002")
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0740_atk-rp-cp_003", false);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_003")
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0740_atk-rp-cp_006", false);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_006")
        && m_pModelCom->Get_Finished())
    {
        //PLAYERMANAGER->Get_Player()->Get_Player_Camera()->End_Following_Bone();
        _float4x4 matResult{};
        XMStoreFloat4x4(&matResult, XMMatrixIdentity());
        PlayEffect("Enemy_Boom", matResult, 1.f, true);

        CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
        m_pBarrier_UI->Dead();
        Dead();
        Make_PickUp_Object(5, CPickUpItem::PICKUP::GEAR);
        Make_PickUp_Object(5, CPickUpItem::PICKUP::HP);

    }

    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_200") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_210")||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_220"))
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0740_dmg-air_050", false);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_050")
        && m_pModelCom->Get_Finished())
    {
        GravityOn();
        m_pModelCom->Set_AnimIndex("em0740_dmg-air_050", true);
    }

    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-die_100"))
        && m_pModelCom->Get_Finished())
    {
        _float4x4 matResult{};
        XMStoreFloat4x4(&matResult, XMMatrixIdentity());
        PlayEffect("Enemy_Boom", matResult, 1.f, true);
        Dead();
        CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
    }
    else if (m_pModelCom->Get_Finished() && m_eState != BlAZE_STATE::DOWN)
    {
        if (m_bPlayerLook == false)
            m_bTurn = true;
        Pattern_Wait();
    }

}

void CMonster_Blaze::Animation(_float fTimeDelta)
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
    _float Seven_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.142f) * m_fTimeMultiflier;
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f) * m_fTimeMultiflier;
    _float Nine_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.111f) * m_fTimeMultiflier;

    _uint index = m_pModelCom->Get_CurrentAnimIndex();
    _float fTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();


    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_000"))
    {
        if (m_pModelCom->Get_Current_Track_Position() > 43.f && m_pModelCom->Get_Current_Track_Position() < 47.f)
            m_bHitOn = false;
        else if (m_pModelCom->Get_Current_Track_Position() > 53.f && m_pModelCom->Get_Current_Track_Position() < 56.f)
            m_bHitOn = false;
        else if (m_pModelCom->Get_Current_Track_Position() > 63.f && m_pModelCom->Get_Current_Track_Position() < 66.f)
            m_bHitOn = false;
        else
            m_bHitOn = true;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f, fmatResult);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_010"))
    {
        if (m_pModelCom->Get_Current_Track_Position() > 88.f && m_pModelCom->Get_Current_Track_Position() < 93.f)
            m_bHitOn = false;
        else
            m_bHitOn = true;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Eight_Beat, 0.1f, fmatResult);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_060"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Seven_Beat, 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk_070"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_other_000"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_other_001"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_000"))
    {
        m_bPlayerLook = false;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    }

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-die_000"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Three_Beat, 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-die_010"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-die_100"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-high_500"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_010"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f, fmatResult);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_006"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_003"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);


    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_052"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.071f) * m_fTimeMultiflier, 0.1f, fmatResult);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_reaction_000"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Seven_Beat, 0.1f, fmatResult);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_100"))
    {
        m_bPlayerLook = false;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    }
    else
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);

    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);

    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);


     m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
}

void CMonster_Blaze::Move(_float fTimeDelta)
{
}

void CMonster_Blaze::Attack(_float fTimeDelta)
{
}

void CMonster_Blaze::LookPlayerPos(_float fTimeDelta)
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

void CMonster_Blaze::DownState(_float fTimeDelta)
{
    _float fBeat = BEATMANAGER->Get_Beat();
    _float fBeatRate = BEATMANAGER->Get_BeatRate();
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_000") || 
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_100") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_010") || 
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_110") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_210") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_250")
        )
        m_fDownTimer += fTimeDelta;

    if (m_fDownTimer > fBeat * 5.f && fBeatRate > 0.92f)
    {
        m_bTurn = true;
        m_fDownTimer = 0.f;
        m_eState = BlAZE_STATE::IDLE;
        if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_000"))
            m_pModelCom->Set_AnimIndexNonCancle("em0740_dmg-down_010", false);
        else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_100"))
            m_pModelCom->Set_AnimIndexNonCancle("em0740_dmg-down_110", false);

        GravityOn();
    }
}

void CMonster_Blaze::Pattern_Change(_float fTimeDelta)
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

        if (m_strCurrent_Pattern == "em0740_atk_000")
        {
            m_bPlayerLook = false;
            m_strNext_Pattern = "em0740_atk_010";
        }
        else if (m_strCurrent_Pattern == "em0740_atk_010")
        {
            if(!m_bFirePunch)
                m_strNext_Pattern = "em0740_atk_060";
            else
                m_strNext_Pattern = "em0740_atk_070";
            m_bPlayerLook = false;
        }
        else if (m_strCurrent_Pattern == "em0740_atk_060" && m_bFirePunch)
        {
            m_bPlayerLook = false;
            m_strNext_Pattern = "em0740_atk_070";
        }
        else if (m_strCurrent_Pattern == "em0740_atk_060" && !m_bFirePunch)
        {
            m_bPlayerLook = false;
            m_strNext_Pattern = "em0740_atk_000";
        }
        else if (m_strCurrent_Pattern == "em0740_atk_070")
        {
            m_bPlayerLook = false;
            m_strNext_Pattern = "em0740_atk_000";
        }
    }
}

void CMonster_Blaze::Play_Pattern(_float fTimeDelta)
{
}

void CMonster_Blaze::Pattern_Wait()
{
    _float distance = Calculate_Distance_To_Player();
    m_bPattern_Timer_On = true;



    if (distance < 3.f)
    {
        _int iRandNum = GAMEINSTANCE->Random_Int(1, 2);
        if (iRandNum == 1)
            m_pModelCom->Set_AnimIndex("em0740_other_000", false);
        else
            m_pModelCom->Set_AnimIndex("em0740_other_001", false);
    }
    else if (distance < 7.f)
        m_pModelCom->Set_AnimIndex("em0740_mvnt-walk_000", false);
    else if (distance < 10.f)
    {
        _int iRandNum = GAMEINSTANCE->Random_Int(1, 2);
        if (iRandNum == 1)
            m_pModelCom->Set_AnimIndex("em0740_mvnt-escape_002", false);
        else
            m_pModelCom->Set_AnimIndex("em0740_mvnt-escape_003", false);
    }
    else
        m_pModelCom->Set_AnimIndex("em0740_mvnt-escape_000", false);

    /* if (distance > 10.f)
         m_pModelCom->Set_AnimIndex("em0740_mvnt-walk-st_020", false);
     else
         m_pModelCom->Set_AnimIndex("em0740_mvnt-walk-st_010", false);*/

}

_float CMonster_Blaze::Calculate_Distance_To_Player()
{
    _float resDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position()));
    return resDistance;
}

void CMonster_Blaze::GravityOn()
{
    m_pCCT->Enable_Gravity();
}

void CMonster_Blaze::GravityOff()
{
    m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    m_pCCT->Disable_Gravity();
}

void CMonster_Blaze::AirStateON()
{
    m_isAirState = true;
    GravityOff();
}

void CMonster_Blaze::AirStateOFF()
{
    m_isAirState = false;
    GravityOn();
}

void CMonster_Blaze::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack || pCaller == m_pCollider_Attack_Left)
    {
        DamageInfo damageInfo{};

        if(m_bFirePunch)
        {
            damageInfo.fAmountDamage = 20.f;
            damageInfo.eAttackType = EAttackType::Fire;
        }
        else
        {
            damageInfo.fAmountDamage = 10.f;
            damageInfo.eAttackType = EAttackType::Base;
        }
        damageInfo.pObjectOwner = shared_from_this();
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
    else if (pCaller == m_pCollider_RP)
    {
        DamageInfo damageInfo{};
        damageInfo.fAmountDamage = 0.f;
        damageInfo.eAttackType = EAttackType::Special;
        damageInfo.pObjectOwner = shared_from_this();
        damageInfo.strAttackName = "Rhythm_Parry";
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

void CMonster_Blaze::TakeDamage(const DamageInfo& damageInfo)
{
    if (IsDeadState() || m_bRPMode)
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
        if (m_fCurHp <= 60.f)
        {
            GravityOn();
           
            m_bRPMode = true;
            return;
        }
        else
        {
            _float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
            _vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
            _float4x4 matResult{};
            XMStoreFloat4x4(&matResult, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            m_fCurHp = fTargetHp;
        }
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
        if (m_fBarrier <= 0.f)
            GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Blaze_Shield_Break",false);

        if (damageInfo.eAttackType == EAttackType::Parry)
            m_fBarrier -= 10.f;
        if (damageInfo.eAttackType == EAttackType::Korsica && m_bFirePunch == true)
        {
            m_bFirePunch = false;
            m_fFirePunch_Timer = 0.f;
            m_strNext_Pattern = "em0740_atk_000";
            m_pModelCom->Set_AnimIndex_NoInterpole("em0740_reaction_000", false);
        }
        return;
    }


    if (m_fCurHp <= 0.f)
    {
        m_bPlayerLook = false;
        GravityOn();
        m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
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
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-air_200", false);
                    break;
                }
                case EAttackDirection::LEFT:
                {
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-air_210", false);
                    break;
                }
                case EAttackDirection::RIGHT:
                {
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-air_220", false);
                    break;
                }
                }
            }
            }
        }
    }
    else // On the ground
    {
        if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_100") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_210") )
        {
            m_pModelCom->Set_AnimIndex("em0740_dmg-down_210", false);
            return;
        }
        else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_000") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em0740_dmg-down_250"))
        {
            m_pModelCom->Set_AnimIndex("em0740_dmg-down_250", false);
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
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-low_000", false);
                break;
            case EAttackDirection::LEFT:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-low_010", false);
                break;
            case EAttackDirection::RIGHT:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-low_020", false);
                break;
            case EAttackDirection::DOWN:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-low_030", false);
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
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-mid_000", false);
                break;
            case EAttackDirection::LEFT:
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-mid_010", false);
                break;
            case EAttackDirection::RIGHT:
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-mid_020", false);
                break;
            case EAttackDirection::DOWN:
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-mid_600", false);
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
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_010", false);
            else
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-high_500", false);

            break;
        }
        case EAttackType::Special:
        {
            PlayEffect("Big_HitEffect", matResult, 1.f, true);
            if (IsDeadState())
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_010", false);
            else
                m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-high_500", false);


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
            m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-mid_000", false);
            m_isAttackOn_Left = false;
            m_isAttackOn_Right = false;
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
            m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-air_000", false);

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
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-low_000", false);
                break;
            default:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-low_000", false);
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
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-high_500", false);
                break;
            default:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-high_500", false);
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
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                {
                    if(m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("em0740_reaction_000"))
                    {
                        if (m_bFirePunch)
                        {
                            m_bFirePunch = false;
                            m_strNext_Pattern = "em0740_atk_000";
                            m_pModelCom->Set_AnimIndex_NoInterpole("em0740_reaction_000", false);
                        }
                        else
                            m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-mid_000", false);
                    }
                }
                break;
            default:
                if (IsDeadState())
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-die_100", false);
                else
                    m_pModelCom->Set_AnimIndex_NoInterpole("em0740_dmg-mid_000", false);
                break;
            }

            break;
        }
        }
    }
}

void CMonster_Blaze::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(&m_matCurrentPos));
    CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier * m_fBeatSpeed, isBillBoard);
}

void CMonster_Blaze::PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard)
{
    CEffect_Manager::Get_Instance()->Call_Union_Effect(strEffect, &m_matCurrentPos, fmultiplier * m_fBeatSpeed, isBillBoard);
}

void CMonster_Blaze::PrepareCallbacks()
{
    m_pModelCom->SetAnimCallback("em0740_atk-rp-cp_010", 0, [&]() { m_isRPOn = true; });
    m_pModelCom->SetAnimCallback("em0740_atk-rp-cp_010", 1, [&]() { m_isRPOn = false; });
    m_pModelCom->SetLastAnimCallback("em0740_atk-rp-cp_010", [&]() { m_isRPOn = false; });

    m_pModelCom->SetAnimCallback("em0740_atk_000", 0, [&]() { m_isAttackOn_Left = true; });
    m_pModelCom->SetAnimCallback("em0740_atk_000", 1, [&]() { m_isAttackOn_Left = false; m_isAttackOn_Right = true; });
    m_pModelCom->SetAnimCallback("em0740_atk_000", 2, [&]() { m_isAttackOn_Right = false; m_isAttackOn_Left = true; });
    m_pModelCom->SetAnimCallback("em0740_atk_000", 3, [&]() { m_isAttackOn_Right = false; m_isAttackOn_Left = false; });
    m_pModelCom->SetLastAnimCallback("em0740_atk_000", [&]() { m_isAttackOn_Right = false; m_isAttackOn_Left = false; });


    m_pModelCom->SetAnimCallback("em0740_atk_010", 1, [&]() { m_isAttackOn_Right = true; });
    m_pModelCom->SetAnimCallback("em0740_atk_010", 2, [&]() { m_isAttackOn_Right = false;});
    m_pModelCom->SetLastAnimCallback("em0740_atk_010", [&]() { m_isAttackOn_Right = false; m_isAttackOn_Left = false; });

    m_pModelCom->SetAnimCallback("em0740_atk_060", 0, [&]() { m_isAttackOn_Right = true;  Fire_Punch_Effect(); });
    m_pModelCom->SetAnimCallback("em0740_atk_060", 1, [&]() { m_isAttackOn_Right = false; });
    m_pModelCom->SetLastAnimCallback("em0740_atk_060", [&]() { m_isAttackOn_Right = false; m_isAttackOn_Left = false; });
}

void CMonster_Blaze::SelectBeat()
{
    _float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

    m_fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
}

void CMonster_Blaze::Set_Position(_matrix mMat)
{
    m_pCCT->Set_Position(mMat);
}

void CMonster_Blaze::Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType)
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

void CMonster_Blaze::Collision_Down(_float fTimeDelta)
{
    PxControllerState pCS;
    m_pCCT->Get_Controller()->getState(pCS);
    if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
        if (
            (m_bAirLunched && currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_000")) ||
            currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_200") ||
            currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_dmg-air_050")
            )
        {
            m_pModelCom->Set_AnimIndexNonCancle("em0740_dmg-air_040", false);
        }
    }
}

void CMonster_Blaze::Set_Ui(_int iSuccessCount)
{
    m_AttackBeatVec.clear();
    m_AttackParryBeatVec.clear();
    m_iAttackReadyStartBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

    if (iSuccessCount == 0)
    {
        m_AttackBeatVec = { 0, 0.5f, 1.f, 1.f ,0.5f };
        m_AttackParryBeatVec = { true, true, true, true, true };
    }
    Create_AttackUi((_uint)m_AttackBeatVec.size(), m_AttackBeatVec, m_AttackParryBeatVec);

    for (size_t i = 1; i < m_AttackBeatVec.size(); ++i)
    {
        m_AttackBeatVec[i] += m_AttackBeatVec[i - 1];
    }
    shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();
    if (iSuccessCount == 0)
        pChai->Start_AttackSequence(6.f, m_iAttackReadyStartBeat, m_AttackBeatVec, m_AttackParryBeatVec);

}

void CMonster_Blaze::Create_AttackUi(_uint iParryCount, vector<_float>& fBeatGap, vector<_bool>& bParry, _float2 fDistance)
{
    CUi_EnemySpecialAttack::Desc Desc;
    Desc.iParryCount = iParryCount;
    Desc.bParry = bParry;
    Desc.fBeatGap = fBeatGap;
    Desc.fDistance = fDistance;
    Desc.TargetTransform = m_pTransformCom;
    Desc.RealAttackGapBeat = 6.f;
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_EnemySpecialAttack::ObjID,
        m_iLevelIndex, L"CUi", CUi_EnemySpecialAttack::ObjID, &Desc);
}

void CMonster_Blaze::Judge_Attack(_float fTimeDelta)
{
    _int CurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    _float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float BeatPercent = Timer / Beat;
    _float JudgeBeat = 0;

    shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();
    _int currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0740_atk-rp-cp_052"))
    {
        JudgeBeat = CurBeat - (m_iAttackReadyStartBeat + 6.f);
    }
    //else if(m_iParry_Success_Count == 1 && currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_040"))
    //{
    //    JudgeBeat = CurBeat - (m_iAttackReadyStartBeat + 8.f);
    //}

    if (m_AttackBeatVec.size() > 0)
    {
        if (m_AttackBeatVec[0] <= JudgeBeat + BeatPercent - 0.2f)
        {
            if (m_AttackParryBeatVec[0])
            {
                if (pChai->Get_ReactTimer() < 0.001f)
                {
                    pChai->Set_DamagedAnim();
                }
                else
                {
                    pChai->Set_ParryAnim();
                    m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Parry_GT");
                    m_iRP_Success_Count++;
                    if (m_iRP_Success_Count > 3)
                    {
                        m_iParry_Success_Count++;
                        m_iRP_Success_Count = 0;
                    }
                }

                pChai->Get_Player_Camera()->Shake(0.1f, 0.1f);
            }
            else
            {
                if (pChai->Get_ReactTimer() < 0.001f)
                    pChai->Set_DamagedAnim();
                else
                    pChai->Set_EvadeAnim();

            }

            pChai->Set_ReactTimer(0.f);
            m_AttackBeatVec.erase(m_AttackBeatVec.begin());
            m_AttackParryBeatVec.erase(m_AttackParryBeatVec.begin());
        }
    }
}

void CMonster_Blaze::Fire_Punch_Effect()
{
    m_fFirePunch_Timer = 0.5f;
    CEffect_Manager::Get_Instance()->Call_Union_Effect("FirePunch", &m_pL_Attach, 1.f, false);
    CEffect_Manager::Get_Instance()->Call_Union_Effect("FirePunch", &m_pR_Attach, 1.f, false);
}
