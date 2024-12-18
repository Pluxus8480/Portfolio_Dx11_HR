#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_RAY, TYPE_END };

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
public:
	virtual ~CCollider();

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix WorldMatrix);

public:
	_bool Intersect(shared_ptr<CCollider> pTargetCollider);
	_bool RayIntersect(XMVECTOR vRayPos, XMVECTOR vRayDir, _float& Dist);
	_bool PointIntersect(XMVECTOR vPoint);
	void  Collision_Enter(shared_ptr<CCollider> pTargetCollider);
	_uint Get_ColliderID() { return m_iColliderID; }
	void Set_ColliderTag(const _wstring& strTag) { m_strColliderTag = strTag; }
	const _wstring& Get_ColliderTag() const { return m_strColliderTag; }

	void Set_Collider_Group(COLLIDER_GROUP eGroup) { m_eCollisionGroup = eGroup; };
	COLLIDER_GROUP Get_Collider_Group()const { return m_eCollisionGroup; }
#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif

private:
	TYPE					m_eType = { TYPE_END };
	shared_ptr<class CBounding> m_pBounding = { nullptr };
	_uint m_iColliderID;
	static _uint s_iNumCollider;
	_wstring m_strColliderTag;

	COLLIDER_GROUP m_eCollisionGroup = COLLIDER_GROUP::COLLISION_END;

public:
	_bool IsCollided();
	_bool IsPicked();


#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

#endif

public:
	static shared_ptr<CCollider> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	void Free();
};

END