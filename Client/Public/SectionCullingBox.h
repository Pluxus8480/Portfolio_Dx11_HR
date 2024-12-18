#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CSectionCullingBox : public CGameObject, public CMisc_Interface
{
public:
	struct SECTIONCULLING_DESC : public CTransform::TRANSFORM_DESC
	{
		_bool bBoarder;
		_float4x4 worldMat;
	};

public:
	static wstring ObjID;
private:
	CSectionCullingBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSectionCullingBox(const CSectionCullingBox& rhs);
public:
	~CSectionCullingBox();

public:
	_bool Get_Collision() const { return m_bCollision; }
	const vector<_uint>& Get_RenderIndex() const { return m_vecRenderIndex; }

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

	_bool m_bCollision = { false };

	_int m_iSectionIndex = { -1 };
	vector<_uint> m_vecRenderIndex;

private:
	_uint m_iCurIndex = {};
	_int m_iRenderIndex = {};

public:
	static shared_ptr<CSectionCullingBox> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END