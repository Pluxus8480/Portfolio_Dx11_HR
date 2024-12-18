#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CBulbTrigger : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	enum ANIM_STATE { SPIN_BEGIN, SPINNING, SPIN_END, ST_END };
	enum PLATFORMS { WALL1, WALL2, WALL3, WALL4, ELEVATOR, PLATFORM_END };

private:
	CBulbTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBulbTrigger(const CBulbTrigger& rhs);
public:
	~CBulbTrigger();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

	virtual void Edit_Attribute() override;
	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	ANIM_STATE m_eState = { ST_END };
	_bool m_bAnimDone = { false };
	_bool m_bReady = { true };

	_uint m_iCurPlatformIndex = { 0 };

	_uint m_iPrevBeat = { 0 };


private:
	void Initialize_Sign();
	shared_ptr<class CUi_MapInterAction> m_pSign;


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Set_Animation();
	void Move_Platforms();

public:
	static shared_ptr<CBulbTrigger> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END