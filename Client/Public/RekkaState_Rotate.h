#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Rotate : public CRekkaState
{
public:
	CRekkaState_Rotate(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	string m_strCurAnim;

	_float4 m_vTargetLook;
};

END