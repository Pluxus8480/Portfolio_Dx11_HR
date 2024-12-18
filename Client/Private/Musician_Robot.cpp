#include "Musician_Robot.h"
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
#include "Musician_Instrument.h"
#include "GameManager.h"
wstring CMusician_Robot::ObjID = TEXT("CMusician_Robot");

CMusician_Robot::CMusician_Robot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMusician_Robot::CMusician_Robot(const CMusician_Robot& rhs)
    : CGameObject(rhs)
{
}

CMusician_Robot::~CMusician_Robot()
{
    Free();
}

HRESULT CMusician_Robot::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Musician_Robot"), TEXT("../Bin/Resources/Models/Monsters/Mimosa_Musician/Musician_Robot(np130).fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_Owning_ObjectID.push_back(CMusician_Instrument::ObjID);

    return S_OK;
}

HRESULT CMusician_Robot::Initialize(void* pArg)
{
    MISCOBJ_DESC* TransformDesc = static_cast<MISCOBJ_DESC*>(pArg);
    TransformDesc->fSpeedPerSec = 4.f;
    TransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(TransformDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(TransformDesc->worldMatrix);

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


    //Create_Instrument();
    //Play_Instrument();

    _float4x4 xMat = {};
    m_pModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, xMat);


    return S_OK;
}

void CMusician_Robot::PriorityTick(_float fTimeDelta)
{
}

void CMusician_Robot::Tick(_float fTimeDelta)
{
    if (!m_bRender)
        return;
    Animation_End();

  
    Animation(fTimeDelta);


    for (auto& pPartObj : m_pWeapon)
        pPartObj->Tick(fTimeDelta);

    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

}

void CMusician_Robot::LateTick(_float fTimeDelta)
{
    if (m_bRender)
    {
        for (auto& pPartObj : m_pWeapon)
            pPartObj->LateTick(fTimeDelta);

        m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    }
}

HRESULT CMusician_Robot::Render()
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

void CMusician_Robot::Edit_Attribute()
{
    ImGui::InputInt("Instrument Type", &m_iType);
    if (ImGui::Button("Create Instrument"))
    {
        for (auto& iter : m_pWeapon)
            iter->Set_Dead(true);
        m_pWeapon.clear();
        Create_Instrument();
        Play_Instrument();
    }
}

void CMusician_Robot::Save_Data(ofstream& os)
{
    write_typed_data(os, m_iType);
}

void CMusician_Robot::Load_Data(ifstream& is)
{
    read_typed_data(is, m_iType);
    Create_Instrument();
    Play_Instrument();
}

void CMusician_Robot::Create_Instrument()
{
    CMusician_Instrument::INSTRUMENT_DESC		instDesc{};
    instDesc.iType = m_iType;
    if(m_iType == 5 || m_iType == 6)
        instDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("l_hand_attach_00");
    else
        instDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("prop_00");
    instDesc.pParentTransform = m_pTransformCom;
    instDesc.fSpeedPerSec = 0.f;
    instDesc.fRotationPerSec = 0.f;
    //instDesc.iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();
    instDesc.iLevelIndex = LEVEL_MIMOSA;

    shared_ptr <CPartObject> pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CMusician_Instrument"), &instDesc));

    m_pWeapon.push_back(pWeapon);
}

void CMusician_Robot::Play_Instrument()
{
    switch (m_iType)
    {
    case 1:
    {
        m_pModelCom->Set_AnimIndex("np0130_idle_010", true);
        break;
    }
    case 2:
    {
        m_pModelCom->Set_AnimIndex("np0130_idle_020", true);
        break;
    }
    case 4:
    {
        m_pModelCom->Set_AnimIndex("np0130_idle_040", true);
        break;
    }
    case 5:
    {
        m_pModelCom->Set_AnimIndex("np0130_idle_050", true);
        break;
    }
    case 6:
    {
        m_pModelCom->Set_AnimIndex("np0130_idle_060", true);
        break;
    }
    }

}

HRESULT CMusician_Robot::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Musician_Robot"), TEXT("Com_Model")));

   

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

shared_ptr<CMusician_Robot> CMusician_Robot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMusician_Robot
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMusician_Robot(pDevice, pContext)
        {}
    };


    shared_ptr<CMusician_Robot> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMusician_Robot"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMusician_Robot::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMusician_Robot
    {
        MakeSharedEnabler(const CMusician_Robot& rhs) : CMusician_Robot(rhs)
        {}
    };


    shared_ptr<CMusician_Robot> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMusician_Robot"));
        assert(false);
    }

    return pInstance;
}

void CMusician_Robot::Free()
{
}

void CMusician_Robot::Dying()
{

}

void CMusician_Robot::Animation_End()
{
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();


}

void CMusician_Robot::Animation(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

    _float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f;

    _uint index = m_pModelCom->Get_CurrentAnimIndex();
    _float fTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);

    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pTransformCom->Set_Position(vCurPos);
}

void CMusician_Robot::Move(_float fTimeDelta)
{
}

void CMusician_Robot::Attack(_float fTimeDelta)
{
}