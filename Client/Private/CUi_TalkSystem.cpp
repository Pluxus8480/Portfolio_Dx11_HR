#include "CUi_TalkSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"
#include "iostream"
#include <fstream>
#include "CUI_Move.h"
#include "CUI_Scale.h"
#include "CUI_Frame.h"
#include "CUI_CallFunction.h"
#include "CUI_Sequence.h"
#include "CUi_BeatMarkerSystem.h"
#include "PlayerManager.h"


wstring CUi_TalkSystem::ObjID = L"CUi_TalkSystem";

CUi_TalkSystem::CUi_TalkSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Canvas(pDevice, pContext)
{
}

CUi_TalkSystem::CUi_TalkSystem(const CUi_TalkSystem& rhs)
	:CUi_Canvas(rhs)
{
}

CUi_TalkSystem::~CUi_TalkSystem()
{
}

HRESULT CUi_TalkSystem::Initialize_Prototype()
{
#pragma region Window

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkBlackBoard", L"../Bin/Resources/Textures/Ui/Talk/Window/BlackBoard.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindowFont", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_02.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindowPoint", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_00.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindowLeft", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_01.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkWindowRight", L"../Bin/Resources/Textures/Ui/Talk/Window/T_Talk_window_03.png")
	);

#pragma endregion Window

#pragma region Chai

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkChaiBase", L"../Bin/Resources/Textures/Ui/Talk/Chai/T_talk_chai_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkChaiNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/Chai/Normal/T_talk_chai_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkChaiAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/Chai/Anger/T_talk_chai_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkChaiConfidentEyes", L"../Bin/Resources/Textures/Ui/Talk/Chai/Confident/T_talk_chai_confident_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkChaiHappyEyes", L"../Bin/Resources/Textures/Ui/Talk/Chai/Happy/T_talk_chai_happy_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkChaiSurpriseEyes", L"../Bin/Resources/Textures/Ui/Talk/Chai/Surprise/T_talk_chai_surprise_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkChaiWorryEyes", L"../Bin/Resources/Textures/Ui/Talk/Chai/Worry/T_talk_chai_worry_eyes_%d.png", 3)
	);

#pragma endregion Chai

#pragma region Speaker

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkSpeaker", L"../Bin/Resources/Textures/Ui/Talk/Speaker/T_talk_speaker_%d.png", 3)
	);

#pragma endregion Speaker

#pragma region Robot

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkRobotA", L"../Bin/Resources/Textures/Ui/Talk/Robot/RobotA/T_talk_robotA_normal.png")
	);

#pragma endregion Robot

#pragma region Hintbot

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkHintbot", L"../Bin/Resources/Textures/Ui/Talk/Hintbot/T_talk_hintbot_normal_%d.png", 3)
	);

#pragma endregion Hintbot

#pragma region CNMN

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkCNMN", L"../Bin/Resources/Textures/Ui/Talk/CNMN/T_talk_CNMN_pleasure_%d.png", 3)
	);

#pragma endregion CNMN

#pragma region Pepper

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkPepperBase", L"../Bin/Resources/Textures/Ui/Talk/Pepper/T_talk_pepper_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkPepperNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/Pepper/Normal/T_talk_pepper_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkPepperAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/Pepper/Anger/T_talk_pepper_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkPepperConfidentEyes", L"../Bin/Resources/Textures/Ui/Talk/Pepper/Confident/T_talk_pepper_confident_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkPepperHappyEyes", L"../Bin/Resources/Textures/Ui/Talk/Pepper/Happy/T_talk_pepper_happy_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkPepperSurpriseEyes", L"../Bin/Resources/Textures/Ui/Talk/Pepper/Surprise/T_talk_pepper_surprise_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkPepperWorryEyes", L"../Bin/Resources/Textures/Ui/Talk/Pepper/Worry/T_talk_pepper_worry_eyes_%d.png", 3)
	);

#pragma endregion Pepper

#pragma region Macaron

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMacaronBase", L"../Bin/Resources/Textures/Ui/Talk/Macaron/T_talk_macaron_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMacaronNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/Macaron/Normal/T_talk_macaron_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMacaronAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/Macaron/Anger/T_talk_macaron_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMacaronConfidentEyes", L"../Bin/Resources/Textures/Ui/Talk/Macaron/Confident/T_talk_macaron_confident_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMacaronHappyEyes", L"../Bin/Resources/Textures/Ui/Talk/Macaron/Happy/T_talk_macaron_happy_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMacaronSurpriseEyes", L"../Bin/Resources/Textures/Ui/Talk/Macaron/Surprise/T_talk_macaron_surprise_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMacaronWorryEyes", L"../Bin/Resources/Textures/Ui/Talk/Macaron/Worry/T_talk_macaron_worry_eyes_%d.png", 3)
	);

#pragma endregion Macaron

#pragma region Korsica

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKorsicaBase", L"../Bin/Resources/Textures/Ui/Talk/Korsica/T_talk_korsica_team_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKorsicaNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/Korsica/Normal/T_talk_korsica_team_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKorsicaAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/Korsica/Anger/T_talk_korsica_team_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKorsicaHappyEyes", L"../Bin/Resources/Textures/Ui/Talk/Korsica/Happy/T_talk_korsica_team_happy_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKorsicaSurpriseEyes", L"../Bin/Resources/Textures/Ui/Talk/Korsica/Surprise/T_talk_korsica_team_surprise_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKorsicaWorryEyes", L"../Bin/Resources/Textures/Ui/Talk/Korsica/Worry/T_talk_korsica_team_worry_eyes_%d.png", 3)
	);

#pragma endregion Korsica

#pragma region Rekka

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkRekkaBase", L"../Bin/Resources/Textures/Ui/Talk/Rekka/T_talk_rekka_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkRekkaNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/Rekka/Normal/T_talk_rekka_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkRekkaAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/Rekka/Anger/T_talk_rekka_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkRekkaHappyEyes", L"../Bin/Resources/Textures/Ui/Talk/Rekka/Happy/T_talk_rekka_happy_eyes_%d.png", 3)
	);

#pragma endregion Rekka

#pragma region MonKorsica

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMonKorsicaBase", L"../Bin/Resources/Textures/Ui/Talk/MonKorsica/T_talk_Monkorsica_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMonKorsicaNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/MonKorsica/Normal/T_talk_Monkorsica_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMonKorsicaAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/MonKorsica/Anger/T_talk_Monkorsica_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMonKorsicaConfidentEyes", L"../Bin/Resources/Textures/Ui/Talk/MonKorsica/Confident/T_talk_Monkorsica_confident_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMonKorsicaSurpriseEyes", L"../Bin/Resources/Textures/Ui/Talk/MonKorsica/Surprise/T_talk_Monkorsica_surprise_eyes_%d.png", 3)
	);

#pragma endregion MonKorsica

#pragma region Mimosa

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMimosaBase", L"../Bin/Resources/Textures/Ui/Talk/Mimosa/T_talk_mimosa_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMimosaNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/Mimosa/Normal/T_talk_mimosa_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMimosaAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/Mimosa/Anger/T_talk_mimosa_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkMimosaConfidentEyes", L"../Bin/Resources/Textures/Ui/Talk/Mimosa/Confident/T_talk_mimosa_confident_eyes_%d.png", 3)
	);

#pragma endregion Mimosa

#pragma region Kale

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKaleBase", L"../Bin/Resources/Textures/Ui/Talk/Kale/T_talk_kale_00_base.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKaleNormalEyes", L"../Bin/Resources/Textures/Ui/Talk/Kale/Normal/T_talk_kale_normal_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKaleAngerEyes", L"../Bin/Resources/Textures/Ui/Talk/Kale/Anger/T_talk_kale_anger_eyes_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkKaleHappyEyes", L"../Bin/Resources/Textures/Ui/Talk/Kale/Happy/T_talk_kale_happy_eyes_%d.png", 3)
	);

