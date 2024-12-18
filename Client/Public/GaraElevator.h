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

class CGaraElevator : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CGaraElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGaraElevator(const CGaraElevator& rhs);
public:
	~CGaraElevator();

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

	shared_ptr<CModel> m_pSwitchModel = { nullptr };
	shared_ptr<CPhysXCollider> m_pSwitchPhysX = { nullptr };
	shared_ptr<CCollider> m_pSwitchCollider = { nullptr };

	_float3 m_vModelCenter = {};
	_float3 m_vModelSize = {};
	_float3 m_vSwitchCenter = {};
	_bool m_bSwitchCollision = { false };

	_bool m_bDown = { true };
	_bool m_bStop = { false };
	_bool m_bRest = { false };
	_bool m_bCollision = { false };
	
	_float m_fTimer = { 0.f };
	_int m_iCount = 0;

	_int m_iPrevBeatCnt = { 0 };

	_bool m_bTest = { false };

	_bool m_bCulled = false;

	_float3 m_vLastPos = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CGaraElevator> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END