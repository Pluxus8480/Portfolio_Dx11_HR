#pragma once

#include "MonsterBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CCharacterController;
class CPartObject;

END


BEGIN(Client)

class CMonster_Korsica : public CMonsterBase
{
public:
	static wstring ObjID;

public:
	enum class AnimType {
		Idle, IdletoMove, Move, MovetoAttackReady, AttackReadytoIdle, AttackReadytoMove,
		AttackEndtoBack, AttackEndtoFly, FourAttack, FiveAttack, SixAttack, NonParryFourAttack,
		NonParryFiveAttack, NonParryFiveAttackTwo, FourAttackTwo, NonParryFourAttackTwo,
		NonParrySevenAttack, NonParryTwoAttack, NonParryThreeAttack, Dummy, NonParrySevenAttackTwo,
		DummyTwo, NonParryFiveAttackThree, DeadFly,Fly ,PerfectCatch, GoodCatch, MissCatch, End, EndMark
	};

	enum class PatternType {
		Chat, Select, FourAttack, FiveAttack, SixAttack, NonParryTwoAttack,
		NonParryFourAttack, NonParryFiveAttack, NonParrySevenAttack, NonParrySevenAttackTwo, ContinueAttack,
		ContinueAttackTwo, FinalAttack, Dead, Test, End
	};


private:
	CMonster_Korsica(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Korsica(const CMonster_Korsica& rhs);


public:
	virtual ~CMonster_Korsica();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	void PartPriorityTick(_float fTimeDelta);
	void PartTick(_float fTimeDelta);
	void PartLateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

private:
	void Initialize_Camera();
	void Initialize_EventChai();
	void Ready_Components();
	void PrepareCallbacks();
	void Initialize_Weapon();


public:
	virtual void TakeDamage(const DamageInfo& damageInfo) override;
	void Set_Anim(AnimType Type, _bool Loop = false, _bool bInterPole = true);
	void Set_DelayTime();
	void Set_Pattern(PatternType ePattern, AnimType Type = AnimType::End);
	void Set_Chai(shared_ptr<class CEvent_Chai> pEventChai) { m_pEventChai = pEventChai; }


private:
	void Adjust_Camera(_float fTimeDelta);
	void Set_Ui(AnimType eType);
	void Create_AttackUi(_uint iParryCount, vector<_float>& fBeatGap, vector<_bool>& bParry, _float2 fDistance = { 0, 150 });
	void Set_Chat(_bool IsChat) { m_bChat = IsChat; }
	void Adjust_KorsicaPos(_float fTimeDelta);
	void Adjust_KorsicaPos_From_Chai(_float fTimeDelta);
	void Adjust_LookAngle(_float fTimeDelta);
	void AdjustCurAnim_ChaiPos();
	void Set_ChaiPos();
	void Set_ChaiPos_Immediate();
	void Testfuction(_float fTimeDelta);
	_bool AnimBeyond(AnimType eType, _float Percent = 1);
	void Set_CameraInterPole(AnimType eType);


private:
	void Progress_Pattern(_float fTimeDelta);
	void Progress_Chat(_float fTimeDelta);
	void Progress_Select(_float fTimeDelta);
	void Progress_FourAttack(_float fTimeDelta);
	void Progress_FiveAttack(_float fTimeDelta);
	void Progress_NonParryTwoAttack(_float fTimeDelta);
	void Progress_NonParryFourAttack(_float fTimeDelta);
	void Progress_NonParryFiveAttack(_float fTimeDelta);
	void Progress_NonParrySevenAttack(_float fTimeDelta);
	void Progress_NonParrySevenAttackTwo(_float fTimeDelta);
	void Progress_SixAttack(_float fTimeDelta);
	void Progress_ContinueAttack(_float fTimeDelta);
	void Progress_ContinueAttackTwo(_float fTimeDelta);
	void Progress_FinalAttack(_float fTimeDelta);
	void Progress_Anim(_float fTimeDelta);
	void Progress_Dead(_float fTimeDelta);
	void Progress_Test(_float fTimeDelta);

	void Progress_Weapon_Anim(_float fTimeDelta);
private:
	void Judge_Attack(_float fTimeDelta);


private:
	_float m_fMaxHp = 100.f;
	_float m_fCurHp = m_fMaxHp;
	PatternType m_ePattern = { PatternType::Chat };
	_bool m_bChat = { true };
	_float m_fDelayTime = { 0 };

	_uint m_iPattern = { 0 }; 

	_uint m_iDeadBone = { 0 };
	_uint m_iCameraBone = { 0 };
	_float3 m_Middle = { 0, 0, 0 };
	_float m_Radius = { 0 };
	AnimType m_ePreviousAnim = { AnimType::End };
	vector<_float> m_AttackBeatVec;
	vector<_bool> m_AttackParryBeatVec;
	_uint m_iAttackStartBeat = { 0 };
	_bool m_bIdle = false;
	_float4x4 m_MatOffset = Identity;
	_float4 m_vTargetPosition{ 0.f,0.f,0.f,0.f };
	_bool m_isTalkOver = true;
	_uint m_iTalkPatternCount = { 0 };
	_bool m_IsStageResultCall = { false };

	_bool m_bMissed = false;
	_bool m_bIsInGame = false;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel>	m_pModelCom = { nullptr };
	shared_ptr<class CEvent_Chai> m_pEventChai = { nullptr };
	shared_ptr<class CEvent_Chai> m_pChai;
	shared_ptr<class CKorsicaCamera> m_pCamera;
	shared_ptr<class CPartObject> m_pWeapon_Right;
	shared_ptr<class CPartObject> m_pWeapon_Left;
	shared_ptr<class CTransform>  m_pRealTransform;

public:
	static shared_ptr<CMonster_Korsica> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END