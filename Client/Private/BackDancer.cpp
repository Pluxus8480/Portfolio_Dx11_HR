#include "BackDancer.h"
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

#include "Weapon_Peppermint.h"
#include "CombatManager.h"
#include "CUi_Mgr.h"
#include "CUi_PlayerBar.h"
#include "Saber_Weapon.h"
#include "GameManager.h"
wstring CBackDancer::ObjID = TEXT("CBackDancer");

CBackDancer::CBackDancer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CBackDancer::CBackDancer(const CBackDancer& rhs)
    : CGameObject(rhs)
{
}

CBackDancer::~CBackDancer()
{
    Free();
}

HRESULT CBackDancer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_BackDancer"), TEXT("../Bin/Resources/Models/Monsters/Mimosa_BackDancer/BackDancer(120).fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_Owning_ObjectID.push_back(CSaber_Weapon::ObjID);

    return S_OK;
}

HRESULT CBackDancer::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    auto bInfo = static_cast<BackDancer_Info*>(pArg);
    if (bInfo->bSpotLight)
        TransformDesc.vPosition = bInfo->fInitPos;

    if (!bInfo->bSpotLight)
    {
        m_bDanceMode = true;
        TransformDesc.vPosition = bInfo->fInitPos;
        //m_pTransformCom->Set_WorldMatrix(bInfo->fInitMat);
        m_bDance_Ready = true;
        m_listDanceEvent = bInfo->listDanceEvent;
        m_list_Mimosa_Dance_Delay = bInfo->list_Mimosa_Dance_Delay;
    }

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (!bInfo->bSpotLight)
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + m_pTransformCom->Get_NormRight());

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

    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Enable_Gravity();
    m_pCCT->Set_Gravity(40.f);

    m_pModelCom->Set_AnimIndex("em0100_event-dance_100", true);
    if(bInfo->bSpotLight)
    {
        _float fBeat = BEATMANAGER->Get_Beat();
        _int iMotion = bInfo->iMotion;
        m_fSpotlight_Timer = 8.f * fBeat;
        m_SpotLight = EFFECTMANAGER->Call_Union_Effect_Return("SpotLight_Enemy", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
        if(iMotion == 0)
            m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_010", false);
        else if(iMotion == 1)
            m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_020", false);
        else if (iMotion == 2)
            m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_030", false);
    }

   
    _float4x4 xMat = {};
    m_pModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, xMat);
    return S_OK;
}

void CBackDancer::PriorityTick(_float fTimeDelta)
{
}

void CBackDancer::Tick(_float fTimeDelta)
{
    if (!m_bRender)
        return;
 


    //if (m_bDance_Ready)
    //{
    //    m_fDanceBattle_Next_Timer += fTimeDelta;
    //    if (m_fDanceBattle_Next_Timer > ((m_list_Mimosa_Dance_Delay.front() * 2.f) + 2.f) * BEATMANAGER->Get_Beat())
    //    {
    //        m_fDanceBattle_Next_Timer = 0;
    //        m_list_Mimosa_Dance_Delay.pop_front();
    //        Start_Dance();
    //    }
    //}

    if(!m_bDanceMode)
    {
        Animation_End();
        _float fBeat = BEATMANAGER->Get_Beat();
        if (m_fSpotlight_Timer > 0.f)
        {
            m_fSpotlight_Timer -= fTimeDelta;
            if (m_fSpotlight_Timer < 0.f)
            {
                m_pTransformCom->LookAt_Horizontal(PLAYERMANAGER->Get_PlayerPos());
                m_fSpotlight_Timer = 0.f;
                Animation(fTimeDelta);
            }
        }
        else
            Animation(fTimeDelta);
    }
    else
    {
        Dance_Event_Animation_End(fTimeDelta);
        Dance_Event_Animation(fTimeDelta);
    }


    for (auto& pPartObj : m_pWeapon)
        pPartObj->Tick(fTimeDelta);

    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

    m_pCCT->Update(fTimeDelta);
}

void CBackDancer::LateTick(_float fTimeDelta)
{
    if (m_bRender)
    {
        if(!m_bDanceMode)
        {
            for (auto& pPartObj : m_pWeapon)
                pPartObj->LateTick(fTimeDelta);
        }

        m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    }
}

HRESULT CBackDancer::Render()
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

    return S_OK;
}

