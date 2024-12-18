#include "CRobotBeamContainer.h"
#include "iostream"


wstring CRobotBeamContainer::ObjID = TEXT("CRobotBeamContainer");

CRobotBeamContainer::CRobotBeamContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CRobotBeamContainer::CRobotBeamContainer(const CRobotBeamContainer& rhs)
	:CJeongGimmicBase(rhs)
{
}

CRobotBeamContainer::~CRobotBeamContainer()
{
}

HRESULT CRobotBeamContainer::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_RobotBeamContainer",
            L"../Bin/Resources/Models/Map/Gimmick/CRobotBeamContainer.fbx",
            MODEL_TYPE::ANIM, fMat)
    );
	return S_OK;
}

HRESULT CRobotBeamContainer::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_RobotBeamContainer",
        L"Prototype_Component_Shader_VtxAnimMesh");
    Initialize_CookPhysX(L"Prototype_Component_Model_RobotBeamContainer");

    m_pModelCom->Set_AnimIndex(0, false);
    _float4x4 Dummy;
    m_pModelCom->Play_Animation(0.001f, 0.001f, 0.1f, Dummy);
    
    m_pModelCom->Set_CurrentTrackPosition(15);
    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

	return S_OK;
}

void CRobotBeamContainer::PriorityTick(_float fTimeDelta)
{
}

void CRobotBeamContainer::Tick(_float fTimeDelta)
{
    if (m_bPlay)
    {
        _float4x4 Dummy;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, Dummy);
    }

    if (m_pModelCom->Get_Finished())
    {
        m_bAnimEnd = true;
    }
}

void CRobotBeamContainer::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fModelRange))
        return;

    if (!m_bAnimEnd)
    {
        pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
    }
}

HRESULT CRobotBeamContainer::Render()
{
    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(Bind_ShaderResources()))
        assert(false);

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);
        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CRobotBeamContainer::Change_Phase(_uint iPhase)
{
    m_iCurPhase = iPhase;
    switch ((KALEMAP_PHASE)m_iCurPhase)
    {
    case KALEMAP_PHASE::ROBOTBEAM:
        m_bPlay = true;
        break;    
    default:
        break;
    }
}

shared_ptr<CRobotBeamContainer> CRobotBeamContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRobotBeamContainer
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRobotBeamContainer(pDevice, pContext)
        {}
    };

    shared_ptr<CRobotBeamContainer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CRobotBeamContainer::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRobotBeamContainer
    {
        MakeSharedEnabler(const CRobotBeamContainer& rhs) : CRobotBeamContainer(rhs)
        {}
    };

    shared_ptr<CRobotBeamContainer> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CRobotBeamContainer::Free()
{

}
