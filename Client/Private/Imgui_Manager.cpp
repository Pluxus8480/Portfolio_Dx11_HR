#include "Imgui_Manager.h"
#include "Light.h"
#include "GameInstance.h"
#include "Effect_Base.h"
#include "Load_Include.h"

#include "MapObject_Static.h"
#include "CullingBox.h"
#include "GimmickManager.h"
#include "Effect_Manager.h"

#include "GameManager.h"

#include "Model.h"
#include "Animation.h"

#include "Free_SK_Object.h"
#include "Free_SM_Object.h"

#include "UnionEffectData.h"

#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

extern IMGUI_IMPL_API bool ImGui_ImplDX11_Init(ID3D11Device* device,
	ID3D11DeviceContext* device_context);

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{

}

CImgui_Manager::~CImgui_Manager()
{
	Free();
}


void CImgui_Manager::Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Device, pContext);

	ImGui::StyleColorsDark();

	m_vecObjectNames.emplace_back("Light");
	m_vecObjectNames.emplace_back("TestObject");
	m_vecObjectNames.emplace_back("StaticObject");

	m_vecObjectTags.emplace_back("CTalkTrigger");
	m_vecObjectTags.emplace_back("CProdDeptDoor");
	m_vecObjectTags.emplace_back("CBulbTrigger");
	m_vecObjectTags.emplace_back("CMovingWall");
	m_vecObjectTags.emplace_back("CElevator");
	m_vecObjectTags.emplace_back("CCommonSwitch");
	m_vecObjectTags.emplace_back("CSwitchFence");
	m_vecObjectTags.emplace_back("CSpawnTrigger_Monster");
	m_vecObjectTags.emplace_back("CMonsterSpawner");
	m_vecObjectTags.emplace_back("CButton_Platform");
	m_vecObjectTags.emplace_back("CSpawnTrigger_Platform");
	m_vecObjectTags.emplace_back("CCatwalk_Bridge");
	m_vecObjectTags.emplace_back("CFloorSwitch");
	m_vecObjectTags.emplace_back("CSmoke_Cover");
	m_vecObjectTags.emplace_back("CConveyorBelt");
	m_vecObjectTags.emplace_back("CLaserCircuit");
	m_vecObjectTags.emplace_back("CLaserPillar");
	m_vecObjectTags.emplace_back("CLaserBoard");
	m_vecObjectTags.emplace_back("CLaserReward");
	m_vecObjectTags.emplace_back("CReverbGaugeRoom");
	m_vecObjectTags.emplace_back("CWoodenBox");
	m_vecObjectTags.emplace_back("CPickUpItem");
	m_vecObjectTags.emplace_back("CInvisibleWall");
	m_vecObjectTags.emplace_back("CFirstBattleWall");
	m_vecObjectTags.emplace_back("CCameraController");
	m_vecObjectTags.emplace_back("CInvisibleCylinderWall");
	m_vecObjectTags.emplace_back("CTrulyInvisibleWall");
	
	m_vecObjectTags.emplace_back("CFloorSwitch");
	m_vecObjectTags.emplace_back("CComboSwitch");
	m_vecObjectTags.emplace_back("CLargeElevator");
	m_vecObjectTags.emplace_back("CSlate_Platform");
	m_vecObjectTags.emplace_back("CGearWall");
	m_vecObjectTags.emplace_back("CHanging_Platform");
	m_vecObjectTags.emplace_back("CQACenterDoor");
	m_vecObjectTags.emplace_back("CFadeElevator");
	m_vecObjectTags.emplace_back("CEnemyGate");
	m_vecObjectTags.emplace_back("CGenerator");
	m_vecObjectTags.emplace_back("CEventTrigger");
	m_vecObjectTags.emplace_back("CSignBoard_Platform");
	m_vecObjectTags.emplace_back("CBattleAreaDoor_PD");
	m_vecObjectTags.emplace_back("CGaraElevator");
	m_vecObjectTags.emplace_back("CLevelUp_Machine");
	m_vecObjectTags.emplace_back("CWireMagnet");
	m_vecObjectTags.emplace_back("CRekkaRoomDoor");
	m_vecObjectTags.emplace_back("CZipLine_Rail");
	m_vecObjectTags.emplace_back("CMovingFloor");
	m_vecObjectTags.emplace_back("CSteelFrame_Platform");
	m_vecObjectTags.emplace_back("CKrsc_Generator");
	m_vecObjectTags.emplace_back("CGiantRobot");
	m_vecObjectTags.emplace_back("CGiantRobot2");
	m_vecObjectTags.emplace_back("CGiantRobot3");
	m_vecObjectTags.emplace_back("CGiantRobot_Arm");
	m_vecObjectTags.emplace_back("CRhythmJump");
	m_vecObjectTags.emplace_back("CPeppermintContainer");
	m_vecObjectTags.emplace_back("CMacaronCrane");
	m_vecObjectTags.emplace_back("CKale_Antenna");
	m_vecObjectTags.emplace_back("CKaleElevatorGenerator");
	m_vecObjectTags.emplace_back("CContainerBreakPannel");
	m_vecObjectTags.emplace_back("CRobotPunchBreakWall");
	m_vecObjectTags.emplace_back("CRobotPunchStaticWall");
	m_vecObjectTags.emplace_back("CSpectraMeshGate");
	m_vecObjectTags.emplace_back("CBuiltIn_Magnet");
	m_vecObjectTags.emplace_back("CRobotBeamContainer");
	m_vecObjectTags.emplace_back("CScroll2DContainer");
	m_vecObjectTags.emplace_back("CTriggerBox");
	m_vecObjectTags.emplace_back("CRailContainer");
	m_vecObjectTags.emplace_back("CBattle3rdAfter");
	m_vecObjectTags.emplace_back("CBattle3rdFloor");
	m_vecObjectTags.emplace_back("CKaleBoss_CineObj");

	m_vecObjectTags.emplace_back("CTriggerBox_Level");
	
	m_vecObjectTags.emplace_back("CCraneMagnet");

	m_vecObjectTags.emplace_back("CCineTrigger");

	m_vecMiscTags.emplace_back("CRekka_Logo");
	m_vecMiscTags.emplace_back("CFloorSign");
	m_vecMiscTags.emplace_back("CFan_Platform");
	m_vecMiscTags.emplace_back("CSignBoard");
	m_vecMiscTags.emplace_back("CSignPlane");
	m_vecMiscTags.emplace_back("CKrscRoomWall");
	m_vecMiscTags.emplace_back("CRhythmWall");
	m_vecMiscTags.emplace_back("CDMGConsole");
	m_vecMiscTags.emplace_back("CRekka_GenCover");
	m_vecMiscTags.emplace_back("CRekka_Monitor_S");
	m_vecMiscTags.emplace_back("CRekka_Monitor_L");
	m_vecMiscTags.emplace_back("CRekka_PhaseText");
	m_vecMiscTags.emplace_back("CRekka_MonitorBase");
	m_vecMiscTags.emplace_back("CRekka_LightShaft");
	m_vecMiscTags.emplace_back("CLight_Controller");
	m_vecMiscTags.emplace_back("CEmissiveArrow");
	m_vecMiscTags.emplace_back("CMisc_Tower_Elec");
	m_vecMiscTags.emplace_back("CMimosa_Floor");
	m_vecMiscTags.emplace_back("CMusician_Robot");
	m_vecMiscTags.emplace_back("CMimosaWall");
	m_vecMiscTags.emplace_back("CMimosa_RingText");
	m_vecMiscTags.emplace_back("CMimosa_BackLEDText");
	m_vecMiscTags.emplace_back("CMimosa_RoofLEDText");
	m_vecMiscTags.emplace_back("CMimosa_Visualizer");
	m_vecMiscTags.emplace_back("CMimosa_BackMonitor");
	m_vecMiscTags.emplace_back("CMimosa_Speaker");
	m_vecMiscTags.emplace_back("CMimosa_LightShaft");
	m_vecMiscTags.emplace_back("CSpectra_FanMachine");
	m_vecMiscTags.emplace_back("CFree_SignBoard");
	m_vecMiscTags.emplace_back("CRhythm_RobotArm");
	m_vecMiscTags.emplace_back("CRhythm_CleanSystem");
	m_vecMiscTags.emplace_back("CGrass");
	
	m_vecPtEffectStyleNames.emplace_back("STYLE_NONE");
	m_vecPtEffectStyleNames.emplace_back("STYLE_SPREAD");
	m_vecPtEffectStyleNames.emplace_back("STYLE_DROP");
	m_vecPtEffectStyleNames.emplace_back("STYLE_FOUNTAIN");
	m_vecPtEffectStyleNames.emplace_back("STYLE_RISING");
	m_vecPtEffectStyleNames.emplace_back("STYLE_CYCLE");
	m_vecPtEffectStyleNames.emplace_back("STYLE_ORBITAL");
	//m_vecPtEffectStyleNames.emplace_back("STYLE_END");

	fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	fileDialog2 = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	fileDialog_for_weapon = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);

	Initialize_BackupVec();
}

HRESULT CImgui_Manager::Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	return S_OK;
}

HRESULT CImgui_Manager::Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ImGui_ImplDX11_Init(pDevice, pContext);

	return S_OK;
}

void CImgui_Manager::PreTick()
{
	if (GAMEINSTANCE->Get_KeyDown(DIK_P)) {

		if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::TRANSLATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::ROTATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::SCALE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}

	if (GAMEINSTANCE->Get_KeyDown(DIK_M)) {
		if (m_tGizmoDesc.CurrentGizmoMode == ImGuizmo::WORLD)
			m_tGizmoDesc.CurrentGizmoMode = ImGuizmo::LOCAL;
		else if (m_tGizmoDesc.CurrentGizmoMode == ImGuizmo::LOCAL)
			m_tGizmoDesc.CurrentGizmoMode = ImGuizmo::WORLD;
	}
}

void CImgui_Manager::Tick()
{
	Tool_Change();

	if (m_eToolType == eToolType::TOOL_MAP) {
		if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
			Pick_Object();
	}

	if (m_bIsUnionLink && m_bUnionPlay && m_bUnionPause == false) {
		m_fUnionLinkTime += GAMEINSTANCE->Get_TimeDelta(TEXT("Timer_60"));
		if (m_fUnionLinkTime > m_fUnionLinkTimeMax) {
			m_bUnionPlay = false;
			m_bUnionPause = false;
			m_fUnionLinkTime = 0.f;
			UnionEnd();
		}
	}

	if (GAMEINSTANCE->Get_KeyDown(DIK_F7))
	{
		MapDelete_CtrlZ();
	}
	//if (GAMEINSTANCE->Get_KeyDown(DIK_F11))
	//{
	//	Clear_BackUpData();
	//}
}

void CImgui_Manager::LateTick()
{
}

void CImgui_Manager::Tick_Engine(_float fTimeDelta)
{
	if (Imgui_GetState() == eImgui_State::IMGUI_STATE_SHUTDOWN) return;

	PreTick();
	Tick();
	LateTick();
}

void CImgui_Manager::Imgui_Render()
{
	if (m_eState != eImgui_State::IMGUI_STATE_RENDER) return;

	switch (m_eToolType)
	{
	case eToolType::TOOL_MAP:
		Imgui_MapTool();
		break;
	case eToolType::TOOL_EFFECT:
		Imgui_LinkTool();
		break;
	case eToolType::TOOL_SOUND:
		Imgui_SoundTool();
		break;
	case eToolType::TOOL_ANIMATION:
		Imgui_AnimationTool();
		break;
	case eToolType::TOOL_CINE:
		Imgui_CineTool();
		break;
	case eToolType::TOOL_END:
		break;
	default:
		break;
	}
}

