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

class CPanel_Lift : public CGameObject
{
public:
	static wstring ObjID;

private:
	CPanel_Lift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPanel_Lift(const CPanel_Lift& rhs);
public:
	virtual ~CPanel_Lift();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Push_From_Pool(void* pArg) override;

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_bool m_bDebug = false;
	_uint m_iSoundCount = 0;

	_float m_fPositionZ = 0.f;
	_uint m_iPrevBeat = { 0 };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CPanel_Lift> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END