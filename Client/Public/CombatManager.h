#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "MonsterBase.h"
BEGIN(Client)

class CCombat_Manager
{
	DECLARE_SINGLETON(CCombat_Manager)

private:
	CCombat_Manager();

public:
	~CCombat_Manager();

public:
	struct RESULT_RANK
	{
		string strCombatTimeRank = "";
		string strJustTimeRank = "";
		string ScoreRank = "";
	};
public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);
	void Render();

	void Register_Enemy(shared_ptr<CGameObject> pEnemy) { m_listEnemy.push_back(pEnemy); }
	list<shared_ptr<CGameObject>>& Get_Enemy_List() { return m_listEnemy; }
	_int Get_Wave_Num() const { return (_int)m_WaveList.size();  }
	void Erase_Enemy(shared_ptr<CGameObject> pEnemy);

	_bool Get_Combat_End() const { return m_bCombat_End; }
	_bool Is_Combat_Engage() const { return m_bCombat_Engage; }
	void Set_Combat_Engage(_bool bEngage) { m_bCombat_Engage = bEngage; }
	void AllDeath();
	void Set_Trasnition_End(_bool bTransitionEnd) { m_bTransition_End = bTransitionEnd; }

	_bool Get_Is_Combat()const {return m_bCombat ; }
	void Set_Combat_True() { m_bCombat = true; }

	void Input_Just_Timing(_bool bIsJust) { m_vecJustTiming.push_back(bIsJust); }
	void Reset_Just_Timing() { m_vecJustTiming.clear(); }
	void Reset_CombatTimer() { m_fCombatTimer = 0.f; }
	void Add_Combo() { m_iCombo++; m_fComboTimer = 2.5f; }

	const string& Get_Current_Battle_ID() const { return m_strCurrent_BattleID; }
	void Set_Current_Battle_ID(string strBattleID) { m_strCurrent_BattleID = strBattleID; }
public:
	void Push_Wave(const vector<CMonsterBase::Monster_DESC>& waveDesc) { m_WaveList.push_back(waveDesc); }
	void Finish_Input_Wave(_bool bSkipEngageMotion = false, _bool bSkipFinalResult = false);

	void Change_Wave();
	RESULT_RANK Calculate_Combat_Rank();

private:
	_int JustTiming_Percentage();
	void Reset_JustTiming();
public:
	_bool Check_Battle_End();
private:
	list<vector<CMonsterBase::Monster_DESC>> m_WaveList;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_int m_iEnemyCount = 0;
	list<shared_ptr<CGameObject>> m_listEnemy = {};
	_bool m_bCombat_Engage = false;
	_bool m_bCombat_End = false;
	_bool m_bWave_Changing = false;
	_bool m_bTransition_End = false;
	_bool m_bCombat = false;

	vector<_bool> m_vecJustTiming = {};
	_float m_fCombatTimer = 0.f;
	_int m_iCombo = 0;
	_float m_fComboTimer = 2.5f;
	_bool m_bSkipFinalResult = false;

	string m_strCurrent_BattleID = {};
public:
	void Free();
};

END