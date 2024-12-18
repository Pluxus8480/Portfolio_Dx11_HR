#include "Rekka_Pipe_Door.h"
#include "GameInstance.h"

#include "Shader.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "OctComp.h"

#include "Effect_Manager.h"


wstring CRekka_Pipe_Door::ObjID = TEXT("CRekka_Pipe_Door");

CRekka_Pipe_Door::CRekka_Pipe_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CRekka_Pipe_Door::CRekka_Pipe_Door(const CRekka_Pipe_Door& rhs)
    : CGameObject(rhs)
{
}

CRekka_Pipe_Door::~CRekka_Pipe_Door()
{
    Free();
}

HRESULT CRekka_Pipe_Door::Initialize_Prototype()
{
    _float4x4 FMat = {};
    XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(
            TEXT("Model_Pipe_Door"),
            TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_PipeShutter_SH_01.FBX"),
            MODEL_TYPE::NONANIM, FMat)
    );

    return S_OK;
}

HRESULT CRekka_Pipe_Door::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
    m_pTransformCom->Set_Speed(1.f);

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CRekka_Pipe_Door::PriorityTick(_float fTimeDelta)
{
}

void CRekka_Pipe_Door::Tick(_float fTimeDelta)
{
    if (m_fTimer < 0.f)
        Dead();

    if (m_doingOpening)
    {
        m_doingOpening = false;

        _vector vCurPos = m_pTransformCom->Get_Position();
        vCurPos = XMVectorSetY(vCurPos, 493.057f);
        m_pTransformCom->Set_Position(vCurPos);
    }
}

void CRekka_Pipe_Door::LateTick(_float fTimeDelta)
{
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_TERRAIN, shared_from_this());
}

HRESULT CRekka_Pipe_Door::Render()
{
    if (!m_pModelCom)
        return S_OK;

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CRekka_Pipe_Door::Ready_Components(const MISCOBJ_DESC* pDesc)
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Pipe_Door"), TEXT("Com_Model")));
    if (!m_pModelCom || !m_pShaderCom)
        assert(false);

    _uint    iNumMeshes = m_pModelCom->Get_NumMeshes();
    string strTag = string(CRekka_Pipe_Door::ObjID.begin(), CRekka_Pipe_Door::ObjID.end());
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        string tmp = strTag + string("_") + to_string(i);
        auto pMesh = m_pModelCom->Get_Mesh((_uint)i);
        _matrix mat = XMLoadFloat4x4(&pDesc->worldMatrix);
        _vector vScale, vRot, vPos;
        XMMatrixDecompose(&vScale, &vRot, &vPos, mat);
        _float3 scale;
        XMStoreFloat3(&scale, vScale);
        PxVec3 pscale;
        pscale.x = scale.x;
        pscale.y = scale.y;
        pscale.z = scale.z;
        pMesh->Cooking_TriangleMesh(pDesc->worldMatrix);
        auto pTriangleMesh = pMesh->Get_TriangleMesh();
        if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
            m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
    }

    if (!m_pPhysXCollider)
        assert(false);

    return S_OK;
}

void CRekka_Pipe_Door::OpenDoor()
{
    // After activing open sequence
    m_pPhysXCollider->Remove_To_Scene();
    m_doingOpening = true;
}

shared_ptr<CRekka_Pipe_Door> CRekka_Pipe_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRekka_Pipe_Door
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Pipe_Door(pDevice, pContext) {}
    };

    shared_ptr<CRekka_Pipe_Door> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRekka_Pipe_Door"));
        assert(false);
    }
    return pInstance;
}

shared_ptr<CGameObject> CRekka_Pipe_Door::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRekka_Pipe_Door
    {
        MakeSharedEnabler(const CRekka_Pipe_Door& rhs) : CRekka_Pipe_Door(rhs) {}
    };

    shared_ptr<CRekka_Pipe_Door> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRekka_Pipe_Door"));
        assert(false);
    }
    return pInstance;
}

void CRekka_Pipe_Door::Free()
{
}
