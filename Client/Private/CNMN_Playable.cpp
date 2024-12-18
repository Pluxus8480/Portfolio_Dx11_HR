#include "CNMN_Playable.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "CharacterController.h"

#include "Effect_Manager.h"
#include "Player_Camera.h"

#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "Chai.h"

#include "Flare_Effect.h"
#include "808_Flare_Effect.h"
#include "JustMark_Effect.h"
#include "808_Controller.h"
#include "808_Companion.h"
#include "CUi_Mgr.h"
#include "CUi_StoreSystem.h"
#include "GameManager.h"

#include "Collider.h"
#include "Bounding_Sphere.h"
#include "Cine_Manager.h"
#include "CUi_TalkSystem.h"

wstring CCNMN_Playable::ObjID = TEXT("CCNMN_Playable");

CCNMN_Playable::CCNMN_Playable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CCNMN_Playable::CCNMN_Playable(const CCNMN_Playable& rhs)
    : CGameObject(rhs)
{
}

CCNMN_Playable::~CCNMN_Playable()
{
    Free();
}

HRESULT CCNMN_Playable::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CNMN_Playable"), TEXT("../Bin/Resources/Models/Player/CNMN/CNMN.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale11_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale11_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale11_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale11_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale11_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale11_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale12_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale12_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale12_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale12_2.ogg")));

    return S_OK;
}

HRESULT CCNMN_Playable::Initialize(void* pArg)
{
    //CTransform::TRANSFORM_DESC         TransformDesc{};
    //TransformDesc.fSpeedPerSec = 4.f;
    //TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

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

    CCine_Manager::Get_Instance()->AddLastCallback("EV1180",
        [&]() {
            Take_Control();
            _float4x4 MatOut;
            CCine_Manager::Get_Instance()->GetMatTrans("ch3000", MatOut);

            auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&MatOut).r[3], 1.f);
            m_pTransformCom->Set_Position(vNewPos);

            vNewPos -= XMVectorSetW(XMLoadFloat4x4(&MatOut).r[2], 0.f);
            m_pTransformCom->LookAt_Horizontal(vNewPos);
            m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        }
    );

    CCine_Manager::Get_Instance()->AddLastCallback("CS1125", [&]() {
        m_bSemiFinalCS = true;
        m_bFinalCS = false;
       
        });
    CCine_Manager::Get_Instance()->AddLastCallback("EV1190", [&]() {
        m_bSemiFinalCS = false;
        m_bFinalCS = true;
        
        });
    CCine_Manager::Get_Instance()->AddLastCallback("CS1130", [&]() {
        End_Control();
        m_pGameInstance.lock()->Change_Level_Delayed(LEVEL_KALEBOSS);
        });

    m_pModelCom->Set_AnimIndex("ch3000_idle_000", true);
    //_float4x4 xMat = {};
    //m_pModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, xMat);

    //auto pGameInstance = m_pGameInstance.lock();

    GAMEINSTANCE->Add_Unique_Sound_Channel("STKale");

    return S_OK;
}

void CCNMN_Playable::PriorityTick(_float fTimeDelta)
{
    if (!m_bRender)
        return;
    if (!m_bEvent && m_bTakeControl)
        Move(fTimeDelta);
    _uint iCurrBeat = BEATMANAGER->Get_BeatCount();
    if (m_iPrevBeat != iCurrBeat)
    {
        m_iPrevBeat = iCurrBeat;
        if (m_bEvent)
        {

        }
    }
}

void CCNMN_Playable::Tick(_float fTimeDelta)
{
    if (m_bSemiFinalCS && !m_bFinalCS)
    {
        m_bSemiFinalCS = false;
        CCine_Manager::Get_Instance()->StartCine("EV1190");
    }
    if (m_bFinalCS && !m_bSemiFinalCS)
    {
        m_bFinalCS = false;
        CCine_Manager::Get_Instance()->StartCine("CS1130");
    }
    if (!m_bRender)
        return;


    Animation_End();
    Animation(fTimeDelta);




    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

    m_pCCT->Update(fTimeDelta);
    auto matrixSpine = XMLoadFloat4x4(m_pBone_Spine);
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
    _matrix World = matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pCollider_Hit->Update(World);
}

