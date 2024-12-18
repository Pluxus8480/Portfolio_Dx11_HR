#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)

class CComboSwitch : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	enum ELEM { GEARWALL, ELEVATOR, ELEM_END };

private:
	CComboSwitch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComboSwitch(const CComboSwitch& rhs);
public:
	~CComboSwitch();

public:
	virtual void Set_Dead(bool bDead = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Edit_Attribute() override;
	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

	shared_ptr<CGameObject> m_pButton = { nullptr };
	_bool m_bGearExecuted = { false };

	string m_strComboTest;

	_uint m_iMaxIndex = {};
	_uint m_iCurIndex = {};

	shared_ptr<class CUi> m_pUI = nullptr;
	shared_ptr<CGameObject> m_pCommandICon = nullptr;
	vector<_bool> m_vecIs_Left = {};
	_int m_iCurrCombo = 0;

	_bool m_bTimer_Start = false;
	_float m_fTimer = 0.f;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Parts();

public:
	static shared_ptr<CComboSwitch> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();

};

END