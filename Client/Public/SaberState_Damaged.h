#pragma once

#include "SaberState.h"

BEGIN(Client)

class CSaberState_Damaged : public CSaberState
{
public:
	CSaberState_Damaged(weak_ptr<class CMonster_Saber> pSaber, State preState, const DamageInfo& damageInfo);

public:
	CSaberState* StepState(_float fTimeDelta) override;
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