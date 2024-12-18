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

class CWoodenBox : public CGimmickBase
{

public:
	static wstring ObjID;

	enum PICKUP
	{
		GEAR,
		HP,
	};

private:
	CWoodenBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWoodenBox(const CWoodenBox& rhs);
public:
	~CWoodenBox();

public:
	virtual void Execute(_bool bExecute = true) override;

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
	virtual void Push_From_Pool(void* pArg) override;
	virtual void Dying() override;

	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	vector<const _float4x4*> m_vecBones;
	vector<shared_ptr<CPhysXCollider>> m_vecPhysxCollider;
	//shared_ptr<CPhysXCollider> m_pPhysxCollider = nullptr;
	_int m_iPrevBeatCnt = { 0 };

	_int m_ePickupType = GEAR;
	_float m_fLifeTime = 3.f;
	_bool m_bInFrustum = true;

private:
	HRESULT Ready_Components(_float4x4* pMat);
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CWoodenBox> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END