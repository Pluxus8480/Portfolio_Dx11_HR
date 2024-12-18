#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_Move : public CRekkaState
{
public:
	CRekkaState_Move(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	void SelectMoveAnim();
	void SelectDucking(_bool isFoward);

private:
	_uint m_iCurBeat = 0;

	_int m_iTargetBeat = 3;
	_int m_iBeatCnt = 0;

private:
	_bool m_isDucking = false;
	_uint m_iDuckingCnt = 0;

private:
	string m_strCurAnim = "";

};

END