#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Rect;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CTrulyInvisibleWall : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CTrulyInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrulyInvisibleWall(const CTrulyInvisibleWall& rhs);
public:
	~CTrulyInvisibleWall();

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
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CTrulyInvisibleWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END