#include "CLevel_KorsicaTest.h"
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
#include "MapManager.h"
#include "iostream"

#include "GimmickManager.h"
#include "Cine_Manager.h"
#include "CCineSubTitleMgr.h"



CLevel_KorsicaTest::CLevel_KorsicaTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_KorsicaTest::~CLevel_KorsicaTest()
{
	Free();
}

HRESULT CLevel_KorsicaTest::Initialize()
{
	m_iLevelID = LEVEL_KORSICA;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	// Init cine
	CCine_Manager::Get_Instance()->Initialize("../Bin/Resources/DataFiles/Cinematics/Korsica_Boss/");

	//CPlayer_Manager::Get_Instance()->Set_Player(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));
	if (CPlayer_Manager::Get_Instance()->Get_Player())
	{
		CPlayer_Manager::Get_Instance()->Get_Player()->Dead();
	}
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		assert(false);

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		assert(false);

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		assert(false);

	if (FAILED(Ready_Light()))
		assert(false);

	CBeat_Manager::Get_Instance()->Reset();
	CBeat_Manager::Get_Instance()->Set_Beat(0.4166f);
	m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, L"Negotiation", 0.4f);
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::MUSIC, L"Negotiation", true);

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	// Add UI Callback
	CCine_Manager::Get_Instance()->AddStartCallback("CS0620",
		[&]() {
			CUi_Mgr::Get_Instance()->Ui_AllActive(false);
		}
	);
	CCine_Manager::Get_Instance()->AddLastCallback("CS0620",
		[&]() {
			CUi_Mgr::Get_Instance()->Ui_AllActive(true);
		}
	);

	CCineSubTitleMgr::Get_Instance()->Initialize_KorsicaCallBack();
	CCine_Manager::Get_Instance()->StartCine("CS0620");

	return S_OK;
}

void CLevel_KorsicaTest::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	// Play Cine
	CCine_Manager::Get_Instance()->PlayCinematic(fTimeDelta);

	if (m_bLevelChanged)
	{
		GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)m_iNextLevel));
	}
	return;
}

HRESULT CLevel_KorsicaTest::Render()
{
	
	return S_OK;
}

void CLevel_KorsicaTest::Terminate()
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	m_pGameInstance.lock()->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Negotiation");
	CGimmickManager::Get_Instance()->Clear_GimmickObj();
	CMapManager::Get_Instance()->Clear_BossObjects();

	CPlayer_Manager::Get_Instance()->Get_Player()->Set_Dead(false);

	m_pGameInstance.lock()->Push_Object(LEVEL_STATIC, L"Layer_Player", CPlayer_Manager::Get_Instance()->Get_Player());
}

HRESULT CLevel_KorsicaTest::Ready_Layer_Camera(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_KorsicaTest::Ready_Layer_Map(const wstring& strLayerTag)
{
	auto pGameInstance = m_pGameInstance.lock();
	if (FAILED(pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_GridBox"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CMapObject_Static::ObjID, &staticDesc)))
	//	return E_FAIL;

	/*if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_TEST, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;*/
		/*CTransform::TRANSFORM_DESC			TransformDesc{};
		TransformDesc.fSpeedPerSec = 1.f;
		//TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_Map_KillRun_1"), &TransformDesc)))
			return E_FAIL;

			//CImgui_Manager::Get_Instance()->Level_MapLoad(LEVEL_TEST, "../Bin/DataFiles/Map/MapData01.bin");
			*/

	CMapManager::Get_Instance()->Push_MapObjects(LEVEL_KORSICA, TEXT("Layer_Map"));

	return S_OK;
}

HRESULT CLevel_KorsicaTest::Ready_Layer_Monster(const wstring& strLayerTag)
{
	GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Korsica::ObjID, nullptr);
	return S_OK;
}

HRESULT CLevel_KorsicaTest::Ready_Layer_Bgm()
{

	return S_OK;
}

HRESULT CLevel_KorsicaTest::Ready_Layer_Effect(const wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_KorsicaTest::Ready_Light()
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

void CLevel_KorsicaTest::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_KorsicaTest> CLevel_KorsicaTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_KorsicaTest
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_KorsicaTest(pDevice, pContext) { }
	};

	shared_ptr<CLevel_KorsicaTest> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_KorsicaTest"));
		assert(false);
	}

	return pInstance;
}

void CLevel_KorsicaTest::Free()
{

}
