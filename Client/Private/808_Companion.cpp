#include "808_Companion.h"
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
#include "GameManager.h"
#include "808_Cat.h"
#include "808_Controller.h"
#include "Cine_Manager.h"
wstring C808_Companion::ObjID = TEXT("C808_Companion");

C808_Companion::C808_Companion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

C808_Companion::C808_Companion(const C808_Companion& rhs)
    : CGameObject(rhs)
{
}

C808_Companion::~C808_Companion()
{
    Free();
}

HRESULT C808_Companion::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_808_Companion"), TEXT("../Bin/Resources/Models/Player/808_Companion/808_Companion.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );

    return S_OK;
}

HRESULT C808_Companion::Initialize(void* pArg)
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
    m_pModelCom->Set_AnimIndex("am0020_Idle_0000",true);

    _float4x4 fmatResult;
    m_pModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, fmatResult);


    auto pGameInstance = m_pGameInstance.lock();

    m_pFlareOuter = static_pointer_cast<C808_Flare_Effect>(pGameInstance->Clone_Object(C808_Flare_Effect::ObjID, nullptr));
    m_pFlareInner = static_pointer_cast<CFlare_Effect>(pGameInstance->Clone_Object(CFlare_Effect::ObjID, nullptr));
    m_pJustMark   = static_pointer_cast<CJustMark_Effect>(pGameInstance->Clone_Object(CJustMark_Effect::ObjID, nullptr));
   const  _float4x4* pMatrixBone =  m_pModelCom->Get_BoneMatrixPtr("root");

    _float4x4 TempMat;
    _float4x4 justMat;
    _matrix BoneMat = XMMatrixTranslation(pMatrixBone->_41 * 0.59f, pMatrixBone->_42 * 0.55f, pMatrixBone->_43 * 0.57f);
    _matrix justBoneMat = XMMatrixTranslation(pMatrixBone->_41 * 0.9f, pMatrixBone->_42 * 0.68f, pMatrixBone->_43 * 0.57f);
 
    XMStoreFloat4x4(&TempMat, XMMatrixScaling(0.5f, 0.5f, 1.f) * XMMatrixRotationX(XM_PIDIV2) * (BoneMat));
    XMStoreFloat4x4(&justMat, XMMatrixScaling(0.5f, 0.5f, 1.f) * (justBoneMat));
    m_pFlareOuter->SetRootMatrix(TempMat);
    m_pFlareOuter->Get_Transform()->Set_WorldMatrix(TempMat);
    m_pFlareInner->SetRootMatrix(TempMat);
    m_pFlareInner->Get_Transform()->Set_WorldMatrix(TempMat);
    m_pFlareOuter->Set_ParentWorld(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pFlareInner->Set_ParentWorld(m_pTransformCom->Get_WorldMatrix_Ptr());

    m_pFlareOuter->SetEffectLoop(true);
    m_pFlareInner->SetEffectLoop(true);


    m_pJustMark->Set_ParentWorld(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pJustMark->SetEffectIsView(false);
    //XMStoreFloat4x4(&TempMat, BoneMat);
    m_pJustMark->SetRootMatrix(justMat);
    m_pJustMark->Get_Transform()->Set_WorldMatrix(justMat);

    return S_OK;
}

void C808_Companion::PriorityTick(_float fTimeDelta)
{
    if (m_bCatMode)
        return;
    Move(fTimeDelta);

    m_pFlareOuter->PriorityTick(fTimeDelta);
    m_pFlareInner->PriorityTick(fTimeDelta);
    m_pJustMark->PriorityTick(fTimeDelta);
}

void C808_Companion::Tick(_float fTimeDelta)
{
    if (m_bCatMode)
        return;
    Animation_End();
    Animation(fTimeDelta);

    //if (m_pGameInstance.lock()->Get_KeyDown(DIK_RETURN))
    //{
    //    static _uint debug = 0;
    //    Play_JustMark_Effect(debug % 4);
    //    ++debug;
    //    
    //}


    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

    m_pFlareOuter->Tick(fTimeDelta);
    m_pFlareInner->Tick(fTimeDelta);
    m_pJustMark->Tick(fTimeDelta);
}

void C808_Companion::LateTick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
    {
        return;
    }
    if (m_bCatMode)
        return;
    if (m_bRender)
        m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    m_pFlareOuter->LateTick(fTimeDelta);
    m_pFlareInner->LateTick(fTimeDelta);
    m_pJustMark->LateTick(fTimeDelta);
    _float fTimerPerBeat = CBeat_Manager::Get_Instance()->Get_Timer() / CBeat_Manager::Get_Instance()->Get_Beat();
    //m_pFlareOuter->Set_ParentWorld(m_pTransformCom->Get_WorldMatrix_Ptr());
    //m_pFlareInner->Set_ParentWorld(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pFlareOuter->SetEffectTimeAcc(fTimerPerBeat);
    m_pFlareInner->SetEffectTimeAcc(fTimerPerBeat);
    m_pJustMark->SetEffectTimeMax(CBeat_Manager::Get_Instance()->Get_Beat());
    _float4 vColor;
    vColor.x = m_vColor[m_iAssist].x;
    vColor.y = m_vColor[m_iAssist].y;
    vColor.z = m_vColor[m_iAssist].z;
    vColor.w = 1.f;
    m_pFlareInner->SetEffectBaseColorToColor(vColor);
    vColor.x *= 4.f;
    vColor.y *= 4.f;
    vColor.z *= 4.f;
    m_pFlareOuter->SetEffectBaseColorToColor(vColor);
}

