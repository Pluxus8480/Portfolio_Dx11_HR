#include "Level_Mimosa.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SkyBox.h"

#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"

#include "Cine_Manager.h"
#include "Imgui_Manager.h"
#include "MapManager.h"
#include "Beat_Manager.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "GimmickManager.h"
#include "CCineSubTitleMgr.h"
CLevel_Mimosa::CLevel_Mimosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevel_Mimosa::~CLevel_Mimosa()
{
	Free();
}

HRESULT CLevel_Mimosa::Initialize()
{
	m_iLevelID = LEVEL_MIMOSA;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);
	if (!CPlayer_Manager::Get_Instance()->Get_Player())
		CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));

	// Init Cinematics
	CCine_Manager::Get_Instance()->Initialize("../Bin/Resources/DataFiles/Cinematics/Mimosa/");
	CCineSubTitleMgr::Get_Instance()->Initialize_MimosaCallBack();
	//CUi_Mgr::Get_Instance()->LevelChange_Create(m_iLevelID);
	// Set chai position
	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Position(XMVectorSet(0, 2.f, 0.f, 1.f));

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
	//m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain", 0.7f);
	//m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain", true);
	CBeat_Manager::Get_Instance()->Set_Beat(0.4054f);
	//BEATMANAGER->Queue_Sequence(L"Mimosa_Verse1");
	CBeat_Manager::Get_Instance()->Set_Volume(0.5f);

	/*
	CCine_Manager::Get_Instance()->AddLastCallback("CS0840", [&]()
		{
			CUi_StageResultSystem::Desc Desc;
			Desc.iNextLevel = LEVEL_KALESTAGE;
			CUi_Mgr::Get_Instance()->Add_Clone(CUi_StageResultSystem::ObjID, m_iLevelID,
				L"CUi", CUi_StageResultSystem::ObjID, &Desc);
		});
	*/

	// Start Cinematic
	
	/*
	CCine_Manager::Get_Instance()->StartCine("CS0820");
	*/

	/*
	CCine_Manager::Get_Instance()->IsInGame();
	CCine_Manager::Get_Instance()->AddStartCallback("CS0820", [&]() {});
	*/

	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	return S_OK;
}

void CLevel_Mimosa::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Effect Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	// Play cinematic
	CCine_Manager::Get_Instance()->PlayCinematic(fTimeDelta);

	FireCrack_Controler(fTimeDelta);
	LandFireCrack_Controler(fTimeDelta);
	StageShine_Controler(fTimeDelta);

	if(GAMEINSTANCE->Get_KeyDown(DIK_F2))
		ShowFireCrackTest(fTimeDelta);



	CURSOR_CONTROL

	if (m_bLevelChanged)
	{
		GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)m_iNextLevel));
	}
}

HRESULT CLevel_Mimosa::Render()
{
	return S_OK;
}

void CLevel_Mimosa::Terminate()
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CMapManager::Get_Instance()->Clear_BossObjects();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	CMapManager::Get_Instance()->Clear_BossObjects();
	m_pGameInstance.lock()->Stop_Sound(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain");
}

HRESULT CLevel_Mimosa::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Player::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(0.f, 0.f, -25.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(65.0f);
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

HRESULT CLevel_Mimosa::Ready_Layer_Map(const wstring& strLayerTag)
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		return E_FAIL;

	MISCOBJ_DESC desc{};
	XMStoreFloat4x4(&desc.worldMatrix, XMMatrixTranslationFromVector({ 1181.382f, 485.374f, 111.367f, 1.f }));
	CMapManager::Get_Instance()->Push_MapObjects(LEVEL_MIMOSA, strLayerTag);

	return S_OK;
}

HRESULT CLevel_Mimosa::Ready_Layer_Monster(const wstring& strLayerTag)
{
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(-22.f, 3.f, 2.f, 1.f);
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Mimosa::ObjID, &TransformDesc);
	TransformDesc.vPosition = _float4(-20.f, -2.f, 4.f, 1.f);

	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMusician_Robot::ObjID, &TransformDesc);

	return S_OK;
}

