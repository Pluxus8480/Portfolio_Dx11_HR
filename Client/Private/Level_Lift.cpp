#include "Level_Lift.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"
#include "SkyBox.h"

#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "Beat_Manager.h"
#include "Lift.h"
#include "Enemy_Lift.h"
#include "CombatManager.h"
#include "MapManager.h"
#include "GimmickManager.h"

CLevel_Lift::CLevel_Lift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Lift::~CLevel_Lift()
{
	Free();
}

HRESULT CLevel_Lift::Initialize()
{
	m_iLevelID = LEVEL_LIFT;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);
	//CImgui_Manager::Get_Instance()->Set_ToolType(eToolType::TOOL_MAP, false);
	/*Player */
	if (!CPlayer_Manager::Get_Instance()->Get_Player())
		CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));
	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Position(XMVectorSet(0.f, 35.f, 0.f, 1.f));

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Camera(static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main"))));

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Bgm()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(5.7f, 0.4f, 88.f, 1.f);
	PLAYERMANAGER->Reset_Assists();
	PLAYERMANAGER->Reset_808_Controller();
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Peppermint"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Peppermint"), CPeppermint::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Macaron"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Macaron"), CMacaron::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Korsica"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Korsica"), CKorsica::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_808_Controller(GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_808_Controller"), C808_Controller::ObjID, &TransformDesc));

	CBeat_Manager::Get_Instance()->Reset();
	m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, L"Can't_Stop_Us_Now", 0.4f);
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::MUSIC, L"Can't_Stop_Us_Now", true);
	m_pGameInstance.lock()->Set_Fog(_float4{ 0.2f,0.2f,0.2f, 1.f }, 2000.f);
	CBeat_Manager::Get_Instance()->Set_Beat(0.4166f);
	Ready_Ui();

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	GAMEINSTANCE->Add_Unique_Sound_Channel("ST_Lift");

	return S_OK;
}

