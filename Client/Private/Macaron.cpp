#include "Macaron.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "CharacterController.h"

#include "Effect_Manager.h"
#include "Player_Camera.h"

#include "Cine_Manager.h"

#include "CombatManager.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "CUi_Mgr.h"
#include "CUi_PlayerBar.h"
wstring CMacaron::ObjID = TEXT("CMacaron");

CMacaron::CMacaron(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMacaron::CMacaron(const CMacaron& rhs)
    : CGameObject(rhs)
{
}

CMacaron::~CMacaron()
{
    Free();
}

HRESULT CMacaron::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Macaron"), TEXT("../Bin/Resources/Models/Player/Macaron/Macaron.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );


    return S_OK;
}

HRESULT CMacaron::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
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
    m_pTransformCom->Set_Position(XMVectorSet(5.7f, 0.4f, 84.f, 1.f));
    m_pModelCom->Set_AnimIndex("ch2000_atk_100", false);

    return S_OK;
}

void CMacaron::PriorityTick(_float fTimeDelta)
{
}

void CMacaron::Tick(_float fTimeDelta)
{
    if (!m_bRender)
        return;
    Animation_End();
    Animation(fTimeDelta);

    if (m_pModelCom->Get_StringAnimIndex("ch2000_atk_102") == m_pModelCom->Get_CurrentAnimIndex())
    {
        Turn(fTimeDelta);
    }
    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();
}

void CMacaron::LateTick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
        return;

    if (m_bRender)
    {
        m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
        Calc_ViewDepth();
    }
}

HRESULT CMacaron::Render()
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
    _float fViewDepth = Get_ViewDepth();
    _float fOutlineWidth = 0.8f;
    if (fViewDepth <= 10.f)
    {
        fOutlineWidth = 0.4f;
    }
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        {
            //if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            //    return E_FAIL;
            //m_pShaderCom->Bind_Float("g_OutlineWidth", 0.01f);
            //m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);


            //m_pModelCom->Render((_uint)i);
            continue;
        }
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
        //    return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //    return E_FAIL;
        _bool bNormal = true;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        {
            bNormal = false;
        }

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        if (i == 0)        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::NormalMap);
        else               m_pShaderCom->Begin(0);

        //if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        //   return E_FAIL;

        m_pModelCom->Render((_uint)i);

        if (i == 0)
            continue;
        m_pShaderCom->Bind_Float("g_OutlineWidth", fOutlineWidth);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        m_pModelCom->Render((_uint)i);

    }

    return S_OK;
}

shared_ptr<CGameObject> CMacaron::Auto_LockOn_Target(list<shared_ptr<CGameObject>> vecTargetList)
{
    auto playerTransform = PLAYERMANAGER->Get_Player()->Get_Transform();
    _float fDistance = FLT_MAX;
    shared_ptr<CGameObject> lockOnTarget = nullptr;
    for (auto& iter : vecTargetList)
    {
        auto targetTransform = iter->Get_Transform();

        _float length = XMVectorGetX(XMVector3Length(targetTransform->Get_Position() - playerTransform->Get_Position()));

        if (fDistance > length)
        {
            fDistance = length;
            lockOnTarget = iter;
        }
    }
    return lockOnTarget;
}

HRESULT CMacaron::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Macaron"), TEXT("Com_Model")));



    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

shared_ptr<CMacaron> CMacaron::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMacaron
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMacaron(pDevice, pContext)
        {}
    };


    shared_ptr<CMacaron> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMacaron"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMacaron::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMacaron
    {
        MakeSharedEnabler(const CMacaron& rhs) : CMacaron(rhs)
        {}
    };


    shared_ptr<CMacaron> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMacaron"));
        assert(false);
    }

    return pInstance;
}

void CMacaron::Free()
{
}

void CMacaron::Assist_Attack(_vector vPos)
{
    m_pLock_On_Target = Auto_LockOn_Target(CCombat_Manager::Get_Instance()->Get_Enemy_List());
    if (!m_bAttacking && m_pLock_On_Target)
    {
        m_bRender = true;
        m_pTransformCom->Set_Position(vPos);
        m_pLock_On_Target = Auto_LockOn_Target(CCombat_Manager::Get_Instance()->Get_Enemy_List());
        
        m_pTransformCom->LookAt_Horizontal(m_pLock_On_Target->Get_Transform()->Get_Position());
        m_pModelCom->Set_AnimIndex("ch2000_atk_100", false);
        m_bAttacking = true;
        
    }
}

void CMacaron::Lift_Attack(_vector vPos, _fvector vLookAt)
{
    if (!m_bAttacking)
    {
        m_bRender = true;
        m_pTransformCom->Set_Position(vPos);
        m_pTransformCom->LookAt_Horizontal(vLookAt);

        m_pModelCom->Set_AnimIndex("ch2000_event_000", false);
        m_bAttacking = true;
    }
}

void CMacaron::Play_Animation(string strAnimName, _bool bLoop)
{
    m_pModelCom->Set_AnimIndex(strAnimName, bLoop);
    m_bRender = true;
}

