#include "SteelFrame_Platform.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "MapManager.h"
#include "BuiltIn_Magnet.h"
#include "Cine_Manager.h"

wstring CSteelFrame_Platform::ObjID = TEXT("CSteelFrame_Platform");
CSteelFrame_Platform::CSteelFrame_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CSteelFrame_Platform::CSteelFrame_Platform(const CSteelFrame_Platform& rhs)
    : CGimmickBase{rhs}
{
}

CSteelFrame_Platform::~CSteelFrame_Platform()
{
    Free();
}

void CSteelFrame_Platform::Execute(_bool bExecute)
{
    if (m_bExecuted == bExecute) return;

    m_bExecuted = bExecute;
}

void CSteelFrame_Platform::Set_Dead(bool bDead)
{
    m_bIsDead = bDead;

    m_pAttachedMagnet->Set_Dead(bDead);
}

HRESULT CSteelFrame_Platform::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SteelFrame_Platform"), TEXT("../Bin/Resources/Models/Map/Gimmick/SteelFrame_Platform.fbx"), MODEL_TYPE::ANIM, fMat)
    );

    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SteelFrame_Platform_Col"), TEXT("../Bin/Resources/Models/Map/Gimmick/SteelFrame_Platform_Col.FBX"), MODEL_TYPE::NONANIM, fMat)
    );

    m_List_Owning_ObjectID.push_back(CBuiltIn_Magnet::ObjID);

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("SteelFrame_Platform"), TEXT("../Bin/Resources/Sounds/MapObject/SteelFrame_Platform.ogg")));


    return S_OK;
}

HRESULT CSteelFrame_Platform::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eState = IDLE_READY;
    m_pModelCom->Set_AnimIndex(IDLE_READY, false);
    _float4x4 fMat{};
    m_pModelCom->Play_Animation(0.016f, 0.016f, 0.1f, fMat);

    Ready_Parts();

    CCine_Manager::Get_Instance()->AddLastCallback
    ("EV1150", [&]()
        {
            Execute();
        }
    );
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        }
    );
    return S_OK;
}

void CSteelFrame_Platform::PriorityTick(_float fTimeDelta)
{
    m_pAttachedMagnet->PriorityTick(fTimeDelta);
}

void CSteelFrame_Platform::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_L))
        Execute();
#endif

    if (m_bBegin)
    {
        if (m_bExecuted)
        {
            _matrix worldMat = m_pTransformCom->Get_WorldMatrix_XMMat();
            //Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);        

            for (size_t i = 0; i < COL_END; i++)
            {
                _matrix Transpose = XMLoadFloat4x4(m_pBoneMatrix[i]) * worldMat;
                Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
                Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
                Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
                _float4x4 mat;
                XMStoreFloat4x4(&mat, Transpose);
                PxMat44 pxWorld;
                pxWorld.column0.x = mat.m[0][0];
                pxWorld.column0.y = mat.m[0][1];
                pxWorld.column0.z = mat.m[0][2];
                pxWorld.column0.w = mat.m[0][3];
                pxWorld.column1.x = mat.m[1][0];
                pxWorld.column1.y = mat.m[1][1];
                pxWorld.column1.z = mat.m[1][2];
                pxWorld.column1.w = mat.m[1][3];
                pxWorld.column2.x = mat.m[2][0];
                pxWorld.column2.y = mat.m[2][1];
                pxWorld.column2.z = mat.m[2][2];
                pxWorld.column2.w = mat.m[2][3];
                pxWorld.column3.x = mat.m[3][0];
                pxWorld.column3.y = mat.m[3][1];
                pxWorld.column3.z = mat.m[3][2];
                pxWorld.column3.w = mat.m[3][3];

                PxTransform transform{ pxWorld };

                m_pPhysXCollider[i]->Get_Static_RigidBody()->setGlobalPose(transform);
            }

            _float4x4 fMat{};
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);
        }
        
        Update_MagnetMatrix();

        m_pAttachedMagnet->Tick(fTimeDelta);
    }

    Update_Animation();
    m_pModelCom->PlayReservedCallbacks();
}

