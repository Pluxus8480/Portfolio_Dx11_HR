#include "RekkaState_Rotate.h"
#include "Monster_Rekka.h"

#include "RekkaState_Idle.h"
#include "RekkaState_Attack.h"

CRekkaState_Rotate::CRekkaState_Rotate(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_pOwner = pRekka;
	m_state = State::ROTATE;
	m_preState = preState;
}

CRekkaState* CRekkaState_Rotate::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (!isFinished)
		return nullptr;

	_vector vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_vTargetLook);
	m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);

	// m_vTargetLook 으로 보도록 설정
	if (m_pOwner.lock()->IsBeatAble() && m_pOwner.lock()->m_fTimer_Attack < 0.f)
		return new CRekkaState_Attack(m_pOwner, m_state);
	else
		return new CRekkaState_Idle(m_pOwner, m_state);
}

void CRekkaState_Rotate::ActiveState(_float fTimeDelta)
{
}

void CRekkaState_Rotate::EnterState()
{
	_float fTargetAngle = XM_PI;

	_float fAngleDegree = m_pOwner.lock()->m_fAngleDegree;
	if (abs(fAngleDegree) > 140.f)  // Turning
	{
		if (fAngleDegree < 0.f)
			m_strCurAnim = "em2000_mvnt-turn_004";
		else
			m_strCurAnim = "em2000_mvnt-turn_005";
	}
	else
	{
		if (fAngleDegree < 0.f)
		{
			m_strCurAnim = "em2000_mvnt-turn_002";
			fTargetAngle *= -0.5f;
		}
		else
		{
			m_strCurAnim = "em2000_mvnt-turn_003";
			fTargetAngle *= 0.5f;
		}
	}

	_vector vQuat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), fTargetAngle);
	XMStoreFloat4(&m_vTargetLook, XMVector3Rotate(m_pOwner.lock()->m_pTransformCom->Get_NormLook(), vQuat));

	m_pOwner.lock()->SetAnim(m_strCurAnim);
}

void CRekkaState_Rotate::ExitState()
{
}
