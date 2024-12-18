#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CFloorSwitch : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CFloorSwitch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFloorSwitch(const CFloorSwitch& rhs);
public:
	~CFloorSwitch();

public:
	virtual void Set_Dead(bool bDead = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	shared_ptr<CGameObject> m_pButton = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Parts();

public:
	static shared_ptr<CFloorSwitch> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END