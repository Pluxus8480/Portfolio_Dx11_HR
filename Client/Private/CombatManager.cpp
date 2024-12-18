#pragma once
#include "CombatManager.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "CUi_PlayerBar.h"
#include "CUi_Mgr.h"
#include "CUi_FinalResultSystem.h"
IMPLEMENT_SINGLETON(CCombat_Manager)

CCombat_Manager::CCombat_Manager()
{

}

CCombat_Manager::~CCombat_Manager()
{
    Free();
}

void CCombat_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

}

void CCombat_Manager::PriorityTick(_float fTimeDelta)
{

}

void CCombat_Manager::Tick(_float fTimeDelta)
{
    Change_Wave();

    if (m_bTransition_End)
    {
        for (auto& iter : m_WaveList.front())
            GAMEINSTANCE->Add_Clone(GAMEMANAGER->Get_Current_Level(), TEXT("Layer_Monster"), iter.strObjID, &iter);
        m_WaveList.pop_front();

        m_bWave_Changing = false;
        m_bTransition_End = false;
    }

    if(m_bCombat)
    {
        m_fCombatTimer += fTimeDelta;
        m_fComboTimer -= fTimeDelta;
        if (m_fComboTimer <= 0)
            m_iCombo = 0;
        auto pPM = PLAYERMANAGER;
        pPM->Set_PlayerCombo(m_iCombo);
        if (m_listEnemy.empty() && m_WaveList.empty())
        {
            if (m_bSkipFinalResult)
            {
                m_bSkipFinalResult = false;
                m_bCombat = false;
                m_bCombat_Engage = false;
                m_bCombat_End = true;
            }
            else
            {
                static_pointer_cast<CUi_PlayerBar>(CUi_Mgr::Get_Instance()->Find_UI(CUi_PlayerBar::ObjID))->Basic_Mode();
                CUi_Mgr::Get_Instance()->Add_Clone(CUi_FinalResultSystem::ObjID, GAMEMANAGER->Get_Current_Level(), L"CUI", CUi_FinalResultSystem::ObjID);
                m_bCombat = false;
                m_bCombat_Engage = false;
                m_bCombat_End = true;
                pPM->Set_CombatTimer(m_fCombatTimer);
                pPM->Set_JustTimingCount(JustTiming_Percentage());
                pPM->Reset_RankScore();

                Reset_JustTiming();
                m_iCombo = 0;
                pPM->Set_PlayerCombo(m_iCombo);
                m_fComboTimer = 2.5f;
            }
        }
        else
            m_bCombat_End = false;
    }
    
}

void CCombat_Manager::LateTick(_float fTimeDelta)
{
    if (GAMEINSTANCE->Get_KeyDown(DIK_F9))
    {
        AllDeath();
    }
}

void CCombat_Manager::Tick_Engine(_float fTimeDelta)
{
    PriorityTick(fTimeDelta);
    Tick(fTimeDelta);
    LateTick(fTimeDelta);
}

void CCombat_Manager::Render()
{

}

void CCombat_Manager::Erase_Enemy(shared_ptr<CGameObject> pEnemy)
{
    auto iter = m_listEnemy.begin();
    while (iter != m_listEnemy.end())
    {
        if (*iter == pEnemy)
            iter = m_listEnemy.erase(iter);
        else
            iter++;
    }
}

void CCombat_Manager::AllDeath()
{
    DamageInfo dInfo = {};
    dInfo.eAttackType = EAttackType::Base;
    dInfo.eAttack_Direction = EAttackDirection::DEFAULT;
    dInfo.fAmountDamage = 9999.f;
    dInfo.pObjectOwner = PLAYERMANAGER->Get_Player();
 
	for (auto& iter : m_listEnemy)
		iter->TakeDamage(dInfo);

}

void CCombat_Manager::Finish_Input_Wave(_bool bSkipEngageMotion, _bool bSkipFinalResult)
{
    for (auto& iter : m_WaveList.front())
        GAMEINSTANCE->Add_Clone(GAMEMANAGER->Get_Current_Level(),TEXT("Layer_Monster"), iter.strObjID, &iter);
    m_WaveList.pop_front();
    m_bCombat_Engage = !bSkipEngageMotion;
    m_bCombat_End = false;
    m_bCombat = true;
    m_bSkipFinalResult = bSkipFinalResult;
}

void CCombat_Manager::Change_Wave()
{
    if (m_listEnemy.empty() && !m_WaveList.empty() && !m_bWave_Changing)
    {
        m_bWave_Changing = true;
        PLAYERMANAGER->Callback_Event("Battle_Transition");
    }
}

_int CCombat_Manager::JustTiming_Percentage()
{   
    _float res = {};
    _int success = 0;
    _int fail = 0;
    for (size_t i = 0; i < m_vecJustTiming.size(); i++)
    {
        if (m_vecJustTiming[i])
            success++;
        else
            fail++;
    }
    
    res = (_float)success / (success + fail);
    res *= 100.f;

    return res;
}

void CCombat_Manager::Reset_JustTiming()
{
    m_vecJustTiming.clear();
}

