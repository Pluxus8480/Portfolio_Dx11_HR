#include "CBattle3rdFloor.h"
#include "Cine_Manager.h"


wstring CBattle3rdFloor::ObjID = TEXT("CBattle3rdFloor");

CBattle3rdFloor::CBattle3rdFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CBattle3rdFloor::CBattle3rdFloor(const CBattle3rdFloor& rhs)
	:CJeongGimmicBase(rhs)
{
}

CBattle3rdFloor::~CBattle3rdFloor()
{
}

HRESULT CBattle3rdFloor::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_Battle3rdFloor",
            L"../Bin/Resources/Models/Map/Gimmick/CBattle3rdFloor.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );
	return S_OK;
}

HRESULT CBattle3rdFloor::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_Battle3rdFloor",
        L"Prototype_Component_Shader_VtxMesh");
    Initialize_CookPhysX(L"Prototype_Component_Model_Battle3rdFloor");

    CCine_Manager::Get_Instance()->AddStartCallback
    ("EV1170", [&]()
        {
            m_bRender = false;
        }
    );

	return S_OK;
}

void CBattle3rdFloor::PriorityTick(_float fTimeDelta)
{
}

void CBattle3rdFloor::Tick(_float fTimeDelta)
{
    if (m_bRender)
    {
        Use_PhysX();
    }
    else
    {
        for (auto& iter : m_vecPxRigid)
        {
            iter->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 100000,-100000.f}});
        }
    }
}

void CBattle3rdFloor::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fModelRange))
        return;

    if (m_bRender)
    {
        pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
    }
}

shared_ptr<CBattle3rdFloor> CBattle3rdFloor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CBattle3rdFloor
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBattle3rdFloor(pDevice, pContext)
        {}
    };

    shared_ptr<CBattle3rdFloor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CBattle3rdFloor::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CBattle3rdFloor
    {
        MakeSharedEnabler(const CBattle3rdFloor& rhs) : CBattle3rdFloor(rhs)
        {}
    };

    shared_ptr<CBattle3rdFloor> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CBattle3rdFloor::Free()
{

}