#pragma endregion Kale

#pragma region CNMNSECOND

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkCNMNSECOND", L"../Bin/Resources/Textures/Ui/Talk/CNMN_SECOND/T_talk_CNMN_sarcasm_%d.png", 3)
	);

#pragma endregion CNMNSECOND

	return S_OK;
}

HRESULT CUi_TalkSystem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	if(Arg != nullptr && !Arg->wsFileName.empty())
	{
		LoadTalkFile(Arg->wsFileName);
	}
	m_endCallback = Arg->endCallback;

	Create_TalkWindow();
	Create_TalkChai();
	Create_TalkSpeaker();
	//Create_TalkRobot();
	//Create_TalkHintbot();
	Create_TalkCNMN();
	Create_TalkPepper();
	Create_TalkMacaron();
	Create_TalkKorsica();
	Create_TalkRekka();
	Create_TalkMonKorsica();
	Create_TalkMimosa();
	Create_TalkKale();
	Create_TalkCNMNSECOND();

	InitializeTalk();
	ShowTalk();

	return S_OK;
}

void CUi_TalkSystem::Tick(_float fTimeDelta)
{
}

void CUi_TalkSystem::Dying()
{
	m_pBlackBoard->Dead();
	m_pWindowFont->Dead();
	
	for (size_t i = 0; i < WINDOW_END; ++i)
	{
		m_pWindowSprite[i]->Dead();
	}
	
	for (size_t i = 0; i < CHAR_END; ++i)
	{
		if (m_pCharacter[i])
		m_pCharacter[i]->Dead();
	}

	for (size_t i = 0; i < CHAR_END; ++i)
	{
		for (auto& facePair : m_pCharacterFace[i]) 
		{
			for (auto& sprite : facePair.second) 
			{
				if (sprite) 
				{
					sprite->Dead(); 
				}
			}
		}
	}
}

void CUi_TalkSystem::Set_Active(_bool Active)
{
	m_pBlackBoard->Set_Active(Active);
	m_pWindowFont->Set_Active(Active);
	
	for (size_t i = 0; i < WINDOW_END; ++i)
	{
		m_pWindowSprite[i]->Set_Active(Active);
	}
	
	for (size_t i = 0; i < CHAR_END; ++i)
	{
		if(m_pCharacter[i])
			m_pCharacter[i]->Set_Active(Active);
	}

	for (size_t i = 0; i < CHAR_END; ++i)
	{
		for (auto& facePair : m_pCharacterFace[i])
		{
			for (auto& sprite : facePair.second)
			{
				if (sprite)
				{
					sprite->Set_Active(Active);
				}
			}
		}
	}
}

void CUi_TalkSystem::Create_TalkWindow()
{
	FontDescInfo FontDesc;
	FontDesc.DirectPos = { -273.f, -224.f };
	FontDesc.Size = 0.3f;
	FontDesc.Color = { 1.f, 1.f, 1.f, 1.f };
	CUi_Font::Desc BoardDesc{};
	BoardDesc.fX = -210.f;
	BoardDesc.fY = -230.f;
	BoardDesc.fZ = 0.69f;
	BoardDesc.fSizeX = 155.f;
	BoardDesc.fSizeY = 100.f;
	BoardDesc.fSizeZ = 1.f;
	BoardDesc.fSpeed = 0.f;
	BoardDesc.eRenderType = RenderType::Priority;
	BoardDesc.iPriority = 6;
	BoardDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	BoardDesc.TextureFileName = L"Prototype_Component_Texture_TalkBlackBoard";
	BoardDesc.TextureLevel = LEVEL_STATIC;
	BoardDesc.iTextureNum = 0;
	BoardDesc.ShaderPass = ShaderPass::Default;
	BoardDesc.Font = FontDesc;
	m_pBlackBoard = static_pointer_cast<CUi_Font>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Font::ObjID, m_iLevelIndex, L"CUi", CUi_Font::ObjID, &BoardDesc));

	FontDesc.DirectPos = { -285.f, -273.f };
	FontDesc.Size = 0.5f;
	FontDesc.Color = { 0.f, 0.f, 0.f, 1.f };
	CUi_Font::Desc WindowFontDesc{};
	WindowFontDesc.fX = 0.f;
	WindowFontDesc.fY = -282.f;
	WindowFontDesc.fZ = 0.69f;
	WindowFontDesc.fSizeX = 1000.f;
	WindowFontDesc.fSizeY = 140.f;
	WindowFontDesc.fSizeZ = 1.f;
	WindowFontDesc.fSpeed = 0.f;
	WindowFontDesc.eRenderType = RenderType::Priority;
	WindowFontDesc.iPriority = 6;
	WindowFontDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	WindowFontDesc.TextureFileName = L"Prototype_Component_Texture_TalkWindowFont";
	WindowFontDesc.TextureLevel = LEVEL_STATIC;
	WindowFontDesc.iTextureNum = 0;
	WindowFontDesc.ShaderPass = ShaderPass::Default;
	WindowFontDesc.Font = FontDesc;
	m_pWindowFont = static_pointer_cast<CUi_Font>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Font::ObjID, m_iLevelIndex, L"CUi", CUi_Font::ObjID, &WindowFontDesc));

	CUi_Sprite::Desc WinPointDesc{};
	WinPointDesc.fX = -322.f;
	WinPointDesc.fY = -282.f;
	WinPointDesc.fZ = 0.67f;
	WinPointDesc.fSizeX = 140.f;
	WinPointDesc.fSizeY = 140.f;
	WinPointDesc.fSizeZ = 1.f;
	WinPointDesc.fSpeed = 0.f;
	WinPointDesc.eRenderType = RenderType::Priority;
	WinPointDesc.iPriority = 8;
	WinPointDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	WinPointDesc.TextureFileName = L"Prototype_Component_Texture_TalkWindowPoint";
	WinPointDesc.TextureLevel = LEVEL_STATIC;
	WinPointDesc.iTextureNum = 0;
	WinPointDesc.ShaderPass = ShaderPass::Default;
	m_pWindowSprite[WINDOW_POINT] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &WinPointDesc));

	CUi_Sprite::Desc WinLeftDesc{};
	WinLeftDesc.fX = -310.f;
	WinLeftDesc.fY = -282.f;
	WinLeftDesc.fZ = 0.68f;
	WinLeftDesc.fSizeX = 140.f;
	WinLeftDesc.fSizeY = 140.f;
	WinLeftDesc.fSizeZ = 1.f;
	WinLeftDesc.fSpeed = 0.f;
	WinLeftDesc.eRenderType = RenderType::Priority;
	WinLeftDesc.iPriority = 7;
	WinLeftDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	WinLeftDesc.TextureFileName = L"Prototype_Component_Texture_TalkWindowLeft";
	WinLeftDesc.TextureLevel = LEVEL_STATIC;
	WinLeftDesc.iTextureNum = 0;
	WinLeftDesc.ShaderPass = ShaderPass::Default;
	m_pWindowSprite[WINDOW_LEFT] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &WinLeftDesc));


	CUi_Sprite::Desc WinRightDesc{};
	WinRightDesc.fX = 310.f;
	WinRightDesc.fY = -282.f;
	WinRightDesc.fZ = 0.68f;
	WinRightDesc.fSizeX = 140.f;
	WinRightDesc.fSizeY = 140.f;
	WinRightDesc.fSizeZ = 1.f;
	WinRightDesc.fSpeed = 0.f;
	WinRightDesc.eRenderType = RenderType::Priority;
	WinRightDesc.iPriority = 7;
	WinRightDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	WinRightDesc.TextureFileName = L"Prototype_Component_Texture_TalkWindowRight";
	WinRightDesc.TextureLevel = LEVEL_STATIC;
	WinRightDesc.iTextureNum = 0;
	WinRightDesc.ShaderPass = ShaderPass::Default;
	m_pWindowSprite[WINDOW_RIGHT] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &WinRightDesc));
}

