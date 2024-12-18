#include "GunnerState_Down.h"
#include "Monster_Gunner.h"

#include "GunnerState_Idle.h"

CGunnerState_Down::CGunnerState_Down(weak_ptr<class CMonster_Gunner> pGunner, State preState)
{
	m_state = State::DOWN;
	m_pOwner = pGunner;
	m_preState = preState;
}

CGunnerState* CGunnerState_Down::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_pOwner.lock()->IsDeadState())
	{
		m_pOwner.lock()->MonsterDead();
		return nullptr;
	}

	if (isFinished && m_strCurAnim == "em0200_dmg-down_110")
		return new CGunnerState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CGunnerState_Down::ActiveState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_strCurAnim == "em0200_dmg-down_200")
	{
		m_strCurAnim = "em0200_dmg-down_110";
		m_pOwner.lock()->SetAnim(m_strCurAnim);
	}
}

void CGunnerState_Down::EnterState()
{
	m_strCurAnim = "em0200_dmg-down_200";
	m_pOwner.lock()->SetAnim(m_strCurAnim);
}

void CGunnerState_Down::ExitState()
{
}

_bool CGunnerState_Down::IsColliderOn()
{
	if (m_pOwner.lock()->IsDeadState())
		return false;
	return true;
}