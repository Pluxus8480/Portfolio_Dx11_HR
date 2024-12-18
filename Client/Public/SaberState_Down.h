#pragma once

#include "SaberState.h"

BEGIN(Client)

class CSaberState_Down : public CSaberState
{
public:
	CSaberState_Down(weak_ptr<class CMonster_Saber> pSaber, State preState);

public:
	CSaberState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

public:
	virtual _bool IsColliderOn() override;

private:
	string m_strCurAnim;
};

END