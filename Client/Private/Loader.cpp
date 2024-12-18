#include "Loader.h"

#include "GameInstance.h"
#include "Load_Include.h"
#include "Component_Manager.h"
#include "Effect_Manager.h"
#include "SkyBox.h"

#include "ResourceWizard.h"
#include "Imgui_Manager.h"
#include "MapManager.h"
#include "GimmickManager.h"
//#include "Monster.h"
//#include "Player.h"
//#include "Camera.h"
//#include "Effect.h"
//#include "Sky.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

CLoader::~CLoader()
{
	Free();
}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);
unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	return pLoader->Loading();
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	CMapManager::Get_Instance()->Set_NextLevel((_uint)eNextLevelID);

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT		hr{};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level();
		break;
	case LEVEL_TEST:
		hr = Loading_For_Test_Level();
		break;
	case LEVEL_ANIMTOOL:
		hr = Loading_For_AnimTool_Level();
		break;
	case LEVEL_MAPTOOL:
		hr = Loading_For_MapTool_Level();
		break;
	case LEVEL_EFFECTTOOL:
		hr = Loading_For_EffectTool_Level();
		break;
	case LEVEL_UITOOL:
		hr = Loading_For_UITool_Level();
		break;
	case LEVEL_LIFT:
		hr = Loading_For_Lift_Level();
		break;
	case LEVEL_KORSICA:
		hr = Loading_For_KorsicaTest_Level();
		break;
	case LEVEL_MODELTEST:
		hr = Loading_For_ModelTest_Level();
		break;
	case LEVEL_KALEBOSS:
		hr = Loading_For_KaleBoss_Level();
		break;
	case LEVEL_CINETOOL:
		hr = Loading_For_CineTool_Level();
		break;
	case LEVEL_MIMOSA:
		hr = Loading_For_MimosaBoss_Level();
		break;
	case LEVEL_GIMMICTEST:
		hr = Loading_For_GimmicTest_Level();
		break;
	case LEVEL_KALESTAGE:
		hr = Loading_For_KaleStage_Level();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	assert(!FAILED(hr));

	return 0;
}

HRESULT CLoader::Load_Ui()
{
	if (FAILED(Load_Ui_Skill_Textures()))
		assert(false);

	if (FAILED(Load_Ui_Skill_Prototype()))
		assert(false);

	return S_OK;
}

HRESULT CLoader::Load_Ui_Skill_Textures()
{
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Hp_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Hp/hud_boss_health_bar_gauge.png")))))
		assert(false);*/

	return S_OK;
}

HRESULT CLoader::Load_Ui_Skill_Prototype()
{
	return S_OK;
}

HRESULT CLoader::Load_Object_Textures()
{
	return S_OK;
}

