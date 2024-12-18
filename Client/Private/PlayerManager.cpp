#pragma once
#include "PlayerManager.h"
#include "GameInstance.h"
#include "iostream"
#include "CharacterController.h"
#include "Chai.h"
#include "Player_Camera.h"
#include "808_Companion.h"
#include "CUi_Mgr.h"
#include "GameManager.h"
#include "CUi_SkillChangeSystem.h"


IMPLEMENT_SINGLETON(CPlayer_Manager)

CPlayer_Manager::CPlayer_Manager()
{

}

CPlayer_Manager::~CPlayer_Manager()
{
    Free();
}

void CPlayer_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    Initialize_TelePortPoint();
    Initialize_KaleStageTelePortPoint();
    Initialize_Assist_Struct();
}

void CPlayer_Manager::PriorityTick(_float fTimeDelta)
{
}

void CPlayer_Manager::Tick(_float fTimeDelta)
{
    Tick_BulletTime(fTimeDelta);
    Tick_Assist_CoolTime(fTimeDelta);
    if (m_bTalkStarted)
    {
        UpdateTalkSystem(fTimeDelta);
    }

    if (m_Desc.fRankScore > 0)
    {
        m_Desc.fRankScore -= 1.f * fTimeDelta;
        if (m_Desc.fRankScore < 0.f)
            m_Desc.fRankScore = 0.f;
    }

    if (GAMEINSTANCE->Get_KeyDown(DIK_F2))
    {
        if (CGame_Manager::Get_Instance()->Get_Current_Level() == LEVEL_TEST)
        {
            TelePort();
        }
        else if (CGame_Manager::Get_Instance()->Get_Current_Level() == LEVEL_KALESTAGE)
        {
            KaleStageTelePort();
        }
    }
    else if (GAMEINSTANCE->Get_KeyDown(DIK_P))
    {
        if (m_pPlayer)
        {
            _float4 PlayerPos = { 0, 0, 0, 0 };
            XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_Position());
            cout << "X: " << PlayerPos.x << endl;
            cout << "Y: " << PlayerPos.y << endl;
            cout << "Z: " << PlayerPos.z << endl;
        }
    }
    else if (GAMEINSTANCE->Get_KeyDown(DIK_F))
    {
        PLAYERMANAGER->Change_Assist();
    }
    
    if (GAMEINSTANCE->Get_KeyDown(DIK_F6))
    {
        PLAYERMANAGER->EnableReverb();
    }

}

void CPlayer_Manager::LateTick(_float fTimeDelta)
{
    
}

void CPlayer_Manager::Tick_Engine(_float fTimeDelta)
{
    PriorityTick(fTimeDelta);
    Tick(fTimeDelta);
    LateTick(fTimeDelta);
}

void CPlayer_Manager::Render()
{

}

void CPlayer_Manager::Add_CorusScore(pair<_int, _int> Score)
{
    m_CorusScore.emplace_back(Score);
}

void CPlayer_Manager::Set_Hp(_float fHp)
{
    { m_Desc.Hp = fHp; }

    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_CHANGE_HP);
}

void CPlayer_Manager::Set_MaxHp(_float fMaxHp)
{
    { m_Desc.MaxHp = fMaxHp; }
    m_pPlayer->Set_MaxHp(fMaxHp);
    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_CHANGE_MAX_HP);
}

void CPlayer_Manager::Set_Reverb(_float fReverb)
{
    { m_Desc.Reverb = fReverb; }
   // m_pPlayer->Set_MaxReverb(fReverb);
    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_CHANGE_REVERB);
}

void CPlayer_Manager::Set_MaxReverb(_float fMaxReverb)
{
    { m_Desc.MaxReverb = fMaxReverb; }

    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_CHANGE_MAX_REVERB);
}

void CPlayer_Manager::Sub_PlayerGear(_uint iGear)
{
    if (m_Desc.iPlayerGear < iGear)
        return;

    m_Desc.iPlayerGear -= (_int)iGear;
}

