#pragma once
#include "CUi_Mgr.h"
#include "CUi.h"
#include "GameInstance.h"
#include "iostream"
#include "CUi_StoreSystem.h"
#include "CUi_KorsicaSystem.h"
#include "CUi_PlayerBar.h"
#include "CUi_FinalResultSystem.h"
#include "CUi_StageResultSystem.h"
#include "CUi_RankSystem.h"
#include "CUi_ScoreSystem.h"
#include "CUi_BeatMarkerSystem.h"
#include "CUi_RekkaSystem.h"
#include "CUi_KaleSystem.h"
#include "CUi_MimosaSystem.h"
#include "CUi_PlayerBar.h" 
#include "CUi_TramTimer.h"
#include "CUi_CineAttackSystem.h"
#include "CUi_GearSystem.h"
#include "PlayerManager.h"

IMPLEMENT_SINGLETON(CUi_Mgr)

CUi_Mgr::CUi_Mgr()
{

}

CUi_Mgr::~CUi_Mgr()
{
	Free();
}

void CUi_Mgr::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

void CUi_Mgr::PriorityTick(_float fTimeDelta)
{
    m_PriorityList.clear();
    m_NormalList.clear();
    m_BackGroundList.clear();
    m_RealBackGroundList.clear();
    m_LowestList.clear();
}

void CUi_Mgr::Tick(_float fTimeDelta)
{
    auto UiMapiter = m_UiMap.begin();

    for (UiMapiter; UiMapiter != m_UiMap.end();)
    {
        if (UiMapiter->second->Is_Dead())
        {
            m_UiMap.erase(UiMapiter++);
        }
        else
        {
            ++UiMapiter;
        }
    }

    auto DefaultListiter = m_DefaultList.begin();
    for (; DefaultListiter != m_DefaultList.end();)
    {
        if ((*DefaultListiter)->Is_Dead())
        {
            DefaultListiter = m_DefaultList.erase(DefaultListiter);
        }
        else
        {
            ++DefaultListiter;
        }
    }

    auto SpriteListiter = m_SpriteList.begin();
    for (; SpriteListiter != m_SpriteList.end();)
    {
        if ((*SpriteListiter)->Is_Dead())
        {
            SpriteListiter = m_SpriteList.erase(SpriteListiter);
        }
        else
        {
            ++SpriteListiter;
        }
    }
}

void CUi_Mgr::LateTick(_float fTimeDelta)
{
    CUi::RenderType Type = CUi::RenderType::End;
    for (auto& iter : m_UiMap)
    {
        if ((iter.second)->Get_Active() && !(iter.second)->Is_Dead() && (iter.second)->Get_Sort())
        {
            Type = (iter.second)->Get_RenderType();

            switch (Type)
            {
            case CUi::RenderType::Priority:
                m_PriorityList.emplace_back(iter.second);
                break;
            case CUi::RenderType::Normal:
                m_NormalList.emplace_back(iter.second);
                break;
            case CUi::RenderType::BackGround:
                m_BackGroundList.emplace_back(iter.second);
                break;
            case CUi::RenderType::RealBackGround:
                m_RealBackGroundList.emplace_back(iter.second);
                break; 
            case CUi::RenderType::Lowest:
                m_LowestList.emplace_back(iter.second);
                break;
            default:
                break;
            }
        }
    }
    for (auto& iter : m_DefaultList)
    {
        if (iter->Get_Active() && !iter->Is_Dead())
        {
            Type = iter->Get_RenderType();

            switch (Type)
            {
            case CUi::RenderType::Priority:
                m_PriorityList.emplace_back(iter);
                break;
            case CUi::RenderType::Normal:
                m_NormalList.emplace_back(iter);
                break;
            case CUi::RenderType::BackGround:
                m_BackGroundList.emplace_back(iter);
                break;
            case CUi::RenderType::RealBackGround:
                m_RealBackGroundList.emplace_back(iter);
                break;
            case CUi::RenderType::Lowest:
                m_LowestList.emplace_back(iter);
                break;
            default:
                break;
            }
        }
    }

    for (auto& iter : m_SpriteList)
    {
        if (iter->Get_Active() && !iter->Is_Dead())
        {
            Type = iter->Get_RenderType();

            switch (Type)
            {
            case CUi::RenderType::Priority:
                m_PriorityList.emplace_back(iter);
                break;
            case CUi::RenderType::Normal:
                m_NormalList.emplace_back(iter);
                break;
            case CUi::RenderType::BackGround:
                m_BackGroundList.emplace_back(iter);
                break;
            case CUi::RenderType::RealBackGround:
                m_RealBackGroundList.emplace_back(iter);
                break;
            case CUi::RenderType::Lowest:
                m_LowestList.emplace_back(iter);
                break;
            default:
                break;
            }
        }
    }


    Sorting_Ui();
}

