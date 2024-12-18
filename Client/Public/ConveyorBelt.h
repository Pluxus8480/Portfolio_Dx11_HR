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

class CConveyorBelt : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CConveyorBelt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConveyorBelt(const CConveyorBelt& rhs);
public:
	~CConveyorBelt();

public:
	virtual void Execute(_bool bExecute = true) override;

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
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;



private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysxCollider = nullptr;

	_int m_iPrevBeatCnt = { 0 };
	static _bool s_bFirst;
	_float m_fUvFlow = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CConveyorBelt> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END