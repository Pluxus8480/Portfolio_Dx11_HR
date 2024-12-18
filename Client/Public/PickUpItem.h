

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

class CPickUpItem : public CGimmickBase
{

public:

	static wstring ObjID;

	enum PICKUP
	{
		GEAR,
		HP,
		BATTERY,
	};
	enum SIZE
	{
		SMALL,
		MID,
		BIG
	};
	struct PICKUP_DESC
	{
		PICKUP eType;
		SIZE   eSize;
	};

private:
	CPickUpItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPickUpItem(const CPickUpItem& rhs);
public:
	~CPickUpItem();

public:
	virtual void Execute(_bool bExecute = true) override;
	void Model_Change(const PICKUP_DESC& desc);
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;


	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CCollider> m_pBiggerColliderCom = { nullptr };

	//shared_ptr<CPhysXCollider> m_pPhysxCollider = nullptr;
	_int m_iPrevBeatCnt = { 0 };

	_int m_ePickupType = GEAR;
	_int m_iSize = SMALL;
	_float m_fLandingPos = 0.f;
	_float3 m_vSpeed{};

private:
	HRESULT Ready_Components(COMMONOBJ_DESC* pDesc);
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CPickUpItem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END