void CLevel_Lift::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	//if (GAMEINSTANCE->Get_KeyDown((DIK_F2)))
	//{
	//	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));
	//}
	//if (GAMEINSTANCE->Get_KeyDown((DIK_F3)))
	//{
	//	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Player_Main"));
	//}
	//ShowCursor(false);

	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
		if (m_bStart && m_iWaveCount == 0)
		{
			vector<_uint> vecMon{0,0,0};
			m_pEnemyLift->Move(CEnemy_Lift::LF, 1, vecMon);
			++m_iWaveCount;
			m_bStart = false;
		}
		else if (m_iWaveCount == 1)
		{
			if (CCombat_Manager::Get_Instance()->Check_Battle_End())
			{
				vector<_uint> vecMon{ 0,0 };
				m_pEnemyLift->Move(CEnemy_Lift::RR, 1, vecMon);
				++m_iWaveCount;
			}
		}
		else if (m_iWaveCount == 2)
		{
			if (CCombat_Manager::Get_Instance()->Check_Battle_End())
			{
				if (!m_bTalk)
				{
					m_bTalk = true;
					m_isTalkOver = false;

					auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
					if (pTalkSystem == nullptr)
					{
						CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
						TalkDesc.endCallback = [&]() {
							m_isTalkOver = true;
							};

						pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_LIFT, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
					}

					auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
					if (PRequiredTalkSystem != nullptr)
					{
						PRequiredTalkSystem->LoadTalkFile(L"Lift_Panel Talk_File.csv");

						CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
					}
				}
				++m_iPanelCount;

				if(m_iPanelCount >= 4 && m_iPanelCount <= 6)
				{
					m_pGameInstance.lock()->Push_Object_From_Pool(CPanel_Lift::ObjID, m_iLevelID, L"Map");
				}
				if (m_iPanelCount >= 12 && m_iPanelCount <= 15)
				{
					m_pGameInstance.lock()->Push_Object_From_Pool(CPanel_Lift::ObjID, m_iLevelID, L"Map");
				}
				if (m_iPanelCount >= 20 && m_iPanelCount <= 23)
				{
					m_pGameInstance.lock()->Push_Object_From_Pool(CPanel_Lift::ObjID, m_iLevelID, L"Map");
				}
				if (m_iPanelCount >= 28)
				{
					m_iPanelCount = 0;
					++m_iWaveCount;
					m_bTalk = false;
				}
			}
		}
		else if (m_iWaveCount == 3)
		{
			m_pLift->Down(true);
			++m_iWaveCount;
		}
		else if (m_iWaveCount == 4)
		{
			if (!m_bTalk)
			{
				m_bTalk = true;
				m_isTalkOver = false;

				auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
				if (pTalkSystem == nullptr)
				{
					CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
					TalkDesc.endCallback = [&]() {
						m_isTalkOver = true;
						};

					pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_LIFT, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
				}

				auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
				if (PRequiredTalkSystem != nullptr)
				{
					PRequiredTalkSystem->LoadTalkFile(L"Lift_Brake Talk_File_1.csv");

					CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
				}
			}
			vector<_uint> vecMon{ 0,0 };
			m_pEnemyLift->Move(CEnemy_Lift::RF, -1, vecMon);
			m_pLift->Brake_On(CLift::FRONT_LEFT);
			++m_iWaveCount;

		}
		else if (m_iWaveCount == 5 || m_iWaveCount == 6)
		{
			m_bTalk = false;
			if (m_pLift->Is_Broken(CLift::FRONT_LEFT))
			{
				++m_iWaveCount;
			}
		}
		else if (m_iWaveCount == 7)
		{
			m_pLift->Brake_On(CLift::FRONT_RIGHT);
			vector<_uint> vecMon{ 0,2 };
			//vector<_uint> vecMon{ 0,1 };
			m_pEnemyLift->Move(CEnemy_Lift::RR, 1, vecMon);
			++m_iWaveCount;
		}
		else if (m_iWaveCount == 8 || m_iWaveCount == 9)
		{
			if (m_pLift->Is_Broken(CLift::FRONT_RIGHT))
			{
				++m_iWaveCount;
			}
		}
		else if (m_iWaveCount == 10)
		{
			m_pLift->Brake_On(CLift::BACK_LEFT);
			vector<_uint> vecMon{ 2,0,0 };
			//vector<_uint> vecMon{ 1,1, };
			m_pEnemyLift->Move(CEnemy_Lift::LF, -1, vecMon);
			++m_iWaveCount;
		}
		else if (m_iWaveCount == 11 || m_iWaveCount == 12)
		{
			if (m_pLift->Is_Broken(CLift::BACK_LEFT))
			{
				++m_iWaveCount;
			}
		}
		else if (m_iWaveCount == 13)
		{
			if (!m_bTalk)
			{
				m_bTalk = true;
				m_isTalkOver = false;

				auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
				if (pTalkSystem == nullptr)
				{
					CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
					TalkDesc.endCallback = [&]() {
						m_isTalkOver = true;
						};

					pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_LIFT, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
				}

				auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
				if (PRequiredTalkSystem != nullptr)
				{
					PRequiredTalkSystem->LoadTalkFile(L"Lift_Brake Talk_File_2.csv");

					CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
				}
			}
			m_pLift->Brake_On(CLift::BACK_RIGHT);
			vector<_uint> vecMon{ 2,0,0 };
			//vector<_uint> vecMon{ 0,1,1 };
			m_pEnemyLift->Move(CEnemy_Lift::LR, 1, vecMon);
			++m_iWaveCount;
		}
		else if(m_iWaveCount == 14)
		{
			if (m_pLift->Is_Broken(CLift::BACK_RIGHT))
			{
				m_bEnd = true;
				++m_iWaveCount;
				GAMEMANAGER->Dissolve(-3.1f * CBeat_Manager::Get_Instance()->Get_Beat());
			}
		}
		else if (m_iWaveCount == 15 || m_iWaveCount == 16 || m_iWaveCount == 17)
		{
			++m_iWaveCount;
			if (m_iWaveCount == 18)
			{
				GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KORSICA));
			}
		}
	}


	if (GAMEINSTANCE->Get_KeyDown(DIK_RETURN))
	{
		m_bStart = true;
		//m_pGameInstance.lock()->Push_Object_From_Pool(CPanel_Lift::ObjID, m_iLevelID, L"Map");
	}
	
	CURSOR_CONTROL

	return;
}