HRESULT CLoader::Load_Model()
{
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/Adrian/Adrian.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PistolGunnerWeapon"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/PistolGunnerWeapon/PistolGunnerWeapon.fbx", LocalTransformMatrix))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Loading..."));
	auto pGameInstance = m_pGameInstance.lock();
	Load_Object_Textures();

	/*m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/Stage1.ogg", TEXT("Sound_BGM"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/Stage2.ogg", TEXT("Sound_BGM1"));*/

	//lstrcpy(m_szLoadingText, TEXT("���̴���(��) �ε� �� �Դϴ�."));

	//lstrcpy(m_szLoadingText, TEXT("Camera Load"));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Tool"),
		CCamera_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//lstrcpy(m_szLoadingText, TEXT("����(��) �ε� �� �Դϴ�."));
	CResourceWizard::LoadResources(CUI_Event_Cut::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	Load_Ui();
	Load_Model();
	CResourceWizard::LoadResources(CBackGround::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MapInterAction::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_SkillChangeSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext); 
	CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_SkillCutScene::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_SpecialComboAttackSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickMeUp_Can::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_GearSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Test_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Level Loading"));
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GridBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/GridBox.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GridBox"),
		CGridBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickMeUp_Can::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMonster_Saber::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Gunner::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMonster_Baboo::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CBarrier::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	/* Map */
	wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	if (FAILED(pGameInstance->Add_Prototype(strSMObj, CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_TEST))))
		return E_FAIL;

	/* General */
	CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CMapObject_Anim::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CButton_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CFloorSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWoodenBox::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickUpItem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEnemyGate::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSignBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CInvisibleWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattleAreaDoor_PD::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSlate_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGrass::ObjID, LEVEL_TEST, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFree_SignBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	/* Area01 */
	CResourceWizard::LoadResources(CProdDeptDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBulbTrigger::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMovingWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFirstBattleWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	/* Area02 */
	CResourceWizard::LoadResources(CCommonSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwitchFence::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGearWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLargeElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CComboSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CComboSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSign::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	/* Area03 */
	CResourceWizard::LoadResources(CCatwalk_Bridge::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	/* 2D Scroll */
	CResourceWizard::LoadResources(CTalkTrigger::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSmoke_Cover::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CHanging_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSignBoard_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CFan_Blade::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFan_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFadeElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	/* Area04 */
	CResourceWizard::LoadResources(CLaserCircuit::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserPod::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserPillar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserReward::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGlassWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CQACenterDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGenerator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLevelUp_Machine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGaraElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CConveyorBelt::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MapInterAction::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CSlate_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CTunnel::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythm_CleanSystem::ObjID, LEVEL_TEST, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_FightString::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_FinalResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CODS_Controller::ObjID, LEVEL_TEST, pGameInstance, m_pDevice, m_pContext);
	lstrcpy(m_szLoadingText, TEXT("Load Map"));
	pGameInstance->Initialize_Octree(32.f, 5, _float3{ 256.f,256.f,256.f });
	auto pMapManager = CMapManager::Get_Instance();
	pMapManager->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_A01.bin");
	pMapManager->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_A02.bin");
	pMapManager->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_A03.bin");
	pMapManager->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_MiddleComb.bin");
	pMapManager->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_A04.bin");
	pMapManager->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_A04B.bin");

	pMapManager->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/St01_2DScroll.bin");
	//pMapManager ->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/Boss_Rekka.bin");

	pMapManager->Clear_Containers();

	// Load cinematic resources
	filesystem::path pathCineStageIntro("../Bin/Resources/DataFiles/Cinematics/Intro/");
	for (const auto& entry : filesystem::directory_iterator(pathCineStageIntro))
	{
		if (entry.is_regular_file())
		{
			string strFileName = entry.path().filename().string();
			string jsonExtension = ".json";
			if (strFileName.find(jsonExtension) != string::npos)
			{
				string filePath = entry.path().string();

				ifstream fileScenario(filePath);
				auto jsonImpoter = json::parse(fileScenario);
				fileScenario.close();

				auto actorArray = jsonImpoter["Actors"];
				for (auto actor : actorArray)
				{
					string strModelTag = actor["ModelTag"];
					string strModelPath = actor["ModelPath"];

					_matrix	preTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
					wstring wstrModelTag = wstring(strModelTag.begin(), strModelTag.end());
					GAMEINSTANCE->Add_Prototype(
						LEVEL_TEST, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strModelPath.c_str(), preTransformMatrix)
					);
				}
			}
			else
			{
				// Sound
				string filePath = entry.path().string();
				wstring wFilePath = wstring(filePath.begin(), filePath.end());

				strFileName.erase(strFileName.length() - 4);
				wstring wstrFileName = wstring(strFileName.begin(), strFileName.end());

				GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, wstrFileName, wFilePath);
			}
		}
	}

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_AnimTool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MapTool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Tool Loading"));
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sky_Cube_Ref"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/skybox.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(CMapObject_Static::ObjID,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_MAPTOOL))))
		return E_FAIL;

	CResourceWizard::LoadResources(CMapObject_Static::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMapObject_Anim::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CProdDeptDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBulbTrigger::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMovingWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonsterSpawner::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpawnTrigger_Monster::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCommonSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwitchFence::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CButton_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCatwalk_Bridge::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSmoke_Cover::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CConveyorBelt::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSlate_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLargeElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CComboSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CComboSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGearWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserCircuit::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserPod::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserPillar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserReward::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGlassWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWoodenBox::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickUpItem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CHanging_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CQACenterDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CInvisibleWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFirstBattleWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFadeElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEnemyGate::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGenerator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSign::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFan_Blade::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFan_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSignBoard_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSignBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattleAreaDoor_PD::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGaraElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLevelUp_Machine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CInvisibleCylinderWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CSignPlane::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CRekka_Logo::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_GenCover::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Monitor_S::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Monitor_L::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_PhaseText::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_MonitorBase::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekkaRoomDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_LightShaft::ObjID, LEVEL_MODELTEST, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CKrscRoomWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythmWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDMGConsole::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CWireMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CZipLine_Rail::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMovingFloor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSteelFrame_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKrsc_Generator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpectraMeshGate::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBuiltIn_Magnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCraneMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEmissiveArrow::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpectra_FanMachine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFree_SignBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot2::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot3::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot_Arm::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythmJump::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKale_Antenna::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_Floor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosaWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermintContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaronCrane::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleElevatorGenerator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CContainerBreakPannel::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotPunchBreakWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotPunchStaticWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotBeamContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CScroll2DContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRailContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattle3rdAfter::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattle3rdFloor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleBoss_CineObj::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMusician_Robot::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_RingText::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_BackLEDText::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_RoofLEDText::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_Visualizer::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_BackMonitor::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_Speaker::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_LightShaft::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	
	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CRhythm_RobotArm::ObjID, LEVEL_MAPTOOL, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythm_CleanSystem::ObjID, LEVEL_MAPTOOL, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CGrass::ObjID, LEVEL_MAPTOOL, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleBoss_ObjSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	pGameInstance->Initialize_Octree(32.f, 5, _float3{ 256.f,256.f,256.f });

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_EffectTool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Tool Loading"));
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(CMapObject_Static::ObjID,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_EFFECTTOOL))))
		return E_FAIL;

	_matrix Scaling = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECTTOOL, TEXT("SM_TPAreaGroundA_IZ_03"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Map/Stage01/SM_TPAreaGroundA_IZ_03.FBX", Scaling))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_UITool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Level_Ui Loading"));
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(CMapObject_Static::ObjID,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_UITOOL))))
		return E_FAIL;

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerPartnerGauge::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CNMNRazer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	
	//CResourceWizard::LoadResources(CUi_StunGaugeSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_GeneratorSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_TreasureBoxSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_ScoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_FinalResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	/*CResourceWizard::LoadResources(CUi_KaleSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);*/
	//CResourceWizard::LoadResources(CUi_FightString::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_GearSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_RekkaSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_KorsicaSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	_matrix Scaling = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SM_TPAreaGroundA_IZ_03"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Map/Stage01/SM_TPAreaGroundA_IZ_03.FBX", Scaling))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_KorsicaTest_Level()
{
	lstrcpy(m_szLoadingText, TEXT("KorsicaLevel Loading"));
	auto pGameInstance = m_pGameInstance.lock();
	pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")));

	pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GridBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/GridBox.dds")));

	pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"), CSkyBox::Create(m_pDevice, m_pContext));

	pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GridBox"), CGridBox::Create(m_pDevice, m_pContext));

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	//CResourceWizard::LoadResources(CMonster_Saber::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CMonster_Baboo::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CBarrier::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	//CResourceWizard::LoadResources(CMapObject_Static::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_KorsicaSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	if (FAILED(pGameInstance->Add_Prototype(strSMObj,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_KORSICA))))
		assert(false);

	CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKrscRoomWall::ObjID, LEVEL_KORSICA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSignPlane::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CInvisibleCylinderWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythmWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDMGConsole::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	// Load cinematic resources
	filesystem::path pathCineKorsica("../Bin/Resources/DataFiles/Cinematics/Korsica_Boss/");
	for (const auto& entry : filesystem::directory_iterator(pathCineKorsica))
	{
		if (entry.is_regular_file())
		{
			string strFileName = entry.path().filename().string();
			string jsonExtension = ".json";
			if (strFileName.find(jsonExtension) != string::npos)
			{
				string filePath = entry.path().string();

				ifstream fileScenario(filePath);
				auto jsonImpoter = json::parse(fileScenario);
				fileScenario.close();

				auto actorArray = jsonImpoter["Actors"];
				for (auto actor : actorArray)
				{
					string strModelTag = actor["ModelTag"];
					string strModelPath = actor["ModelPath"];

					_matrix	preTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
					wstring wstrModelTag = wstring(strModelTag.begin(), strModelTag.end());
					GAMEINSTANCE->Add_Prototype(
						LEVEL_MIMOSA, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strModelPath.c_str(), preTransformMatrix)
					);
				}
			}
			else
			{
				// Sound
				string filePath = entry.path().string();
				wstring wFilePath = wstring(filePath.begin(), filePath.end());

				strFileName.erase(strFileName.length() - 4);
				wstring wstrFileName = wstring(strFileName.begin(), strFileName.end());

				GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, wstrFileName, wFilePath);
			}
		}
	}

	auto pMapManager = CMapManager::Get_Instance();
	pMapManager->Level_MapLoad(LEVEL_STATIC, "../Bin/DataFiles/Map/Boss_Korsica.bin");
	pMapManager->Clear_Containers();

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_ModelTest_Level()
{
	auto pGameInstance = m_pGameInstance.lock();
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
	//	CSkyBox::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	if (FAILED(pGameInstance->Add_Prototype(strSMObj,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_MODELTEST))))
		return E_FAIL;

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCNMN_Playable::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);


	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RekkaSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_ScoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MapInterAction::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMonster_Rekka::ObjID, LEVEL_MODELTEST, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CBattleAreaDoor_PD::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	pGameInstance->Initialize_Octree(32.f, 5, _float3{ 256.f,256.f,256.f });

	CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Logo::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Generator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_GenCover::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Monitor_S::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Monitor_L::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_PhaseText::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_MonitorBase::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekkaRoomDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_LightShaft::ObjID, LEVEL_MODELTEST, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CWireMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_FinalResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CODS_Controller::ObjID, LEVEL_MODELTEST, pGameInstance, m_pDevice, m_pContext);
	// Load cinematic resources
	filesystem::path pathCineRekka("../Bin/Resources/DataFiles/Cinematics/Rekka/");
	for (const auto& entry : filesystem::directory_iterator(pathCineRekka))
	{
		if (entry.is_regular_file())
		{
			string strFileName = entry.path().filename().string();
			string jsonExtension = ".json";
			if (strFileName.find(jsonExtension) != string::npos)
			{
				string filePath = entry.path().string();

				ifstream fileScenario(filePath);
				auto jsonImpoter = json::parse(fileScenario);
				fileScenario.close();

				auto actorArray = jsonImpoter["Actors"];
				for (auto actor : actorArray)
				{
					string strModelTag = actor["ModelTag"];
					string strModelPath = actor["ModelPath"];

					_matrix	preTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
					wstring wstrModelTag = wstring(strModelTag.begin(), strModelTag.end());
					GAMEINSTANCE->Add_Prototype(
						LEVEL_MODELTEST, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strModelPath.c_str(), preTransformMatrix)
					);
				}
			}
			else
			{
				// Sound
				string filePath = entry.path().string();
				wstring wFilePath = wstring(filePath.begin(), filePath.end());

				strFileName.erase(strFileName.length() - 4);
				wstring wstrFileName = wstring(strFileName.begin(), strFileName.end());

				GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, wstrFileName, wFilePath);
			}
		}
	}

	auto pMapManager = CMapManager::Get_Instance();
	pMapManager->Level_MapLoad(LEVEL_MODELTEST, "../Bin/DataFiles/Map/Boss_Rekka.bin");
	pMapManager->Clear_Containers();

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Lift_Level()
{
	lstrcpy(m_szLoadingText, TEXT("LiftLevel Loading"));

	auto pGameInstance = m_pGameInstance.lock();
	pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")));

	pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GridBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/GridBox.dds")));


	pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext));

	pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GridBox"),
		CGridBox::Create(m_pDevice, m_pContext));

	wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	if (FAILED(pGameInstance->Add_Prototype(strSMObj,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_LIFT))))
		return E_FAIL;

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMonster_Saber::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMonster_Baboo::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Gunner::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CBarrier::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_FinalResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_ScoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TramTimer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPickUpItem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CSlate_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CTunnel::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLift::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBrake_Part::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPanel_Lift::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPanel_Warning::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEnemy_Lift::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CODS_Controller::ObjID, LEVEL_LIFT, pGameInstance, m_pDevice, m_pContext);
	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_KaleBoss_Level()
{
	auto pGameInstance = m_pGameInstance.lock();

	wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	if (FAILED(pGameInstance->Add_Prototype(strSMObj,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_KALEBOSS))))
		return E_FAIL;

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_KaleSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_ScoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MapInterAction::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StunGaugeSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CineAttackSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMonster_Kale::ObjID, LEVEL_KALEBOSS, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Playable::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CBattleAreaDoor_PD::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	pGameInstance->Initialize_Octree(32.f, 5, _float3{ 256.f,256.f,256.f });

	CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDMGConsole::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFree_SignBoard::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CWireMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKale_FireWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleBoss_ObjSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
		
	CResourceWizard::LoadResources(CKale_BigSword::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKale_Antenna::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CODS_Controller::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickUpItem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	// Load cinematic resources
	filesystem::path pathCineKaleBoss("../Bin/Resources/DataFiles/Cinematics/Kale_Boss/");
	for (const auto& entry : filesystem::directory_iterator(pathCineKaleBoss))
	{
		if (entry.is_regular_file())
		{
			string strFileName = entry.path().filename().string();
			string jsonExtension = ".json";
			if (strFileName.find(jsonExtension) != string::npos)
			{
				string filePath = entry.path().string();

				ifstream fileScenario(filePath);
				auto jsonImpoter = json::parse(fileScenario);
				fileScenario.close();

				auto actorArray = jsonImpoter["Actors"];
				for (auto actor : actorArray)
				{
					string strModelTag = actor["ModelTag"];
					string strModelPath = actor["ModelPath"];

					_matrix	preTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
					wstring wstrModelTag = wstring(strModelTag.begin(), strModelTag.end());
					GAMEINSTANCE->Add_Prototype(
						LEVEL_KALEBOSS, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strModelPath.c_str(), preTransformMatrix)
					);
				}
			}
			else
			{
				// Sound
				string filePath = entry.path().string();
				wstring wFilePath = wstring(filePath.begin(), filePath.end());

				strFileName.erase(strFileName.length() - 4);
				wstring wstrFileName = wstring(strFileName.begin(), strFileName.end());

				GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, wstrFileName, wFilePath);
			}
		}
	}

	auto pMapManager = CMapManager::Get_Instance();
	pMapManager->Level_MapLoad(LEVEL_KALEBOSS, "../Bin/DataFiles/Map/Boss_Kale.bin");

	CGimmickManager::Get_Instance()->Add_GimmickObj(static_pointer_cast<CKaleBoss_ObjSystem>
		(m_pGameInstance.lock()->Add_Clone_Return(m_eNextLevelID, L"Obj", CKaleBoss_ObjSystem::ObjID)));


	pMapManager->Clear_Containers();

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_CineTool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Cine Load"));
	auto pGameInstance = m_pGameInstance.lock();

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sky_Cube_Ref"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/skybox.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(CMapObject_Static::ObjID,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_CINETOOL))))
		return E_FAIL;

	CResourceWizard::LoadResources(CMapObject_Static::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMapObject_Anim::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CProdDeptDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBulbTrigger::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMovingWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonsterSpawner::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpawnTrigger_Monster::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCommonSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwitchFence::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CButton_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCatwalk_Bridge::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSmoke_Cover::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CConveyorBelt::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSlate_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLargeElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CComboSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CComboSwitchButton::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGearWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserCircuit::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserPod::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserPillar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaserReward::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGlassWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWoodenBox::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickUpItem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CHanging_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CQACenterDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CInvisibleWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFirstBattleWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFadeElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEnemyGate::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGenerator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSign::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFan_Blade::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFan_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSignBoard_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSignBoard::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattleAreaDoor_PD::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGaraElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLevelUp_Machine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CInvisibleCylinderWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CSignPlane::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CRekka_Logo::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_GenCover::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Monitor_S::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Monitor_L::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_PhaseText::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_MonitorBase::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekkaRoomDoor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_LightShaft::ObjID, LEVEL_MODELTEST, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CKrscRoomWall::ObjID, LEVEL_CINETOOL, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythmWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDMGConsole::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CWireMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CZipLine_Rail::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMovingFloor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSteelFrame_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKrsc_Generator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpectraMeshGate::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBuiltIn_Magnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCraneMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEmissiveArrow::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpectra_FanMachine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot2::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot3::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot_Arm::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythmJump::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKale_Antenna::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_Floor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosaWall::ObjID, LEVEL_CINETOOL, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermintContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaronCrane::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleElevatorGenerator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CContainerBreakPannel::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotPunchBreakWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotPunchStaticWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotBeamContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CScroll2DContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRailContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattle3rdAfter::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattle3rdFloor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleBoss_CineObj::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CineAttackSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CRhythm_RobotArm::ObjID, LEVEL_CINETOOL, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythm_CleanSystem::ObjID, LEVEL_CINETOOL, pGameInstance, m_pDevice, m_pContext);

	pGameInstance->Initialize_Octree(32.f, 5, _float3{ 256.f,256.f,256.f });

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MimosaBoss_Level()
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sky_Cube_Ref"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/skybox.dds")))))
		return E_FAIL;
	wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	if (FAILED(pGameInstance->Add_Prototype(strSMObj,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_MIMOSA))))
		return E_FAIL;

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RekkaSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_ScoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MapInterAction::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CMonster_Mimosa::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);

	pGameInstance->Initialize_Octree(32.f, 5, _float3{ 256.f,256.f,256.f });

	CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	
	CResourceWizard::LoadResources(CMimosa_Floor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_MimosaSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_SpecialComboAttackSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Mimosa::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MImosaMusicBattleSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_Letter_Bomb::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBackDancer::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CMonster_Baboo::ObjID, LEVEL_MODELTEST, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUI_MimosaCutin::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMusician_Robot::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMusician_Instrument::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosaWall::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_RingText::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_BackLEDText::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_RoofLEDText::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_Visualizer::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_BackMonitor::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_Speaker::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMimosa_LightShaft::ObjID, LEVEL_MIMOSA, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickMeUp_Can::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CODS_Controller::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_FinalResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	// Load cinematic resources
	filesystem::path pathCineMimosa("../Bin/Resources/DataFiles/Cinematics/Mimosa/");
	for (const auto& entry : filesystem::directory_iterator(pathCineMimosa))
	{
		if (entry.is_regular_file())
		{
			string strFileName = entry.path().filename().string();
			string jsonExtension = ".json";
			if (strFileName.find(jsonExtension) != string::npos)
			{
				string filePath = entry.path().string();

				ifstream fileScenario(filePath);
				auto jsonImpoter = json::parse(fileScenario);
				fileScenario.close();

				auto actorArray = jsonImpoter["Actors"];
				for (auto actor : actorArray)
				{
					string strModelTag = actor["ModelTag"];
					string strModelPath = actor["ModelPath"];

					_matrix	preTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
					wstring wstrModelTag = wstring(strModelTag.begin(), strModelTag.end());
					GAMEINSTANCE->Add_Prototype(
						LEVEL_MIMOSA, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strModelPath.c_str(), preTransformMatrix)
					);
				}
			}
			else
			{
				// Sound
				string filePath = entry.path().string();
				wstring wFilePath = wstring(filePath.begin(), filePath.end());

				strFileName.erase(strFileName.length() - 4);
				wstring wstrFileName = wstring(strFileName.begin(), strFileName.end());

				GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, wstrFileName, wFilePath);
			}
		}
	}

	auto pMapManager = CMapManager::Get_Instance();
	pMapManager->Level_MapLoad(LEVEL_MIMOSA, "../Bin/DataFiles/Map/MimosaBoss_Final.bin");
	pMapManager->Clear_Containers();

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GimmicTest_Level()
{
	lstrcpy(m_szLoadingText, TEXT("GimmicTestLoading"));
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(CMapObject_Static::ObjID,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_GIMMICTEST))))
		return E_FAIL;

	//wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	//if (FAILED(pGameInstance->Add_Prototype(strSMObj,
	//	CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_GIMMICTEST))))
	//	return E_FAIL;
	//CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_SuccessSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MapInterAction::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CCommonSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CZipLine_Rail::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleBoss_ObjSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSteelFrame_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CWireMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_SwordMachine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwordMachine_Sword::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CRhythmJump::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Blaze::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CChai_Random_Sound::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickMeUp_Can::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLevelUp_Machine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StunGaugeSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CODS_Controller::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	//_matrix Scaling = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GIMMICTEST, TEXT("SM_TPAreaGroundA_IZ_03"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Map/Stage01/SM_TPAreaGroundA_IZ_03.FBX", Scaling))))
	//	return E_FAIL;
	
	_matrix Scaling = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GIMMICTEST, TEXT("SM_TPAreaGroundA_IZ_03"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/Models/Map/Stage01/SM_TPAreaGroundA_IZ_03.FBX", Scaling))))
		return E_FAIL;

	//CMapManager::Get_Instance()->Level_MapLoad(LEVEL_GIMMICTEST, "../Bin/DataFiles/Map/MapData01.bin");

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_KaleStage_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Kale Stage Loading"));
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Test/city_box.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	wstring strSMObj = CMapObject_Static::ObjID + to_wstring((_uint)m_eNextLevelID);
	if (FAILED(pGameInstance->Add_Prototype(strSMObj,
		CMapObject_Static::Create(m_pDevice, m_pContext, LEVEL_KALESTAGE))))
		return E_FAIL;

	CResourceWizard::LoadResources(strSMObj, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	// Load cinematic resources
	filesystem::path pathCineKaleStage("../Bin/Resources/DataFiles/Cinematics/Kale_Stage/");
	for (const auto& entry : filesystem::directory_iterator(pathCineKaleStage))
	{
		if (entry.is_regular_file())
		{
			string strFileName = entry.path().filename().string();
			string jsonExtension = ".json";
			if (strFileName.find(jsonExtension) != string::npos)
			{
				string filePath = entry.path().string();

				ifstream fileScenario(filePath);
				auto jsonImpoter = json::parse(fileScenario);
				fileScenario.close();

				auto actorArray = jsonImpoter["Actors"];
				for (auto actor : actorArray)
				{
					string strModelTag = actor["ModelTag"];
					string strModelPath = actor["ModelPath"];

					_matrix	preTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
					wstring wstrModelTag = wstring(strModelTag.begin(), strModelTag.end());
					GAMEINSTANCE->Add_Prototype(
						LEVEL_KALESTAGE, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strModelPath.c_str(), preTransformMatrix)
					);
				}
			}
			else
			{
				// Sound
				string filePath = entry.path().string();
				wstring wFilePath = wstring(filePath.begin(), filePath.end());

				strFileName.erase(strFileName.length() - 4);
				wstring wstrFileName = wstring(strFileName.begin(), strFileName.end());

				GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, wstrFileName, wFilePath);
			}
		}
	}

	/* Chai / Assist */
	CResourceWizard::LoadResources(CChai::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPeppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaron::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKorsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Companion::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Cat::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Peppermint::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWeapon_Korsica::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBasic_Guitar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCNMN_Playable::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	
	CResourceWizard::LoadResources(CUi_PlayerBar::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_BeatMarkerSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_SuccessSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CommandIcon::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_MapInterAction::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_CNMNRazer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_TalkSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_EnemySpecialAttack::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	/* Gimmick */
	CResourceWizard::LoadResources(CInvisibleWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFan_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCommonSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpectraMeshGate::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CZipLine_Rail::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWireMagnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBuiltIn_Magnet::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMovingFloor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSmoke_Cover::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKrsc_Generator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSteelFrame_Platform::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFadeElevator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFloorSwitch::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRhythmJump::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot2::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot3::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGiantRobot_Arm::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpectra_FanMachine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CTalkTrigger::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFree_SignBoard::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPeppermintContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMacaronCrane::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleElevatorGenerator::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CContainerBreakPannel::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotPunchBreakWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotPunchStaticWall::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRobotBeamContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CScroll2DContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRailContainer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattle3rdAfter::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBattle3rdFloor::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CKaleBoss_CineObj::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CRhythm_RobotArm::ObjID, LEVEL_KALESTAGE, pGameInstance, m_pDevice, m_pContext);

	/* Monsters */
	CResourceWizard::LoadResources(CMonster_SwordMachine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwordMachine_Sword::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Blaze::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Saber::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Gunner::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMonster_Baboo::ObjID, m_eNextLevelID, pGameInstance, m_pDevice, m_pContext);
	
	/* UI */
	CResourceWizard::LoadResources(CUi_RankSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StageResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_ScoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_FightString::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_FinalResultSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StunGaugeSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CUi_CNMNRazer::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CPickUpItem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPickMeUp_Can::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CODS_Controller::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLevelUp_Machine::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_StoreSystem::ObjID, LEVEL_STATIC, pGameInstance, m_pDevice, m_pContext);
	pGameInstance->Initialize_Octree(64.f, 5, _float3{ -25.f,90.f,250.f });

	auto pMapManager = CMapManager::Get_Instance();
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_01.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_2DScroll.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_2F.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_3F.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_4F.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_5F.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_Battle2nd.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_Battle3rd.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_CNMN.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_Jump.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_LeftBLDG.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_MagnetRail1.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_NonPlaying.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_RobotBeam2.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_RoofTop.bin");
	pMapManager->Level_MapLoad(LEVEL_KALESTAGE, "../Bin/DataFiles/Map/Stage_Kale_Spectra.bin");
	pMapManager->Clear_Containers();

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

shared_ptr<CLoader> CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	struct MakeSharedEnabler : public CLoader
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLoader(pDevice, pContext) { }
	};

	shared_ptr<CLoader> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		assert(false);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}