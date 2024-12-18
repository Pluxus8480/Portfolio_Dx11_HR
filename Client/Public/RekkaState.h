#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CRekkaState abstract
{
public:
	enum class State {
		INTRO, IDLE, MOVE, ROTATE, GROGGY, ELECT, PIPE, PIPE_ATTACK, ATTACK, DAMAGED, DEAD, GIMMICK, END_STATE
	};

	virtual ~CRekkaState() {};

public:
	virtual CRekkaState* StepState(_float fTimeDelta) PURE;
	virtual void ActiveState(_float fTimeDelta) PURE;

	void ChangeState(CRekkaState* pCurState, CRekkaState* pNextState);

	virtual void EnterState() PURE;
	virtual void ExitState() PURE;

protected:
	weak_ptr<class CMonster_Rekka> m_pOwner;

	State m_state = State::END_STATE;
	State m_preState = State::END_STATE;

public:
	State GetState() const { return m_state; };
};

END