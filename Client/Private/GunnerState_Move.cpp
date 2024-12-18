#include "GunnerState_Move.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Monster_Gunner.h"

#include "GunnerState_Idle.h"
#include "GunnerState_Attack.h"

CGunnerState_Move::CGunnerState_Move(weak_ptr<class CMonster_Gunner> pGunner, State preState)
{
	m_state = State::MOVE;
	m_pOwner = pGunner;
	m_preState = preState;
}

CGunnerState* CGunnerState_Move::StepState(_float fTimeDelta)
{
	auto pOwner = m_pOwner.lock();
	_bool isBeatAble = pOwner->IsBeatAble();
	if (!isBeatAble)
		return nullptr;
	if (pOwner->IsNeedToMove())
		return nullptr;

	if (pOwner->m_fTimer_Attack <= 0.f)
		return new CGunnerState_Attack(m_pOwner, m_state);

	return nullptr;
}

void CGunnerState_Move::ActiveState(_float fTimeDelta)
{
	auto pOwner = m_pOwner.lock();

	if (!pOwner->IsNeedToMove())
		pOwner->SetAnim("em0200_idle_000", true);
	else
		pOwner->SetAnim("em0200_mvnt-walk-st_000", true);

	_float fSpeedRotation = 1.5f;
	auto vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
	pOwner->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
}

void CGunnerState_Move::EnterState()
{
	auto pOwner = m_pOwner.lock();
	pOwner->SetAnim("em0200_mvnt-walk-st_000", true);
}

void CGunnerState_Move::ExitState()
{
}
