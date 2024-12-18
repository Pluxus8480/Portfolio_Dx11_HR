#include "CRobotPunchStaticWall.h"
#include "Cine_Manager.h"


wstring CRobotPunchStaticWall::ObjID = TEXT("CRobotPunchStaticWall");

CRobotPunchStaticWall::CRobotPunchStaticWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CRobotPunchStaticWall::CRobotPunchStaticWall(const CRobotPunchStaticWall& rhs)
	:CJeongGimmicBase(rhs)
{
}

CRobotPunchStaticWall::~CRobotPunchStaticWall()
{
}

HRESULT CRobotPunchStaticWall::Initialize_Prototype()
{
    _float4x4 fMat;
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_CRobotPunchStaticBeforeWall",
            L"../Bin/Resources/Models/Map/Gimmick/CRobotPunchStaticWall0.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_CRobotPunchStaticWall",
            L"../Bin/Resources/Models/Map/Gimmick/CRobotPunchStaticWall1.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_CRobotPunchStaticAfterWall",
            L"../Bin/Resources/Models/Map/Gimmick/CRobotPunchStaticWall2.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );

	return S_OK;
}

HRESULT CRobotPunchStaticWall::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_CRobotPunchStaticWall",
        L"Prototype_Component_Shader_VtxMesh");

    m_pBreakBeforeModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC,
        L"Prototype_Component_Model_CRobotPunchStaticBeforeWall", L"BreakBeforeModel_Com"));

    m_pBreakAfterModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC,
        L"Prototype_Component_Model_CRobotPunchStaticAfterWall", L"BreakAfterModel_Com"));

    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

    CCine_Manager::Get_Instance()->AddStartCallback
    ("EV1160", [&]()
        {
            m_bRender = true;
            m_iWallIndex = 1;
        }
    );

    CCine_Manager::Get_Instance()->AddStartCallback
    ("EV1170", [&]()
        {
            m_bRender = true;
            m_iWallIndex = 2;
        }
    );

	return S_OK;
}

void CRobotPunchStaticWall::PriorityTick(_float fTimeDelta)
{
}

void CRobotPunchStaticWall::Tick(_float fTimeDelta)
{
}

void CRobotPunchStaticWall::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 50))
        return;

    if (m_bRender)
    {
        pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
    }
}

HRESULT CRobotPunchStaticWall::Render()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

    _uint	iNumMeshes = 0;

    switch (m_iWallIndex)
    {
    case 0:
        iNumMeshes = m_pBreakBeforeModelCom->Get_NumMeshes();
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pBreakBeforeModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                assert(false);

            m_pShaderCom->Begin(m_iShaderPassIndex);
            m_pBreakBeforeModelCom->Render((_uint)i);
        }
        break; 
    case 1:
        iNumMeshes = m_pModelCom->Get_NumMeshes();
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                assert(false);

            m_pShaderCom->Begin(m_iShaderPassIndex);
            m_pModelCom->Render((_uint)i);
        }       
        break;
    case 2:
        iNumMeshes = m_pBreakAfterModelCom->Get_NumMeshes();
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pBreakAfterModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                assert(false);

            m_pShaderCom->Begin(m_iShaderPassIndex);
            m_pBreakAfterModelCom->Render((_uint)i);
        }
        break;
    default:
        break;
    }
    return S_OK;
}

void CRobotPunchStaticWall::Change_Phase(_uint iPhase)
{
    m_iCurPhase = iPhase;
    switch ((KALEMAP_PHASE)m_iCurPhase)
    {
    case KALEMAP_PHASE::ROBOTPUNCHCHAI:
        m_bRender = false;
        break;
    case KALEMAP_PHASE::ROBOTPUNCHMACARON:
        m_bRender = false;
        break;
    default:
        break;
    }
}

shared_ptr<CRobotPunchStaticWall> CRobotPunchStaticWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRobotPunchStaticWall
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRobotPunchStaticWall(pDevice, pContext)
        {}
    };

    shared_ptr<CRobotPunchStaticWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CRobotPunchStaticWall::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRobotPunchStaticWall
    {
        MakeSharedEnabler(const CRobotPunchStaticWall& rhs) : CRobotPunchStaticWall(rhs)
        {}
    };

    shared_ptr<CRobotPunchStaticWall> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CRobotPunchStaticWall::Free()
{

}


