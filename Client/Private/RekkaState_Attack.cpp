#include "RekkaState_Attack.h"
#include "GameInstance.h"
#include "Monster_Rekka.h"

#include "Rekka_Gauntlet.h"

#include "MapManager.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"

#include "Model.h"
#include "CharacterController.h"

#include "RekkaState_Idle.h"
#include "RekkaState_Rotate.h"
#include "RekkaState_Groggy.h"

CRekkaState_Attack::CRekkaState_Attack(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
    m_pOwner = pRekka;
    m_state = State::ATTACK;
    m_preState = preState;
}

CRekkaState* CRekkaState_Attack::StepState(_float fTimeDelta)
{
    _bool isFinished = m_pOwner.lock()->IsAnimFinished();
    if (!isFinished)
        return nullptr;

    // If doing Sequence Dash
    if (m_iSeqDash_Cnt > 0)
        return nullptr;
    // If doing Elect Sequence
    if (m_strCurAnim == "em2000_mvnt-jump_001")
        return nullptr;
    // If doing WWE
    if (m_strCurAnim == "em2000_atk_120")
        return nullptr;

    if (m_strCurAnim == "em2000_atk_250")
        return new CRekkaState_Groggy(m_pOwner, m_state);

    if (abs(m_pOwner.lock()->m_fAngleDegree) > 70.f)
        return new CRekkaState_Rotate(m_pOwner, m_state);
    else
        return new CRekkaState_Idle(m_pOwner, m_state);

    return nullptr;
}

void CRekkaState_Attack::ActiveState(_float fTimeDelta)
{
    if (m_strCurAnim == "em2000_atk_056" || m_strCurAnim == "em2000_atk_170")
    {
        _float fSpeedRotation = 1.f;
        auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer); 
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
    }
    else if (m_strCurAnim == "em2000_atk_060")
    {
        _float fSpeedRotation = 2.f;
        auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);

        if (m_pOwner.lock()->IsAnimFinished())
        {
            m_iSeqDash_Cnt -= 1;
            m_strCurAnim = "em2000_atk_100";
            m_pOwner.lock()->SetAnim(m_strCurAnim);

            m_dashState = EDashState::TARGETING;
        }
    }
    else if (m_strCurAnim == "em2000_atk_100")
    {
        switch (m_dashState)
        {
        case EDashState::TARGETING:
        {
            _float fSpeedRotation = 1.5f;
            auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
            m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
            break;
        }
        case EDashState::DASH_ON:
        {
            m_pOwner.lock()->MoveStraight(m_fDashSpeed);
            break;
        }
        case EDashState::DASH_OVER:
        {
            m_iSeqDash_Cnt -= 1;

            m_strCurAnim = "em2000_atk_104";
            m_pOwner.lock()->SetAnim(m_strCurAnim);
            m_dashState = EDashState::TARGETING;
            break;
        }
        }
    }
    else if (m_strCurAnim == "em2000_atk_104")
    {
        switch (m_dashState)
        {
        case EDashState::TARGETING:
        {
            _float fSpeedRotation = 2.f;
            auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
            m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
            break;
        }
        case EDashState::DASH_ON:
        {
            m_pOwner.lock()->MoveStraight(m_fDashSpeed);
            break;
        }
        case EDashState::DASH_OVER:
        {
            if (m_iSeqDash_Cnt == 0)
            {
                m_strCurAnim = "em2000_atk_101";
                m_pOwner.lock()->SetAnim(m_strCurAnim);
            }
            else
            {
                m_iSeqDash_Cnt -= 1;
                m_pOwner.lock()->SetAnimHard(m_strCurAnim);
                m_dashState = EDashState::TARGETING;
            }
            break;
        }
        }
    }
    else if (m_strCurAnim == "em2000_atk_110")
    {
        switch (m_dashState)
        {
        case EDashState::TARGETING:
        {
            _float fSpeedRotation = 1.5f;
            auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
            m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
            break;
        }
        case EDashState::DASH_ON:
        {
            m_pOwner.lock()->MoveStraight(m_fDashSpeed);
            break;
        }
        case EDashState::DASH_OVER:
            break;
        }

    }
    else if (m_strCurAnim == "em2000_atk_400")
    {
        switch (m_fistState)
        {
        case EFistState::TARGETING:
        {
            _float fSpeedRotation = 1.5f;
            auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
            m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta * fSpeedRotation);
            break;
        }
        case EFistState::JUMP_ON:
        {
            m_pOwner.lock()->MoveUp(12.f);
            m_pOwner.lock()->MoveStraight(12.f);
            break;
        }
        case EFistState::JUMPING:
        {
            break;
        }
        case EFistState::DOWN:
        {
            PxControllerState pCS;
            m_pOwner.lock()->m_pCCT->Get_Controller()->getState(pCS);
            if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)    // TODO: Player checking
            {
                if (pCS.touchedActor != nullptr && pCS.touchedActor->getName() != "Player")
                {
                    m_pOwner.lock()->GravityOn();
                    m_pOwner.lock()->AnimationOn(119.0f);
                    m_fistState = EFistState::FIST_OVER;
                }
            }
            else
            {
                m_pOwner.lock()->MoveUp(-20.f);
                m_pOwner.lock()->MoveStraight(m_fFistDownSpeed);
            }
            break;
        }
        case EFistState::FIST_OVER:
        {
            break;
        }
        }
    }
    else if (m_strCurAnim == "em2000_atk_120")
    {
        _float fSpeedRotation = 1.5f;
        auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta* fSpeedRotation);

        if (m_pOwner.lock()->IsAnimFinished())
        {
            if (m_pOwner.lock()->IsBeatAble())
            {
                m_strCurAnim = "em2000_atk_123";
                m_pOwner.lock()->SetAnim(m_strCurAnim);
                m_pOwner.lock()->m_pGauntlet->StartAttack();
            }
            else
            {
                m_strCurAnim = "em2000_atk_121";
                m_pOwner.lock()->SetAnim(m_strCurAnim, true);
                m_pOwner.lock()->m_pGauntlet->StayForAttack();
            }
        }
    }
    else if (m_strCurAnim == "em2000_atk_121")
    {
        _float fSpeedRotation = 1.5f;
        auto vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal_With_Speed(vTargetPos, fTimeDelta* fSpeedRotation);

        if (m_pOwner.lock()->IsBeatAble())
        {
            m_strCurAnim = "em2000_atk_123";
            m_pOwner.lock()->SetAnim(m_strCurAnim);
            m_pOwner.lock()->m_pGauntlet->StartAttack();
        }
    }
    else if (m_strCurAnim == "em2000_mvnt-jump_001")
    {
        if (m_pOwner.lock()->IsAnimFinished())
        {
            _vector vTargetDir = { -1.f, 0.f, 0.f, 0.f };
            _vector vCurPos = m_pOwner.lock()->m_pTransformCom->Get_Position();
            vCurPos += vTargetDir;
            m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vCurPos);

            m_strCurAnim = "em2000_atk_250";
            m_pOwner.lock()->SetAnim(m_strCurAnim);
        }
        else
        {
            m_pOwner.lock()->MoveStraight(m_fElectGoSpeed);
        }
    }
}

