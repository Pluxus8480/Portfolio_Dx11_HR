#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)

class CFloorSwitchButton : public CPartObject
{
public:
	static wstring ObjID;

private:
	CFloorSwitchButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFloorSwitchButton(const CFloorSwitchButton& rhs);
public:
	~CFloorSwitchButton();

public:
	_bool Get_SwitchOn() const { return m_bOn; }

	void Set_SwitchOn(_bool bOn) { m_bOn = bOn; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	vector<PxRigidStatic*> m_vecPxRigid;

	_bool m_bOn = { true };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CFloorSwitchButton> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END