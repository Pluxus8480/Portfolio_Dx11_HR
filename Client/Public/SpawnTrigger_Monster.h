#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CSpawnTrigger_Monster : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CSpawnTrigger_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawnTrigger_Monster(const CSpawnTrigger_Monster& rhs);
public:
	~CSpawnTrigger_Monster();

public:
	void Start_Spawn(_bool bEventEnd);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

	virtual void Edit_Attribute() override;
	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_int m_iWaveCount = { 1 };
	_uint m_iPrevBeat = {};

	_bool m_bWaitEvent = { false };
	_bool m_bUsed = false;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSpawnTrigger_Monster> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END