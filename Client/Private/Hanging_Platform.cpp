#include "Hanging_Platform.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "Mesh.h"

#include "CharacterController.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "MapManager.h"

wstring CHanging_Platform::ObjID = TEXT("CHanging_Platform");
CHanging_Platform::CHanging_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CHanging_Platform::CHanging_Platform(const CHanging_Platform& rhs)
    : CGimmickBase{rhs}
{
}

CHanging_Platform::~CHanging_Platform()
{
    Free();
}

HRESULT CHanging_Platform::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Hanging_Platform"), TEXT("../Bin/Resources/Models/Map/Gimmick/Hanging_Platform.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    return S_OK;
}

HRESULT CHanging_Platform::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    pDesc->fSpeedPerSec = 40.f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimIndex(1, true);

    return S_OK;
}

void CHanging_Platform::PriorityTick(_float fTimeDelta)
{
}

void CHanging_Platform::Tick(_float fTimeDelta)
{
    //if (GAMEINSTANCE->Get_KeyDown(DIK_L))
    //{
    //    m_bExecuted = true;
    //    m_pModelCom->Set_AnimIndex(0, false);
    //}

    if (m_bExecuted && m_pModelCom->Get_Finished())
    {
        m_bExecuted = false;
        m_bDone = true;
    }

    _float4x4 fMat{};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);

    _float4x4 fOffsetMat = *m_pFloorMatrix;
    fOffsetMat._42 += 31.85f;

    _matrix Transpose = XMLoadFloat4x4(&fOffsetMat) * m_pTransformCom->Get_WorldMatrix_XMMat();
    XMStoreFloat4x4(&fMat, Transpose);
    
    //Transpose = XMMatrixMultiply(Transpose, XMMatrixTranslation(0.f, -m_fOffsetY, 0.f));
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
    for (auto pRigid : m_vecPxRigid)
        pRigid->setGlobalPose(transform);

    if (m_bExecuted && m_bCollision)
    {
        auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));

        _float3 vVelocity = pCCT->Get_Velocity();
        
        if (vVelocity.y <= 0)
        {
            auto vPlayerPos = pCCT->Get_Controller()->getPosition();

            _matrix BoneMat = XMLoadFloat4x4(m_pFloorMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
            _matrix PlayerMat = XMLoadFloat4x4(&m_PlayerLocal) * BoneMat;
            _vector vPos = PlayerMat.r[3];
            _float fTopY = XMVectorGetY(vPos);

            _vector vFootPos = XMVectorSet(vPlayerPos.x, fTopY + 0.101f, vPlayerPos.z, 1.f);

            //pCCT->Set_Position(XMMatrixTranslationFromVector(vFootPos));
            pCCT->Set_FootPosition(XMMatrixTranslationFromVector(vFootPos));
            dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Transform()->Set_Position(vFootPos);
        }
    }

    m_pColliderCom->Update(XMLoadFloat4x4(m_pFloorMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CHanging_Platform::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;
    
    auto pGI = m_pGameInstance.lock();
    //if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f)) return;

    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    pGI->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CHanging_Platform::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        //	return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //	return E_FAIL;
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

void CHanging_Platform::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CHanging_Platform::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        if (!m_bExecuted && !m_bDone)
        {
            auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));
            auto vPlayerPos = pCCT->Get_Controller()->getFootPosition();

            _matrix BoneMat = XMLoadFloat4x4(m_pFloorMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
            _vector vBonePos = BoneMat.r[3];
            
            if (fabs(vPlayerPos.y - XMVectorGetY(vBonePos)) < 0.2f)
            {
                PLAYERMANAGER->Callback_Event("Hanging_Platform");
                Execute();
                m_pModelCom->Set_AnimIndex(0, false);

                _vector xmPlayerPos = XMVectorSet(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z, 1.f);
                _matrix xmPlayerLocal = XMMatrixTranslationFromVector(XMVector3TransformCoord(xmPlayerPos, XMMatrixInverse(nullptr, BoneMat)));
                XMStoreFloat4x4(&m_PlayerLocal, xmPlayerLocal);
            }
        }

        m_bCollision = true;
    }
}

void CHanging_Platform::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        m_bCollision = false;
    }
}

HRESULT CHanging_Platform::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hanging_Platform"), TEXT("Model_Com")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

    _float3 vWorldSize{};
    _matrix matWorld = m_pTransformCom->Get_WorldMatrix_XMMat();
    vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
    vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
    vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));

    _matrix scaleMatrix = XMMatrixScaling(vWorldSize.x, vWorldSize.y, vWorldSize.z);
    _float4x4 fScaleMat{};
    XMStoreFloat4x4(&fScaleMat, scaleMatrix);
    PxVec3 pscale = { vWorldSize.x, vWorldSize.y, vWorldSize.z };
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    string strTag = string(CHanging_Platform::ObjID.begin(), CHanging_Platform::ObjID.end());
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        string tmp = strTag + string("_") + to_string(m_iObjectID) + string("_") + to_string(i);
        auto pMesh = m_pModelCom->Get_Mesh((_uint)i);
        pMesh->Cooking_TriangleMesh(fScaleMat);
        auto pTriangleMesh = pMesh->Get_TriangleMesh();
        if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
        {
            auto pRigid = m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
            if (pRigid)
                m_vecPxRigid.emplace_back(pRigid);
        }
    }

    m_pFloorMatrix = m_pModelCom->Get_BoneMatrixPtr("lift_010");
    
    //_matrix floorMat = XMLoadFloat4x4(m_pFloorMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
    //_float4x4 fMat{};
    //XMStoreFloat4x4(&fMat, floorMat);
    m_fOffsetY = m_pModelCom->Get_BoneBasicMatrixPtr("lift_010")->_42;

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = { 0.f, 0.f, -5.f };
    obbDesc.vExtents = { 3.5f, 5.f, 2.4f };
    obbDesc.vRotation = { XMConvertToRadians(90.f), 0.f, 0.f};

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

    assert(m_pColliderCom);

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
        assert(false);

    return S_OK;
}

HRESULT CHanging_Platform::Bind_ShaderResources()
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

shared_ptr<CHanging_Platform> CHanging_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CHanging_Platform
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CHanging_Platform(pDevice, pContext)
        {}
    };

    shared_ptr<CHanging_Platform> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHanging_Platform"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CHanging_Platform::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CHanging_Platform
    {
        MakeSharedEnabler(const CHanging_Platform& rhs) : CHanging_Platform(rhs)
        {}
    };

    shared_ptr<CHanging_Platform> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHanging_Platform"));
        assert(false);
    }

    return pInstance;
}

void CHanging_Platform::Free()
{
    for (auto& Rigid : m_vecPxRigid)
    {
        if (Rigid != nullptr)
        {
            auto Scene = Rigid->getScene();
            if (Scene)
                Scene->removeActor(*Rigid);
        }
    }
    m_vecPxRigid.clear();

}
