#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
class CTexture;
END

BEGIN(Client)

class CGenerator : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	enum ANIM_STATE { IDLE_OUT, IDLE_ON, HIT, ANIM_END };
	enum EMI_COLOR { RED, GREEN, YELLOW, COLOR_END };

private:
	CGenerator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGenerator(const CGenerator& rhs);
public:
	~CGenerator();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

	virtual void Edit_Attribute() override;
	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pSwitchColliderCom = { nullptr };
	shared_ptr<CCollider> m_pBodyColliderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[COLOR_END] = { nullptr, };

	weak_ptr<CGameObject> m_pGenerator_System = {};

	_bool m_bEvent_Start = { false };
	_bool m_bOn = { false };

	_int m_iMaxHit = {};
	_int m_iCurHit = {};
	_int m_iBefore_Hit = {};

	ANIM_STATE m_eAnimState = { ANIM_END };
	_uint m_iEmiIndex = {};


private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

	void Update_AnimState();


private:
	void Cal_PlayerDistance();
	void Initialize_Sign();


private:
	shared_ptr<class CUi_MapInterAction> m_pSign;
	_float m_fWorkDistance = { 5 };


public:
	static shared_ptr<CGenerator> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END