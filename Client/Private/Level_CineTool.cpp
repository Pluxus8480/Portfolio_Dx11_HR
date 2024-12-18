#include "Level_CineTool.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SkyBox.h"

#include "Load_Include.h"
#include "Beat_Manager.h"
#include "GameManager.h"

#include "Cine_Sequence.h"


CLevel_CineTool::CLevel_CineTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

CLevel_CineTool::~CLevel_CineTool()
{
    Free();
}

HRESULT CLevel_CineTool::Initialize()
{
	m_iLevelID = LEVEL_CINETOOL;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Light()))
		return E_FAIL;
	CBeat_Manager::Get_Instance()->Set_Beat(0.5f);

	// Init free transform
	m_pTrash_Trans = static_pointer_cast<CTransform>(
		GAMEINSTANCE->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform")));

	return S_OK;
}

void CLevel_CineTool::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	if (GAMEINSTANCE->Get_KeyDown(DIK_SPACE))
	{
		if (m_CurScenario.IsPlayMode())
		{
			m_CurScenario.ChangeToToolMode();
			if (!m_curSoundTag.empty())
				GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::SOUND_EFFECT, m_curSoundTag);
		}
		else
		{
			m_CurScenario.ChangeToPlayMode();
			if (!m_curSoundTag.empty())
				GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, m_curSoundTag);
		}
	}

	if (m_CurScenario.IsPlayMode())
		m_CurScenario.PlayScenario(fTimeDelta);

	if (GAMEINSTANCE->Get_KeyDown(DIK_P)) {
		if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::TRANSLATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::ROTATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::SCALE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
}

HRESULT CLevel_CineTool::Render()
{
	if (eImgui_State::IMGUI_STATE_RENDER == CImgui_Manager::Get_Instance()->Imgui_GetState())
		return S_OK;

	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);
	
	OptionTool();
	if (!m_CurScenario.IsPlayMode() && nullptr != m_CurScenario.GetSelectedSeq())
	{
		CineTool();
		CameraTool();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

HRESULT CLevel_CineTool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Tool::CAMERA_FREE_DESC			CameraDesc{};
	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(55.f, 69.f, 202.f, 1.f);
	CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
	CameraDesc.fFovy = 0.954f;
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 30.f;
	CameraDesc.strTag = TEXT("Camera_Tool_Main");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	m_pToolCamera = static_pointer_cast<CTool_Cine_Camera>(
		m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, CTool_Cine_Camera::ObjID, &CameraDesc));

	m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Tool_Main"), m_pToolCamera);
	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));

	return S_OK;
}

HRESULT CLevel_CineTool::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.g_fIntensity = 1.f;
	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_CineTool::Imgui_GizmoRender()
{
	if (m_pCurSeq_Trans == nullptr && m_pSelectAction_Trans == nullptr)
		return;

	_float4x4 WorldMat, ViewMat, ProjMat;
	_float4  vPos;
	static _float3 vRot = { 0.f,0.f,0.f };
	_float3 vScale;

	if (nullptr != m_pSelectAction_Trans)
		WorldMat = m_pSelectAction_Trans->Get_WorldMatrix();
	else
		WorldMat = m_pCurSeq_Trans->Get_WorldMatrix();
	ViewMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	ProjMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	_bool flag = false;

	ImGui::Text("");
	if (nullptr != m_pSelectAction_Trans)
		ImGui::Text("Model Trans");
	else
		ImGui::Text("Sequence Trans");
	ImGui::SameLine();
	if (ImGui::Button("SetTrans"))
	{
		m_pCurSeq_Trans->Set_WorldMatrix(m_CurScenario.GetBeforeTrans());
		WorldMat = m_pCurSeq_Trans->Get_WorldMatrix();
	}

	ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &vRot.x, &vScale.x);
	ImGui::PushItemWidth(200.f);
	if (ImGui::InputFloat3("Trans", &vPos.x))
	{
		if (nullptr != m_pSelectAction_Trans)
			m_pSelectAction_Trans->Set_Position(XMVectorSet(vPos.x, vPos.y, vPos.z, vPos.w));
		else
			m_pCurSeq_Trans->Set_Position(XMVectorSet(vPos.x, vPos.y, vPos.z, vPos.w));
	}

	ImGui::PushItemWidth(200.f);

	_float3 vRadianRot = {};
	flag = ImGui::InputFloat3("Rotat", &vRot.x);
	if (flag) {
		vRadianRot.x = XMConvertToRadians(vRot.x);
		vRadianRot.y = XMConvertToRadians(vRot.y);
		vRadianRot.z = XMConvertToRadians(vRot.z);
		if (nullptr != m_pSelectAction_Trans)
			m_pSelectAction_Trans->Rotation_Quaternion(XMLoadFloat3(&vRadianRot));
		else
			m_pCurSeq_Trans->Rotation_Quaternion(XMLoadFloat3(&vRadianRot));
	}

	ImGui::PushItemWidth(200.f);

	if (ImGui::InputFloat3("Scale", &vScale.x))
	{
		if (vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f) {
			if (nullptr != m_pSelectAction_Trans)
				m_pSelectAction_Trans->Set_Scale(vScale);
			else
				m_pCurSeq_Trans->Set_Scale(vScale);
		}
	}
	ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, &WorldMat._11);

	if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
		, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
		, m_tGizmoDesc.CurrentGizmoMode					// WORLD, LOCAL
		, (_float*)&WorldMat
		, NULL
		, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL
		, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
		, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
	{
		if (nullptr != m_pSelectAction_Trans)
			m_pSelectAction_Trans->Set_WorldMatrix(WorldMat);
		else
			m_pCurSeq_Trans->Set_WorldMatrix(WorldMat);
	}
}

