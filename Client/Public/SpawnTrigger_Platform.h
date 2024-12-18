#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class Collider;
END

BEGIN(Client)

class CSpawnTrigger_Platform : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CSpawnTrigger_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawnTrigger_Platform(const CSpawnTrigger_Platform& rhs);
public:
	~CSpawnTrigger_Platform();

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

	_uint m_iPrevBeat = { 0 };
	_uint m_iCurIndex = { 0 };

	_bool m_bInit = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSpawnTrigger_Platform> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END