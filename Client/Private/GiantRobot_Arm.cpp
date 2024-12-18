#include "GiantRobot_Arm.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "PhysxCollider.h"
#include "Texture.h"

#include "QACenterDoor.h"
#include "iostream"
#include "PlayerManager.h"
#include "MapManager.h"
#include "CUi_MapInterAction.h"
#include "Cine_Manager.h"


wstring CGiantRobot_Arm::ObjID = TEXT("CGiantRobot_Arm");
CGiantRobot_Arm::CGiantRobot_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{ pDevice, pContext }
{
}

CGiantRobot_Arm::CGiantRobot_Arm(const CGiantRobot_Arm& rhs)
    : CGimmickBase{ rhs }
{
}

CGiantRobot_Arm::~CGiantRobot_Arm()
{
    Free();
}

HRESULT CGiantRobot_Arm::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_GiantRobot_Arm"), TEXT("../Bin/Resources/Models/Map/Gimmick/GiantRobotArm.FBX"),
            MODEL_TYPE::NONANIM, fMat)
    );

    return S_OK;
}

HRESULT CGiantRobot_Arm::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    CCine_Manager::Get_Instance()->AddLastCallback("EV1180",
        [&]() {
            m_bExecuted = true;
        }
    );
    //m_pModelCom->Set_AnimIndex("em8100_event-punch_012", true);
    m_bDone = false;

    return S_OK;
}

void CGiantRobot_Arm::PriorityTick(_float fTimeDelta)
{
}

void CGiantRobot_Arm::Tick(_float fTimeDelta)
{
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
        m_bExecuted = !m_bExecuted;
    }
#endif
    _matrix Mat = XMMatrixScalingFromVector(XMLoadFloat4(&m_vCollScale)) * XMMatrixTranslationFromVector(XMLoadFloat4(&m_vCollPos)) * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pBodyColliderCom->Update(Mat);
}

void CGiantRobot_Arm::LateTick(_float fTimeDelta)
{
    if (!m_bDone)
    {
        GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pBodyColliderCom);
    }
    else
        return;
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    if (!m_bExecuted || !CCine_Manager::Get_Instance()->IsInGame()) return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());


}

HRESULT CGiantRobot_Arm::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        //if (i == 21 || i == 22 || i == 26 )
        //    continue;

        _bool bEmi = { false };
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
            bEmi = false;
        else
        {
            bEmi = true;
        }

        if (bEmi)
            m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
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

void CGiantRobot_Arm::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    //if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    //{
    //    PLAYERMANAGER->Callback_Event("GiantRobot_Arm_Start");
    //}
    if (pOther->Get_ColliderTag() == TEXT("CNMN_HIT"))
    {

    }
}

void CGiantRobot_Arm::Collision(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{

}

void CGiantRobot_Arm::Collision_Exit(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    //if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    //{
    //    PLAYERMANAGER->Callback_Event("GiantRobot_Arm_End");
    //}
    if (pOther->Get_ColliderTag() == TEXT("CNMN_HIT"))
    {

    }

}

void CGiantRobot_Arm::TakeDamage(const DamageInfo& damageInfo)
{
    //if (m_iCurHit < m_iMaxHit)
    //{
    //    m_eAnimState = HIT;
    //    m_pModelCom->Set_AnimIndex(2, false);
    //    ++m_iCurHit;
    //}
}

void CGiantRobot_Arm::Edit_Attribute()
{

    ImGui::DragFloat3("ColPos", &m_vCollPos.x);
    ImGui::DragFloat3("ColScl", &m_vCollScale.x);

}

void CGiantRobot_Arm::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CGiantRobot_Arm::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CGiantRobot_Arm::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);

    write_typed_data(os, m_vCollPos);
    write_typed_data(os, m_vCollScale);
}

void CGiantRobot_Arm::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);

    read_typed_data(is, m_vCollPos);
    read_typed_data(is, m_vCollScale);
}

HRESULT CGiantRobot_Arm::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GiantRobot_Arm"), TEXT("Model_Com")));

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = { 0.f, 0.f, 0.f };
    obbDesc.vExtents = { 1.f, 1.f, 1.f };
    obbDesc.vRotation = { 0.f, 0.f, 0.f };

    m_pBodyColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("BodyCollider_Com"), &obbDesc));

    //for (_uint i = 0; i < 3; ++i)
    //{
    //    obbDesc.vExtents = { 1.f, 1.f, 1.f };
    //    obbDesc.vRotation = { 0.f, 0.f, 0.f };
    //    obbDesc.vCenter = { 0.f, 0.f, 0.f };
    //    m_pEventCollider[i] = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), _wstring(TEXT("EventCollider_Com")) + to_wstring(i), &obbDesc));
    //    m_ColliderMat[i] = Identity;
    //}
    m_pBodyColliderCom->Set_ColliderTag(L"Giant_Arm");

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
    return S_OK;
}

HRESULT CGiantRobot_Arm::Ready_Parts()
{
    return S_OK;
}

HRESULT CGiantRobot_Arm::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
         return E_FAIL;


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


shared_ptr<CGiantRobot_Arm> CGiantRobot_Arm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CGiantRobot_Arm
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGiantRobot_Arm(pDevice, pContext)
        {}
    };

    shared_ptr<CGiantRobot_Arm> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGiantRobot_Arm"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CGiantRobot_Arm::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CGiantRobot_Arm
    {
        MakeSharedEnabler(const CGiantRobot_Arm& rhs) : CGiantRobot_Arm(rhs)
        {}
    };

    shared_ptr<CGiantRobot_Arm> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGiantRobot_Arm"));
        assert(false);
    }

    return pInstance;
}

void CGiantRobot_Arm::Free()
{
}
