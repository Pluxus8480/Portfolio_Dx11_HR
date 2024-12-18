#include "ReverbGaugeRoom.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Beat_Manager.h"
#include "PodLaser.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "LaserBoard.h"
#include "LaserCircuit.h"
#include "LaserPillar.h"
#include "LaserReward.h"
#include "GlassWall.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "Player_Camera.h"

#include "MapManager.h"
wstring CReverbGaugeRoom::ObjID = TEXT("CReverbGaugeRoom");

CReverbGaugeRoom::CReverbGaugeRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CReverbGaugeRoom::CReverbGaugeRoom(const CReverbGaugeRoom& rhs)
	: CGimmickBase{ rhs }
{
}

CReverbGaugeRoom::~CReverbGaugeRoom()
{
	Free();
}

void CReverbGaugeRoom::Execute(_bool bExecute)
{
	m_bExecuted = true;
	m_iCircuitExecuteCount = 0;

}

HRESULT CReverbGaugeRoom::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CReverbGaugeRoom::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Components();

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	m_BoardMat = Identity;
	m_CircuitMat = Identity;
	XMStoreFloat4x4(&m_LPillarMat, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, -XM_PIDIV2, 0.f), XMVectorSet(-6.f, 4.7f, -5.5f, 1.f)));
	XMStoreFloat4x4(&m_RPillarMat, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, 0.f), XMVectorSet(-6.f, 4.7f, 5.5f, 1.f)));
	m_RewardMat = Identity;
	m_RewardMat._41 = -9.3f;
	m_RewardMat._42 = 0.8f;
	m_GlassWallMat = Identity;
	m_GlassWallMat._41 = -13.f;
	m_GlassWallMat._42 = 0.6f;
	m_GlassWallMat._43 = 5.5f;

	_float4x4 Static;
	XMStoreFloat4x4(&Static, XMLoadFloat4x4(&m_BoardMat) * m_pTransformCom->Get_WorldMatrix_XMMat());

	m_pBoard = static_pointer_cast<CLaserBoard>(GAMEINSTANCE->Clone_Object(CLaserBoard::ObjID, &Static));
	m_pCircuit = static_pointer_cast<CLaserCircuit>(GAMEINSTANCE->Clone_Object(CLaserCircuit::ObjID, nullptr));
	m_pLPillar = static_pointer_cast<CLaserPillar>(GAMEINSTANCE->Clone_Object(CLaserPillar::ObjID, nullptr));
	m_pRPillar = static_pointer_cast<CLaserPillar>(GAMEINSTANCE->Clone_Object(CLaserPillar::ObjID, nullptr));
	m_pReward = static_pointer_cast<CLaserReward>(GAMEINSTANCE->Clone_Object(CLaserReward::ObjID, nullptr));

	XMStoreFloat4x4(&Static, XMLoadFloat4x4(&m_GlassWallMat) * m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pGlassWall = static_pointer_cast<CGlassWall>(GAMEINSTANCE->Clone_Object(CGlassWall::ObjID, &Static));

	m_fLPillarPos = m_fPillarStart;
	m_fLPillarDest = m_fPillarStart;
	m_fRPillarPos = m_fPillarEnd;
	m_fRPillarDest = m_fPillarEnd;
	return S_OK;
}

void CReverbGaugeRoom::PriorityTick(_float fTimeDelta)
{
	m_pBoard->PriorityTick(fTimeDelta);
	m_pCircuit->PriorityTick(fTimeDelta);
	m_pLPillar->PriorityTick(fTimeDelta);
	m_pRPillar->PriorityTick(fTimeDelta);
	m_pReward->PriorityTick(fTimeDelta);
	m_pGlassWall->PriorityTick(fTimeDelta);
}

