#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CSaberState abstract
{
public:
	enum class State {
		INTRO, IDLE, DOWN, MOVE, ATTACK, DAMAGED, DEAD, END_STATE
	};

	virtual ~CSaberState() {};

public:
	virtual CSaberState* StepState(_float fTimeDelta) PURE;
	virtual void ActiveState(_float fTimeDelta) PURE;

	void ChangeState(CSaberState* pCurState, CSaberState* pNextState);

	virtual void EnterState() PURE;
	virtual void ExitState() PURE;

protected:
	weak_ptr<class CMonster_Saber> m_pOwner;

	State m_state = State::END_STATE;
	State m_preState = State::END_STATE;

public:
	State GetState() const { return m_state; };

public:
	virtual _bool IsSlowAnim() { return false; };
	virtual _bool IsColliderOn() { return true; };
};

END