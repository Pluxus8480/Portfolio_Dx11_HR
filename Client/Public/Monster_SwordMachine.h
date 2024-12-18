#pragma once

#include "MonsterBase.h"
#include "Effect_Union.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPartObject;
class CCharacterController;
class CCollider;
END


BEGIN(Client)

class CMonster_SwordMachine : public CMonsterBase
{
public:
	static wstring ObjID;

public:
	enum class SWORDMACHINE_STATE
	{
		IDLE, ATTACK, DOWN, DMG_AIR, STATE_END	};
private:
	CMonster_SwordMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_SwordMachine(const CMonster_SwordMachine& rhs);
public:
	virtual ~CMonster_SwordMachine();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();
private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CMonster_SwordMachine> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual void Dying()override;
private:

	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);

	void LookPlayerPos(_float fTimeDelta);

	void	DownState(_float fTimeDelta);
	void	Pattern_Change(_float fTimeDelta);
	void	Play_Pattern(_float fTimeDelta);
	void	Pattern_Wait();
	_float  Calculate_Distance_To_Player();
public:
	void GravityOn();
	void GravityOff();
public:
	void AirStateON();
	void AirStateOFF();

public:
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;
public:
	void PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard);
	void PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard);
private:
	void	PrepareCallbacks();
	void SelectBeat();
	void Set_Position(_matrix mMat);
	void Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType);
	void	Collision_Down(_float fTimeDelta);
private:
	shared_ptr<CUi> m_pBarrier_UI = nullptr;
	_int m_fBarrier = 0.f;
	_float m_fShakeTime = 0.f;
	_float m_fShakeMagnitude = 0.f;

	vector<string> m_vecAnimList;
	_bool m_bRender = true;
	_bool m_bAttacking = false;
	_bool m_bHitOn = true;

	_float m_fLookDiffDegree = 0.f;
	_float m_fTurnTimer = 0.1f;
	_bool m_bTrunRight = false;
	_bool m_bTurning = false;
	_bool m_isAirState = false;

	_bool m_isAttackOn = false;

	_bool m_bFinishBlow = false;
	_bool m_doing_AirBone = false;
	_bool m_bAirLunched = false;

	_float m_fBeatSpeed = 0.f;

	vector <shared_ptr< CPartObject>> m_pWeapon;

	_float m_fDeadTimer = 0.f;
	_bool m_bAttack_End = false;
	
	_bool m_bPlayerLook = true;
	_bool m_bTurn = false;
	_float4		m_vPlayerPos = { 0.f,0.f,0.f,1.f };

	_float m_fDownTimer = 0.f;
	_float m_fPatternTimer = 0.f;

	SWORDMACHINE_STATE m_eState = SWORDMACHINE_STATE::STATE_END;
	_bool m_bPattern_Timer_On = false;
	string m_strCurrent_Pattern = "";
	string m_strNext_Pattern = "";

private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };

private:
	const _float4x4* m_pMat_RightHand = { nullptr };
	const _float4x4* m_pMat_Spine = { nullptr };

private:	// For Effect
	_float4x4 m_matCurrentPos = {};

};

END