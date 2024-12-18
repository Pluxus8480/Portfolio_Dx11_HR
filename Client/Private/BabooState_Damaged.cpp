#include "BabooState_Damaged.h"
#include "GameInstance.h"
#include "Monster_Baboo.h"
#include "PlayerManager.h"

#include "BabooState_Idle.h"
#include "Beat_Manager.h"
#include "Chai.h"

CBabooState_Damaged::CBabooState_Damaged(weak_ptr<class CMonster_Baboo> pBaboo, State preState, const DamageInfo& damageInfo, _bool isGroggy)
{
    m_state = State::DAMAGED;
    m_pOwner = pBaboo;
    m_preState = preState;

    m_damageInfo = damageInfo;

	m_isGroggy = isGroggy;
}

CBabooState* CBabooState_Damaged::StepState(_float fTimeDelta)
{
	auto pOwner = m_pOwner.lock();
	_bool isAnimFinished = pOwner->IsAnimFinished();
	if (isAnimFinished && m_strCurAnim == "em0400_dmg-ex_500")
	{
		m_strCurAnim = "em0400_dmg-ex_501";
		pOwner->SetAnimHard(m_strCurAnim);
	}
	else if (isAnimFinished && m_strCurAnim == "em0400_dmg-ex_501")
	{
		m_strCurAnim = "em0400_dmg-ex_502";
		pOwner->SetAnimHard(m_strCurAnim);
	}
	else if (isAnimFinished && !m_pOwner.lock()->m_isAirState)
		return new CBabooState_Idle(m_pOwner, m_state);

	return nullptr;
}

void CBabooState_Damaged::ActiveState(_float fTimeDelta)
{
	_bool isFinished = m_pOwner.lock()->IsAnimFinished();
	if (isFinished)
		m_doing_AirBone = false;
	if (isFinished && m_pOwner.lock()->m_isAirState)
	{
		m_pOwner.lock()->GravityOn();

		m_strCurAnim = "em0400_dmg-air_050";
		m_pOwner.lock()->SetAnimHard(m_strCurAnim, true);
	}
}