void CCNMN_Playable::LateTick(_float fTimeDelta)
{
    if (!m_bRender)
        return;
    auto pGI = m_pGameInstance.lock();
    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    if (!m_bTakeControl)
        return;
    if (!m_bEvent)
        pGI->Add_Collider(COLLIDER_GROUP::COLLISION_PLAYER, m_pCollider_Hit);
}

HRESULT CCNMN_Playable::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
    //   return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;
    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        {

            continue;
        }

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;
        if (i == 3)
        {
            _float4 vColor{ 0.f,1.f,1.f, 1.f };

            m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::EmissionColor);
        }
        else
            m_pShaderCom->Begin(0);


        m_pModelCom->Render((_uint)i);
        if (i == 0)
            continue;
        m_pShaderCom->Bind_Float("g_OutlineWidth", 1.0f);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        m_pModelCom->Draw((_uint)i);
    }

#ifdef _DEBUG
    m_pCollider_Hit->Render();
#endif

    return S_OK;
}

void CCNMN_Playable::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{

}

void CCNMN_Playable::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (m_bEvent)
        return;
    if (pOther->Get_ColliderTag() == TEXT("Giant_Arm"))
    {
        m_pModelCom->Set_AnimIndex("ch3000_action_000", false);
        m_bEvent = true;

        if (!m_bTalkCol)
        {
            m_bTalkCol = true;
            m_isTalkOver = false;

            auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
            if (pTalkSystem == nullptr)
            {
                CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
                TalkDesc.endCallback = [&]() {
                    m_isTalkOver = true;
                    };

                pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_KALESTAGE, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
            }

            auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
            if (PRequiredTalkSystem != nullptr)
            {
                PRequiredTalkSystem->LoadTalkFile(L"STKale Talk_File_12.csv");

                CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
            }
        }
    }
}

void CCNMN_Playable::TakeDamage(const DamageInfo& damageInfo)
{
}

void CCNMN_Playable::Set_Position(const _float4& vPosition)
{
    m_pCCT->Set_FootPosition(XMMatrixTranslationFromVector(XMLoadFloat4(&vPosition)));
    m_pTransformCom->Set_Position(XMLoadFloat4(&vPosition));
}

void CCNMN_Playable::Take_Control()
{
    auto pPM = PLAYERMANAGER;
    auto pPlayer = pPM->Get_Player();
    pPlayer->Dead();
    pPlayer->Get_Player_Camera()->Set_Player(shared_from_this());
    m_pTransformCom->Set_WorldMatrix(pPlayer->Get_Transform()->Get_WorldMatrix_XMMat());
    m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
    m_bTakeControl = true;
    m_bRender = true;

    if (!m_bTalk)
    {
        m_bTalk = true;
        m_isTalkOver = false;

        auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
        if (pTalkSystem == nullptr)
        {
            CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
            TalkDesc.endCallback = [&]() {
                m_isTalkOver = true;
                };

            pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_KALESTAGE, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
        }

        auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
        if (PRequiredTalkSystem != nullptr)
        {
            PRequiredTalkSystem->LoadTalkFile(L"STKale Talk_File_11.csv");

            CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
        }
    }
}

void CCNMN_Playable::End_Control()
{
    auto pPM = PLAYERMANAGER;
    auto pPlayer = pPM->Get_Player();
    pPlayer->Set_Dead(false);
    auto pGI = m_pGameInstance.lock();
    pGI->Push_Object(LEVEL_STATIC, TEXT("Layer_Player"), pPlayer);
    pPlayer->Get_Player_Camera()->Set_Player(pPlayer);
    m_bTakeControl = false;
    m_bRender = false;
    m_bEvent = false;
    //Dead();
}

HRESULT CCNMN_Playable::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CNMN_Playable"), TEXT("Com_Model")));

    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 0.6f;
    cctDesc.radius = 0.3f;
    cctDesc.stepOffset = 0.1f;
    cctDesc.position = PxExtendedVec3(0.0f, 0.0f, 0.0f);
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));

    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Enable_Gravity();


    {
        CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
        sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        sphereDesc.fRadius = 1.0f;
        m_pCollider_Hit =
            static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
        m_pCollider_Hit->Set_Owner(shared_from_this());
        m_pCollider_Hit->Set_ColliderTag(L"CNMN_HIT");
    }

    m_pBone_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

