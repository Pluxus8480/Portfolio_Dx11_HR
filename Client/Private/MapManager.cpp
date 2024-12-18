#include "MapManager.h"
#include "MapObject_Static.h"
#include "MapObject_Anim.h"
#include "GimmickBase.h"
#include "Misc_Interface.h"
#include "Light.h"
#include "Model.h"

#include "GameInstance.h"
#include "GimmickManager.h"
#include "CullingBox.h"
#include "CMagnet.h"


IMPLEMENT_SINGLETON(CMapManager)
CMapManager::CMapManager()
{
}

CMapManager::~CMapManager()
{
	Free();
}

void CMapManager::Assign_BossObject(shared_ptr<CGameObject> pBossObj)
{
	shared_ptr<CBoss_InteractObj> pObj = dynamic_pointer_cast<CBoss_InteractObj>(pBossObj);
	if (pObj == nullptr)
	{
		assert(false);
	}
	m_BossObjectList.emplace_back(pObj);
}

void CMapManager::Boss_ChangePhase(_uint iPhase)
{
	for (auto& pBossObj : m_BossObjectList)
		pBossObj->Change_Phase(iPhase);
}

void CMapManager::Boss_ChangePattern(_uint iPattern)
{
	for (auto& pBossObj : m_BossObjectList)
		pBossObj->Change_Pattern(iPattern);
}

void CMapManager::Clear_BossObjects()
{
	for (auto& pBossObject : m_BossObjectList)
		pBossObject = nullptr;
	m_BossObjectList.clear();
}

void CMapManager::Assign_Magnet(shared_ptr<CMagnet> pMagnet)
{
	m_MagnetList.emplace_back(pMagnet);
}

list<shared_ptr<CMagnet>>& CMapManager::Get_MagnetList()
{
	m_AvailableMagnet.clear();

	for (auto& pMagnet : m_MagnetList)
	{
		if (static_pointer_cast<CGimmickBase>(pMagnet)->Get_Executed())
			m_AvailableMagnet.push_back(pMagnet);
	}

	return m_AvailableMagnet;
}

void CMapManager::Add_CurrentSection(_uint iSectionIndex)
{
	auto iter = find(m_CurrentIndexList.begin(), m_CurrentIndexList.end(), iSectionIndex);
	if (iter == m_CurrentIndexList.end())
	{
		m_CurrentIndexList.push_back(iSectionIndex);

		
	}
}

void CMapManager::Remove_CurrentSection(_uint iSectionIndex)
{
	//auto iter = m_CurrentIndexList.find(iSectionIndex);
	auto iter = find(m_CurrentIndexList.begin(), m_CurrentIndexList.end(), iSectionIndex);
	if (iter == m_CurrentIndexList.end()) return;

	m_CurrentIndexList.erase(iter);
}

void CMapManager::Update()
{
	_bool bCollision = { false };
	for (size_t i = 0; i < m_vecCullingBox.size(); ++i)
	{
		if (m_vecCullingBox[i]->Get_Collision())
		{
			if (i == m_iCurBoxIndex) return;

			m_iCurBoxIndex = (_int)i;
			bCollision = true;

			m_vecRenderIndex.clear();
			if (m_vecCullingBox[i]->Is_Boarder())
			{
				m_vecRenderIndex.emplace_back(-1);
				m_vecRenderIndex.emplace_back(m_iCurBoxIndex - 1);
				m_vecRenderIndex.emplace_back(m_iCurBoxIndex);
				m_vecRenderIndex.emplace_back(m_iCurBoxIndex + 1);
			}
			else
			{
				m_vecRenderIndex.emplace_back(m_iCurBoxIndex - 1);
				m_vecRenderIndex.emplace_back(m_iCurBoxIndex);
				m_vecRenderIndex.emplace_back(m_iCurBoxIndex + 1);
			}
		}
	}

	if (!bCollision)
	{
		m_iCurBoxIndex = -1;

		m_vecRenderIndex.clear();
		m_vecRenderIndex.emplace_back(-1);
	}


	shared_ptr<CMagnet> m_pMagnet = { nullptr };
	_float LowDistance = { 100000 };
	for (auto& iter : m_MagnetList)
	{
		if (!iter->Get_UseMagnet())
		{
			iter->Set_MagnetUiActive(false);
			_float Distance = iter->Magnet_DistancePlayer();
			if (Distance <= LowDistance)
			{
				LowDistance = Distance;
				m_pMagnet = iter;
			}
		}
	}
	if (m_pMagnet)
	{
		m_pMagnet->Set_MagnetUiActive(true);
	}
}

