#include "Level_ModelTest.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SkyBox.h"

#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"

#include "Imgui_Manager.h"
#include "MapManager.h"
#include "Beat_Manager.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "Cine_Manager.h"
#include "Effect_Manager.h"
#include "GimmickManager.h"
#include "CCineSubTitleMgr.h"


CLevel_ModelTest::CLevel_ModelTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevel_ModelTest::~CLevel_ModelTest()
{
	Free();
}

HRESULT CLevel_ModelTest::Initialize()
{
	m_iLevelID = LEVEL_MODELTEST;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);
	if (!CPlayer_Manager::Get_Instance()->Get_Player())
		CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));

	// Load Cinematics
	CCine_Manager::Get_Instance()->Initialize("../Bin/Resources/DataFiles/Cinematics/Rekka/");
	CCineSubTitleMgr::Get_Instance()->Initialize_RekkaCallBack();
	CUi_Mgr::Get_Instance()->LevelChange_Create(m_iLevelID);
	// Set chai position
	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Position(XMVectorSet(1185.7f, 496.4f, 110.f, 1.f));

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Camera(static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main"))));

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	if (FAILED(Ready_Light()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Assist()))
		return E_FAIL;

	CBeat_Manager::Get_Instance()->Reset();
	CBeat_Manager::Get_Instance()->Set_Beat(0.4225f);

	// Callback
	CCine_Manager::Get_Instance()->AddSequenceCallback("CS0230", 10, 0, [&](_float4, _float4, _float3) {
		CBeat_Manager::Get_Instance()->Reset();
		m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, L"Captive_Normals", 0.4f);
		m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::MUSIC, L"Captive_Normals", true);
		}
	);

	// Start intro cinematic
	CUi_Mgr::Get_Instance()->Ui_AllActive(false);
	CCine_Manager::Get_Instance()->StartCine("CS0230");

	string strEffectName = "Rekka_Map_Spark_Ctr";
	_float4x4 matWorld = INIT_MATRIX;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 3.f);

	CCine_Manager::Get_Instance()->AddLastCallback("CS0245", [&]()
		{
			CUi_StageResultSystem::Desc Desc;
			Desc.iNextLevel = LEVEL_LIFT;
			CUi_Mgr::Get_Instance()->Add_Clone(CUi_StageResultSystem::ObjID, m_iLevelID,
				L"CUi", CUi_StageResultSystem::ObjID, &Desc);
		});

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(5.7f, 0.4f, 88.f, 1.f);
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Peppermint"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Peppermint"), CPeppermint::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Macaron"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Macaron"), CMacaron::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Korsica"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Korsica"), CKorsica::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_808_Controller(GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_808_Controller"), C808_Controller::ObjID, &TransformDesc));

	GAMEINSTANCE->Add_Clone(m_iLevelID, L"Layer_CNMN", CCNMN_Playable::ObjID, &TransformDesc);

	return S_OK;
}

void CLevel_ModelTest::Tick(_float fTimeDelta)
{
	// Play cinematic
	CCine_Manager::Get_Instance()->PlayCinematic(fTimeDelta);

	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Effect Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	CURSOR_CONTROL

		//if (GAMEINSTANCE->Get_KeyDown(DIK_F6)) {
		//	string strEffectName = "Rekka_Map_Spark_Ctr";
		//	_float4x4 matWorld = INIT_MATRIX;

		//	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);
		//}

	if (m_bLevelChanged)
	{
		GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)m_iNextLevel));
	}
}

HRESULT CLevel_ModelTest::Render()
{
	return S_OK;
}

void CLevel_ModelTest::Terminate()
{
	PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Get_Wire_Target().clear();
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CMapManager::Get_Instance()->Clear_BossObjects();
	m_pGameInstance.lock()->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Captive_Normals");
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
}

HRESULT CLevel_ModelTest::Ready_Layer_Camera(const wstring& strLayerTag)
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
	}
	return S_OK;
}

HRESULT CLevel_ModelTest::Ready_Layer_Map(const wstring& strLayerTag)
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		return E_FAIL;

	MISCOBJ_DESC desc{};
	XMStoreFloat4x4(&desc.worldMatrix, XMMatrixTranslationFromVector({ 1181.382f, 485.374f, 111.367f, 1.f }));
	GAMEINSTANCE->Add_Clone(LEVEL_MODELTEST, TEXT("Layer_Map"), CRekka_Generator::ObjID, &desc);
	CMapManager::Get_Instance()->Push_MapObjects(LEVEL_MODELTEST, strLayerTag);

	return S_OK;
}

HRESULT CLevel_ModelTest::Ready_Layer_Monster(const wstring& strLayerTag)
{
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(1192.09f, 487.398f, 111.292f, 1.f);
	GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Rekka::ObjID, &TransformDesc);
	return S_OK;
}

HRESULT CLevel_ModelTest::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
	//strcpy_s(LightDesc.szName, "Light_Directional_Main");
	//LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//LightDesc.g_fIntensity = 0.4f;

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.2f, 0.2f, 0.2f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.f, 0.f);
	LightDesc.g_fIntensity = 0.3f;

	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_ModelTest::Ready_Layer_Assist()
{
	PLAYERMANAGER->Reset_Assists();
	PLAYERMANAGER->Reset_808_Controller();
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(5.7f, 0.4f, 88.f, 1.f);
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Peppermint"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Peppermint"), CPeppermint::ObjID, &TransformDesc));

	return S_OK;
}

void CLevel_ModelTest::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_ModelTest> CLevel_ModelTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_ModelTest
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_ModelTest(pDevice, pContext) { }
	};

	shared_ptr<CLevel_ModelTest> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_ModelTest"));
		assert(false);
	}

	return pInstance;
}

void CLevel_ModelTest::Free()
{
}
