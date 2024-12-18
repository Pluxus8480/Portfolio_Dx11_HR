#pragma once

#include "MonsterBase.h"
#include "PickUpItem.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;

class CPartObject;
class CCharacterController;
END

class CKale_FinaleSequence;

BEGIN(Client)
class CKale_Sword;
class CKale_Funnel;
class CKale_BArm;
class CKale_FArm;
class CKale_SArm;
class C808_Playable;
class CUi_KaleSystem;
class CAfterImage;

class CMonster_Kale : public CMonsterBase
{
public:
	static wstring ObjID;
	enum STATE
	{
		ATK_45L_60R,
		ATK_LUNGE_75L,
		ATK_LIGHTNING,
		ATK_JUMP_75D,
		ATK_SLASH_60L,
		ATK_75L_90R_105L,
		ATK_DASH_60L_75R_90L_130R_140L_165U,
		ATK_POWERWAVE_120L,
		ATK_POWERWAVE_45R,
		ATK_POWERWAVE_45R_67L,
		ATK_POWERWAVE_45R_67L_82R,
		ATK_120R_150L_165R,
		ATK_LUNGE_45L_72R,
		ATK_FUNNEL_105,

		ATK_COUNTER_45R,
		ATK_ATTACH_ARM,

		B_ARM_START,
		B_ARM_LOOP,
		B_ARM_SHOOT_30_60_90,
		B_ARM_GROGGY,
		B_ARM_GROGGY_LOOP,
		B_ARM_RECOVER,
		B_ARM_DESTROY,

		F_ARM_START,
		F_ARM_LOOP,
		F_ARM_SHOOT_30TO75,
		F_ARM_GROGGY,
		F_ARM_GROGGY_LOOP,
		F_ARM_RECOVER,
		F_ARM_DESTROY,

		S_ARM_START_90,
		S_ARM_LOOP,
		S_ARM_SHOOT,
		S_ARM_GROGGY,
		S_ARM_GROGGY_LOOP,
		S_ARM_RECOVER,
		S_ARM_DESTROY,

		DMG_AIR_START,
		DMG_AIR_LOOP,
		DMG_AIR_LAND,
		DMG_AIR_LIEDOWN,
		DMG_AIR_RECOVER,
		DMG_AIR_HIT,
		
		DMG_AIR_THROW_START,
		DMG_AIR_THROW_FALL,
		DMG_AIR_THROW_LAND,

		DMG_GUARD_ON,
		DMG_GUARD_LOOP,
		DMG_GUARD_OFF,

		DMG_KNOCKBACK,
		
		DMG_GROGGY_START,
		DMG_GROGGY_LOOP,
		DMG_GROGGY_RECOVER,
		DMG_GROGGY_HIT,
		DMG_GROGGY_HIT_KNOCKBACK,

		EVENT_808_RUN,
		EVENT_808_SUCCESS,
		EVENT_808_FAIL,

		EVENT_FIRST_PHASE_END,
		EVENT_FIRST_RP_READY,
		EVENT_FIRST_RP_ATK,
		EVENT_FIRST_RP_FAIL,
		EVENT_FIRST_RP_SUCCESS,

		EVENT_THIRD_PHASE_END,
		EVENT_THIRD_RP_READY,
		EVENT_THIRD_RP_ATK,
		EVENT_THIRD_RP_FAIL,
		EVENT_THIRD_RP_SUCCESS,
		EVENT_FINALE,

		IDLE_0,
		IDLE_BOARED,
		IDLE_TAUNT,

		MVNT_RUN_15,

		MVNT_STEP_FORWARD,
		MVNT_STEP_BACKWARD,
		MVNT_STEP_LEFT,
		MVNT_STEP_RIGHT,

		MVNT_WALK_FORWARD,
		MVNT_WALK_BACKWARD,
		MVNT_WALK_LEFT,
		MVNT_WALK_RIGHT,
		MVNT_WALK_LEFT_BACK,
		MVNT_WALK_RIGHT_BACK,

		STATE_END,
	};

private:
	CMonster_Kale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Kale(const CMonster_Kale& rhs);
public:
	virtual ~CMonster_Kale();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

	void Start_808Event(shared_ptr<C808_Playable>& p808);


public:
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;
	shared_ptr<CModel> Get_Model() { return m_pModelCom; }

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel>	m_pModelCom = { nullptr };
	shared_ptr<CModel>	m_pModelNoCoatCom = { nullptr };
	shared_ptr<CModel>	m_pModelCoatCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };

private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();

	void	PrepareCallbacks();
#pragma region AI_CONTROL
	void	Progress(_float fTimeDelta);
	void	Animation_End();
	void	To_Idle();
	void	Beat_Move();
	void	Idle(_float fTimeDelta, _bool bMovementControl = true);
	void	Melee_Start();
	void	Look_At_Atk_Timing(_bool bAtk = true);
	void	LookPlayerPos();
	void	Movement_Control(_float fTimeDelta);
	void	Groggy_Recover();
	void	Air_Loop(_float fTimeDelta);
	_bool	Is_Idle() const;
	void	Funnel_Control();

	void	Create_AttackUi(_uint iParryCount, vector<_float>& fBeatGap, vector<_bool>& bParry, _float2 fDistance, _bool SecondPattern) const;
	void	Setting_RP();
	void	Judge_RP();
	void	FireWall(_int iDelay, _fmatrix Mat);

	void	Lightning(_float fTimeDelta);
	void	BigSword();

	void	Change_Animation(_bool bLoop);

	void	Step(_float fTimeDelta);

	

