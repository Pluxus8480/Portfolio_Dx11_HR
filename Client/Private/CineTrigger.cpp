#include "CineTrigger.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "CharacterController.h"
#include "Cine_Manager.h"
#include "MapManager.h"
#include "CombatManager.h"

wstring CCineTrigger::ObjID = TEXT("CCineTrigger");
CCineTrigger::CCineTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{ pDevice, pContext }
{
}

CCineTrigger::CCineTrigger(const CCineTrigger& rhs)
    : CGimmickBase{ rhs }
{
}

CCineTrigger::~CCineTrigger()
{
    Free();
}

void CCineTrigger::SetUp_CinematicCallBack()
{
    if (m_strCineTag == "EV1120")
    {
        CCine_Manager::Get_Instance()->AddLastCallback("EV1120", [&]() {
            _float4x4 matResult_0{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0100", matResult_0);

            _float4x4 matResult_1{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0100_1", matResult_1);

            _float4x4 matResult_2{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0100_2", matResult_2);

            vector<CMonsterBase::Monster_DESC> vecMonsterDesc;

            CMonsterBase::Monster_DESC monsterDesc{};
            monsterDesc.strObjID = L"Saber";
            monsterDesc.strDesc = "";
            monsterDesc.matTransform = matResult_0;
            vecMonsterDesc.emplace_back(monsterDesc);

            monsterDesc.matTransform = matResult_1;
            vecMonsterDesc.emplace_back(monsterDesc);

            monsterDesc.matTransform = matResult_2;
            vecMonsterDesc.emplace_back(monsterDesc);

            _float4x4 matResult_3{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0200", matResult_3);

            _float4x4 matResult_4{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0200_1", matResult_4);

            monsterDesc.strObjID = L"Gunner";
            monsterDesc.matTransform = matResult_3;
            vecMonsterDesc.emplace_back(monsterDesc);

            monsterDesc.matTransform = matResult_4;
            vecMonsterDesc.emplace_back(monsterDesc);

            _float4x4 matResult_5{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0700", matResult_5);

            monsterDesc.strObjID = L"SwordMachine";
            monsterDesc.matTransform = matResult_5;
            vecMonsterDesc.emplace_back(monsterDesc);

            CCombat_Manager::Get_Instance()->Push_Wave(vecMonsterDesc);
            CCombat_Manager::Get_Instance()->Finish_Input_Wave();

            }
        );
    }

}

HRESULT CCineTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCineTrigger::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CCineTrigger::PriorityTick(_float fTimeDelta)
{
}

void CCineTrigger::Tick(_float fTimeDelta)
{
    if (CMapManager::Get_Instance()->Get_NextLevel() != LEVEL_MAPTOOL)
    {
        if (!m_bTriggerAction && m_strCineTag == "EV1170")
        {
            auto pCombatManager = CCombat_Manager::Get_Instance();
            if (pCombatManager->Get_Is_Combat() && pCombatManager->Get_Current_Battle_ID() == "Stage_Kale_Battle3rd_2")
                m_b3rdBattle = true;

            if (m_b3rdBattle && pCombatManager->Get_Combat_End())
            {
                CCine_Manager::Get_Instance()->StartCine(m_strCineTag);
                CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::KALEMAP_PHASE::ROBOTPUNCHMACARON);

                m_bTriggerAction = true;
                m_b3rdBattle = false;
            }
        }
    }

#ifdef _DEBUG
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
#endif
}

void CCineTrigger::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

#ifdef _DEBUG
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CCineTrigger::Render()
{
#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif
    return S_OK;
}

void CCineTrigger::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && !m_bTriggerAction)
    {
        m_bTriggerAction = true;
        if (m_strCineTag == "EV1130")
        {
            CCine_Manager::Get_Instance()->StartCine(m_strCineTag);
            CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::KALEMAP_PHASE::CONTAINERCUTSCENE);
        }   
        else if (m_strCineTag == "EV1150")
        {
            CCine_Manager::Get_Instance()->StartCine(m_strCineTag);
            CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::KALEMAP_PHASE::SCROLLCUTSCENEENTER);
        }
        else if (m_strCineTag == "EV1160")
        {
            CCine_Manager::Get_Instance()->StartCine(m_strCineTag);
            CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::KALEMAP_PHASE::ROBOTPUNCHCHAI);
        }
        //else if (m_strCineTag == "EV1170")
        //{
        //    CCine_Manager::Get_Instance()->StartCine(m_strCineTag);
        //    CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::KALEMAP_PHASE::ROBOTPUNCHMACARON);
        //}
        else if (m_strCineTag == "EV1180")
        {
            CCine_Manager::Get_Instance()->StartCine(m_strCineTag);
            CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::KALEMAP_PHASE::CUTROBOTARM);
        }
        else if (m_strCineTag == "EV1120")
        {
            CCine_Manager::Get_Instance()->StartCine(m_strCineTag);
            CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::KALEMAP_PHASE::FIRSTBATTLE);
        }
    }
}

void CCineTrigger::Edit_Attribute()
{
    _char szCineTag[MAX_PATH];
    strcpy(szCineTag, m_strCineTag.c_str());

    if (ImGui::InputText("Cine Tag", szCineTag, MAX_PATH))
        m_strCineTag = szCineTag;
}

void CCineTrigger::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CCineTrigger::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CCineTrigger::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);

    size_t size = m_strCineTag.size();
    write_typed_data(os, m_strCineTag.size());
    os.write(&m_strCineTag[0], m_strCineTag.size());
}

void CCineTrigger::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);

    size_t size = {};

    read_typed_data(is, size);
    m_strCineTag.resize(size);
    is.read(&m_strCineTag[0], size);
}

HRESULT CCineTrigger::Ready_Components()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vCenter = { 0.f, 0.f, 0.f };
    aabbDesc.vExtents = { 1.f, 1.f, 1.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));
    assert(m_pColliderCom);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

    return S_OK;
}

HRESULT CCineTrigger::Bind_ShaderResources()
{
    return S_OK;
}

shared_ptr<CCineTrigger> CCineTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CCineTrigger
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCineTrigger(pDevice, pContext)
        {}
    };

    shared_ptr<CCineTrigger> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCineTrigger"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CCineTrigger::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CCineTrigger
    {
        MakeSharedEnabler(const CCineTrigger& rhs) : CCineTrigger(rhs)
        {}
    };

    shared_ptr<CCineTrigger> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCineTrigger"));
        assert(false);
    }

    return pInstance;
}

void CCineTrigger::Free()
{
}
