#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Pase_4 : public CRekkaState
{
public:
	CRekkaState_Pase_4(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;
};

END