#include "Mimosa_Letter_Bomb.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "CharacterController.h"

wstring CMimosa_Letter_Bomb::ObjID = TEXT("CMimosa_Letter_Bomb");

CMimosa_Letter_Bomb::CMimosa_Letter_Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMimosa_Letter_Bomb::CMimosa_Letter_Bomb(const CMimosa_Letter_Bomb& rhs)
	: CGameObject(rhs)
{
}

CMimosa_Letter_Bomb::~CMimosa_Letter_Bomb()
{
	Free();
}

HRESULT CMimosa_Letter_Bomb::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	Set_PoolSize(12);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("LetterBomb"), TEXT("../Bin/Resources/Models/Monsters/Mimosa/Sound/LetterBomb.ogg")));
	return S_OK;
}

HRESULT CMimosa_Letter_Bomb::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Speed(2.f);
	m_eLetter_State = LETTER_STATE::UP;
	return S_OK;
}

void CMimosa_Letter_Bomb::PriorityTick(_float fTimeDelta)
{

}

void CMimosa_Letter_Bomb::Tick(_float fTimeDelta)
{
	m_pTransformCom->LookAt(XMLoadFloat4(&m_fCenterPos));


	switch (m_eLetter_State)
	{
	case Client::CMimosa_Letter_Bomb::LETTER_STATE::UP:
	{
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMVectorSet(0, 1, 0, 0) * fTimeDelta * m_pTransformCom->Get_Speed());

		if (XMVectorGetY(m_pTransformCom->Get_Position()) >= m_fCenterPos.y + 2.f)
		{
			m_bDown = true;
			m_eLetter_State = LETTER_STATE::SPIN;
		}
		break;
	}
	case Client::CMimosa_Letter_Bomb::LETTER_STATE::SPIN:
	{
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + m_pTransformCom->Get_NormRight() * fTimeDelta * 6.f);

		m_fShoot_Delay -= fTimeDelta;
		break;
	}
	case Client::CMimosa_Letter_Bomb::LETTER_STATE::SHOOT:
		break;
	case Client::CMimosa_Letter_Bomb::LETTER_STATE::EXPLODE:
	{
		m_fExplode_Delay -= fTimeDelta;
		if (m_fExplode_Delay < 0.f)
		{
			 m_iLetterModel = -1;
			 m_fCenterPos = {};
			 m_fMimosaLookDir = {};
			 m_fShootDir = {};
			 m_fShootDistance = {};
			 m_fExplode_Delay = 0.f;
			 m_bDown = false;
			 m_bShooting = false;

			 m_fShoot_Delay = 0.f;
			 m_eLetter_State = LETTER_STATE::UP;

			m_pEffect->UnionEffectStop();
			m_pEffect = nullptr;

			_float4x4 matResult{};
			XMStoreFloat4x4(&matResult,m_pTransformCom->Get_WorldMatrix_XMMat());
			CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect("Mimosa_Doremi_Boom", &matResult, 1.f , true);
			Dead();

		}
		break;
	}
	case Client::CMimosa_Letter_Bomb::LETTER_STATE::LETTER_STATE_END:
		break;
	default:
		break;
	}


	if (m_bDown)
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMVectorSet(0, -1, 0, 0) * fTimeDelta * m_pTransformCom->Get_Speed());

	if (XMVectorGetY(m_pTransformCom->Get_Position()) <= m_fCenterPos.y)
		m_bDown = false;

	if (m_fShoot_Delay < 0.f && !m_bShooting)
	{
		_vector playerPos = PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position();
		
		m_fShoot_Delay = 0.f;
		//playerPos = XMVectorSetY(playerPos, -1.8f);

		_vector vDir = XMVector3Normalize(playerPos - m_pTransformCom->Get_Position());
		XMStoreFloat4(&m_fShootDir, vDir);
		m_fShootDistance = XMVectorGetX(XMVector3Length(playerPos - m_pTransformCom->Get_Position()));
		m_bShooting = true;
		m_eLetter_State = LETTER_STATE::SHOOT;

		GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LetterBomb");
	}

	if (m_bShooting)
	{
		_float fBeat = BEATMANAGER->Get_Beat();
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMLoadFloat4(&m_fShootDir) * m_fShootDistance * fBeat * 0.5f);
		_vector vPos = m_pTransformCom->Get_Position();

		if (XMVectorGetY( vPos) < -1.5f)
		{
			vPos = XMVectorSetY(vPos, -1.5f);
			m_pTransformCom->Set_Position(vPos);
			m_fExplode_Delay = fBeat * 3.f;
			m_eLetter_State = LETTER_STATE::EXPLODE;
			m_bShooting = false;
		}
		
	
		//_float3 sweepPos = {};

		//XMStoreFloat3(&sweepPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//// 스윕 테스트 설정


		//PxTransform startTransform(PxVec3(sweepPos.x, sweepPos.y, sweepPos.z));
		//PxVec3 sweepDirection(m_fShootDir.x, m_fShootDir.y, m_fShootDir.z);
		//if (!sweepDirection.isZero())
		//{
		//	sweepDirection.normalize();
		//	float sweepDistance = m_fShootDistance * fBeat * 0.3f;

		//	PxSphereGeometry sphereGeom(0.1f);
		//	PxSweepBuffer sweepHit;
		//	bool status = GAMEINSTANCE->Get_Scene()->sweep(sphereGeom, startTransform, sweepDirection, sweepDistance, sweepHit);

		//	if (status ) 
		//	{
		//		const PxSweepHit& hit = sweepHit.block;

		//		if (hit.actor->getName() != "Player" && hit.actor->getName() != "Monster")
		//		{
		//			m_fExplode_Delay = fBeat * 3.f;
		//			m_eLetter_State = LETTER_STATE::EXPLODE;
		//			m_bShooting = false;
		//		}
		//	}
		//}
	}
}

void CMimosa_Letter_Bomb::LateTick(_float fTimeDelta)
{

}

HRESULT CMimosa_Letter_Bomb::Render()
{
	return S_OK;
}

void CMimosa_Letter_Bomb::Push_From_Pool(void* pArg)
{
	auto pBM = BEATMANAGER;
	MiMOSA_LETTER ls = (*(MiMOSA_LETTER*)(pArg));
	m_fCenterPos = ls.fCenterPos;
	m_iLetterModel = ls.iLetterModel;
	m_fShoot_Delay = ls.fShootDelay;

	switch (m_iLetterModel)
	{
	case 0:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_Do", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	case 1:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_Re", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	case 2:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_Mi", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	case 3:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_Fa", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	case 4:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_So", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	case 5:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_La", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	case 6:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_Ti", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	case 7:
	{
		m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Mimosa_Doremi_Do", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
		break;
	}
	}
}

void CMimosa_Letter_Bomb::Dying()
{
}

HRESULT CMimosa_Letter_Bomb::Ready_Components()
{
	return S_OK;
}

shared_ptr<CMimosa_Letter_Bomb> CMimosa_Letter_Bomb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_Letter_Bomb
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_Letter_Bomb(pDevice, pContext)
		{}
	};


	shared_ptr<CMimosa_Letter_Bomb> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_Letter_Bomb"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CMimosa_Letter_Bomb::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_Letter_Bomb
	{
		MakeSharedEnabler(const CMimosa_Letter_Bomb& rhs) : CMimosa_Letter_Bomb(rhs)
		{}
	};


	shared_ptr<CMimosa_Letter_Bomb> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_Letter_Bomb"));
		assert(false);
	}

	return pInstance;
}

void CMimosa_Letter_Bomb::Free()
{
}