_bool CMapManager::IsIn_RenderBoundary(_uint iObjectID)
{
	auto Pair = m_mapBoundingIndex.find(iObjectID);
	if (Pair == m_mapBoundingIndex.end())
		return true;

	if (find(m_vecRenderIndex.begin(), m_vecRenderIndex.end(), Pair->second) != m_vecRenderIndex.end())
		return true;
	
	if (find(m_vecBoarderIndex.begin(), m_vecBoarderIndex.end(), Pair->second) != m_vecBoarderIndex.end())
		return true;

	return false;
}

void CMapManager::Level_MapLoad(_uint _iMapNum, const string& strFilePath)
{
	std::ifstream is(strFilePath, std::ios::binary);

#ifdef _DEBUG
	assert(is.is_open());
#endif // _DEBUG

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	CGimmickBase::COMMONOBJ_DESC commonDesc{};
	MISCOBJ_DESC miscDesc{};

	_tchar strTextBuffer[MAX_PATH] = {};
	wstring strSMObj = CMapObject_Static::ObjID + to_wstring(m_iNextLevel);

	for (_uint e = 0; e < OBJTYPE_END; ++e)
	{
		size_t iSize = {};
		read_typed_data(is, iSize);

		for (_uint i = 0; i < iSize; i++) {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
			MapObjData data;
			read_typed_data(is, data);

			switch (data.iObjType)
			{
			case LIGHT:
				m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&data.tLightDesc));
				m_vecObjects[LIGHT].back()->Get_Transform()->Set_WorldMatrix(data.matWorld);
				break;
			case TESTOBJECT:
				m_vecObjects[TESTOBJECT].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Layer_Map"), TEXT("Prototype_GameObject_TestObject")));
				m_vecObjects[TESTOBJECT].back()->Get_Transform()->Set_WorldMatrix(data.matWorld);
				break;
			case MAPOBJ_STATIC: {
				staticDesc.fSpeedPerSec = 0.f;
				staticDesc.fRotationPerSec = 0.f;
				staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
				MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
				staticDesc.strModelTag = wstring(strTextBuffer);
				staticDesc.bCollision = data.bCollision;
				staticDesc.worldMatrix = data.matWorld;
				auto pObject = GAMEINSTANCE->Pool_Object_Return(strSMObj, &staticDesc);
				auto pCasted = dynamic_pointer_cast<CMisc_Interface>(pObject);
				if (pCasted)
					pCasted->Load_Data(is);
				m_vecObjects[MAPOBJ_STATIC].push_back(pObject);
				break;
			}
			case MAPOBJ_ANIM: {
				animDesc.fSpeedPerSec = 0.f;
				animDesc.fRotationPerSec = 0.f;
				animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
				MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
				animDesc.strModelTag = wstring(strTextBuffer);
				animDesc.worldMatrix = data.matWorld;
				auto pObject = GAMEINSTANCE->Pool_Object_Return(CMapObject_Anim::ObjID, &animDesc);
				break;
			}
			default:
				break;
			}
		}
	}

	is.close();

	string strGameObjFilePath;
	_char szGameObjFilePath[MAX_PATH] = {};
	auto ext = strFilePath.find(".bin");
	if (ext != strFilePath.npos)
	{
		strGameObjFilePath = string(strFilePath.begin(), strFilePath.begin() + ext);
		strcat_s(szGameObjFilePath, strGameObjFilePath.c_str());
		strcat_s(szGameObjFilePath, "_GameObject.bin");
	}

	Level_MapObjLoad(_iMapNum, szGameObjFilePath);
}

