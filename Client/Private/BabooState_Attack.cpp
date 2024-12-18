#include "BabooState_Attack.h"
#include "GameInstance.h"
#include "Monster_Baboo.h"

#include "BabooState_Idle.h"
#include "BabooState_Move.h"

CBabooState_Attack::CBabooState_Attack(weak_ptr<class CMonster_Baboo> pBaboo, State preState)
{
    m_pOwner = pBaboo;
    m_state = State::ATTACK;
    m_preState = preState;
}

CBabooState* CBabooState_Attack::StepState(_float fTimeDelta)
{
    if (!m_pOwner.lock()->IsAnimFinished())
        return nullptr;

    if (m_pOwner.lock()->IsBeatAble())
        return new CBabooState_Move(m_pOwner, m_state);
    else
        return new CBabooState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CBabooState_Attack::ActiveState(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();
    _bool isAttackOn = pOwner->m_isAttackOn;

    if (m_strCurAnim == "em0400_shield-a_020")
    {
        if (m_fSpeedFixed > 0.f)
            pOwner->MoveStraight(m_fSpeedFixed);
        
        if(!m_isJumpMode)
        {
            _float fSpeedRotation = 2.f;
            auto vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
            pOwner->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
            }
    }
    else if (m_strCurAnim == "em0400_shield-a_000")
    {
        if (!isAttackOn)
        {
            _float fSpeedRotation = 3.f;
            auto vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
            pOwner->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
        }
    }
    else if (m_strCurAnim == "em0400_shield-a_050")
    {
        if (!isAttackOn)
        {
            _float fSpeedRotation = 3.f;
            auto vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
            pOwner->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
        }
    }
    else if (m_strCurAnim == "em0400_shield-a_030")
    {

    }
    else
    {
        _float fSpeedRotation = 2.f;
        auto vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
        pOwner->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
    }
}

void CBabooState_Attack::EnterState()
{
    // Init Attack Timer
    auto iTimeDelta = GAMEINSTANCE->Random_Int(1, 5);
    m_pOwner.lock()->m_fTimer_Attack = m_pOwner.lock()->m_fTimer_Attack_C + iTimeDelta;

    SelectAttack();
}

void CBabooState_Attack::ExitState()
{
}

void CBabooState_Attack::SelectAttack()
{
    auto pOwner = m_pOwner.lock();
    _float m_fDistance = pOwner->m_fDistance;
    _float m_fAngle = pOwner->m_fAngleDegree;
    if (m_fDistance < 4.f)
    {
        if (m_fAngle >= 0.f)
            m_strCurAnim = "em0400_shield-a_040";
        else
            m_strCurAnim = "em0400_shield-a_041";
        pOwner->SetAnim(m_strCurAnim);
    }
    else if (m_fDistance < 6.f)
    {
        m_strCurAnim = "em0400_shield-a_000";
        pOwner->SetAnim(m_strCurAnim);
    }
    else if (m_fDistance < 8.f)
    {
        m_strCurAnim = "em0400_shield-a_050";
        pOwner->SetAnim(m_strCurAnim);
    }
    else if (m_fDistance < 20.f)
    {
        m_strCurAnim = "em0400_shield-a_020";
        pOwner->SetAnim(m_strCurAnim);
    }
    else
        pOwner->SetAnim("em0400_shield-a_030");
}

void CBabooState_Attack::JumpOn()
{
    m_isJumpMode = true;
    m_fSpeedFixed = m_pOwner.lock()->m_fDistance * 1.2f;
}

void CBabooState_Attack::JumpOff()
{
    m_fSpeedFixed = 0.f;
}