void CUi_TalkSystem::Create_TalkChai()
{
	CUi_Sprite::Desc ChaiBaseDesc{};
	ChaiBaseDesc.fX = -500.f;
	ChaiBaseDesc.fY = -250.f;
	ChaiBaseDesc.fZ = 0.7f;
	ChaiBaseDesc.fSizeX = 550.f;
	ChaiBaseDesc.fSizeY = 550.f;
	ChaiBaseDesc.fSizeZ = 1.f;
	ChaiBaseDesc.fSpeed = 0.f;
	ChaiBaseDesc.eRenderType = RenderType::Normal;
	ChaiBaseDesc.iPriority = 7;
	ChaiBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ChaiBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkChaiBase";
	ChaiBaseDesc.TextureLevel = LEVEL_STATIC;
	ChaiBaseDesc.iTextureNum = 0;
	ChaiBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_CHAI] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ChaiBaseDesc));

	CUi_Sprite::Desc ChaiNormalEyesDesc{};
	ChaiNormalEyesDesc.fX = -500.f;
	ChaiNormalEyesDesc.fY = -250.f;
	ChaiNormalEyesDesc.fZ = 0.69f;
	ChaiNormalEyesDesc.fSizeX = 550.f;
	ChaiNormalEyesDesc.fSizeY = 550.f;
	ChaiNormalEyesDesc.fSizeZ = 1.f;
	ChaiNormalEyesDesc.fSpeed = 0.f;
	ChaiNormalEyesDesc.iMaxFrame = 2;
	ChaiNormalEyesDesc.eRenderType = RenderType::Normal;
	ChaiNormalEyesDesc.iPriority = 8;
	ChaiNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ChaiNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkChaiNormalEyes";
	ChaiNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	ChaiNormalEyesDesc.iTextureNum = 0;
	ChaiNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_CHAI, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ChaiNormalEyesDesc)));

	CUi_Sprite::Desc ChaiAngerEyesDesc = ChaiNormalEyesDesc;
	ChaiAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkChaiAngerEyes";
	AddCharacterFace(CHAR_CHAI, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ChaiAngerEyesDesc)));

	CUi_Sprite::Desc ChaiConfidentEyesDesc = ChaiNormalEyesDesc;
	ChaiConfidentEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkChaiConfidentEyes";
	AddCharacterFace(CHAR_CHAI, FACE_CONFIDENT, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ChaiConfidentEyesDesc)));

	CUi_Sprite::Desc ChaiHappyEyesDesc = ChaiNormalEyesDesc;
	ChaiHappyEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkChaiHappyEyes";
	AddCharacterFace(CHAR_CHAI, FACE_HAPPY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ChaiHappyEyesDesc)));

	CUi_Sprite::Desc ChaiSurpriseEyesDesc = ChaiNormalEyesDesc;
	ChaiSurpriseEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkChaiSurpriseEyes";
	AddCharacterFace(CHAR_CHAI, FACE_SURPRISE, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ChaiSurpriseEyesDesc)));

	CUi_Sprite::Desc ChaiWorryEyesDesc = ChaiNormalEyesDesc;
	ChaiWorryEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkChaiWorryEyes";
	AddCharacterFace(CHAR_CHAI, FACE_WORRY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ChaiWorryEyesDesc)));
}

void CUi_TalkSystem::Create_TalkSpeaker()
{
	CUi_Sprite::Desc SpeakerDesc{};
	SpeakerDesc.fX = -500.f;
	SpeakerDesc.fY = -250.f;
	SpeakerDesc.fZ = 0.7f;
	SpeakerDesc.fSizeX = 550.f;
	SpeakerDesc.fSizeY = 550.f;
	SpeakerDesc.fSizeZ = 1.f;
	SpeakerDesc.fSpeed = 0.f;
	SpeakerDesc.iMaxFrame = 2;
	SpeakerDesc.eRenderType = RenderType::Normal;
	SpeakerDesc.iPriority = 7;
	SpeakerDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	SpeakerDesc.TextureFileName = L"Prototype_Component_Texture_TalkSpeaker";
	SpeakerDesc.TextureLevel = LEVEL_STATIC;
	SpeakerDesc.iTextureNum = 0;
	SpeakerDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_SPEAKER] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &SpeakerDesc));
}

void CUi_TalkSystem::Create_TalkRobot()
{
	CUi_Sprite::Desc RobotADesc{};
	RobotADesc.fX = -500.f;
	RobotADesc.fY = -250.f;
	RobotADesc.fZ = 0.7f;
	RobotADesc.eRenderType = RenderType::Normal;
	RobotADesc.iPriority = 7;
	RobotADesc.fSizeX = 550.f;
	RobotADesc.fSizeY = 550.f;
	RobotADesc.fSizeZ = 1.f;
	RobotADesc.fSpeed = 0.f;
	RobotADesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	RobotADesc.TextureFileName = L"Prototype_Component_Texture_TalkRobotA";
	RobotADesc.TextureLevel = LEVEL_STATIC;
	RobotADesc.iTextureNum = 0;
	RobotADesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_ROBOT] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &RobotADesc));

}

void CUi_TalkSystem::Create_TalkHintbot()
{
	CUi_Sprite::Desc HinbotDesc{};
	HinbotDesc.fX = -500.f;
	HinbotDesc.fY = -250.f;
	HinbotDesc.fZ = 0.7f;
	HinbotDesc.fSizeX = 550.f;
	HinbotDesc.fSizeY = 550.f;
	HinbotDesc.fSizeZ = 1.f;
	HinbotDesc.fSpeed = 0.f;
	HinbotDesc.iMaxFrame = 2;
	HinbotDesc.eRenderType = RenderType::Normal;
	HinbotDesc.iPriority = 7;
	HinbotDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	HinbotDesc.TextureFileName = L"Prototype_Component_Texture_TalkHintbot";
	HinbotDesc.TextureLevel = LEVEL_STATIC;
	HinbotDesc.iTextureNum = 0;
	HinbotDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_HINTBOT] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &HinbotDesc));
}

void CUi_TalkSystem::Create_TalkCNMN()
{
	CUi_Sprite::Desc CNMNDesc{};
	CNMNDesc.fX = -500.f;
	CNMNDesc.fY = -250.f;
	CNMNDesc.fZ = 0.7f;
	CNMNDesc.iMaxFrame = 2;
	CNMNDesc.eRenderType = RenderType::Normal;
	CNMNDesc.iPriority = 7;
	CNMNDesc.fSizeX = 550.f;
	CNMNDesc.fSizeY = 550.f;
	CNMNDesc.fSizeZ = 1.f;
	CNMNDesc.fSpeed = 0.f;
	CNMNDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	CNMNDesc.TextureFileName = L"Prototype_Component_Texture_TalkCNMN";
	CNMNDesc.TextureLevel = LEVEL_STATIC;
	CNMNDesc.iTextureNum = 0;
	CNMNDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_CNMN] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &CNMNDesc));
}

