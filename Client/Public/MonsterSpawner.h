#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CMonsterSpawner : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CMonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterSpawner(const CMonsterSpawner& rhs);
public:
	~CMonsterSpawner();

public:
	virtual void Execute(_bool bExecute = true);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Edit_Attribute() override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

public:
	CMonsterBase::Monster_DESC Make_MonsterDesc();

private:
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	string m_strMonsterTag;
	string m_strMonsterAttribute;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CMonsterSpawner> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END