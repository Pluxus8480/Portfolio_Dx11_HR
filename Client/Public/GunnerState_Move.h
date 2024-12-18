#pragma once

#include "GunnerState.h"

BEGIN(Client)

class CGunnerState_Move : public CGunnerState
{
public:
	CGunnerState_Move(weak_ptr<class CMonster_Gunner> pGunner, State preState);

public:
	CGunnerState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	void SelectMoveAnim();

private:
	_float m_fDistance_Close = 0.f;
	_float m_fDistance_Too_Close = 0.f;

private:
	_float m_fCurTimer = 0.f;
	_bool m_isActivingShort = false;

	_float4 m_vTargetPos;
};

END