#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)
class CPodLaser;
class CLaserBoard;
class CLaserCircuit;
class CLaserPillar;
class CLaserReward;
class CGlassWall;


class CReverbGaugeRoom : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CReverbGaugeRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CReverbGaugeRoom(const CReverbGaugeRoom& rhs);
public:
	~CReverbGaugeRoom();

public:
	virtual void Execute(_bool bExecute = true);


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Edit_Attribute() override;

	void Break_Glass();
	void Open_Reward();


private:
	_uint m_iPrevBeatCnt = 0;
	shared_ptr<CLaserBoard> m_pBoard = nullptr;
	_float4x4 m_BoardMat{};
	shared_ptr<CLaserCircuit> m_pCircuit = nullptr;
	_float4x4 m_CircuitMat{};
	shared_ptr<CLaserPillar>  m_pLPillar = nullptr;
	_float4x4 m_LPillarMat{};
	shared_ptr<CLaserPillar>  m_pRPillar = nullptr;
	_float4x4 m_RPillarMat{};
	shared_ptr<CLaserReward>  m_pReward = nullptr;
	_float4x4 m_RewardMat{};
	shared_ptr<CGlassWall>  m_pGlassWall = nullptr;
	_float4x4 m_GlassWallMat{};

	shared_ptr<CCollider> m_pColliderCom = nullptr;

	_float m_fPillarStart = -6.0f;
	_float m_fLPillarPos = 0.f;
	_float m_fLPillarDest = 0.f;
	_float m_fRPillarPos = 0.f;
	_float m_fRPillarDest = 0.f;
	_float m_fPillarEnd = 10.0f;

	_bool m_bPillarMove = false;
	_int m_iLDir = 1;
	_int m_iRDir = -1;
	_uint m_iCircuitExecuteCount = 0;
	_uint m_iPillarMoveCount = 0;
	_bool m_bStarted = false;
	_bool m_bUsedUp = false;

	_float m_fDelayTimer = 0.f;
	_bool m_bCamChange = false;
	_bool m_bCamChangeEnd = false;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Set_ChildMatrix(shared_ptr<CGameObject> pChild, _float4x4& fMat);


public:
	static shared_ptr<CReverbGaugeRoom> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END