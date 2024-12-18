#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Attack : public CRekkaState
{
public:
	CRekkaState_Attack(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	string m_strCurAnim;

private: // For Sequence Dash
	_int		m_iSeqDash_Cnt = 0;

public:	// For Dash
	void DashOn();
	void DashOff() { m_dashState = EDashState::DASH_OVER; };
private:
	enum class EDashState { TARGETING, DASH_ON, DASH_OVER };
	EDashState m_dashState = EDashState::TARGETING;
	_float m_fDashSpeed = 0.f;

public: // For Fist_Down
	void FistJumpOn();
	void FistJumpOff();
	void FistDown();
private:
	enum class EFistState { TARGETING, JUMP_ON, JUMPING, DOWN, FIST_OVER };
	EFistState m_fistState = EFistState::TARGETING;
	_float m_fFistDownSpeed = 0.f;

public: // For Electric Range
	void GoToCenter();
private:
	_float m_fElectGoSpeed = 0.f;
};

END