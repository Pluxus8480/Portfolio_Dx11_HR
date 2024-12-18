#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)

class CHanging_Platform : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CHanging_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHanging_Platform(const CHanging_Platform& rhs);
public:
	~CHanging_Platform();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	vector<PxRigidStatic*> m_vecPxRigid;

	_bool m_bInit = { false };

	_float3 m_vFinalPosition = {};
	_float3 m_vInitialPosition = {};

	const _float4x4* m_pFloorMatrix = {};
	_float m_fOffsetY = {};
	_bool m_bCollision = { false };

	_float4x4 m_PlayerLocal = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CHanging_Platform> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END