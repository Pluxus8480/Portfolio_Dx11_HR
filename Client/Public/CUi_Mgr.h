#pragma once
#include "Client_Defines.h"
#include "CUi.h"


BEGIN(Client)

class CUi_Mgr
{
	DECLARE_SINGLETON(CUi_Mgr)

public:
	enum class Sorting { Priority, Miiddle, BackGround, RealBackGround, Lowest, End };


private:
	CUi_Mgr();

public:
	~CUi_Mgr();


public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);
	void Render();


public:
	void Add_Clone(wstring UiTag, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	shared_ptr<class CUi> Add_Clone_Return(wstring UiTag, _uint iLevelIndex, 
		const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);



public:
	void Ui_Dead(const wstring& UiTag);
	void Ui_AllActive(_bool Active = true);
	void Ui_RhythmParryActive(_bool Active = false);
	void Ui_Respawn(_uint iLevelIndex, const wstring& strLayerTag, const wstring& UiTag);
	void Set_Active(const wstring& UiTag, _bool Active);
	void LevelChange_Delete();
	void LevelChange_Create(_uint CurLevel);
	shared_ptr<class CUi> Find_UI(const wstring& wstrName);
	void Open_Store();
	void Close_Store();
	void Battle_Start();
	void Battle_End();
	void Start_CutScene();

	void AddCanvase(shared_ptr<class CUi_Canvas> InCanvas);
	void DeleteCanvase(shared_ptr<class CUi_Canvas> InCanvas);
	void NotifyUIEvent(UI_EVENT InUIEvent);
private:
	void Sorting_Ui();
	_bool Overlap(shared_ptr<class CUi> Source, shared_ptr<class CUi> Dest);


private:
	map<wstring, shared_ptr<class CUi>> m_UiMap;

	list<shared_ptr<class CUi>> m_PriorityList;
	list<shared_ptr<class CUi>> m_NormalList;
	list<shared_ptr<class CUi>> m_BackGroundList;
	list<shared_ptr<class CUi>> m_RealBackGroundList;
	list<shared_ptr<class CUi>> m_LowestList;

	list<shared_ptr<class CUi>> m_DefaultList;
	list<shared_ptr<class CUi>> m_SpriteList;
	
	LEVEL m_ePreviousLevel = {};

	list<shared_ptr<class CUi_Canvas>> m_CanvasList;
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	void Free();
};

END