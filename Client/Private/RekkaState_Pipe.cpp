#include "RekkaState_Pipe.h"
#include "Monster_Rekka.h"

#include "Beat_Manager.h"

#include "Model.h"

#include "RekkaState_PipeAtt_Long.h"
#include "RekkaState_PipeAtt_Short.h"

CRekkaState_Pipe::CRekkaState_Pipe(weak_ptr<class CMonster_Rekka> pRekka, State preState, _bool isLong = false)
{
	m_pOwner = pRekka;
	m_state = State::PIPE;
	m_preState = preState;

	m_isLong = isLong;
}
CRekkaState_Pipe::CRekkaState_Pipe(weak_ptr<class CMonster_Rekka> pRekka, State preState, _bool isLong, EPipeStep curStep)
{
	m_pOwner = pRekka;
	m_state = State::PIPE;
	m_preState = preState;
	m_isLong = isLong;
	m_curStep = curStep;
}

CRekkaState* CRekkaState_Pipe::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (!isFinished)
		return nullptr;

	if (m_curStep == EPipeStep::MOVE)
		return new CRekkaState_Pipe(m_pOwner, m_state, m_isLong, m_curStep);

	if (m_curStep == EPipeStep::STEP_END)
	{
		// Rotate 180
		_vector vQuat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XM_PI);
		_vector vTargetLook = XMVector3Rotate(m_pOwner.lock()->m_pTransformCom->Get_NormLook(), vQuat);
		vTargetLook = vTargetLook + m_pOwner.lock()->m_pTransformCom->Get_Position();
		m_pOwner.lock()->m_pTransformCom->LookAt(vTargetLook);

		if (m_isLong)
			return new CRekkaState_PipeAtt_Long(m_pOwner, m_state);
		else
			return new CRekkaState_PipeAtt_Short(m_pOwner, m_state);
	}

	return nullptr;
}

void CRekkaState_Pipe::ActiveState(_float fTimeDelta)
{
	// Moving State
	if (m_curStep == EPipeStep::MOVE)
		m_pOwner.lock()->MoveStraight(m_fVelocity);
}

void CRekkaState_Pipe::EnterState()
{
	auto pOwner = m_pOwner.lock();

	// 기본 위치
	_vector vTargetPos;
	_vector vTargetDir;
	if (m_isLong)
	{
		vTargetPos = { 1181.41f, 487.38f, 130.573f, 1.f };
		vTargetDir = { 0.f, 0.f, 1.f, 0.f };
	}
	else
	{
		vTargetPos = { 1181.41f, 487.38f, 92.18f, 1.f };
		vTargetDir = { 0.f, 0.f, -1.f, 0.f };
	}

	_vector vCurLook = pOwner->m_pTransformCom->Get_NormLook();
	_vector vDirToTarget = XMVectorSetY(vTargetPos - pOwner->m_pTransformCom->Get_Position(), 0.f);
	_float fDistance = XMVectorGetX(XMVector4Length(vDirToTarget));
	vDirToTarget = XMVector3Normalize(vDirToTarget);

	_float fCrossY = XMVectorGetY(XMVector3Cross(vCurLook, vDirToTarget));
	_float fDot = XMVectorGetX(XMVector3Dot(vCurLook, vDirToTarget));
	_float fAngleDegree = acosf(fDot);
	if (fCrossY < 0.f)
		fAngleDegree *= -1.f;

	
	switch (m_curStep)
	{
		case EPipeStep::NOTHING:
		{
			m_curStep = EPipeStep::MOVE;
	
			_vector vQuat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), fAngleDegree);
			_vector vTargetLook = XMVector3Rotate(pOwner->m_pTransformCom->Get_NormLook(), vQuat);
			vTargetLook = vTargetLook + m_pOwner.lock()->m_pTransformCom->Get_Position();
			m_pOwner.lock()->m_pTransformCom->LookAt(vTargetLook);
	
			m_pOwner.lock()->SetAnim("em2000_mvnt-jump_001");

			_float fTotalBeat = m_pOwner.lock()->m_pModelCom->Get_Duration() / 15.f;
			_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
			_float fDuration = fTotalBeat * fBeat;
			m_fVelocity = fDistance / fDuration;
	
			break;
		}
		case EPipeStep::MOVE:
		{
			m_curStep = EPipeStep::STEP_END;

			_vector vTargetLook = vTargetDir;
			vTargetLook = vTargetLook + m_pOwner.lock()->m_pTransformCom->Get_Position();
			m_pOwner.lock()->m_pTransformCom->LookAt(vTargetLook);

			if (m_isLong)
				m_pOwner.lock()->SetAnim("em2000_atk-pipe_020");
			else
				m_pOwner.lock()->SetAnim("em2000_atk-pipe_030");

			break;
		}
	}
}

void CRekkaState_Pipe::ExitState()
{
}
