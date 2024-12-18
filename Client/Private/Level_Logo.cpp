#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Load_Include.h"
#include "GameManager.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

CLevel_Logo::~CLevel_Logo()
{
	Free();
}

HRESULT CLevel_Logo::Initialize()
{
	m_iLevelID = LEVEL_LOGO;
	CGame_Manager::Get_Instance()->Set_Current_Level(m_iLevelID);

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	m_pLogoButton = static_pointer_cast<CUi_LogoButtonSystem>(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelID, L"CUi", CUi_LogoButtonSystem::ObjID));

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	{
		LEVEL ChangeLevel = m_pLogoButton->Get_LevelIndex();
		if (ChangeLevel != LEVEL_STATIC && ChangeLevel != LEVEL_LOADING
			&& ChangeLevel != LEVEL_LOGO)
		{
			if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, ChangeLevel))))
				return;
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_DELETE))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ANIMTOOL))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_F1))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_EFFECTTOOL))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_BACKSPACE))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MAPTOOL))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_RETURN))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_UITOOL))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_F2))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LIFT))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_F3))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KORSICA))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_F4))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MODELTEST))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_ADD))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_KALEBOSS))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_F8))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CINETOOL))))
			return;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_MINUS))
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MIMOSA))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("Load"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring& strLayerTag)
{

	return S_OK;
}


shared_ptr<CLevel_Logo> CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CLevel_Logo
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel_Logo(pDevice, pContext) { }
	};

	shared_ptr<CLevel_Logo> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		assert(false);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{

}
