#include "BabooState_Move.h"
#include "GameInstance.h"
#include "Monster_Baboo.h"

#include "BabooState_Attack.h"

#include "Beat_Manager.h"


CBabooState_Move::CBabooState_Move(weak_ptr<class CMonster_Baboo> pBaboo, State preState)
{
    m_pOwner = pBaboo;
    m_state = State::MOVE;
    m_preState = preState;
}

CBabooState* CBabooState_Move::StepState(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();
    _bool isBeatAble = pOwner->IsBeatAble();
    if (!isBeatAble)
        return nullptr;

    if (pOwner->m_fTimer_Attack <= 0.f)
        return new CBabooState_Attack(m_pOwner, m_state);

	return nullptr;
}

void CBabooState_Move::ActiveState(_float fTimeDelta)
{
    _uint iBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    if (m_iCurBeat != iBeat)
    {
        m_iCurBeat = iBeat;
        m_iBeatCnt++;
    }

    if (m_iBeatCnt >= m_iTargetBeat)
        SelectMoveAnim();
    auto pOwner = m_pOwner.lock();

    if (m_isLookStrong)
    {
        _vector vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
        pOwner->m_pTransformCom->LookAt_Horizontal(vTargetPos);
    }
    else
    {
        _float fSpeedRotation = 1.5f;
        auto vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
        pOwner->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
    }

    if (m_strCurAnim == "em0400_mvnt-walk-st_010")
    {
        pOwner->MoveStraight(4.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_001")
    {
        pOwner->MoveStraight(-2.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_012")
    {
        pOwner->MoveRight(-2.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_013")
    {
        pOwner->MoveRight(2.f);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_006")
    {
        _float fSpeed = sqrtf(2.f);
        pOwner->MoveStraight(-fSpeed);
        pOwner->MoveRight(-fSpeed);
    }
    else if (m_strCurAnim == "em0400_mvnt-walk-st_007")
    {
        _float fSpeed = sqrtf(2.f);
        pOwner->MoveStraight(-fSpeed);
        pOwner->MoveRight(fSpeed);
    }
}

void CBabooState_Move::EnterState()
{
    m_pOwner.lock()->m_fTimeMultiflier = 0.7f;

    SelectMoveAnim();
}

void CBabooState_Move::ExitState()
{
    m_pOwner.lock()->m_fTimeMultiflier = 1.f;
}

void CBabooState_Move::SelectMoveAnim()
{
    m_iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    if (CBeat_Manager::Get_Instance()->Get_Timer() > CBeat_Manager::Get_Instance()->Get_Beat() * 0.9f)
        m_iBeatCnt = -1;
    else
        m_iBeatCnt = 0;
    auto pOwner = m_pOwner.lock();
    _float fDistance = pOwner->m_fDistance;
    if (fDistance > pOwner->m_fDistance_Far_C)
    {
        m_iTargetBeat = 2;
        m_isLookStrong = false;

        m_strCurAnim = "em0400_mvnt-walk-st_010";
        pOwner->SetAnim(m_strCurAnim, true);
    }
    else if (fDistance < pOwner->m_fDistance_Near_C)
    {
        m_iTargetBeat = 1;
        m_isLookStrong = true;

        switch (GAMEINSTANCE->Random_Int(0, 2))
        {
        case 0:
            m_strCurAnim = "em0400_mvnt-walk-st_001";
            break;
        case 1:
            m_strCurAnim = "em0400_mvnt-walk-st_006";
            break;
        case 2:
            m_strCurAnim = "em0400_mvnt-walk-st_007";
            break;
        }
        pOwner->SetAnim(m_strCurAnim, true);
    }
    else
    {
        m_iTargetBeat = 3;
        m_isLookStrong = true;

        switch (GAMEINSTANCE->Random_Int(0, 1))
        {
        case 0:
            m_strCurAnim = "em0400_mvnt-walk-st_012";
            break;
        case 1:
            m_strCurAnim = "em0400_mvnt-walk-st_013";
            break;
        }
        pOwner->SetAnim(m_strCurAnim, true);
    }
}