void CMapManager::Level_MapLoad_BySection(_uint iLevelIndex, const string& strFilePath)
{
//	std::ifstream is(strFilePath, std::ios::binary);
//
//#ifdef _DEBUG
//	assert(is.is_open());
//#endif // _DEBUG
//
//	_char szFileName[MAX_PATH] = {};
//	_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, MAX_PATH);
//
//	list<shared_ptr<CGameObject>> ObjectList;
//	m_CullingMap.insert({ szFileName, ObjectList });
//
//	CMapObject_Static::STATICOBJ_DESC staticDesc{};
//	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
//	CGimmickBase::COMMONOBJ_DESC commonDesc{};
//	MISCOBJ_DESC miscDesc{};
//
//	_tchar strTextBuffer[MAX_PATH] = {};
//	wstring strSMObj = CMapObject_Static::ObjID + to_wstring(m_iNextLevel);
//
//	for (_uint e = 0; e < OBJTYPE_END; ++e)
//	{
//		size_t iSize = {};
//		read_typed_data(is, iSize);
//
//		for (_uint i = 0; i < iSize; i++) {
//			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
//			MapObjData data;
//			read_typed_data(is, data);
//
//			switch (data.iObjType)
//			{
//			case LIGHT:
//				m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&data.tLightDesc));
//				m_vecObjects[LIGHT].back()->Get_Transform()->Set_WorldMatrix(data.matWorld);
//				break;
//			case TESTOBJECT:
//				m_vecObjects[TESTOBJECT].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Layer_Map"), TEXT("Prototype_GameObject_TestObject")));
//				m_vecObjects[TESTOBJECT].back()->Get_Transform()->Set_WorldMatrix(data.matWorld);
//				break;
//			case MAPOBJ_STATIC: {
//				staticDesc.fSpeedPerSec = 0.f;
//				staticDesc.fRotationPerSec = 0.f;
//				staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
//				MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
//				staticDesc.strModelTag = wstring(strTextBuffer);
//				staticDesc.bCollision = data.bCollision;
//				staticDesc.worldMatrix = data.matWorld;
//				auto pObject = GAMEINSTANCE->Pool_Object_Return(strSMObj, &staticDesc);
//				auto pCasted = dynamic_pointer_cast<CMisc_Interface>(pObject);
//				if (pCasted)
//					pCasted->Load_Data(is);
//				//m_vecObjects[MAPOBJ_STATIC].push_back(pObject);
//				ObjectList.push_back(pObject);
//				break;
//			}
//			case MAPOBJ_ANIM: {
//				animDesc.fSpeedPerSec = 0.f;
//				animDesc.fRotationPerSec = 0.f;
//				animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
//				MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
//				animDesc.strModelTag = wstring(strTextBuffer);
//				animDesc.worldMatrix = data.matWorld;
//				//m_vecObjects[MAPOBJ_ANIM].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Layer_Map"), CMapObject_Anim::ObjID, &animDesc));
//				auto pObject = GAMEINSTANCE->Pool_Object_Return(CMapObject_Anim::ObjID, &animDesc);
//				ObjectList.push_back(pObject);
//				break;
//			}
//			default:
//				break;
//			}
//		}
//	}
//
//	is.close();
//
//	string strGameObjFilePath;
//	_char szGameObjFilePath[MAX_PATH] = {};
//	auto ext = strFilePath.find(".bin");
//	if (ext != strFilePath.npos)
//	{
//		strGameObjFilePath = string(strFilePath.begin(), strFilePath.begin() + ext);
//		strcat_s(szGameObjFilePath, strGameObjFilePath.c_str());
//		strcat_s(szGameObjFilePath, "_GameObject.bin");
//	}
//
//	Level_MapObjLoad_BySection(iLevelIndex, szFileName, szGameObjFilePath);
}

void CMapManager::Push_MapObjects(_uint iLevelIndex, const wstring& strLayerTag)
{
	wstring strSMObj = CMapObject_Static::ObjID;

	if (iLevelIndex != LEVEL_MAPTOOL && iLevelIndex != LEVEL_EFFECTTOOL)
		strSMObj += to_wstring(iLevelIndex);

	GAMEINSTANCE->Push_ObjectsPool(strSMObj, iLevelIndex, strLayerTag);
	GAMEINSTANCE->Push_ObjectsPool(CMapObject_Anim::ObjID, iLevelIndex, strLayerTag);
	GAMEINSTANCE->Push_ObjectsPool(CCullingBox::ObjID, iLevelIndex, strLayerTag);

	for (auto strPrototypeTag : m_setPrototypeTag)
		GAMEINSTANCE->Push_ObjectsPool(strPrototypeTag, iLevelIndex, strLayerTag);

	m_setPrototypeTag.clear();
}