HRESULT CLevel_Mimosa::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.4f, 0.4f, 0.4f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.f, 0.f);
	LightDesc.g_fIntensity = 0.8f;

	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	_float4x4 matWorld;
	matWorld = INIT_MATRIX;
	matWorld._11 = 0.4f;
	matWorld._22 = 0.4f;
	matWorld._33 = 0.4f;


	EFFECTMANAGER->Call_Fixed_Union_Effect("LightMagic", &matWorld, 1.f);

	return S_OK;
}

HRESULT CLevel_Mimosa::Ready_Layer_Assist()
{
	PLAYERMANAGER->Reset_Assists();
	PLAYERMANAGER->Reset_808_Controller();
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(5.7f, 0.4f, 88.f, 1.f);
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Peppermint"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Peppermint"), CPeppermint::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Macaron"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Macaron"), CMacaron::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Korsica"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Korsica"), CKorsica::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_808_Controller(GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_808_Controller"), C808_Controller::ObjID, &TransformDesc));
	return S_OK;
}

void CLevel_Mimosa::FireCrack_Controler(_float fTimeDelta)
{

	_float4x4 matWorld;

	m_fFireCrack_1_Time += fTimeDelta;
	m_fFireCrack_2_Time += fTimeDelta;
	m_fFireCrack_3_Time += fTimeDelta;

	if (m_fFireCrack_1_Time > m_fFireCrack_1_TimeMax)
	{
		m_fFireCrack_1_Time = 0.f;

		_float fSize = GAMEINSTANCE->Random_Float(2.f, 8.f);

		matWorld = INIT_MATRIX;
		matWorld._11 = fSize;
		matWorld._22 = fSize;
		matWorld._33 = fSize;
		matWorld._41 = GAMEINSTANCE->Random_Float(200.f, 400.f);
		matWorld._42 = GAMEINSTANCE->Random_Float(100.f, 250.f);
		matWorld._43 = GAMEINSTANCE->Random_Float(-400.f, 400.f);

		switch (GAMEINSTANCE->Random_Int(1, 4))
		{
		case 1:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Orange", &matWorld, 1.f);
			break;
		case 2:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Miku", &matWorld, 1.f);
			break;
		case 3:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Purple", &matWorld, 1.f);
			break;
		case 4:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_30", &matWorld, 1.f);
			break;
		default:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_30", &matWorld, 1.f);
			break;

		}

	}

	if (m_fFireCrack_2_Time > m_fFireCrack_2_TimeMax)
	{
		m_fFireCrack_2_Time = 0.f;

		_float fSize = GAMEINSTANCE->Random_Float(2.f, 8.f);

		matWorld = INIT_MATRIX;
		matWorld._11 = fSize;
		matWorld._22 = fSize;
		matWorld._33 = fSize;
		matWorld._41 = GAMEINSTANCE->Random_Float(200.f, 400.f);
		matWorld._42 = GAMEINSTANCE->Random_Float(100.f, 250.f);
		matWorld._43 = GAMEINSTANCE->Random_Float(-400.f, 400.f);

		switch (GAMEINSTANCE->Random_Int(1, 4))
		{
		case 1:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Orange", &matWorld, 1.f);
			break;
		case 2:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Miku", &matWorld, 1.f);
			break;
		case 3:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Purple", &matWorld, 1.f);
			break;
		case 4:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_30", &matWorld, 1.f);
			break;
		default:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_30", &matWorld, 1.f);
			break;

		}
	}

	if (m_fFireCrack_3_Time > m_fFireCrack_3_TimeMax)
	{
		m_fFireCrack_3_Time = 0.f;

		_float fSize = GAMEINSTANCE->Random_Float(2.f, 8.f);

		matWorld = INIT_MATRIX;
		matWorld._11 = fSize;
		matWorld._22 = fSize;
		matWorld._33 = fSize;
		matWorld._41 = GAMEINSTANCE->Random_Float(200.f, 400.f);
		matWorld._42 = GAMEINSTANCE->Random_Float(100.f, 250.f);
		matWorld._43 = GAMEINSTANCE->Random_Float(-400.f, 400.f);

		switch (GAMEINSTANCE->Random_Int(1, 4))
		{
		case 1:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Orange", &matWorld, 1.f);
			break;
		case 2:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Miku", &matWorld, 1.f);
			break;
		case 3:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_20_Purple", &matWorld, 1.f);
			break;
		case 4:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_30", &matWorld, 1.f);
			break;
		default:
			EFFECTMANAGER->Call_Fixed_Union_Effect("FireCracker_30", &matWorld, 1.f);
			break;

		}
	}



}