CPlayer_Manager::ChipDesc CPlayer_Manager::Get_PlayerChip(wstring StrName)
{
    auto iter = m_PlayerChipMap.find(StrName);

    if (iter == m_PlayerChipMap.end())
    {
        CPlayer_Manager::ChipDesc Desc;
        Desc.iUpGrade = 0;
        return Desc;
    }

    return iter->second;
}

_bool CPlayer_Manager::Is_HasThing(ThingType Type, wstring ThingName)
{
    switch (Type)
    {
    case ThingType::Attack:
    {
        auto iter = m_PlayerAttackMap.find(ThingName);
        if (iter == m_PlayerAttackMap.end())
            return false;
    }
        break;
    case ThingType::Skill:
    {
        auto iter = m_PlayerSkillMap.find(ThingName);
        if (iter == m_PlayerSkillMap.end())
            return false;
    }
        break;
    case ThingType::Item:
    {
        auto iter = m_PlayerItemMap.find(ThingName);
        if (iter == m_PlayerItemMap.end())
            return false;
    }
        break;
    case ThingType::Chip:
    {
        auto iter = m_PlayerChipMap.find(ThingName);
        if (iter == m_PlayerChipMap.end())
            return false;
    }
        break;
    default:
        assert(false);
        break;
    }
    return true;
}

void CPlayer_Manager::Add_Thing(ThingType Type, wstring ThingName, void* pArg)
{
    switch (Type)
    {
    case ThingType::Attack:
        m_PlayerAttackMap.emplace(ThingName, *(reinterpret_cast<AttackDesc*>(pArg)));
        break;
    case ThingType::Skill:
        m_PlayerSkillMap.emplace(ThingName, *(reinterpret_cast<CChai::SkillDesc*>(pArg)));
        break;
    case ThingType::Item:
        m_PlayerItemMap.emplace(ThingName, *(reinterpret_cast<ItemDesc*>(pArg)));
        break;
    case ThingType::Chip:
    {
        auto iter = m_PlayerChipMap.find(ThingName);
        if (iter != m_PlayerChipMap.end())
            ++iter->second.iUpGrade;

        m_PlayerChipMap.emplace(ThingName, *(reinterpret_cast<ChipDesc*>(pArg)));
    }
        break;
    default:
        assert(false);
        break;
    }  
}

_uint CPlayer_Manager::Is_UpGradeCount(wstring ThingName)
{
    auto iter = m_PlayerChipMap.find(ThingName);
    if (iter == m_PlayerChipMap.end())
        return 0;

    return iter->second.iUpGrade;
}

void CPlayer_Manager::Add_PlayerSkill(wstring SkillName, CChai::SkillDesc Desc)
{
    auto iter = m_PlayerSkillMap.find(SkillName);
    if (iter == m_PlayerSkillMap.end())
    {
        m_PlayerSkillMap.emplace(SkillName, Desc);
    }
    else
    {
        cout << "Add_PlayerSkill Error" << endl;
        assert(false);
    }
}

CChai::SkillDesc CPlayer_Manager::Find_PlayerSkill(wstring SkillName) const
{
    auto iter = m_PlayerSkillMap.find(SkillName);
    if (iter == m_PlayerSkillMap.end())
    {
        cout << "Find_PlayerSkill Error" << endl;
        return CChai::SkillDesc();
    }
    return iter->second;
}

void CPlayer_Manager::Set_Player(shared_ptr<CGameObject> pPlayer)
{
    m_pPlayer = static_pointer_cast<CChai>(pPlayer);
    GAMEINSTANCE->Set_Listener(m_pPlayer);
}

shared_ptr<CTransform> CPlayer_Manager::Get_PlayerTransform() const
{
    if (m_pPlayer)
    {
        return m_pPlayer->Get_Transform();
    }

    return nullptr;
}

_vector CPlayer_Manager::Get_PlayerPos() const
{
    if (m_pPlayer)
    {
        return m_pPlayer->Get_Transform()->Get_Position();
    }
    else
    {
        _float Temp = 0;
        return XMLoadFloat(&Temp);
    }
}

