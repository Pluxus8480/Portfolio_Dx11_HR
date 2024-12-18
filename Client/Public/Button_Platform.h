#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CButton_Platform : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CButton_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButton_Platform(const CButton_Platform& rhs);
public:
	~CButton_Platform();

public:
	void Set_DecalIndex(_uint iIndex) { m_iDecalIndex = iIndex; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_bool m_bInit = { false };

	_float3 m_vFinalPosition = {};
	_float3 m_vInitialPosition = {};

	_uint m_iDecalIndex = {};
	
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CButton_Platform> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END