void CImgui_Manager::Imgui_MapTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("MapData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_MenuBar();

		ImGui::InputText("Data File Name", m_strMapName.data(), sizeof(_tchar) * MAX_PATH);

		ImGui::SeparatorText("Camera Position");
		_float4 vCamPos = GAMEINSTANCE->Get_CamPosition_Float4();
		_float fPosition[3] = { vCamPos.x, vCamPos.y, vCamPos.z };
		if (ImGui::DragFloat3("##", fPosition))
		{
			auto pCam = GAMEINSTANCE->Get_MainCamera();
			pCam->Get_Transform()->Set_Position(XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1));
		}

		if (ImGui::TreeNode("Translate"))
		{
			ImGui::InputFloat3("Trans Pos", m_fTransPos);
			_matrix TransMat = XMMatrixTranslation(m_fTransPos[0], m_fTransPos[1], m_fTransPos[2]);
			if (ImGui::Button("Translate"))
			{
				for (_uint i = 0; i < OBJTYPE_END; ++i)
				{
					for (_uint j = 0; j < m_vecObjects[i].size(); ++j)
					{
						_matrix PrevMat = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix_XMMat();
						m_vecObjects[i][j]->Get_Transform()->Set_WorldMatrix(PrevMat * TransMat);
					}
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::BeginTabBar("##", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Static"))
			{
				m_eObjectType = MAPOBJ_STATIC;

				if (ImGui::BeginListBox("Model List"))
				{
					for (_uint i = 0; i < m_vecModelTags.size(); i++)
					{
						_bool bIsSelected = (m_iCurrModelIndex == i);
						if (ImGui::Selectable(m_vecModelTags[i].c_str(), bIsSelected))
							m_iCurrModelIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(7, 35.5));
				if (ImGui::Button("Create", ImVec2(107, 19))) {
					Imgui_ObjCreate();
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[MAPOBJ_STATIC].size(); i++)
					{
						_bool bIsSelected = (m_iCurrCloneIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[MAPOBJ_STATIC][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrCloneIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[MAPOBJ_STATIC][m_iCurrCloneIndex]);
							
							if (io.WantCaptureMouse == false && io.MouseDown[0])
								ImGui::SetScrollHereY(0.5f);
						}
					}

					ImGui::EndListBox();
				}

				if (m_vecObjects[MAPOBJ_STATIC].size() > 0)
				{
					/* Collision */
					_bool bCollision = static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC][m_iCurrCloneIndex])->Get_Collision();
					if (ImGui::Checkbox("Collision", &bCollision))
						static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC][m_iCurrCloneIndex])->Set_Collision(bCollision);

					ImGui::SameLine();
					if (ImGui::Button("Apply All"))
					{
						for (_uint i = 0; i < m_vecObjects[MAPOBJ_STATIC].size(); ++i)
							static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC][i])->Set_Collision(bCollision);
					}

					/* Material */
					dynamic_pointer_cast<CMisc_Interface>(m_vecObjects[MAPOBJ_STATIC][m_iCurrCloneIndex])->Edit_Attribute();

					if (ImGui::Button("Copy Material"))
					{
						m_strSourModelTag = m_vecCloneNames[MAPOBJ_STATIC][m_iCurrCloneIndex];
						static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC][m_iCurrCloneIndex])->Get_MaterialSet(m_vecPassIndexCopy, m_vecColorsCopy);
					}

					ImGui::SameLine();
					if (ImGui::Button("Paste Material"))
					{
						if (m_vecCloneNames[MAPOBJ_STATIC][m_iCurrCloneIndex] == m_strSourModelTag)
							static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC][m_iCurrCloneIndex])->Set_MaterialSet(m_vecPassIndexCopy, m_vecColorsCopy);
					}

					ImGui::SameLine();
					if (ImGui::Button("Paste to All"))
					{
						for (size_t i = 0; i < m_vecCloneNames[MAPOBJ_STATIC].size(); ++i)
						{
							if (m_vecCloneNames[MAPOBJ_STATIC][i] == m_strSourModelTag)
								static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC][i])->Set_MaterialSet(m_vecPassIndexCopy, m_vecColorsCopy);
						}
					}
				}

				//ImGui::SetCursorPos(ImVec2(227, 35.5));
				if (ImGui::Button("Delete", ImVec2(107, 19)) || CGameInstance::Get_Instance()->Get_KeyDown(DIK_DELETE))
				{				 
					if (m_pSelectObject != nullptr)
					{
						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject)
							{
								BackUp_MapDelete(m_vecObjects[m_eObjectType][i], m_vecCloneNames[m_eObjectType][i]);
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
						m_iCurrCloneIndex = max(m_iCurrCloneIndex - 1, 0);

						/*m_pSelectObject->Dead();

						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[MAPOBJ_STATIC].erase(m_vecCloneNames[MAPOBJ_STATIC].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
						m_iCurrCloneIndex = max(m_iCurrCloneIndex - 1, 0);*/
					}
				}

				if (ImGui::Button("Clear", ImVec2(107, 19)))
				{
					system("cls");
					Clear_Objects();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				if (m_eObjectType != MAPOBJ_MISC)
				{
					m_eObjectType = MAPOBJ_MISC;
					m_iCurrCloneIndex = 0;
				}

				if (ImGui::BeginListBox("Object List"))
				{
					for (_uint i = 0; i < m_vecMiscTags.size(); i++)
					{
						_bool bIsSelected = (m_iCurrMiscIndex == i);
						if (ImGui::Selectable(m_vecMiscTags[i].c_str(), bIsSelected))
							m_iCurrMiscIndex = i;

						if (bIsSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(7, 35.5));
				if (ImGui::Button("Create", ImVec2(107, 19))) {
					Imgui_ObjCreate();
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); i++)
					{
						_bool bIsSelected = (m_iCurrCloneIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[m_eObjectType][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrCloneIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[m_eObjectType][m_iCurrCloneIndex]);

							if (io.WantCaptureMouse == false && io.MouseDown[0])
								ImGui::SetScrollHereY(0.5f);
						}
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(227, 35.5));
				if (ImGui::Button("Delete", ImVec2(107, 19))) {
					if (m_pSelectObject != nullptr) 
					{
						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject)
							{
								BackUp_MapDelete(m_vecObjects[m_eObjectType][i], m_vecCloneNames[m_eObjectType][i]);
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
						m_iCurrCloneIndex = max(m_iCurrCloneIndex - 1, 0);
						//m_pSelectObject->Dead();

						//for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
						//	if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
						//		m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
						//		m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
						//		break;
						//	}
						//}
						//m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
					}
				}

				if (ImGui::Button("Clear", ImVec2(107, 19))) {
					Clear_Objects();
				}

				if (m_pSelectObject != nullptr)
				{
					ImGui::SeparatorText("Edit Attribute");
					shared_ptr<CMisc_Interface> pCastedObject = dynamic_pointer_cast<CMisc_Interface>(m_pSelectObject);

					if (pCastedObject)
					{
						pCastedObject->Edit_Attribute();
					}
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Animated"))
			{
				m_eObjectType = MAPOBJ_ANIM;

				if (ImGui::BeginListBox("Model List"))
				{
					for (_uint i = 0; i < m_vecModelTags.size(); i++)
					{
						_bool bIsSelected = (m_iCurrModelIndex == i);
						if (ImGui::Selectable(m_vecModelTags[i].c_str(), bIsSelected))
							m_iCurrModelIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(7, 35.5));
				if (ImGui::Button("Create", ImVec2(107, 19))) {
					Imgui_ObjCreate();
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[MAPOBJ_ANIM].size(); i++)
					{
						_bool bIsSelected = (m_iCurrCloneIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[m_eObjectType][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrCloneIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[MAPOBJ_ANIM][m_iCurrCloneIndex]);

							if (io.WantCaptureMouse == false && io.MouseDown[0])
								ImGui::SetScrollHereY(0.5f);
						}
					}

					ImGui::EndListBox();
				}

				//ImGui::SetCursorPos(ImVec2(227, 35.5));
				if (ImGui::Button("Delete", ImVec2(107, 19))) {
					if (m_pSelectObject != nullptr)
					{
						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i)
						{
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject)
							{
								BackUp_MapDelete(m_vecObjects[m_eObjectType][i], m_vecCloneNames[m_eObjectType][i]);
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
						m_iCurrCloneIndex = max(m_iCurrCloneIndex - 1, 0);
						//m_pSelectObject->Dead();

						//for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
						//	if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
						//		m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
						//		m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
						//		break;
						//	}
						//}
						//m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
					}
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Interactive"))
			{
				m_eObjectType = MAPOBJ_INTERACTIVE;

				if (m_strSelectObjectTag.size() == 0)
					m_strSelectObjectTag = m_vecObjectTags[0];

				if (ImGui::BeginCombo("GameObject", m_strSelectObjectTag.data(), ImGuiComboFlags_None))
				{
					for (int n = 0; n < m_vecObjectTags.size(); n++)
					{
						if (ImGui::Selectable(m_vecObjectTags[n].data(), n == m_eObjectType))
							m_strSelectObjectTag = m_vecObjectTags[n];
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Create", ImVec2(214, 19))) {
					Imgui_ObjCreate();
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[MAPOBJ_INTERACTIVE].size(); i++)
					{
						string strIndex = "_" + to_string(iCloneIndex);
						_char szGimmickID[MAX_PATH] = {};
						strcat_s(szGimmickID, m_strMapName.data());
						strcat_s(szGimmickID, strIndex.data());
						//string GimmickID = m_strMapName + to_string(iCloneIndex);
						dynamic_pointer_cast<CGimmickBase>(m_vecObjects[MAPOBJ_INTERACTIVE][i])->Set_GimmickID(szGimmickID);

						_bool bIsSelected = (m_iCurrInteractiveIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[m_eObjectType][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrInteractiveIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[MAPOBJ_INTERACTIVE][m_iCurrInteractiveIndex]);

							if (io.WantCaptureMouse == false && io.MouseDown[0])
								ImGui::SetScrollHereY(0.5f);
						}
					}

					ImGui::EndListBox();
				}

				if (ImGui::Button("Delete", ImVec2(214, 19))) {
					if (m_pSelectObject != nullptr) 
					{
						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i)
						{
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject)
							{
								BackUp_MapDelete(m_vecObjects[m_eObjectType][i], m_vecCloneNames[m_eObjectType][i]);
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;
							}
						}
						//m_pSelectObject->Dead();

						//for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
						//	if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
						//		m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
						//		m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
						//		break;
						//	}
						//}
						//m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
						//m_iCurrInteractiveIndex = max(m_iCurrInteractiveIndex - 1, 0);

						//for (auto pObject : m_vecObjects[MAPOBJ_INTERACTIVE])
						//	dynamic_pointer_cast<CGimmickBase>(pObject)->Update_Relatives();
					}
				}

				if (m_pSelectObject != nullptr)
				{
					ImGui::SeparatorText("Edit Attribute");
					shared_ptr<CGimmickBase> pCastedObject = dynamic_pointer_cast<CGimmickBase>(m_pSelectObject);

					if (pCastedObject)
					{
						pCastedObject->Edit_Attribute();

						ImGui::SeparatorText("Set Relatives");

						if (ImGui::BeginListBox("##Dest", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 150.f)))
						{
							_int iGimmickIndex = 0;
							for (_uint i = 0; i < m_vecObjects[MAPOBJ_INTERACTIVE].size(); i++)
							{
								_bool bIsSelected = (m_iCurrGimmickIndex == i);
								if (ImGui::Selectable((m_vecCloneNames[m_eObjectType][i] + "_" + to_string(iGimmickIndex++)).c_str(), bIsSelected))
									m_iCurrGimmickIndex = i;

								if (bIsSelected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}

							ImGui::EndListBox();
						}

						ImGui::SameLine(0.f, 5.f);

						vector<shared_ptr<CGimmickBase>> vecRelatives = pCastedObject->Get_Relatives();

						if (ImGui::BeginListBox("##", ImVec2(0.f, 150.f)))
						{
							for (_uint i = 0; i < vecRelatives.size(); ++i)
							{
								_bool bIsSelected = (m_iCurrRelativeIndex == i);
								if (ImGui::Selectable(vecRelatives[i]->Get_GimmickID().c_str(), bIsSelected))
									m_iCurrRelativeIndex = i;

								if (bIsSelected)
									ImGui::SetItemDefaultFocus();

							}

							ImGui::EndListBox();
						}

						if (ImGui::Button("Add"))
							pCastedObject->Store_Relatives(dynamic_pointer_cast<CGimmickBase>(m_vecObjects[MAPOBJ_INTERACTIVE][m_iCurrGimmickIndex]));

						ImGui::SameLine();

						if (ImGui::Button("Remove"))
						{
							string strGimmickID = vecRelatives[m_iCurrRelativeIndex]->Get_GimmickID();
							shared_ptr<CGameObject> CurObject = { nullptr };
							for (auto iter = m_vecObjects[MAPOBJ_INTERACTIVE].begin(); iter != m_vecObjects[MAPOBJ_INTERACTIVE].end(); ++iter)
							{
								if (dynamic_pointer_cast<CGimmickBase>(*iter)->Get_GimmickID() == strGimmickID)
								{
									CurObject = *iter;
									break;
								}
							}

							if (CurObject)
								pCastedObject->Remove_Relatives(dynamic_pointer_cast<CGimmickBase>(CurObject));
						}


					}

				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Culling Box"))
			{
				if (m_eObjectType != CULLINGBOX)
				{
					m_iCurrCloneIndex = 0;
					m_eObjectType = CULLINGBOX;
				}

				_int iCloneIndex = 0;
				if (ImGui::BeginListBox("Clone List"))
				{
					for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); i++)
					{
						_bool bIsSelected = (m_iCurrCloneIndex == i);
						if (ImGui::Selectable((m_vecCloneNames[m_eObjectType][i] + "_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
							m_iCurrCloneIndex = i;

						if (bIsSelected)
						{
							ImGui::SetItemDefaultFocus();
							Imgui_SetSelectObject(m_vecObjects[m_eObjectType][m_iCurrCloneIndex]);
						}
					}

					ImGui::EndListBox();
				}

				if (ImGui::Button("Create"))
				{
					Imgui_ObjCreate();
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete"))
				{
					if (m_pSelectObject != nullptr) 
					{
						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject)
							{
								BackUp_MapDelete(m_vecObjects[m_eObjectType][i], m_vecCloneNames[m_eObjectType][i]);
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
						m_iCurrCloneIndex = max(m_iCurrCloneIndex - 1, 0);


						/*m_pSelectObject->Dead();

						for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
							if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
								m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
								m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
								break;]
							}
						}
						m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
						m_iCurrCloneIndex = max(m_iCurrCloneIndex - 1, 0);*/
					}
				}

				if (m_vecObjects[m_eObjectType].size() > 0)
				{
					_bool bBoarder = static_pointer_cast<CCullingBox>(m_vecObjects[m_eObjectType][m_iCurrCloneIndex])->Is_Boarder();
					if (ImGui::Checkbox("Is Boarder?", &bBoarder))
						static_pointer_cast<CCullingBox>(m_vecObjects[m_eObjectType][m_iCurrCloneIndex])->Set_Boarder(bBoarder);
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();

			if (ImGui::Button("Copy Transform"))
				m_WorldMatCopy = m_pSelectObject->Get_Transform()->Get_WorldMatrix();
			
			ImGui::SameLine();
			if (ImGui::Button("Paste Transform"))
				m_pSelectObject->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatCopy));
		}
	}

	ImGui::End();

	if (ImGui::Begin("Directional Light"))
	{
		ImGui::Text("@@ NOT SAVED @@");
		auto pLight = CGameInstance::Get_Instance()->Return_Light("Light_Directional_Main");

		if (pLight)
		{
			auto LightDesc = pLight->Get_LightDesc();
			_float4 vDirection = LightDesc->vDirection;
			if (ImGui::DragFloat3("Direction", (_float*)&vDirection, 0.01f, 0.f, 1.f))
			{
				LIGHT_DESC TempDesc;
				TempDesc = *LightDesc;
				TempDesc.vDirection = vDirection;
				GAMEINSTANCE->Rewrite_Description("Light_Directional_Main", TempDesc);
			}
			_float* pDiffuse = pLight->Get_LightDiffusePtr();
			ImGui::DragFloat3("Diffuse", pDiffuse, 0.01f, 0.f, 1.f);
			_float* pAmbient = pLight->Get_LightAmbientPtr();
			ImGui::DragFloat3("Ambient", pAmbient, 0.01f, 0.f, 1.f);
			_float* pSpecular = pLight->Get_LightSpecularPtr();
			ImGui::DragFloat3("Specular", pSpecular, 0.01f, 0.f, 1.f);
			_float fIntensity = pLight->Get_LightIntensity();
			if (ImGui::DragFloat("Intensity", &fIntensity, 0.01f, 0.f, 1.f))
				pLight->Set_LightIntensity(fIntensity);
		}

		_float* pLightCulling = GAMEINSTANCE->Get_LightCullingPtr();
		ImGui::DragFloat("Light Culling", pLightCulling, 1.f, 0.f);
	}

	ImGui::End();

	/*if (ImGui::Begin("Section Culling", &window, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open File"))
				{
					fileDialog = ImGui::FileBrowser();
					fileDialog.Open();
					m_eMapLoadType = eMapLoadType::SECTION_LOAD;
				}

				if (ImGui::MenuItem("Save File"))
				{
					fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
					fileDialog.Open();
					m_eMapLoadType = eMapLoadType::SECTION_SAVE;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::InputText("File Name", m_strSectionFileName.data(), sizeof(_tchar)* MAX_PATH);

		_int iCloneIndex = 0;
		if (ImGui::BeginListBox("Clone List"))
		{
			for (_uint i = 0; i < m_vecSectionCulling.size(); i++)
			{
				_bool bIsSelected = (m_iSectionIndex == i);
				if (ImGui::Selectable(("CullingBox_" + to_string(iCloneIndex++)).c_str(), bIsSelected))
					m_iSectionIndex = i;

				if (bIsSelected)
				{
					ImGui::SetItemDefaultFocus();
					Imgui_SetSelectObject(m_vecSectionCulling[i]);
				}
			}
			
			ImGui::EndListBox();	
		}

		if (ImGui::Button("Create"))
		{
			CSectionCullingBox::SECTIONCULLING_DESC sectionDesc{};
			sectionDesc.fSpeedPerSec = 0.f;
			sectionDesc.fRotationPerSec = 0.f;
			sectionDesc.bBoarder = false;
			sectionDesc.worldMat = Identity;

			auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CSectionCullingBox::ObjID, &sectionDesc);
			m_vecSectionCulling.push_back(static_pointer_cast<CSectionCullingBox>(pObject));
		}

		ImGui::SameLine();
		if (ImGui::Button("Delete") && m_vecSectionCulling.size() > 0)
		{
			m_vecSectionCulling[m_iSectionIndex]->Set_Dead();
			m_vecSectionCulling.erase(m_vecSectionCulling.begin() + m_iSectionIndex);
		}

		if (m_pSelectObject != nullptr)
		{
			ImGui::SeparatorText("Edit Attribute");
			shared_ptr<CMisc_Interface> pCastedObject = dynamic_pointer_cast<CMisc_Interface>(m_pSelectObject);

			if (pCastedObject)
				pCastedObject->Edit_Attribute();
		}

		ImGui::End();
	}*/

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		if (m_eMapLoadType == eMapLoadType::MODELNAME_LOAD)
			Load_ModelNames(fileDialog.GetSelected().wstring() + L"\\");
		else
			Imgui_MapDataControl();

		m_eMapLoadType = eMapLoadType::MAP_END;
		fileDialog.ClearSelected();
	}


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_EffectTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("EffectData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_EffectMenuBar();

		ImGui::InputText("Effect Key Input", m_strUnionEffectKey.data(), sizeof(_tchar) * MAX_PATH);

		ImGui::PushItemWidth(200.f);
		if (ImGui::Checkbox("Effect Union Link", &m_bIsUnionLink)) {
			if (m_bIsUnionLink == false) {
				EffectUnLink();
			}
			else {
				EffectLink();
			}
		}
		if (m_bIsUnionLink) {
			ImGui::SameLine();
			if (!m_bUnionPlay) {
				if (ImGui::Button("Effect Union Play")) {
					UnionStart();
					m_bUnionPlay = true;
					m_fUnionLinkTime = 0;
				}
				if (GAMEINSTANCE->Get_KeyDown(DIK_LSHIFT)) {
					UnionStart();
					m_bUnionPlay = true;
					m_fUnionLinkTime = 0;
				}
			}
			else {
				if (!m_bUnionPause) {
					if (ImGui::Button("Effect Union Pause")) {
						m_bUnionPause = true;
						EffectPause();
					}
				}
				else {
					if (ImGui::Button("Effect Union Resume")) {
						m_bUnionPause = false;
						EffectPause();
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Effect Union Stop")) {
				m_bUnionPlay = false;
				m_bUnionPause = false;
				m_fUnionLinkTime = 0;
				UnionEnd();
			}
		}

		ImGui::BeginChild(13, ImVec2(460, 501), true);

		ImGui::SetCursorPos(ImVec2(6, 9.5));
		ImGui::PushItemWidth(435);

		if (ImGui::BeginCombo("##", m_vecEffectNames[m_strSelectEffectNum].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecEffectNames.size(); n++) {
				if (ImGui::Selectable(m_vecEffectNames[n].data(), n == m_strSelectEffectNum)) {
					m_strSelectEffectNum = n;
					m_pSelectObject = nullptr;
				}
			}
			ImGui::EndCombo();
		}


		ImGui::PopItemWidth();

		ImGui::SetCursorPos(ImVec2(7, 35.5));
		if (ImGui::Button("Create", ImVec2(214, 19))) {
			Imgui_EffectCreate();
		}

		ImGui::SetCursorPos(ImVec2(227, 35.5));
		if (ImGui::Button("Delete", ImVec2(214, 19))) {
			if (m_pSelectObject != nullptr) {
				m_pSelectObject->Dead();

				for (_uint i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
					if (m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][i] == m_pSelectObject) {
						m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].erase(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].begin() + i);
						break;
					}
				}

				m_pSelectObject = m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].empty() ? nullptr : static_pointer_cast<CEffect_Base>(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back());
			}
		}

		ImGui::SetCursorPos(ImVec2(5, 67));
		ImGui::PushItemWidth(438);
		static int item_current36 = 0;

		if (ImGui::BeginListBox("##", ImVec2(428, 150))) {
			for (int i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
				string str = m_vecEffectNames[m_strSelectEffectNum];
				if (ImGui::Selectable((str += to_string(i)).c_str(), item_current36 == i)) {
					item_current36 = i;
					Imgui_SetSelectObject(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][item_current36]);
				}
			}
			ImGui::EndListBox();
		}


		ImGui::PopItemWidth();

		if (m_pSelectObject != nullptr) {
			Imgui_EffectOptionRender();
		}

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_EffectDataControl();
		m_eEffectLoadType = eEffectLoadType::EFFECT_END;
		fileDialog.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_SoundTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("SoundData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_MenuBar();

		ImGui::SetCursorPos(ImVec2(19, 34));
		ImGui::Button("Object", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(74.5, 34));
		ImGui::Button("Light", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(122.5, 34));
		ImGui::Button("Model", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(12, 63));
		ImGui::BeginChild(13, ImVec2(448, 501), true);

		ImGui::SetCursorPos(ImVec2(6, 9.5));
		ImGui::PushItemWidth(435);

		if (ImGui::BeginCombo("##", m_vecObjectNames[m_eObjectType].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecObjectNames.size(); n++) {
				if (ImGui::Selectable(m_vecObjectNames[n].data(), n == m_eObjectType)) {
					m_eObjectType = static_cast<eObjectType>(n);
					m_pSelectObject = nullptr;
				}
			}
			ImGui::EndCombo();
		}


		ImGui::PopItemWidth();

		ImGui::SetCursorPos(ImVec2(7, 35.5));
		if (ImGui::Button("Create", ImVec2(214, 19))) {
			Imgui_ObjCreate();
		}

		ImGui::SetCursorPos(ImVec2(227, 35.5));
		if (ImGui::Button("Delete", ImVec2(214, 19))) {
			if (m_pSelectObject != nullptr) {
				m_pSelectObject->Dead();

				for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
					if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
						m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
						break;
					}
				}
				m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
			}

		}

		ImGui::SetCursorPos(ImVec2(5, 67));
		ImGui::PushItemWidth(438);
		static int item_current36 = 0;


		if (ImGui::BeginListBox("##", ImVec2(428, 300))) {
			for (int i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
				if (ImGui::Selectable((m_vecObjectNames[i] += to_string(i)).c_str(), item_current36 == i)) {
					item_current36 = i;
					Imgui_SetSelectObject(m_vecObjects[m_eObjectType][item_current36]);
				}
			}
			ImGui::EndListBox();
		}


		ImGui::PopItemWidth();

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_MapDataControl();
		m_eMapLoadType = eMapLoadType::MAP_END;
		fileDialog.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_AnimationTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	ImGui::Begin("Animation Setting");
	{
		// Resource List
		if (ImGui::Button("Load Model"))
		{
			fileDialog = ImGui::FileBrowser(0, "../Bin/Resources/Models/");
			fileDialog.SetTypeFilters({ ".fbx" });
			fileDialog.Open();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Model"))
		{
			if (m_pFreeObj != nullptr)
			{
				m_pFreeObj->Dead();
				m_pFreeObj = nullptr;

				m_iAnim_Select = -1;
				m_AnimList.clear();

				m_iSound_Select = -1;
				m_SoundList.clear();

				m_iFrame_Select = -1;
			}
		}

		if (ImGui::BeginListBox("##Anim List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_int iSelectIndex = -1;
			for (auto& strAnim : m_AnimList)
			{
				++iSelectIndex;
				const bool is_selected = (m_iAnim_Select == iSelectIndex);
				if (ImGui::Selectable(strAnim.c_str(), is_selected))
				{
					if (m_iAnim_Select == iSelectIndex)
					{
						m_iAnim_Select = -1;
						m_iFrame_Select = -1;
					}
					else
					{
						m_iAnim_Select = iSelectIndex;
						m_iFrame_Select = -1;

						m_pFreeObj->SwitchAnim(m_AnimList[m_iAnim_Select]);
					}
				}
			}
			ImGui::EndListBox();
		}

		if (nullptr != m_pFreeObj && -1 != m_iAnim_Select)
		{
			const char* items[] = { "Logic", "Sound" };
			static int item_selected_idx = 0;
			const char* combo_preview_value = items[item_selected_idx];
			if (ImGui::BeginCombo("Mode", combo_preview_value))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					const bool is_selected = (item_selected_idx == n);
					if (ImGui::Selectable(items[n], is_selected))
						item_selected_idx = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			shared_ptr<CAnimation> pAnimSelected = nullptr;
			m_pFreeObj->GetAnimInfo(pAnimSelected, m_iAnim_Select);

			if (item_selected_idx == 1)
			{
				if (ImGui::BeginListBox("##SoundList"))
				{
					_int iSelectIndex = -1;
					for (auto pairSound : m_SoundList)
					{
						ImGui::PushID(iSelectIndex);

						++iSelectIndex;
						const bool is_selected = (m_iSound_Select == iSelectIndex);
						if (ImGui::Selectable(pairSound.first.c_str(), is_selected))
						{
							if (m_iSound_Select == iSelectIndex)
								m_iSound_Select = -1;
							else
								m_iSound_Select = iSelectIndex;
						}

						ImGui::PopID();
					}
					ImGui::EndListBox();
				}
				ImGui::Spacing();

				ImGui::Checkbox("RootAnim", &pAnimSelected->m_isRootAnim);

				static _float fFramePos = 0.f;
				ImGui::SliderFloat("Frame", &fFramePos, 0.f, pAnimSelected->Get_Duration(), "%.1f");
				ImGui::SameLine();
				ImGui::InputFloat("##inputfloat", &fFramePos);
				m_pFreeObj->SetAnimFrame(fFramePos);

				static string strPlay = "Play";
				if (ImGui::Button(strPlay.c_str()) || GAMEINSTANCE->Get_KeyDown(DIK_SPACE)) {
					strPlay = m_pFreeObj->SwitchPlayMode();
				}
				ImGui::SameLine();
				ImGui::ProgressBar(pAnimSelected->Get_Current_Track_Posiiton() / pAnimSelected->Get_Duration());

				ImGui::Spacing();

				static _float fVolume = 1.f;
				ImGui::SliderFloat("Volume", &fVolume, 0.f, 1.f, "%.1f");

				if (ImGui::Button("Save Sound"))
				{
					if (m_iSound_Select != -1)
					{
						pAnimSelected->PushSoundCallback(fFramePos, m_SoundList[m_iSound_Select].first, fVolume);
						m_iFrame_Select = -1;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete Sound"))
				{
					pAnimSelected->PopSoundCallback(m_iFrame_Select);
					m_iFrame_Select = -1;
				}

				if (ImGui::BeginListBox("##SoundCallbackList"))
				{
					_int iSelectIndex = -1;
					for (auto fFrame : pAnimSelected->m_Frame_Sound_List)
					{
						ImGui::PushID(iSelectIndex);

						++iSelectIndex;
						const bool is_selected = (m_iFrame_Select == iSelectIndex);
						if (ImGui::Selectable(to_string(fFrame).c_str(), is_selected))
						{
							if (m_iFrame_Select == iSelectIndex)
								m_iFrame_Select = -1;
							else
							{
								m_iFrame_Select = iSelectIndex;
								fFramePos = fFrame;
							}
						}

						ImGui::PopID();
					}
					ImGui::EndListBox();
				}
				if (-1 != m_iFrame_Select)
				{
					wstring strSoundName = pAnimSelected->m_Sound_List[m_iFrame_Select];
					ImGui::Text(string(strSoundName.begin(), strSoundName.end()).c_str());
					ImGui::Text("Volume: %f", pAnimSelected->m_Sound_Volume[m_iFrame_Select]);
				}

				// Save Final
				if (ImGui::Button("Save as binary"))
				{
					string strFolderPath = m_strAnimFilePath;
					strFolderPath.erase(strFolderPath.length() - 4);
					strFolderPath += "/";

					if (FAILED(pAnimSelected->ExportSoundInfo(strFolderPath)))
						MSG_BOX(TEXT("Failed to save sound info"));
					else
						MSG_BOX(TEXT("Success to save sound info"));
				}
			}
			else
			{
				ImGui::Checkbox("RootAnim", &pAnimSelected->m_isRootAnim);
				ImGui::SameLine();
				ImGui::Checkbox("ForcedNonRoot", &pAnimSelected->m_isForcedNonRootAnim);

				static _float fFramePos = 0.f;
				ImGui::SliderFloat("Frame", &fFramePos, 0.f, pAnimSelected->Get_Duration(), "%.1f");
				ImGui::SameLine();
				ImGui::InputFloat("##inputfloat", &fFramePos);
				m_pFreeObj->SetAnimFrame(fFramePos);

				if (ImGui::Button("Save CallBack"))
				{
					pAnimSelected->m_FrameCallbackList.push_back(fFramePos);
					sort(pAnimSelected->m_FrameCallbackList.begin(), pAnimSelected->m_FrameCallbackList.end());
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete CallBack"))
				{
					if (pAnimSelected->m_FrameCallbackList.size() > 0)
						pAnimSelected->m_FrameCallbackList.pop_back();
				}

				if (ImGui::BeginListBox("##AnimationCallbackList"))
				{
					for (auto fFrame : pAnimSelected->m_FrameCallbackList)
					{
						_bool test = false;
						if (ImGui::Selectable(to_string(fFrame).c_str(), test))
							fFramePos = fFrame;
					}
					ImGui::EndListBox();
				}

				// Save animation set info
				if (ImGui::Button("Save Custom AnimInfo"))
				{
					string lowerStr = m_strAnimFilePath;
					transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

					string strFolderPath = m_strAnimFilePath;
					string subStr = ".fbx";
					size_t pos = lowerStr.find(subStr);
					while (pos != string::npos) {
						strFolderPath.erase(pos, subStr.length());
						pos = strFolderPath.find(subStr);
					}
					strFolderPath += "/";

					if (FAILED(m_pFreeObj->ExportAnimInfo(strFolderPath)))
						MSG_BOX(TEXT("Failed to save"));
					else
						MSG_BOX(TEXT("Success to save"));
				}
			}
		}
	}
	ImGui::End();

	fileDialog.Display();
	if (fileDialog.HasSelected())
	{
		m_strAnimFilePath = fileDialog.GetSelected().string();
		auto iCurLevel = CGame_Manager::Get_Instance()->Get_Current_Level();

		CFree_SK_Object::DESC desc{};
		desc.strModelPath = m_strAnimFilePath;
		m_pFreeObj = dynamic_pointer_cast<CFree_SK_Object>(GAMEINSTANCE->Add_Clone_Return(iCurLevel, TEXT("TEST_LAYER"), CFree_SK_Object::ObjID, &desc));
		m_pFreeObj->GetAnimStrList(m_AnimList);

		// Load Sound List
		m_iSound_Select = -1;
		m_SoundList.clear();

		filesystem::path pathSound(m_strAnimFilePath);
		string strSoundPath = pathSound.parent_path().string();
		strSoundPath += "/Sound/";
		for (const auto& entry : filesystem::directory_iterator(strSoundPath)) {
			if (entry.is_regular_file()) {
				string fileName = entry.path().filename().string();
				fileName.erase(fileName.length() - 4);
				string filePath = entry.path().string();

				m_SoundList.push_back({ fileName, filePath });
			}
		}

		fileDialog.ClearSelected();
	}


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_LinkTool()
{

	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (GAMEINSTANCE->Get_KeyDown(DIK_T))
	{
		EffectAnchor();
	}

	if (ImGui::Begin("EffectData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_EffectMenuBar();

		ImGui::InputText("Effect Key Input", m_strUnionEffectKey.data(), sizeof(_tchar) * MAX_PATH);

		ImGui::PushItemWidth(200.f);

		if (ImGui::Checkbox("Effect Union Link", &m_bIsUnionLink)) {
			if (m_bIsUnionLink == false) {
				EffectUnLink();
			}
			else {
				EffectLink();
			}
		}
		if (m_bIsUnionLink) {
			ImGui::SameLine();
			if (!m_bUnionPlay) {
				if (ImGui::Button("Effect Union Play")) {
					UnionStart();
					m_bUnionPlay = true;
					m_fUnionLinkTime = 0;
				}
				if (GAMEINSTANCE->Get_KeyDown(DIK_LSHIFT)) {
					UnionStart();
					m_bUnionPlay = true;
					m_fUnionLinkTime = 0;
				}
			}
			else {
				if (!m_bUnionPause) {
					if (ImGui::Button("Effect Union Pause")) {
						m_bUnionPause = true;
						EffectPause();
					}
				}
				else {
					if (ImGui::Button("Effect Union Resume")) {
						m_bUnionPause = false;
						EffectPause();
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Effect Union Stop")) {
				m_bUnionPlay = false;
				m_bUnionPause = false;
				m_fUnionLinkTime = 0;
				UnionEnd();
			}
		}

		ImGui::BeginChild(13, ImVec2(460, 501), true);

		ImGui::SetCursorPos(ImVec2(6, 9.5));
		ImGui::PushItemWidth(435);

		if (ImGui::BeginCombo("##", m_vecEffectNames[m_strSelectEffectNum].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecEffectNames.size(); n++) {
				if (ImGui::Selectable(m_vecEffectNames[n].data(), n == m_strSelectEffectNum)) {
					m_strSelectEffectNum = n;
					m_pSelectObject = nullptr;
				}
			}
			ImGui::EndCombo();
		}


		ImGui::PopItemWidth();

		ImGui::SetCursorPos(ImVec2(7, 35.5));
		if (ImGui::Button("Create", ImVec2(138, 19))) {
			Imgui_EffectCreate();
		}

		ImGui::SetCursorPos(ImVec2(152, 35.5));
		if (ImGui::Button("Delete", ImVec2(138, 19))) {
			if (m_pSelectObject != nullptr) {
				m_pSelectObject->Dead();

				for (_uint i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
					if (m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][i] == m_pSelectObject) {
						m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].erase(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].begin() + i);
						break;
					}
				}

				m_pSelectObject = m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].empty() ? nullptr : static_pointer_cast<CEffect_Base>(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back());
			}
		}

		ImGui::SetCursorPos(ImVec2(297, 35.5));
		if (ImGui::Button("Clone", ImVec2(138, 19))) {
			Imgui_EffectCopy();
		}


		ImGui::SetCursorPos(ImVec2(5, 67));
		ImGui::PushItemWidth(438);
		static int item_current36 = 0;

		if (ImGui::BeginListBox("##", ImVec2(428, 150))) {
			for (int i = 0; i < m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].size(); ++i) {
				string str = m_vecEffectNames[m_strSelectEffectNum];
				if (ImGui::Selectable((str += to_string(i)).c_str(), item_current36 == i)) {
					item_current36 = i;
					Imgui_SetSelectObject(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]][item_current36]);
				}
			}
			ImGui::EndListBox();
		}


		ImGui::PopItemWidth();

		if (m_pSelectObject != nullptr) {
			Imgui_EffectOptionRender();
		}

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_EffectDataControl();
		m_eEffectLoadType = eEffectLoadType::EFFECT_END;
		fileDialog.ClearSelected();
	}


	//Animation Tool
	ImGui::Begin("Animation Setting");
	{
		// Resource List
		if (ImGui::Button("Load Model"))
		{
			fileDialog2 = ImGui::FileBrowser(0, "../Bin/Resources/Models/");
			fileDialog2.SetTypeFilters({ ".fbx" });
			fileDialog2.Open();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Model"))
		{
			if (m_pFreeObj != nullptr)
			{
				m_pFreeObj->Dead();
				m_pFreeObj = nullptr;

				m_iAnim_Select = -1;
				m_AnimList.clear();

				m_iEffect_Select = -1;
				m_EffectList.clear();

				m_iFrame_Select = -1;
			}
		}

		if (ImGui::BeginListBox("##Anim List", ImVec2(240.f, 10 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_int iSelectIndex = -1;
			for (auto& strAnim : m_AnimList)
			{
				++iSelectIndex;
				const bool is_selected = (m_iAnim_Select == iSelectIndex);
				if (ImGui::Selectable(strAnim.c_str(), is_selected))
				{
					if (m_iAnim_Select == iSelectIndex)
					{
						m_iAnim_Select = -1;
						m_iFrame_Select = -1;
					}
					else
					{
						m_iAnim_Select = iSelectIndex;
						m_iFrame_Select = -1;

						m_pFreeObj->SwitchAnim(m_AnimList[m_iAnim_Select]);
					}
				}
			}
			ImGui::EndListBox();
		}
		ImGui::SameLine();
		if (ImGui::BeginListBox("##Effect List", ImVec2(240.f, 10 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_int iSelectIndex = -1;
			for (auto& strEffect : m_EffectList)
			{
				++iSelectIndex;
				const bool is_selected = (m_iEffect_Select == iSelectIndex);
				if (ImGui::Selectable(strEffect.c_str(), is_selected))
				{
					if (m_iEffect_Select == iSelectIndex)
						m_iEffect_Select = -1;
					else
						m_iEffect_Select = iSelectIndex;
				}
			}

			ImGui::EndListBox();
		}

		if (nullptr != m_pFreeObj && -1 != m_iAnim_Select)
		{
			shared_ptr<CAnimation> pAnimSelected = nullptr;
			m_pFreeObj->GetAnimInfo(pAnimSelected, m_iAnim_Select);
			ImGui::Checkbox("RootAnim", &pAnimSelected->m_isRootAnim);

			static _float fFramePos = 0.f;
			ImGui::SliderFloat("Frame", &fFramePos, 0.f, pAnimSelected->Get_Duration(), "%.1f");
			ImGui::SameLine();
			ImGui::InputFloat("##inputfloat", &fFramePos);
			m_pFreeObj->SetAnimFrame(fFramePos);

			static string strPlay = "Play";
			if (ImGui::Button(strPlay.c_str()) || GAMEINSTANCE->Get_KeyDown(DIK_SPACE) || GAMEINSTANCE->Get_KeyDown(DIK_LSHIFT)) {
				strPlay = m_pFreeObj->SwitchPlayMode();
			}
			ImGui::SameLine();
			ImGui::ProgressBar(pAnimSelected->Get_Current_Track_Posiiton() / pAnimSelected->Get_Duration());


			if (ImGui::Button("Save Eff_Callback"))
			{
				if (m_iEffect_Select != -1)
				{
					_float4x4 matTest{};
					XMStoreFloat4x4(&matTest, XMMatrixIdentity());
					pAnimSelected->PushEffectCallback(fFramePos, m_EffectList[m_iEffect_Select], matTest);

					m_iFrame_Select = -1;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Eff_Callback"))
			{
				pAnimSelected->PopEffectCallback(m_iFrame_Select);
				m_iFrame_Select = -1;
			}

			if (ImGui::BeginListBox("##AnimationCallbackList"))
			{
				_int iSelectIndex = -1;
				for (auto fFrame : pAnimSelected->m_Frame_Effect_List)
				{
					ImGui::PushID(iSelectIndex);

					++iSelectIndex;
					const bool is_selected = (m_iFrame_Select == iSelectIndex);
					if (ImGui::Selectable(to_string(fFrame).c_str(), is_selected))
					{
						if (m_iFrame_Select == iSelectIndex)
							m_iFrame_Select = -1;
						else
						{
							m_iFrame_Select = iSelectIndex;
							fFramePos = fFrame;
						}
					}

					ImGui::PopID();
				}
				ImGui::EndListBox();
			}

			if (-1 != m_iFrame_Select)
			{
				ImGui::Text(pAnimSelected->m_EffectList[m_iFrame_Select].c_str());

				_bool isFollow = pAnimSelected->m_Effect_IsFollow_List.at(m_iFrame_Select);
				if (ImGui::Checkbox("AttachEffect", &isFollow))
					pAnimSelected->m_Effect_IsFollow_List[m_iFrame_Select] = isFollow;
				ImGui::SameLine();
				_bool isBillBoard = pAnimSelected->m_Effect_IsBillBoard_List.at(m_iFrame_Select);
				if (ImGui::Checkbox("BillBoard", &isBillBoard))
					pAnimSelected->m_Effect_IsBillBoard_List[m_iFrame_Select] = isBillBoard;
			}

			// Save animation set info
			if (ImGui::Button("Save Custom AnimInfo"))
			{
				string lowerStr = m_strAnimFilePath;
				transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

				string strFolderPath = m_strAnimFilePath;
				string subStr = ".fbx";
				size_t pos = lowerStr.find(subStr);
				while (pos != string::npos) {
					strFolderPath.erase(pos, subStr.length());
					pos = strFolderPath.find(subStr);
				}
				strFolderPath += "/";

				if (FAILED(m_pFreeObj->ExportAnimInfo(strFolderPath)))
					MSG_BOX(TEXT("Failed to save"));
				else
					MSG_BOX(TEXT("Success to save"));
			}
		}
	}
	ImGui::End();

	fileDialog2.Display();
	if (fileDialog2.HasSelected())
	{
		m_strAnimFilePath = fileDialog2.GetSelected().string();
		auto iCurLevel = CGame_Manager::Get_Instance()->Get_Current_Level();

		CFree_SK_Object::DESC desc{};
		desc.strModelPath = m_strAnimFilePath;
		m_pFreeObj = dynamic_pointer_cast<CFree_SK_Object>(GAMEINSTANCE->Add_Clone_Return(iCurLevel, TEXT("TEST_LAYER"), CFree_SK_Object::ObjID, &desc));
		m_pFreeObj->GetAnimStrList(m_AnimList);

		EFFECTMANAGER->Get_EffectKey(m_EffectList);

		fileDialog2.ClearSelected();
	}

	ImGui::Begin("Weapon Setting");
	{
		// Resource List
		if (ImGui::Button("Load Model"))
		{
			fileDialog_for_weapon = ImGui::FileBrowser(0, "../Bin/Resources/Models/");
			fileDialog_for_weapon.SetTypeFilters({ ".fbx" });
			fileDialog_for_weapon.Open();
		}

		if (nullptr != m_pFreeObj && nullptr != m_pWeaponObj)
		{
			// Bone List ???
			if (ImGui::BeginListBox("##Bone List", ImVec2(240.f, 10 * ImGui::GetTextLineHeightWithSpacing())))
			{
				_int iSelectIndex = -1;
				for (auto& strBone : m_BoneList)
				{
					++iSelectIndex;
					const bool is_selected = (m_iBone_Select == iSelectIndex);
					if (ImGui::Selectable(strBone.c_str(), is_selected))
					{
						if (m_iBone_Select == iSelectIndex)
							m_iBone_Select = -1;
						else
						{
							m_iBone_Select = iSelectIndex;
							m_pWeaponObj->AttachBone(m_pFreeObj->GetBoneMatrix(strBone));
						}
					}
				}
				ImGui::EndListBox();
			}
		}
	}
	ImGui::End();

	fileDialog_for_weapon.Display();
	if (fileDialog_for_weapon.HasSelected())
	{
		auto iCurLevel = CGame_Manager::Get_Instance()->Get_Current_Level();

		CFree_SM_Object::DESC desc{};
		desc.strModelPath = fileDialog_for_weapon.GetSelected().string();
		m_pWeaponObj = dynamic_pointer_cast<CFree_SM_Object>(GAMEINSTANCE->Add_Clone_Return(iCurLevel, TEXT("TEST_LAYER"), CFree_SM_Object::ObjID, &desc));

		m_BoneList.clear();
		if (nullptr != m_pFreeObj)
		{
			m_pFreeObj->GetBoneList(m_BoneList);
			m_pWeaponObj->AttachBone(m_pFreeObj->GetBoneMatrix("r_attach_hand_00"));
		}

		fileDialog_for_weapon.ClearSelected();
	}











	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_CineTool()
{
	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	ImGui::Begin("Actors Setting");
	{

	}
	ImGui::End();

	fileDialog.Display();
	if (fileDialog.HasSelected())
	{
		m_strAnimFilePath = fileDialog.GetSelected().string();
		auto iCurLevel = CGame_Manager::Get_Instance()->Get_Current_Level();

		fileDialog.ClearSelected();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Imgui_DebugTool()
{

}

void CImgui_Manager::Imgui_Main()
{
}

void CImgui_Manager::Imgui_GizmoRender()
{
	if (m_pSelectObject == nullptr)
		return;

	shared_ptr<CTransform> pTerrainTransform = static_pointer_cast<CTransform>(m_pSelectObject->Find_Component(L"Com_Transform"));
	_float4x4 WorldMat, ViewMat, ProjMat;
	_float4  vPos;
	static _float3 vRot = { 0.f,0.f,0.f };
	_float3 vScale;

	if (pTerrainTransform == nullptr)
		return;

	WorldMat = pTerrainTransform->Get_WorldMatrix();
	ViewMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	ProjMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	_bool flag = false;

	ImGui::Text("");
	ImGui::Text("Transform Data");

	ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &vRot.x, &vScale.x);
	ImGui::PushItemWidth(200.f);
	flag = ImGui::InputFloat3("Trans", &vPos.x);
	if (flag) {
		pTerrainTransform->Set_Position(XMVectorSet(vPos.x, vPos.y, vPos.z, vPos.w));
		if (eToolType::TOOL_EFFECT == m_eToolType) static_pointer_cast<CEffect_Base>(m_pSelectObject)->SetRootMatrix(pTerrainTransform->Get_WorldMatrix());
	}
	ImGui::PushItemWidth(200.f);

	_float3 vRadianRot = {};

	flag = ImGui::InputFloat3("Rotat", &vRot.x);
	if (flag) {
		ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, &WorldMat._11);
		pTerrainTransform->Set_WorldMatrix(WorldMat);
		if (eToolType::TOOL_EFFECT == m_eToolType) static_pointer_cast<CEffect_Base>(m_pSelectObject)->SetRootMatrix(pTerrainTransform->Get_WorldMatrix());
	}
	ImGui::PushItemWidth(200.f);

	flag = ImGui::InputFloat3("Scale", &vScale.x);
	if (flag) {
		if (vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f) {
			pTerrainTransform->Set_Scale(vScale);
			if (eToolType::TOOL_EFFECT == m_eToolType) static_pointer_cast<CEffect_Base>(m_pSelectObject)->SetRootMatrix(pTerrainTransform->Get_WorldMatrix());
		}
	}
	if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
		, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
		, m_tGizmoDesc.CurrentGizmoMode				// WORLD, LOCAL
		, (_float*)&WorldMat
		, NULL
		, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL
		, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
		, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
	{
		m_pSelectObject->Get_Transform()->Set_WorldMatrix(WorldMat);
		if (eToolType::TOOL_EFFECT == m_eToolType)
			static_pointer_cast<CEffect_Base>(m_pSelectObject)->SetRootMatrix(WorldMat);
	}

	if (m_eObjectType == LIGHT) {
		shared_ptr<CLight> pLight = static_pointer_cast<CLight>(m_pSelectObject);

		ImGui::PushItemWidth(200.f);
		ImGui::InputFloat4("Diffuse", pLight->Get_LightDiffusePtr());

		ImGui::InputFloat4("Ambient", pLight->Get_LightAmbientPtr());

		ImGui::InputFloat4("Specular", pLight->Get_LightSpecularPtr());

		ImGui::InputFloat("Range", pLight->Get_LightRangePtr());
	}


}

void CImgui_Manager::Imgui_EffectOptionRender()
{
	if (m_pSelectObject == nullptr)
		return;

	_bool flag = false;

	shared_ptr<CEffect_Base> pEffect = static_pointer_cast<CEffect_Base>(m_pSelectObject);

	ImGui::Text("");
	ImGui::Text("TimeData");

	ImGui::PushItemWidth(200.f);
	_float fEffectTimeAcc = pEffect->GetEffectTimeAcc();
	flag = ImGui::InputFloat("TimeAcc", &fEffectTimeAcc);
	if (flag) {
		pEffect->SetEffectTimeAcc(fEffectTimeAcc);
	}
	ImGui::SameLine();
	flag = ImGui::Button("+");
	if (flag) {
		pEffect->SetEffectTimeAcc(pEffect->GetEffectTimeAcc() + 0.1f);
		pEffect->ToolTurning(0.1f);
	}
	ImGui::SameLine();
	flag = ImGui::Button("-");
	if (flag) {
		pEffect->SetEffectTimeAcc(pEffect->GetEffectTimeAcc() - 0.1f);
		pEffect->ToolTurning(-0.1f);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimeMin = pEffect->GetEffectTimeMin();
	flag = ImGui::InputFloat("TimeAccMin", &fEffectTimeMin);
	if (flag) {
		pEffect->SetEffectTimeMin(fEffectTimeMin);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimeMax = pEffect->GetEffectTimeMax();
	flag = ImGui::InputFloat("TimeAccMax", &fEffectTimeMax);
	if (flag) {
		pEffect->SetEffectTimeMax(fEffectTimeMax);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimeMultiplier = pEffect->GetEffectTimeAccMultiplier();
	flag = ImGui::InputFloat("TimeAccMulti", &fEffectTimeMultiplier);
	if (flag) {
		pEffect->SetEffectTimeAccMultiplier(fEffectTimeMultiplier);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectTimePercent = pEffect->GetEffectTimePercent();
	//flag = ImGui::InputFloat("TimePersect", &fEffectTimePercent);
	if (ImGui::SliderFloat("TimePersect", &fEffectTimePercent, 0.f, 1.f)) {
		pEffect->Set_ProgressBar_Inversion(fEffectTimePercent);
	}
	ImGui::ProgressBar(fEffectTimePercent);

	ImGui::PushItemWidth(200.f);

	_bool fEffectTimeLoop = pEffect->GetEffectLoop();
	flag = ImGui::Checkbox("TimeAccLoop", &fEffectTimeLoop);
	if (flag) {
		pEffect->SetEffectLoop(fEffectTimeLoop);
	}
	ImGui::SameLine();
	_bool fEffectTimeFreeze = pEffect->GetEffectTimeFreeze();
	flag = ImGui::Checkbox("TimeAccFreeze", &fEffectTimeFreeze);
	if (flag) {
		pEffect->SetEffectTimeFreeze(fEffectTimeFreeze);
	}
	ImGui::SameLine();
	_bool fEffectView = pEffect->GetEffectIsView();
	flag = ImGui::Checkbox("EffectIsView", &fEffectView);
	if (flag) {
		pEffect->SetEffectIsView(fEffectView);
	}

	ImGui::Text("");
	ImGui::Text("BaseColorData");

	ImGui::PushItemWidth(200.f);

	_float4 fEffectBaseColor = pEffect->GetEffectBaseColor();
	flag = ImGui::InputFloat4("BaseColor", &fEffectBaseColor.x);
	if (flag) {
		pEffect->SetEffectBaseColorToColor(fEffectBaseColor);
	}

	ImGui::PushItemWidth(200.f);

	_float fEffectBaseColorBlendRate = pEffect->GetEffectColorBlendRate();
	flag = ImGui::InputFloat("BaseColorBlendRate", &fEffectBaseColorBlendRate);
	if (flag) {
		pEffect->SetEffectColorBlendRate(fEffectBaseColorBlendRate);
	}

	ImGui::Text("");
	ImGui::Text("MaskColorData");

	ImGui::PushItemWidth(200.f);
	_float4 fEffectColorPresetR = pEffect->GetColorPresetR();
	flag = ImGui::InputFloat4("ColorPresetR", &fEffectColorPresetR.x);
	if (flag) {
		pEffect->SetColorPresetR(fEffectColorPresetR);
	}

	ImGui::PushItemWidth(200.f);
	_float4 fEffectColorPresetG = pEffect->GetColorPresetG();
	flag = ImGui::InputFloat4("ColorPresetG", &fEffectColorPresetG.x);
	if (flag) {
		pEffect->SetColorPresetG(fEffectColorPresetG);
	}

	ImGui::PushItemWidth(200.f);
	_float4 fEffectColorPresetB = pEffect->GetColorPresetB();
	flag = ImGui::InputFloat4("ColorPresetB", &fEffectColorPresetB.x);
	if (flag) {
		pEffect->SetColorPresetB(fEffectColorPresetB);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectColorAlpha = pEffect->GetEffectAlpha();
	flag = ImGui::InputFloat("ColorAlpha", &fEffectColorAlpha);
	if (flag) {
		pEffect->SetEffectAlpha(fEffectColorAlpha);
	}

	ImGui::SameLine();

	_bool fEffectAlphaLink = pEffect->GetEffectAlphaLinkTime();
	flag = ImGui::Checkbox("Color Time-Alpha Link", &fEffectAlphaLink);
	if (flag) {
		pEffect->SetEffectAlphaLinkTime(fEffectAlphaLink);
	}

	ImGui::PushItemWidth(200.f);

	ImGui::Text("");
	ImGui::Text("UvFlowData");

	_bool fEffectUvFlowX = pEffect->GetUseEffectUvFlowX();
	flag = ImGui::Checkbox("Effect Uv Flow X", &fEffectUvFlowX);
	if (flag) {
		pEffect->SetUseEffectUvFlowX(fEffectUvFlowX);
	}
	ImGui::SameLine();
	_bool fEffectUvFlowY = pEffect->GetUseEffectUvFlowY();
	flag = ImGui::Checkbox("Effect Uv Flow Y", &fEffectUvFlowY);
	if (flag) {
		pEffect->SetUseEffectUvFlowY(fEffectUvFlowY);
	}

	ImGui::PushItemWidth(200.f);

	_float2 fEffectUvFlow = pEffect->GetEffectUvFlow();
	flag = ImGui::InputFloat2("UvFlow XY", &fEffectUvFlow.x);
	if (flag) {
		pEffect->SetEffectUvFlow(fEffectUvFlow.x, fEffectUvFlow.y);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSpeedX = pEffect->GetEffectUvFlowXSpeed();
	flag = ImGui::InputFloat("UvFlowSpeed X", &fEffectUvFlowSpeedX);
	if (flag) {
		pEffect->SetEffectUvFlowXSpeed(fEffectUvFlowSpeedX);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSpeedY = pEffect->GetEffectUvFlowYSpeed();
	flag = ImGui::InputFloat("UvFlowSpeed Y", &fEffectUvFlowSpeedY);
	if (flag) {
		pEffect->SetEffectUvFlowYSpeed(fEffectUvFlowSpeedY);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSizeMultiX = pEffect->GetEffectUvSizeXMultiplier();
	flag = ImGui::InputFloat("UvMultiple X", &fEffectUvFlowSizeMultiX);
	if (flag) {
		pEffect->SetEffectUvSizeXMultiplier(fEffectUvFlowSizeMultiX);
	}

	ImGui::PushItemWidth(200.f);
	_float fEffectUvFlowSizeMultiY = pEffect->GetEffectUvSizeYMultiplier();
	flag = ImGui::InputFloat("UvMultiple Y", &fEffectUvFlowSizeMultiY);
	if (flag) {
		pEffect->SetEffectUvSizeYMultiplier(fEffectUvFlowSizeMultiY);
	}

	ImGui::Text("");
	ImGui::Text("ETC Option");

	_bool fEffectGlow = pEffect->GetUseGlow();
	flag = ImGui::Checkbox("Effect Glow", &fEffectGlow);
	if (flag) {
		pEffect->SetUseGlow(fEffectGlow);
	}
	ImGui::SameLine();
	_bool fEffectDistortion = pEffect->GetUseDistortion();
	flag = ImGui::Checkbox("Effect Distortion", &fEffectDistortion);
	if (flag) {
		pEffect->SetUseDistortion(fEffectDistortion);
	}
	ImGui::SameLine();
	_bool fEffectTurn = pEffect->GetEffectIsTurn();
	flag = ImGui::Checkbox("Effect Turn", &fEffectTurn);
	if (flag) {
		pEffect->SetEffectIsTurn(fEffectTurn);
	}
	ImGui::PushItemWidth(200.f);
	_float fEffectTurnSpeed = pEffect->GetTurnSpeed();
	flag = ImGui::InputFloat("TurnSpeed", &fEffectTurnSpeed);
	if (flag) {
		pEffect->SetTurnSpeed(fEffectTurnSpeed);
	}

	if (static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectType() == CEffect_Base::EFFECT_PARTICLE) {
		ImGui::Text("");
		ImGui::Text("Particle Option");

		shared_ptr<CParticle_Effect> pParticleEffect = static_pointer_cast<CParticle_Effect>(m_pSelectObject);

		ImGui::PushItemWidth(200.f);
		_float3 fEffectPivot = pParticleEffect->Get_Pivot();
		flag = ImGui::InputFloat3("Pivot", &fEffectPivot.x);
		if (flag) {
			pParticleEffect->Set_Pivot(fEffectPivot);
		}

		ImGui::PushItemWidth(60.f);
		_float2 fEffectSpeed = pParticleEffect->Get_Speed();
		flag = ImGui::InputFloat("SpeedMin", &fEffectSpeed.x);
		if (flag) {
			if (fEffectSpeed.y > fEffectSpeed.x)
				pParticleEffect->Set_Speed(_float2(fEffectSpeed.x, fEffectSpeed.y));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("SpeedMax", &fEffectSpeed.y);
		if (flag) {
			if (fEffectSpeed.y > fEffectSpeed.x)
				pParticleEffect->Set_Speed(_float2(fEffectSpeed.x, fEffectSpeed.y));
		}

		ImGui::Text("");
		ImGui::Text("Particle Range");

		ImGui::PushItemWidth(40.f);
		_float3 fEffectRange = pParticleEffect->Get_Range();
		flag = ImGui::InputFloat("RangeX", &fEffectRange.x);
		if (flag) {
			pParticleEffect->Set_Range(_float3(fEffectRange.x, fEffectRange.y, fEffectRange.z));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("RangeY", &fEffectRange.y);
		if (flag) {
			pParticleEffect->Set_Range(_float3(fEffectRange.x, fEffectRange.y, fEffectRange.z));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("RangeZ", &fEffectRange.z);
		if (flag) {
			pParticleEffect->Set_Range(_float3(fEffectRange.x, fEffectRange.y, fEffectRange.z));
		}

		ImGui::PushItemWidth(60.f);
		_float2 fEffectScale = pParticleEffect->Get_Scale();
		flag = ImGui::InputFloat("ScaleMin", &fEffectScale.x);
		if (flag) {
			if (fEffectScale.y > fEffectScale.x)
				pParticleEffect->Set_Scale(_float2(fEffectScale.x, fEffectScale.y));
		}
		ImGui::SameLine();
		flag = ImGui::InputFloat("ScaleMax", &fEffectScale.y);
		if (flag) {
			if (fEffectScale.y > fEffectScale.x)
				pParticleEffect->Set_Scale(_float2(fEffectScale.x, fEffectScale.y));
		}



		ImGui::PushItemWidth(200.f);
		_bool fEffectRandomize = pParticleEffect->Get_IsRandomize();
		flag = ImGui::Checkbox("Effect Randomize", &fEffectRandomize);
		if (flag) {
			pParticleEffect->Set_IsRandomize(fEffectRandomize);
		}

		ImGui::Text("");
		ImGui::Text("Particle Style");

		ImGui::PushItemWidth(200.f);
		_int iStyleNum = static_cast<_int>(pParticleEffect->Get_Style());
		if (ImGui::BeginCombo("####", m_vecPtEffectStyleNames[iStyleNum].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecPtEffectStyleNames.size(); n++) {
				if (ImGui::Selectable(m_vecPtEffectStyleNames[n].data(), n == iStyleNum)) {
					pParticleEffect->Set_Style(static_cast<CVIBuffer_Instance::INSTANCE_STYLE>(n));
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

	}

	if (GAMEINSTANCE->Get_KeyDown(DIK_SPACE)) {
		if (GAMEINSTANCE->Get_KeyPressing(DIK_LCONTROL)) {
			for (_int i = 0; i < m_vecEffectNames.size(); ++i) {
				for (_int j = 0; j < m_mEffects[m_vecEffectNames[i]].size(); ++j)
					m_mEffects[m_vecEffectNames[i]][j]->SetEffectTimeFreeze(!(m_mEffects[m_vecEffectNames[i]][j]->GetEffectTimeFreeze()));
			}

		}
		else {
			pEffect->SetEffectTimeFreeze(!(pEffect->GetEffectTimeFreeze()));
		}//pEffect->SetEffectIsTurn(!(pEffect->GetEffectIsTurn()));
	}




}

void CImgui_Manager::Imgui_MenuBar()
{

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Map"))
			{
				fileDialog = ImGui::FileBrowser(0, "../Bin/DataFiles/Map/");
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MAP_LOAD;
			}

			if (ImGui::MenuItem("Save Map"))
			{
				fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory, "../Bin/DataFiles/Map/");
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MAP_SAVE;
				//Make_Map();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Load ModelNames"))
			{
				fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory, "../Bin/Resources/Models/Map/");
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MODELNAME_LOAD;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Load FModel Data"))
			{
				fileDialog = ImGui::FileBrowser(0, "../Bin/DataFiles/Map/json/");
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::JSON_LOAD;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}

void CImgui_Manager::Imgui_EffectMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Effect"))
			{
				fileDialog = ImGui::FileBrowser();
				fileDialog.Open();
				m_eEffectLoadType = eEffectLoadType::EFFECT_LOAD;
			}

			if (ImGui::MenuItem("Save Effect"))
			{
				fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
				fileDialog.Open();
				m_eEffectLoadType = eEffectLoadType::EFFECT_SAVE;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void CImgui_Manager::Imgui_ObjCreate()
{
	CTransform::TRANSFORM_DESC tDesc{};
	XMStoreFloat4(&tDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());

	/*LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	XMStoreFloat4(&LightDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());
	LightDesc.fRange = 20.f;
	strcpy_s(LightDesc.szName, "Light_Point");
	LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;*/

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	XMStoreFloat4(&LightDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());
	LightDesc.fRange = 20.f;
	strcpy_s(LightDesc.szName, "Light_Point");
	LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.g_fSpotInnerConeAngle = 0.f;
	LightDesc.g_fSpotOuterConeAngle = 0.7f;
	LightDesc.g_fLightFalloff = 0.01f;

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	staticDesc.fSpeedPerSec = 0.f;
	staticDesc.fRotationPerSec = 0.f;
	staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
	staticDesc.worldMatrix = Identity;
	staticDesc.worldMatrix._11 = 0.01f;
	staticDesc.worldMatrix._22 = 0.01f;
	staticDesc.worldMatrix._33 = 0.01f;

	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	animDesc.fSpeedPerSec = 0.f;
	animDesc.fRotationPerSec = 0.f;
	animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
	animDesc.worldMatrix = Identity;

	CGimmickBase::COMMONOBJ_DESC commonDesc{};
	commonDesc.fSpeedPerSec = 0.f;
	commonDesc.fRotationPerSec = 0.f;
	commonDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
	commonDesc.worldMatrix = Identity;

	MISCOBJ_DESC miscDesc{};
	miscDesc.bCollision = true;
	miscDesc.fRotationPerSec = 0.f;
	miscDesc.fSpeedPerSec = 0.f;
	miscDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	miscDesc.worldMatrix = Identity;

	CCullingBox::CULLING_DESC cullDesc{};
	cullDesc.fRotationPerSec = 0.f;
	cullDesc.fSpeedPerSec = 0.f;
	cullDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };

	_tchar strWcharBuffer[MAX_PATH] = {};

	switch (m_eObjectType)
	{
	case LIGHT: {
		m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&LightDesc));
		Imgui_SetSelectObject(m_vecObjects[LIGHT].back());
		break;
	}
	case TESTOBJECT: {
		m_vecObjects[TESTOBJECT].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), TEXT("Prototype_GameObject_TestObject"), &tDesc));
		Imgui_SetSelectObject(m_vecObjects[TESTOBJECT].back());
		break;
	}
	case MAPOBJ_STATIC: {
		staticDesc.vPosition = GAMEINSTANCE->Get_CamPosition_Float4();
		MultiByteToWideChar(CP_ACP, 0, m_vecModelTags[m_iCurrModelIndex].c_str(), (_int)m_vecModelTags[m_iCurrModelIndex].length(), strWcharBuffer, MAX_PATH);
		staticDesc.strModelTag = strWcharBuffer;
		m_vecObjects[MAPOBJ_STATIC].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Static::ObjID, &staticDesc));
		//static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC].back())->Set_ModelComponent(m_vecModelTags[m_iCurrModelIndex]);
		Imgui_SetSelectObject(m_vecObjects[MAPOBJ_STATIC].back());
		m_vecCloneNames[MAPOBJ_STATIC].emplace_back(m_vecModelTags[m_iCurrModelIndex]);
		break;
	}
	case MAPOBJ_ANIM: {
		animDesc.vPosition = GAMEINSTANCE->Get_CamPosition_Float4();
		MultiByteToWideChar(CP_ACP, 0, m_vecModelTags[m_iCurrModelIndex].c_str(), (_int)m_vecModelTags[m_iCurrModelIndex].length(), strWcharBuffer, MAX_PATH);
		animDesc.strModelTag = strWcharBuffer;
		m_vecObjects[MAPOBJ_ANIM].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Anim::ObjID, &animDesc));
		//static_pointer_cast<CMapObject_Anim>(m_vecObjects[MAPOBJ_ANIM].back())->Set_ModelComponent(m_vecModelTags[m_iCurrModelIndex]);
		Imgui_SetSelectObject(m_vecObjects[MAPOBJ_ANIM].back());
		m_vecCloneNames[MAPOBJ_ANIM].emplace_back(m_vecModelTags[m_iCurrModelIndex]);
		break;
	}
	case MAPOBJ_INTERACTIVE: {
		MultiByteToWideChar(CP_ACP, 0, m_strSelectObjectTag.c_str(), (_int)m_strSelectObjectTag.length(), strWcharBuffer, MAX_PATH);
		m_vecObjects[MAPOBJ_INTERACTIVE].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), wstring(strWcharBuffer), &commonDesc));
		Imgui_SetSelectObject(m_vecObjects[MAPOBJ_INTERACTIVE].back());
		m_vecCloneNames[MAPOBJ_INTERACTIVE].emplace_back(m_strSelectObjectTag);
		break;
	}
	case MAPOBJ_MISC: {
		MultiByteToWideChar(CP_ACP, 0, m_vecMiscTags[m_iCurrMiscIndex].c_str(), (_int)m_vecMiscTags[m_iCurrMiscIndex].length(), strWcharBuffer, MAX_PATH);
		m_vecObjects[MAPOBJ_MISC].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), wstring(strWcharBuffer), &miscDesc));
		Imgui_SetSelectObject(m_vecObjects[MAPOBJ_MISC].back());
		m_vecCloneNames[MAPOBJ_MISC].emplace_back(m_vecMiscTags[m_iCurrMiscIndex]);
		break;
	}
	case CULLINGBOX: {
		m_vecObjects[CULLINGBOX].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CCullingBox::ObjID, &cullDesc));
		Imgui_SetSelectObject(m_vecObjects[CULLINGBOX].back());
		string ObjID = string(CCullingBox::ObjID.begin(), CCullingBox::ObjID.end());
		m_vecCloneNames[CULLINGBOX].emplace_back(ObjID);
		break;
	}
	default:
		break;
	}
}

void CImgui_Manager::Imgui_EffectCreate()
{
	m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].push_back(static_pointer_cast<CEffect_Base>(GAMEINSTANCE->Add_Clone_Return(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), wstring().assign(m_vecEffectNames[m_strSelectEffectNum].begin(), m_vecEffectNames[m_strSelectEffectNum].end()))));
	Imgui_SetSelectObject(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back());
}

void CImgui_Manager::Imgui_EffectCopy()
{
	if (m_pSelectObject == nullptr) return;

	m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].push_back(static_pointer_cast<CEffect_Base>(GAMEINSTANCE->Add_Clone_Return(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), wstring().assign(m_vecEffectNames[m_strSelectEffectNum].begin(), m_vecEffectNames[m_strSelectEffectNum].end()))));
	
	shared_ptr<CEffect_Base> pEffect = m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back(); 
	pEffect->SetEffectTimeMin(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectTimeMin());
	pEffect->SetEffectTimeMax(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectTimeMax());
	pEffect->SetEffectTimeAccMultiplier(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectTimeAccMultiplier());

	pEffect->SetEffectBaseColorToColor(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectBaseColor());
	pEffect->SetEffectColorBlendRate(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectColorBlendRate());

	pEffect->SetColorPresetR(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetColorPresetR());
	pEffect->SetColorPresetG(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetColorPresetG());
	pEffect->SetColorPresetB(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetColorPresetB());

	pEffect->SetEffectLoop(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectLoop());

	pEffect->SetUseGlow(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetUseGlow());
	pEffect->SetUseDistortion(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetUseDistortion());

	pEffect->SetEffectIsTurn(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectIsTurn());

	pEffect->SetUseEffectUvFlowX(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetUseEffectUvFlowX());
	pEffect->SetUseEffectUvFlowY(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetUseEffectUvFlowY());

	pEffect->SetEffectUvFlow(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectUvFlow().x, static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectUvFlow().y);
	pEffect->SetEffectUvFlowXSpeed(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectUvFlowXSpeed());
	pEffect->SetEffectUvFlowYSpeed(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectUvFlowYSpeed());

	pEffect->SetEffectUvSizeXMultiplier(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectUvSizeXMultiplier());
	pEffect->SetEffectUvSizeYMultiplier(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectUvSizeYMultiplier());

	pEffect->SetEffectAlpha(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectAlpha());
	pEffect->SetEffectAlphaLinkTime(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectAlphaLinkTime());

	pEffect->SetTurnSpeed(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetTurnSpeed());

	pEffect->SetEffectType(static_pointer_cast<CEffect_Base>(m_pSelectObject)->GetEffectType());

	pEffect->Get_Transform()->Set_WorldMatrix(m_pSelectObject->Get_Transform()->Get_WorldMatrix());
	pEffect->SetRootMatrix(m_pSelectObject->Get_Transform()->Get_WorldMatrix());

	if (pEffect->GetEffectType() == CEffect_Base::EFFECT_PARTICLE)
	{
		shared_ptr<CParticle_Effect> pParticleEffect = static_pointer_cast<CParticle_Effect>(m_pSelectObject);
		shared_ptr<CParticle_Effect> pParticleEffectCopy = static_pointer_cast<CParticle_Effect>(pEffect);

		pParticleEffectCopy->Set_Pivot(pParticleEffect->Get_Pivot());
		pParticleEffectCopy->Set_Speed(pParticleEffect->Get_Speed());
		pParticleEffectCopy->Set_Range(pParticleEffect->Get_Range());
		pParticleEffectCopy->Set_Scale(pParticleEffect->Get_Scale());
		pParticleEffectCopy->Set_IsRandomize(pParticleEffect->Get_IsRandomize());
		pParticleEffectCopy->Set_Style(pParticleEffect->Get_Style());
	}

	Imgui_SetSelectObject(m_mEffects[m_vecEffectNames[m_strSelectEffectNum]].back());
}

void CImgui_Manager::Imgui_MapDataControl()
{
	switch (m_eMapLoadType)
	{
	case eMapLoadType::MAP_LOAD:
		Load_Map();
		break;
	case eMapLoadType::MAP_SAVE:
		Save_Map();
		break;
	case eMapLoadType::JSON_LOAD:
		Load_FModel();
		break;
	case eMapLoadType::SECTION_LOAD:
		Load_SectionCulling();
		break;
	case eMapLoadType::SECTION_SAVE:
		Save_SectionCulling();
		break;
	case eMapLoadType::MAP_END:
		break;
	default:
		break;
	}
}

void CImgui_Manager::Imgui_EffectDataControl()
{
	switch (m_eEffectLoadType)
	{
	case eEffectLoadType::EFFECT_LOAD:
		Load_Effect();
		break;
	case eEffectLoadType::EFFECT_SAVE:
		Save_Effect();
		break;
	case eEffectLoadType::EFFECT_END:
		break;
	default:
		break;
	}
}

void CImgui_Manager::Load_ModelNames(wstring strFilePath)
{
	//m_vecModelTags.clear();
	m_iCurrModelIndex = 0;

	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	wstring strTargetPath = strFilePath + L"*.*";

	WIN32_FIND_DATA	fd;
	HANDLE	hFind = FindFirstFile(strTargetPath.c_str(), &fd);

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
				wstring strChildPath = strFilePath + fd.cFileName + L"\\";

				Load_ModelNames(strChildPath);
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

				if (strFileName.find(L"_Lod") != strFileName.npos)
					continue;

				_char szStringPath[MAX_PATH] = {};
				WideCharToMultiByte(CP_ACP, 0, szFileName, MAX_PATH, szStringPath, MAX_PATH, NULL, NULL);

				m_vecModelTags.emplace_back(szStringPath);
				//wcscpy_s(szFullPath, strFilePath.c_str());
				//wcscat_s(szFullPath, strFileName.c_str());

				//size_t index = strFilePath.find_last_of(L'/');
				//wstring strSectionName = strFilePath.substr(0, index);
				//index = strSectionName.find_last_of(L'/');
				//strSectionName = strSectionName.substr(index + 1, strSectionName.length());
				//
				//wstring strPrototypeTag = L"Prototype_Component_Model_" + strSectionName;

				//m_List_ResourceInfo.push_back(
				//	make_shared<RES_MODEL>(strFileName, wstring(szFullPath), MODEL_TYPE::NONANIM, FMat)
				//);

			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}

void CImgui_Manager::Load_FModel()
{
	//Clear_Objects();
	for (size_t i = 0; i < OBJTYPE_END; i++)
		m_vecMapObjData[i].clear();

	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();
	//szBinFilePath += "\\";
	//szBinFilePath += m_strMapName;
	//szBinFilePath += ".json";

	ifstream MapData(szBinFilePath);
	json importer = json::parse(MapData);
	MapData.close();

	for (auto& object : importer)
	{
		if (!object.contains("Properties"))
			continue;

		MapObjData tObjData;
		string strObjName;


		if (object["Type"] == "SceneComponent")
		{
			string strBPName = object["Outer"];

			if (strBPName.find("bg") == strBPName.npos && strBPName.find("SM_") == strBPName.npos)
				continue;

			auto index = strBPName.find("_BP");
			if (index != strBPName.npos)
				strBPName.erase(strBPName.begin() + index, strBPName.end());
			else
				continue;

			if (find(m_vecModelTags.begin(), m_vecModelTags.end(), strBPName) == m_vecModelTags.end())
			{
				cout << "No BluePrintModel : " + strBPName << endl;
				continue;
			}

			tObjData.iObjType = (_uint)MAPOBJ_STATIC;
			strObjName = strBPName;
		}

		else if (object["Properties"].contains("StaticMesh"))
		{
			tObjData.iObjType = (_uint)MAPOBJ_STATIC;

			if (object["Properties"]["StaticMesh"].is_null())
				continue;

			strObjName = object["Properties"]["StaticMesh"]["ObjectName"];
			auto index = strObjName.find("StaticMesh");
			if (index != strObjName.npos)
				strObjName.erase(index, strlen("StaticMesh"));
			index = strObjName.find("\'");
			while (index != strObjName.npos)
			{
				strObjName.erase(index, 1);
				index = strObjName.find("\'");
			}

			if (find(m_vecModelTags.begin(), m_vecModelTags.end(), strObjName) == m_vecModelTags.end())
			{
				cout << "No Model : " + strObjName << endl;
				continue;
			}
		}

		else
			continue;

		tObjData.tLightDesc = {};
		strcpy_s(tObjData.szPrototypeTag, strObjName.c_str());
		_float3 vPosition{ 0.f, 0.f, 0.f }, vRotation{ 0.f, 0.f, 0.f }, vScale{ 1.f, 1.f, 1.f };

		if (object["Properties"].contains("RelativeLocation"))
		{
			auto vLocation = object["Properties"]["RelativeLocation"];
			vPosition.x = vLocation["X"] * +0.01f;
			vPosition.y = vLocation["Z"] * +0.01f;
			vPosition.z = vLocation["Y"] * -0.01f;
		}
		else
			vPosition = { 0.f, 0.f, 0.f };

		if (object["Properties"].contains("RelativeRotation"))
		{
			auto vRot = object["Properties"]["RelativeRotation"];
			//vRotation.x = XMConvertToRadians(vRot["Roll"]) * -1.f;
			//vRotation.y = XMConvertToRadians(vRot["Yaw"]);
			//vRotation.z = XMConvertToRadians(vRot["Pitch"]);
			vRotation.x = (vRot["Roll"]) * -1.f;
			vRotation.y = (vRot["Yaw"]);
			vRotation.z = (vRot["Pitch"]);
		}
		else
			vRotation = { 0.f, 0.f, 0.f };

		if (object["Properties"].contains("RelativeScale3D"))
		{
			auto vScl = object["Properties"]["RelativeScale3D"];
			vScale.x = max(vScl["X"] * 0.01f, 0.005f);
			vScale.y = max(vScl["Z"] * 0.01f, 0.005f);
			vScale.z = max(vScl["Y"] * 0.01f, 0.005f);
		}
		else
			vScale = { 0.01f, 0.01f, 0.01f };

		//_vector vQuatRot = XMVectorZero();
		//vQuatRot = XMQuaternionRotationRollPitchYaw(vRotation.x, vRotation.y, vRotation.z);
		////vQuatRot = RotZ * RotY * RotX;
		//_matrix MatWorld = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuatRot, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

		
		ImGuizmo::RecomposeMatrixFromComponents(&vPosition.x, &vRotation.x, &vScale.x, &tObjData.matWorld._11);
		//XMStoreFloat4x4(&tObjData.matWorld, MatWorld);

		// ??????? ?????? ????
		m_vecMapObjData[tObjData.iObjType].emplace_back(tObjData);
	}

	Make_Map();

	//ofstream Output;
	//Output.open("../bin/DataFiles/Map/Output.json");
	//
	//for (auto ObjData : m_vecMapObjData)
	//{
	//	json j;
	//	j["ModelName"] = ObjData.szPrototypeTag;
	//
	//	Output << j << endl;
	//}
	//
	//int a = 0;
}

void CImgui_Manager::Load_MapObj_Interactive(const string& strFilePath)
{
	std::ifstream gameObjIs(strFilePath, std::ios::binary);

	if (gameObjIs.is_open())
	{
		size_t gimmickSize = {};
		read_typed_data(gameObjIs, gimmickSize);

		for (size_t i = 0; i < gimmickSize; i++)
		{
			_tchar strTextBuffer[MAX_PATH] = {};
			CGimmickBase::COMMONOBJ_DESC commonDesc{};

			NewMapObjData data;
			read_typed_data(gameObjIs, data);
			commonDesc.worldMatrix = data.matWorld;

			MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
			m_vecObjects[MAPOBJ_INTERACTIVE].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), wstring(strTextBuffer), &commonDesc));
			//m_vecObjects[MAPOBJ_INTERACTIVE].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			dynamic_pointer_cast<CGimmickBase>(m_vecObjects[MAPOBJ_INTERACTIVE].back())->Load_Data(gameObjIs);
			m_vecCloneNames[MAPOBJ_INTERACTIVE].emplace_back(data.szPrototypeTag);
			CGimmickManager::Get_Instance()->Add_GimmickObj(dynamic_pointer_cast<CGimmickBase>(m_vecObjects[MAPOBJ_INTERACTIVE].back()));
		}

		size_t miscSize = {};
		read_typed_data(gameObjIs, miscSize);

		for (size_t i = 0; i < miscSize; i++)
		{
			_tchar strTextBuffer[MAX_PATH] = {};
			MISCOBJ_DESC miscDesc{};

			NewMapObjData data;
			read_typed_data(gameObjIs, data);
			miscDesc.worldMatrix = data.matWorld;

			MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
			m_vecObjects[MAPOBJ_MISC].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), wstring(strTextBuffer), &miscDesc));
			//m_vecObjects[MAPOBJ_INTERACTIVE].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			auto Casted = dynamic_pointer_cast<CMisc_Interface>(m_vecObjects[MAPOBJ_MISC].back());
			if(Casted)
				Casted->Load_Data(gameObjIs);
			m_vecCloneNames[MAPOBJ_MISC].emplace_back(data.szPrototypeTag);
		}

		if (gameObjIs.peek() != EOF)
		{
			size_t boxSize = {};
			read_typed_data(gameObjIs, boxSize);

			for (size_t i = 0; i < boxSize; i++)
			{
				CCullingBox::CULLING_DESC cullBoxDesc{};

				NewMapObjData data;
				read_typed_data(gameObjIs, data);
				cullBoxDesc.worldMat = data.matWorld;
				cullBoxDesc.bBoarder = data.bCollision;

				m_vecObjects[CULLINGBOX].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CCullingBox::ObjID, &cullBoxDesc));
				m_vecCloneNames[CULLINGBOX].emplace_back(data.szPrototypeTag);
			}
		}
	}
	else
		gameObjIs.close();

	for (auto pObject : m_vecObjects[MAPOBJ_INTERACTIVE])
		dynamic_pointer_cast<CGimmickBase>(pObject)->Link_Relatives();
}

