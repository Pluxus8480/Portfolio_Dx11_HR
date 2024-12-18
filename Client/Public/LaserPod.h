#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)
class CPodLaser;
class CLaserPod : public CGameObject
{
public:
	static wstring ObjID;
private:
	CLaserPod(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLaserPod(const CLaserPod& rhs);
public:
	virtual ~CLaserPod();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);
	void Set_Parent(_float4x4* pParent)
	{
		m_pParent = pParent;
	}
	void Shoot() { m_bShoot = true; }
	void Stop() { m_bShoot = false; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = nullptr;

	_uint m_iPrevBeat = { 0 };
	vector<_uint> m_MeshIdx;

	_bool m_bShoot = false;

	_float4x4* m_pParent = nullptr;
	shared_ptr<CPodLaser> m_pLaser = nullptr;


private:
	HRESULT Ready_Components();


public:
	static shared_ptr<CLaserPod> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END