void CUi_TalkSystem::Create_TalkPepper()
{
	CUi_Sprite::Desc PepperBaseDesc{};
	PepperBaseDesc.fX = -500.f;
	PepperBaseDesc.fY = -250.f;
	PepperBaseDesc.fZ = 0.7f;
	PepperBaseDesc.fSizeX = 550.f;
	PepperBaseDesc.fSizeY = 550.f;
	PepperBaseDesc.fSizeZ = 1.f;
	PepperBaseDesc.fSpeed = 0.f;
	PepperBaseDesc.eRenderType = RenderType::Normal;
	PepperBaseDesc.iPriority = 7;
	PepperBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PepperBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkPepperBase";
	PepperBaseDesc.TextureLevel = LEVEL_STATIC;
	PepperBaseDesc.iTextureNum = 0;
	PepperBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_PEPPER] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperBaseDesc));

	CUi_Sprite::Desc PepperNormalEyesDesc{};
	PepperNormalEyesDesc.fX = -500.f;
	PepperNormalEyesDesc.fY = -250.f;
	PepperNormalEyesDesc.fZ = 0.69f;
	PepperNormalEyesDesc.fSizeX = 550.f;
	PepperNormalEyesDesc.fSizeY = 550.f;
	PepperNormalEyesDesc.fSizeZ = 1.f;
	PepperNormalEyesDesc.fSpeed = 0.f;
	PepperNormalEyesDesc.iMaxFrame = 2;
	PepperNormalEyesDesc.eRenderType = RenderType::Normal;
	PepperNormalEyesDesc.iPriority = 8;
	PepperNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PepperNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkPepperNormalEyes";
	PepperNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	PepperNormalEyesDesc.iTextureNum = 0;
	PepperNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_PEPPER, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperNormalEyesDesc)));

	CUi_Sprite::Desc PepperAngerEyesDesc = PepperNormalEyesDesc;
	PepperAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkPepperAngerEyes";
	AddCharacterFace(CHAR_PEPPER, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperAngerEyesDesc)));

	CUi_Sprite::Desc PepperConfidentEyesDesc = PepperNormalEyesDesc;
	PepperConfidentEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkPepperConfidentEyes";
	AddCharacterFace(CHAR_PEPPER, FACE_CONFIDENT, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperConfidentEyesDesc)));

	CUi_Sprite::Desc PepperHappyEyesDesc = PepperNormalEyesDesc;
	PepperHappyEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkPepperHappyEyes";
	AddCharacterFace(CHAR_PEPPER, FACE_HAPPY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperHappyEyesDesc)));

	CUi_Sprite::Desc PepperSurpriseEyesDesc = PepperNormalEyesDesc;
	PepperSurpriseEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkPepperSurpriseEyes";
	AddCharacterFace(CHAR_PEPPER, FACE_SURPRISE, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperSurpriseEyesDesc)));

	CUi_Sprite::Desc PepperWorryEyesDesc = PepperNormalEyesDesc;
	PepperWorryEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkPepperWorryEyes";
	AddCharacterFace(CHAR_PEPPER, FACE_WORRY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperWorryEyesDesc)));
}

void CUi_TalkSystem::Create_TalkMacaron()
{
	CUi_Sprite::Desc MacaronBaseDesc{};
	MacaronBaseDesc.fX = -500.f;
	MacaronBaseDesc.fY = -250.f;
	MacaronBaseDesc.fZ = 0.7f;
	MacaronBaseDesc.fSizeX = 550.f;
	MacaronBaseDesc.fSizeY = 550.f;
	MacaronBaseDesc.fSizeZ = 1.f;
	MacaronBaseDesc.fSpeed = 0.f;
	MacaronBaseDesc.eRenderType = RenderType::Normal;
	MacaronBaseDesc.iPriority = 7;
	MacaronBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MacaronBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkMacaronBase";
	MacaronBaseDesc.TextureLevel = LEVEL_STATIC;
	MacaronBaseDesc.iTextureNum = 0;
	MacaronBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_MACARON] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronBaseDesc));

	CUi_Sprite::Desc MacaronNormalEyesDesc{};
	MacaronNormalEyesDesc.fX = -500.f;
	MacaronNormalEyesDesc.fY = -250.f;
	MacaronNormalEyesDesc.fZ = 0.69f;
	MacaronNormalEyesDesc.fSizeX = 550.f;
	MacaronNormalEyesDesc.fSizeY = 550.f;
	MacaronNormalEyesDesc.fSizeZ = 1.f;
	MacaronNormalEyesDesc.fSpeed = 0.f;
	MacaronNormalEyesDesc.iMaxFrame = 2;
	MacaronNormalEyesDesc.eRenderType = RenderType::Normal;
	MacaronNormalEyesDesc.iPriority = 8;
	MacaronNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MacaronNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMacaronNormalEyes";
	MacaronNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	MacaronNormalEyesDesc.iTextureNum = 0;
	MacaronNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_MACARON, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronNormalEyesDesc)));

	CUi_Sprite::Desc MacaronAngerEyesDesc = MacaronNormalEyesDesc;
	MacaronAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMacaronAngerEyes";
	AddCharacterFace(CHAR_MACARON, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronAngerEyesDesc)));

	CUi_Sprite::Desc MacaronConfidentEyesDesc = MacaronNormalEyesDesc;
	MacaronConfidentEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMacaronConfidentEyes";
	AddCharacterFace(CHAR_MACARON, FACE_CONFIDENT, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronConfidentEyesDesc)));

	CUi_Sprite::Desc MacaronHappyEyesDesc = MacaronNormalEyesDesc;
	MacaronHappyEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMacaronHappyEyes";
	AddCharacterFace(CHAR_MACARON, FACE_HAPPY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronHappyEyesDesc)));

	CUi_Sprite::Desc MacaronSurpriseEyesDesc = MacaronNormalEyesDesc;
	MacaronSurpriseEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMacaronSurpriseEyes";
	AddCharacterFace(CHAR_MACARON, FACE_SURPRISE, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronSurpriseEyesDesc)));

	CUi_Sprite::Desc MacaronWorryEyesDesc = MacaronNormalEyesDesc;
	MacaronWorryEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMacaronWorryEyes";
	AddCharacterFace(CHAR_MACARON, FACE_WORRY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronWorryEyesDesc)));
}

