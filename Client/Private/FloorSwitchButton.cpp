#include "FloorSwitchButton.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "Mesh.h"
#include "PhysxCollider.h"

wstring CFloorSwitchButton::ObjID = TEXT("CFloorSwitchButton");
CFloorSwitchButton::CFloorSwitchButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{pDevice, pContext}
{
}

CFloorSwitchButton::CFloorSwitchButton(const CFloorSwitchButton& rhs)
    : CPartObject{rhs}
{
}

CFloorSwitchButton::~CFloorSwitchButton()
{
    Free();
}

HRESULT CFloorSwitchButton::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_FloorSwitch_On"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_FloorSwitchA_On_IZ_01.FBX"),
            MODEL_TYPE::NONANIM, Identity)
    );

    return S_OK;
}

HRESULT CFloorSwitchButton::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CFloorSwitchButton::PriorityTick(_float fTimeDelta)
{
}

void CFloorSwitchButton::Tick(_float fTimeDelta)
{
    SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());

    if (m_bOn)
    {
        _matrix Transpose = XMLoadFloat4x4(&m_WorldMatrix);
        //Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
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
    }

    else
    {
        for (auto pRigid : m_vecPxRigid)
            pRigid->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 300,-100000.f} });
    }
}

void CFloorSwitchButton::LateTick(_float fTimeDelta)
{
    if (!m_bOn)
        return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    //GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CFloorSwitchButton::Render()
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
        m_pShaderCom->Begin(0);

        m_pModelCom->Render((_uint)i);
    }

//#ifdef _DEBUG
//    if (m_pColliderCom)
//        m_pColliderCom->Render();
//#endif

    return S_OK;
}

HRESULT CFloorSwitchButton::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FloorSwitch_On"), TEXT("Model_Com")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

    SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
    _float3 vWorldSize{};
    _matrix matWorld = XMLoadFloat4x4(&m_WorldMatrix);
    vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
    vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
    vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));
    //m_vModelSize.x *= vWorldSize.x;
    //m_vModelSize.y *= vWorldSize.y;
    //m_vModelSize.z *= vWorldSize.z;
    //m_vModelCenter.x *= vWorldSize.x;
    //m_vModelCenter.y *= vWorldSize.y;
    //m_vModelCenter.z *= vWorldSize.z;
    //m_vModelSize = { m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
    ////XMStoreFloat3(&m_vModelSize, XMVector3Dot(XMLoadFloat3(&m_vModelSize), XMLoadFloat3(&vWorldSize)));
    ////XMStoreFloat3(&m_vModelCenter, XMVector3TransformCoord(XMLoadFloat3(&m_vModelCenter), XMLoadFloat4x4(&m_WorldMatrix)));
    //PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
    //PxBoxGeometry Box{ BoxDimension };
    //m_pPhysXCollider->createStaticShapeActor(Box, "Test");

    _matrix scaleMatrix = XMMatrixScaling(vWorldSize.x, vWorldSize.y, vWorldSize.z);
    _float4x4 fScaleMat{};
    XMStoreFloat4x4(&fScaleMat, scaleMatrix);
    PxVec3 pscale = { vWorldSize.x, vWorldSize.y, vWorldSize.z };
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    string strTag = string(CFloorSwitchButton::ObjID.begin(), CFloorSwitchButton::ObjID.end());
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

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

HRESULT CFloorSwitchButton::Bind_ShaderResources()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    //	return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CFloorSwitchButton> CFloorSwitchButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CFloorSwitchButton
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFloorSwitchButton(pDevice, pContext)
        {}
    };

    shared_ptr<CFloorSwitchButton> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFloorSwitchButton"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CFloorSwitchButton::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CFloorSwitchButton
    {
        MakeSharedEnabler(const CFloorSwitchButton& rhs) : CFloorSwitchButton(rhs)
        {}
    };

    shared_ptr<CFloorSwitchButton> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFloorSwitchButton"));
        assert(false);
    }

    return pInstance;
}

void CFloorSwitchButton::Free()
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
