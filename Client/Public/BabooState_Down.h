#pragma once

#include "BabooState.h"

BEGIN(Client)

class CBabooState_Down : public CBabooState
{
public:
	CBabooState_Down(weak_ptr<class CMonster_Baboo> pBaboo, State preState);

public:
	CBabooState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

public:
	virtual _bool IsColliderOn() override;

private:
	string m_strCurAnim;
};

END