void CUi_TalkSystem::Create_TalkKorsica()
{
	CUi_Sprite::Desc KorsicaBaseDesc{};
	KorsicaBaseDesc.fX = -500.f;
	KorsicaBaseDesc.fY = -250.f;
	KorsicaBaseDesc.fZ = 0.7f;
	KorsicaBaseDesc.fSizeX = 550.f;
	KorsicaBaseDesc.fSizeY = 550.f;
	KorsicaBaseDesc.fSizeZ = 1.f;
	KorsicaBaseDesc.fSpeed = 0.f;
	KorsicaBaseDesc.eRenderType = RenderType::Normal;
	KorsicaBaseDesc.iPriority = 7;
	KorsicaBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	KorsicaBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkKorsicaBase";
	KorsicaBaseDesc.TextureLevel = LEVEL_STATIC;
	KorsicaBaseDesc.iTextureNum = 0;
	KorsicaBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_KORSICA] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KorsicaBaseDesc));

	CUi_Sprite::Desc KorsicaNormalEyesDesc{};
	KorsicaNormalEyesDesc.fX = -500.f;
	KorsicaNormalEyesDesc.fY = -250.f;
	KorsicaNormalEyesDesc.fZ = 0.69f;
	KorsicaNormalEyesDesc.fSizeX = 550.f;
	KorsicaNormalEyesDesc.fSizeY = 550.f;
	KorsicaNormalEyesDesc.fSizeZ = 1.f;
	KorsicaNormalEyesDesc.fSpeed = 0.f;
	KorsicaNormalEyesDesc.iMaxFrame = 2;
	KorsicaNormalEyesDesc.eRenderType = RenderType::Normal;
	KorsicaNormalEyesDesc.iPriority = 8;
	KorsicaNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	KorsicaNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKorsicaNormalEyes";
	KorsicaNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	KorsicaNormalEyesDesc.iTextureNum = 0;
	KorsicaNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_KORSICA, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KorsicaNormalEyesDesc)));

	CUi_Sprite::Desc KorsicaAngerEyesDesc = KorsicaNormalEyesDesc;
	KorsicaAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKorsicaAngerEyes";
	AddCharacterFace(CHAR_KORSICA, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KorsicaAngerEyesDesc)));

	CUi_Sprite::Desc KorsicaHappyEyesDesc = KorsicaNormalEyesDesc;
	KorsicaHappyEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKorsicaHappyEyes";
	AddCharacterFace(CHAR_KORSICA, FACE_HAPPY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KorsicaHappyEyesDesc)));

	CUi_Sprite::Desc KorsicaSurpriseEyesDesc = KorsicaNormalEyesDesc;
	KorsicaSurpriseEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKorsicaSurpriseEyes";
	AddCharacterFace(CHAR_KORSICA, FACE_SURPRISE, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KorsicaSurpriseEyesDesc)));

	CUi_Sprite::Desc KorsicsWorryEyesDesc = KorsicaNormalEyesDesc;
	KorsicsWorryEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKorsicaWorryEyes";
	AddCharacterFace(CHAR_KORSICA, FACE_WORRY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KorsicsWorryEyesDesc)));
}

void CUi_TalkSystem::Create_TalkRekka()
{
	CUi_Sprite::Desc RekkaBaseDesc{};
	RekkaBaseDesc.fX = -500.f;
	RekkaBaseDesc.fY = -250.f;
	RekkaBaseDesc.fZ = 0.7f;
	RekkaBaseDesc.fSizeX = 550.f;
	RekkaBaseDesc.fSizeY = 550.f;
	RekkaBaseDesc.fSizeZ = 1.f;
	RekkaBaseDesc.fSpeed = 0.f;
	RekkaBaseDesc.eRenderType = RenderType::Normal;
	RekkaBaseDesc.iPriority = 7;
	RekkaBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	RekkaBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkRekkaBase";
	RekkaBaseDesc.TextureLevel = LEVEL_STATIC;
	RekkaBaseDesc.iTextureNum = 0;
	RekkaBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_REKKA] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &RekkaBaseDesc));

	CUi_Sprite::Desc RekkaNormalEyesDesc{};
	RekkaNormalEyesDesc.fX = -500.f;
	RekkaNormalEyesDesc.fY = -250.f;
	RekkaNormalEyesDesc.fZ = 0.69f;
	RekkaNormalEyesDesc.fSizeX = 550.f;
	RekkaNormalEyesDesc.fSizeY = 550.f;
	RekkaNormalEyesDesc.fSizeZ = 1.f;
	RekkaNormalEyesDesc.fSpeed = 0.f;
	RekkaNormalEyesDesc.iMaxFrame = 2;
	RekkaNormalEyesDesc.eRenderType = RenderType::Normal;
	RekkaNormalEyesDesc.iPriority = 8;
	RekkaNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	RekkaNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkRekkaNormalEyes";
	RekkaNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	RekkaNormalEyesDesc.iTextureNum = 0;
	RekkaNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_REKKA, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &RekkaNormalEyesDesc)));

	CUi_Sprite::Desc RekkaAngerEyesDesc = RekkaNormalEyesDesc;
	RekkaAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkRekkaAngerEyes";
	AddCharacterFace(CHAR_REKKA, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &RekkaAngerEyesDesc)));

	CUi_Sprite::Desc RekkaHappyEyesDesc = RekkaNormalEyesDesc;
	RekkaHappyEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkRekkaHappyEyes";
	AddCharacterFace(CHAR_REKKA, FACE_HAPPY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &RekkaHappyEyesDesc)));
}

void CUi_TalkSystem::Create_TalkMonKorsica()
{
	CUi_Sprite::Desc MonKorsicaBaseDesc{};
	MonKorsicaBaseDesc.fX = -500.f;
	MonKorsicaBaseDesc.fY = -250.f;
	MonKorsicaBaseDesc.fZ = 0.7f;
	MonKorsicaBaseDesc.fSizeX = 550.f;
	MonKorsicaBaseDesc.fSizeY = 550.f;
	MonKorsicaBaseDesc.fSizeZ = 1.f;
	MonKorsicaBaseDesc.fSpeed = 0.f;
	MonKorsicaBaseDesc.eRenderType = RenderType::Normal;
	MonKorsicaBaseDesc.iPriority = 7;
	MonKorsicaBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MonKorsicaBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkMonKorsicaBase";
	MonKorsicaBaseDesc.TextureLevel = LEVEL_STATIC;
	MonKorsicaBaseDesc.iTextureNum = 0;
	MonKorsicaBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_MONKORSICA] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MonKorsicaBaseDesc));

	CUi_Sprite::Desc MonKorsicaNormalEyesDesc{};
	MonKorsicaNormalEyesDesc.fX = -500.f;
	MonKorsicaNormalEyesDesc.fY = -250.f;
	MonKorsicaNormalEyesDesc.fZ = 0.69f;
	MonKorsicaNormalEyesDesc.fSizeX = 550.f;
	MonKorsicaNormalEyesDesc.fSizeY = 550.f;
	MonKorsicaNormalEyesDesc.fSizeZ = 1.f;
	MonKorsicaNormalEyesDesc.fSpeed = 0.f;
	MonKorsicaNormalEyesDesc.iMaxFrame = 2;
	MonKorsicaNormalEyesDesc.eRenderType = RenderType::Normal;
	MonKorsicaNormalEyesDesc.iPriority = 8;
	MonKorsicaNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MonKorsicaNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMonKorsicaNormalEyes";
	MonKorsicaNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	MonKorsicaNormalEyesDesc.iTextureNum = 0;
	MonKorsicaNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_MONKORSICA, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MonKorsicaNormalEyesDesc)));

	CUi_Sprite::Desc MonKorsicaAngerEyesDesc = MonKorsicaNormalEyesDesc;
	MonKorsicaAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMonKorsicaAngerEyes";
	AddCharacterFace(CHAR_MONKORSICA, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MonKorsicaAngerEyesDesc)));

	CUi_Sprite::Desc MonKorsicaConfidentEyesDesc = MonKorsicaNormalEyesDesc;
	MonKorsicaConfidentEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMonKorsicaConfidentEyes";
	AddCharacterFace(CHAR_MONKORSICA, FACE_CONFIDENT, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MonKorsicaConfidentEyesDesc)));

	CUi_Sprite::Desc MonKorsicaSurpriseEyesDesc = MonKorsicaNormalEyesDesc;
	MonKorsicaSurpriseEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMonKorsicaSurpriseEyes";
	AddCharacterFace(CHAR_MONKORSICA, FACE_SURPRISE, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MonKorsicaSurpriseEyesDesc)));
}

