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

class CFadeElevator : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CFadeElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFadeElevator(const CFadeElevator& rhs);
public:
	~CFadeElevator();

public:
	virtual void Done() override;

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
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	virtual void Execute(_bool bExecute = true) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_float3 m_vModelCenter = {};
	_float3 m_vModelSize = {};

	_float3 m_vInitialPosition = {};
	_float3 m_vFinalPosition = {};
	_float m_fMoveDistance = {};
	_float m_fTimer = {};
	_bool m_bEventTrigger = { false };
	_bool m_bDown = { true };
	_bool m_bStop = { false };

	_float4x4 m_ChildMat = {};
	_bool m_bInit = { false };

	_bool m_bCollision = { false };

	_float m_fMoveSpeed = { 15.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CFadeElevator> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END