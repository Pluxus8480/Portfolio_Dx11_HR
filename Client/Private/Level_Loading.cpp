#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"

#include "Level_Logo.h"
#include "Level_Test.h"
#include "Level_AnimTool.h"
#include "Level_MapTool.h"
#include "Level_Effect.h"
#include "Level_Ui.h"
#include "Level_Lift.h"
#include "CLevel_KorsicaTest.h"
#include "Level_ModelTest.h"
#include "Level_KaleBoss.h"
#include "Level_CineTool.h"
#include "Level_Mimosa.h"
#include "Level_KaleStage.h"
#include "GameManager.h"

#include "Load_Include.h"
#include "CUi_LoadingSystem.h"
#include "CLevel_GimmicTest.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Loading::~CLevel_Loading()
{
	Free();
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_iLevelID = LEVEL_LOADING;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	m_eNextLevelID = eNextLevelID;

	if (FAILED(Ready_Ui(L"Layer_Ui")))
		return E_FAIL;

	/* 자원로딩을 위한 로더객체를 생성해준다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	
	if (nullptr == m_pLoader)
		return E_FAIL;
	m_pGameInstance.lock()->Simulate_Physx(false);
	m_pGameInstance.lock()->Clear_Octree();
	m_pGameInstance.lock()->Stop_Octree();
	/*GAMEMANAGER->VideoSetting();*/
	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			shared_ptr<CLevel> pLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_TEST:
				pLevel = CLevel_Test::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_ANIMTOOL:
				pLevel = CLevel_AnimTool::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_MAPTOOL:
				pLevel = CLevel_MapTool::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_EFFECTTOOL:
				pLevel = CLevel_Effect::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_UITOOL:
				pLevel = CLevel_Ui::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_LIFT:
				pLevel = CLevel_Lift::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_KORSICA:
				pLevel = CLevel_KorsicaTest::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_MODELTEST:
				pLevel = CLevel_ModelTest::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_KALEBOSS:
				pLevel = CLevel_KaleBoss::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_CINETOOL:
				pLevel = CLevel_CineTool::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_MIMOSA:
				pLevel = CLevel_Mimosa::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GIMMICTEST:
				pLevel = CLevel_GimmicTest::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_KALESTAGE:
				pLevel = CLevel_KaleStage::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;
			m_pGameInstance.lock()->Simulate_Physx(true);
			//m_pGameInstance.lock()->Start_Octree();
			if (FAILED(m_pGameInstance.lock()->Change_Level(pLevel)))
				return;
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Show_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Ui(const wstring& strLayerTag)
{
	m_pLoadingCanvas = static_pointer_cast<CUi_Canvas>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_LoadingSystem::ObjID, LEVEL_LOADING, L"CUi", CUi_LoadingSystem::ObjID, nullptr));


	return S_OK;
}

shared_ptr<CLevel_Loading> CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	struct MakeSharedEnabler : public CLevel_Loading
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Loading(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Loading> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	//m_pLoadingCanvas->Dead();
}
