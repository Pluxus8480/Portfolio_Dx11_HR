#include "RekkaState_Pase_3.h"
#include "Monster_Rekka.h"

#include "PlayerManager.h"
#include "Beat_Manager.h"

#include "CUi_Mgr.h"
#include "MapManager.h"
#include "Cine_Manager.h"

#include "RekkaState_Idle.h"


CRekkaState_Pase_3::CRekkaState_Pase_3(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
    m_pOwner = pRekka;
    m_state = State::INTRO;
    m_preState = preState;
}

CRekkaState* CRekkaState_Pase_3::StepState(_float fTimeDelta)
{
    auto isInGame = CCine_Manager::Get_Instance()->IsInGame();
    if (isInGame && m_strCurAnim == "Over")
        return new CRekkaState_Idle(m_pOwner, m_state);

    return nullptr;
}

void CRekkaState_Pase_3::ActiveState(_float fTimeDelta)
{
    if (m_pOwner.lock()->m_fTimeMultiflier <= 1.f && m_strCurAnim == "em2000_dmg-ex_410")
    {
        m_strCurAnim = "Over";

        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        CCine_Manager::Get_Instance()->StartCine("CS0240");

        m_pOwner.lock()->m_iCurPase = 3;
        CMapManager::Get_Instance()->Boss_ChangePhase(_uint(CBoss_InteractObj::REKKA_PHASE::PHASE_3));
    }
}

void CRekkaState_Pase_3::EnterState()
{
    auto pOwner = m_pOwner.lock();

    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat() * 2.f, 0.03f, pOwner);
    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat() * 2.f, 0.03f, PLAYERMANAGER->Get_Player());

    m_strCurAnim = "em2000_dmg-ex_410";
    pOwner->SetAnim(m_strCurAnim);
}

void CRekkaState_Pase_3::ExitState()
{
}