void CUi_TalkSystem::Create_TalkMimosa()
{
	CUi_Sprite::Desc MimosaBaseDesc{};
	MimosaBaseDesc.fX = -500.f;
	MimosaBaseDesc.fY = -250.f;
	MimosaBaseDesc.fZ = 0.7f;
	MimosaBaseDesc.fSizeX = 550.f;
	MimosaBaseDesc.fSizeY = 550.f;
	MimosaBaseDesc.fSizeZ = 1.f;
	MimosaBaseDesc.fSpeed = 0.f;
	MimosaBaseDesc.eRenderType = RenderType::Normal;
	MimosaBaseDesc.iPriority = 7;
	MimosaBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MimosaBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkMimosaBase";
	MimosaBaseDesc.TextureLevel = LEVEL_STATIC;
	MimosaBaseDesc.iTextureNum = 0;
	MimosaBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_MIMOSA] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MimosaBaseDesc));

	CUi_Sprite::Desc MimosaNormalEyesDesc{};
	MimosaNormalEyesDesc.fX = -500.f;
	MimosaNormalEyesDesc.fY = -250.f;
	MimosaNormalEyesDesc.fZ = 0.69f;
	MimosaNormalEyesDesc.fSizeX = 550.f;
	MimosaNormalEyesDesc.fSizeY = 550.f;
	MimosaNormalEyesDesc.fSizeZ = 1.f;
	MimosaNormalEyesDesc.fSpeed = 0.f;
	MimosaNormalEyesDesc.iMaxFrame = 2;
	MimosaNormalEyesDesc.eRenderType = RenderType::Normal;
	MimosaNormalEyesDesc.iPriority = 8;
	MimosaNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MimosaNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMimosaNormalEyes";
	MimosaNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	MimosaNormalEyesDesc.iTextureNum = 0;
	MimosaNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_MIMOSA, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MimosaNormalEyesDesc)));

	CUi_Sprite::Desc MimosaAngerEyesDesc = MimosaNormalEyesDesc;
	MimosaAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMimosaAngerEyes";
	AddCharacterFace(CHAR_MIMOSA, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MimosaAngerEyesDesc)));

	CUi_Sprite::Desc MimosaConfidentEyesDesc = MimosaNormalEyesDesc;
	MimosaConfidentEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkMimosaConfidentEyes";
	AddCharacterFace(CHAR_MIMOSA, FACE_CONFIDENT, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MimosaConfidentEyesDesc)));
}

void CUi_TalkSystem::Create_TalkKale()
{
	CUi_Sprite::Desc KaleBaseDesc{};
	KaleBaseDesc.fX = -500.f;
	KaleBaseDesc.fY = -250.f;
	KaleBaseDesc.fZ = 0.7f;
	KaleBaseDesc.fSizeX = 550.f;
	KaleBaseDesc.fSizeY = 550.f;
	KaleBaseDesc.fSizeZ = 1.f;
	KaleBaseDesc.fSpeed = 0.f;
	KaleBaseDesc.eRenderType = RenderType::Normal;
	KaleBaseDesc.iPriority = 7;
	KaleBaseDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	KaleBaseDesc.TextureFileName = L"Prototype_Component_Texture_TalkKaleBase";
	KaleBaseDesc.TextureLevel = LEVEL_STATIC;
	KaleBaseDesc.iTextureNum = 0;
	KaleBaseDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_KALE] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KaleBaseDesc));

	CUi_Sprite::Desc KaleNormalEyesDesc{};
	KaleNormalEyesDesc.fX = -500.f;
	KaleNormalEyesDesc.fY = -250.f;
	KaleNormalEyesDesc.fZ = 0.69f;
	KaleNormalEyesDesc.fSizeX = 550.f;
	KaleNormalEyesDesc.fSizeY = 550.f;
	KaleNormalEyesDesc.fSizeZ = 1.f;
	KaleNormalEyesDesc.fSpeed = 0.f;
	KaleNormalEyesDesc.iMaxFrame = 2;
	KaleNormalEyesDesc.eRenderType = RenderType::Normal;
	KaleNormalEyesDesc.iPriority = 8;
	KaleNormalEyesDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	KaleNormalEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKaleNormalEyes";
	KaleNormalEyesDesc.TextureLevel = LEVEL_STATIC;
	KaleNormalEyesDesc.iTextureNum = 0;
	KaleNormalEyesDesc.ShaderPass = ShaderPass::Blend;
	AddCharacterFace(CHAR_KALE, FACE_NORMAL, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KaleNormalEyesDesc)));

	CUi_Sprite::Desc KaleAngerEyesDesc = KaleNormalEyesDesc;
	KaleAngerEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKaleAngerEyes";
	AddCharacterFace(CHAR_KALE, FACE_ANGER, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KaleAngerEyesDesc)));

	CUi_Sprite::Desc KaleHappyEyesDesc = KaleNormalEyesDesc;
	KaleHappyEyesDesc.TextureFileName = L"Prototype_Component_Texture_TalkKaleHappyEyes";
	AddCharacterFace(CHAR_KALE, FACE_HAPPY, static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KaleHappyEyesDesc)));
}

void CUi_TalkSystem::Create_TalkCNMNSECOND()
{
	CUi_Sprite::Desc CNMNSECDesc{};
	CNMNSECDesc.fX = -500.f;
	CNMNSECDesc.fY = -250.f;
	CNMNSECDesc.fZ = 0.7f;
	CNMNSECDesc.iMaxFrame = 2;
	CNMNSECDesc.eRenderType = RenderType::Normal;
	CNMNSECDesc.iPriority = 7;
	CNMNSECDesc.fSizeX = 550.f;
	CNMNSECDesc.fSizeY = 550.f;
	CNMNSECDesc.fSizeZ = 1.f;
	CNMNSECDesc.fSpeed = 0.f;
	CNMNSECDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	CNMNSECDesc.TextureFileName = L"Prototype_Component_Texture_TalkCNMNSECOND";
	CNMNSECDesc.TextureLevel = LEVEL_STATIC;
	CNMNSECDesc.iTextureNum = 0;
	CNMNSECDesc.ShaderPass = ShaderPass::Blend;
	m_pCharacter[CHAR_CNMN_SECOND] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &CNMNSECDesc));
}

void CUi_TalkSystem::LoadTalkFile(_wstring wsFileName)
{
	if (wsFileName.empty())
	{
		return;
	}

	std::locale::global(std::locale("ko_KR.UTF-8"));

	std::wifstream ReadFile;
	ReadFile.open(wsFileName.c_str());

	bool bFirstLine = false;
	m_TalkInfoList.clear();
	m_iCurrentIndex = 0;
	m_PreTalkInfo.CharType = CHAR_END;
	m_PreTalkInfo.FaceType = FACE_END;

	if (ReadFile.is_open())
	{
		std::wstring DataString;
		while (std::getline(ReadFile, DataString))
		{
			if (DataString.empty())
			{
				continue;
			}

			if (!bFirstLine)
			{
				bFirstLine = true;
				continue;
			}

			TalkInfoDesc TalkInfo;

			for (int i = 0; i < 6; ++i)  
			{
				std::wstring sString = L"";

				auto ObjectTypeIndex = DataString.find(L",");
				if (ObjectTypeIndex == std::wstring::npos)
				{
					sString = DataString;
				}
				else
				{
					sString = DataString.substr(0, ObjectTypeIndex);
					DataString = DataString.substr(ObjectTypeIndex + 1);
				}

				if (sString.empty())
				{
					break;
				}

				switch (i)
				{
				case INFO_NAME_CHARACTER:
				{
					TalkInfo.CharType = TALK_CHAR(std::stoi(sString));
				}
				break;

				case INFO_NAME_TEXT:
				{
					TalkInfo.Text = sString;
				}
				break;

				case INFO_NAME_NAME:
				{
					TalkInfo.Name = sString;
				}
				break;

				case INFO_NAME_FACE:
				{
					TalkInfo.FaceType = TALK_FACE(std::stoi(sString));
				}
				break;

				case 4: 
				{
					TalkInfo.SoundChannel = WStringToString(sString);
				}
				break;

				case 5: 
				{
					TalkInfo.SoundFileName = sString;
				}
				break;

				default: break;
				}
			}
			m_TalkInfoList.emplace_back(TalkInfo);
		}
	}

	ReadFile.close();
}

