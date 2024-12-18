#include "RekkaState_Groggy.h"
#include "Monster_Rekka.h"

#include "RekkaState_Idle.h"


CRekkaState_Groggy::CRekkaState_Groggy(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_state = State::GROGGY;
	m_pOwner = pRekka;
	m_preState = preState;

	m_pOwner.lock()->m_fTimerGroggy = 4.f;
	m_isDamaged = false;
}

CRekkaState_Groggy::CRekkaState_Groggy(weak_ptr<class CMonster_Rekka> pRekka, State preState, EAttackDirection eAttDir)
{
	m_isDamaged = true;
	m_eAttDir = eAttDir;

	m_state = State::GROGGY;
	m_pOwner = pRekka;
	m_preState = preState;
}

CRekkaState* CRekkaState_Groggy::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (m_pOwner.lock()->m_fTimerGroggy <= 0.f && !m_isDamaged)
		return new CRekkaState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CRekkaState_Groggy::ActiveState(_float fTimeDelta)
{
	m_pOwner.lock()->m_fTimerGroggy -= fTimeDelta;

	if (m_pOwner.lock()->IsAnimFinished())
	{
		m_isDamaged = false;
		m_pOwner.lock()->SetAnim("em2000_atk_251", true);
	}
}

void CRekkaState_Groggy::EnterState()
{
	if (!m_isDamaged)
		m_pOwner.lock()->SetAnim("em2000_atk_251", true);
	else
		m_pOwner.lock()->SetAnim("em2000_dmg-low_050");
}

void CRekkaState_Groggy::ExitState()
{
	m_pOwner.lock()->m_fTimerGroggy = -1.f;
}
