#pragma once

#include "SaberState.h"

BEGIN(Client)

class CSaberState_Intro : public CSaberState
{
public:
	CSaberState_Intro(weak_ptr<class CMonster_Saber> pSaber, State preState);
	CSaberState_Intro(weak_ptr<class CMonster_Saber> pSaber, State preState, _fvector vTargetPos);

public:
	CSaberState* StepState(_float fTimeDelta) override;
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