void CUi_Mgr::Tick_Engine(_float fTimeDelta)
{
	PriorityTick(fTimeDelta);
	Tick(fTimeDelta);
	LateTick(fTimeDelta);
}

void CUi_Mgr::Render()
{

}

void CUi_Mgr::Add_Clone(wstring UiTag, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
    shared_ptr<CUi> pInstance;
    pInstance = static_pointer_cast<CUi>
        (GAMEINSTANCE->Add_Clone_Return(iLevelIndex, strLayerTag, strPrototypeTag, pArg));

    auto iter = m_UiMap.find(UiTag);

    if (UiTag == L"CUi_Default")
    {
        m_DefaultList.emplace_back(pInstance);
    }
    else if (UiTag == L"CUi_Sprite")
    {
        m_SpriteList.emplace_back(pInstance);
    }
    else if (UiTag == L"CUi_Font")
    {
        m_SpriteList.emplace_back(pInstance);
    }
    else if (iter != m_UiMap.end())
    {
        while (m_UiMap.end() != m_UiMap.find(UiTag))
        {
            wstring Str = L"1";
            UiTag += Str;
        }
        m_UiMap.emplace(UiTag, pInstance);
    }
    else
    {
        m_UiMap.emplace(UiTag, pInstance);
    }
}


shared_ptr<CUi> CUi_Mgr::Add_Clone_Return(wstring UiTag, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
    shared_ptr<CUi> pInstance;
    pInstance = static_pointer_cast<CUi>
        (GAMEINSTANCE->Add_Clone_Return(iLevelIndex, strLayerTag, strPrototypeTag, pArg));


    auto iter = m_UiMap.find(UiTag);

    if (UiTag == L"CUi_Default")
    {
        m_DefaultList.emplace_back(pInstance);
    }
    else if (UiTag == L"CUi_Sprite")
    {
        m_SpriteList.emplace_back(pInstance);
    }
    else if (UiTag == L"CUi_Font")
    {
        m_SpriteList.emplace_back(pInstance);
    }
    else if (iter != m_UiMap.end())
    {
        while (m_UiMap.end() != m_UiMap.find(UiTag))
        {
            wstring Str = L"1";
            UiTag += Str;
        }
        m_UiMap.insert({ UiTag, pInstance });
    }
    else
    {
        m_UiMap.insert({ UiTag, pInstance });
    }

    return pInstance;
}

void CUi_Mgr::Ui_Dead(const wstring& UiTag)
{
   auto iter = m_UiMap.find(UiTag);

   if (iter == m_UiMap.end())
       return;
   
   iter->second->Dead();
   m_UiMap.erase(iter);
}

void CUi_Mgr::Ui_AllActive(_bool Active)
{
    for (auto& iter : m_DefaultList)
        iter->Set_Active(Active);
    for (auto& iter : m_SpriteList)
        iter->Set_Active(Active);
    for (auto& iter : m_UiMap)
        (iter.second)->Set_Active(Active);
}

void CUi_Mgr::Ui_RhythmParryActive(_bool Active)
{
    for (auto& iter : m_DefaultList)
        iter->Set_Active(Active);
    for (auto& iter : m_SpriteList)
        iter->Set_Active(Active);
    for (auto& iter : m_UiMap)
        (iter.second)->Set_Active(Active);

    Set_Active(CUi_RankSystem::ObjID, true);
    Set_Active(CUi_PlayerBar::ObjID, true);
    Set_Active(CUi_ScoreSystem::ObjID, true);
}

void CUi_Mgr::Ui_Respawn(_uint iLevelIndex, const wstring& strLayerTag, const wstring& UiTag)
{
    auto iter = m_UiMap.find(UiTag);

    if (iter == m_UiMap.end())
        return;

    (iter->second)->Set_Dead(false);
    (iter->second)->Respawn();
    GAMEINSTANCE->Push_Object(iLevelIndex, strLayerTag, iter->second);
}

void CUi_Mgr::Set_Active(const wstring& UiTag, _bool Active)
{
    auto iter = m_UiMap.find(UiTag);

    if (iter == m_UiMap.end())
        return;
    
    (iter->second)->Set_Active(Active);
}

void CUi_Mgr::LevelChange_Delete()
{
    m_PriorityList.clear();
    m_NormalList.clear();
    m_BackGroundList.clear();
    m_DefaultList.clear();
    m_SpriteList.clear();
    m_UiMap.clear();
}

