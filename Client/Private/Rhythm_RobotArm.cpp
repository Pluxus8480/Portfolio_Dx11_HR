#include "Rhythm_RobotArm.h"
#include "Misc_Interface.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Effect_Manager.h"

wstring CRhythm_RobotArm::ObjID = TEXT("CRhythm_RobotArm");
CRhythm_RobotArm::CRhythm_RobotArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CRhythm_RobotArm::CRhythm_RobotArm(const CRhythm_RobotArm& rhs)
    : CGameObject{rhs}
{
}

CRhythm_RobotArm::~CRhythm_RobotArm()
{
    Free();
}

HRESULT CRhythm_RobotArm::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RobotArm"), TEXT("../Bin/Resources/Models/Map/AnimObject/SK_bg0010.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    return S_OK;
}

HRESULT CRhythm_RobotArm::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_vecAnimSpeed[3]);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_vecAnimSpeed[3]);
        }
    );

    m_pModelCom->Set_AnimIndex(1, false);

    return S_OK;
}

void CRhythm_RobotArm::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

        m_vecAnimSpeed[0] = m_pModelCom->Get_Animation_PlayTime(0) / (fBeat * 2.f);
        m_vecAnimSpeed[1] = m_pModelCom->Get_Animation_PlayTime(1) / (fBeat * 1.5f);
        m_vecAnimSpeed[2] = m_pModelCom->Get_Animation_PlayTime(2) / (fBeat * 1.5f);
        m_vecAnimSpeed[3] = m_pModelCom->Get_Animation_PlayTime(3) / (fBeat * 2.f);
        m_vecAnimSpeed[4] = m_pModelCom->Get_Animation_PlayTime(4) / (fBeat * 2.f);

        m_bInit = true;
    }
}

void CRhythm_RobotArm::Tick(_float fTimeDelta)
{
    _float4x4 fMat{};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_vecAnimSpeed[m_vecAnimIndex[m_iCurIndex]], 0.1f, fMat);

    if (m_pModelCom->Get_Finished())
    {
        ++m_iCurIndex;
        m_iCurIndex %= m_vecAnimIndex.size();

        m_pModelCom->Set_AnimIndex(m_vecAnimIndex[m_iCurIndex], false);
    }

    m_pModelCom->PlayReservedCallbacks();
}

void CRhythm_RobotArm::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f)) return;

    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRhythm_RobotArm::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        m_pShaderCom->Begin((_uint)MESH_PASS::Default);
        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

HRESULT CRhythm_RobotArm::Ready_Components()
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();
 
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Model_RobotArm"), TEXT("Com_Model")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhysxCollider")));

    string strObjTag = string(ObjID.begin(), ObjID.end());
    _float3 m_vModelSize = { 0.6f, 1.5f, 1.f };
    _float3 m_vModelCenter = { 0.f, 1.5f, 0.5f };
    PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
    PxBoxGeometry Box{ BoxDimension };
    auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, strObjTag.c_str());
    assert(pRigidBody);

    _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
    Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
    //Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
    //Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
    //Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
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
    pRigidBody->setGlobalPose(transform);

#ifdef _DEBUG
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vExtents = m_vModelSize;
    aabbDesc.vCenter = m_vModelCenter;

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));
    assert(m_pColliderCom);
#endif

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
        assert(false);

    m_vecAnimSpeed.resize(m_pModelCom->Get_NumAnimation());
    m_vecAnimIndex.assign({4, 2, 3, 1});

    return S_OK;
}

HRESULT CRhythm_RobotArm::Bind_ShaderResources()
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

shared_ptr<CRhythm_RobotArm> CRhythm_RobotArm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRhythm_RobotArm
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRhythm_RobotArm(pDevice, pContext)
        {}
    };

    shared_ptr<CRhythm_RobotArm> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRhythm_RobotArm"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CRhythm_RobotArm::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRhythm_RobotArm
    {
        MakeSharedEnabler(const CRhythm_RobotArm& rhs) : CRhythm_RobotArm(rhs)
        {}
    };

    shared_ptr<CRhythm_RobotArm> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRhythm_RobotArm"));
        assert(false);
    }

    return pInstance;
}

void CRhythm_RobotArm::Free()
{
}