HRESULT C808_Companion::Render()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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
        _bool bNoEmit = false;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        if (i == 1)
        {
            _float4 vColor{};
            vColor.x = m_vColor[m_iAssist].x;
            vColor.y = m_vColor[m_iAssist].y;
            vColor.z = m_vColor[m_iAssist].z;
            vColor.w = m_fEmit;

            m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::EmissionColor);
        }
        else
            m_pShaderCom->Begin(0);

        //if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        //	return E_FAIL;

        m_pModelCom->Render((_uint)i);

        if (i == 1)
            continue;
        m_pShaderCom->Bind_Float("g_OutlineWidth", 1.0f);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        m_pModelCom->Render((_uint)i);

    }

    return S_OK;
}

HRESULT C808_Companion::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_808_Companion"), TEXT("Com_Model")));



    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

shared_ptr<C808_Companion> C808_Companion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public C808_Companion
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : C808_Companion(pDevice, pContext)
        {}
    };


    shared_ptr<C808_Companion> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : C808_Companion"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> C808_Companion::Clone(void* pArg)
{
    struct MakeSharedEnabler :public C808_Companion
    {
        MakeSharedEnabler(const C808_Companion& rhs) : C808_Companion(rhs)
        {}
    };


    shared_ptr<C808_Companion> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : C808_Companion"));
        assert(false);
    }

    return pInstance;
}

void C808_Companion::Free()
{
}

void C808_Companion::Play_JustMark_Effect(_uint iStreak)
{
    m_pJustMark->EffectStart();
    m_pJustMark->Set_BeatStreak(iStreak);
   /* const  _float4x4* pMatrixBone = m_pModelCom->Get_BoneMatrixPtr("root");
    _float4x4 justMat;
    _matrix justBoneMat = XMMatrixTranslation(pMatrixBone->_41 * 0.9f, pMatrixBone->_42 * 0.68f, pMatrixBone->_43 * 0.57f);

    _float4x4 TempMat;
    auto pGameInstance = m_pGameInstance.lock();
    XMStoreFloat4x4(&TempMat, XMMatrixScaling(0.5f, 0.5f, 1.f) * (justBoneMat) * XMMatrixTranslation(pGameInstance->Random_Float(-0.4f, -0.2f), 0.f , 0.f));
    m_pJustMark->SetRootMatrix(TempMat);
    m_pJustMark->Get_Transform()->Set_WorldMatrix(TempMat);
    m_pJustMark->SetRootMatrix(TempMat);*/

}

void C808_Companion::Shop_Animation()
{

}

void C808_Companion::Animation_End()
{
    if (m_pModelCom->Get_StringAnimIndex("am0020_action-cp_040") == m_pModelCom->Get_CurrentAnimIndex() 
        && m_pModelCom->Get_Current_Track_Position() > 3.f)
    {
        shared_ptr<C808_Cat> pCat = static_pointer_cast<C808_Cat> (static_pointer_cast<C808_Controller>(PLAYERMANAGER->Get_808_Controller())->Get_808_Cat());
        pCat->Set_CatMode(true);
        pCat->Get_Transform()->Set_Position(PLAYERMANAGER->Get_PlayerPos());
        pCat->Get_Transform()->LookAt_Horizontal(pCat->Get_Transform()->Get_Position() + PLAYERMANAGER->Get_Player()->Get_Transform()->Get_NormLook());
        shared_ptr<CModel> pCatModel = static_pointer_cast<CModel>(pCat->Find_Component(TEXT("Com_Model")));
        pCatModel->Set_AnimIndexNonCancle("am0000_action-cp_040",false);
        m_bCatMode = true;
    }
    if (m_pModelCom->Get_StringAnimIndex("am0020_action-cp_042") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_pModelCom->Set_AnimIndexNonCancle_NoInterpole("am0020_Idle_0000", false);
        m_bZero_Pos = false;
    }
}

void C808_Companion::Animation(_float fTimeDelta)
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

    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);



    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pTransformCom->Set_Position(vCurPos);
}

void C808_Companion::Move(_float fTimeDelta)
{

    shared_ptr<CTransform> pCamTransform = GAMEINSTANCE->Get_MainCamera()->Get_Transform();
    auto pPlayerManager = CPlayer_Manager::Get_Instance();
    m_iAssist = (_uint)pPlayerManager->Get_Assist();
    shared_ptr<CTransform> pPlayerTransform = pPlayerManager->Get_Player()->Get_Transform();
    _vector vPos = pPlayerTransform->Get_Position();
    //vPos -= XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE_RIGHT)) * 0.65f;
    //vPos -= XMVector3Normalize(XMVectorSetY(pCamTransform->Get_State(CTransform::STATE_LOOK), 0)) * 0.35f;
    //_float yPos = XMVectorGetY(pPlayerTransform->Get_Position());
    _float yPos = XMVectorGetY(pPlayerTransform->Get_Position());
    //yPos += 1.5f;

    _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    m_fEmit = (sin((fTimer / fBeat) * XM_2PI));
    yPos += 0.03f * m_fEmit;
    m_fEmit = (1.f - fTimer / fBeat) * .7f + .3f;
    vPos = XMVectorSetY(vPos, yPos);
    m_pTransformCom->Set_Position(vPos);
    m_pTransformCom->LookAt_Horizontal(vPos + XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE_LOOK)) * 20.f);
    


     //_float fSpeed = 3.8f;

    //_vector InitDir = XMVector3Normalize(vPos - m_pTransformCom->Get_Position());
    //_float dist = XMVectorGetX(XMVector3Length((vPos)-(m_pTransformCom->Get_Position())));

    //if (dist > XMVectorGetX(XMVector3Length(InitDir * fSpeed * fTimeDelta)))
    //{
    //    m_pTransformCom->Set_Position(m_pTransformCom->Get_Position()  + InitDir  * fSpeed * fTimeDelta);
    //}
    //else
    //    m_pTransformCom->Set_Position(vPos);


}