void CLevel_Mimosa::LandFireCrack_Controler(_float fTimeDelta)
{
	m_fLandFireCrack_Time += fTimeDelta;

	if (m_fLandFireCrack_TimeMax <= m_fLandFireCrack_Time) {
		m_fLandFireCrack_Time = 0.f;

		if(m_bFireCrackPattern)
			SideFireCrack();
		else
			FrontFireCrack();
	}

}

void CLevel_Mimosa::StageShine_Controler(_float fTimeDelta)
{
	_float4x4 matWorld;

	m_fStageShine_Time += fTimeDelta;

	if (m_fStageShine_Time > m_fStageShine_TimeMax)
	{
		m_fStageShine_Time = 0.f;

		matWorld = INIT_MATRIX;
		matWorld._41 = -37.f;
		matWorld._42 = 2.f;
		matWorld._43 = 0.f;
		
		EFFECTMANAGER->Call_Fixed_Union_Effect("Stage_Shine", &matWorld, 1.f);
		

	}
}

void CLevel_Mimosa::ShowFireCrackTest(_float fTimeDelta)
{
	string strEffectName = "Base_FireCracker_Yellow";

	_float fSize = 1.5f;
	_float fSpeed = 0.5f;

	_float4x4 matWorld = INIT_MATRIX;
	matWorld._11 = fSize;
	matWorld._22 = 1.f;
	matWorld._33 = fSize;

	matWorld._41 = 1.f;
	matWorld._42 = 0.f;

	matWorld._43 = -8.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = -11.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = -14.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = -17.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = -20.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = 8.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = 11.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = 14.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = 17.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

	matWorld._43 = 20.f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

}

void CLevel_Mimosa::SideFireCrack()
{
	string strEffectName;

	switch (GAMEINSTANCE->Random_Int(1, 3))
	{
	case 1:
		strEffectName = "Base_FireCracker_Yellow";
		break;
	case 2:
		strEffectName = "Base_FireCracker_Purple";
		break;
	case 3:
		strEffectName = "Base_FireCracker_Emerald";
		break;
	default:
		strEffectName = "Base_FireCracker_Rainbow";
		break;
	}

	_float4x4 matWorld = INIT_MATRIX;
	matWorld._42 = -1.37f;

	matWorld._41 = -27.02f;
	matWorld._43 = 40.16f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

	matWorld._43 = -40.46f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

	matWorld._41 = -28.29f;
	matWorld._43 = 40.16f;

	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

	matWorld._43 = -40.46f;
	EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);
	
	m_bFireCrackPattern = !m_bFireCrackPattern;
}

void CLevel_Mimosa::FrontFireCrack()
{
	string strEffectName;

	switch (GAMEINSTANCE->Random_Int(1, 3))
	{
	case 1:
		strEffectName = "Base_FireCracker_Yellow";
		break;
	case 2:
		strEffectName = "Base_FireCracker_Purple";
		break;
	case 3:
		strEffectName = "Base_FireCracker_Emerald";
		break;
	default:
		strEffectName = "Base_FireCracker_Rainbow";
		break;
	}

	_float4x4 matWorld = INIT_MATRIX;
	matWorld._42 = -0.49f;
	
	if (GAMEINSTANCE->Random_Bool()) {
		matWorld._41 = -10.614f;
		matWorld._43 = 40.506f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

		matWorld._41 = -5.620f;
		matWorld._43 = -37.7f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

		matWorld._41 = -8.3f;
		matWorld._43 = -39.18f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

		matWorld._41 = -3.55f;
		matWorld._43 = -36.27f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);
	}
	else {
		matWorld._41 = -8.11f;
		matWorld._43 = 39.24f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

		matWorld._41 = -3.47f;
		matWorld._43 = 36.16f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

		matWorld._41 = -6.f;
		matWorld._43 = -37.88f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);

		matWorld._41 = -1.34f;
		matWorld._43 = -37.54f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, 1.f);
	}
	m_bFireCrackPattern = !m_bFireCrackPattern;
}

void CLevel_Mimosa::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_Mimosa> CLevel_Mimosa::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_Mimosa
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Mimosa(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Mimosa> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Mimosa"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Mimosa::Free()
{
}
