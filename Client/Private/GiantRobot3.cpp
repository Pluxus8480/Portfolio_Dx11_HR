#include "GiantRobot3.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "PhysxCollider.h"
#include "Texture.h"

#include "UI_Event_Cut.h"

#include "QACenterDoor.h"
#include "iostream"
#include "PlayerManager.h"
#include "MapManager.h"
#include "CUi_MapInterAction.h"
#include "Cine_Manager.h"

wstring CGiantRobot3::ObjID = TEXT("CGiantRobot3");
CGiantRobot3::CGiantRobot3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{ pDevice, pContext }
{
}

CGiantRobot3::CGiantRobot3(const CGiantRobot3& rhs)
    : CGimmickBase{ rhs }
{
}

CGiantRobot3::~CGiantRobot3()
{
    Free();
}

void CGiantRobot3::SetUp_CinematicCallBack()
{
    // Arm cutting scene
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1180", 2, 0,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::CutRobotArm;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1180", 2, 1,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );
}

HRESULT CGiantRobot3::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_GiantRobot"), TEXT("../Bin/Resources/Models/Map/Gimmick/GiantRobot.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    return S_OK;
}

HRESULT CGiantRobot3::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    CCine_Manager::Get_Instance()->AddLastCallback("EV1160",
        [&]() {
            m_pModelCom->Set_AnimIndex("em8100_event-punch_001", true);
            m_bSecPos = false;
            m_bThirdPos = false;
            m_bExecuted = true;
        }
    );

    CCine_Manager::Get_Instance()->AddLastCallback("EV1170",
        [&]() {
            m_pModelCom->Set_AnimIndex("em8100_event-punch_009", true);
            m_bSecPos = true;
            m_bThirdPos = false;
            m_bExecuted = true;
        }
    );
    CCine_Manager::Get_Instance()->AddLastCallback("EV1180",
        [&]() {
            m_pModelCom->Set_AnimIndex("em8100_event-punch_012", true);
            m_bSecPos = false;
            m_bThirdPos = true;
            m_bExecuted = true;
        }
    );

    //m_vThirdPos
    //m_pModelCom->Set_AnimIndex("em8100_event-punch_012", true);
    m_bDone = false;

    return S_OK;
}

void CGiantRobot3::PriorityTick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
    {
        m_bExecuted = false;
    }
}

void CGiantRobot3::Tick(_float fTimeDelta)
{
    //if (!m_bOn)
    //{
    //    // Arm cutting scene
    //    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1180", 2, 0,
    //        [&](_float4, _float4, _float3) {
    //            CUI_Event_Cut::Desc Desc;
    //            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::CutRobotArm;
    //            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
    //        }
    //    );
    //    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1180", 2, 1,
    //        [&](_float4, _float4, _float3) {
    //            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
    //        }
    //    );
    //
    //    m_bOn = true;
    //}

    //if (!m_bDone && m_bOn)
    //{
    //    if (m_vecRelatives.size() > 0)
    //    {
    //        CGimmickBase::Done();
    //    }
    //}
#ifdef _DEBUG
    if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
    {
        m_pModelCom->Set_AnimIndex("em8100_event-punch_001", true);
        m_bSecPos = false;
        m_bThirdPos = false;
        m_bExecuted = true;
    }
    if (m_pGameInstance.lock()->Get_KeyDown(DIK_O))
    {
        m_pModelCom->Set_AnimIndex("em8100_event-punch_009", true);
        m_bSecPos = true;
        m_bThirdPos = false;
        m_bExecuted = true;
    }
    if (m_pGameInstance.lock()->Get_KeyDown(DIK_P))
    {
        m_pModelCom->Set_AnimIndex("em8100_event-punch_012", true);
        m_bSecPos = false;
        m_bThirdPos = true;
        m_bExecuted = true;
    }

#endif
    _matrix Mat = XMMatrixScalingFromVector(XMLoadFloat4(&m_vCollScale)) * XMMatrixTranslationFromVector(XMLoadFloat4(&m_vCollPos)) * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pBodyColliderCom->Update(Mat);

    _float4x4 fMat = {};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);


    if (m_pModelCom->Get_Finished())
    {
        string strName = m_pModelCom->Get_AnimName();
    }

}

void CGiantRobot3::LateTick(_float fTimeDelta)
{
    if (!m_bDone)
    {
        //GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pBodyColliderCom);
    }
    else
        return;
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    if (!m_bExecuted) return;
    if (!CCine_Manager::Get_Instance()->IsInGame()) return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());


}

