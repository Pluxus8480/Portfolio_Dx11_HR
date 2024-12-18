#pragma once

#include "GunnerState.h"

BEGIN(Client)

class CGunnerState_Intro : public CGunnerState
{
public:
	CGunnerState_Intro(weak_ptr<class CMonster_Gunner> pGunner, State preState);
	CGunnerState_Intro(weak_ptr<class CMonster_Gunner> pGunner, State preState, _fvector vTargetPos);

public:
	CGunnerState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

public:
	void FinishLifting();

private:
	_bool IsOver();

private:
	_float4 m_vStartPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 m_vTargetPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 m_vDir = { 0.f, 0.f, 0.f, 0.f };

	_float m_fTotalTime = 0.f;
	_float m_fCurTime = 0.f;
	_float m_fCCT_Time = 0.f;

	_float m_fVelocity = 0.f;

private:
	string m_strCurAnim;
};

END