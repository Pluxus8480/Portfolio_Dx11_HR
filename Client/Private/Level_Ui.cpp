#include "Level_Ui.h"
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
#include "iostream"

CLevel_Ui::CLevel_Ui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Ui::~CLevel_Ui()
{
	Free();
}

HRESULT CLevel_Ui::Initialize()
{
	m_iLevelID = LEVEL_UITOOL;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);
	/*Player */
	CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	static_pointer_cast<CChai>(CPlayer_Manager::Get_Instance()->Get_Player())->Set_Player_Camera(static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main"))));

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	Ready_Ui();

	PLAYERMANAGER->TelePort();

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	/*m_pGameInstance.lock()->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT,  L"ZipLineRail", TEXT("../Bin/Resources/Sounds/MapObject/ZipLineRail.mp3"));*/

	return S_OK;
}

void CLevel_Ui::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	m_fTime += fTimeDelta;
	//if (GAMEINSTANCE->Get_KeyDown(DIK_F7))
	//{	

	//	CPlayer_Manager::SkillDesc EquipSkillDesc;
	//	EquipSkillDesc.ConsumeReverb = 1;
	//	//EquipSkillDesc.SkillName = L"픽 미 업!";
	//	EquipSkillDesc.SkillName = L"Pickmeup!";
	//	EquipSkillDesc.SkillType = CPlayer_Manager::SkillType::Recover;

	//	CPlayer_Manager::SkillDesc NonEquipSkillDesc;
	//	NonEquipSkillDesc.ConsumeReverb = 3;
	//	//NonEquipSkillDesc.SkillName = L"오버드라이브";
	//	NonEquipSkillDesc.SkillName = L"Overdrive";
	//	NonEquipSkillDesc.SkillType = CPlayer_Manager::SkillType::Attack;
	//	static_pointer_cast<CUi_SkillChangeSystem>(CUi_Mgr::Get_Instance()->Find_UI
	//	(CUi_SkillChangeSystem::ObjID))->Set_Change(EquipSkillDesc, NonEquipSkillDesc);
	//}


	return;
}

HRESULT CLevel_Ui::Render()
{

	return S_OK;
}

HRESULT CLevel_Ui::Ready_Layer_Camera(const wstring& strLayerTag)
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

	return S_OK;
}

HRESULT CLevel_Ui::Ready_Layer_Map(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		assert(false);

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	staticDesc.strModelTag = TEXT("SM_TPAreaGroundA_IZ_03");
	staticDesc.worldMatrix = Identity;
	staticDesc.worldMatrix._41 = 25.f;
	staticDesc.worldMatrix._42 = 0.f;
	staticDesc.worldMatrix._43 = 0.f;

	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_UITOOL, strLayerTag, CMapObject_Static::ObjID, &staticDesc)))
		assert(false);

	return S_OK;
}

HRESULT CLevel_Ui::Ready_Layer_Monster(const wstring& strLayerTag)
{
	//CTransform::TRANSFORM_DESC	TransformDesc{};
	//TransformDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	///* Monsters */
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Saber::ObjID, &TransformDesc);
	//TransformDesc.vPosition = _float4(0.f, 0.f, 10.f, 1.f);
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Baboo::ObjID, &TransformDesc);
	return S_OK;
}

HRESULT CLevel_Ui::Ready_Light()
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

void CLevel_Ui::Ready_Ui()
{
	auto pGI = GAMEINSTANCE;
	CBeat_Manager::Get_Instance()->Set_Beat(0.4f);
	CUi_Mgr::Get_Instance()->LevelChange_Create(m_iLevelID);
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_CNMNRazer::ObjID, m_iLevelID, L"CUi", CUi_CNMNRazer::ObjID);

	 CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);
	//CUi_Mgr::Get_Instance()->Set_Active(CUi_DashEffect::ObjID, true);

	
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_CNMNRazer::ObjID, m_iLevelID, L"CUi", CUi_CNMNRazer::ObjID);
	/*CUi_Mgr::Get_Instance()->Add_Clone(CUi_KaleSystem::ObjID, m_iLevelID, L"CUi", CUi_KaleSystem::ObjID, nullptr);*/

	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_StunGaugeSystem::ObjID, m_iLevelID, L"CUi", CUi_StunGaugeSystem::ObjID, nullptr);

	//����
	// 
	//CUi_EnemySpecialAttack::Desc Desc;
	//Desc.iParryCount = 5;
	//vector<_float> Gap = { 2, 2, 1, 0.5, 1 };
	// vector<_bool> Parry = { false, true, false, true, false };
	//Desc.bParry = Parry;
	//Desc.TargetTransform;
	//Desc.fDistance = { 50, 50 };
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_EnemySpecialAttack::ObjID,
	//	m_iLevelID, L"CUi", CUi_EnemySpecialAttack::ObjID, &Desc);
	// 

	/*CUi_TalkSystem::CUi_TalkSystemInfo TalkADesc{};
	TalkADesc.wsFileName = L"Talk_File_1.csv";
	TalkADesc.endCallback = [&]() {};
	CUi_Mgr::Get_Instance()->Add_Clone(CUi_TalkSystem::ObjID, m_iLevelID, L"CUi", CUi_TalkSystem::ObjID, &TalkADesc);*/
}

void CLevel_Ui::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_Ui> CLevel_Ui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_Ui
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Ui(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Ui> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Ui"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Ui::Free()
{

}