//void CImgui_Manager::Level_MapObjLoad(_uint iMapNum, const string& strFilePath)
//{
//	std::ifstream gameObjIs(strFilePath, std::ios::binary);
//
//	vector<shared_ptr<CGimmickBase>> vecObjects;
//
//	if (gameObjIs.is_open())
//	{
//		size_t gimmickSize = {};
//		read_typed_data(gameObjIs, gimmickSize);
//
//		for (size_t i = 0; i < gimmickSize; i++)
//		{
//			_tchar strTextBuffer[MAX_PATH] = {};
//			CGimmickBase::COMMONOBJ_DESC commonDesc{};
//
//			MapObjData data;
//			read_typed_data(gameObjIs, data);
//			commonDesc.worldMatrix = data.matWorld;
//
//			MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
//			shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(wstring(strTextBuffer), &commonDesc);
//			shared_ptr<CGimmickBase> pCasted = dynamic_pointer_cast<CGimmickBase>(pGameObject);
//			pCasted->Load_Data(gameObjIs);
//			m_setPrototypeTag.emplace(wstring(strTextBuffer));
//			CGimmickManager::Get_Instance()->Add_GimmickObj(pCasted);
//			vecObjects.emplace_back(pCasted);
//		}
//
//		size_t miscSize = {};
//		read_typed_data(gameObjIs, miscSize);
//
//		for (size_t i = 0; i < miscSize; i++)
//		{
//			_tchar strTextBuffer[MAX_PATH] = {};
//			MISCOBJ_DESC miscDesc{};
//
//			MapObjData data;
//			read_typed_data(gameObjIs, data);
//			miscDesc.worldMatrix = data.matWorld;
//
//			MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
//			shared_ptr<CGameObject> pGameObject = GAMEINSTANCE->Pool_Object_Return(wstring(strTextBuffer), &miscDesc);
//			shared_ptr<CMisc_Interface> pCasted = dynamic_pointer_cast<CMisc_Interface>(pGameObject);
//			pCasted->Load_Data(gameObjIs);
//			m_setPrototypeTag.emplace(wstring(strTextBuffer));
//		}
//	}
//	else
//		gameObjIs.close();
//
//	for (auto pObject : vecObjects)
//		pObject->Link_Relatives();
//}

