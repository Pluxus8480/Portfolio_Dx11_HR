#include "GimmickManager.h"
#include "GameInstance.h"
#include "GimmickBase.h"

IMPLEMENT_SINGLETON(CGimmickManager)

CGimmickManager::CGimmickManager()
{
}

CGimmickManager::~CGimmickManager()
{
    Free();
}

shared_ptr<CGimmickBase> CGimmickManager::Get_GimmckObj(string strGimmickObjID)
{
    auto iter = m_mapGimmickObjs.find(strGimmickObjID);

    if (iter == m_mapGimmickObjs.end())
        return nullptr;

    return iter->second;
}

void CGimmickManager::Add_GimmickObj(shared_ptr<CGimmickBase> pGimmickObj)
{
    string strGimmickID = pGimmickObj->Get_GimmickID();

    auto iter = m_mapGimmickObjs.find(strGimmickID);

    if (iter != m_mapGimmickObjs.end())
        return;

    m_mapGimmickObjs.insert({ strGimmickID, pGimmickObj });
}

void CGimmickManager::Clear_GimmickObj()
{
    m_mapGimmickObjs.clear();
}

void CGimmickManager::SetUp_CinematicCallBack()
{
    for (auto& Pair : m_mapGimmickObjs)
    {
        Pair.second->SetUp_CinematicCallBack();
    }
}

void CGimmickManager::Free()
{
    m_mapGimmickObjs.clear();
}