void CUi_Mgr::LevelChange_Create(_uint CurLevel)
{
    CUi_BeatMarkerSystem::Desc BeatMarkerDesc;
    BeatMarkerDesc.eType = CUi_BeatMarkerSystem::Type::Default;
    shared_ptr<CUi_PlayerBar> PlayerBar = { nullptr };
    switch (CurLevel)
    {
    case LEVEL_TEST:
        PlayerBar = static_pointer_cast<CUi_PlayerBar>(Add_Clone_Return(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr));
        PlayerBar->PartnerOff_Both();
        Add_Clone(CUi_RankSystem::ObjID, CurLevel, L"CUi", CUi_RankSystem::ObjID, nullptr);
        Add_Clone(CUi_ScoreSystem::ObjID, CurLevel, L"CUi", CUi_ScoreSystem::ObjID, nullptr);
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        Add_Clone(CUi_GearSystem::ObjID, CurLevel, L"CUi", CUi_GearSystem::ObjID, &BeatMarkerDesc);
        break;
    case LEVEL_UITOOL:
        Add_Clone(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr);
        Add_Clone(CUi_RankSystem::ObjID, CurLevel, L"CUi", CUi_RankSystem::ObjID, nullptr);
        Add_Clone(CUi_ScoreSystem::ObjID, CurLevel, L"CUi", CUi_ScoreSystem::ObjID, nullptr);
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        break;
    case LEVEL_LIFT:
        PlayerBar = static_pointer_cast<CUi_PlayerBar>(Add_Clone_Return(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr));
        PlayerBar->PartnerOff_Korsica();
        PlayerBar->VisbleReverb();
        Add_Clone(CUi_RankSystem::ObjID, CurLevel, L"CUi", CUi_RankSystem::ObjID, nullptr);
        Add_Clone(CUi_ScoreSystem::ObjID, CurLevel, L"CUi", CUi_ScoreSystem::ObjID, nullptr);
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        Add_Clone(CUi_GearSystem::ObjID, CurLevel, L"CUi", CUi_GearSystem::ObjID, &BeatMarkerDesc);
        break;
    case LEVEL_KORSICA:
        break;
    case LEVEL_MODELTEST:
        PlayerBar = static_pointer_cast<CUi_PlayerBar>(Add_Clone_Return(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr));
        PlayerBar->PartnerOff_Both();
        PlayerBar->VisbleReverb();
        PlayerBar->Fight_Mode();
        Add_Clone(CUi_RankSystem::ObjID, CurLevel, L"CUi", CUi_RankSystem::ObjID, nullptr);
        Add_Clone(CUi_ScoreSystem::ObjID, CurLevel, L"CUi", CUi_ScoreSystem::ObjID, nullptr);
        Add_Clone(CUi_RekkaSystem::ObjID, CurLevel, L"CUi", CUi_RekkaSystem::ObjID, nullptr);
        BeatMarkerDesc.eType = CUi_BeatMarkerSystem::Type::Rekka;
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        break;
    case LEVEL_KALEBOSS:
        PlayerBar = static_pointer_cast<CUi_PlayerBar>(Add_Clone_Return(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr));
        PlayerBar->VisbleReverb();
        CPlayer_Manager::Get_Instance()->Set_MaxReverb(150.f);
        CPlayer_Manager::Get_Instance()->Set_Reverb(150.f);
        PlayerBar->Fight_Mode();
        Add_Clone(CUi_RankSystem::ObjID, CurLevel, L"CUi", CUi_RankSystem::ObjID, nullptr);
        Add_Clone(CUi_ScoreSystem::ObjID, CurLevel, L"CUi", CUi_ScoreSystem::ObjID, nullptr);
        Add_Clone(CUi_KaleSystem::ObjID, CurLevel, L"CUi", CUi_KaleSystem::ObjID, nullptr);
        BeatMarkerDesc.eType = CUi_BeatMarkerSystem::Type::Kale;
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        break;
    case LEVEL_MIMOSA:
        PlayerBar = static_pointer_cast<CUi_PlayerBar>(Add_Clone_Return(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr));
        PlayerBar->VisbleReverb();
        PlayerBar->Fight_Mode();
        Add_Clone(CUi_RankSystem::ObjID, CurLevel, L"CUi", CUi_RankSystem::ObjID, nullptr);
        Add_Clone(CUi_ScoreSystem::ObjID, CurLevel, L"CUi", CUi_ScoreSystem::ObjID, nullptr);
        CUi_Mgr::Get_Instance()->Add_Clone(CUi_MimosaSystem::ObjID, CurLevel, L"CUi", CUi_MimosaSystem::ObjID, nullptr);
        BeatMarkerDesc.eType = CUi_BeatMarkerSystem::Type::Mimosa;
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        break;
    case LEVEL_GIMMICTEST:
        Add_Clone(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr);
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        break;
    case LEVEL_KALESTAGE:
        PlayerBar = static_pointer_cast<CUi_PlayerBar>(Add_Clone_Return(CUi_PlayerBar::ObjID, CurLevel, L"CUi", CUi_PlayerBar::ObjID, nullptr));
        PlayerBar->VisbleReverb();
        PlayerBar->Basic_Mode();
        Add_Clone(CUi_BeatMarkerSystem::ObjID, CurLevel, L"CUi", CUi_BeatMarkerSystem::ObjID, &BeatMarkerDesc);
        Add_Clone(CUi_GearSystem::ObjID, CurLevel, L"CUi", CUi_GearSystem::ObjID, &BeatMarkerDesc);
        break;
    default:
        assert(false);
        break;
    }
}

