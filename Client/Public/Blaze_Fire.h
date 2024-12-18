#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)
class CEffect_Union;

class CBlaze_Fire : public CGameObject
{

public:
	static wstring ObjID;

public:
	enum class BLAZE_FIRE_STATE
	{
		BLAZE_FIRE_START, BLAZE_FIRE_LOOP, BLAZE_FIRE_END, BLAZE_FIRE_SLEEP
	};

private:
	CBlaze_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlaze_Fire(const CBlaze_Fire& rhs);

public:
	virtual ~CBlaze_Fire();

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
private:
	HRESULT Ready_Components();

private:
	void State_Machine(_float fTimeDelta);
	
	void State_Start(_float fTimeDelta);
	void State_Loop(_float fTimeDelta);
	void State_End(_float fTimeDelta);

public:
	_bool Get_Fire_Activate() { return m_bActive; }

private:
	_bool   m_bActive = false;

	// Laser Control Effect
private:
	BLAZE_FIRE_STATE m_eBlazeFireState = BLAZE_FIRE_STATE::BLAZE_FIRE_SLEEP;

private:
	_float m_fBlazeFireTime = 0.f;
	_float m_fBlazeFireTimeMax = 5.f;

private:
	shared_ptr<CEffect_Union> m_pBlaze_Oil_Effect = nullptr;
	shared_ptr<CEffect_Union> m_pBlaze_Fire_Effect = nullptr;
	shared_ptr<CEffect_Union> m_pBlaze_Fire_Effect2 = nullptr;
	shared_ptr<CEffect_Union> m_pBlaze_Fire_End_Effect = nullptr;

private:
	shared_ptr<CCollider>	  m_pColliderCom = { nullptr };

public:
	static shared_ptr<CBlaze_Fire> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END