void CLevel_CineTool::Imgui_Trash_GizmoRender()
{
	_float4x4 WorldMat, ViewMat, ProjMat;
	_float4  vPos;
	static _float3 vRot = { 0.f,0.f,0.f };
	_float3 vScale;

	WorldMat = m_pTrash_Trans->Get_WorldMatrix();
	ViewMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	ProjMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	_bool flag = false;

	ImGui::Text("");
	ImGui::Text("Only for you!! Free Trans");
	ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &vRot.x, &vScale.x);
	ImGui::PushItemWidth(200.f);
	if (ImGui::InputFloat3("Trans", &vPos.x))
		m_pTrash_Trans->Set_Position(XMVectorSet(vPos.x, vPos.y, vPos.z, vPos.w));

	ImGui::PushItemWidth(200.f);
	_float3 vRadianRot = {};
	flag = ImGui::InputFloat3("Rotat", &vRot.x);
	if (flag) {
		vRadianRot.x = XMConvertToRadians(vRot.x);
		vRadianRot.y = XMConvertToRadians(vRot.y);
		vRadianRot.z = XMConvertToRadians(vRot.z);
		m_pTrash_Trans->Rotation_Quaternion(XMLoadFloat3(&vRadianRot));
	}

	ImGui::PushItemWidth(200.f);
	if (ImGui::InputFloat3("Scale", &vScale.x))
	{
		if (vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f) {
			m_pTrash_Trans->Set_Scale(vScale);
		}
	}
	ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, &WorldMat._11);

	if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
		, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
		, m_tGizmoDesc.CurrentGizmoMode					// WORLD, LOCAL
		, (_float*)&WorldMat
		, NULL
		, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL
		, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
		, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
	{
		m_pTrash_Trans->Set_WorldMatrix(WorldMat);
	}
}

