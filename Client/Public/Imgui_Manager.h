#pragma once

#include "Client_Defines.h"
#include "Effect_Base.h"
#include "ObjEnum.h"
#include "UnionEffectData.h"

BEGIN(Engine)
class CGameObject;
class CShader;
class CModel;
END

BEGIN(Client)

enum class eImgui_State
{
	IMGUI_STATE_WAIT,
	IMGUI_STATE_RENDER,
	IMGUI_STATE_SHUTDOWN,
	IMGUI_STATE_END
};

enum class eMapLoadType
{
	MAP_LOAD,
	MAP_SAVE,
	MODELNAME_LOAD,
	JSON_LOAD,
	SECTION_SAVE,
	SECTION_LOAD,
	MAP_END
};

enum class eEffectLoadType
{
	EFFECT_LOAD,
	EFFECT_SAVE,
	EFFECT_END
};

enum class eToolType
{
	TOOL_MAP,
	TOOL_EFFECT,
	TOOL_SOUND,
	TOOL_ANIMATION,
	TOOL_CINE,
	TOOL_DEBUG,
	TOOL_END
};

using GIZMODESC = struct tagGizmoDesc
{
	ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::WORLD;
	ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	bool bUseSnap = false;
	bool boundSizing = false;
	bool boundSizingSnap = false;
	float snap[3] = { 0.1f, 0.1f, 0.1f };
	float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	float boundsSnap[3] = { 0.1f, 0.1f, 0.1f };
};

class CImgui_Manager
{
	DECLARE_SINGLETON(CImgui_Manager)

private:
	CImgui_Manager();

public:
	~CImgui_Manager();

private:
	struct NewMapObjData
	{
		_bool			bCollision = { true };
		_uint			iObjType;
		_char			szPrototypeTag[MAX_PATH] = {};
		_float4			Buffer = {};
		_float4x4		matWorld;
		LightDesc		tLightDesc;
	};

public:
	void	Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext);
	HRESULT Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void    PreTick();
	void    Tick();
	void	LateTick();
	void	Tick_Engine(_float fTimeDelta);

	void	Imgui_Render();

	void	Imgui_MapTool();
	void	Imgui_EffectTool();
	void	Imgui_SoundTool();
	void	Imgui_AnimationTool();
	void	Imgui_LinkTool();
	void	Imgui_CineTool();
	void	Imgui_DebugTool();

	void    Imgui_Main();

	void	Imgui_GizmoRender();
	void	Imgui_EffectOptionRender();
	void	Imgui_MenuBar();
	void	Imgui_EffectMenuBar();

	void	Imgui_ObjCreate();
	void	Imgui_EffectCreate();
	void	Imgui_EffectCopy();

private:
	void    Imgui_MapDataControl();
	void    Imgui_EffectDataControl();

	void	Load_ModelNames(wstring strFilePath);
	void	Load_FModel();
	void	Load_MapObj_Interactive(const string& strFilePath);
	//void	Level_MapObjLoad(_uint iMapNum, const string& strFilePath);

	void	Pick_Object();

	//Imgui Control Function
public:
	void			Imgui_SetState(eImgui_State eState) { m_eState = eState; }
	eImgui_State	Imgui_GetState() { return m_eState; }

	void 			Imgui_SetSelectObject(shared_ptr< CGameObject> pObject) { m_pSelectObject = pObject; }
	vector<shared_ptr<CGameObject>>* Get_Objects() { return m_vecObjects; }

	void			Set_ToolType(eToolType eToolType, _bool bCanChange) { m_eToolType = eToolType; m_bCanChange = bCanChange; }

	void			Clear_Objects();

	void			Tool_Change();

public:
	void			Make_Map();
	void			Save_Map();
	void			Load_Map();
	void			Save_SectionCulling();
	void			Load_SectionCulling();

	void			Make_EffectData(UNION_EFFECT_DATA _sUnionData);
	void			Save_EffectData();
	void			Load_EffectData();

	void			Init_Effect_Prototype();

	void			Make_Effect();
	void			Save_Effect();
	void			Load_Effect();

private:
	_bool m_bRotCheck = false;
	_float3 m_vRot = { 0.f,0.f,0.f };