HRESULT CGiantRobot3::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (m_bThirdPos && (i == 21 || i == 22 || i == 26))
            continue;

        _bool bEmi = { false };
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
            bEmi = false;
        else
        {
            bEmi = true;
        }

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        if (bEmi)
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
        else
            m_pShaderCom->Begin(0);

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG

    if (m_pBodyColliderCom)
        m_pBodyColliderCom->Render();

#endif

    return S_OK;
}

void CGiantRobot3::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    //if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    //{
    //    PLAYERMANAGER->Callback_Event("GiantRobot_Start");
    //}
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
    }
}

void CGiantRobot3::Collision(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{

}

void CGiantRobot3::Collision_Exit(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    //if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    //{
    //    PLAYERMANAGER->Callback_Event("GiantRobot_End");
    //}
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
    }

}

void CGiantRobot3::TakeDamage(const DamageInfo& damageInfo)
{
    //if (m_iCurHit < m_iMaxHit)
    //{
    //    m_eAnimState = HIT;
    //    m_pModelCom->Set_AnimIndex(2, false);
    //    ++m_iCurHit;
    //}
}

void CGiantRobot3::Edit_Attribute()
{
    if (ImGui::DragFloat3("SecPos", &m_vSecondPos.x))
    {

    }
    if (ImGui::DragFloat3("SecPos", &m_vThirdPos.x))
    {

    }
    if (ImGui::DragFloat3("SecLook", &m_vLook.x))
    {

    }
    ImGui::DragFloat3("ColPos", &m_vCollPos.x);
    ImGui::DragFloat3("ColScl", &m_vCollScale.x);

}

void CGiantRobot3::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CGiantRobot3::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CGiantRobot3::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);
    write_typed_data(os, m_vSecondPos);
    write_typed_data(os, m_vLook);
    write_typed_data(os, m_vCollPos);
    write_typed_data(os, m_vCollScale);
}

void CGiantRobot3::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);
    read_typed_data(is, m_vSecondPos);
    read_typed_data(is, m_vLook);
    read_typed_data(is, m_vCollPos);
    read_typed_data(is, m_vCollScale);
}

HRESULT CGiantRobot3::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GiantRobot"), TEXT("Model_Com")));

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = { 0.5f, 2.f, 1.5f };
    obbDesc.vExtents = { 1.f, 1.f, 1.f };
    obbDesc.vRotation = { 0.f, 0.f, 0.f };
    obbDesc.vExtents = { 3.f, 2.f, 2.f };
    obbDesc.vCenter = { -1.f, 2.f, 3.f };
    m_pBodyColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("BodyCollider_Com"), &obbDesc));

    //for (_uint i = 0; i < 3; ++i)
    //{
    //    obbDesc.vExtents = { 1.f, 1.f, 1.f };
    //    obbDesc.vRotation = { 0.f, 0.f, 0.f };
    //    obbDesc.vCenter = { 0.f, 0.f, 0.f };
    //    m_pEventCollider[i] = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), _wstring(TEXT("EventCollider_Com")) + to_wstring(i), &obbDesc));
    //    m_ColliderMat[i] = Identity;
    //}


    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
    return S_OK;
}

HRESULT CGiantRobot3::Ready_Parts()
{
    return S_OK;
}

HRESULT CGiantRobot3::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (m_bSecPos || m_bThirdPos)
    {
        _float4x4 Mat;

        _float fYaw = atan2(-m_vLook.x, -m_vLook.z);
        _float fXZLen = sqrtf(m_vLook.x * m_vLook.x + m_vLook.z * m_vLook.z);
        _float fPitch = atan2(m_vLook.y, fXZLen);
        if(m_bSecPos)
        XMStoreFloat4x4(&Mat, XMMatrixRotationRollPitchYaw(fPitch, fYaw, 0.f) *
            XMMatrixTranslationFromVector(XMLoadFloat4(&m_vSecondPos)) * m_pTransformCom->Get_WorldMatrix_XMMat());
        else
            XMStoreFloat4x4(&Mat, XMMatrixRotationRollPitchYaw(fPitch, fYaw, 0.f) *
                XMMatrixTranslationFromVector(XMLoadFloat4(&m_vThirdPos)) * m_pTransformCom->Get_WorldMatrix_XMMat());
        m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Mat);
    }
    else
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;
    }

    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}


shared_ptr<CGiantRobot3> CGiantRobot3::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CGiantRobot3
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGiantRobot3(pDevice, pContext)
        {}
    };

    shared_ptr<CGiantRobot3> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGiantRobot3"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CGiantRobot3::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CGiantRobot3
    {
        MakeSharedEnabler(const CGiantRobot3& rhs) : CGiantRobot3(rhs)
        {}
    };

    shared_ptr<CGiantRobot3> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGiantRobot3"));
        assert(false);
    }

    return pInstance;
}

void CGiantRobot3::Free()
{
}
