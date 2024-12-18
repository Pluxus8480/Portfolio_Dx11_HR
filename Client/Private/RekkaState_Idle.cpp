#include "RekkaState_Idle.h"
#include "Monster_Rekka.h"

#include "RekkaState_Move.h"
#include "RekkaState_Attack.h"
#include "RekkaState_Rotate.h"

CRekkaState_Idle::CRekkaState_Idle(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_pOwner = pRekka;
	m_state = State::IDLE;
	m_preState = preState;
}

CRekkaState* CRekkaState_Idle::StepState(_float fTimeDelta)
{
	_bool isBeatAble = m_pOwner.lock()->IsBeatAble();
	if (!isBeatAble)
		return nullptr;

	if (m_pOwner.lock()->m_fTimer_Attack <= 0.f)
		return new CRekkaState_Attack(m_pOwner, m_state);
	else if (abs(m_pOwner.lock()->m_fAngleDegree) > 70.f)
		return new CRekkaState_Rotate(m_pOwner, m_state);
	else
		return new CRekkaState_Move(m_pOwner, m_state);

	return nullptr;
}

void CRekkaState_Idle::ActiveState(_float fTimeDelta)
{
}

void CRekkaState_Idle::EnterState()
{
	m_pOwner.lock()->SetAnim("em2000_idle_020", true);
}

void CRekkaState_Idle::ExitState()
{
}