void CMapManager::Read_ModelNames(const wstring& strLevelDir)
{
	_float4x4 FMat = {};
	//XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	XMStoreFloat4x4(&FMat, XMMatrixIdentity());

	wstring strFilePath = strLevelDir + L"*.*";

	WIN32_FIND_DATA	fd;
	HANDLE	hFind = FindFirstFile(strFilePath.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL bContinue = true;

		do
		{
			if (!lstrcmp(L".", fd.cFileName) || !lstrcmp(L"..", fd.cFileName))
				continue;

			DWORD dwFileAttribute = fd.dwFileAttributes;

			if (dwFileAttribute == FILE_ATTRIBUTE_DIRECTORY)
			{
				wstring strChildPath = strLevelDir + fd.cFileName + L"/";

				Read_ModelNames(strChildPath);
			}
			else if (dwFileAttribute == FILE_ATTRIBUTE_ARCHIVE)
			{
				wstring strFileName = fd.cFileName;

				_tchar szFullPath[MAX_PATH] = {};

				_tchar szFileName[MAX_PATH] = {};
				_tchar szExt[MAX_PATH] = {};

				_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

				if (lstrcmp(L".FBX", szExt) && lstrcmp(L".fbx", szExt))
					continue;

				wcscpy_s(szFullPath, strLevelDir.c_str());
				wcscat_s(szFullPath, strFileName.c_str());

				size_t index = strLevelDir.find_last_of(L'/');
				wstring strSectionName = strLevelDir.substr(0, index);
				index = strSectionName.find_last_of(L'/');
				strSectionName = strSectionName.substr(index + 1, strSectionName.length());

				wstring strPrototypeTag = strSectionName;

				//m_List_ResourceInfo.push_back(
				//	make_shared<RES_MODEL>(szFileName, wstring(szFullPath), MODEL_TYPE::NONANIM, FMat)
				//);

				auto iter = m_ModelNameMap.find(szFileName);
				if (iter == m_ModelNameMap.end())
					m_ModelNameMap.insert({ szFileName, wstring(szFullPath) });


			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}

HRESULT CMapManager::Add_SMModelPrototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex, const wstring& strModelTag)
{
	if (m_iNextLevel == LEVEL_MAPTOOL || m_iNextLevel == LEVEL_EFFECTTOOL || m_iNextLevel == LEVEL_CINETOOL /*|| m_iNextLevel == LEVEL_GIMMICTEST*/
		|| m_iNextLevel == LEVEL_UITOOL)
		return S_OK;

	auto iter = m_ModelNameMap.find(strModelTag);

	if (iter == m_ModelNameMap.end())
		return E_FAIL;
	
	auto pGameInstance = CGameInstance::Get_Instance();

	auto pComp = pGameInstance->Get_Component_Prototype(iLevelIndex, strModelTag);
	if (pComp)
		return S_OK;

	string strFullPath = string(iter->second.begin(), iter->second.end());
	_matrix PreMat = XMMatrixIdentity();
	
	return pGameInstance->Add_Prototype(iLevelIndex, strModelTag, CModel::Create(pDevice, pContext, MODEL_TYPE::NONANIM, strFullPath.c_str(), PreMat));
}

void CMapManager::Clear_Containers()
{
	m_ModelNameMap.clear();
}

void CMapManager::Adjust_MagnetUi()
{
}

void CMapManager::Level_MapMake(_uint iMapNum)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	CGimmickBase::COMMONOBJ_DESC commonDesc{};
	MISCOBJ_DESC miscDesc{};
	_tchar strTextBuffer[MAX_PATH] = {};

	wstring strSMObj = CMapObject_Static::ObjID + to_wstring(m_iNextLevel);

	for (_uint i = 0; i < m_vecMapObjData.size(); i++) {
		switch (m_vecMapObjData[i].iObjType)
		{
		case LIGHT: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = GAMEINSTANCE->Add_Light_Return(&m_vecMapObjData[i].tLightDesc);
			obj->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		}
		case TESTOBJECT: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_TestObject"), &TransformDesc);
			obj->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		}
		case MAPOBJ_STATIC: {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
			staticDesc.fSpeedPerSec = 0.f;
			staticDesc.fRotationPerSec = 0.f;
			staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			staticDesc.strModelTag = wstring(strTextBuffer);
			staticDesc.bCollision = m_vecMapObjData[i].bCollision;
			staticDesc.worldMatrix = m_vecMapObjData[i].matWorld;
			auto pObject = GAMEINSTANCE->Pool_Object_Return(strSMObj, &staticDesc);
			m_vecObjects[MAPOBJ_STATIC].emplace_back(pObject);
			break;
		}
		case MAPOBJ_ANIM: {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
			animDesc.fSpeedPerSec = 0.f;
			animDesc.fRotationPerSec = 0.f;
			animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
			MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[i].szPrototypeTag, (_int)strlen(m_vecMapObjData[i].szPrototypeTag), strTextBuffer, MAX_PATH);
			animDesc.strModelTag = wstring(strTextBuffer);
			animDesc.worldMatrix = m_vecMapObjData[i].matWorld;
			auto pObject = GAMEINSTANCE->Pool_Object_Return(CMapObject_Anim::ObjID, &animDesc);
			m_vecObjects[MAPOBJ_ANIM].emplace_back(pObject);
			break;
		}
		default:
			break;
		}
	}

	m_vecMapObjData.clear();
}

