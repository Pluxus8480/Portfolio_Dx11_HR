#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Chai.h"

BEGIN(Engine)
class CCharacterController;
class CTransform;
END


BEGIN(Client)

class CPlayer_Manager
{
	DECLARE_SINGLETON(CPlayer_Manager)

public:
	enum class ThingType { Attack, Skill, Item, Chip, End };

	enum class Assist {Peppermint, Macaron, Korsica, End };
	struct Assist_Struct
	{
		_float fInitCoolTime	= 5.f;
		_float fCurr_CoolTime	= 0.f;
		_bool  bIs_CoolTime = false;
	};

	using AttackDesc = struct PlayerAttackInfo
	{
		wstring AttackName = {};
		_uint Test = {};
	};

	using ItemDesc = struct PlayerItemInfo
	{
		wstring ItemName = {};
		_uint Test = {};
	};
	using ChipDesc = struct PlayerChipInfo
	{
		_uint iUpGrade = { 0 };
	};
	using Desc = struct PlayerDataInfo // 저 이거 ui띄우는데 필요한 정보들입니다.
	{
		_float fRankScore = { 0 };

		_uint iCombatTimer = { 0 };

		_uint iPlayerCombo = { 0 };

		_uint iPlayerScore = { 0 };

		_uint JustTimingCount = { 100 };

		_uint iPlayerGear = { 0 };

		_uint iPlayerPauseChip = { 1 };

		_float Reverb = { 150 };
		_float MaxReverb = { 150 };

		CChai::SkillDesc iPlayerEquipSkill = {};
		CChai::SkillDesc iPlayerNonEquipSkill = {};

		_float AllMinusHp = {};

		_float Hp = { 150 };
		_float MaxHp = { 150 };
	};


private:
	CPlayer_Manager();

public:
	~CPlayer_Manager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);
	void Render();


public:
	_float Get_PlayerRankScore() const { return m_Desc.fRankScore; }
	void Add_PlayerRankScore(_float fScore) { m_Desc.fRankScore += fScore; }
	void Reset_RankScore( ) { m_Desc.fRankScore = 0; }

	_uint Get_PlayerScore() const { return m_Desc.iPlayerScore; }
	void Add_PlayerScore(_int iScore) { m_Desc.iPlayerScore += iScore; }
	void Rest_PlayerScore() { m_Desc.iPlayerScore = 0; }

	void Set_PlayerCombo(_int iCombo) { m_Desc.iPlayerCombo = iCombo; }

	_uint Get_PlayerCombo() const { return m_Desc.iPlayerCombo; }

	void Add_CorusScore(pair<_int, _int> Score);
	vector<pair<_int, _int>> Get_CorusScore() { return m_CorusScore; }
	void Clear_CorusScore() { m_CorusScore.clear(); }

	void Set_CombatTimer(_int iCombatTimer) { m_Desc.iCombatTimer = iCombatTimer; }
	_int Get_CombatTimer() const {return m_Desc.iCombatTimer; }

	_float Get_Hp() const { return m_Desc.Hp; }
	void Set_Hp(_float fHp);

	_float Get_MaxHp() const { return m_Desc.MaxHp; }
	void Set_MaxHp(_float fMaxHp);

	_float Get_AllMinusHp() const { return m_Desc.AllMinusHp; }

	_float Get_Reverb() const { return m_Desc.Reverb; }
	void Set_Reverb(_float fReverb);

	_float Get_MaxReverb() const { return m_Desc.MaxReverb; }
	void Set_MaxReverb(_float fMaxReverb);

	_uint Get_JustTimingCount() const { return m_Desc.JustTimingCount; }
	void  Set_JustTimingCount(_int iJustCount) { m_Desc.JustTimingCount = iJustCount; }


	_uint Get_PlayerGear() const { return m_Desc.iPlayerGear; }
	void Add_PlayerGear(_uint iGear) { m_Desc.iPlayerGear += iGear; }
	void Sub_PlayerGear(_uint iGear);


	_uint Get_PlayerPauseChip() const { return m_Desc.iPlayerPauseChip; }


	ChipDesc Get_PlayerChip(wstring StrName);

	_bool Is_HasThing(ThingType Type, wstring ThingName);
	void Add_Thing(ThingType Type, wstring ThingName, void* pArg);

	_uint Is_UpGradeCount(wstring ThingName);

	CChai::SkillDesc Get_PlayerEquipSkill() const { return m_Desc.iPlayerEquipSkill; }
	CChai::SkillDesc Get_PlayerNonEquipSkill() const { return m_Desc.iPlayerNonEquipSkill; }

	map<wstring, CChai::SkillDesc> Get_PlayerSkillMap() const { return m_PlayerSkillMap; };
	void Add_PlayerSkill(wstring SkillName, CChai::SkillDesc Desc);


	CChai::SkillDesc Find_PlayerSkill(wstring SkillName) const;
	void Set_PlayerEquipSkill(CChai::SkillDesc Skill) { m_Desc.iPlayerEquipSkill = Skill ; }
	void Set_PlayerNonEquipSkill(CChai::SkillDesc Skill) { m_Desc.iPlayerNonEquipSkill = Skill ; }

	void Set_Player(shared_ptr<CGameObject> pPlayer); 
	shared_ptr<CChai> Get_Player() { return m_pPlayer; }
	_float Get_FootPosY() { return m_pPlayer->Get_Foot_Position().y; }

	shared_ptr<CTransform> Get_PlayerTransform() const;
	_vector Get_PlayerPos() const; 
	_matrix Get_PlayerMatrix() const;
	void Set_Assist(const wstring& AssistName, shared_ptr<CGameObject> pAssist) { m_Assist.emplace(AssistName, pAssist); }
	void Reset_Assists();
	shared_ptr<CGameObject> Get_Assist(const wstring& AssistName);

	void Shake(_float fDuration , _float fPower , _bool bClear = true);

	void Bullet_Time(_float fDuration, _float fMultiflier, shared_ptr<CGameObject> pObject);
	void Callback_Event(string strEvent);

	void Start_2DScroll(_fvector vLook, _float fDistance);
	void Start_2DScroll(const _float3& vLook, _float fDistance);
	void End_2DScroll();

	Assist Get_Assist() const { return (Assist)m_iAssist;  }
	void   Change_Assist();

	void ProceedTalk();
	void UpdateTalkSystem(float fTimeDelta);
	void ResetAutoTalkTimer();
	void EnableReverb();

	void Set_808_Controller(shared_ptr<CGameObject> p808_Cat) { if (!m_p808_Controller) { m_p808_Controller = p808_Cat; } }
	shared_ptr<CGameObject> Get_808_Controller() { return m_p808_Controller; }
	void Reset_808_Controller();

	Assist_Struct Get_Assist_Struct(_int iAssistNum) const { return m_Assists[iAssistNum]; }
	void Set_Assist_CoolTime(Assist assist, _float fCoolTime);
	void Set_Assist_Is_CoolTime(Assist assist, _bool bCoolTime);
	void Set_Generator_System(weak_ptr<CGameObject> wptrSystem) { m_pGenerator_System = wptrSystem; }
	weak_ptr<CGameObject> Get_Generator_System() {return m_pGenerator_System; }

	void Set_Dance_Rhythm(list <list<pair<_float, _int>>> Rhythmlist) { m_listDanceEvent_Beats = Rhythmlist; }
	void Set_Dance_Beats(list <_float> fBeatlist) { m_listDance_Beats = fBeatlist; }
	list <list<pair<_float, _int>>> Get_Dance_Rhythm()const { return m_listDanceEvent_Beats; }
	list <_float> Get_Dance_Beats() { return m_listDance_Beats;  }

	void SetTalkStarted(_bool started) { m_bTalkStarted = started;}