HRESULT CLevel_Lift::Render()
{

	return S_OK;
}

void CLevel_Lift::Terminate()
{
	auto pGI = m_pGameInstance.lock();
	pGI->Set_Fog({});
	pGI->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Can't_Stop_Us_Now");
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	CMapManager::Get_Instance()->Clear_BossObjects();
}

HRESULT CLevel_Lift::Ready_Layer_Camera(const wstring& strLayerTag)
{
	{
		CCamera_Player::CAMERA_FREE_DESC			CameraDesc{};

		CameraDesc.fMouseSensor = 0.05f;
		CameraDesc.vEye = _float4(0.f, 0.f, -25.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.fFovy = XMConvertToRadians(90.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 3000.f;
		CameraDesc.fSpeedPerSec = 50.f;
		CameraDesc.strTag = TEXT("Camera_Player_Main");
		CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		shared_ptr<CCamera_Player> Camera = static_pointer_cast<CCamera_Player>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc));

		m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Player_Main"), Camera);
		m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Player_Main"));
	}

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

HRESULT CLevel_Lift::Ready_Layer_Map(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_GridBox"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CSlate_Platform::ObjID)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CTunnel::ObjID)))
		return E_FAIL;
	CLift::ANIMOBJ_DESC desc{};
	desc.fRotationPerSec = 1.f;
	desc.fSpeedPerSec = 1.f;
	XMStoreFloat4x4(&desc.worldMatrix, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f), XMVectorSet(0.f, 24.f, 0.f, 1.f)));
	m_pLift = static_pointer_cast<CLift>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, CLift::ObjID, &desc));
	

	CEnemy_Lift::ANIMOBJ_DESC enemyLiftDesc{};
	enemyLiftDesc.fRotationPerSec = 1.f;
	enemyLiftDesc.fSpeedPerSec = 1.f;
	XMStoreFloat4x4(&enemyLiftDesc.worldMatrix, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)));

	m_pEnemyLift = static_pointer_cast<CEnemy_Lift>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, CEnemy_Lift::ObjID, &enemyLiftDesc));


	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CLift_Speed_Effect::ObjID)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CPanel_Lift::ObjID)))
	//	return E_FAIL;
	
	//CImgui_Manager::Get_Instance()->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_A01");
	//CImgui_Manager::Get_Instance()->Push_MapObjects(LEVEL_TEST, strLayerTag);


	return S_OK;
}

HRESULT CLevel_Lift::Ready_Layer_Monster(const wstring& strLayerTag)
{

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	/* Monsters */
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Saber::ObjID, &TransformDesc);
	//
	//TransformDesc.vPosition = _float4(0.f, 0.f, 10.f, 1.f);
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Baboo::ObjID, &TransformDesc);



	//GAMEINSTANCE->Map_Load(LEVEL_FIRSTSTAGE);
	return S_OK;
}

HRESULT CLevel_Lift::Ready_Layer_Bgm()
{

	return S_OK;
}

HRESULT CLevel_Lift::Ready_Layer_Effect(const wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Lift::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.4f, 0.4f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.g_fIntensity = 1.f;

	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	return S_OK;

}

void CLevel_Lift::Ready_Ui()
{
	CUi_Mgr::Get_Instance()->LevelChange_Create(m_iLevelID);
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
}

void CLevel_Lift::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_Lift> CLevel_Lift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_Lift
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Lift(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Lift> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Lift"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Lift::Free()
{

}