void CMapManager::Level_MapObjLoad(_uint iMapNum, const string& strFilePath)
{
	std::ifstream gameObjIs(strFilePath, std::ios::binary);

	vector<shared_ptr<CGimmickBase>> vecObjects;

	if (gameObjIs.is_open())
	{
		size_t gimmickSize = {};
		read_typed_data(gameObjIs, gimmickSize);

		for (size_t i = 0; i < gimmickSize; i++)
		{
			_tchar strTextBuffer[MAX_PATH] = {};
			CGimmickBase::COMMONOBJ_DESC commonDesc{};

			MapObjData data;
			read_typed_data(gameObjIs, data);
			commonDesc.worldMatrix = data.matWorld;

			MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
			shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(wstring(strTextBuffer), &commonDesc);
			m_vecObjects[MAPOBJ_INTERACTIVE].emplace_back(pGameObject);
			shared_ptr<CGimmickBase> pCasted = dynamic_pointer_cast<CGimmickBase>(pGameObject);
			pCasted->Load_Data(gameObjIs);
			m_setPrototypeTag.emplace(wstring(strTextBuffer));
			CGimmickManager::Get_Instance()->Add_GimmickObj(pCasted);
			vecObjects.emplace_back(pCasted);
		}

		size_t miscSize = {};
		read_typed_data(gameObjIs, miscSize);

		for (size_t i = 0; i < miscSize; i++)
		{
			_tchar strTextBuffer[MAX_PATH] = {};
			MISCOBJ_DESC miscDesc{};

			MapObjData data;
			read_typed_data(gameObjIs, data);
			miscDesc.worldMatrix = data.matWorld;

			MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
			shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(wstring(strTextBuffer), &miscDesc);
			m_vecObjects[MAPOBJ_MISC].emplace_back(pGameObject);
			shared_ptr<CMisc_Interface> pCasted = dynamic_pointer_cast<CMisc_Interface>(pGameObject);
			if(pCasted)
				pCasted->Load_Data(gameObjIs);
			m_setPrototypeTag.emplace(wstring(strTextBuffer));
		}

		if (gameObjIs.peek() != EOF)
		{
			size_t boxSize = {};
			read_typed_data(gameObjIs, boxSize);

			for (size_t i = 0; i < boxSize; i++)
			{
				CCullingBox::CULLING_DESC cullBoxDesc{};

				MapObjData data;
				read_typed_data(gameObjIs, data);
				cullBoxDesc.worldMat = data.matWorld;
				cullBoxDesc.bBoarder = data.bCollision;

				shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(CCullingBox::ObjID, &cullBoxDesc);
				shared_ptr<CCullingBox> pCasted = static_pointer_cast<CCullingBox>(pGameObject);
				m_vecCullingBox.emplace_back(pCasted);

				if (pCasted->Is_Boarder())
					m_vecBoarderIndex.emplace_back((_int)m_vecCullingBox.size() - 1);
			}
		}
	}
	else
		gameObjIs.close();

	for (auto pObject : vecObjects)
		pObject->Link_Relatives();

	SetUp_BoundingIndex();
}

