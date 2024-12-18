#include "BabooState_Intro.h"
#include "Monster_Baboo.h"

#include "Model.h"
#include "CharacterController.h"

#include "Beat_Manager.h"

#include "BabooState_Idle.h"

CBabooState_Intro::CBabooState_Intro(weak_ptr<class CMonster_Baboo> pBaboo, State preState)
{
    m_state = State::INTRO;
    m_pOwner = pBaboo;
    m_preState = preState;
}
CBabooState_Intro::CBabooState_Intro(weak_ptr<class CMonster_Baboo> pBaboo, State preState, _fvector vTargetPos)
{
    m_state = State::INTRO;
    m_pOwner = pBaboo;
    m_preState = preState;

    XMStoreFloat4(&m_vTargetPos, vTargetPos);
}

CBabooState* CBabooState_Intro::StepState(_float fTimeDelta)
{
    _bool isFinished = m_pOwner.lock()->IsAnimFinished();
    if (isFinished && m_strCurAnim == "em0400_mvnt-jump_053")
        return new CBabooState_Idle(m_pOwner, m_state);

    return nullptr;
}

void CBabooState_Intro::ActiveState(_float fTimeDelta)
{
    _bool isFinished = m_pOwner.lock()->IsAnimFinished();
    if (m_strCurAnim == "em0400_mvnt-jump_051")
    {
        if (isFinished)
        {
            m_strCurAnim = "em0400_mvnt-jump_052";
            m_pOwner.lock()->SetAnim(m_strCurAnim, true);
        }
    }

    _float fTargetTime = m_fCurTime + fTimeDelta;
    if (m_fCurTime <= m_fCCT_Time && fTargetTime >= m_fCCT_Time)
    {
        m_pOwner.lock()->CCT_ON();
        m_pOwner.lock()->m_pCCT->Set_FootPosition(m_pOwner.lock()->m_pTransformCom->Get_WorldMatrix_XMMat());
    }
    m_fCurTime = fTargetTime;

    if (m_pOwner.lock()->m_isCCTWork)
    {
        m_pOwner.lock()->MoveStraight(m_fVelocity);
    }
    else
    {
        _float fRatio = m_fCurTime / m_fTotalTime;
        _float fNewHeight = -24.f * fRatio * (fRatio - 1.f) + m_vStartPos.y;
        auto vNewPos = XMVectorLerp(XMLoadFloat4(&m_vStartPos), XMLoadFloat4(&m_vTargetPos), fRatio);
        vNewPos = XMVectorSetY(vNewPos, fNewHeight);
        vNewPos = XMVectorSetW(vNewPos, 1.f);
        m_pOwner.lock()->m_pTransformCom->Set_Position(vNewPos);
    }
}

void CBabooState_Intro::EnterState()
{
    // Set Speed
    m_pOwner.lock()->m_pTransformCom->Set_Speed(1.f);

    // Rotate to target position
    _vector vCurPos = m_pOwner.lock()->m_pTransformCom->Get_Position();
    _vector vTargetPos = XMLoadFloat4(&m_vTargetPos);
    _vector vDir = XMVectorSetW(XMVector3Normalize(vTargetPos - vCurPos), 0.f);
    m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vCurPos + vDir);

    XMStoreFloat4(&m_vStartPos, vCurPos);
    XMStoreFloat4(&m_vDir, m_pOwner.lock()->m_pTransformCom->Get_NormLook());

    m_strCurAnim = "em0400_mvnt-jump_051";
    m_pOwner.lock()->SetAnim(m_strCurAnim);

    // Set time
    m_fTotalTime = CBeat_Manager::Get_Instance()->Get_Beat() * 4.f;
    m_fCurTime = 0.f;
    m_fCCT_Time = m_fTotalTime * 0.9f;

    _float vDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(vTargetPos - vCurPos, 0.f)));
    m_fVelocity = vDistance / m_fTotalTime;
}

void CBabooState_Intro::ExitState()
{
}

void CBabooState_Intro::FinishLifting()
{
    m_strCurAnim = "em0400_mvnt-jump_053";
    m_pOwner.lock()->SetAnim(m_strCurAnim);
    m_fVelocity = 0.f;
}

_bool CBabooState_Intro::IsOver()
{
    _vector vCurPos = m_pOwner.lock()->m_pTransformCom->Get_Position();
    _vector vTargetPos = XMLoadFloat4(&m_vTargetPos);

    _vector vCurDir = XMVectorSetY(vTargetPos - vCurPos, 0.f);
    _vector vDir = XMLoadFloat4(&m_vDir);

    return XMVectorGetX(XMVector3Dot(vCurDir, vDir)) < 0.f;
}