shared_ptr<CUi> CUi_Mgr::Find_UI(const wstring& wstrName)
{
    auto iter = m_UiMap.find(wstrName);

    if (iter == m_UiMap.end())
    {
        return nullptr;
    }

    return iter->second;
}

void CUi_Mgr::Open_Store()
{
    Add_Clone(CUi_StoreSystem::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi", CUi_StoreSystem::ObjID);
    Battle_End();
    Set_Active(CUi_PlayerPartnerGauge::ObjID, false);
    Set_Active(CUi_PlayerBar::ObjID, false);
    Set_Active(CUi_BeatMarkerSystem::ObjID, false);
}

void CUi_Mgr::Close_Store()
{
    Ui_Dead(CUi_StoreSystem::ObjID);

    Battle_End();

    Set_Active(CUi_PlayerPartnerGauge::ObjID, true);
    Set_Active(CUi_PlayerBar::ObjID, true);
    Set_Active(CUi_BeatMarkerSystem::ObjID, true);
}

void CUi_Mgr::Battle_Start()
{
    Set_Active(CUi_PlayerPartnerGauge::ObjID, true);
    Set_Active(CUi_RankSystem::ObjID, true);
    Set_Active(CUi_ScoreSystem::ObjID, true);
    Set_Active(CUi_PlayerBar::ObjID, true);
    static_pointer_cast<CUi_PlayerBar>(Find_UI(CUi_PlayerBar::ObjID))->Fight_Mode();
}

void CUi_Mgr::Battle_End()
{
    Set_Active(CUi_RankSystem::ObjID, false);
    Set_Active(CUi_ScoreSystem::ObjID, false);
    static_pointer_cast<CUi_PlayerBar>(Find_UI(CUi_PlayerBar::ObjID))->Basic_Mode();
}

void CUi_Mgr::Start_CutScene()
{
    Set_Active(CUi_PlayerPartnerGauge::ObjID, false);
    Set_Active(CUi_RankSystem::ObjID, false);
    Set_Active(CUi_BeatMarkerSystem::ObjID, false);
    Set_Active(CUi_ScoreSystem::ObjID, false);
    Set_Active(CUi_PlayerBar::ObjID, false);
}

void CUi_Mgr::AddCanvase(shared_ptr<class CUi_Canvas> InCanvas)
{
    for (auto& iter : m_CanvasList)
    {
        if (iter == InCanvas)
        {
            return;
        }
    }

    m_CanvasList.emplace_back(InCanvas);
}

void CUi_Mgr::DeleteCanvase(shared_ptr<class CUi_Canvas> InCanvas)
{
    auto iter = find(m_CanvasList.begin(), m_CanvasList.end(), InCanvas);

    if (iter != m_CanvasList.end())
    {
        m_CanvasList.erase(iter);
    }
}

void CUi_Mgr::NotifyUIEvent(UI_EVENT InUIEvent)
{
    for (auto& iter : m_CanvasList)
    {
        iter->ReceiveUIEvent(InUIEvent);
    }
    
}

void CUi_Mgr::Sorting_Ui()
{  
    for (auto& Sourceiter: m_PriorityList)
    {
        _uint iPriorityOverlapCount = { 0 };

        for (auto& Destiter : m_PriorityList)
        {                
            if (Overlap(Sourceiter, Destiter))
            {
                if (Destiter->Get_RenderPriority() < Sourceiter->Get_RenderPriority())
                {
                    ++iPriorityOverlapCount;
                }
                else if (Destiter->Get_RenderPriority() == Sourceiter->Get_RenderPriority())
                {
                    if (Sourceiter->Get_Scale().x + Sourceiter->Get_Scale().y < Destiter->Get_Scale().x + Destiter->Get_Scale().y)
                    {
                        ++iPriorityOverlapCount;
                    }
                }
            }
        }

        Sourceiter->Set_RenderZ(iPriorityOverlapCount);
    }

    for (auto& Sourceiter : m_NormalList)
    {
        _uint iNormalOverlapCount = { 0 };
        for (auto& Destiter : m_NormalList)
        {
            if (Overlap(Sourceiter, Destiter))
            {
                if (Destiter->Get_RenderPriority() < Sourceiter->Get_RenderPriority())
                {
                    ++iNormalOverlapCount;
                }
                else if (Destiter->Get_RenderPriority() == Sourceiter->Get_RenderPriority())
                {
                    if (Sourceiter->Get_Scale().x + Sourceiter->Get_Scale().y < Destiter->Get_Scale().x + Destiter->Get_Scale().y)
                    {
                        ++iNormalOverlapCount;
                        _uint i = 0;
                    }
                }
            }
        }

        Sourceiter->Set_RenderZ(iNormalOverlapCount);
    }

    for (auto& Sourceiter : m_BackGroundList)
    {
        _uint iBackGroundOverlapCount = { 0 };
        for (auto& Destiter : m_BackGroundList)
        {
            if (Overlap(Sourceiter, Destiter))
            {
                if (Destiter->Get_RenderPriority() < Sourceiter->Get_RenderPriority())
                {
                    ++iBackGroundOverlapCount;
                }
                else if (Destiter->Get_RenderPriority() == Sourceiter->Get_RenderPriority())
                {
                    if (Sourceiter->Get_Scale().x + Sourceiter->Get_Scale().y < Destiter->Get_Scale().x + Destiter->Get_Scale().y)
                    {
                        ++iBackGroundOverlapCount;
                    }
                }
            }
        }

        Sourceiter->Set_RenderZ(iBackGroundOverlapCount);
    }

    for (auto& Sourceiter : m_RealBackGroundList)
    {
        _uint iRealBackGroundOverlapCount = { 0 };
        for (auto& Destiter : m_RealBackGroundList)
        {
            if (Overlap(Sourceiter, Destiter))
            {
                if (Destiter->Get_RenderPriority() < Sourceiter->Get_RenderPriority())
                {
                    ++iRealBackGroundOverlapCount;
                }
                else if (Destiter->Get_RenderPriority() == Sourceiter->Get_RenderPriority())
                {
                    if (Sourceiter->Get_Scale().x + Sourceiter->Get_Scale().y < Destiter->Get_Scale().x + Destiter->Get_Scale().y)
                    {
                        ++iRealBackGroundOverlapCount;
                    }
                }
            }
        }

        Sourceiter->Set_RenderZ(iRealBackGroundOverlapCount);
    }

    for (auto& Sourceiter : m_LowestList)
    {
        _uint iLowestOverlapCount = { 0 };
        for (auto& Destiter : m_LowestList)
        {
            if (Overlap(Sourceiter, Destiter))
            {
                if (Destiter->Get_RenderPriority() < Sourceiter->Get_RenderPriority())
                {
                    ++iLowestOverlapCount;
                }
                else if (Destiter->Get_RenderPriority() == Sourceiter->Get_RenderPriority())
                {
                    if (Sourceiter->Get_Scale().x + Sourceiter->Get_Scale().y < Destiter->Get_Scale().x + Destiter->Get_Scale().y)
                    {
                        ++iLowestOverlapCount;
                    }
                }
            }
        }

        Sourceiter->Set_RenderZ(iLowestOverlapCount);
    }
}

_bool CUi_Mgr::Overlap(shared_ptr<CUi> Source, shared_ptr<CUi> Dest)
{
    _float3 SrcScale = Source->Get_Scale();
    _float3 SrcPos = Source->Get_Pos();

    _float3 DstScale = Dest->Get_Scale();
    _float3 DstPos = Dest->Get_Pos();

    _float DistanceX = fabs(SrcPos.x - DstPos.x);
    _float DistanceY = fabs(SrcPos.y - DstPos.y);

    if (DistanceX <= DstScale.x * 0.5f + SrcScale.x * 0.5f)
    {
        if (DistanceY <= DstScale.y * 0.5f + SrcScale.y * 0.5f)
        {
            return true;
        }
    }
    return false;
}

void CUi_Mgr::Free()
{
    m_UiMap.clear();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}