void CMapManager::Level_MapObjLoad_BySection(_uint iMapNum, const string& strFileTag, const string& strFilePath)
{
	//std::ifstream gameObjIs(strFilePath, std::ios::binary);
	//
	//vector<shared_ptr<CGimmickBase>> vecObjects;
	//list<shared_ptr<CGameObject>> ObjectList = m_CullingMap.find(strFilePath)->second;
	//
	//if (gameObjIs.is_open())
	//{
	//	size_t gimmickSize = {};
	//	read_typed_data(gameObjIs, gimmickSize);
	//
	//	for (size_t i = 0; i < gimmickSize; i++)
	//	{
	//		_tchar strTextBuffer[MAX_PATH] = {};
	//		CGimmickBase::COMMONOBJ_DESC commonDesc{};
	//
	//		MapObjData data;
	//		read_typed_data(gameObjIs, data);
	//		commonDesc.worldMatrix = data.matWorld;
	//
	//		MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
	//		shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(wstring(strTextBuffer), &commonDesc);
	//		//m_vecObjects[MAPOBJ_INTERACTIVE].emplace_back(pGameObject);
	//		ObjectList.emplace_back(pGameObject);
	//		shared_ptr<CGimmickBase> pCasted = dynamic_pointer_cast<CGimmickBase>(pGameObject);
	//		pCasted->Load_Data(gameObjIs);
	//		m_setPrototypeTag.emplace(wstring(strTextBuffer));
	//		CGimmickManager::Get_Instance()->Add_GimmickObj(pCasted);
	//		vecObjects.emplace_back(pCasted);
	//	}
	//
	//	size_t miscSize = {};
	//	read_typed_data(gameObjIs, miscSize);
	//
	//	for (size_t i = 0; i < miscSize; i++)
	//	{
	//		_tchar strTextBuffer[MAX_PATH] = {};
	//		MISCOBJ_DESC miscDesc{};
	//
	//		MapObjData data;
	//		read_typed_data(gameObjIs, data);
	//		miscDesc.worldMatrix = data.matWorld;
	//
	//		MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
	//		shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(wstring(strTextBuffer), &miscDesc);
	//		//m_vecObjects[MAPOBJ_MISC].emplace_back(pGameObject);
	//		ObjectList.emplace_back(pGameObject);
	//		shared_ptr<CMisc_Interface> pCasted = dynamic_pointer_cast<CMisc_Interface>(pGameObject);
	//		if (pCasted)
	//			pCasted->Load_Data(gameObjIs);
	//		m_setPrototypeTag.emplace(wstring(strTextBuffer));
	//	}
	//
	//	if (gameObjIs.peek() != EOF)
	//	{
	//		size_t boxSize = {};
	//		read_typed_data(gameObjIs, boxSize);
	//
	//		for (size_t i = 0; i < boxSize; i++)
	//		{
	//			CCullingBox::CULLING_DESC cullBoxDesc{};
	//
	//			MapObjData data;
	//			read_typed_data(gameObjIs, data);
	//			cullBoxDesc.worldMat = data.matWorld;
	//			cullBoxDesc.bBoarder = data.bCollision;
	//
	//			shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(CCullingBox::ObjID, &cullBoxDesc);
	//			shared_ptr<CCullingBox> pCasted = static_pointer_cast<CCullingBox>(pGameObject);
	//			m_vecCullingBox.emplace_back(pCasted);
	//
	//		}
	//	}
	//}
	//else
	//	gameObjIs.close();
	//
	//for (auto pObject : vecObjects)
	//	pObject->Link_Relatives();
}

void CMapManager::SetUp_BoundingIndex()
{
	for (size_t i = MAPOBJ_STATIC; i <= MAPOBJ_MISC; ++i)
	{
		for (size_t j = 0; j < m_vecObjects[i].size(); ++j)
		{
			_vector vObjectPos = m_vecObjects[i][j]->Get_Transform()->Get_Position();

			_bool bInBounding = { false };
			for (_int m = 0; m < m_vecCullingBox.size(); ++m)
			{
				if (m_vecCullingBox[m]->IsIn_Bounding(vObjectPos))
				{
					m_mapBoundingIndex.insert({ m_vecObjects[i][j]->Get_ObjectID(), m });
					bInBounding = true;
					break;
				}
			}

			if(!bInBounding)
				m_mapBoundingIndex.insert({ m_vecObjects[i][j]->Get_ObjectID(), -1 });

			m_vecObjects[i][j] = nullptr;
		}

		m_vecObjects[i].clear();
	}
}

void CMapManager::Free()
{
}
