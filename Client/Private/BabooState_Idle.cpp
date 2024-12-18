#include "BabooState_Idle.h"
#include "GameInstance.h"
#include "Monster_Baboo.h"

#include "BabooState_Move.h"
#include "BabooState_Attack.h"

CBabooState_Idle::CBabooState_Idle(weak_ptr<class CMonster_Baboo> pBaboo, State preState)
{
    m_pOwner = pBaboo;
    m_state = State::IDLE;
    m_preState = preState;
}

CBabooState* CBabooState_Idle::StepState(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();
    _bool isBeatAble = pOwner->IsBeatAble();
    if (!isBeatAble)
        return nullptr;

    if (m_preState == State::END_STATE)
    {
        if (pOwner->IsAnimFinished())
            m_preState = State::IDLE;
    }
    else
    {
        if (pOwner->m_fTimer_Attack <= 0.f)
            return new CBabooState_Attack(m_pOwner, m_state);
        if (pOwner->IsNeedToMove())
            return new CBabooState_Move(m_pOwner, m_state);
    }

	return nullptr;
}

void CBabooState_Idle::ActiveState(_float fTimeDelta)
{
}

void CBabooState_Idle::EnterState()
{
    if (m_preState == State::END_STATE)
        m_pOwner.lock()->SetAnim("em0400_shield-a_010", false);
    else
        m_pOwner.lock()->SetAnim("em0400_idle_000", true);
}

void CBabooState_Idle::ExitState()
{
}