private:
	eImgui_State m_eState = eImgui_State::IMGUI_STATE_WAIT;

	eMapLoadType	m_eMapLoadType		= eMapLoadType::MAP_END;
	eEffectLoadType m_eEffectLoadType	= eEffectLoadType::EFFECT_END;

	eToolType		m_eToolType		= eToolType::TOOL_END;
	
	shared_ptr<CGameObject> m_pSelectObject = nullptr;

	tagGizmoDesc m_tGizmoDesc;

	eObjectType m_eObjectType = TESTOBJECT;

	vector<shared_ptr<CGameObject>> m_vecObjects[OBJTYPE_END];

	vector<MapObjData> m_vecMapObjData[OBJTYPE_END];

	string m_strMapName = "MapData01";

	vector<string> m_vecObjectNames;

private:
	map<string,vector<shared_ptr<CEffect_Base>>> m_mEffects;
	vector<string> m_vecEffectNames;
	vector<string> m_vecPtEffectStyleNames;
	int m_strSelectEffectNum = 0;
	
	CEffect_Base::EFFECT_TYPE m_eEffectType = CEffect_Base::EFFECT_TEXTURE;
	_bool m_bCanChange = true;

	string m_strUnionEffectKey = "";

	/* Map Tool */
	vector<string> m_vecModelTags;
	vector<string> m_vecCloneNames[OBJTYPE_END];
	vector<string> m_vecObjectTags;
	vector<string> m_vecMiscTags;
	string m_strSelectObjectTag;

	_int m_iCurrModelIndex = 0;
	_int m_iCurrCloneIndex = 0;
	_int m_iCurrInteractiveIndex = 0;
	_int m_iCurrMiscIndex = 0;
	_int m_iCurrCullBoxIndex = 0;

	set<wstring> m_setPrototypeTag;

	//vector<MapObjData> m_vecMapObjIAData;
	//vector<MapObjData> m_vecMiscObjData;
	//vector<MapObjData> m_vecCullBoxData;
	_int m_iCurrGimmickIndex = 0;
	_int m_iCurrRelativeIndex = 0;

	_float m_fTransPos[3] = { 0, };

	vector<NewMapObjData> m_vecNewMapObjData[OBJTYPE_END];

	string m_strSourModelTag;
	vector<_uint> m_vecPassIndexCopy;
	vector<_float4> m_vecColorsCopy;

	string m_strSectionFileName;
	vector<shared_ptr<class CSectionCullingBox>> m_vecSectionCulling;
	_uint m_iSectionIndex;

	_float4x4 m_WorldMatCopy = { Identity };

private:
	_bool m_bIsUnionLink = false;
	_bool m_bUnionPlay = false;
	_bool m_bUnionPause = false;

	_float m_fUnionLinkTime = 0.f;
	_float m_fUnionLinkTimeMax = 0.f;
	_float m_fUnionLinkTimePercent = 0.f;

	void EffectLink();
	void EffectUnLink();
	void EffectAnchor();
	void EffectPause();
	void UnionEnd();
	void UnionStart();

private:
	ImGui::FileBrowser fileDialog;
	ImGui::FileBrowser fileDialog2;
	ImGui::FileBrowser fileDialog_for_weapon;

/* For Anim Tool */
private:
	vector<string> m_AnimList;
	_int m_iAnim_Select = -1;

	vector<string> m_BoneList;
	_int m_iBone_Select = -1;

	vector<string> m_EffectList;
	_int m_iEffect_Select = -1;

	vector<pair<string, string>> m_SoundList;
	_int m_iSound_Select = -1;

	_int m_iFrame_Select = -1;

	string m_strAnimFilePath = "";

	shared_ptr<class CFree_SK_Object> m_pFreeObj = nullptr;
	shared_ptr<class CFree_SM_Object> m_pWeaponObj = nullptr;



private:
	void MapDelete_CtrlZ();
	void BackUp_MapDelete(shared_ptr<CGameObject> pObj, string Name);
	void Initialize_BackupVec();
	void Clear_BackUpData();


private:
	vector<shared_ptr<CGameObject>> m_ObjectBackUpVec[OBJTYPE_END];
	vector<string> m_CloneNamesBackupVec[OBJTYPE_END];


public:
	void Free();
};

END
