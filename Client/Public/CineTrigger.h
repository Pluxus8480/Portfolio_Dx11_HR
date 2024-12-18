#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CCineTrigger : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CCineTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineTrigger(const CCineTrigger& rhs);
public:
	~CCineTrigger();

public:
	virtual void SetUp_CinematicCallBack() override;

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
	string m_strCineTag;
	_bool m_bTriggerAction = { false };

	_bool m_b3rdBattle = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CCineTrigger> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END