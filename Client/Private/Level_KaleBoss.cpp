#include "Level_KaleBoss.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SkyBox.h"

#include "Cine_Manager.h"

#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"

#include "Imgui_Manager.h"
#include "MapManager.h"
#include "Beat_Manager.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "GimmickManager.h"
#include "CCineSubTitleMgr.h"

CLevel_KaleBoss::CLevel_KaleBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevel_KaleBoss::~CLevel_KaleBoss()
{
	Free();
}

HRESULT CLevel_KaleBoss::Initialize()
{
	m_iLevelID = LEVEL_KALEBOSS;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);
	if (!CPlayer_Manager::Get_Instance()->Get_Player())
		CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));

	PLAYERMANAGER->Get_Player()->Set_MaxReverb(150.f);
	// Init Cinematics
	CCine_Manager::Get_Instance()->Initialize("../Bin/Resources/DataFiles/Cinematics/Kale_Boss/");
	CCineSubTitleMgr::Get_Instance()->Initialize_KaleBossCallBack();

	CUi_Mgr::Get_Instance()->LevelChange_Create(m_iLevelID);
	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Position(XMVectorSet(0, 2, 0, 1.f));

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Camera(static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main"))));

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	if (FAILED(Ready_Light()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Assist()))
		return E_FAIL;

	CBeat_Manager::Get_Instance()->Reset();
	//m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, L"InABlink", 0.7f);
	//m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::MUSIC, L"InABlink", true);
	CBeat_Manager::Get_Instance()->Set_Beat(0.402684f);
	CBeat_Manager::Get_Instance()->Queue_Sequence(L"Kale_Verse1");
	CBeat_Manager::Get_Instance()->Set_Volume(0.5f);
	// Start Cinematic

	CCine_Manager::Get_Instance()->StartCine("CS1205");

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_CineAttackSystem::ObjID, m_iLevelID, L"CUi", CUi_CineAttackSystem::ObjID);

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	return S_OK;
}

void CLevel_KaleBoss::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Effect Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	// Play cinematic
	CCine_Manager::Get_Instance()->PlayCinematic(fTimeDelta);

	CURSOR_CONTROL
}

HRESULT CLevel_KaleBoss::Render()
{
	return S_OK;
}

void CLevel_KaleBoss::Terminate()
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CMapManager::Get_Instance()->Clear_BossObjects();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	m_pGameInstance.lock()->Stop_Sound(SOUND_CHANNEL::MUSIC, L"InABlink");
}

HRESULT CLevel_KaleBoss::Ready_Layer_Camera(const wstring& strLayerTag)
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

HRESULT CLevel_KaleBoss::Ready_Layer_Map(const wstring& strLayerTag)
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		return E_FAIL;

	CMapManager::Get_Instance()->Push_MapObjects(LEVEL_KALEBOSS, strLayerTag);
	return S_OK;
}

HRESULT CLevel_KaleBoss::Ready_Layer_Monster(const wstring& strLayerTag)
{
	shared_ptr<CKaleBoss_ObjSystem> pInstance = static_pointer_cast
		<CKaleBoss_ObjSystem>(CGimmickManager::Get_Instance()->Get_GimmckObj
		(string(CKaleBoss_ObjSystem::ObjID.begin(), CKaleBoss_ObjSystem::ObjID.end())));

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(-5, -149.f, 0, 1);
	auto pKale = GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Monster"), CMonster_Kale::ObjID, &TransformDesc);

	pInstance->Set_KaleInfo(pKale->Get_Transform(), static_pointer_cast<CCharacterController>(pKale->Find_Component(L"Com_CCT")));
	pInstance->Set_PlayerInfo(CPlayer_Manager::Get_Instance()->Get_Player()->Get_Transform(), 
		static_pointer_cast<CCharacterController>(CPlayer_Manager::Get_Instance()->Get_Player()->Find_Component(L"Com_CCT")));

	TransformDesc.vPosition = _float4(15, -149.f, 0, 1);
	auto p808 = GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_808"), C808_Playable::ObjID, &TransformDesc);
	//static_pointer_cast<C808_Playable>(p808)->Take_Control();
	return S_OK;
}

HRESULT CLevel_KaleBoss::Ready_Light()
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

HRESULT CLevel_KaleBoss::Ready_Layer_Assist()
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

void CLevel_KaleBoss::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_KaleBoss> CLevel_KaleBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_KaleBoss
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_KaleBoss(pDevice, pContext) { }
	};

	shared_ptr<CLevel_KaleBoss> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_KaleBoss"));
		assert(false);
	}

	return pInstance;
}

void CLevel_KaleBoss::Free()
{
}
