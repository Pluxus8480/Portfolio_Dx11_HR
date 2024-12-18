#pragma once

#include "GunnerState.h"

BEGIN(Client)

class CGunnerState_Down : public CGunnerState
{
public:
	CGunnerState_Down(weak_ptr<class CMonster_Gunner> pGunner, State preState);

public:
	CGunnerState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

public:
	virtual _bool IsColliderOn() override;

private:
	string m_strCurAnim;
};

END