void CUi_TalkSystem::ProceedTalk()
{
	if (m_iCurrentIndex < m_TalkInfoList.size())
	{
		ShowTalk();
	}
	else
	{
		ClearTalk();
	}
}

void CUi_TalkSystem::ShowTalk()
{
	PLAYERMANAGER->SetTalkStarted(true);
	PLAYERMANAGER->ResetAutoTalkTimer();

	if (m_pBlackBoard != nullptr)
	{
		m_pBlackBoard->SetVisible(true);
	}

	if (m_pWindowFont != nullptr)
	{
		m_pWindowFont->SetVisible(true);
	}

	for (int i = WINDOW_POINT; i < WINDOW_END; ++i)
	{
		if (m_pWindowSprite[i] != nullptr)
		{
			m_pWindowSprite[i]->SetVisible(true);
		}
	}

	if (m_iCurrentIndex < m_TalkInfoList.size())
	{
		TalkInfoDesc TalkInfo = m_TalkInfoList[m_iCurrentIndex++];

		m_pBlackBoard->Set_Text(TalkInfo.Name);
		m_pWindowFont->Set_Text(TalkInfo.Text);

		if (!TalkInfo.SoundFileName.empty())
		{
			 GAMEINSTANCE->Play_Unique_Sound_Channel(TalkInfo.SoundChannel, SOUND_CHANNEL::SOUND_EFFECT, TalkInfo.SoundFileName);
		}

		bool isSameCharAndFace = (m_PreTalkInfo.CharType == TalkInfo.CharType) && (m_PreTalkInfo.FaceType == TalkInfo.FaceType);

		for (int i = 0; i < CHAR_END; ++i)
		{
			if (m_pCharacter[i] != nullptr)
			{
				_bool ChangedCharacter = m_PreTalkInfo.CharType == i && m_PreTalkInfo.CharType != TalkInfo.CharType;

				m_pCharacter[i]->SetVisible(TalkInfo.CharType == i || ChangedCharacter);

				if (TalkInfo.CharType == i && !isSameCharAndFace)
				{
					int iMaxFrame = m_pCharacter[i]->GetMaxFrame();
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							for (int i = WINDOW_POINT; i < WINDOW_END; ++i)
							{
								m_pWindowSprite[i]->SetVisible(true);
							}
							m_pWindowFont->SetVisible(true);
							m_pBlackBoard->SetVisible(true);

							m_pCharacter[i]->Set_RenderType(RenderType::Normal);
							m_pCharacter[i]->Set_RenderPriority(9);
							m_pCharacter[i]->PlayAnimation(CUI_Sequence::Create(
								                           CUI_Move::Create(_float3(-440.f, -230.f, 1.f), 0.15f),
								                           CUI_Frame::Create(0, iMaxFrame, 0.3f),
								                           CUI_SequenceEnd::Create()
							));

							m_pCharacter[i]->Set_Color({ 0.f, 0.f, 0.f, 1.f });
							m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
								{
									m_pCharacter[i]->Set_Color({ 0.3f, 0.3f, 0.3f, 1.f });
								}, 0.03f));
							m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
								{
									m_pCharacter[i]->Set_Color({ 0.5f, 0.5f, 0.5f, 1.f });
								}, 0.06f));
							m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
								{
									m_pCharacter[i]->Set_Color({ 0.7f, 0.7f, 0.7f, 1.f });
								}, 0.09f));
							m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
								{
									m_pCharacter[i]->Set_Color({ 0.9f, 0.9f, 0.9f, 1.f });
								}, 0.12f));
							m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
								{
									m_pCharacter[i]->Set_Color({ 1.f, 1.f, 1.f, 1.f });
								}, 0.15f));
						}, 0.05f));
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->PlayAnimation(CUI_Frame::Create(0, iMaxFrame, 0.3f));
						}, 1.7f));

					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->PlayAnimation(CUI_Frame::Create(0, iMaxFrame, 0.3f));
						}, 3.2f));

				}
				else if (TalkInfo.CharType == i && isSameCharAndFace) 
				{
					int iMaxFrame = m_pCharacter[i]->GetMaxFrame();
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->PlayAnimation(CUI_Frame::Create(0, iMaxFrame, 0.3f));
						}, 0.2f));
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->PlayAnimation(CUI_Frame::Create(0, iMaxFrame, 0.3f));
						}, 1.7f));
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->PlayAnimation(CUI_Frame::Create(0, iMaxFrame, 0.3f));
						}, 3.2f));
				}
				else if (ChangedCharacter)
				{
					for (int i = WINDOW_POINT; i < WINDOW_END; ++i)
					{
						m_pWindowSprite[i]->SetVisible(false);
					}
					m_pWindowFont->SetVisible(false);
					m_pBlackBoard->SetVisible(false);

					m_pCharacter[i]->Set_RenderType(RenderType::BackGround);
					m_pCharacter[i]->Set_RenderPriority(7);
					m_pCharacter[i]->PlayAnimation(CUI_Move::Create(_float3(-500.f, -250.f, 1.f), 0.15f));

					m_pCharacter[i]->Set_Color({ 0.9f, 0.9f, 0.9f, 0.9f });
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->Set_Color({ 0.7f, 0.7f, 0.7f, 1.f });
						}, 0.03f));
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->Set_Color({ 0.5f, 0.5f, 0.5f, 1.f });
						}, 0.06f));
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->Set_Color({ 0.2f, 0.2f, 0.2f, 1.f });
						}, 0.09f));
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->Set_Color({ 0.1f, 0.1f, 0.1f, 1.f });
						}, 0.12f));
					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->Set_Color({ 0.f, 0.f, 0.f, 1.f });
						}, 0.15f));

					m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pCharacter[i]->SetVisible(false);
						}, 0.15f));

					if (m_PreTalkInfo.CharType == i)
					{
						for (auto& iter : m_pCharacterFace[i])
						{
							for (auto& viter : iter.second)
							{
								viter->Set_RenderType(RenderType::BackGround);
								viter->Set_RenderPriority(8);
								viter->PlayAnimation(CUI_Move::Create(_float3(-500.f, -250.f, 1.f), 0.15f));

								viter->Set_Color({ 0.9f, 0.9f, 0.9f, 1.f });
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.7f, 0.7f, 0.7f, 1.f });
									}, 0.03f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.5f, 0.5f, 0.5f, 1.f });
									}, 0.06f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.2f, 0.2f, 0.2f, 1.f });
									}, 0.09f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.1f, 0.1f, 0.1f, 1.f });
									}, 0.12f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.f, 0.f, 0.f, 1.f });
									}, 0.15f));

								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->SetVisible(false);
									}, 0.15f));
							}
						}
					}
				}
			}

			for (auto& iter : m_pCharacterFace[i])
			{
				for (auto& viter : iter.second)
				{
					if (TalkInfo.CharType == i && TalkInfo.FaceType == iter.first && !isSameCharAndFace)
					{
						viter->PlayAnimation(CUI_CallFunction::Create([=]()
							{
								viter->Set_RenderType(RenderType::Normal);
								viter->Set_RenderPriority(10);
								viter->PlayAnimation(CUI_Sequence::Create(
									CUI_Move::Create(_float3(-440.f, -230.f, 1.f), 0.15f),
									CUI_Frame::Create(0, 2, 0.3f),
									CUI_SequenceEnd::Create()
								));

								viter->Set_Color({ 0.f, 0.f, 0.f, 1.f });
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.3f, 0.3f, 0.3f, 1.f });
									}, 0.03f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.5f, 0.5f, 0.5f, 1.f });
									}, 0.06f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.7f, 0.7f, 0.7f, 1.f });
									}, 0.09f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 0.9f, 0.9f, 0.9f, 1.f });
									}, 0.12f));
								viter->PlayAnimation(CUI_CallFunction::Create([=]()
									{
										viter->Set_Color({ 1.f, 1.f, 1.f, 1.f });
									}, 0.15f));
							}, 0.05f));

						viter->PlayAnimation(CUI_CallFunction::Create([=]()
							{
								viter->PlayAnimation(CUI_Frame::Create(0, 2, 0.3f));
							}, 1.7f));

						viter->PlayAnimation(CUI_CallFunction::Create([=]()
							{
								viter->PlayAnimation(CUI_Frame::Create(0, 2, 0.3f));
							}, 3.2f));
					}
					else if (TalkInfo.CharType == i && TalkInfo.FaceType == iter.first && isSameCharAndFace)
					{
						viter->PlayAnimation(CUI_CallFunction::Create([=]()
							{
								viter->PlayAnimation(CUI_Frame::Create(0, 2, 0.3f));
							}, 0.2f));
						viter->PlayAnimation(CUI_CallFunction::Create([=]()
							{
								viter->PlayAnimation(CUI_Frame::Create(0, 2, 0.3f));
							}, 1.7f));
						viter->PlayAnimation(CUI_CallFunction::Create([=]()
							{
								viter->PlayAnimation(CUI_Frame::Create(0, 2, 0.3f));
							}, 3.2f));
					}

					if (TalkInfo.CharType == i && TalkInfo.FaceType == iter.first)
					{
						viter->SetVisible(true); 
					}
					else if (m_PreTalkInfo.CharType == i)
					{
						viter->PlayAnimation(CUI_CallFunction::Create([=]()
							{
								viter->SetVisible(false); 
							}, 0.15f));
					}
				}
			}
		}

		m_PreTalkInfo = TalkInfo;
	}
}

