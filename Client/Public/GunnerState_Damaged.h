#pragma once

#include "GunnerState.h"

BEGIN(Client)

class CGunnerState_Damaged : public CGunnerState
{
public:
	CGunnerState_Damaged(weak_ptr<class CMonster_Gunner> pGunner, State preState, const DamageInfo& damageInfo);

public:
	CGunnerState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	DamageInfo m_damageInfo;

public:
	_bool DoingAirBone() const { return m_doing_AirBone; }
private:
	_bool m_doing_AirBone = false;

public:
	virtual _bool IsSlowAnim() override;
	virtual _bool IsColliderOn() override;

private:
	string m_strCurAnim;
};

END