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

class CCameraController : public CGimmickBase
{

public:
	static wstring ObjID;

private:
	CCameraController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraController(const CCameraController& rhs);
public:
	~CCameraController();

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
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_bool m_bUsedUp = false;

	_int    m_bEnder = 0;
	_float3 m_vLook{};
	_float  m_fDistance = 3.6f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CCameraController> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END