void CImgui_Manager::Pick_Object()
{
	if (Imgui_GetState() == eImgui_State::IMGUI_STATE_WAIT) return;

	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;

	auto MousePos = io.MousePos;

	shared_ptr<CGameObject> pPicked = GAMEINSTANCE->Pick_Object((_uint)MousePos.x, (_uint)MousePos.y);

	if (pPicked != nullptr)
	{
		m_pSelectObject = pPicked;

		_bool bPicked = false;
		
		for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i)
		{
			if (m_vecObjects[m_eObjectType][i].get()->Get_ObjectID() == m_pSelectObject.get()->Get_ObjectID())
			{
				m_iCurrCloneIndex = i;
				bPicked = true;
				break;
			}
		}
	}
}

void CImgui_Manager::Clear_Objects()
{
	for (_uint i = 0; i < OBJTYPE_END; ++i) {
		for (_uint j = 0; j < m_vecObjects[i].size(); ++j)
			m_vecObjects[i][j]->Dead();

		m_vecObjects[i].clear();
	}

	for (_uint i = 0; i < OBJTYPE_END; ++i) {
		m_vecCloneNames[i].clear();
	}

	m_iCurrCloneIndex = 0;
	m_iCurrCullBoxIndex = 0;
	m_iCurrGimmickIndex = 0;
	m_iCurrInteractiveIndex = 0;
	m_iCurrMiscIndex = 0;
	m_iCurrRelativeIndex = 0;
}

