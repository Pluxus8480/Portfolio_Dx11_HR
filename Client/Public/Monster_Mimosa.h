#pragma once

#include "MonsterBase.h"
#include "Effect_Union.h"
#include "Camera_Bone.h"
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;

class CPartObject;
class CCharacterController;
class CTexture;
END


BEGIN(Client)

class CMonster_Mimosa : public CMonsterBase
{
public:
	static wstring ObjID;

private:
	CMonster_Mimosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Mimosa(const CMonster_Mimosa& rhs);

public:
	virtual ~CMonster_Mimosa();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel>	m_pModelCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };
	shared_ptr<CTexture> m_pWingTexture = nullptr;

private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();
	void	Initialize_Weapon();

	void	PrepareCallbacks();


#pragma region AI_CONTROL
	void	Animation_End();
	void	Animation(_float fTimeDelta);
	void	Dance_Event_Animation(_float fTimeDelta);
	void	Dance_Event_Animation_End(_float fTimeDelta);
	void	Rhythm_Parry_Animation(_float fTimeDelta);
	void	Rhythm_Parry_Animation_End(_float fTimeDelta);

	void	Initialize_Dance_Event();
	void	Start_Dance();
	void	LookPlayerPos(_float fTimeDelta);
	_float  Calculate_Distance_To_Player();
	void	Collision_Down(_float fTimeDelta);

	void	DownState(_float fTimeDelta);
	void	Pattern_Change(_float fTimeDelta);
	void	Play_Pattern(_float fTimeDelta);
	void	Pattern_Wait();

	void	Start_RP();
	void	Phase2_SpotLight_Pattern(_float fTimeDelta);
	void	Check_SpawnPos(_float4& fPos);
	void	Spawn_Mirror_Ball();
#pragma endregion
	void	Create_Letter_Bomb();
	void	Lazer_Charge(string strBoneName);
	void	Lazer_Fire(_int iIndex);
	void	RP_Lazer_Fire();
	_float4x4 Lazer_Matrix(string strBoneName);
	void	Update_Lazer(_float fTimeDelta);
	void Create_AttackUi(_uint iParryCount, vector<_float>& fBeatGap, vector<_bool>& bParry, _float2 fDistance = { 0, 400 });
	void Set_Ui(_int iSuccessCount);
	void Create_CutScene();

	void Move_To_CutScene_Pos_1();
	void Move_To_CutScene_Pos_2();
	void Move_To_CutScene_Pos_3();
	void Move_To_CutScene_Pos_4();
#pragma region WING_CONTROL
	void   Wing_On(_float fOnTime);
	void   Wing_Off(_float fOffTime);
	void   Wing_Controller(_float fTimeDelta);
	void   Off_Attach_Wing();
#pragma endregion
private:
	void Judge_Attack(_float fTimeDelta);
private:

	const _float4x4* m_pMat_Camera = { nullptr };
	const _float4x4* m_pChai_Attach = { nullptr };

	_bool m_bPlayerLook = true;
	_bool m_bTurn = false;
	_float m_fTurnTimer = 1.f;

	_float m_fDownTimer = 0.f;
	_bool m_bPattern_Timer_On = true;
	_float m_fPatternTimer = 0.f;
	string m_strCurrent_Pattern = "";
	string m_strNext_Pattern = "";

	_bool m_bWakeUp_Fly = false;
	_bool m_bDance_Event = false;
	_bool m_bDance_Rest = false;
	_bool m_bDance_Ready = false;
	_bool m_bRhythm_Parry = false;
	_bool m_bRP_Ready = false;
	_bool m_bFinish = false;
	_float m_fFinish_Timer = 0.f;

	_float m_fRP_Ready_Timer = 1.f;
	_float m_fDanceBattle_Next_Timer = 0.f;
	_int m_iParry_Success_Count = 0;
	_float m_fDance_PlaySpeed = 1.f;
	_float m_fPhase2_Timer = 0.f;
	_float m_fPhaseChange_Timer = 0.f;
	_bool m_bInvincivle = false;

	list<list<pair<string,_float>>> m_listDanceEvent;
	list <list<pair<_float,_int>>> m_listDanceEvent_Beats;
	list <_float> m_list_Dance_Beat;
	list <_float> m_list_Mimosa_Dance_Delay;

	vector<_float> m_AttackBeatVec;
	vector<_bool> m_AttackParryBeatVec;
	_int m_iAttackReadyStartBeat = { 0 };
	_int m_iRP_Success_Count = { 0 };

	_float m_fSpotlight_Timer = 0.f;
	shared_ptr<class CPartObject> m_pWeapon_Mic;
	_int m_iLetterIdx = 0;
	_bool m_bCreate_Letter = false;
	vector<shared_ptr<CEffect_Union>> m_listLazer = {};
	/* WING CONTROLL */
private:
	_bool m_bWingOff = true;
	_bool m_bLeftWing_On = false;
	_bool m_bRightWing_On = false;
	_float m_fWingSize = 1.f;
	_float m_fWingChange_Speed = 1.f;

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

	const _float4x4* Get_Chai_Attach_Matrix() { return m_pChai_Attach; }
public:
	void PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard);
	shared_ptr<CEffect_Union> PlayEffect_Return(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard);
	void PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard);

public:
	void Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType);
private:
	//shared_ptr<CPartObject> m_pWeapon;

	_bool m_isAirState = false;

private:
	_bool		m_isAttackOn = false;

	_float4		m_vPlayerPos = { 0.f,0.f,0.f,1.f };
	_float		m_fTrackFrame = 0.f;
	_float		m_fTime_Offset = 1.f;

private:
	const _float4x4* m_pMat_RightHand = { nullptr };
	const _float4x4* m_pMat_Spine = { nullptr };

private:
	const _float m_fMaxHp = 300.f;
	const _float m_fMaxBarrier = 80.f;
	 _float m_fHp = 300.f;
	 _float m_fBarrier = 80.f;
	_int m_iPhase = 1;

	_bool m_bFlyState = false;
private:
	_bool m_isCCTWork = true;
private:
	_bool m_bFinishBlow = false;
private:	// For Effect
	_float4x4 m_matCurrentPos = {};
	_float4x4 m_matSizeUp = {};
	_float4x4 m_matSizeUp_Att = {};


	shared_ptr<CGameObject> m_pSpotLight = {};
	vector<_float4> m_vecSpawnPos = {};
	vector<shared_ptr<CGameObject>> m_vecBackDancer = {};
private: //CameraControll
	CCamera_Bone m_CameraBone;
	_bool m_bIsOwnCamBone = false;

private:
	_bool m_bTalkStartBarrier   = { false };
	_bool m_bTalkEndBarrier     = { false };
	_bool m_bTalkRestartBarrier = { false };
	_bool m_isTalkOver = { true };

private:
	void Initialize_Ui();

public:
	static shared_ptr<CMonster_Mimosa> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END