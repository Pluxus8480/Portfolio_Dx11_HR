#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Groggy : public CRekkaState
{
public:
	CRekkaState_Groggy(weak_ptr<class CMonster_Rekka> pRekka, State preState);
	CRekkaState_Groggy(weak_ptr<class CMonster_Rekka> pRekka, State preState, EAttackDirection eAttDir);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	_bool m_isDamaged = false;
	EAttackDirection m_eAttDir = EAttackDirection::DEFAULT;
};

END