void CImgui_Manager::Tool_Change()
{
	if (!m_bCanChange)
		return;

	if (GAMEINSTANCE->Get_KeyDown(DIK_F1)) {
		if (m_eToolType == eToolType::TOOL_END) {
			m_eToolType = eToolType::TOOL_MAP;
			return;
		}
		m_eToolType = static_cast<eToolType>(static_cast<int>(m_eToolType) + 1);
	}

	if (GAMEINSTANCE->Get_KeyPressing(DIK_LSHIFT))
	{
		if (GAMEINSTANCE->Get_KeyDown(DIK_1)) m_eToolType = eToolType::TOOL_MAP;
		if (GAMEINSTANCE->Get_KeyDown(DIK_2)) m_eToolType = eToolType::TOOL_EFFECT;
		if (GAMEINSTANCE->Get_KeyDown(DIK_3)) m_eToolType = eToolType::TOOL_SOUND;
		if (GAMEINSTANCE->Get_KeyDown(DIK_4)) m_eToolType = eToolType::TOOL_ANIMATION;
		if (GAMEINSTANCE->Get_KeyDown(DIK_5)) m_eToolType = eToolType::TOOL_CINE;
	}

}

void CImgui_Manager::Make_Map()
{
	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	CGimmickBase::COMMONOBJ_DESC commonDesc{};
	MISCOBJ_DESC miscDesc{};
	//staticDesc.worldMatrix = Identity;
	//animDesc.worldMatrix = Identity;

	_tchar strTextBuffer[MAX_PATH] = {};

	for (_uint e = 0; e < OBJTYPE_END; ++e)
	{
		for (_uint i = 0; i < m_vecMapObjData[e].size(); i++) {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));

			switch (m_vecMapObjData[e][i].iObjType)
			{
			case LIGHT:
				m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&m_vecMapObjData[e][i].tLightDesc));
				m_vecObjects[LIGHT].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[e][i].matWorld);
				break;
			case TESTOBJECT:
				m_vecObjects[TESTOBJECT].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), TEXT("Prototype_GameObject_TestObject")));
				m_vecObjects[TESTOBJECT].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[e][i].matWorld);
				break;
			case MAPOBJ_STATIC:
				staticDesc.fSpeedPerSec = 0.f;
				staticDesc.fRotationPerSec = 0.f;
				staticDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
				MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[e][i].szPrototypeTag, (_int)strlen(m_vecMapObjData[e][i].szPrototypeTag), strTextBuffer, MAX_PATH);
				staticDesc.strModelTag = wstring(strTextBuffer);
				staticDesc.bCollision = m_vecMapObjData[e][i].bCollision;
				staticDesc.worldMatrix = m_vecMapObjData[e][i].matWorld;
				m_vecObjects[MAPOBJ_STATIC].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Static::ObjID, &staticDesc));
				//m_vecObjects[MAPOBJ_STATIC].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
				m_vecCloneNames[MAPOBJ_STATIC].emplace_back(m_vecMapObjData[e][i].szPrototypeTag);
				break;
			case MAPOBJ_ANIM:
				animDesc.fSpeedPerSec = 0.f;
				animDesc.fRotationPerSec = 0.f;
				animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
				MultiByteToWideChar(CP_ACP, 0, m_vecMapObjData[e][i].szPrototypeTag, (_int)strlen(m_vecMapObjData[e][i].szPrototypeTag), strTextBuffer, MAX_PATH);
				animDesc.strModelTag = wstring(strTextBuffer);
				animDesc.worldMatrix = m_vecMapObjData[e][i].matWorld;
				m_vecObjects[MAPOBJ_ANIM].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Anim::ObjID, &animDesc));
				//m_vecObjects[MAPOBJ_ANIM].back()->Get_Transform()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
				m_vecCloneNames[MAPOBJ_ANIM].emplace_back(m_vecMapObjData[e][i].szPrototypeTag);
				break;
			default:
				break;
			}
		}

		m_vecMapObjData[e].clear();
	}	
}