_matrix CPlayer_Manager::Get_PlayerMatrix() const
{
    return m_pPlayer->Get_Transform()->Get_WorldMatrix_XMMat();
}

void CPlayer_Manager::Reset_Assists()
{
    for (auto& iter : m_Assist)
    {
        if (iter.second)
            iter.second->Dead();
    }
    m_Assist.clear();
}

shared_ptr<CGameObject> CPlayer_Manager::Get_Assist(const wstring& AssistName)
{
    auto iter = m_Assist.find(AssistName);
    if (iter == m_Assist.end())
    {
        return nullptr;
    }
    return iter->second;
}

void CPlayer_Manager::Shake(_float fDuration, _float fPower, _bool bClear)
{
    static_pointer_cast<CCamera_Player>(static_pointer_cast<CChai>(m_pPlayer)->Get_Player_Camera())->Shake(fDuration, fPower, bClear);
}

void CPlayer_Manager::Bullet_Time(_float fDuration, _float fMultiflier, shared_ptr<CGameObject> pObject)
{
    m_listBulletTime_Object.push_back(pObject);
    pObject->Set_Time_Multiflier_Duration(fDuration);
    pObject->Set_Time_Multiflier(fMultiflier);
}

void CPlayer_Manager::Callback_Event(string strEvent)
{
    static_pointer_cast<CChai>(m_pPlayer)->CallBack_Event(strEvent);
}

void CPlayer_Manager::Start_2DScroll(_fvector vLook, _float fDistance)
{
    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(m_pPlayer);
    pPlayer->Get_Player_Camera()->Start_2DScroll(vLook, fDistance);
    pPlayer->Set_2D_Mode(true);

}

void CPlayer_Manager::Start_2DScroll(const _float3& vLook, _float fDistance)
{
    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(m_pPlayer);
    pPlayer->Get_Player_Camera()->Start_2DScroll(vLook, fDistance);
    pPlayer->Set_2D_Mode(true);
}

void CPlayer_Manager::End_2DScroll()
{
    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(m_pPlayer);
    pPlayer->Get_Player_Camera()->End_2DScroll();
    pPlayer->Set_2D_Mode(false);
    pPlayer->Set_2D_Fixed_Pos(0.f);
    pPlayer->Set_2D_Fixed_XYZ("");
}

void CPlayer_Manager::Change_Assist()
{
    m_iAssist++;
    if (m_iAssist > 2)
        m_iAssist = 0;

    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_CHANGE_PARTNER);
}

void CPlayer_Manager::ProceedTalk()
{
    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
}

void CPlayer_Manager::UpdateTalkSystem(float fTimeDelta)
{
    if (!m_bProgressTalk)
    {
        m_fAutoTalkTimer += fTimeDelta;

        if (m_fAutoTalkTimer >= AUTO_TALK_TIME)
        {
            PLAYERMANAGER->ProceedTalk();
            m_bProgressTalk = true;
            m_fAutoTalkTimer = 0.f;  
        }

        if (GAMEINSTANCE->Get_KeyDown(DIK_Z))
        {
            PLAYERMANAGER->ProceedTalk();
            m_bProgressTalk = true;
            m_fAutoTalkTimer = 0.f;  
        }
    }

    if (m_bProgressTalk)
    {
        m_fCoolTimeTalk += fTimeDelta;

        if (m_fCoolTimeTalk >= TALK_COOLDOWN)
        {
            m_fCoolTimeTalk = 0.f;
            m_bProgressTalk = false;
        }
    }
}

void CPlayer_Manager::ResetAutoTalkTimer()
{
    m_fAutoTalkTimer = 0.f;
}

void CPlayer_Manager::EnableReverb()
{
    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_ENABLE_REVERB);
}


void CPlayer_Manager::Reset_808_Controller()
{
    if (m_p808_Controller)
    {
        m_p808_Controller->Dead();
        m_p808_Controller = nullptr;
    }
}