shared_ptr<CCNMN_Playable> CCNMN_Playable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CCNMN_Playable
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCNMN_Playable(pDevice, pContext)
        {}
    };


    shared_ptr<CCNMN_Playable> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCNMN_Playable"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CCNMN_Playable::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CCNMN_Playable
    {
        MakeSharedEnabler(const CCNMN_Playable& rhs) : CCNMN_Playable(rhs)
        {}
    };


    shared_ptr<CCNMN_Playable> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCNMN_Playable"));
        assert(false);
    }

    return pInstance;
}

void CCNMN_Playable::Free()
{
}



void CCNMN_Playable::Animation_End()
{
}

void CCNMN_Playable::Animation(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    //_float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    //_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

    //_float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat;
    //_float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f;
    //_float Three_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.3333f);
    //_float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f;
    //_float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f);
    //_float Six_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f);
    //_float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f);

    _uint index = m_pModelCom->Get_CurrentAnimIndex();

    _float fTotalBeat = 0.f;
    if (m_bTakeControl)
        fTotalBeat = m_pModelCom->Get_Duration() / 15.f;
    else
        fTotalBeat = m_pModelCom->Get_Duration() / 20.f;

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();



    _float fBeatSpeed = fPlayTime / fBeat / fTotalBeat;

    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fBeatSpeed, 0.1f, fmatResult);

    if (m_pModelCom->Get_Finished())
    {
        if (m_bOnAir && !m_bEvent)
        {
            m_pModelCom->Set_AnimIndexNonCancle("ch3000_mvnt-jump_001", true);
        }
        string Tag = m_pModelCom->Get_AnimName();
        if (Tag == "ch3000_action_000")
        {
            m_pModelCom->Set_AnimIndex("ch3000_action_001", false);
        }
        else if (Tag == "ch3000_action_001")
        {
            m_pModelCom->Set_AnimIndex("ch3000_action_002", false);
        }
        else if (Tag == "ch3000_action_002")
        {
            if(m_bRender)
            {
                CCine_Manager::Get_Instance()->StartCine("CS1125");
                m_bRender = false;
            }
        }
    }

}

void CCNMN_Playable::Move(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    m_bInput = false;
    _vector vPos = m_pTransformCom->Get_Position();
    _vector vCamLook = pGI->Get_CamLook_Vector();
    vCamLook.m128_f32[1] = 0.f;
    vCamLook = XMVector3Normalize((vCamLook));
    _vector vCamRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook);
    _bool bW = pGI->Get_KeyPressing(DIK_W);
    _bool bA = pGI->Get_KeyPressing(DIK_A);
    _bool bS = pGI->Get_KeyPressing(DIK_S);
    _bool bD = pGI->Get_KeyPressing(DIK_D);
    _vector vLook = XMVectorZero();
    if (bW)
    {
        m_bInput = true;
        m_pCCT->Add_Displacement(vCamLook * m_fSpeed);
        vLook += vCamLook;
    }
    if (bA)
    {
        m_bInput = true;
        m_pCCT->Add_Displacement(vCamRight * -m_fSpeed);
        vLook -= vCamRight;
    }
    if (bS)
    {
        m_bInput = true;
        m_pCCT->Add_Displacement(vCamLook * -m_fSpeed);
        vLook -= vCamLook;
    }
    if (bD)
    {
        m_bInput = true;
        m_pCCT->Add_Displacement(vCamRight * m_fSpeed);
        vLook += vCamRight;
    }
    if (XMVector3Length(vLook).m128_f32[0] >= 0.001f)
    {
        m_pTransformCom->LookAt_Horizontal(vPos + vLook);
    }


    if (!m_bInput && !m_bOnAir)
    {
        m_pModelCom->Set_AnimIndexNonCancle("ch3000_idle_000", true);
    }
    else if (m_bInput && !m_bOnAir)
    {
        m_pModelCom->Set_AnimIndexNonCancle("ch3000_mvnt-run_000", true);
    }
    if (m_bOnAir)
    {
        PxControllerState pCS;
        m_pCCT->Get_Controller()->getState(pCS);
        if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
        {
            m_bOnAir = false;
            m_iJumpCount = 1;
            m_pModelCom->Set_AnimIndex("ch3000_mvnt-jump_002", false);
        }
    }
    if (pGI->Get_KeyDown(DIK_SPACE) && m_iJumpCount > 0)
    {
        --m_iJumpCount;
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 10.f, 0.f, 0.f));
        m_pModelCom->Set_AnimIndex("ch3000_mvnt-jump_000", false);

        m_bOnAir = true;
    }

}