void CImgui_Manager::Save_Map()
{
	for (_uint i = 0; i < OBJTYPE_END; ++i)
	{
		for (_uint j = 0; j < m_vecObjects[i].size(); ++j)
		{
			if (i == MAPOBJ_STATIC) {
				NewMapObjData tData;
				tData.iObjType = i;
				tData.bCollision = static_pointer_cast<CMapObject_Static>(m_vecObjects[MAPOBJ_STATIC][j])->Get_Collision();
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[MAPOBJ_STATIC][j].c_str());

				m_vecNewMapObjData[i].push_back(tData);
			}
			else if (i == MAPOBJ_MISC) {
				NewMapObjData tData;
				tData.iObjType = i;
				tData.bCollision = true;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[MAPOBJ_MISC][j].c_str());

				m_vecNewMapObjData[i].push_back(tData);
			}
			else if (i == MAPOBJ_ANIM) {
				NewMapObjData tData;
				tData.iObjType = i;
				tData.bCollision = true;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[MAPOBJ_ANIM][j].c_str());

				m_vecNewMapObjData[i].push_back(tData);
			}
			else if (i == MAPOBJ_INTERACTIVE) {
				NewMapObjData tData;
				tData.iObjType = i;
				tData.bCollision = true;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[MAPOBJ_INTERACTIVE][j].c_str());

				m_vecNewMapObjData[i].push_back(tData);
			}
			else if (i == CULLINGBOX)
			{
				NewMapObjData tData;
				tData.iObjType = i;
				tData.bCollision = static_pointer_cast<CCullingBox>(m_vecObjects[CULLINGBOX][j])->Is_Boarder();
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = {};
				strcpy_s(tData.szPrototypeTag, m_vecCloneNames[CULLINGBOX][j].c_str());

				m_vecNewMapObjData[i].push_back(tData);
			}
			else if (i == LIGHT) {
				NewMapObjData tData;
				tData.iObjType = i;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldMatrix();
				tData.tLightDesc = *(static_pointer_cast<CLight>(m_vecObjects[i][j])->Get_LightDesc());

				m_vecNewMapObjData[i].push_back(tData);
			}
		}
	}

	string szBinFilePath;
	_char szCommonDataFilePath[MAX_PATH] = {};

	szBinFilePath += fileDialog.GetSelected().string();
	szBinFilePath += "\\";
	//szBinFilePath += m_strMapName;
	//szBinFilePath += ".bin";

	strcat_s(szCommonDataFilePath, szBinFilePath.c_str());
	strcat_s(szCommonDataFilePath, m_strMapName.c_str());
	strcat_s(szCommonDataFilePath, ".bin");

	_char szGameObjectDataFilePath[MAX_PATH] = {};
	strcat_s(szGameObjectDataFilePath, szBinFilePath.c_str());
	strcat_s(szGameObjectDataFilePath, m_strMapName.c_str());
	strcat_s(szGameObjectDataFilePath, "_GameObject.bin");

	std::ofstream os(szCommonDataFilePath, std::ios::binary);
	std::ofstream gameObjOs(szGameObjectDataFilePath, std::ios::binary);