void CPlayer_Manager::Set_Assist_CoolTime(Assist assist, _float fCoolTime)
{
    m_Assists[(_int)assist].fInitCoolTime = fCoolTime;
}

void CPlayer_Manager::Set_Assist_Is_CoolTime(Assist assist, _bool bCoolTime)
{
    m_Assists[(_int)assist].bIs_CoolTime = bCoolTime;
}

_float CPlayer_Manager::Get_DistancePlayer(const shared_ptr<CTransform> pTransform)
{
    if (m_pPlayer)
    {
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(pTransform->Get_Position(), Get_PlayerPos())));
    }

    return 0;
}

_float CPlayer_Manager::Get_DistancePlayer(_float3 Pos)
{
    if (m_pPlayer)
    {
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&Pos), Get_PlayerPos())));
    }

    return 0;
}


void CPlayer_Manager::Initialize_Assist_Struct()
{
    m_Assists[(_int)Assist::Peppermint].fInitCoolTime = 5.f;

    m_Assists[(_int)Assist::Macaron].fInitCoolTime    = 7.f;

    m_Assists[(_int)Assist::Korsica].fInitCoolTime    = 6.f;
}

void CPlayer_Manager::Initialize_TelePortPoint()
{
    PlayerTeleportPos.reserve(20);

    PlayerTeleportPos.emplace_back(_float4(0, 2, 0, 1)); // LEVEL_GIMMICTESTStartPos and UiTool

    PlayerTeleportPos.emplace_back(_float4(7.8f, 9, 162.f, 1)); // ProdDeptDoor

    PlayerTeleportPos.emplace_back(_float4(-28.f, 23.5f, 211.f, 1)); // Bulb

    PlayerTeleportPos.emplace_back(_float4(52.0883f, 41, 309.182f, 1)); // LargeElevator 

    PlayerTeleportPos.emplace_back(_float4(98.f, 118.f, 410.f, 1)); // 2d Entrance

    PlayerTeleportPos.emplace_back(_float4(-316.f, 90.f, -56.430f, 1)); // 2d translation

    PlayerTeleportPos.emplace_back(_float4(-316, 65, 140, 1)); // 2d elevator

    PlayerTeleportPos.emplace_back(_float4(66.f, 31.f, 804.f, 1)); // Generator

    PlayerTeleportPos.emplace_back(_float4(84.1956f, 55, 647.496f, 1)); // GaraElevator

    PlayerTeleportPos.emplace_back(_float4(90.f, 44.f, 695.f, 1)); // laser

    PlayerTeleportPos.emplace_back(_float4(92.6f, 30.f, 720.5f, 1)); // levelup_Machine
}

void CPlayer_Manager::Initialize_KaleStageTelePortPoint()
{
    KaleStagePlayerTeleportPos.reserve(20);

    KaleStagePlayerTeleportPos.emplace_back(_float3(-24, 86, 184)); // ThirdBattle(RobotPunch)

    KaleStagePlayerTeleportPos.emplace_back(_float3(25, 81, 129)); // Scroll2DMiddle

    KaleStagePlayerTeleportPos.emplace_back(_float3(76, 64, 216)); // Scroll2D

    KaleStagePlayerTeleportPos.emplace_back(_float3(-36, 8, 26));  // FirstButton

    KaleStagePlayerTeleportPos.emplace_back(_float3(-26, 11, 54)); // PeppermintGimmic

    KaleStagePlayerTeleportPos.emplace_back(_float3(-15, 11, 80)); // MacaronGimmic

    KaleStagePlayerTeleportPos.emplace_back(_float3(0, 19, 122)); // FirstBattle

    KaleStagePlayerTeleportPos.emplace_back(_float3(-18, 23, 197)); // FirstWire

    KaleStagePlayerTeleportPos.emplace_back(_float3(3, 35, 135)); // AfterContainerCutscene

    KaleStagePlayerTeleportPos.emplace_back(_float3(-51, 71, 156)); // KorsicaGimmic

    KaleStagePlayerTeleportPos.emplace_back(_float3(-60, 95, 208)); // SecondWire(Robot)

    KaleStagePlayerTeleportPos.emplace_back(_float3(105, 56, 187)); // SecondBattle

    KaleStagePlayerTeleportPos.emplace_back(_float3(179, 104, 315)); // SecondRobotBeam

    KaleStagePlayerTeleportPos.emplace_back(_float3(-52, 94, 141)); // Scroll2DEnd

    KaleStagePlayerTeleportPos.emplace_back(_float3(0, 49, 256)); // RhythmJump

    // CNMNStart 
}

