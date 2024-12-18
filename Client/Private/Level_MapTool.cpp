#include "Level_MapTool.h"
#include "Tool_Camera.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SkyBox.h"

#include "Load_Include.h"
#include "Imgui_Manager.h"
#include "Beat_Manager.h"
#include "GameManager.h"

CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_MapTool::~CLevel_MapTool()
{
	Free();
}

HRESULT CLevel_MapTool::Initialize()
{
	m_iLevelID = LEVEL_MAPTOOL;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	CImgui_Manager::Get_Instance()->Set_ToolType(eToolType::TOOL_MAP, false);
	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	GAMEINSTANCE->Add_Clone(LEVEL_STATIC, L"Sky", CSkySphere::ObjID);

	if (FAILED(Ready_Layer_TestObject(TEXT("Layer_Test"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Assist()))
		return E_FAIL;

	CBeat_Manager::Get_Instance()->Set_Beat(0.5f);

	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	wstring strFps = L"[Test Level] FPS : " + to_wstring(iFps);
	SetWindowText(g_hWnd, strFps.c_str());

	DebugMode();
	//ShowCursor(false);

	/*if (GAMEINSTANCE->Get_KeyDown(DIK_1))
		Level_Chanege_Sequenece(fTimeDelta);*/

	return;
}

HRESULT CLevel_MapTool::Render()
{

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Tool::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(60.f, 20.f, -55.f, 1.f);
	CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 800.f;
	CameraDesc.fSpeedPerSec = 30.f;
	CameraDesc.strTag = TEXT("Camera_Tool_Main");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CCamera_Tool> Camera = static_pointer_cast<CCamera_Tool>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));

	m_pGameInstance.lock()->Add_Camera(TEXT("Camera_Tool_Main"), Camera);
	m_pGameInstance.lock()->Set_MainCamera(TEXT("Camera_Tool_Main"));

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_TestObject(const wstring& strLayerTag)
{
	CGimmickBase::COMMONOBJ_DESC commonDesc{};
	commonDesc.worldMatrix = Identity;

	//if (FAILED(GAMEINSTANCE->Add_Clone(LEVEL_STATIC, strLayerTag, CBulbTrigger::ObjID, &commonDesc)))
	//	return E_FAIL;
	//
	//commonDesc.worldMatrix._11 = 15.f;
	//
	//if (FAILED(GAMEINSTANCE->Add_Clone(LEVEL_STATIC, strLayerTag, CMovingWall::ObjID, &commonDesc)))
	//	return E_FAIL;



	return S_OK;
}


HRESULT CLevel_MapTool::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	/* Default */
	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
	//strcpy_s(LightDesc.szName, "Light_Directional_Main");
	//LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//LightDesc.g_fIntensity = 1.f;

	/* Rekka */
	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vDiffuse = _float4(0.2f, 0.2f, 0.2f, 1.f);
	//strcpy_s(LightDesc.szName, "Light_Directional_Main");
	//LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	//LightDesc.g_fIntensity = 0.3f;

	/* Mimosa */
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.4f, 0.4f, 0.4f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.f, 0.f);
	LightDesc.g_fIntensity = 0.8f;

	if (FAILED(m_pGameInstance.lock()->Add_Light(&LightDesc)))
		return E_FAIL;

	return S_OK;

}

HRESULT CLevel_MapTool::Ready_Layer_Assist()
{
	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(5.7f, 0.4f, 88.f, 1.f);
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Peppermint"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Peppermint"), CPeppermint::ObjID, &TransformDesc));
	//CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Macaron"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Macaron"), CMacaron::ObjID, &TransformDesc));
	CPlayer_Manager::Get_Instance()->Set_Assist(TEXT("Korsica"), GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_Korsica"), CKorsica::ObjID, &TransformDesc));
	//CPlayer_Manager::Get_Instance()->Set_808_Controller(GAMEINSTANCE->Add_Clone_Return(m_iLevelID, TEXT("Layer_808_Controller"), C808_Controller::ObjID, &TransformDesc));

	return S_OK;
}

void CLevel_MapTool::DebugMode()
{
}

void CLevel_MapTool::Level_Chanege_Sequenece(_float fTimeDelta)
{
	//GAMEINSTANCE->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIRSTSTAGE));
}

shared_ptr<CLevel_MapTool> CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_MapTool
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_MapTool(pDevice, pContext) { }
	};

	shared_ptr<CLevel_MapTool> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_MapTool"));
		assert(false);
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{

}
