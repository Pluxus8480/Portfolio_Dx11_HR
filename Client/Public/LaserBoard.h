

#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)
class CPodLaser;

class CLaserBoard : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CLaserBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLaserBoard(const CLaserBoard& rhs);
public:
	~CLaserBoard();

public:
	virtual void Execute(_bool bExecute = true);


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Edit_Attribute() override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysxCollider = nullptr;

	_float3 m_vModelCenter = {};
	_float3 m_vModelSize = {};
	_uint m_iPrevBeatCnt = 0;

private:
	HRESULT Ready_Components(_float4x4* pMat);
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CLaserBoard> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END