void CPlayer_Manager::TelePort()
{
    _float4 PlayerPos = {0, 0, 0, 1};

	if (m_pPlayer)
	{
		PlayerPos.x = PlayerTeleportPos[m_iTeleportCount].x;
		PlayerPos.y = PlayerTeleportPos[m_iTeleportCount].y;
		PlayerPos.z = PlayerTeleportPos[m_iTeleportCount].z;
		m_pPlayerCCT = static_pointer_cast<CCharacterController>(static_pointer_cast<CChai>(m_pPlayer)->Find_Component(L"Com_CCT"));
		static_pointer_cast<CChai>(m_pPlayer)->Set_Player_Position(XMLoadFloat4(&PlayerPos));
	}
	m_iTeleportCount += 1;
	if (PlayerTeleportPos.size() <= m_iTeleportCount)
	{
		m_iTeleportCount = 0;
	}
}

void CPlayer_Manager::KaleStageTelePort()
{
    _float3 PlayerPos = { 0, 0, 0 };

    if (m_pPlayer)
    {
        PlayerPos.x = KaleStagePlayerTeleportPos[m_iKaleTeleportCount].x;
        PlayerPos.y = KaleStagePlayerTeleportPos[m_iKaleTeleportCount].y;
        PlayerPos.z = KaleStagePlayerTeleportPos[m_iKaleTeleportCount].z;
        m_pPlayerCCT = static_pointer_cast<CCharacterController>(static_pointer_cast<CChai>(m_pPlayer)->Find_Component(L"Com_CCT"));
        static_pointer_cast<CChai>(m_pPlayer)->Set_Player_Position(XMLoadFloat3(&PlayerPos));
    }
    m_iKaleTeleportCount += 1;
    if (KaleStagePlayerTeleportPos.size() <= m_iKaleTeleportCount)
    {
        m_iKaleTeleportCount = 0;
    }
}

void CPlayer_Manager::Tick_Assist_CoolTime(_float fTimeDelta)
{
    for (size_t i = 0; i < (_int)Assist::End; i++)
    {
        if (m_Assists[i].bIs_CoolTime)
        {
            m_Assists[i].fCurr_CoolTime += fTimeDelta;
            if (m_Assists[i].fCurr_CoolTime >= m_Assists[i].fInitCoolTime)
            {
                m_Assists[i].bIs_CoolTime = false;
                m_Assists[i].fCurr_CoolTime = 0;
            }
        }
    }
}

void CPlayer_Manager::Tick_BulletTime(_float fTimeDelta)
{
    for (auto& iter : m_listBulletTime_Object)
    {
        _float fDuration = iter->Get_Time_Multiflier_Duration();
        if (fDuration > 0.f)
            fDuration -= fTimeDelta;
        else if(fDuration < 0.f)
        {
            fDuration = 0.f;
            iter->Set_Time_Multiflier(1.f);
        }
        
        iter->Set_Time_Multiflier_Duration(fDuration);
    }

    auto iter = m_listBulletTime_Object.begin();
    while (iter != m_listBulletTime_Object.end())
    {
        if (iter->get()->Get_Time_Multiflier_Duration() == 0.f)
            iter = m_listBulletTime_Object.erase(iter);
        else
            iter++;
    }
}

void CPlayer_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
