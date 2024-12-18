#include "EventTrigger.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "CharacterController.h"
#include "PlayerManager.h"
wstring CEventTrigger::ObjID = TEXT("CEventTrigger");
CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
    : CGimmickBase{rhs}
{
}

CEventTrigger::~CEventTrigger()
{
    Free();
}

HRESULT CEventTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CEventTrigger::PriorityTick(_float fTimeDelta)
{
}

void CEventTrigger::Tick(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CEventTrigger::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

#ifdef _DEBUG
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CEventTrigger::Render()
{
#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif
    return S_OK;
}

void CEventTrigger::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        PLAYERMANAGER->Callback_Event(m_strEventTag);
    }
}

void CEventTrigger::Edit_Attribute()
{
    _char szEventTag[MAX_PATH];
    strcpy(szEventTag, m_strEventTag.c_str());

    if (ImGui::InputText("Event Tag", szEventTag, MAX_PATH))
        m_strEventTag = szEventTag;
}

void CEventTrigger::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CEventTrigger::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CEventTrigger::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);

    size_t size = m_strEventTag.size();
    write_typed_data(os, m_strEventTag.size());
    os.write(&m_strEventTag[0], m_strEventTag.size());
}

void CEventTrigger::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);

    size_t size = {};

    read_typed_data(is, size);
    m_strEventTag.resize(size);
    is.read(&m_strEventTag[0], size);
}

HRESULT CEventTrigger::Ready_Components()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vCenter = { 0.f, 0.f, 0.f };
    aabbDesc.vExtents = { 1.f, 1.f, 1.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

    assert(m_pColliderCom);

    return S_OK;
}

HRESULT CEventTrigger::Bind_ShaderResources()
{
    return S_OK;
}

shared_ptr<CEventTrigger> CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CEventTrigger
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEventTrigger(pDevice, pContext)
        {}
    };

    shared_ptr<CEventTrigger> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEventTrigger"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CEventTrigger::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CEventTrigger
    {
        MakeSharedEnabler(const CEventTrigger& rhs) : CEventTrigger(rhs)
        {}
    };

    shared_ptr<CEventTrigger> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CEventTrigger"));
        assert(false);
    }

    return pInstance;
}

void CEventTrigger::Free()
{
}
