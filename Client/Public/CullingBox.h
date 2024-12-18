#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CCullingBox : public CGameObject
{
public:
	struct CULLING_DESC : public CTransform::TRANSFORM_DESC
	{
		_bool bBoarder;
		_float4x4 worldMat;
	};

public:
	static wstring ObjID;
private:
	CCullingBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCullingBox(const CCullingBox& rhs);
public:
	~CCullingBox();

public:
	_bool Get_Collision() const { return m_bCollision; }
	_bool Is_Boarder() const { return m_bBoarder; }

	void Set_Boarder(_bool bBoarder) { m_bBoarder = bBoarder; }

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

	_bool IsIn_Bounding(_fvector vPosition);

private:
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_bool m_bCollision = { false };
	_bool m_bBoarder = { false };

public:
	static shared_ptr<CCullingBox> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END