#ifdef _DEBUG

	assert(os.is_open());
	assert(gameObjOs.is_open());

#endif // _DEBUG

	write_typed_data(os, m_vecNewMapObjData[LIGHT].size());
	for (int i = 0; i < m_vecNewMapObjData[LIGHT].size(); ++i)
	{
		write_typed_data(os, m_vecNewMapObjData[LIGHT][i]);
	}
	m_vecNewMapObjData[LIGHT].clear();

	write_typed_data(os, m_vecNewMapObjData[MAPOBJ_STATIC].size());
	for (int i = 0; i < m_vecNewMapObjData[MAPOBJ_STATIC].size(); ++i)
	{
		write_typed_data(os, m_vecNewMapObjData[MAPOBJ_STATIC][i]);
		dynamic_pointer_cast<CMisc_Interface>(m_vecObjects[MAPOBJ_STATIC][i])->Save_Data(os);
	}
	m_vecNewMapObjData[MAPOBJ_STATIC].clear();

	write_typed_data(os, m_vecNewMapObjData[MAPOBJ_ANIM].size());
	for (int i = 0; i < m_vecNewMapObjData[MAPOBJ_ANIM].size(); ++i)
	{
		write_typed_data(os, m_vecNewMapObjData[MAPOBJ_ANIM][i]);
	}
	m_vecNewMapObjData[MAPOBJ_ANIM].clear();

	os.close();

	write_typed_data(gameObjOs, m_vecNewMapObjData[MAPOBJ_INTERACTIVE].size());
	for (int i = 0; i < m_vecNewMapObjData[MAPOBJ_INTERACTIVE].size(); ++i)
	{
		write_typed_data(gameObjOs, m_vecNewMapObjData[MAPOBJ_INTERACTIVE][i]);
		dynamic_pointer_cast<CGimmickBase>(m_vecObjects[MAPOBJ_INTERACTIVE][i])->Save_Data(gameObjOs);
	}
	m_vecNewMapObjData[MAPOBJ_INTERACTIVE].clear();

	write_typed_data(gameObjOs, m_vecNewMapObjData[MAPOBJ_MISC].size());
	for (int i = 0; i < m_vecNewMapObjData[MAPOBJ_MISC].size(); ++i)
	{
		write_typed_data(gameObjOs, m_vecNewMapObjData[MAPOBJ_MISC][i]);
		auto pCasted = dynamic_pointer_cast<CMisc_Interface>(m_vecObjects[MAPOBJ_MISC][i]);
		if(pCasted)
			pCasted->Save_Data(gameObjOs);
	}
	m_vecNewMapObjData[MAPOBJ_MISC].clear();

	write_typed_data(gameObjOs, m_vecNewMapObjData[CULLINGBOX].size());
	for (_uint i = 0; i < m_vecNewMapObjData[CULLINGBOX].size(); ++i)
		write_typed_data(gameObjOs, m_vecNewMapObjData[CULLINGBOX][i]);
	m_vecNewMapObjData[CULLINGBOX].clear();

	gameObjOs.close();
}

