#include "RekkaState_PipeAtt_Long.h"
#include "Monster_Rekka.h"

#include "Beat_Manager.h"

#include "RekkaState_Idle.h"

CRekkaState_PipeAtt_Long::CRekkaState_PipeAtt_Long(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_state = State::PIPE_ATTACK;
	m_pOwner = pRekka;
	m_preState = preState;
}

CRekkaState* CRekkaState_PipeAtt_Long::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && EStep::STEP_END == m_eStep)
		return new CRekkaState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CRekkaState_PipeAtt_Long::ActiveState(_float fTimeDelta)
{
	// Look Player
	_float fSpeedRotation = 1.5f;
	auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
	m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);

	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	switch (m_eStep)
	{
		case EStep::NOTHING:
		{
			if (isFinished)
			{
				m_eStep = EStep::ROT;
				m_pOwner.lock()->SetAnim("em2000_atk-pipe_101", true);

				m_pOwner.lock()->m_curAttackBone = CMonster_Rekka::EAttackBone::Body;
				m_pOwner.lock()->m_fAttackScale = 4.5f;
				m_pOwner.lock()->m_isAttackOn = true;
			}
		}
		case EStep::ROT:
		{
			m_pOwner.lock()->m_isAttackOn = true;

			_uint iBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
			if (m_iCurBeat != iBeat)
			{
				m_iCurBeat = iBeat;
				m_iBeatCnt++;

				m_pOwner.lock()->m_isAttackOn = false;
			}

			if (m_iBeatCnt == m_iTargetBeat_C)
			{
				m_eStep = EStep::STEP_END;
				m_pOwner.lock()->SetAnim("em2000_atk-pipe_102");
			}
			break;
		}
	}
}

void CRekkaState_PipeAtt_Long::EnterState()
{
	m_pOwner.lock()->SetAnim("em2000_atk-pipe_100");
}

void CRekkaState_PipeAtt_Long::ExitState()
{
}