void CSteelFrame_Platform::LateTick(_float fTimeDelta)
{
    //if (m_eState == IDLE_READY) return;
    
    if (m_bBegin)
    {
        //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

        auto pGI = m_pGameInstance.lock();
        pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
        pGI->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

        m_pAttachedMagnet->LateTick(fTimeDelta);
    }
}

HRESULT CSteelFrame_Platform::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        m_pShaderCom->Begin(0);
        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CSteelFrame_Platform::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (m_eState == IDLE_FIRST && pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        Execute();
    }
}

void CSteelFrame_Platform::Edit_Attribute()
{
    _float fPos[3] = {}, fRot[3] = {}, fScl[3] = {};
    ImGuizmo::DecomposeMatrixToComponents((_float*)&m_ColliderOffset, fPos, fRot, fScl);

    if (ImGui::DragFloat3("Collider Pos Offset", fPos))
        m_ColliderOffset._41 = fPos[0]; m_ColliderOffset._42 = fPos[1]; m_ColliderOffset._43 = fPos[2];

    if (ImGui::DragFloat3("Collider Scl Offset", fScl))
    {
        ImGuizmo::RecomposeMatrixFromComponents(fPos, fRot, fScl, (_float*)&m_ColliderOffset);
    }
}

void CSteelFrame_Platform::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    write_typed_data(os, m_ColliderOffset);
}

void CSteelFrame_Platform::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    read_typed_data(is, m_ColliderOffset);

    m_pColliderCom->Update(XMLoadFloat4x4(&m_ColliderOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
}

HRESULT CSteelFrame_Platform::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SteelFrame_Platform"), TEXT("Model_Com")));

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vCenter = { 0.f, 0.f, 0.f };
    aabbDesc.vExtents = { 1.f, 1.f, 1.f };
    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

    m_pPhysXCollider[COL_A] = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider1")));
    m_pPhysXCollider[COL_B] = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider2")));
    m_pPhysXCollider[COL_C] = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider3")));
    m_pPhysXCollider[COL_D] = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider4")));

    auto ColModel = GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SteelFrame_Platform_Col"));

    auto pMesh = static_pointer_cast<CModel>(ColModel)->Get_Mesh(0);
    _float3 vScale = m_pTransformCom->Get_Scale();
    _float4x4 fScaleMat = {};
    XMStoreFloat4x4(&fScaleMat, XMMatrixScaling(vScale.x, vScale.y, vScale.z));
    pMesh->Cooking_TriangleMesh(fScaleMat);
    auto pTriangleMesh = pMesh->Get_TriangleMesh();
    if (pTriangleMesh && vScale.x && vScale.y && vScale.z)
    {
        for (size_t i = 0; i < COL_END; ++i)
        {
            string tmp = string(CSteelFrame_Platform::ObjID.begin(), CSteelFrame_Platform::ObjID.end()) + to_string(i);
            m_pPhysXCollider[i]->createStaticMeshActor(pTriangleMesh, tmp.c_str());

            m_pPhysXCollider[i]->Get_Static_RigidBody()->setGlobalPose(PxTransform{PxVec3{-100000.f,-100000.f + rand() % 100000,-100000.f}});
        }
    }

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider || !m_pColliderCom)
        assert(false);

    m_pBoneMatrix[COL_A] = m_pModelCom->Get_BoneMatrixPtr("steelframeA_03");
    m_pBoneMatrix[COL_B] = m_pModelCom->Get_BoneMatrixPtr("steelframeB_03");
    m_pBoneMatrix[COL_C] = m_pModelCom->Get_BoneMatrixPtr("steelframeC_03");
    m_pBoneMatrix[COL_D] = m_pModelCom->Get_BoneMatrixPtr("steelframeD_03");

    return S_OK;
}

