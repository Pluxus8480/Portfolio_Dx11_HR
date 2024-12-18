#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Pipe : public CRekkaState
{
private:
	enum class EPipeStep { NOTHING, ROT, MOVE, ROT_2, STEP_END };
private:
	EPipeStep m_curStep = EPipeStep::NOTHING;

public:
	CRekkaState_Pipe(weak_ptr<class CMonster_Rekka> pRekka, State preState, _bool isLong);
	CRekkaState_Pipe(weak_ptr<class CMonster_Rekka> pRekka, State preState, _bool isLong, EPipeStep curStep);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	_bool m_isLong = false;

private:
	_float m_fVelocity;
};

END