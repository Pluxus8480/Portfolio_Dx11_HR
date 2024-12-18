#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Pase_3 : public CRekkaState
{
public:
	CRekkaState_Pase_3(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	string m_strCurAnim;

private:
	enum class EStep {
		Damaged, Down, WakeUp, DashOpen, End_Step
	};
	EStep m_eCurStep = EStep::Damaged;
};

END