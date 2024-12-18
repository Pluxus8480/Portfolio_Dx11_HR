#include "LevelUp_Machine.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "CUi_StoreSystem.h"
#include "CUi_Mgr.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "MapManager.h"

wstring CLevelUp_Machine::ObjID = TEXT("CLevelUp_Machine");
CLevelUp_Machine::CLevelUp_Machine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CLevelUp_Machine::CLevelUp_Machine(const CLevelUp_Machine& rhs)
    : CGimmickBase{rhs}
{
}

CLevelUp_Machine::~CLevelUp_Machine()
{
    Free();
}

HRESULT CLevelUp_Machine::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LevelUp_Machine"), TEXT("../Bin/Resources/Models/Map/NPC/LevelUp_Machine/LevelUp_Machine.FBX"), MODEL_TYPE::ANIM, fMat)
    );

    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LevelUp_Machine_Col"), TEXT("../Bin/Resources/Models/Map/NPC/LevelUp_Machine/LevelUp_Machine_Col.FBX"), MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_LevelUp_Machine"), TEXT("../Bin/Resources/Models/Map/NPC/LevelUp_Machine/T_DMG_LevelUpMachine_2.dds"))
    );

    return S_OK;
}

HRESULT CLevelUp_Machine::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pModelCom->Set_AnimIndex(5, true);

    Execute();

    return S_OK;
}

void CLevelUp_Machine::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat() * 0.5f;
        m_fSecPerFrame = fBeat / 16.f;

        m_bInit = true;
    }
}

void CLevelUp_Machine::Tick(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    if (m_fTime >= m_fSecPerFrame)
    {
        m_fTime -= m_fTime;

        ++m_iTextureIndex;
        m_iTextureIndex %= m_iMaxIndex;
    }

    _float4x4 fMat{};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);

    Animation_End();
    Update_Animation();
}

void CLevelUp_Machine::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CLevelUp_Machine::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();    
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        _bool bDisplay = { false };
        if (i == 8)
        {
            if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iTextureIndex, sizeof(_int))))
                return E_FAIL;
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
                return E_FAIL;
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
                return E_FAIL;

            bDisplay = true;
        }
        else
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                continue;
        }

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        if(bDisplay)
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::EmissionFrame);
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CLevelUp_Machine::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        PLAYERMANAGER->Callback_Event("Shopping_Zone");
        m_eState = READY_SHOP;
        m_pModelCom->Set_AnimIndex(3, false);
    }
}

void CLevelUp_Machine::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        if (GAMEINSTANCE->Get_KeyDown(DIK_E) && CUi_Mgr::Get_Instance()->Find_UI(CUi_StoreSystem::ObjID) == nullptr)
        {
            const _float4x4  boneMat = *m_pModelCom->Get_BoneMatrixPtr("prop_00");
           _matrix mBoneMat = XMLoadFloat4x4(& boneMat) * m_pTransformCom->Get_WorldMatrix_XMMat();

           _vector bonePos = mBoneMat.r[3];
            static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Set_Player_Position(bonePos);

            PLAYERMANAGER->Get_Player()->Get_Transform()->LookAt_Horizontal(bonePos - m_pTransformCom->Get_NormLook());
            PLAYERMANAGER->Callback_Event("Shopping_Start");

            m_pModelCom->Set_AnimIndex(1, false);
        }
    }
}

void CLevelUp_Machine::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Set_Interaction(false);

        m_eState = SHOP_END;
        m_pModelCom->Set_AnimIndex(4, false);
    }
}

HRESULT CLevelUp_Machine::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LevelUp_Machine"), TEXT("Model_Com")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

    auto ColModel = GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LevelUp_Machine_Col"));
    
    auto pMesh = static_pointer_cast<CModel>(ColModel)->Get_Mesh(0);
    _float3 vScale = m_pTransformCom->Get_Scale();
    _float4x4 fScaleMat = {};
    XMStoreFloat4x4(&fScaleMat, XMMatrixScaling(vScale.x, vScale.y, vScale.z));
    pMesh->Cooking_TriangleMesh(fScaleMat);
    auto pTriangleMesh = pMesh->Get_TriangleMesh();
    if (pTriangleMesh && vScale.x && vScale.y && vScale.z)
    {
        string tmp = string(CLevelUp_Machine::ObjID.begin(), CLevelUp_Machine::ObjID.end());
        m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
    }

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vExtents = { 2.f, 2.f, 2.f };
    obbDesc.vCenter = { 0.f, 2.f, 2.f };
    obbDesc.vRotation = { 0.f, 0.f, 0.f };
    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

    m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUp_Machine"), TEXT("Texture_Com")));

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider || !m_pColliderCom || !m_pTextureCom)
        assert(false);

    m_pRootMat = m_pModelCom->Get_BoneMatrixPtr("RootNode");
    m_pModelCom->Set_AnimIndex(5, true);
    _float4x4 fMat{};
    m_pModelCom->Play_Animation(0.016f, 0.016f, 0.1f, fMat);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
    
    _matrix WorldMat = XMLoadFloat4x4(m_pRootMat) * m_pTransformCom->Get_WorldMatrix_XMMat();
    WorldMat.r[0] = XMVector3Normalize(WorldMat.r[0]);
    WorldMat.r[1] = XMVector3Normalize(WorldMat.r[1]);
    WorldMat.r[2] = XMVector3Normalize(WorldMat.r[2]);
    _float4x4 mat;
    XMStoreFloat4x4(&mat, WorldMat);
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
    pxWorld.column3.z = mat.m[3][2] - 0.5f;
    pxWorld.column3.w = mat.m[3][3];

    PxTransform transform{ pxWorld };
    m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

    return S_OK;
}

HRESULT CLevelUp_Machine::Bind_ShaderResources()
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

    XMINT2 vDivide = { 8, 8 };
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &vDivide, sizeof(XMINT2))))
        return E_FAIL;

    return S_OK;
}

void CLevelUp_Machine::Update_Animation()
{
    switch (m_eState)
    {
    case Client::CLevelUp_Machine::IDLE:
        m_pModelCom->Set_AnimIndex(5, true);
        
        break;
    case Client::CLevelUp_Machine::READY_SHOP: {
        if (m_pModelCom->Get_Finished())
            m_eState = SHOPPING;
        break;
    }
    case Client::CLevelUp_Machine::SHOPPING:
        m_pModelCom->Set_AnimIndex(6, true);
        
        break;
    case Client::CLevelUp_Machine::SHOP_END: {
        if (m_pModelCom->Get_Finished())
            m_eState = IDLE;  
        break;
    }
    }
}

void CLevelUp_Machine::Animation_End()
{
    if ((m_pModelCom->Get_CurrentAnimIndex() == 3
        || m_pModelCom->Get_CurrentAnimIndex() == 1)
        && m_pModelCom->Get_Finished())
    {
        m_pModelCom->Set_AnimIndex(0,true);
    }
}

shared_ptr<CLevelUp_Machine> CLevelUp_Machine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CLevelUp_Machine
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevelUp_Machine(pDevice, pContext)
        {}
    };

    shared_ptr<CLevelUp_Machine> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLevelUp_Machine"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CLevelUp_Machine::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CLevelUp_Machine
    {
        MakeSharedEnabler(const CLevelUp_Machine& rhs) : CLevelUp_Machine(rhs)
        {}
    };

    shared_ptr<CLevelUp_Machine> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLevelUp_Machine"));
        assert(false);
    }

    return pInstance;
}

void CLevelUp_Machine::Free()
{
}
