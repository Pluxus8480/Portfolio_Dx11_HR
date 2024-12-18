#include "SpawnTrigger_Monster.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "MonsterSpawner.h"
#include "MonsterBase.h"
#include "CombatManager.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "Cine_Manager.h"

wstring CSpawnTrigger_Monster::ObjID = TEXT("CSpawnTrigger_Monster");

CSpawnTrigger_Monster::CSpawnTrigger_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CSpawnTrigger_Monster::CSpawnTrigger_Monster(const CSpawnTrigger_Monster& rhs)
    : CGimmickBase{rhs}
{
}

CSpawnTrigger_Monster::~CSpawnTrigger_Monster()
{
    Free();
}

void CSpawnTrigger_Monster::Start_Spawn(_bool bEventEnd)
{
    m_bWaitEvent = !bEventEnd;

    m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
}

HRESULT CSpawnTrigger_Monster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpawnTrigger_Monster::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    return S_OK;
}

void CSpawnTrigger_Monster::PriorityTick(_float fTimeDelta)
{
}

void CSpawnTrigger_Monster::Tick(_float fTimeDelta)
{
    //if (GAMEINSTANCE->Get_KeyDown(DIK_O))
    //{
    //    Execute();
    //    m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    //}
    if (m_bExecuted && m_bWaitEvent)
    {
        if(!static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Get_IsEvent())
            m_bWaitEvent = false;

        if(!m_bWaitEvent)
            m_bWaitEvent = (CCine_Manager::Get_Instance()->IsInGame()) ? false : true;
    }

    if (!m_bWaitEvent && !m_bUsed)
    {
        _uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
        if (m_bExecuted && m_iPrevBeat != iCurBeat)
        {
            for (auto& iter : m_vecRelatives)
                iter->Execute();
            m_bUsed = true;
        }
    }

    _int iDoneCount = { 0 };
    for (auto& iter : m_vecRelatives)
    {
        if (iter->Get_Done())
            ++iDoneCount;
    }

    if (!m_bDone && iDoneCount != 0 && iDoneCount == m_vecRelatives.size())
    {
        for (_int i = 0; i < m_iWaveCount; ++i)
        {
            vector<CMonsterBase::Monster_DESC> vecMonsterDesc;

            for (auto pObject : m_vecRelatives)
            {
                auto pMonsterSpawner = dynamic_pointer_cast<CMonsterSpawner>(pObject);
                if(pMonsterSpawner)
                    vecMonsterDesc.emplace_back(pMonsterSpawner->Make_MonsterDesc());
            }

            CCombat_Manager::Get_Instance()->Push_Wave(vecMonsterDesc);
        }

        CCombat_Manager::Get_Instance()->Finish_Input_Wave();

        CGimmickBase::Done();
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CSpawnTrigger_Monster::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

#ifdef _DEBUG
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CSpawnTrigger_Monster::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CSpawnTrigger_Monster::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (!m_bExecuted && !m_bDone && pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        CCombat_Manager::Get_Instance()->Set_Current_Battle_ID(m_strGimmickID);
        Execute();
        m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    }
}

void CSpawnTrigger_Monster::Edit_Attribute()
{
    ImGui::InputInt("Wave Count", &m_iWaveCount);
    ImGui::Checkbox("Ready For Event", &m_bWaitEvent);
}

void CSpawnTrigger_Monster::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CSpawnTrigger_Monster::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CSpawnTrigger_Monster::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);

    write_typed_data(os, m_iWaveCount);
    write_typed_data(os, m_bWaitEvent);
}

void CSpawnTrigger_Monster::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);

    read_typed_data(is, m_iWaveCount);
    read_typed_data(is, m_bWaitEvent);
}

HRESULT CSpawnTrigger_Monster::Ready_Components()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vExtents = { 1.f, 1.f, 1.f };
    aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Collider_Com"), &aabbDesc));

    assert(m_pColliderCom);

    return S_OK;
}

HRESULT CSpawnTrigger_Monster::Bind_ShaderResources()
{
    return S_OK;
}

shared_ptr<CSpawnTrigger_Monster> CSpawnTrigger_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CSpawnTrigger_Monster
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSpawnTrigger_Monster(pDevice, pContext)
        {}
    };

    shared_ptr<CSpawnTrigger_Monster> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSpawnTrigger_Monster"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CSpawnTrigger_Monster::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CSpawnTrigger_Monster
    {
        MakeSharedEnabler(const CSpawnTrigger_Monster& rhs) : CSpawnTrigger_Monster(rhs)
        {}
    };

    shared_ptr<CSpawnTrigger_Monster> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSpawnTrigger_Monster"));
        assert(false);
    }

    return pInstance;
}

void CSpawnTrigger_Monster::Free()
{
}
