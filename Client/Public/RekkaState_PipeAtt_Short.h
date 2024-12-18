#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_PipeAtt_Short : public CRekkaState
{
public:
	CRekkaState_PipeAtt_Short(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	enum class EStep { NOTHING, ATTACK, LAST, STEP_END };
	EStep m_eStep = EStep::NOTHING;

public:
	void DashOn();
	void AttackOn();
private:
	enum class EAttackState { TARGETING, DASH_ON, ATTACK, DASH_OVER };
	EAttackState m_AttackState = EAttackState::TARGETING;

	_float m_fTargetSpeed;


private:
	_uint m_iCnt_Attack = 0;
};

END