void CLevel_CineTool::OptionTool()
{
	ImGui::Begin("Optional Tool");
	{
		// For free mode ON/OFF
		ImGui::Checkbox("FreeMode", &m_isTrashMode);
		if (m_isTrashMode)
			ImGui::Text("Now FreeMode!!!!!!!!");
		ImGui::Spacing();

		// For scenario file
		if (ImGui::Button("New Scenario"))
		{
			m_CurScenario = CCine_Scenario{};
			m_CurScenario.SetActorsAndCamera();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			m_fileDialog_Json = ImGui::FileBrowser(0, "../Bin/Resources/DataFiles/Cinematics/");
			m_fileDialog_Json.SetTypeFilters({ ".json" });
			m_fileDialog_Json.Open();
		}
		ImGui::Text(m_CurScenario.GetSceneName().c_str());
		if (m_CurScenario.IsPlayMode())
		{
			auto pSeq = m_CurScenario.GetSelectedSeq();
			if (nullptr != pSeq)
			{
				string strSeqName = "Seq: ";
				strSeqName += pSeq->GetName();
				ImGui::Text(strSeqName.c_str());
				ImGui::ProgressBar(pSeq->GetProgress());
			}
		}
		ImGui::Spacing();

		// For actors
		if (ImGui::Button("Load Actor"))
		{
			m_fileDialog = ImGui::FileBrowser(0, "../Bin/Resources/Models/Cinematic/");
			m_fileDialog.SetTypeFilters({ ".fbx" });
			m_fileDialog.Open();
		}
		if (ImGui::BeginListBox("##UsableActors"))
		{
			for (auto pair : m_CurScenario.GetActors())
				ImGui::Selectable(pair.first.c_str(), false);
			ImGui::EndListBox();
		}
		ImGui::Spacing();

		// For select sequence
		static _char szName[MAX_PATH];
		ImGui::InputText("Title", szName, MAX_PATH);
		if (ImGui::Button("New Seq"))
			m_CurScenario.CreateSeq(szName);

		if (ImGui::BeginListBox("##SequenceList"))
		{
			_int iSelectIndex = -1;
			for (auto sequence : m_CurScenario.GetSequences())
			{
				ImGui::PushID(iSelectIndex);

				++iSelectIndex;
				if (ImGui::Selectable(sequence->GetName().c_str(), false))
				{
					m_CurScenario.SelectSeq(iSelectIndex);
					m_pCurSeq_Trans = m_CurScenario.GetSelectedSeq()->GetTransPointer();
					m_pSelectAction_Trans = nullptr;

					// Init all select variable
					m_strSelectedActor = "";
					m_strSelectedAnim = "";
					m_iActionSelect = -1;
					m_iCamSelect = -1;
					m_iEffectSelect = -1;
					m_iCallbackSelect = -1;
					m_iConversationSelect = -1;
				}

				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		// For save scenario
		ImGui::Spacing();
		if (ImGui::Button("Save Scenario"))
		{
			m_fileDialog_Save_Scenario = ImGui::FileBrowser(0, "../Bin/Resources/DataFiles/Cinematics/");
			m_fileDialog_Save_Scenario.SetTypeFilters({ ".json" });
			m_fileDialog_Save_Scenario.Open();
		}

		// Set Sound
		ImGui::Spacing();
		if (ImGui::Button("Load Sound"))
		{
			m_fileDialog_Sound = ImGui::FileBrowser(0, "../Bin/Resources/Sounds/Cinematic/");
			m_fileDialog_Sound.SetTypeFilters({ ".mp3", ".ogg" });
			m_fileDialog_Sound.Open();
		}
	}
	ImGui::End();


	// For load scenario
	m_fileDialog_Json.Display();
	if (m_fileDialog_Json.HasSelected())
	{
		string strJsonPath = m_fileDialog_Json.GetSelected().string();
		m_CurScenario = CCine_Scenario{ strJsonPath };
		m_CurScenario.SetActorsAndCamera();
		m_fileDialog_Json.ClearSelected();
	}

	// For load actors
	m_fileDialog.Display();
	if (m_fileDialog.HasSelected())
	{
		filesystem::path pathSelected = m_fileDialog.GetSelected();
		filesystem::path pathBase = filesystem::current_path();
		filesystem::path relativePath = filesystem::relative(pathSelected, pathBase);

		string strModelPath = relativePath.string();
		m_CurScenario.PushActor(strModelPath);
		m_fileDialog.ClearSelected();
	}

	// For Save scenario
	m_fileDialog_Save_Scenario.Display();
	if (m_fileDialog_Save_Scenario.HasSelected())
	{
		string strJsonPath = m_fileDialog_Save_Scenario.GetSelected().string();
		m_CurScenario.SaveScenario(strJsonPath);
		m_fileDialog_Save_Scenario.ClearSelected();
	}

	// Start Scenario
	m_fileDialog_Sound.Display();
	if (m_fileDialog_Sound.HasSelected())
	{
		string strFileName = m_fileDialog_Sound.GetSelected().filename().string();
		strFileName.erase(strFileName.length() - 4);
		m_curSoundTag = wstring(strFileName.begin(), strFileName.end());

		string strSoundPath = m_fileDialog_Sound.GetSelected().string();
		wstring wstrSoundPath = wstring(strSoundPath.begin(), strSoundPath.end());

		GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, m_curSoundTag, wstrSoundPath);

		m_fileDialog_Sound.ClearSelected();
	}
}

void CLevel_CineTool::CineTool()
{
	ImGui::Begin("Cine Main");
	{
		auto pSequence = m_CurScenario.GetSelectedSeq();
		ImGui::Text(pSequence->GetName().c_str());

		static _float fTimeLine = 0.f;
		ImGui::SliderFloat("TimeLine", &fTimeLine, 0.f, pSequence->GetTotalFrame(), "%.1f");
		ImGui::SameLine();
		ImGui::InputFloat("##inputfloat", &fTimeLine);
		pSequence->SetFrame(fTimeLine);

		ImGui::Spacing();
		if (ImGui::Button("Add Action"))
		{
			ImGui::OpenPopup("Actors");
			m_iActionSelect = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Action") && m_iActionSelect != -1)
		{
			pSequence->DeleteAction(m_iActionSelect);
			m_iActionSelect = -1;
		}

		// Modal page for adding Action
		if (ImGui::BeginPopupModal("Actors", NULL, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginListBox("Model List"))
			{
				for (auto pairActor : m_CurScenario.GetActors())
				{
					if (ImGui::Selectable(pairActor.first.c_str(), false))
						m_strSelectedActor = pairActor.first;
				}
				ImGui::EndListBox();
			}

			if (ImGui::Button("Select Anim"))
				ImGui::OpenPopup("Animations");
			if (ImGui::BeginPopupModal("Animations", NULL, ImGuiWindowFlags_MenuBar))
			{
				vector<string> animList;
				m_CurScenario.GetActors()[m_strSelectedActor]->GetAnimStrList(animList);
				if (ImGui::BeginListBox("##AnimList"))
				{
					for (auto strAnim : animList)
					{
						if (strAnim.find(m_CurScenario.GetSceneName()) != std::string::npos)
						{
							if (ImGui::Selectable(strAnim.c_str(), false))
								m_strSelectedAnim = strAnim;
						}
					}
					ImGui::EndListBox();
				}

				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			string strSelected = "";
			strSelected += m_strSelectedActor;
			strSelected += " / ";
			strSelected += m_strSelectedAnim;
			ImGui::Text(strSelected.c_str());
			ImGui::Spacing();

			if (ImGui::Button("Save"))
			{
				CCine_Sequence::ActionInfo actionInfo{};
				actionInfo.fFrame_Start = fTimeLine;
				actionInfo.strActor = m_strSelectedActor;
				actionInfo.strAnim = m_strSelectedAnim;
				XMStoreFloat4x4(&actionInfo.matInit, XMMatrixIdentity());
				pSequence->PushAction(actionInfo);

				m_iActionSelect = -1;
				m_pCurSeq_Trans = nullptr;

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		// List of added Actions
		if (ImGui::BeginListBox("##ActionList"))
		{
			_int iSelectIndex = -1;
			for (auto action : pSequence->GetActionList())
			{
				ImGui::PushID(iSelectIndex);

				string strValue;
				{	// Set keywords
					ostringstream oss;
					oss << fixed << setprecision(1) << action.fFrame_Start;
					strValue = oss.str();
					strValue += " | ";
					strValue += action.strAnim;
				}

				++iSelectIndex;
				const bool is_selected = (m_iActionSelect == iSelectIndex);
				if (ImGui::Selectable(strValue.c_str(), is_selected))
				{
					if (m_iActionSelect == iSelectIndex)
					{
						m_iActionSelect = -1;
						m_pSelectAction_Trans = nullptr;
					}
					else
					{
						m_iActionSelect = iSelectIndex;
						m_pSelectAction_Trans = pSequence->GetActionTransPointer(m_iActionSelect);
					}
				}

				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		// Selected actorInfo
		if (m_isTrashMode) {
			Imgui_Trash_GizmoRender();
		}
		else if (m_pCurSeq_Trans != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::Spacing();
		ImGui::Text("Camera Point List");
		if (ImGui::Button("Save Cam") || GAMEINSTANCE->Get_KeyDown(DIK_LSHIFT))
		{
			ImGui::OpenPopup("Select_CamIntp");
		}
		ImGui::SameLine();
		if (ImGui::Button("Del Cam"))
		{
			pSequence->DeleteCamInfo(m_iCamSelect);
			m_iCamSelect = -1;
		}
		
		// Select Camera inter
		if (ImGui::BeginPopupModal("Select_CamIntp", NULL, ImGuiWindowFlags_MenuBar))
		{
			static int selected = -1;
			if (ImGui::Selectable("None", selected == 0, ImGuiSelectableFlags_DontClosePopups))
				selected = 0;
			if (ImGui::Selectable("Linear", selected == 1, ImGuiSelectableFlags_DontClosePopups))
				selected = 1;
			if (ImGui::Selectable("Curve", selected == 2, ImGuiSelectableFlags_DontClosePopups))
				selected = 2;

			if (ImGui::Button("Save") && selected != -1)
			{
				CCine_Sequence::CineCamInfo camInfo{};
				m_pToolCamera->GetCamInfo(camInfo);
				camInfo.fTimeLine = fTimeLine;
				camInfo.iInterpType = selected;
				pSequence->PushCamInfo(camInfo);

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}


		if (ImGui::BeginListBox("##Camera"))
		{
			_int iSelectIndex = -1;
			for (auto camInfo : pSequence->GetCamList())
			{
				ImGui::PushID(iSelectIndex);

				string strName = to_string(camInfo.fTimeLine);
				if (camInfo.iInterpType == 0)
					strName += "| None";
				else if (camInfo.iInterpType == 1)
					strName += "| Linear";
				else if (camInfo.iInterpType == 2)
					strName += "| Curve";

				++iSelectIndex;
				const bool is_selected = (m_iCamSelect == iSelectIndex);
				if (ImGui::Selectable(strName.c_str(), is_selected))
				{
					if (m_iCamSelect == iSelectIndex)
					{
						m_iCamSelect = -1;
						fTimeLine = 0.f;
					}
					else
					{
						m_iCamSelect = iSelectIndex;
						m_pToolCamera->SetCamInfo(camInfo);
						fTimeLine = camInfo.fTimeLine;
					}
				}

				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		ImGui::Spacing();
		ImGui::Text("Effect List");
		if (ImGui::Button("Save Effect"))
		{
			ImGui::OpenPopup("vibe_setting");
		}
		ImGui::SameLine();
		if (ImGui::Button("Del Effect"))
		{
			pSequence->DeleteEffect(m_iEffectSelect);
			m_iEffectSelect = -1;
		}
		if (ImGui::BeginListBox("##EffectList"))
		{
			_int iSelectIndex = -1;
			for (auto effect : pSequence->GetEffectList())
			{
				ImGui::PushID(iSelectIndex);

				iSelectIndex++;
				const bool is_selected = (m_iEffectSelect == iSelectIndex);
				string strName = to_string(effect.fTimeLine);
				strName += "| ";
				strName += to_string(effect.fVibeScale);
				if (ImGui::Selectable(strName.c_str(), is_selected))
				{
					if (m_iEffectSelect == iSelectIndex)
						m_iEffectSelect = -1;
					else
					{
						m_iEffectSelect = iSelectIndex;
						fTimeLine = effect.fTimeLine;
					}
				}

				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		ImGui::Spacing();
		ImGui::Text("Callback List");
		if (ImGui::Button("Save Callback"))
		{
			CCine_Sequence::CineCallback callback{};
			callback.fTimeLine = fTimeLine;
			callback.vLook = { 0.f, 0.f, 1.f, 0.f };
			callback.vPos = {0.f, 0.f, 0.f, 1.f};
			callback.funcCallback = [](_float4, _float4, _float3) {};
			pSequence->PushCallback(callback);
			m_iCallbackSelect = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Del Callback"))
		{
			pSequence->DeleteCallback(m_iCallbackSelect);
			m_iCallbackSelect = -1;
		}
		if (ImGui::BeginListBox("##CallbackList_TT"))
		{
			_int iSelectIndex = -1;
			for (auto callback : pSequence->GetCallbackList())
			{
				ImGui::PushID(iSelectIndex);

				iSelectIndex++;
				const bool is_selected = (m_iCallbackSelect == iSelectIndex);
				string strName = to_string(callback.fTimeLine);
				if (ImGui::Selectable(strName.c_str(), is_selected))
				{
					if (m_iCallbackSelect == iSelectIndex)
						m_iCallbackSelect = -1;
					else
					{
						m_iCallbackSelect = iSelectIndex;
						fTimeLine = callback.fTimeLine;
					}
				}

				ImGui::PopID();
			}

			ImGui::EndListBox();
		}
		// Check
		if (m_iCallbackSelect != -1)
		{
			if (ImGui::Button("Save Pos"))
				pSequence->EditCallback_Pos(m_iCallbackSelect, m_pTrash_Trans->Get_Position());
			ImGui::SameLine();
			if (ImGui::Button("Save Look"))
				pSequence->EditCallback_Look(m_iCallbackSelect, m_pTrash_Trans->Get_Position());
			ImGui::SameLine();
			if (ImGui::Button("Save Size"))
				pSequence->EditCallback_Size(m_iCallbackSelect, m_pTrash_Trans->Get_Scale());

			auto vPos = pSequence->GetCallbackList()[m_iCallbackSelect].vPos;
			ImGui::InputFloat3("Pos", &vPos.x);
			auto vLook = pSequence->GetCallbackList()[m_iCallbackSelect].vLook;
			ImGui::InputFloat3("Look", &vLook.x);
			auto vSize = pSequence->GetCallbackList()[m_iCallbackSelect].vSize;
			ImGui::InputFloat3("Size", &vSize.x);
		}

		ImGui::Spacing();
		ImGui::Text("Conversation List");
		static _float fConvFrame = 0.f;
		ImGui::InputFloat("Frame", &fConvFrame);
		if (ImGui::Button("Save Conversation"))
		{
			CCine_Sequence::ConvCallback convCallback{};
			convCallback.fTimeLine = fTimeLine;
			convCallback.fConvFrame = fConvFrame;
			convCallback.funcCallback = [](_float) {};
			pSequence->PushConversation(convCallback);
			m_iConversationSelect = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Del Conversation"))
		{
			pSequence->DeleteConversation(m_iConversationSelect);
			m_iConversationSelect = -1;
		}
		if (ImGui::BeginListBox("##ConvList_TT"))
		{
			_int iSelectIndex = -1;
			for (auto callback : pSequence->GetConvList())
			{
				ImGui::PushID(iSelectIndex);

				iSelectIndex++;
				const bool is_selected = (m_iConversationSelect == iSelectIndex);
				string strName = to_string(callback.fTimeLine);
				strName += "| ";
				strName += to_string(callback.fConvFrame / 60.f);
				if (ImGui::Selectable(strName.c_str(), is_selected))
				{
					if (m_iConversationSelect == iSelectIndex)
						m_iConversationSelect = -1;
					else
					{
						m_iConversationSelect = iSelectIndex;
						fTimeLine = callback.fTimeLine;
					}
				}
				ImGui::PopID();
			}

			ImGui::EndListBox();
		}

		if (ImGui::BeginPopupModal("vibe_setting", NULL, ImGuiWindowFlags_MenuBar))
		{
			static _float fScale = 0.f;
			ImGui::SliderFloat("Scale", &fScale, 0.f, 1.f, "%.01f");
			ImGui::SameLine();
			ImGui::InputFloat("##Scal", &fScale);

			static _float fFrame = 0.f;
			ImGui::SliderFloat("Frame", &fFrame, 0.f, 30.f, "%.1f");
			ImGui::SameLine();
			ImGui::InputFloat("##Fram", &fFrame);

			ImGui::Spacing();
			if (ImGui::Button("Save"))
			{
				CCine_Sequence::EffectInfo effectInfo{};
				effectInfo.fTimeLine = fTimeLine;
				effectInfo.fVibeScale = fScale;
				effectInfo.fVibeFrame = fFrame;
				pSequence->PushEffect(effectInfo);

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

	}
	ImGui::End();
}

void CLevel_CineTool::CameraTool()
{
	ImGui::Begin("Camera Setting");
	{
		ImGui::SliderFloat("MoveSpeed", &m_pToolCamera->m_fMoveSpeed, 1.f, 30.f);
		ImGui::SameLine();
		ImGui::InputFloat("##MoveSpee", &m_pToolCamera->m_fMoveSpeed);

		ImGui::Spacing();
		ImGui::SliderFloat("Fov", &m_pToolCamera->m_fFovy, 0.1f, 2.f);
		ImGui::SameLine();
		if (ImGui::InputFloat("##Fo", &m_pToolCamera->m_fFovy))
		{
			if (m_pToolCamera->m_fFovy <= 0.f)
				m_pToolCamera->m_fFovy = 0.954f;
		}
	}
	ImGui::End();
}

shared_ptr<CLevel_CineTool> CLevel_CineTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_CineTool
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_CineTool(pDevice, pContext) {}
	};

	shared_ptr<CLevel_CineTool> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_CineTool"));
		assert(false);
	}
	return pInstance;
}

void CLevel_CineTool::Free()
{
}
