#include "GunnerState_Idle.h"
#include "GameInstance.h"
#include "Monster_Gunner.h"

#include "GunnerState_Move.h"
#include "GunnerState_Attack.h"

CGunnerState_Idle::CGunnerState_Idle(weak_ptr<class CMonster_Gunner> pGunner, State preState)
{
    m_pOwner = pGunner;
    m_state = State::IDLE;
    m_preState = preState;
}

CGunnerState* CGunnerState_Idle::StepState(_float fTimeDelta)
{
    if (false == m_pOwner.lock()->IsBeatAble())
        return nullptr;

    if (m_pOwner.lock()->IsNeedToMove())
        return new CGunnerState_Move(m_pOwner, m_state);
    else if (m_pOwner.lock()->m_fTimer_Attack <= 0.f)
        return new CGunnerState_Attack(m_pOwner, m_state);

    return nullptr;
}

void CGunnerState_Idle::ActiveState(_float fTimeDelta)
{
    _float fRotationSpeed = 1.5f;
    auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
    m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fRotationSpeed);
}

void CGunnerState_Idle::EnterState()
{
    m_pOwner.lock()->SetAnim("em0200_idle_000", true);
}

void CGunnerState_Idle::ExitState()
{
}
