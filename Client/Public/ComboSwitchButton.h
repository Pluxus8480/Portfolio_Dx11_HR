#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
class CPhysXCollider;
END

BEGIN(Client)

class CComboSwitchButton : public CPartObject
{
public:
	static wstring ObjID;

private:
	CComboSwitchButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComboSwitchButton(const CComboSwitchButton& rhs);
public:
	~CComboSwitchButton();

public:
	_bool Get_SwtichOn() const { return m_bOn; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

	_bool m_bOn = { true };

	_uint m_iCurFrame = 0.f;

	_uint m_iPrevBeat = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CComboSwitchButton> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END