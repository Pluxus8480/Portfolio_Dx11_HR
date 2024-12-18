#include "Level_KaleStage.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"
#include "MapManager.h"
#include "SkyBox.h"

#include "Cine_Manager.h"

#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "Beat_Manager.h"
#include "Chai.h"
#include "808_Controller.h"
#include "CUi_CNMNRazer.h"
#include "GimmickManager.h"
#include "CCineSubTitleMgr.h"
#include "CUi_TalkSystem.h"


CLevel_KaleStage::CLevel_KaleStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_KaleStage::~CLevel_KaleStage()
{
	Free();
} 

HRESULT CLevel_KaleStage::Initialize()
{
	m_iLevelID = LEVEL_KALESTAGE;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	// Load Cinematics
	CCine_Manager::Get_Instance()->Initialize("../Bin/Resources/DataFiles/Cinematics/Kale_Stage/");
	CCineSubTitleMgr::Get_Instance()->Initialize_KaleStageCallBack();
	//CImgui_Manager::Get_Instance()->Set_ToolType(eToolType::TOOL_MAP, false);
	/*Player */
	if (!CPlayer_Manager::Get_Instance()->Get_Player())
	{
		CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));
	}
	auto pPlayer = CPlayer_Manager::Get_Instance()->Get_Player();
	pPlayer->Set_Player_Position(XMVectorSet(0.f, 1.f, -20.f, 1.f));
	CPlayer_Manager::Get_Instance()->Set_Player(pPlayer);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Camera(static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main"))));

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if(FAILED(Ready_Layer_Bgm()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Assist()))
		return E_FAIL;
	CBeat_Manager::Get_Instance()->Reset();
	m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, L"Surfacing", 0.3f);
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::MUSIC, L"Surfacing", true);
	CBeat_Manager::Get_Instance()->Set_Beat(0.410958f);
	Ready_Ui();
	CUi_Mgr::Get_Instance()->Add_Clone(CUi_CNMNRazer::ObjID, m_iLevelID, L"CUi", CUi_CNMNRazer::ObjID);
	//CBeat_Manager::Get_Instance()->Start_Sequence();

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	// Cine
	CCine_Manager::Get_Instance()->AddSequenceCallback("EV1150", 7, 0,
		[&] (_float4, _float4, _float3) {
			if (!m_bTalkCol)
			{
				m_bTalkCol = true;
				m_isTalkOver = false;

				auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
				if (pTalkSystem == nullptr)
				{
					CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
					TalkDesc.endCallback = [&]() {
						m_isTalkOver = true;
						};

					pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_KALESTAGE, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
				}

				auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
				if (PRequiredTalkSystem != nullptr)
				{
					PRequiredTalkSystem->LoadTalkFile(L"STKale Talk_File_23.csv");

					CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
				}
			}

		}
	);

	return S_OK;
}

void CLevel_KaleStage::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	// Play cinematic
	CCine_Manager::Get_Instance()->PlayCinematic(fTimeDelta);


	//if (GAMEINSTANCE->Get_KeyDown((DIK_F2)))
	//{
	//	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));
	//}
	//if (GAMEINSTANCE->Get_KeyDown((DIK_F3)))
	//{
	//	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Player_Main"));
	//}

	//if (GAMEINSTANCE->Get_KeyDown((DIK_F9)))
	//{
	//	GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KALEBOSS));
	//	return;
	//}

	CURSOR_CONTROL
	if (m_bLevelChanged)
	{
		GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)m_iNextLevel));
	}

	return;
}

HRESULT CLevel_KaleStage::Render()
{

	return S_OK;
}

void CLevel_KaleStage::Terminate()
{
	PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Get_Wire_Target().clear();
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CMapManager::Get_Instance()->Clear_BossObjects();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	auto pGI = m_pGameInstance.lock();
	pGI->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Surfacing");
}

HRESULT CLevel_KaleStage::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Player::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(0.f, 0.f, -25.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(70.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fSpeedPerSec = 50.f;
	CameraDesc.strTag = TEXT("Camera_Player_Main");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CCamera_Player> Camera = static_pointer_cast<CCamera_Player>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc));

	m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Player_Main"), Camera);
	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Player_Main"));

	{
		CCamera_Tool::CAMERA_FREE_DESC			CameraDesc{};

		CameraDesc.fMouseSensor = 0.05f;
		CameraDesc.vEye = _float4(0.f, 1.5f, -3.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.fFovy = XMConvertToRadians(90.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 3000.f;
		CameraDesc.fSpeedPerSec = 10.f;
		CameraDesc.strTag = TEXT("Camera_Tool_Main");
		CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		shared_ptr<CCamera_Tool> Camera = static_pointer_cast<CCamera_Tool>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));

		m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Tool_Main"), Camera);
		//m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));
	}
	return S_OK;
}

HRESULT CLevel_KaleStage::Ready_Layer_Map(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		return E_FAIL;

	CGimmickManager::Get_Instance()->SetUp_CinematicCallBack();

	CMapManager::Get_Instance()->Push_MapObjects(m_iLevelID, strLayerTag);

	return S_OK;
}

HRESULT CLevel_KaleStage::Ready_Layer_Monster(const wstring& strLayerTag)
{

	//CTransform::TRANSFORM_DESC	TransformDesc{};
	//TransformDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	///* Monsters */
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Saber::ObjID, &TransformDesc);
	//
	//TransformDesc.vPosition = _float4(0.f, 0.f, 10.f, 1.f);
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Baboo::ObjID, &TransformDesc);

	//GAMEINSTANCE->Map_Load(LEVEL_FIRSTSTAGE);
	return S_OK;
}

HRESULT CLevel_KaleStage::Ready_Layer_Bgm()
{

	return S_OK;
}

HRESULT CLevel_KaleStage::Ready_Layer_Effect(const wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_KaleStage::Ready_Layer_Assist()
{
	PLAYERMANAGER->Reset_Assists();
	PLAYERMANAGER->Reset_808_Controller();
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(5.7f, 0.4f, 88.f, 1.f);
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Peppermint"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Peppermint"), CPeppermint::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Macaron"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Macaron"), CMacaron::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Korsica"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Korsica"), CKorsica::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_808_Controller(GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_808_Controller"), C808_Controller::ObjID, &TransformDesc));

	GAMEINSTANCE->Add_Clone(m_iLevelID, L"Layer_CNMN", CCNMN_Playable::ObjID, &TransformDesc);
	
	return S_OK;
}

HRESULT CLevel_KaleStage::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -0.5f, 0.f, 0.f);
	LightDesc.g_fIntensity = 1.f;

	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.4f, 0.4f, 0.4f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main2");
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(-1.f, -0.5f, 0.f, 0.f);
	LightDesc.g_fIntensity = 1.f;

	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDiffuse = _float4(0.3f, 0.3f, 0.3f, 1.f);
	//strcpy_s(LightDesc.szName, "Light_Directional_Main3");
	//LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vDirection = _float4(1.f, 0.f, 1.f, 0.f);
	//LightDesc.g_fIntensity = 1.f;

	//if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
	//	return E_FAIL;

	return S_OK;

}

void CLevel_KaleStage::Ready_Ui()
{
	CUi_Mgr::Get_Instance()->LevelChange_Create(m_iLevelID);
}

void CLevel_KaleStage::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_KaleStage> CLevel_KaleStage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_KaleStage
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_KaleStage(pDevice, pContext) { }
	};

	shared_ptr<CLevel_KaleStage> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_KaleStage"));
		assert(false);
	}

	return pInstance;
}

void CLevel_KaleStage::Free()
{

}