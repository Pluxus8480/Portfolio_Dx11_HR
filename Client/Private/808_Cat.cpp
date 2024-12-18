#include "808_Cat.h"
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
wstring C808_Cat::ObjID = TEXT("C808_Cat");

C808_Cat::C808_Cat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

C808_Cat::C808_Cat(const C808_Cat& rhs)
    : CGameObject(rhs)
{
}

C808_Cat::~C808_Cat()
{
    Free();
}

HRESULT C808_Cat::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_808_Cat"), TEXT("../Bin/Resources/Models/Player/808_Cat/808_Cat.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );

    return S_OK;
}

HRESULT C808_Cat::Initialize(void* pArg)
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
    m_pModelCom->Set_AnimIndex("am0000_action-cp_021", false);
    _float4x4 xMat = {};
    m_pModelCom->Play_Animation(0.0001f, 0.0001f,0.1f, xMat);

    auto pGameInstance = m_pGameInstance.lock();

    return S_OK;
}

void C808_Cat::PriorityTick(_float fTimeDelta)
{
    if (!m_bCatMode)
        return;
    Move(fTimeDelta);

}

void C808_Cat::Tick(_float fTimeDelta)
{
    if (!m_bCatMode)
        return;
    Animation_End();
    Animation(fTimeDelta);




    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

}

void C808_Cat::LateTick(_float fTimeDelta)
{
    if (!m_bCatMode)
        return;
    if (m_bRender)
        m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());


}

HRESULT C808_Cat::Render()
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

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        //if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        //   return E_FAIL;

        m_pModelCom->Render((_uint)i);
        if (i == 9)
            continue;
        m_pShaderCom->Bind_Float("g_OutlineWidth", 1.0f);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        m_pModelCom->Draw((_uint)i);
    }

    return S_OK;
}

HRESULT C808_Cat::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_808_Cat"), TEXT("Com_Model")));



    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

shared_ptr<C808_Cat> C808_Cat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public C808_Cat
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : C808_Cat(pDevice, pContext)
        {}
    };


    shared_ptr<C808_Cat> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : C808_Cat"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> C808_Cat::Clone(void* pArg)
{
    struct MakeSharedEnabler :public C808_Cat
    {
        MakeSharedEnabler(const C808_Cat& rhs) : C808_Cat(rhs)
        {}
    };


    shared_ptr<C808_Cat> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : C808_Cat"));
        assert(false);
    }

    return pInstance;
}

void C808_Cat::Free()
{
}



void C808_Cat::Animation_End()
{
    if (m_pModelCom->Get_StringAnimIndex("am0000_action-cp_042") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        shared_ptr<C808_Companion> pCompaion = static_pointer_cast<C808_Companion> (static_pointer_cast<C808_Controller>(PLAYERMANAGER->Get_808_Controller())->Get_808_Companion());
        pCompaion->Set_CatMode(false);
        pCompaion->Set_Zero_Pos(true);
        shared_ptr<CModel> pCatModel = static_pointer_cast<CModel>(pCompaion->Find_Component(TEXT("Com_Model")));
        pCatModel->Set_AnimIndexNonCancle_NoInterpole("am0020_Idle_0000", false);
        m_bCatMode = false;
    }
    if (m_pModelCom->Get_StringAnimIndex("am0000_action-cp_040") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        auto UIManager = CUi_Mgr::Get_Instance();
        _uint iLevelIndex = GAMEMANAGER->Get_Current_Level();
        auto StoreSystem = UIManager->Find_UI(CUi_StoreSystem::ObjID);
        if (nullptr == StoreSystem)
        {
            CUi_Mgr::Get_Instance()->Open_Store();
        }
        m_pModelCom->Set_AnimIndex("am0000_action-cp_041",true);
    }
}

void C808_Cat::Animation(_float fTimeDelta)
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

void C808_Cat::Move(_float fTimeDelta)
{



}