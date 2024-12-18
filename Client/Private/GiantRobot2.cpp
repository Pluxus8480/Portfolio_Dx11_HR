#include "GiantRobot2.h"
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

wstring CGiantRobot2::ObjID = TEXT("CGiantRobot2");
CGiantRobot2::CGiantRobot2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{ pDevice, pContext }
{
}

CGiantRobot2::CGiantRobot2(const CGiantRobot2& rhs)
    : CGimmickBase{ rhs }
{
}

CGiantRobot2::~CGiantRobot2()
{
    Free();
}

HRESULT CGiantRobot2::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_GiantRobot"), TEXT("../Bin/Resources/Models/Map/Gimmick/GiantRobot.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    return S_OK;
}

HRESULT CGiantRobot2::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pModelCom->Set_AnimIndex("em8100_event-punch_012", true);
    m_bDone = false;
    m_pBoneMat = m_pModelCom->Get_BoneMatrixPtr("r_Canon_02");

    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_HandMat, 1, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_HandMat, 1, isBillBoard);
        }
    );
    m_pModelCom->Set_AnimIndex("em8100_event-beam_020", false);
    return S_OK;
}

void CGiantRobot2::PriorityTick(_float fTimeDelta)
{
}

void CGiantRobot2::Tick(_float fTimeDelta)
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
        m_pModelCom->Set_AnimIndex("em8100_event-beam_000", false);
        m_bSecPos = false;
        m_bExecuted = true;
    }
    if (m_pGameInstance.lock()->Get_KeyDown(DIK_O))
    {
        m_pModelCom->Set_AnimIndex("em8100_event-beam_020", false);
        m_bSecPos = true;
    }

#endif
    _matrix Bone = XMLoadFloat4x4(m_pBoneMat);
    Bone.r[0] = XMVector3Normalize(Bone.r[0]);
    Bone.r[1] = XMVector3Normalize(Bone.r[1]);
    Bone.r[2] = XMVector3Normalize(Bone.r[2]);
    if (m_bSecPos)
    {
        _float fYaw = atan2(-m_vLook.x, -m_vLook.z);
        _float fXZLen = sqrtf(m_vLook.x * m_vLook.x + m_vLook.z * m_vLook.z);
        _float fPitch = atan2(m_vLook.y, fXZLen);
        XMStoreFloat4x4(&m_HandMat, XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(90.f), XMConvertToRadians(90.f)) * Bone * XMMatrixRotationRollPitchYaw(fPitch, fYaw, 0.f) *
            XMMatrixTranslationFromVector(XMLoadFloat4(&m_vSecondPos)) * m_pTransformCom->Get_WorldMatrix_XMMat());
        m_fMove = 0.f;
    }
    else
    {
        _float fTPR = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();

        _float fMove = 2.f;
        if (m_bLeft && !m_bChanged || !m_bLeft && m_bChanged)
        {
            fMove = -2.f;
        }
        if (fTPR <= 0.5f)
        {
            if (m_bChanged)
            {
                fMove *= -1.f;
            }
            fMove = fTPR * fMove * 2.f + (0.5f - fTPR) * 2.f * (-fMove);
            m_bChanged = false;
        }
        else if (fTPR >= 0.9f && !m_bChanged)
        {
            m_bLeft = !m_bLeft;
            m_bChanged = true;
        }
        m_fMove = fMove;

        XMStoreFloat4x4(&m_HandMat, XMMatrixScaling(1.25f,1.25f,1.25f) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(-10.f), XMConvertToRadians(90.f), XMConvertToRadians(0.f))
            * Bone * XMMatrixTranslation(m_fMove, 0.f, 0.f) * m_pTransformCom->Get_WorldMatrix_XMMat());
    }
    
    _matrix Mat = XMMatrixScalingFromVector(XMLoadFloat4(&m_vCollScale)) * XMMatrixTranslationFromVector(XMLoadFloat4(&m_vCollPos)) * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pBodyColliderCom->Update(Mat);

    _float4x4 fMat = {};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);


    if (m_pModelCom->Get_Finished())
    {
        string strName = m_pModelCom->Get_AnimName();
        if (strName == "em8100_event-beam_000")
        {
            m_pModelCom->Set_AnimIndex("em8100_event-beam_010", true);
        }
        else
        {
            //m_bExecuted = false;
        }
        m_bLeft = !m_bLeft;
    }

    if (m_bSecPos)
    {
        m_fTimer -= fTimeDelta;
        if (m_fTimer <= 0.f)
        {
            m_bExecuted = false;
        }
    }
    m_pModelCom->PlayReservedCallbacks();

}