void CSteelFrame_Platform::Ready_Parts()
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

    CGimmickBase::COMMONOBJ_DESC commonDesc{};
    commonDesc.fRotationPerSec = 0.f;
    commonDesc.fSpeedPerSec = 0.f;
    commonDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    //commonDesc.worldMatrix = Identity;
    XMStoreFloat4x4(&commonDesc.worldMatrix, XMMatrixRotationX(XMConvertToRadians(180.f)));
    commonDesc.worldMatrix._41 = -0.5f;
    commonDesc.worldMatrix._42 = 1.5f;

    //auto pObject = m_pGameInstance.lock()->Add_Clone_Return(iLevelIndex, TEXT("Layer_Map"), CBuiltIn_Magnet::ObjID, &commonDesc);
    auto pPrototype = m_pGameInstance.lock()->Get_Object_Prototype(CBuiltIn_Magnet::ObjID);
    auto pObject = pPrototype->Clone(&commonDesc);
    assert(pObject);

    m_pAttachedMagnet = static_pointer_cast<CBuiltIn_Magnet>(pObject);
    m_MagnetLocalMat = commonDesc.worldMatrix;

    m_pAttachedMagnet->Set_MagnetAttribute(true, 0);

    Update_MagnetMatrix();
}

HRESULT CSteelFrame_Platform::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

void CSteelFrame_Platform::Update_Animation()
{
    switch (m_eState)
    {
    case Client::CSteelFrame_Platform::IDLE_READY:
        if (m_bExecuted) {
            m_eState = ANIM_FIRST;
            m_pModelCom->Set_AnimIndex((_uint)m_eState, false);
            m_bBegin = true;
            Update_MagnetMatrix();
        }
        break;
    case Client::CSteelFrame_Platform::ANIM_FIRST:
        if (m_pModelCom->Get_Finished()) {
            m_eState = IDLE_FIRST;
            m_pModelCom->Set_AnimIndex((_uint)m_eState, false);
            m_bExecuted = false;
            m_pAttachedMagnet->Execute();
        }
        break;
    case Client::CSteelFrame_Platform::IDLE_FIRST:
        if (m_bExecuted) {
            m_eState = ANIM_LEFTOVER;
            m_pModelCom->Set_AnimIndex((_uint)m_eState, false);
        }
        break;
    case Client::CSteelFrame_Platform::ANIM_LEFTOVER:
        if (m_pModelCom->Get_Finished()) {
            m_eState = IDLE_DONE;
            m_pModelCom->Set_AnimIndex((_uint)m_eState, false);
            Done();
        }
        break;
    case Client::CSteelFrame_Platform::IDLE_DONE:
        break;
    }
}

void CSteelFrame_Platform::Update_MagnetMatrix()
{
    _matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix[COL_A]);
    BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
    BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
    BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

    //auto pGI = m_pGameInstance.lock();
    //if (pGI->Get_KeyDown(DIK_UP))
    //    m_MagnetLocalMat._42 += 0.5f;
    //if (pGI->Get_KeyDown(DIK_DOWN))
    //    m_MagnetLocalMat._42 -= 0.5f;
    //if (pGI->Get_KeyDown(DIK_LEFT))
    //{
    //    if (pGI->Get_KeyPressing(DIK_LSHIFT))
    //        m_MagnetLocalMat._43 -= 0.5f;
    //    else
    //        m_MagnetLocalMat._41 -= 0.5f;
    //}
    //if (pGI->Get_KeyDown(DIK_RIGHT))
    //{
    //    if (pGI->Get_KeyPressing(DIK_LSHIFT))
    //        m_MagnetLocalMat._43 += 0.5f;
    //    else
    //        m_MagnetLocalMat._41 += 0.5f;
    //}

    _matrix MagnetMatrix = XMLoadFloat4x4(&m_MagnetLocalMat) * BoneMatrix * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pAttachedMagnet->Get_Transform()->Set_WorldMatrix(MagnetMatrix);
}

shared_ptr<CSteelFrame_Platform> CSteelFrame_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CSteelFrame_Platform
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSteelFrame_Platform(pDevice, pContext)
        {}
    };

    shared_ptr<CSteelFrame_Platform> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSteelFrame_Platform"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CSteelFrame_Platform::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CSteelFrame_Platform
    {
        MakeSharedEnabler(const CSteelFrame_Platform& rhs) : CSteelFrame_Platform(rhs)
        {}
    };

    shared_ptr<CSteelFrame_Platform> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSteelFrame_Platform"));
        assert(false);
    }

    return pInstance;
}

void CSteelFrame_Platform::Free()
{
}
