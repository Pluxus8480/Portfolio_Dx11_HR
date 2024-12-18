#include "RekkaState_PipeAtt_Short.h"
#include "Monster_Rekka.h"

#include "RekkaState_Idle.h"

CRekkaState_PipeAtt_Short::CRekkaState_PipeAtt_Short(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_state = State::PIPE_ATTACK;
	m_pOwner = pRekka;
	m_preState = preState;
}

CRekkaState* CRekkaState_PipeAtt_Short::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (EStep::STEP_END == m_eStep && isFinished)
		return new CRekkaState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CRekkaState_PipeAtt_Short::ActiveState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	switch (m_eStep)
	{
		case EStep::ATTACK:
		{
			_float fSpeedRotation = 3.f;
			switch (m_AttackState)
			{
				case EAttackState::TARGETING:
				{
					// Smooth Rotation
					auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
					m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
					break;
				}
				case EAttackState::DASH_ON:
				{
					// Smooth Rotation
					auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
					m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);

					// Set Move Speed
					m_fTargetSpeed = m_pOwner.lock()->m_fDistance * 2.f;
					break;
				}
			}

			// Position
			m_pOwner.lock()->MoveStraight(m_fTargetSpeed);

			if (isFinished)
			{
				if (m_iCnt_Attack == 2)
				{
					m_pOwner.lock()->SetAnim("em2000_atk-pipe_205");
					m_eStep = EStep::LAST;
				}
				else
				{
					m_AttackState = EAttackState::TARGETING;
					m_fTargetSpeed = 0.f;

					m_pOwner.lock()->SetAnimHard("em2000_atk-pipe_210");
					++m_iCnt_Attack;
				}
			}
			break;
		}
		case EStep::LAST:
		{
			if (isFinished)
			{
				m_pOwner.lock()->SetAnim("em2000_atk-pipe_050");
				m_eStep = EStep::STEP_END;
			}
			break;
		}
	}
}

void CRekkaState_PipeAtt_Short::EnterState()
{
	m_eStep = EStep::ATTACK;

	m_AttackState = EAttackState::TARGETING;
	m_fTargetSpeed = 0.f;

	m_pOwner.lock()->SetAnim("em2000_atk-pipe_201");
}

void CRekkaState_PipeAtt_Short::ExitState()
{
}

void CRekkaState_PipeAtt_Short::DashOn()
{
	m_AttackState = EAttackState::DASH_ON;
	m_fTargetSpeed = m_pOwner.lock()->m_fDistance * 2.f;
}

void CRekkaState_PipeAtt_Short::AttackOn()
{
	m_AttackState = EAttackState::ATTACK;
	m_fTargetSpeed = m_pOwner.lock()->m_fDistance * 3.f;
}