void CGiantRobot2::LateTick(_float fTimeDelta)
{
    if (!m_bDone)
    {
        GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pBodyColliderCom);
    }
    else
        return;
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    if (!m_bExecuted) return;
    if (!CCine_Manager::Get_Instance()->IsInGame()) return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());


}

HRESULT CGiantRobot2::Render()
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

void CGiantRobot2::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    //if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    //{
    //    PLAYERMANAGER->Callback_Event("GiantRobot_Start");
    //}
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        PLAYERMANAGER->Callback_Event("GiantRobot_2");

        m_pModelCom->Set_AnimIndex("em8100_event-beam_000", false);
        m_bSecPos = false;
        m_bExecuted = true;
    }
}

void CGiantRobot2::Collision(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{

}

void CGiantRobot2::Collision_Exit(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    //if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    //{
    //    PLAYERMANAGER->Callback_Event("GiantRobot_End");
    //}
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        m_pModelCom->Set_AnimIndex("em8100_event-beam_020", false);
        m_bSecPos = true;
        m_fTimer = 30.f;
    }

}

void CGiantRobot2::TakeDamage(const DamageInfo& damageInfo)
{
    //if (m_iCurHit < m_iMaxHit)
    //{
    //    m_eAnimState = HIT;
    //    m_pModelCom->Set_AnimIndex(2, false);
    //    ++m_iCurHit;
    //}
}

void CGiantRobot2::Edit_Attribute()
{
    if (ImGui::DragFloat3("SecPos", &m_vSecondPos.x))
    {

    }
    if (ImGui::DragFloat3("SecLook", &m_vLook.x))
    {

    }
    ImGui::DragFloat3("ColPos", &m_vCollPos.x);
    ImGui::DragFloat3("ColScl", &m_vCollScale.x);

}

void CGiantRobot2::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CGiantRobot2::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CGiantRobot2::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);
    write_typed_data(os, m_vSecondPos);
    write_typed_data(os, m_vLook);
    write_typed_data(os, m_vCollPos);
    write_typed_data(os, m_vCollScale);
}

void CGiantRobot2::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);
    read_typed_data(is, m_vSecondPos);
    read_typed_data(is, m_vLook);
    read_typed_data(is, m_vCollPos);
    read_typed_data(is, m_vCollScale);
}

HRESULT CGiantRobot2::Ready_Components()
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

HRESULT CGiantRobot2::Ready_Parts()
{
    return S_OK;
}

HRESULT CGiantRobot2::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (m_bSecPos)
    {
        _float4x4 Mat;

        _float fYaw = atan2(-m_vLook.x, -m_vLook.z);
        _float fXZLen = sqrtf(m_vLook.x * m_vLook.x + m_vLook.z * m_vLook.z);
        _float fPitch = atan2(m_vLook.y, fXZLen);
        XMStoreFloat4x4(&Mat, XMMatrixRotationRollPitchYaw(fPitch, fYaw, 0.f) *
            XMMatrixTranslationFromVector(XMLoadFloat4(&m_vSecondPos)) * m_pTransformCom->Get_WorldMatrix_XMMat());
        m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Mat);
    }
    else
    {
        _float4x4 Mat;
        XMStoreFloat4x4(&Mat, XMMatrixTranslation(m_fMove, 0.f, 0.f) * m_pTransformCom->Get_WorldMatrix_XMMat());
        m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Mat);

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


shared_ptr<CGiantRobot2> CGiantRobot2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CGiantRobot2
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGiantRobot2(pDevice, pContext)
        {}
    };

    shared_ptr<CGiantRobot2> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGiantRobot2"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CGiantRobot2::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CGiantRobot2
    {
        MakeSharedEnabler(const CGiantRobot2& rhs) : CGiantRobot2(rhs)
        {}
    };

    shared_ptr<CGiantRobot2> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGiantRobot2"));
        assert(false);
    }

    return pInstance;
}

void CGiantRobot2::Free()
{
}