void CRekkaState_Attack::EnterState()
{
    // Init Attack Timer
    m_pOwner.lock()->m_fTimer_Attack = m_pOwner.lock()->m_fTimer_Attack_C;

     _uint iCurAttackId = m_pOwner.lock()->m_iAttack_Seq++;
    switch (iCurAttackId % 6)
    {
        case 0:
            m_strCurAnim = "em2000_atk_110";    // Dash
            break;
        case 1:
            m_strCurAnim = "em2000_atk_330";    // Range Attack
            break;
        case 2:
            m_strCurAnim = "em2000_atk_400";    // Fist Down
            break;
        case 3:
        {
            if (m_pOwner.lock()->m_isElectric)
                m_strCurAnim = "em2000_atk_170";    // Elect punch attack
            else
                m_strCurAnim = "em2000_atk_056";    // Base Attack
            break;
        }
        case 4:
        {
            if (m_pOwner.lock()->m_isElectric)
                m_strCurAnim = "em2000_mvnt-jump_001";    // Elect Range(250)
            else if (m_pOwner.lock()->m_iCurPase > 2)
            {
                m_strCurAnim = "em2000_atk_120";    // 잡기 모션
                m_pOwner.lock()->m_pGauntlet->ReadyForAttack();
                CMapManager::Get_Instance()->Boss_ChangePattern(_uint(CBoss_InteractObj::REKKA_PATTERN::GRAB));
            }
            else
                m_strCurAnim = "em2000_atk_330";    // Range Attack
            break;
        }
        case 5:
        {
            m_iSeqDash_Cnt = 3;
            m_strCurAnim = "em2000_atk_060";    // Sequence Dash
            break;
        }
    }

    m_pOwner.lock()->SetAnim(m_strCurAnim);

    if (m_strCurAnim == "em2000_mvnt-jump_001")
    {
        _vector vTargetPos = { 1181.41f, 485.374f, 111.367f, 1.f };
        _vector vCurPos = m_pOwner.lock()->m_pTransformCom->Get_Position();
        m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);

        _float fDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(vTargetPos - vCurPos, 0.f)));
        _float fTotalBeat = m_pOwner.lock()->m_pModelCom->Get_Duration() / 15.f;
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
        _float fDuration = fTotalBeat * fBeat;
        m_fElectGoSpeed = fDistance / fDuration;
    }
}

void CRekkaState_Attack::ExitState()
{
}

void CRekkaState_Attack::DashOn()
{
    // Check Current Distance from player
    m_fDashSpeed = m_pOwner.lock()->m_fDistance * 3.f;
    if (m_fDashSpeed < 40.f)
        m_fDashSpeed = 40.f;
    m_dashState = EDashState::DASH_ON;
}

void CRekkaState_Attack::FistJumpOn()
{
    m_pOwner.lock()->GravityOff();
    m_fistState = EFistState::JUMP_ON;
}

void CRekkaState_Attack::FistJumpOff()
{
    m_fistState = EFistState::JUMPING;
}

void CRekkaState_Attack::FistDown()
{
    _vector vTargetPos = m_pOwner.lock()->m_pTransformCom->Get_Position() + XMLoadFloat4(&m_pOwner.lock()->m_vfDirToPlayer);
    m_pOwner.lock()->m_pTransformCom->LookAt_Horizontal(vTargetPos);

    m_fFistDownSpeed = m_pOwner.lock()->m_fDistance * 3.f;

    m_pOwner.lock()->AnimationOff();

    m_fistState = EFistState::DOWN;
}