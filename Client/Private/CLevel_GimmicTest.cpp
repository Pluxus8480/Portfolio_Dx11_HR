#include "CLevel_GimmicTest.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Imgui_Manager.h"
#include "SkyBox.h"
#include "CUi_Mgr.h"
#include "PlayerManager.h"
#include "Load_Include.h"
#include "TestEffect.h"
#include "Effect_Manager.h"
#include "GameManager.h"
#include "Beat_Manager.h"
#include "Generator.h"
#include "GimmickManager.h"
#include "MonsterBase.h"

CLevel_GimmicTest::CLevel_GimmicTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_GimmicTest::~CLevel_GimmicTest()
{
	Free();
}

HRESULT CLevel_GimmicTest::Initialize()
{
	m_iLevelID = LEVEL_GIMMICTEST;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);
	IMGUIMANAGER->Set_ToolType(eToolType::TOOL_MAP, false);

	_bool CollisionTest = { true };

	Ready_Layer_PlayerAndCamera(CollisionTest);
	Ready_Layer_Map(L"Layer_Map");
	Ready_Light();
	Ready_Setting();

	Ready_Test();
	Ready_Layer_Assist();
	//CTransform::TRANSFORM_DESC	TransformDesc{};
	//TransformDesc.vPosition = _float4(-0.f, 1.f, 5.f, 1.f);
	//GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_SwordMachine::ObjID, &TransformDesc);
	CMonsterBase::Monster_DESC monsterDesc{};
	monsterDesc.matTransform = Identity;
	monsterDesc.matTransform._42 = 1.f;
	monsterDesc.matTransform._43 = 5.f;
	GAMEINSTANCE->Add_Clone(m_iLevelID, TEXT("Layer_Monster"), CMonster_Blaze::ObjID, &monsterDesc);

	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillChangeSystem::ObjID, m_iLevelID, L"CUi", CUi_SkillChangeSystem::ObjID);

	return S_OK;
}

void CLevel_GimmicTest::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Effect Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	//ShowCursor(false);

	/*if (GAMEINSTANCE->Get_KeyDown(DIK_RSHIFT))
		EFFECT_CALL("Combo_LLLL_1_1", GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix_Ptr(), 1.f);*/
		//EFFECTMANAGER->Call_Union_Effect("Combo_LLLL_1_1", GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix(), 1.f);


	/*if (GAMEINSTANCE->Get_KeyDown(DIK_1))
		Level_Chanege_Sequenece(fTimeDelta);*/

	return;
}

HRESULT CLevel_GimmicTest::Render()
{

	return S_OK;
}

void CLevel_GimmicTest::Ready_Layer_PlayerAndCamera(_bool CollisionTest)
{
	if (CollisionTest)
	{
		shared_ptr<CChai> Player = static_pointer_cast<CChai>(m_pGameInstance.lock()->
			Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Player"), CChai::ObjID));
		Player->Set_Player_Position(XMVectorSet(0.f,1.f,0.f,1.f));
		CPlayer_Manager::Get_Instance()->Set_Player(Player);


		CCamera_Player::CAMERA_FREE_DESC CameraDesc{};
		CameraDesc.fMouseSensor = 0.05f;
		CameraDesc.vEye = _float4(0.f, 0.f, -25.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.fFovy = XMConvertToRadians(70.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 1000.f;
		CameraDesc.fSpeedPerSec = 50.f;
		CameraDesc.strTag = L"Camera_Player_Main";
		CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		shared_ptr<CCamera_Player> Camera = static_pointer_cast<CCamera_Player>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, L"Layer_Camera", L"Prototype_GameObject_Camera_Player", &CameraDesc));
		m_pGameInstance.lock()->Add_Camera(L"Camera_Player_Main", Camera);
		m_pGameInstance.lock()->Set_MainCamera(L"Camera_Player_Main");

		Player->Set_Player_Camera(static_pointer_cast<CCamera_Player>(Camera));
	}
	else
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
		shared_ptr<CCamera_Tool> Camera = static_pointer_cast<CCamera_Tool>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, L"Camera", TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));
		m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Tool_Main"), Camera);
		m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));
	}
}

