#pragma once

#include "RekkaState.h"

BEGIN(Client)

class CRekkaState_PipeAtt_Long : public CRekkaState
{
public:
	CRekkaState_PipeAtt_Long(weak_ptr<class CMonster_Rekka> pRekka, State preState);

public:
	CRekkaState* StepState(_float fTimeDelta) override;
	void ActiveState(_float fTimeDelta) override;
	void EnterState() override;
	void ExitState() override;

private:
	enum class EStep { NOTHING, ROT, STEP_END };
	EStep m_eStep = EStep::NOTHING;

private:
	const _uint m_iTargetBeat_C = 20;
	_int m_iCurBeat = -1;
	_int m_iBeatCnt = 0;
};

END