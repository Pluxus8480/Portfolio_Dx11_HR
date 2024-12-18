#include "RekkaState_Intro.h"
#include "GameInstance.h"
#include "Monster_Rekka.h"

#include "Player_Camera.h"
#include "Model.h"

#include "RekkaState_Idle.h"


CRekkaState_Intro::CRekkaState_Intro(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
	m_pOwner = pRekka;
	m_preState = preState;
	m_state = State::INTRO;
}

CRekkaState* CRekkaState_Intro::StepState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished && m_strCurAnim == "CS0230_00_029_em2000_00")
		return new CRekkaState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CRekkaState_Intro::ActiveState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished)
		PlayNextAnim();
}

void CRekkaState_Intro::EnterState()
{
	// Animation List
	m_strCurAnim = "CS0230_00_025_em2000_00";
	m_pOwner.lock()->SetAnim(m_strCurAnim);

	static_pointer_cast<CCamera_Player> (GAMEINSTANCE->Get_MainCamera())->Start_Following_Bone(
		m_pOwner.lock()->m_pTransformCom->Get_WorldMatrix_Ptr(),
		const_cast <_float4x4*>(m_pOwner.lock()->m_pModelCom->Get_BoneMatrixPtr("attach_cam")));
}

void CRekkaState_Intro::ExitState()
{
}

void CRekkaState_Intro::PlayNextAnim()
{
	if (m_strCurAnim == "CS0230_00_025_em2000_00")
	{
		m_strCurAnim = "CS0230_00_026_em2000_00";
	}
	else if (m_strCurAnim == "CS0230_00_026_em2000_00")
	{
		m_strCurAnim = "CS0230_00_027_em2000_00";
	}
	else if (m_strCurAnim == "CS0230_00_027_em2000_00")
	{
		m_strCurAnim = "CS0230_00_028_em2000_00";
	}
	else if (m_strCurAnim == "CS0230_00_028_em2000_00")
	{
		m_strCurAnim = "CS0230_00_029_em2000_00";
	}
	m_pOwner.lock()->SetAnim(m_strCurAnim);
}
