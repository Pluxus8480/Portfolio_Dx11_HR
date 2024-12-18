#include "CUi_LogoButtonSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "Load_Include.h"
#include "Level_Loading.h"


wstring CUi_LogoButtonSystem::ObjID = L"CUi_LogoButtonSystem";

CUi_LogoButtonSystem::CUi_LogoButtonSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_LogoButtonSystem::CUi_LogoButtonSystem(const CUi_LogoButtonSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_LogoButtonSystem::~CUi_LogoButtonSystem()
{
}

HRESULT CUi_LogoButtonSystem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_LogoButtonSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_Font();

	return S_OK;
}

void CUi_LogoButtonSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_LogoButtonSystem::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < LEVEL_END; i++)
	{
		if(i==m_iCurIndex)
			m_LevelFontDesc[i].Color = { 1, 0, 0, 1 };
		else
			m_LevelFontDesc[i].Color = { 1, 1, 1, 1 };
	}

	if (m_pGameInstance.lock()->Get_KeyDown(DIK_LEFT))
	{
		if (0 < m_iCurIndex)
		{
			--m_iCurIndex;
		}
		else if (0 == m_iCurIndex)
		{
			m_iCurIndex = LEVEL_END - 1;
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_RIGHT))
	{
		if (m_iCurIndex < LEVEL_END - 1)
		{
			++m_iCurIndex;
		}
		else if (m_iCurIndex == LEVEL_END - 1)
		{
			m_iCurIndex = 0;
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_DOWN))
	{
		if (m_iCurIndex + 5 < LEVEL_END)
			m_iCurIndex += 5;
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_UP))
	{
		if (0 <= m_iCurIndex - 5)
			m_iCurIndex -= 5;
	}
}

void CUi_LogoButtonSystem::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_LogoButtonSystem::Render()
{
	for (size_t i = 0; i < LEVEL_END; i++)
	{
		RenderFont(m_LevelFontDesc[i]);
	}

	return S_OK;
}

void CUi_LogoButtonSystem::Initialize_Font()
{
	m_LevelFontDesc[LEVEL_STATIC].String = L"LEVEL_STATIC";
	m_LevelFontDesc[LEVEL_LOADING].String = L"LEVEL_LOADING";
	m_LevelFontDesc[LEVEL_LOGO].String = L"LEVEL_LOGO";	
	m_LevelFontDesc[LEVEL_TEST].String = L"Level_Test";
	m_LevelFontDesc[LEVEL_EFFECTTOOL].String = L"LEVEL_EFFECTTOOL";
	m_LevelFontDesc[LEVEL_ANIMTOOL].String = L"LEVEL_ANIMTOOL";
	m_LevelFontDesc[LEVEL_MAPTOOL].String = L"LEVEL_MAPTOOL";
	m_LevelFontDesc[LEVEL_UITOOL].String = L"LEVEL_UI";
	m_LevelFontDesc[LEVEL_CINETOOL].String = L"LEVEL_CINETOOL";
	m_LevelFontDesc[LEVEL_LIFT].String = L"LEVEL_LIFT";
	m_LevelFontDesc[LEVEL_KORSICA].String = L"LEVEL_KORSICA";
	m_LevelFontDesc[LEVEL_MODELTEST].String = L"LEVEL_MODELTEST";
	m_LevelFontDesc[LEVEL_KALEBOSS].String = L"LEVEL_KALEBOSS";
	m_LevelFontDesc[LEVEL_MIMOSA].String = L"LEVEL_MIMOSA";
	m_LevelFontDesc[LEVEL_GIMMICTEST].String = L"LEVEL_GIMMICTEST";
	m_LevelFontDesc[LEVEL_KALESTAGE].String = L"LEVEL_KALESTAGE";

	for (_int i = 0; i < LEVEL_END; ++i)
	{
		m_LevelFontDesc[i].DirectPos = { _float(-550 + 200 * (i % 5)), _float(250 - (_int(i / 5) * 125))};
		m_LevelFontDesc[i].Size = 0.4f;
	}
}


shared_ptr<CUi_LogoButtonSystem> CUi_LogoButtonSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_LogoButtonSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_LogoButtonSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_LogoButtonSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_LogoButtonSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_LogoButtonSystem
	{
		MakeSharedEnabler(const CUi_LogoButtonSystem& rhs) : CUi_LogoButtonSystem(rhs) { }
	};

	shared_ptr<CUi_LogoButtonSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
