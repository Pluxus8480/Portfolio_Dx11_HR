#include "CRailContainer.h"
#include "Cine_Manager.h"


wstring CRailContainer::ObjID = TEXT("CRailContainer");

CRailContainer::CRailContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CRailContainer::CRailContainer(const CRailContainer& rhs)
	:CJeongGimmicBase(rhs)
{
}

CRailContainer::~CRailContainer()
{
}

HRESULT CRailContainer::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_RailContainerBefore",
            L"../Bin/Resources/Models/Map/Gimmick/CRailContainerBefore.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_RailContainerAfter",
            L"../Bin/Resources/Models/Map/Gimmick/CRailContainerAfter.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );
    
	return S_OK;
}

HRESULT CRailContainer::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_RailContainerBefore",
        L"Prototype_Component_Shader_VtxMesh");
    Initialize_CookPhysX(L"Prototype_Component_Model_RailContainerBefore");

    m_pAfterModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC,
        L"Prototype_Component_Model_RailContainerAfter", L"AfterModel_Com"));

    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

    CCine_Manager::Get_Instance()->AddLastCallback
    ("EV1130", [&]()
	{
        m_bBefore = false;
        m_bRender = true;
	}
    );
	return S_OK;
}

void CRailContainer::PriorityTick(_float fTimeDelta)
{
}

void CRailContainer::Tick(_float fTimeDelta)
{
}

void CRailContainer::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 100))
        return;

    if (m_bRender)
    {
        pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
    }
}

HRESULT CRailContainer::Render()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

    if (m_bBefore)
    {
        _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                assert(false);

            m_pShaderCom->Begin(m_iShaderPassIndex);
            m_pModelCom->Render((_uint)i);
        }
    }
    else
    {
        _uint	iNumMeshes = m_pAfterModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pAfterModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                assert(false);

            m_pShaderCom->Begin(m_iShaderPassIndex);
            m_pAfterModelCom->Render((_uint)i);
        }
    }

    return S_OK;
}

void CRailContainer::Change_Phase(_uint iPhase)
{
    m_iCurPhase = iPhase;
    switch (KALEMAP_PHASE(m_iCurPhase))
    {
    case Client::CBoss_InteractObj::KALEMAP_PHASE::CONTAINERCUTSCENE:
        m_bRender = false;
        break;
    default:
        break;
    }
}

shared_ptr<CRailContainer> CRailContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRailContainer
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRailContainer(pDevice, pContext)
        {}
    };

    shared_ptr<CRailContainer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CRailContainer::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRailContainer
    {
        MakeSharedEnabler(const CRailContainer& rhs) : CRailContainer(rhs)
        {}
    };

    shared_ptr<CRailContainer> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CRailContainer::Free()
{

}