void CBabooState_Damaged::EnterState()
{
	auto pOwner = m_pOwner.lock();
	if (m_isGroggy)
	{
		m_strCurAnim = "em0400_dmg-ex_500";
		pOwner->SetAnimHard(m_strCurAnim);
		return;
	}

	m_doing_AirBone = false;

	// Hardly look player
	_vector vTargetPos = pOwner->m_pTransformCom->Get_Position() + XMLoadFloat4(&pOwner->m_vfDirToPlayer);
	pOwner->m_pTransformCom->LookAt_Horizontal(vTargetPos);

	// Select effect position
	_float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
	_vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
	_float4x4 matResult{};
	XMStoreFloat4x4(&matResult, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));

	if (pOwner->m_isAirState)
	{
		if (EAttackDirection::DOWN == m_damageInfo.eAttack_Direction)
		{
		}
		else
		{
			pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
			switch (m_damageInfo.eAttackType)
			{
			case EAttackType::Base:
			{
				shared_ptr<CTransform> playerTransform = PLAYERMANAGER->Get_Player()->Get_Transform();
				_matrix playerMat = playerTransform->Get_WorldMatrix_XMMat();
				_vector playerPos = playerMat.r[3];

				_vector backMovePos = {};
				//backMovePos = pOwner->Get_Transform()->Get_Position() + -pOwner->Get_Transform()->Get_NormLook() * 0.5f;
				backMovePos = (playerPos + playerTransform->Get_NormLook() * 2.f);
				backMovePos = XMVectorSetY(backMovePos, XMVectorGetY(backMovePos) + 1.5f);
				playerMat.r[3] = backMovePos;

				pOwner->Set_Position(playerMat);

				m_pOwner.lock()->GravityOff();
				switch (m_damageInfo.eAttack_Direction)
				{
					case EAttackDirection::DEFAULT: case EAttackDirection::UP:
					{
						m_strCurAnim = "em0400_dmg-air_200";
						m_pOwner.lock()->SetAnim(m_strCurAnim);
						break;
					}
					case EAttackDirection::LEFT:
					{
						m_strCurAnim = "em0400_dmg-air_210";
						m_pOwner.lock()->SetAnim(m_strCurAnim);
						break;
					}
					case EAttackDirection::RIGHT:
					{
						m_strCurAnim = "em0400_dmg-air_220";
						m_pOwner.lock()->SetAnim(m_strCurAnim);
						break;
					}
				}
			}
			}
		}
	}
	else // On the ground
	{
		switch (m_damageInfo.eAttackType)
		{
			case EAttackType::Base:
			{
				pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
				switch (m_damageInfo.eAttack_Direction)
				{
				case EAttackDirection::DEFAULT: case EAttackDirection::UP:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-low_000";
					break;
				case EAttackDirection::LEFT:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-low_010";
					break;
				case EAttackDirection::RIGHT:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-low_020";
					break;
				case EAttackDirection::DOWN:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-low_030";
					break;
				}

				m_pOwner.lock()->SetAnimHard(m_strCurAnim);
				break;
			}
			case EAttackType::Hard:
			{
				pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
				switch (m_damageInfo.eAttack_Direction)
				{
					case EAttackDirection::DEFAULT: case EAttackDirection::UP:
						m_strCurAnim = "em0400_dmg-mid_010";
						break;
					case EAttackDirection::LEFT:
						m_strCurAnim = "em0400_dmg-mid_010";
						break;
					case EAttackDirection::RIGHT:
						m_strCurAnim = "em0400_dmg-mid_020";
						break;
					case EAttackDirection::DOWN:
						m_strCurAnim = "em0400_dmg-mid_600";
						break;
				}

				m_pOwner.lock()->SetAnimHard(m_strCurAnim);
				break;
			}
			case EAttackType::SP:
			{
				pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
				if (m_pOwner.lock()->IsDeadState())
					m_strCurAnim = "em0400_dmg-die_010";
				else
					m_strCurAnim = "em0400_dmg-high_520";
				m_pOwner.lock()->SetAnimHard(m_strCurAnim);
				break;
			}
			case EAttackType::Special:
			{
				pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
				if (m_pOwner.lock()->IsDeadState())
					m_strCurAnim = "em0400_dmg-die_010";
				else
					m_strCurAnim = "em0400_dmg-high_520";
				m_pOwner.lock()->SetAnimHard(m_strCurAnim);


				if (m_damageInfo.strAttackName == "RRR")
				{
					if (!pOwner->m_bFinishBlow)
					{
						PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, pOwner);
						PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, PLAYERMANAGER->Get_Player());
					}
					PLAYERMANAGER->Callback_Event("RRR");
				}
				else if (m_damageInfo.strAttackName == "LL_LLL")
				{
					if (!pOwner->m_bFinishBlow)
					{
						PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, pOwner);
						PLAYERMANAGER->Bullet_Time(CBeat_Manager::Get_Instance()->Get_Beat(), 0.03f, PLAYERMANAGER->Get_Player());
					}
					PLAYERMANAGER->Callback_Event("LL_LLL");
				}
				break;
			}
			case EAttackType::Air:
			{
				pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
				m_pOwner.lock()->AirStateON();
				m_strCurAnim = "em0400_dmg-air_000";
				m_pOwner.lock()->SetAnimHard(m_strCurAnim);

				m_doing_AirBone = true;
				break;
			}
			case EAttackType::Macaron:
			{
				pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
				switch (m_damageInfo.eAttack_Direction)
				{
				case EAttackDirection::DEFAULT:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-high_520";
					break;
				default:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-high_520";
					break;
				}

				m_pOwner.lock()->SetAnimHard(m_strCurAnim);
				break;
			}
			case EAttackType::Korsica:
			{
				pOwner->PlayEffect("Big_HitEffect", matResult, 1.f, true);
				switch (m_damageInfo.eAttack_Direction)
				{
				case EAttackDirection::DEFAULT:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-mid_000";
					break;
				default:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-mid_000";
					break;
				}

				m_pOwner.lock()->SetAnimHard(m_strCurAnim);
				break;
			}

			case EAttackType::Peppermint:
			{
				pOwner->PlayEffect("PepperMint_HitEffect", matResult, 1.f, true);
				switch (m_damageInfo.eAttack_Direction)
				{
				case EAttackDirection::DEFAULT:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-low_010";
					break;
				default:
					if (m_pOwner.lock()->IsDeadState())
						m_strCurAnim = "em0400_dmg-die_100";
					else
						m_strCurAnim = "em0400_dmg-low_010";
					break;
				}

				m_pOwner.lock()->SetAnimHard(m_strCurAnim);
				break;
			}
		}
	}
}

void CBabooState_Damaged::ExitState()
{
}

_bool CBabooState_Damaged::IsSlowAnim()
{
	return m_strCurAnim == "em0400_dmg-air_200" || m_strCurAnim == "em0400_dmg-air_210" || m_strCurAnim == "em0400_dmg-air_220";
}

_bool CBabooState_Damaged::IsColliderOn()
{
	if (m_strCurAnim == "em0400_dmg-die_100")
		return false;
	else if (m_strCurAnim == "em0400_dmg-die_010")
		return false;

	if (m_strCurAnim == "em0400_dmg-mid_010" && m_pOwner.lock()->IsDeadState())
		return false;
	else if (m_strCurAnim == "em0400_dmg-mid_020" && m_pOwner.lock()->IsDeadState())
		return false;
	else if (m_strCurAnim == "em0400_dmg-mid_600" && m_pOwner.lock()->IsDeadState())
		return false;

	return true;
}
