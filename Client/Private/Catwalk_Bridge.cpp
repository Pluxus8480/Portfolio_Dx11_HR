#include "Catwalk_Bridge.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"

#include "Beat_Manager.h"
#include "MapManager.h"

wstring CCatwalk_Bridge::ObjID = TEXT("CCatwalk_Bridge");
CCatwalk_Bridge::CCatwalk_Bridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CCatwalk_Bridge::CCatwalk_Bridge(const CCatwalk_Bridge& rhs)
    : CGimmickBase{rhs}
{
}

CCatwalk_Bridge::~CCatwalk_Bridge()
{
    Free();
}

void CCatwalk_Bridge::Execute(_bool bExecute)
{
    m_bExecuted = bExecute;
    m_bDone = true;

    m_pModelCom->Set_AnimIndex(0, false);

    m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
}

HRESULT CCatwalk_Bridge::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Catwalk_Bridge"), TEXT("../Bin/Resources/Models/Map/Gimmick/Catwalk_Bridge.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    return S_OK;
}

HRESULT CCatwalk_Bridge::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_pModelCom->Set_AnimIndex(1, false);
    
    //_float f8Beat = CBeat_Manager::Get_Instance()->Get_Beat() * 8.f;
    //m_fAnimSpeed = m_pModelCom->Get_Animation_PlayTime(0) / f8Beat;

    return S_OK;
}

void CCatwalk_Bridge::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat() * 8.f;
        _float fPlayTime = m_pModelCom->Get_Animation_PlayTime(0);
        m_fAnimSpeed = fPlayTime / fBeat;

        _float4x4 Mat{};
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, Mat);

        m_bInit = true;
    }
}

void CCatwalk_Bridge::Tick(_float fTimeDelta)
{
    //if (GAMEINSTANCE->Get_KeyDown(DIK_L))
    //    Execute();

    if (m_bExecuted)
    {
        if (!m_bAnimStart)
        {
            _uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
            if (m_iPrevBeat != iCurBeat)
                m_bAnimStart = true;
        }

        if (m_pModelCom->Get_Finished())
        {
            m_bDone = true;
            m_bExecuted = false;
        }
    }

    if (m_bDone)
    {
        _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
        Transpose = XMMatrixMultiply(XMMatrixTranslation(9.7f, -0.2f, 0.f), Transpose);
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
        m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
    }

    else
    {
        m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 100000,-100000.f} });
    }

    if (m_bAnimStart)
    {
        _float4x4 Mat{};
        m_pModelCom->Play_Animation(fTimeDelta, m_fAnimSpeed * fTimeDelta, 0.1f, Mat);
    }
}

void CCatwalk_Bridge::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f))
        return;

    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CCatwalk_Bridge::Render()
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

    return S_OK;
}

HRESULT CCatwalk_Bridge::Ready_Components(COMMONOBJ_DESC* pDesc)
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Catwalk_Bridge"), TEXT("Model_Com")));
    
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysXCollider_Com")));
    
    PxVec3 BoxDimension{ 9.7f, 0.2f, 2.f };
    PxBoxGeometry Box{ BoxDimension };
    string ColliderTag = m_strGimmickID;
    m_pPhysXCollider->createStaticShapeActor(Box, ColliderTag.c_str());

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
        assert(false);

    return S_OK;
}

HRESULT CCatwalk_Bridge::Bind_ShaderResources()
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

shared_ptr<CCatwalk_Bridge> CCatwalk_Bridge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CCatwalk_Bridge
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCatwalk_Bridge(pDevice, pContext)
        {}
    };

    shared_ptr<CCatwalk_Bridge> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCatwalk_Bridge"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CCatwalk_Bridge::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CCatwalk_Bridge
    {
        MakeSharedEnabler(const CCatwalk_Bridge& rhs) : CCatwalk_Bridge(rhs)
        {}
    };

    shared_ptr<CCatwalk_Bridge> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCatwalk_Bridge"));
        assert(false);
    }

    return pInstance;
}

void CCatwalk_Bridge::Free()
{
}
