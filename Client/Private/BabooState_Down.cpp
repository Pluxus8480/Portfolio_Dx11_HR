#include "BabooState_Down.h"
#include "Monster_Baboo.h"

#include "BabooState_Idle.h"

CBabooState_Down::CBabooState_Down(weak_ptr<class CMonster_Baboo> pBaboo, State preState)
{
	m_state = State::DOWN;
	m_pOwner = pBaboo;
	m_preState = preState;
}

CBabooState* CBabooState_Down::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_pOwner.lock()->IsDeadState())
	{
		m_pOwner.lock()->MonsterDead();
		return nullptr;
	}

	if (isFinished && m_strCurAnim == "em0400_dmg-down_110")
		return new CBabooState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CBabooState_Down::ActiveState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_strCurAnim == "em0400_dmg-down_200")
	{
		m_strCurAnim = "em0400_dmg-down_110";
		m_pOwner.lock()->SetAnim(m_strCurAnim);
	}
}

void CBabooState_Down::EnterState()
{
	m_strCurAnim = "em0400_dmg-down_200";
	m_pOwner.lock()->SetAnim(m_strCurAnim);
}

void CBabooState_Down::ExitState()
{
}

_bool CBabooState_Down::IsColliderOn()
{
	if (m_pOwner.lock()->IsDeadState())
		return false;
	return true;
}