void CImgui_Manager::Load_Map()
{
	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();

	cout << "Load: " << szBinFilePath << endl;
	string strGameObjFilePath;
	_char szGameObjFilePath[MAX_PATH] = {};
	auto ext = szBinFilePath.find('.');
	if (ext != szBinFilePath.npos)
	{
		strGameObjFilePath = string(szBinFilePath.begin(), szBinFilePath.begin() + ext);
		strcat_s(szGameObjFilePath, strGameObjFilePath.c_str());
		strcat_s(szGameObjFilePath, "_GameObject.bin");
	}

	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG

	assert(is.is_open());

#endif // _DEBUG

	//while (is.peek() != EOF) {
	//	MapObjData data;
	//	read_typed_data(is, data);
	//	m_vecMapObjData[data.iObjType].push_back(data);
	//}

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	CMapObject_Anim::ANIMOBJ_DESC animDesc{};
	CGimmickBase::COMMONOBJ_DESC commonDesc{};
	MISCOBJ_DESC miscDesc{};
	//staticDesc.worldMatrix = Identity;
	//animDesc.worldMatrix = Identity;

	_tchar strTextBuffer[MAX_PATH] = {};

	for (_uint e = 0; e < OBJTYPE_END; ++e)
	{
		size_t iSize = {};
		read_typed_data(is, iSize);

		for (_uint i = 0; i < iSize; i++) {
			ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
			NewMapObjData data;
			read_typed_data(is, data);

			switch (data.iObjType)
			{
			case LIGHT:
				m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&data.tLightDesc));
				m_vecObjects[LIGHT].back()->Get_Transform()->Set_WorldMatrix(data.matWorld);
				break;
			case TESTOBJECT:
				m_vecObjects[TESTOBJECT].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), TEXT("Prototype_GameObject_TestObject")));
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
				auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Static::ObjID, &staticDesc);
				auto pCasted = dynamic_pointer_cast<CMisc_Interface>(pObject);
				if (pCasted)
					pCasted->Load_Data(is);
				m_vecObjects[MAPOBJ_STATIC].push_back(pObject);
				m_vecCloneNames[MAPOBJ_STATIC].emplace_back(data.szPrototypeTag);
				break;
			}
			case MAPOBJ_ANIM:
				animDesc.fSpeedPerSec = 0.f;
				animDesc.fRotationPerSec = 0.f;
				animDesc.vPosition = _float4{ 0.f, 0.f, 0.f, 1.f };
				MultiByteToWideChar(CP_ACP, 0, data.szPrototypeTag, (_int)strlen(data.szPrototypeTag), strTextBuffer, MAX_PATH);
				animDesc.strModelTag = wstring(strTextBuffer);
				animDesc.worldMatrix = data.matWorld;
				m_vecObjects[MAPOBJ_ANIM].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CMapObject_Anim::ObjID, &animDesc));
				m_vecCloneNames[MAPOBJ_ANIM].emplace_back(data.szPrototypeTag);
				break;
			default:
				break;
			}
		}

		//m_vecMapObjData[e].clear();
	}


	//Make_Map();

	is.close();

	Load_MapObj_Interactive(szGameObjFilePath);
}

void CImgui_Manager::Save_SectionCulling()
{
	string szBinFilePath;
	_char szDataFilePath[MAX_PATH] = {};

	szBinFilePath += fileDialog.GetSelected().string();
	szBinFilePath += "\\";

	strcat_s(szDataFilePath, szBinFilePath.c_str());
	strcat_s(szDataFilePath, m_strSectionFileName.c_str());
	strcat_s(szDataFilePath, ".bin");

	std::ofstream os(szDataFilePath, std::ios::binary);

#ifdef _DEBUG
	assert(os.is_open());
#endif // _DEBUG

	vector<NewMapObjData> vecObjData;

	for (size_t i = 0; i < m_vecSectionCulling.size(); ++i)
	{
		NewMapObjData tData{};
		tData.iObjType = OBJTYPE_END;
		tData.matWorld = m_vecSectionCulling[i]->Get_Transform()->Get_WorldMatrix();
		strcpy_s(tData.szPrototypeTag, "CSectionCullingBox");

		vecObjData.push_back(tData);
	}

	write_typed_data(os, vecObjData.size());
	for (int i = 0; i < vecObjData.size(); ++i)
	{
		write_typed_data(os, vecObjData[i]);
		m_vecSectionCulling[i]->Save_Data(os);
	}
	vecObjData.clear();

	os.close();
}

void CImgui_Manager::Load_SectionCulling()
{
	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();

	cout << "Load: " << szBinFilePath << endl;
	
	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	assert(is.is_open());
#endif // _DEBUG

	CSectionCullingBox::SECTIONCULLING_DESC sectionDesc{};
	sectionDesc.fSpeedPerSec = 0.f;
	sectionDesc.fRotationPerSec = 0.f;
	sectionDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };

	_tchar strTextBuffer[MAX_PATH] = {};

	size_t size{};
	read_typed_data(is, size);
	for (_uint i = 0; i < size; i++) {
		ZeroMemory(strTextBuffer, sizeof(_tchar) * wcslen(strTextBuffer));
		NewMapObjData data;
		read_typed_data(is, data);

		sectionDesc.bBoarder = data.bCollision;
		sectionDesc.worldMat = data.matWorld;
		
		auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("TestObject"), CSectionCullingBox::ObjID, &sectionDesc);
		auto pCasted = dynamic_pointer_cast<CSectionCullingBox>(pObject);
		if (pCasted)
			pCasted->Load_Data(is);
		m_vecSectionCulling.push_back(pCasted);
	}

	is.close();
}

void CImgui_Manager::Make_EffectData(UNION_EFFECT_DATA _sUnionData)
{
	m_strUnionEffectKey = _sUnionData.szUnionEffectName;


	EffectUnLink();
	for (auto& Effect : m_mEffects)
	{
		Effect.second.clear();
	}

	for (_uint i = 0; i < _sUnionData.iNumEffectDatas; ++i)
	{
		shared_ptr<CEffect_Base> pEffect = static_pointer_cast<CEffect_Base>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), TEXT("Layer_Effect"), wstring().assign(_sUnionData.vEffectDatas[i]->szPrototypeTag.begin(), _sUnionData.vEffectDatas[i]->szPrototypeTag.end())));
		_sUnionData.Make_UnionEffectData(pEffect, i);

		m_mEffects[_sUnionData.vEffectDatas[i]->szPrototypeTag].push_back(pEffect);
		pEffect->EffectEnd();
	}

	EffectLink();
}

void CImgui_Manager::Save_EffectData()
{
	UNION_EFFECT_DATA sUnionData;

	_char szUnionEffectName[MAX_PATH];
	strcpy(szUnionEffectName, m_strUnionEffectKey.c_str());
	sUnionData.szUnionEffectName = szUnionEffectName;

	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			shared_ptr<EFFECT_DATA> sEffectData = make_shared<EFFECT_DATA>();
			sEffectData->Make_EffectDatas(pEffect, Effect.first);

			(sUnionData.vEffectDatas).push_back(sEffectData);
		}
	}

	sUnionData.iNumEffectDatas = static_cast<_uint>((sUnionData.vEffectDatas).size());

	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();
	szBinFilePath += "\\";
	szBinFilePath += sUnionData.szUnionEffectName;
	szBinFilePath += ".bin";

	std::ofstream os(szBinFilePath, std::ios::binary);

#ifdef _DEBUG

	assert(os.is_open());
#endif // _DEBUG

	sUnionData.Bake_Binary(os);
}

void CImgui_Manager::Load_EffectData()
{
	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();

	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	//Failed To File Open
	assert(is.is_open());
#endif // _DEBUG

	UNION_EFFECT_DATA sUnionData;
	sUnionData.Load_FromBinary(is);

	is.close();

	Make_EffectData(sUnionData);
}

void CImgui_Manager::Init_Effect_Prototype()
{
	auto& Tags = CEffect_Base::GetProtoTag();
	for (auto& Tag : Tags)
	{
		m_vecEffectNames.push_back(string(Tag.begin(), Tag.end()));
	}

	for (auto& Tag : m_vecEffectNames)
	{
		m_mEffects.insert(make_pair(Tag, vector<shared_ptr<CEffect_Base>>()));
	}
}

void CImgui_Manager::Make_Effect()
{
}

void CImgui_Manager::Save_Effect()
{
	Save_EffectData();
}

void CImgui_Manager::Load_Effect()
{
	Load_EffectData();
}

void CImgui_Manager::EffectLink()
{
	_float fTimeMaxCheck = 0.f;
	m_bIsUnionLink = true;
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->Set_EffectLink(true);
			pEffect->Set_EffectLinkTime(&m_fUnionLinkTime);
			pEffect->EffectEnd();
			pEffect->Set_PlayOnce(false);
			if (pEffect->GetEffectTimeMax() > fTimeMaxCheck) {
				fTimeMaxCheck = pEffect->GetEffectTimeMax();
				m_fUnionLinkTimeMax = fTimeMaxCheck;
			}
		}
	}
}

void CImgui_Manager::EffectUnLink()
{
	m_bIsUnionLink = false;
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->Set_EffectLink(false);
			pEffect->Set_EffectLinkTime(nullptr);
			m_fUnionLinkTimeMax = 0.f;
		}
	}
}

void CImgui_Manager::EffectAnchor()
{
	if (GAMEINSTANCE->Get_KeyPressing(DIK_LCONTROL)) {

		if (m_pSelectObject == nullptr)
			return;

		for (auto& Effect : m_mEffects)
		{
			for (auto& pEffect : Effect.second)
			{
				_float3 vScale = pEffect->Get_Transform()->Get_Scale();
				pEffect->Get_Transform()->Set_WorldMatrix(m_pSelectObject->Get_Transform()->Get_WorldMatrix());
				pEffect->Get_Transform()->Set_Scale(vScale);
				pEffect->SetRootMatrix(pEffect->Get_Transform()->Get_WorldMatrix());
			}
		}
	}
}

void CImgui_Manager::EffectPause()
{
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->Set_Pause(!pEffect->Get_Pause());
		}
	}
}

void CImgui_Manager::UnionStart()
{
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->SetEffectTimeAcc(pEffect->GetEffectTimeMin());
		}
	}
}

void CImgui_Manager::UnionEnd()
{
	for (auto& Effect : m_mEffects)
	{
		for (auto& pEffect : Effect.second)
		{
			pEffect->Set_Pause(false);
			pEffect->EffectEnd();
			pEffect->Set_PlayOnce(false);
		}
	}
}

void CImgui_Manager::MapDelete_CtrlZ()
{
	if (m_ObjectBackUpVec[m_eObjectType].size() != 0)
	{	
		shared_ptr<CGameObject> Obj = m_ObjectBackUpVec[m_eObjectType].back();
		string Name = m_CloneNamesBackupVec[m_eObjectType].back();	
		Obj->Set_Dead(false);
		GAMEINSTANCE->Push_Object(GAMEMANAGER->Get_Current_Level(), L"CGameObject", Obj);
		m_vecObjects[m_eObjectType].emplace_back(Obj);
		m_vecCloneNames[m_eObjectType].emplace_back(Name);
		m_ObjectBackUpVec[m_eObjectType].pop_back();
		m_CloneNamesBackupVec[m_eObjectType].pop_back();
	}
	else
	{
	}
}

void CImgui_Manager::BackUp_MapDelete(shared_ptr<CGameObject> pObj, string Name)
{
	pObj->Dead();
	m_ObjectBackUpVec[m_eObjectType].emplace_back(pObj);
	m_CloneNamesBackupVec[m_eObjectType].emplace_back(Name);
}

void CImgui_Manager::Initialize_BackupVec()
{
	for (_uint i = 0; i < OBJTYPE_END; i++)
	{
		m_ObjectBackUpVec[i].reserve(100);
		m_CloneNamesBackupVec[i].reserve(100);
	}
}

void CImgui_Manager::Clear_BackUpData()
{
	cout << "clear" << endl;
	for (size_t i = 0; i < m_eObjectType; i++)
	{
		for (auto& iter : m_ObjectBackUpVec[i])
		{
			iter->Dead();
		}
		m_ObjectBackUpVec[i].clear();
		m_CloneNamesBackupVec[i].clear();
	}
}

void CImgui_Manager::Free()
{

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//if (GAMEINSTANCE->Get_KeyDown(DIK_F7))
//{
//	for (size_t i = 0; i < m_vecObjects[m_eObjectType].size(); ++i)
//	{
//		_float Distance = {};
//		XMStoreFloat(&Distance, XMVector3Length(m_vecObjects[m_eObjectType][i]->Get_Transform()->Get_Position()));
//		if (Distance > 250)
//		{
//			m_vecObjects[m_eObjectType][i]->Set_Dead();
//			m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
//			m_vecCloneNames[m_eObjectType].erase(m_vecCloneNames[m_eObjectType].begin() + i);
//		}
//	}
//}