void CMacaron::Animation_End()
{
    if (m_pModelCom->Get_StringAnimIndex("ch2000_atk_100") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch2000_atk_101", false);

    else if (m_pModelCom->Get_StringAnimIndex("ch2000_atk_101") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch2000_atk_102", false);

    else if (m_pModelCom->Get_StringAnimIndex("ch2000_atk_102") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        static_pointer_cast<CUi_PlayerBar>(CUi_Mgr::Get_Instance()->Find_UI(CUi_PlayerBar::ObjID))->Partner_Respawn(CUi_PlayerBar::PARTNER_MACARON);
        PLAYERMANAGER->Set_Assist_Is_CoolTime(CPlayer_Manager::Assist::Macaron, true);
        m_bAttacking = false;
        m_bRender = false;
    }

    if (m_pModelCom->Get_StringAnimIndex("ch2000_event_000") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_iHit_Count = 4;
        m_pModelCom->Set_AnimIndex("ch2000_event_001", false);
    }
    else if (m_pModelCom->Get_StringAnimIndex("ch2000_event_001") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_iHit_Count = 6;
        m_pModelCom->Set_AnimIndex("ch2000_event_002", false);
    }

    else if (m_pModelCom->Get_StringAnimIndex("ch2000_event_002") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_pModelCom->Set_AnimIndex("ch2000_atk_102", false);
        _float fDuration = m_pModelCom->Get_Duration();
        m_pModelCom->Set_CurrentTrackPosition(fDuration-1.f);
        m_iHit_Count = 0;
        m_bAttacking = false;
        m_bRender = false;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1110_90_000_ch2000_00") && m_pModelCom->Get_Finished())
    {
        m_bRender = false;
    }
}

void CMacaron::Animation(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    /* 반박자에 끝나는 모션 ex) 단타 공격모션 */
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f;
    _float Three_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.3333f);
    _float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f;
    _float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f);
    _float Six_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f);
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f);

    _uint index = m_pModelCom->Get_CurrentAnimIndex();

    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_atk_100"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f, fmatResult);
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_atk_101"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_atk_102"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);

    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_000"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_001"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_002"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1110_90_000_ch2000_00"))
    {
        _float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

        _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

        _float fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
        //100 ~ 300
        _float fTP = m_pModelCom->Get_Current_Track_Position();
        if (fTP >= 100.f && fTP < 150.f)
        {
            _vector vPos = m_pTransformCom->Get_Position();
            auto pChai = PLAYERMANAGER->Get_Player();
            _vector vChaiPos = pChai->Get_Transform()->Get_Position();
            _vector vDir = XMVectorSetY(XMVector3Normalize(XMVectorSetY(vPos - vChaiPos, 0.f)), 0.f);

            pChai->Get_Player_Camera()->Camera_Fixed(m_pTransformCom, vDir, 12.f);
        }
        else if (fTP >= 300.f)
        {
            auto pChai = PLAYERMANAGER->Get_Player();
            pChai->Get_Player_Camera()->End_Camera_Fixed();
        }
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fBeatSpeed, 0.1f, fmatResult);
    }
    else
    {
        _float fTotalBeat = m_pModelCom->Get_Duration() / 20.f;

        _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

        _float fBeatSpeed = fPlayTime / fBeat / fTotalBeat;

        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fBeatSpeed, 0.1f, fmatResult);
    }

    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_000")
        && m_pModelCom->Get_Current_Track_Position() >= 45.f)
        m_iHit_Count = 4;
    else  if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_000")
        && m_pModelCom->Get_Current_Track_Position() >= 42.f)
        m_iHit_Count = 3;
    else  if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_000")
        && m_pModelCom->Get_Current_Track_Position() >= 34.f)
        m_iHit_Count = 2;
    else  if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_000")
        && m_pModelCom->Get_Current_Track_Position() >= 30.f)
        m_iHit_Count = 1;

    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_001")
        && m_pModelCom->Get_Current_Track_Position() >= 10.f)
        m_iHit_Count = 6;
    else  if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_001")
        && m_pModelCom->Get_Current_Track_Position() >= 25.f)
        m_iHit_Count = 5;

    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_002")
        && m_pModelCom->Get_Current_Track_Position() >= 30.f)
        m_iHit_Count = 0;
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch2000_event_002")
        && m_pModelCom->Get_Current_Track_Position() >= 10.f)
        m_iHit_Count = 7;

    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pTransformCom->Set_Position(vCurPos);
}

void CMacaron::Move(_float fTimeDelta)
{
}

void CMacaron::Attack(_float fTimeDelta)
{
}

void CMacaron::Turn(_float fTimeDelta)
{
    _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
    _vector camLookVec = XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0);

    _vector camRightvec = {};
    shared_ptr <CTransform> camTrasnform = static_pointer_cast<CTransform> (GAMEINSTANCE->Get_MainCamera()->Get_Transform());
    camRightvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_RIGHT), 0);



    _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
    m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

    _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
    _float turnFloat = XMVectorGetY(LookCross);
    if (turnFloat < 0)
        m_bTrunRight = false;
    else
        m_bTrunRight = true;

    if (m_fLookDiffDegree > 1.f)
    {
        m_fTurnTimer = 0.1f;
        m_bTurning = true;
    }
    else
    {
        m_fTurnTimer = 0.f;
        m_bTurning = false;
    }

    if (m_fLookDiffDegree != 0.f && m_fTurnTimer > 0.f && m_bTurning == true)
    {
        m_fTurnTimer -= fTimeDelta;
        _vector turnDir = {};
        if (m_bTrunRight)
            turnDir = { 0.f, 1.f, 0.f, 0.f };
        else
            turnDir = { 0.f, -1.f, 0.f, 0.f };
        m_pTransformCom->Turn(turnDir, XMConvertToRadians(m_fLookDiffDegree), fTimeDelta * 10.f);

    }
    else if (m_fTurnTimer <= 0.f)
    {
        m_bTurning = false;
        m_fTurnTimer = 0.1f;
        m_fLookDiffDegree = 0.f;
    }
}
