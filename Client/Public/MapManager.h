#pragma once
#include "Client_Defines.h"
#include "ObjEnum.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CMapManager
{
	DECLARE_SINGLETON(CMapManager)

private:
	CMapManager();
public:
	~CMapManager();

public:
	_uint Get_NextLevel() const { return m_iNextLevel; }
	void Set_NextLevel(_uint iNextLevel) { m_iNextLevel = iNextLevel; }

	/* Boss Interaction */
	void Assign_BossObject(shared_ptr<CGameObject> pBossObj);
	void Boss_ChangePhase(_uint iPhase);
	void Boss_ChangePattern(_uint iPattern);
	void Clear_BossObjects();

	/* Wire Magnet */
	void Assign_Magnet(shared_ptr<class CMagnet> pMagnet);
	list<shared_ptr<class CMagnet>>& Get_MagnetList();

	/* Section Culling */
	void Add_CurrentSection(_uint iSectionIndex);
	void Remove_CurrentSection(_uint iSectionIndex);

public:
	void Update();
	_bool IsIn_RenderBoundary(_uint iObjectID);

	void Level_MapLoad(_uint _iMapNum, const string& strFilePath);
	void Level_MapLoad_BySection(_uint iLevelIndex, const string& strFilePath);

	void Push_MapObjects(_uint iLevelIndex, const wstring& strLayerTag);

	void Read_ModelNames(const wstring& strLevelDir);
	HRESULT Add_SMModelPrototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex, const wstring& strModelTag);

	void Clear_Containers();

	void Adjust_MagnetUi();

private:
	vector<shared_ptr<CGameObject>> m_vecObjects[OBJTYPE_END];
	vector<MapObjData> m_vecMapObjData;
	set<wstring> m_setPrototypeTag;

	map<wstring, wstring> m_ModelNameMap;

	/* Culling Box */
	_int m_iCurBoxIndex = { -1 };
	vector<_int> m_vecRenderIndex;
	vector<_int> m_vecBoarderIndex;
	vector<shared_ptr<class CCullingBox>> m_vecCullingBox;
	map<_uint, _int> m_mapBoundingIndex;

	/* Boss Interation */
	list<shared_ptr<CBoss_InteractObj>> m_BossObjectList;

	/* Wire Magnet */
	list<shared_ptr<class CMagnet>> m_MagnetList;
	list<shared_ptr<class CMagnet>> m_AvailableMagnet;

	/* Section Culling */
	map<string, list<shared_ptr<CGameObject>>> m_CullingMap;
	list<_uint> m_CurrentIndexList;
	set<_uint> m_RenderIndexSet;

private:
	_uint m_iNextLevel = { LEVEL_END };

private:
	void Level_MapMake(_uint iMapNum);
	void Level_MapObjLoad(_uint iMapNum, const string& strFilePath);
	void Level_MapObjLoad_BySection(_uint iLevelIndex, const string& strFileTag, const string& strFilePath);

	void SetUp_BoundingIndex();

public:
	void Free();
};

END