public:
	_float Get_DistancePlayer(const shared_ptr<CTransform> pTransform);
	_float Get_DistancePlayer(_float3 Pos);


	void TelePort();
	void KaleStageTelePort();


private:
	void Initialize_Assist_Struct();
	void Initialize_TelePortPoint();
	void Initialize_KaleStageTelePortPoint();
	void Tick_Assist_CoolTime(_float fTimeDelta);


private:
	void Tick_BulletTime(_float fTimeDelta);


private:
	Desc m_Desc;
	map<wstring, AttackDesc> m_PlayerAttackMap;
	map<wstring, CChai::SkillDesc> m_PlayerSkillMap;
	map<wstring, ItemDesc> m_PlayerItemMap;
	map<wstring, ChipDesc> m_PlayerChipMap;
	shared_ptr<CChai>  m_pPlayer = nullptr;
	shared_ptr<class CCharacterController>  m_pPlayerCCT = nullptr;
	vector<pair<_int, _int>> m_CorusScore;
	map<wstring, shared_ptr<CGameObject>> m_Assist;
	shared_ptr<CGameObject>  m_p808_Controller = nullptr;
	list<shared_ptr<CGameObject>> m_listBulletTime_Object = {};
	vector<_float4> PlayerTeleportPos;
	vector<_float3> KaleStagePlayerTeleportPos;
	_uint m_iTeleportCount = { 0 };
	_uint m_iKaleTeleportCount = { 0 };
	_int m_iAssist = 0;
	_int m_i808Rhythm = 0;
	_float m_fCoolTimeTalk = { 0.f };
	_float m_fAutoTalkTimer = { 0.f };
	_bool m_bProgressTalk = { false };
	const _float TALK_COOLDOWN = 1.0f;
	const _float AUTO_TALK_TIME = 2.0f;
	_bool  m_bTalkStarted = { false };
	

	Assist_Struct m_Assists[(_int)Assist::End];
	weak_ptr<CGameObject>  m_pGenerator_System = {};

	list <list<pair<_float, _int>>> m_listDanceEvent_Beats;
	list <_float> m_listDance_Beats;
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	void Free();
};

END

//if (GAMEINSTANCE->Get_KeyDown(DIK_E))
//{
//	if (m_pPlayer)
//	{
//		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_Position());
//		PlayerPos.x = 0.9f;
//		PlayerPos.y = 0.5f;
//		PlayerPos.z = 15.8f;
//		m_pPlayerCCT = static_pointer_cast<CCharacterController>(static_pointer_cast<CChai>(m_pPlayer)->Find_Component(L"Com_CCT"));
//		m_pPlayerCCT->Disable_Gravity();
//		m_pPlayerCCT->Disable_Pseudo_Physics();
//		static_pointer_cast<CChai>(m_pPlayer)->Set_Player_Position(XMLoadFloat4(&PlayerPos));
//	}
//}
//else if (GAMEINSTANCE->Get_KeyDown(DIK_R))
//{
//	if (m_pPlayer)
//	{
//		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_Position());
//		cout << "X: " << PlayerPos.x << endl;
//		cout << "Y: " << PlayerPos.y << endl;
//		cout << "Z: " << PlayerPos.z << endl;
//	}
//}