HRESULT CLevel_GimmicTest::Ready_Layer_Map(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance.lock()->Add_Clone(LEVEL_STATIC, strLayerTag, CSkySphere::ObjID)))
		return E_FAIL;

	CMapObject_Static::STATICOBJ_DESC staticDesc{};
	staticDesc.strModelTag = TEXT("SM_TPAreaGroundA_IZ_03");
	staticDesc.worldMatrix = Identity;
	staticDesc.worldMatrix._41 = 25.f;
	staticDesc.worldMatrix._42 = 0.f;
	staticDesc.worldMatrix._43 = 0.f;
	
	//wstring strSMObj = CMapObject_Static::ObjID + to_wstring(m_iLevelID);
	if (FAILED(m_pGameInstance.lock()->Add_Clone(m_iLevelID, strLayerTag, CMapObject_Static::ObjID, &staticDesc)))
		return E_FAIL;

	CGimmickBase::COMMONOBJ_DESC commonDesc{};
	commonDesc.fRotationPerSec = 0.f;
	commonDesc.fSpeedPerSec = 0.f;
	commonDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	commonDesc.worldMatrix = Identity;
	commonDesc.worldMatrix._41 = 5.f;
	commonDesc.worldMatrix._42 = 0.f;
	
	//m_pGameInstance.lock()->Add_Clone(m_iLevelID, strLayerTag, CLevelUp_Machine::ObjID, &commonDesc);

	//auto pObject = m_pGameInstance.lock()->Add_Clone_Return(m_iLevelID, strLayerTag, CZipLine_Rail::ObjID, &commonDesc);
	//static_pointer_cast<CZipLine_Rail>(pObject)->Load_Children();

	commonDesc.worldMatrix._42 = 1.f;
	m_pGameInstance.lock()->Add_Clone(m_iLevelID, strLayerTag, CSteelFrame_Platform::ObjID, &commonDesc);
	
	//commonDesc.worldMatrix._41 = -28.f;
	//commonDesc.worldMatrix._42 = 5.f;
	//commonDesc.worldMatrix._43 = -6.f;
	//
	//m_pGameInstance.lock()->Add_Clone(m_iLevelID, strLayerTag, CWireMagnet::ObjID, &commonDesc);

	//CMapManager::Get_Instance()->Push_MapObjects(m_iLevelID, strLayerTag);

	return S_OK;
}


HRESULT CLevel_GimmicTest::Ready_Light()
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

void CLevel_GimmicTest::Ready_Test()
{
}

void CLevel_GimmicTest::Ready_Setting()
{
	CBeat_Manager::Get_Instance()->Set_Beat(0.4f);
	CUi_Mgr::Get_Instance()->LevelChange_Create(m_iLevelID);
	// CPlayer_Manager::Get_Instance()->TelePort();

}

HRESULT CLevel_GimmicTest::Ready_Layer_Assist()
{
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(5.7f, 0.4f, 88.f, 1.f);
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Peppermint"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Peppermint"), CPeppermint::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Macaron"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Macaron"), CMacaron::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Korsica"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Korsica"), CKorsica::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_808_Controller(GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_808_Controller"), C808_Controller::ObjID, &TransformDesc));

	return S_OK;
}

void CLevel_GimmicTest::Level_Chanege_Sequenece(_float fTimeDelta)
{
	CUi_Mgr::Get_Instance()->LevelChange_Delete();
}

shared_ptr<CLevel_GimmicTest> CLevel_GimmicTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_GimmicTest
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_GimmicTest(pDevice, pContext) { }
	};

	shared_ptr<CLevel_GimmicTest> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GimmicTest"));
		assert(false);
	}

	return pInstance;
}

void CLevel_GimmicTest::Free()
{

}