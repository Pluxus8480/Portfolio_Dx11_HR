#include "CTriggerBox.h"


wstring CTriggerBox::ObjID = TEXT("CTriggerBox");

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& rhs)
	:CJeongGimmicBase(rhs)
{
}

CTriggerBox::~CTriggerBox()
{
}

HRESULT CTriggerBox::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_KaleElevatorGenerator",
            L"../Bin/Resources/Models/Map/Gimmick/KaleElevatorGenerator.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );
	return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);

    CBounding_AABB::boundingAABBDesc Desc{};
    Desc.vExtents = { 1.f, 1.f, 1.f };
    Desc.vCenter = { 0, 0, 0 };
    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, 
        L"Prototype_Component_Collider_AABB", L"Collider_Com", &Desc));

	return S_OK;
}

void CTriggerBox::PriorityTick(_float fTimeDelta)
{
}

void CTriggerBox::Tick(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CTriggerBox::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10))
        return;

    pGI->Add_Collider(COLLIDER_GROUP::COLLISION_GIMMICK, m_pColliderCom);
    pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
}

HRESULT CTriggerBox::Render()
{
#ifdef PERFECTION_LOAD

#else
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CTriggerBox::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && !m_bExecuted)
    {
        Execute(true);
        for (auto& iter : m_vecRelatives)
            iter->Execute();
    }
}

void CTriggerBox::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CTriggerBox::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CTriggerBox::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);
}

void CTriggerBox::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);
}

shared_ptr<CTriggerBox> CTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CTriggerBox
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTriggerBox(pDevice, pContext)
        {}
    };

    shared_ptr<CTriggerBox> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CTriggerBox::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CTriggerBox
    {
        MakeSharedEnabler(const CTriggerBox& rhs) : CTriggerBox(rhs)
        {}
    };

    shared_ptr<CTriggerBox> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CTriggerBox::Free()
{

}
