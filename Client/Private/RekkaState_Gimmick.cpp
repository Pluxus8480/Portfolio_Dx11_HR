#include "RekkaState_Gimmick.h"
#include "Monster_Rekka.h"

#include "Beat_Manager.h"

#include "RekkaState_Pipe.h"


CRekkaState_Gimmick::CRekkaState_Gimmick(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_state = State::GIMMICK;
	m_pOwner = pRekka;
	m_preState = preState;
}

CRekkaState* CRekkaState_Gimmick::StepState(_float fTimeDelta)
{
	return nullptr;
}

void CRekkaState_Gimmick::ActiveState(_float fTimeDelta)
{
}

void CRekkaState_Gimmick::EnterState()
{

}

void CRekkaState_Gimmick::ExitState()
{
}
