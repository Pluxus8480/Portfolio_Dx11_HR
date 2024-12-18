#include "Button_Platform.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"
#include "Math_Manager.h"
#include "GameManager.h"
#include "MapManager.h"

wstring CButton_Platform::ObjID = TEXT("CButton_Platform");

CButton_Platform::CButton_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CButton_Platform::CButton_Platform(const CButton_Platform& rhs)
    : CGimmickBase{rhs}
{
}

CButton_Platform::~CButton_Platform()
{
    Free();
}

HRESULT CButton_Platform::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Button_Platform"), TEXT("../Bin/Resources/Models/Map/Gimmick/Platform_Cylinder.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Platform_Land"), TEXT("../Bin/Resources/Sounds/MapObject/Platform_Land.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("ButtonPlatformTemp"), TEXT("../Bin/Resources/Sounds/MapObject/ButtonPlatformTemp.ogg")));

    
    return S_OK;
}

HRESULT CButton_Platform::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    pDesc->fSpeedPerSec = 40.f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;
    
    m_vFinalPosition = _float3{ pDesc->worldMatrix._41, pDesc->worldMatrix._42, pDesc->worldMatrix._43 };
    m_vInitialPosition = _float3{ m_vFinalPosition.x, m_vFinalPosition.y - 20.f, m_vFinalPosition.z };
    
    m_pModelCom->Set_AnimIndex(0, true);
    
    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        }
    );

    return S_OK;
}

void CButton_Platform::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit && CGame_Manager::Get_Instance()->Get_Current_Level() != LEVEL_MAPTOOL)
    {
        m_pTransformCom->Set_Position(XMLoadFloat3(&m_vInitialPosition));

        m_bInit = true;
    }
}

void CButton_Platform::Tick(_float fTimeDelta)
{
    if (m_bExecuted && !m_bDone)
        Move(fTimeDelta);

    if (m_bDone)
    {
        _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
        Transpose = XMMatrixMultiply(XMMatrixTranslation(0.f, -0.5f, 0.f), Transpose);
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
        m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
        //PxTransform px = m_pPhysXCollider->Get_Static_RigidBody()->getGlobalPose();
        //int a = 0;
    }
    else
    {
        m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 100000,-100000.f} });
    }

    _float4x4 Mat;
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * 2.f, 0.1f, Mat);

    // Call all reserved effects
    m_pModelCom->PlayReservedCallbacks();

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CButton_Platform::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 3.f))
        return;
    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    pGI->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CButton_Platform::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);      

        if (i == 2)
        {
            XMINT2 vDivide = { 4, 4 };
            if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &vDivide, sizeof(XMINT2))))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iDecalIndex, sizeof(_int))))
                return E_FAIL;
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                return E_FAIL;

            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::EmissionFrame);
        }
        else
            m_pShaderCom->Begin(0);

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CButton_Platform::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
     m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Platform_Land", m_pTransformCom->Get_Position());
	 EFFECTMANAGER->Call_Union_Effect("Platform_Cylinder_Effect", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
}

HRESULT CButton_Platform::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Button_Platform"), TEXT("Model_Com")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

    string strTag = m_strGimmickID;

    _float3 m_vModelSize = { 2.f, 0.5f, 2.f };
    _float3 m_vModelCenter = { 0.f, -0.5f, 0.f };
    PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
    PxBoxGeometry Box{ BoxDimension };
    auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, strTag.c_str());
    assert(pRigidBody);

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vExtents = { 2.f, 0.5f, 2.f };
    aabbDesc.vCenter = { 0.f,  0.5f, 0.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));
    assert(m_pColliderCom);

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
        assert(false);

    return S_OK;
}

HRESULT CButton_Platform::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    _float4x4 TempScale;
    _float fScale = max(BEATRATE * 0.5f + 0.5f, 0.75f);
    XMStoreFloat4x4(&TempScale, XMMatrixScaling(fScale, 1.f, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());

   /* if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;*/
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &TempScale)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

void CButton_Platform::Move(_float fTimeDelta)
{
    _vector vCurPosition = m_pTransformCom->Get_Position();

    if (m_vFinalPosition.y < XMVectorGetY(vCurPosition)
        || XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vFinalPosition) - vCurPosition)) < 0.5f)
    {
        m_pTransformCom->Set_Position(XMLoadFloat3(&m_vFinalPosition));
    
        m_pModelCom->Set_AnimIndex(1, false);
        m_bDone = true;
        m_bExecuted = false;
        m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"ButtonPlatformTemp", m_pTransformCom->Get_Position());

    }
    
    else
    {
        m_pTransformCom->Go_Up(fTimeDelta);
    }
    
    //_float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
    //_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    //
    //_float fRatio = fTimer / fBeat;
    //
    //_vector vPosition = XMVectorLerp(XMLoadFloat3(&m_vInitialPosition), XMLoadFloat3(&m_vFinalPosition), fRatio);
    //m_pTransformCom->Set_Position(vPosition);
    //
    //if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vFinalPosition) - vCurPosition)) < 0.5f)
    //{
    //    m_pTransformCom->Set_Position(XMLoadFloat3(&m_vFinalPosition));
    //
    //    m_pModelCom->Set_AnimIndex(1, false);
    //    m_bDone = true;
    //}
}

shared_ptr<CButton_Platform> CButton_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CButton_Platform
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CButton_Platform(pDevice, pContext)
        {}
    };

    shared_ptr<CButton_Platform> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CButton_Platform"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CButton_Platform::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CButton_Platform
    {
        MakeSharedEnabler(const CButton_Platform& rhs) : CButton_Platform(rhs)
        {}
    };

    shared_ptr<CButton_Platform> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CButton_Platform"));
        assert(false);
    }

    return pInstance;
}

void CButton_Platform::Free()
{
}