#pragma endregion

	void Generate_Talk(_int iNum);

public:
	virtual void SetPos_Hard(_fvector vPos) override;
	virtual void MoveToPos(_fvector vTargetPos) override;

public:
	void MonsterDead();

public:
	_bool IsAnimFinished();
	void SetAnim(const string& strAnimName, _bool isLoop = false);
	void SetAnimHard(const string& strAnimName, _bool isLoop = false);

public:
	void GravityOn();
	void GravityOff();

public:
	void AirStateON();
	void AirStateOFF();

public:
	void MoveStraight(_float fSpeed);
	void MoveUp(_float fSpeed);
	void Set_Position(_matrix mMat);

public:
	void CCT_ON();
	void CCT_OFF();

private:
	void SelectBeat();
private:
	_float m_fBeatSpeed = 1.f;

public:
	void PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard);
	void PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard);

public:
	void Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType);
private:
	//shared_ptr<CPartObject> m_pWeapon;
	shared_ptr<CKale_Sword>  m_pSword;
	shared_ptr<CKale_BArm>   m_pBArm;
	shared_ptr<CKale_FArm>   m_pFArm;
	shared_ptr<CKale_SArm>   m_pSArmL;
	shared_ptr<CKale_SArm>   m_pSArmR;
	_bool	m_bBArm = false;
	_bool	m_bFArm = false;
	_bool	m_bSArm = false;

	_bool m_bIsInGame = true;
	_bool m_bTalk[5] = { false,false,false,false,false };

	shared_ptr<CKale_Funnel>  m_ArrFunnel[4];
	
	_bool m_bFinalCS = false;

	_bool m_isAirState = false;

	shared_ptr<CAfterImage> m_pAfterImage;

private:
	_bool	    m_bBodySlam = false;

	_bool		m_bCallBack = false;
	_bool		m_isAttackOn = false;
	_float4		m_vCurDir_Attack = { 0.f, 0.f, 0.f, 0.f };
	_float4		m_vPrePos_Attack = { 0.f, 0.f, 0.f, 1.f };

	string		m_strAnimTag[STATE_END];
	vector<_float> m_AnimFlag[STATE_END];
	STATE		m_eCurrState = IDLE_0;
	STATE		m_ePrevState = IDLE_0;
	_int		m_iIdleBeat = 0;
	_int		m_iIdleDelay = 8;
	_uint		m_iPrevBeat = 0;
	_int		m_iPhase = -1;
	_float		m_fMinRange = 3.f;
	_float		m_fMaxRange = 8.f;
	STATE		m_eNextState = ATK_45L_60R;

	_float4		m_vPlayerPos = {0.f,0.f,0.f,1.f};
	_float		m_fPlayerDist = 0.f;
	_float		m_fTrackFrame = 0.f;
	_bool		m_bBeatChanged = false;
	_int		m_iHitStreak = 0;
	_float		m_fShakeTime = 0.f;
	_float		m_fShakeMagnitude = 0.f;

	_float		m_arrFunnelPattern[3][4]{
		{0.0f, 22.5f, 37.5f, 45.0f},{0.0f, 7.5f, 22.5f, 37.5f},{0.0f, 7.5f, 30.0f, 37.5f}
	};
	_uint		m_iFunnelPattern = 0;
	_uint		m_ArrFunnelSequence[4]{ 0,1,2,3 };
	_uint		m_iCurrFunnelToActivate = 0;

	_int		m_iGroggyDuration = 0;
	_int		m_iAirborneDutation = 0;

	_int		m_iIdleFunnelShoot = 0;

	_int		m_iGroggyGauge = 40;
	const _int	m_iMaxGroggyGauge = 40;

	vector<_float> m_AttackBeatVec;
	vector<_bool> m_AttackParryBeatVec;
	_int m_iAttackReadyStartBeat = { 0 };

	_uint m_iFireWallCount = 0;

	_float m_fAfterImageCooldown = 0.f;

private:
	const _float4x4* m_pMat_RightHand = { nullptr };
	//"prop_03,4,5,6"
	const _float4x4* m_ArrMat_Funnel[4]{};
	const _float4x4* m_pMat_Spine = { nullptr };

	//backarm_attach_up_R, backarm_attach_up_L, backarm_attach_down_R, backarm_attach_down_L
	const _float4x4* m_ArrMat_BackArm[4]{};

private:
	//_float m_fDistance = 0.f;
	//_float4 m_vfDirToPlayer;
	//_float m_fAngleDegree = 0.f;

private:
	_float m_fMaxHp = 1.f;

private:
	_bool m_isCCTWork = false;
private:
	_bool m_bFinishBlow = false;
private:	// For Effect
	_float4x4 m_matCurrentPos = {};
	_float4x4 m_matSizeUp = {};
	_float4x4 m_matSizeUp_Att = {};

private: //Sequence
	shared_ptr<CKale_FinaleSequence> m_FinaleSequencer;
	shared_ptr<CGameObject> m_p808;

public:
	static shared_ptr<CMonster_Kale> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END