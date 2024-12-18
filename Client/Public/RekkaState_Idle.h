#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Idle : public CRekkaState
{
public:
	CRekkaState_Idle(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;
};

END