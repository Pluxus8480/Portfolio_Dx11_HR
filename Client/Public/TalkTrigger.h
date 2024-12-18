#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CTalkTrigger : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CTalkTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTalkTrigger(const CTalkTrigger& rhs);
public:
	~CTalkTrigger();

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

private:
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_int m_iCollisionTiming = { 0 };
	string m_strTalkTag;
	_bool m_isTalkOver = { true };
	_bool m_bTalk = { false };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CTalkTrigger> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END