#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CLargeElevator : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CLargeElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLargeElevator(const CLargeElevator& rhs);
public:
	~CLargeElevator();

public:
	virtual void Execute(_bool bExecute = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

	virtual void Edit_Attribute() override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_float3 m_vModelCenter = {};
	_float3 m_vModelSize = {};

	_bool m_bDown = { false };
	_bool m_bStop = { false };
	_bool m_bDelay = { false };
	_float m_fTime = { 0.f };
	_float m_fDelayedTime = { 0.f };
	_float m_fAccTime = { 0.f };

	_float m_fInitialPosY = { 0.f };
	_float m_fPositionY[5] = { 0, };
	_int m_iMoveCount = { 0 };

	_int m_iPrevBeatCnt = { 0 };

	_bool m_bTest = { false };

	_bool m_bPhysXCollided = { false };

	_bool m_bCollision = { false };
	_float m_fTimer = 0.f;
	_int m_iCount = 0;
	_float4 m_fInitPos = {};
	_bool m_bStart = false;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CLargeElevator> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END