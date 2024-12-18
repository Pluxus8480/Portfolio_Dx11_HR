#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Client)

class CEnemyGate : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CEnemyGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyGate(const CEnemyGate& rhs);
public:
	~CEnemyGate();

public:
	virtual void Execute(_bool bExecute = true) override;
	virtual void Set_Dead(bool bDead = true) override;

	_bool Get_Opened() const { return m_bOpen; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Edit_Attribute() override;
	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	vector<shared_ptr<class CPartObject>> m_vecParts;

	_bool m_bOpen = { false };
	_bool m_bOpenSetted = { false };
	_float m_fTime = { 0.f };
	_bool m_bAllDone = { false };
	//_int m_iDelay = {};
	//
	//_uint m_iPrevBeat = {};

private:
	HRESULT Ready_Parts(const COMMONOBJ_DESC* pDesc);
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Execute_Door(_float fTimeDelta);

public:
	static shared_ptr<CEnemyGate> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END