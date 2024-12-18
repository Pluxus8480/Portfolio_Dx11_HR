#include "RekkaState_Pase_4.h"
#include "Monster_Rekka.h"

#include "PlayerManager.h"
#include "Beat_Manager.h"

#include "RekkaState_Pipe.h"

CRekkaState_Pase_4::CRekkaState_Pase_4(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
    m_pOwner = pRekka;
    m_state = State::INTRO;
    m_preState = preState;
}

CRekkaState* CRekkaState_Pase_4::StepState(_float fTimeDelta)
{
    if (m_pOwner.lock()->IsAnimFinished())
        return new CRekkaState_Pipe(m_pOwner, m_state, true);

    return nullptr;
}

void CRekkaState_Pase_4::ActiveState(_float fTimeDelta)
{
}

void CRekkaState_Pase_4::EnterState()
{
    auto pOwner = m_pOwner.lock();

    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat() * 2.f, 0.03f, pOwner);
    PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat() * 2.f, 0.03f, PLAYERMANAGER->Get_Player());

    pOwner->SetAnim("em2000_dmg-ex_410");
}

void CRekkaState_Pase_4::ExitState()
{
}
