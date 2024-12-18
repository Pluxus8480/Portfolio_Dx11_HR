#include "SaberState_Down.h"
#include "Monster_Saber.h"

#include "SaberState_Idle.h"

CSaberState_Down::CSaberState_Down(weak_ptr<class CMonster_Saber> pSaber, State preState)
{
	m_state = State::DOWN;
	m_pOwner = pSaber;
	m_preState = preState;
}

CSaberState* CSaberState_Down::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_pOwner.lock()->IsDeadState())
	{
		m_pOwner.lock()->MonsterDead();
		return nullptr;
	}

	if (isFinished && m_strCurAnim == "em0100_dmg-down_110")
		return new CSaberState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CSaberState_Down::ActiveState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_strCurAnim == "em0100_dmg-down_200")
	{
		m_strCurAnim = "em0100_dmg-down_110";
		m_pOwner.lock()->SetAnim(m_strCurAnim);
	}
}

void CSaberState_Down::EnterState()
{
	m_strCurAnim = "em0100_dmg-down_200";
	m_pOwner.lock()->SetAnim(m_strCurAnim);
}

void CSaberState_Down::ExitState()
{
}

_bool CSaberState_Down::IsColliderOn()
{
	if (m_pOwner.lock()->IsDeadState())
		return false;
	return true;
}