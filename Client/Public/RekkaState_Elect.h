#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Elect : public CRekkaState
{
public:
	CRekkaState_Elect(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	enum class EStep {
		Default, Move, Elect
	};
	EStep m_eStep = EStep::Default;

private:
	_float m_fVelocity;
};

END