CCombat_Manager::RESULT_RANK CCombat_Manager::Calculate_Combat_Rank()
{
    RESULT_RANK resREsult = {};
    string combatTimeRank = {};
    string JustTimeRank = {};
    string ScoreRank = {};

    _int iJustPerCentage = JustTiming_Percentage();
    if (iJustPerCentage >= 80)
        JustTimeRank = "S";
    else if (iJustPerCentage >= 70.f && iJustPerCentage < 80.f)
        JustTimeRank = "A";
    else if (iJustPerCentage >= 60.f && iJustPerCentage < 70.f)
        JustTimeRank = "B";
    else if (iJustPerCentage >= 50.f && iJustPerCentage < 60.f)
        JustTimeRank = "C";
    else
        JustTimeRank = "D";

    _int iPlayerScore = PLAYERMANAGER->Get_PlayerScore();

    /* Level_1 Battle  1 */
    if (m_strCurrent_BattleID == "St01_A01_7")
    {
        if (m_fCombatTimer <= 30.f)
            combatTimeRank = "S";
        else if (m_fCombatTimer > 30.f && m_fCombatTimer <= 40.f)
            combatTimeRank = "A";
        else if (m_fCombatTimer > 40.f && m_fCombatTimer <= 50.f)
            combatTimeRank = "B";
        else if (m_fCombatTimer > 50.f && m_fCombatTimer <= 60.f)
            combatTimeRank = "C";
        else 
            combatTimeRank = "D";

        if(iPlayerScore >= 3000)
            ScoreRank = "S";
        else if (iJustPerCentage >= 2500 && iJustPerCentage < 3000)
            ScoreRank = "A";
        else if (iJustPerCentage >= 2000 && iJustPerCentage < 2500)
            ScoreRank = "B";
        else if (iJustPerCentage >= 1000 && iJustPerCentage < 2000)
            ScoreRank = "C";
        else
            ScoreRank = "D";
   }

    /* Level_1 Battle  2 */
    else if (m_strCurrent_BattleID == "St01_A02_13")
    {
        if (m_fCombatTimer <= 60.f)
            combatTimeRank = "S";
        else if (m_fCombatTimer > 60.f && m_fCombatTimer <= 70.f)
            combatTimeRank = "A";
        else if (m_fCombatTimer > 70.f && m_fCombatTimer <= 80.f)
            combatTimeRank = "B";
        else if (m_fCombatTimer > 80.f && m_fCombatTimer <= 90.f)
            combatTimeRank = "C";
        else
            combatTimeRank = "D";

        if (iPlayerScore >= 10000)
            ScoreRank = "S";
        else if (iJustPerCentage >= 9000 && iJustPerCentage < 10000)
            ScoreRank = "A";
        else if (iJustPerCentage >= 7000 && iJustPerCentage < 9000)
            ScoreRank = "B";
        else if (iJustPerCentage >= 5000 && iJustPerCentage < 7000)
            ScoreRank = "C";
        else
            ScoreRank = "D";
    }
    /* Level_1 Battle  3 */
    else if (m_strCurrent_BattleID == "St01_A02_23")
    {
        if (m_fCombatTimer <= 120.f)
            combatTimeRank = "S";
        else if (m_fCombatTimer > 110.f && m_fCombatTimer <= 120.f)
            combatTimeRank = "A";
        else if (m_fCombatTimer > 100.f && m_fCombatTimer <= 110.f)
            combatTimeRank = "B";
        else if (m_fCombatTimer > 90.f && m_fCombatTimer <= 100.f)
            combatTimeRank = "C";
        else
            combatTimeRank = "D";

        if (iPlayerScore >= 10000)
            ScoreRank = "S";
        else if (iJustPerCentage >= 9000 && iJustPerCentage < 10000)
            ScoreRank = "A";
        else if (iJustPerCentage >= 7000 && iJustPerCentage < 9000)
            ScoreRank = "B";
        else if (iJustPerCentage >= 5000 && iJustPerCentage < 7000)
            ScoreRank = "C";
        else
            ScoreRank = "D";
    }

    /* Level_1 Battle  4 */
    else if (m_strCurrent_BattleID == "St01_A04B_1")
    {
        if (m_fCombatTimer <= 120.f)
            combatTimeRank = "S";
        else if (m_fCombatTimer > 110.f && m_fCombatTimer <= 120.f)
            combatTimeRank = "A";
        else if (m_fCombatTimer > 100.f && m_fCombatTimer <= 110.f)
            combatTimeRank = "B";
        else if (m_fCombatTimer > 90.f && m_fCombatTimer <= 100.f)
            combatTimeRank = "C";
        else
            combatTimeRank = "D";

        if (iPlayerScore >= 15000)
            ScoreRank = "S";
        else if (iJustPerCentage >= 13000 && iJustPerCentage < 15000)
            ScoreRank = "A";
        else if (iJustPerCentage >= 11000 && iJustPerCentage < 13000)
            ScoreRank = "B";
        else if (iJustPerCentage >= 10000 && iJustPerCentage < 11000)
            ScoreRank = "C";
        else
            ScoreRank = "D";
    }
    else
    {
        combatTimeRank = "A";
        ScoreRank = "S";
    }
    resREsult.ScoreRank = ScoreRank;
    resREsult.strCombatTimeRank = "S";
    resREsult.strJustTimeRank = JustTimeRank;

    return resREsult;
}

_bool CCombat_Manager::Check_Battle_End()
{
    _bool bBattleEnd = false;

    if (m_listEnemy.empty())
        bBattleEnd = true;

    return bBattleEnd;
}

void CCombat_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
