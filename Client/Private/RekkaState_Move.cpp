#include "RekkaState_Move.h"
#include "GameInstance.h"
#include "Monster_Rekka.h"

#include "RekkaState_Attack.h"

#include "Beat_Manager.h"

CRekkaState_Move::CRekkaState_Move(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_pOwner = pRekka;
	m_state = State::MOVE;
	m_preState = preState;
}

CRekkaState* CRekkaState_Move::StepState(_float fTimeDelta)
{
	if (m_pOwner.lock()->m_fTimer_Attack <= 0.f && m_pOwner.lock()->IsBeatAble())
	{
		if (m_isDucking)
		{
			if (m_pOwner.lock()->IsAnimFinished())
				return new CRekkaState_Attack(m_pOwner, m_state);
		}
		else
			return new CRekkaState_Attack(m_pOwner, m_state);
	}

	return nullptr;
}

void CRekkaState_Move::ActiveState(_float fTimeDelta)
{
	if (m_isDucking)
	{
		// look with speed
		_float fSpeedRotation = 5.f;
		auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
		m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);

		_bool isFinished = m_pOwner.lock()->IsAnimFinished();
		if (isFinished)
			SelectMoveAnim();

		return;
	}

	_uint iBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iCurBeat != iBeat)
	{
		m_iCurBeat = iBeat;
		m_iBeatCnt++;
	}

	if (m_iBeatCnt >= m_iTargetBeat)
		SelectMoveAnim();

	// look with speed
	_float fSpeedRotation = 2.5f;
	auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
	m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);

	// Real movement
	if (m_strCurAnim == "em2000_mvnt-walk_000")
		m_pOwner.lock()->MoveStraight(2.f);
	else if (m_strCurAnim == "em2000_mvnt-walk_001")
		m_pOwner.lock()->MoveStraight(-2.f);
	else if (m_strCurAnim == "em2000_mvnt-walk_002")
		m_pOwner.lock()->MoveRight(-2.f);
	else if (m_strCurAnim == "em2000_mvnt-walk_003")
		m_pOwner.lock()->MoveRight(2.f);
}

void CRekkaState_Move::EnterState()
{
	m_iDuckingCnt = 0;
	SelectMoveAnim();
}

void CRekkaState_Move::ExitState()
{
}

void CRekkaState_Move::SelectMoveAnim()
{
	_float fDistance = m_pOwner.lock()->m_fDistance;
	if ((fDistance > 10.f || GAMEINSTANCE->Random_Bool()) && m_iDuckingCnt < 2)
	{
		m_isDucking = true;
		++m_iDuckingCnt;
		m_iBeatCnt = m_iTargetBeat;
		SelectDucking(fDistance > 10.f);
		return;
	}

	m_isDucking = false;

	m_iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (CBeat_Manager::Get_Instance()->Get_Timer() > CBeat_Manager::Get_Instance()->Get_Beat() * 0.9f)
		m_iBeatCnt = -1;
	else
		m_iBeatCnt = 0;

	if (fDistance > 3.f)
	{
		m_iTargetBeat = 2;
		m_strCurAnim = "em2000_mvnt-walk_000";
		m_pOwner.lock()->SetAnim(m_strCurAnim, true);
	}
	else
	{
		m_iTargetBeat = 2;
		switch (GAMEINSTANCE->Random_Int(0, 2))
		{
		case 0:
			m_strCurAnim = "em2000_mvnt-walk_001";
			break;
		case 1:
			m_strCurAnim = "em2000_mvnt-walk_002";
			break;
		case 2:
			m_strCurAnim = "em2000_mvnt-walk_003";
			break;
		}
		m_pOwner.lock()->SetAnim(m_strCurAnim, true);
	}
}

void CRekkaState_Move::SelectDucking(_bool isFoward)
{
	if (isFoward)
		m_strCurAnim = "em2000_mvnt-escape_040";
	else if (m_strCurAnim == "em2000_mvnt-escape_040")
	{
		if (GAMEINSTANCE->Random_Binary())
			m_strCurAnim = "em2000_mvnt-escape_042";
		else
			m_strCurAnim = "em2000_mvnt-escape_043";
	}
	else
	{
		auto iValue = GAMEINSTANCE->Random_Int(0, 2);
		if (iValue == 0)
			m_strCurAnim = "em2000_mvnt-escape_040";
		else if (iValue == 1)
			m_strCurAnim = "em2000_mvnt-escape_042";
		else
			m_strCurAnim = "em2000_mvnt-escape_043";
	}
	m_pOwner.lock()->SetAnim(m_strCurAnim);
}
