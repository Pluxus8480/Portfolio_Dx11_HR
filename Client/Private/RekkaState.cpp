#include "RekkaState.h"

void CRekkaState::ChangeState(CRekkaState* pCurState, CRekkaState* pNextState)
{
	if (nullptr != pCurState)
	{
		pCurState->ExitState();
		Safe_Delete(pCurState);
	}

	pNextState->EnterState();
}