void CReverbGaugeRoom::Tick(_float fTimeDelta)
{

	auto pGameInstance = m_pGameInstance.lock();
	if (m_bStarted)
	{

		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

		_matrix rotationMatrix = XMMatrixRotationX(0.f);
		_vector rotatedVector = XMVector3Transform(-m_pReward->Get_Transform()->Get_NormRight(), rotationMatrix);
		rotatedVector = XMVectorSetY(rotatedVector, XMVectorGetY(rotatedVector) - 0.225f);

		if(!m_bCamChangeEnd)
		{
			m_fDelayTimer += fTimeDelta;
			if (m_fDelayTimer < fBeat * 2.f)
			{
				return;
			}
			else if (m_fDelayTimer >= fBeat * 2.f)
				m_bCamChange = true;
		}
		if(m_bCamChange)
		{
			m_bCamChangeEnd = true;
			m_bCamChange = false;
			static_pointer_cast<CChai> (PLAYERMANAGER->Get_Player())->Get_Player_Camera()->Camera_Fixed(m_pReward->Get_Transform(), rotatedVector, 20.f);
		}
		
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat - 1;
			m_bExecuted = true;
			m_bStarted = false;
		}
	}
	if(m_iCircuitExecuteCount >= 7 && !m_pCircuit->Is_Executed())
	{
		m_bPillarMove = false;
		m_bExecuted = false;
		m_pCircuit->Execute(false);
		Open_Reward();
		static_pointer_cast<CChai> (PLAYERMANAGER->Get_Player())->Get_Player_Camera()->End_Camera_Fixed();
	}

	if (m_bExecuted)
	{
		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
			if (m_iCircuitExecuteCount <= 1 && !m_pCircuit->Is_Executed())
			{
				m_pCircuit->Execute();
				++m_iCircuitExecuteCount;
			}
			if (m_iCircuitExecuteCount == 2 && !m_pCircuit->Is_Executed())
			{
				m_bPillarMove = true;
				if (!pGameInstance->Is_Sound_3D_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot"))
					pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot", m_pTransformCom->Get_Position(), XMVectorZero(), false);
			}
			if (m_bPillarMove)
			{
				++m_iPillarMoveCount;
				if (m_iPillarMoveCount == 8)
				{
					m_iCircuitExecuteCount = 3;
				}
			}
			if (m_iCircuitExecuteCount >= 3 && !m_pCircuit->Is_Executed())
			{
				m_pCircuit->Execute();
				++m_iCircuitExecuteCount;
				if (m_iCircuitExecuteCount >= 7)
				{
					
				}
			}
			if (m_bPillarMove)
			{
				m_fLPillarDest += 4.f * (_float)m_iLDir;
				m_fRPillarDest += 4.f * (_float)m_iRDir;
			}
		}
	}
	if (m_fLPillarPos <= m_fLPillarDest && m_iLDir == 1)
	{
		m_fLPillarPos += 4.f * fTimeDelta / CBeat_Manager::Get_Instance()->Get_Beat() * 2.f;
		if (m_fLPillarPos > m_fPillarEnd)
		{
			m_fLPillarPos = m_fPillarEnd;
			m_iLDir = -1;
			if (!pGameInstance->Is_Sound_3D_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot"))
				pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot", m_pTransformCom->Get_Position(), XMVectorZero(), false);
		}
	}
	if (m_fLPillarPos >= m_fLPillarDest && m_iLDir == -1)
	{
		m_fLPillarPos -= 4.f * fTimeDelta / CBeat_Manager::Get_Instance()->Get_Beat() * 2.f;
		if (m_fLPillarPos < m_fPillarStart)
		{
			m_fLPillarPos = m_fPillarStart;
			m_iLDir = 1;
			if (!pGameInstance->Is_Sound_3D_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot"))
				pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot", m_pTransformCom->Get_Position(), XMVectorZero(), false);

		}
	}
	if (m_fRPillarPos <= m_fRPillarDest && m_iRDir == 1)
	{
		m_fRPillarPos += 4.f * fTimeDelta / CBeat_Manager::Get_Instance()->Get_Beat() * 2.f;
		if (m_fRPillarPos > m_fPillarEnd)
		{
			m_fRPillarPos = m_fPillarEnd;
			m_iRDir = -1;
			if (!pGameInstance->Is_Sound_3D_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot"))
				pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot", m_pTransformCom->Get_Position(), XMVectorZero(), false);

		}
	}
	if (m_fRPillarPos >= m_fRPillarDest && m_iRDir == -1)
	{
		m_fRPillarPos -= 4.f * fTimeDelta / CBeat_Manager::Get_Instance()->Get_Beat() * 2.f;
		if (m_fRPillarPos < m_fPillarStart)
		{
			m_fRPillarPos = m_fPillarStart;
			m_iRDir = 1;
			if (!pGameInstance->Is_Sound_3D_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot"))
				pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot", m_pTransformCom->Get_Position(), XMVectorZero(), false);

		}
	}
	if (m_iLDir == 1 && m_bPillarMove)
	{
		m_pLPillar->Execute(true);
	}
	else
	{
		m_pLPillar->Execute(false);
	}
	if (m_iRDir == 1 && m_bPillarMove)
	{
		m_pRPillar->Execute(true);
	}
	else
	{
		m_pRPillar->Execute(false);
	}
	m_LPillarMat._41 = m_fLPillarPos;
	m_RPillarMat._41 = m_fRPillarPos;
	
	Set_ChildMatrix(m_pBoard, m_BoardMat);
	Set_ChildMatrix(m_pCircuit, m_CircuitMat);
	Set_ChildMatrix(m_pLPillar, m_LPillarMat);
	Set_ChildMatrix(m_pRPillar, m_RPillarMat);
	Set_ChildMatrix(m_pReward, m_RewardMat);
	Set_ChildMatrix(m_pGlassWall, m_GlassWallMat);

	m_pBoard->Tick(fTimeDelta);
	m_pCircuit->Tick(fTimeDelta);
	m_pLPillar->Tick(fTimeDelta);
	m_pRPillar->Tick(fTimeDelta);
	m_pReward->Tick(fTimeDelta);
	m_pGlassWall->Tick(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CReverbGaugeRoom::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	if(!m_bUsedUp)
		GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_GIMMICK, m_pColliderCom);
	m_pBoard->LateTick(fTimeDelta);
	m_pCircuit->LateTick(fTimeDelta);
	m_pLPillar->LateTick(fTimeDelta);
	m_pRPillar->LateTick(fTimeDelta);
	m_pReward->LateTick(fTimeDelta);
	m_pGlassWall->LateTick(fTimeDelta);

#ifdef _DEBUG
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CReverbGaugeRoom::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CReverbGaugeRoom::Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
	m_bUsedUp = true;
	m_bStarted = true;
}

void CReverbGaugeRoom::Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CReverbGaugeRoom::Edit_Attribute()
{
}

HRESULT CReverbGaugeRoom::Ready_Components()
{
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 3.5f, 0.f };
	obbDesc.vExtents = { 10.f, 7.0f, 10.0f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	assert(m_pColliderCom);

	return S_OK;
}

HRESULT CReverbGaugeRoom::Bind_ShaderResources()
{
	return S_OK;
}

void CReverbGaugeRoom::Set_ChildMatrix(shared_ptr<CGameObject> pChild, _float4x4& fMat)
{
	pChild->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&fMat) * m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CReverbGaugeRoom::Break_Glass()
{
	m_pGlassWall->Execute();
}

void CReverbGaugeRoom::Open_Reward()
{
	m_pReward->Execute();
}



shared_ptr<CReverbGaugeRoom> CReverbGaugeRoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CReverbGaugeRoom
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CReverbGaugeRoom(pDevice, pContext)
		{}
	};

	shared_ptr<CReverbGaugeRoom> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CReverbGaugeRoom"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CReverbGaugeRoom::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CReverbGaugeRoom
	{
		MakeSharedEnabler(const CReverbGaugeRoom& rhs) : CReverbGaugeRoom(rhs)
		{}
	};

	shared_ptr<CReverbGaugeRoom> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CReverbGaugeRoom"));
		assert(false);
	}

	return pInstance;
}

void CReverbGaugeRoom::Free()
{
}
