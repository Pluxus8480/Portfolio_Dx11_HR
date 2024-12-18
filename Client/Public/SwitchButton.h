#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CSwitchButton : public CPartObject
{
public:
	static wstring ObjID;

private:
	CSwitchButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwitchButton(const CSwitchButton& rhs);
public:
	~CSwitchButton();

public:
	_bool Get_SwtichOn() const { return m_bOn; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	//virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	//virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	//shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_bool m_bOn = { true };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSwitchButton> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END