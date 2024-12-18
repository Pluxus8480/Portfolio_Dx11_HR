#include "SaberState_Intro.h"
#include "Monster_Saber.h"

#include "Model.h"
#include "CharacterController.h"

#include "Beat_Manager.h"

#include "SaberState_Idle.h"

CSaberState_Intro::CSaberState_Intro(weak_ptr<class CMonster_Saber> pSaber, State preState)
{
    m_state = State::INTRO;
    m_pOwner = pSaber;
    m_preState = preState;
}

CSaberState_Intro::CSaberState_Intro(weak_ptr<class CMonster_Saber> pSaber, State preState, _fvector vTargetPos)
{
    m_state = State::INTRO;
    m_pOwner = pSaber;
    m_preState = preState;

    XMStoreFloat4(&m_vTargetPos, vTargetPos);
}

CSaberState* CSaberState_Intro::StepState(_float fTimeDelta)
{
    _bool isFinished = m_pOwner.lock()->IsAnimFinished();
    if (isFinished && m_strCurAnim == "em0100_event_022")
        return new CSaberState_Idle(m_pOwner, m_state);

    return nullptr;
}

void CSaberState_Intro::ActiveState(_float fTimeDelta)
{
    _bool isFinished = m_pOwner.lock()->IsAnimFinished();
    if (m_strCurAnim == "em0100_event_020")
    {
        if (isFinished)
        {
            m_strCurAnim = "em0100_event_021";
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

void CSaberState_Intro::EnterState()
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

    m_strCurAnim = "em0100_event_020";
    m_pOwner.lock()->SetAnimHard(m_strCurAnim);
    m_pOwner.lock()->m_pModelCom->Set_AnimFrame(15.f);

    // Set time
    m_fTotalTime = CBeat_Manager::Get_Instance()->Get_Beat() * 4.f;
    m_fCurTime = 0.f;
    m_fCCT_Time = m_fTotalTime * 0.9f;

    _float vDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(vTargetPos - vCurPos, 0.f)));
    m_fVelocity = vDistance / m_fTotalTime;
}

void CSaberState_Intro::ExitState()
{
}

void CSaberState_Intro::FinishLifting()
{
    m_strCurAnim = "em0100_event_022";
    m_pOwner.lock()->SetAnim(m_strCurAnim);
    m_fVelocity = 0.f;
}

_bool CSaberState_Intro::IsOver()
{
    _vector vCurPos = m_pOwner.lock()->m_pTransformCom->Get_Position();
    _vector vTargetPos = XMLoadFloat4(&m_vTargetPos);

    _vector vCurDir = XMVectorSetY(vTargetPos - vCurPos, 0.f);
    _vector vDir = XMLoadFloat4(&m_vDir);

    return XMVectorGetX(XMVector3Dot(vCurDir, vDir)) < 0.f;
}
