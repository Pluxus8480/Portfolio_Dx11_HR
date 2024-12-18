#include "SpawnTrigger_Platform.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Button_Platform.h"
#include "Beat_Manager.h"
#include "CombatManager.h"

wstring CSpawnTrigger_Platform::ObjID = TEXT("CSpawnTrigger_Platform");

CSpawnTrigger_Platform::CSpawnTrigger_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CSpawnTrigger_Platform::CSpawnTrigger_Platform(const CSpawnTrigger_Platform& rhs)
    : CGimmickBase{rhs}
{
}

CSpawnTrigger_Platform::~CSpawnTrigger_Platform()
{
    Free();
}

HRESULT CSpawnTrigger_Platform::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpawnTrigger_Platform::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    return S_OK;
}

void CSpawnTrigger_Platform::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        for (_uint i = 0; i < m_vecRelatives.size(); ++i)
        {
            auto pButtonPlatform = dynamic_pointer_cast<CButton_Platform>(m_vecRelatives[i]);
            if (pButtonPlatform)
                pButtonPlatform->Set_DecalIndex(i);
        }

        m_bInit = true;
    }
}

void CSpawnTrigger_Platform::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (GAMEINSTANCE->Get_KeyDown(DIK_L))
        m_bExecuted = true;
#endif

    if (m_bExecuted && !m_bDone)
    {
        _uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
        if (iCurBeat != m_iPrevBeat)
        {
            m_iPrevBeat = iCurBeat;

            m_vecRelatives[m_iCurIndex]->Execute(true);

            ++m_iCurIndex;
            if (m_iCurIndex == m_vecRelatives.size())
            {
                m_iCurIndex = 0;
                m_bExecuted = false;
                m_bDone = true;
            }
        }
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CSpawnTrigger_Platform::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

#ifdef _DEBUG
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CSpawnTrigger_Platform::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CSpawnTrigger_Platform::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        if (!CCombat_Manager::Get_Instance()->Get_Is_Combat())
        {
            m_bExecuted = true;
            m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
        }
    }
}

void CSpawnTrigger_Platform::Edit_Attribute()
{
}

void CSpawnTrigger_Platform::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CSpawnTrigger_Platform::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CSpawnTrigger_Platform::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);
}

void CSpawnTrigger_Platform::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);
}

HRESULT CSpawnTrigger_Platform::Ready_Components()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vExtents = { 1.f, 1.f, 1.f };
    aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Collider_Com"), &aabbDesc));

    assert(m_pColliderCom);

    return S_OK;
}

HRESULT CSpawnTrigger_Platform::Bind_ShaderResources()
{
    return S_OK;
}

shared_ptr<CSpawnTrigger_Platform> CSpawnTrigger_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CSpawnTrigger_Platform
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSpawnTrigger_Platform(pDevice, pContext)
        {}
    };

    shared_ptr<CSpawnTrigger_Platform> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSpawnTrigger_Platform"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CSpawnTrigger_Platform::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CSpawnTrigger_Platform
    {
        MakeSharedEnabler(const CSpawnTrigger_Platform& rhs) : CSpawnTrigger_Platform(rhs)
        {}
    };

    shared_ptr<CSpawnTrigger_Platform> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSpawnTrigger_Platform"));
        assert(false);
    }

    return pInstance;
}

void CSpawnTrigger_Platform::Free()
{
}
