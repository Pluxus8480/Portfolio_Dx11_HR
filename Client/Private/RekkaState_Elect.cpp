#include "RekkaState_Elect.h"
#include "Monster_Rekka.h"

#include "Beat_Manager.h"

#include "Model.h"

#include "RekkaState_Idle.h"

CRekkaState_Elect::CRekkaState_Elect(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_state = State::ELECT;
	m_pOwner = pRekka;
	m_preState = preState;
}

CRekkaState* CRekkaState_Elect::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_eStep == EStep::Elect)
		return new CRekkaState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CRekkaState_Elect::ActiveState(_float fTimeDelta)
{
	if (m_eStep == EStep::Move)
	{
		if (m_pOwner.lock()->IsAnimFinished())
		{
			m_eStep = EStep::Elect;

			_vector vTargetDir = { -1.f, 0.f, 0.f, 0.f };
			_vector vCurPos = m_pOwner.lock()->m_pTransformCom->Get_Position();
			vCurPos += vTargetDir;
			m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vCurPos);

			m_pOwner.lock()->SetAnim("em2000_atk_150");
		}
		else
		{
			m_pOwner.lock()->MoveStraight(m_fVelocity);
		}
	}
}

void CRekkaState_Elect::EnterState()
{
	m_eStep = EStep::Move;

	_vector vTargetPos = { 1181.41f, 485.374f, 111.367f, 1.f };
	_vector vCurPos = m_pOwner.lock()->m_pTransformCom->Get_Position();
	m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);

	m_pOwner.lock()->SetAnim("em2000_mvnt-jump_001");

	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(vTargetPos - vCurPos, 0.f)));
	_float fTotalBeat = m_pOwner.lock()->m_pModelCom->Get_Duration() / 15.f;
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
	_float fDuration = fTotalBeat * fBeat;
	m_fVelocity = fDistance / fDuration;
}

void CRekkaState_Elect::ExitState()
{
}