void CUi_TalkSystem::ClearTalk()
{
	for (int i = 0; i < CHAR_END; ++i)
	{
		if (m_pCharacter[i] != nullptr)
		{
			m_pCharacter[i]->Set_RenderType(RenderType::BackGround);
			m_pCharacter[i]->Set_RenderPriority(7);
			m_pCharacter[i]->PlayAnimation(CUI_Move::Create(_float3(-500.f, -250.f, 1.f), 0.15f));

			m_pCharacter[i]->Set_Color({ 0.9f, 0.9f, 0.9f, 0.9f });
			m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
				{
					m_pCharacter[i]->Set_Color({ 0.7f, 0.7f, 0.7f, 1.f });
				}, 0.03f));
			m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
				{
					m_pCharacter[i]->Set_Color({ 0.5f, 0.5f, 0.5f, 1.f });
				}, 0.06f));
			m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
				{
					m_pCharacter[i]->Set_Color({ 0.2f, 0.2f, 0.2f, 1.f });
				}, 0.09f));
			m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
				{
					m_pCharacter[i]->Set_Color({ 0.1f, 0.1f, 0.1f, 1.f });
				}, 0.12f));
			m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
				{
					m_pCharacter[i]->Set_Color({ 0.f, 0.f, 0.f, 1.f });
				}, 0.15f));

			m_pCharacter[i]->PlayAnimation(CUI_CallFunction::Create([=]()
				{
					m_pCharacter[i]->SetVisible(false);
				}, 0.15f));
		}

		for (auto& iter : m_pCharacterFace[i])
		{
			for (auto& viter : iter.second)
			{
				viter->Set_RenderType(RenderType::BackGround);
				viter->Set_RenderPriority(8);
				viter->PlayAnimation(CUI_Move::Create(_float3(-500.f, -250.f, 1.f), 0.15f));

				viter->Set_Color({ 0.9f, 0.9f, 0.9f, 1.f });
				viter->PlayAnimation(CUI_CallFunction::Create([=]()
					{
						viter->Set_Color({ 0.7f, 0.7f, 0.7f, 1.f });
					}, 0.03f));
				viter->PlayAnimation(CUI_CallFunction::Create([=]()
					{
						viter->Set_Color({ 0.5f, 0.5f, 0.5f, 1.f });
					}, 0.06f));
				viter->PlayAnimation(CUI_CallFunction::Create([=]()
					{
						viter->Set_Color({ 0.2f, 0.2f, 0.2f, 1.f });
					}, 0.09f));
				viter->PlayAnimation(CUI_CallFunction::Create([=]()
					{
						viter->Set_Color({ 0.1f, 0.1f, 0.1f, 1.f });
					}, 0.12f));
				viter->PlayAnimation(CUI_CallFunction::Create([=]()
					{
						viter->Set_Color({ 0.f, 0.f, 0.f, 1.f });
					}, 0.15f));

				viter->PlayAnimation(CUI_CallFunction::Create([=]()
					{
						viter->SetVisible(false);
					}, 0.15f));
			}
		}
	}

	if (m_pBlackBoard != nullptr)
	{
		m_pBlackBoard->SetVisible(false);
	}

	if (m_pWindowFont != nullptr)
	{
		m_pWindowFont->SetVisible(false);
	}

	for (int i = WINDOW_POINT; i < WINDOW_END; ++i)
	{
		if (m_pWindowSprite[i] != nullptr)
		{
			m_pWindowSprite[i]->SetVisible(false);
		}
	}

	m_endCallback();
	PLAYERMANAGER->SetTalkStarted(false);
}

void CUi_TalkSystem::InitializeTalk()
{
	if (m_pBlackBoard != nullptr)
	{
		m_pBlackBoard->SetVisible(false);
	}

	if (m_pWindowFont != nullptr)
	{
		m_pWindowFont->SetVisible(false);
	}

	for (int i = WINDOW_POINT; i < WINDOW_END; ++i)
	{
		if (m_pWindowSprite[i] != nullptr)
		{
			m_pWindowSprite[i]->SetVisible(false);
		}
	}

	for (int i = 0; i < CHAR_END; ++i)
	{
		if (m_pCharacter[i] != nullptr)
		{
			m_pCharacter[i]->SetVisible(false);
		}
		for (auto& iter : m_pCharacterFace[i])
		{
			for (auto& viter : iter.second)
			{
				viter->SetVisible(false);
			}
		}
	}
}

void CUi_TalkSystem::AddCharacterFace(TALK_CHAR eCharacterType, TALK_FACE eFaceType, shared_ptr<class CUi_Sprite> InFaceSprite)
{
	if (InFaceSprite == nullptr)
	{
		return;
	}

	if (m_pCharacterFace[eCharacterType].find(eFaceType) == m_pCharacterFace[eCharacterType].end())
	{
		vector<shared_ptr<class CUi_Sprite>> vec;
		m_pCharacterFace[eCharacterType].emplace(eFaceType, vec);
	}

	m_pCharacterFace[eCharacterType][eFaceType].emplace_back(InFaceSprite);
}

void CUi_TalkSystem::ReceiveUIEvent(UI_EVENT InUIEvent)
{
	switch (InUIEvent)
	{
	case UI_EVENT_PROCEED_TALK:
	{
		return ProceedTalk();
	}
	break;

	default:break;
	}
}

shared_ptr<CUi_TalkSystem> CUi_TalkSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TalkSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TalkSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_TalkSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TalkSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TalkSystem
	{
		MakeSharedEnabler(const CUi_TalkSystem& rhs) : CUi_TalkSystem(rhs) { }
	};

	shared_ptr<CUi_TalkSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