void CBackDancer::Attack_Start()
{
    if (m_bAttacking)
        return;
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_011"))
        m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_012", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_021"))
        m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_022", false);
 
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_031"))
        m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_032", false);
    m_bAttacking = true;
}

void CBackDancer::Start_Dance()
{
    if (m_listDanceEvent.empty())
        Dead();
    else
    {
        list<pair<string, _float>>& dance_list = m_listDanceEvent.front();
        m_bDance_Rest = false;
        string DanceName = dance_list.front().first;
        string findStr = "em5100";
        string replaceStr = "em0100";
        DanceName.replace(DanceName.find(findStr), findStr.length(), replaceStr);
        m_pModelCom->Set_AnimIndex(DanceName, false);
        m_fDance_PlaySpeed = dance_list.front().second;
    }
}

void CBackDancer::Dance_Event_Animation(_float fTimeDelta)
{
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * m_fTimeMultiflier;
    _float Half_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 2.f * m_fTimeMultiflier;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f * m_fTimeMultiflier;

    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    //if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_event-dance_100"))
    //    m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    //else

    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0100_event-dance_025"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Half_Beat, 0.1f, fmatResult);
    else
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat * m_fDance_PlaySpeed, 0.1f,fmatResult);

    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
}

void CBackDancer::Dance_Event_Animation_End(_float fTimeDelta)
{
    if (m_bDance_Rest)
        return;
    list<pair<string, _float>>& dance_list = m_listDanceEvent.front();

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    string DanceName = dance_list.front().first;
    string findStr = "em5100";
    string replaceStr = "em0100";
    DanceName.replace(DanceName.find(findStr), findStr.length(), replaceStr);

    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex(DanceName) && m_pModelCom->Get_Finished())
    {
        dance_list.pop_front();
        if (dance_list.empty())
        {
            m_bDance_Rest = true;
            m_listDanceEvent.pop_front();
        }
        else
        {
            m_fDance_PlaySpeed = dance_list.front().second;
            m_pModelCom->Set_AnimIndex(DanceName, false);
        }
    }
}

void CBackDancer::Dance(string strDance , _float danceSpeed)
{
    string DanceName = strDance;
    string findStr = "em5100";
    string replaceStr = "em0100";
    DanceName.replace(DanceName.find(findStr), findStr.length(), replaceStr);
    m_pModelCom->Set_AnimIndex(DanceName, false);
    m_fDance_PlaySpeed = danceSpeed;
}


HRESULT CBackDancer::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_BackDancer"), TEXT("Com_Model")));

    CSaber_Weapon::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("r_hand_attach_00");
    WeaponDesc.pParentTransform = m_pTransformCom;
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;
    WeaponDesc.iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();
    shared_ptr <CPartObject> pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CSaber_Weapon"), &WeaponDesc));
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



    return S_OK;
}

shared_ptr<CBackDancer> CBackDancer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CBackDancer
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBackDancer(pDevice, pContext)
        {}
    };


    shared_ptr<CBackDancer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBackDancer"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CBackDancer::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CBackDancer
    {
        MakeSharedEnabler(const CBackDancer& rhs) : CBackDancer(rhs)
        {}
    };


    shared_ptr<CBackDancer> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBackDancer"));
        assert(false);
    }

    return pInstance;
}

void CBackDancer::Free()
{
}

void CBackDancer::Dying()
{
    if (m_SpotLight)
        m_SpotLight->UnionEffectEnd();
}

void CBackDancer::Animation_End()
{
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_010")
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_011", true);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_020")
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_021", true);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_030")
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em0121_atk-mimosa_031", true);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_012")
        && m_pModelCom->Get_Finished())
        m_bAttack_End = true;
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_022")
        && m_pModelCom->Get_Finished())
        m_bAttack_End = true;
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_032")
        && m_pModelCom->Get_Finished())
        m_bAttack_End = true;
}

void CBackDancer::Animation(_float fTimeDelta)
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
    _float fTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();


    if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_012") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_022")||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em0121_atk-mimosa_032"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    else
     m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pTransformCom->Set_Position(vCurPos);
}

void CBackDancer::Move(_float fTimeDelta)
{
}

void CBackDancer::Attack(_float fTimeDelta)
{
}

void CBackDancer::Turn(_float fTimeDelta)
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
