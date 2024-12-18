#include "CUi_StageResultSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "CUi_Mgr.h"
#include "CUi_StageCorus.h"
#include "iostream"
#include "PlayerManager.h"
#include "CUi_StageResult_Bonus.h"
#include "CUi_StageResult_FinalScore.h"

wstring CUi_StageResultSystem::ObjID = L"CUi_StageResultSystem";

CUi_StageResultSystem::CUi_StageResultSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StageResultSystem::CUi_StageResultSystem(const CUi_StageResultSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_StageResultSystem::~CUi_StageResultSystem()
{
}

HRESULT CUi_StageResultSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_StageResult_Bonus::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_StageResult_FinalScore::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_StageCorus::ObjID);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("StageResult1"), TEXT("../Bin/Resources/Sounds/Ui/Result/StageResult1.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("StageResult2"), TEXT("../Bin/Resources/Sounds/Ui/Result/StageResult2.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("StageResult3"), TEXT("../Bin/Resources/Sounds/Ui/Result/StageResult3.ogg")));

	return S_OK;
}

HRESULT CUi_StageResultSystem::Initialize(void* pArg)
{
	m_iNextLevel = reinterpret_cast<Desc*>(pArg)->iNextLevel;
	CUi_Mgr::Get_Instance()->Ui_AllActive(false);
	Initialize_Transform();
	Initialize_Screen();
	Initialize_Title();
	Initialize_CorusVec();

	return S_OK;
}

void CUi_StageResultSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_StageResultSystem::Tick(_float fTimeDelta)
{
	Check_Fade(fTimeDelta);
	Fade_Out(fTimeDelta);

	Check_Corus(fTimeDelta);
	Check_Bonus(fTimeDelta);
	Check_FinalScore(fTimeDelta);
	Check_Rank(fTimeDelta);
	Check_RankString(fTimeDelta);
	if (m_bCreateRank)
	{
		Adjust_Rank(fTimeDelta);
	}
}

void CUi_StageResultSystem::LateTick(_float fTimeDelta)
{
}

void CUi_StageResultSystem::Create_Corus()
{
	CUi_StageCorus::Desc Desc;
	Desc.CorusScore = m_CorusScoreVec[m_CorusCreateCount].first;
	Desc.eRank = m_CorusScoreVec[m_CorusCreateCount].second;
	Desc.CorusStage = m_CorusCreateCount + 1;
	Desc.fSizeX = 260; //180 * 1.4f;
	Desc.fSizeY = 120; //85 * 1.4f;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fX = -500.f + (_int(m_CorusCreateCount % 5) * 220);
	Desc.fY = 110.f - (_int(m_CorusCreateCount / 5) * 140);
	Desc.fZ = 0.7f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StageResult_Corus";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pCorusVec.emplace_back(static_pointer_cast<CUi_StageCorus>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StageCorus::ObjID, m_iLevelIndex, L"CUi", CUi_StageCorus::ObjID, &Desc)));

	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StageResult1");
}

void CUi_StageResultSystem::Create_Fade()
{
	CUi_Default::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.1f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 100;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_Fade";
	Desc.TextureLevel = LEVEL_STATIC;

	m_pFade = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_StageResultSystem::Create_Bonus()
{
	CUi_StageResult_Bonus::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fX = -450;
	Desc.fY = -205;
#ifndef PERFECTION_LOAD
	Desc.fSizeX = g_iWinSizeX * 0.5f - 50;
	Desc.fSizeY = g_iWinSizeY * 0.3f;
#else
	Desc.fSizeX = g_iWinSizeX * 0.25f - 50;
	Desc.fSizeY = g_iWinSizeY * 0.15f;
#endif
	Desc.fZ = 0.7f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.fSpeed = 100;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StageResult_Bonus";
	Desc.TextureLevel = LEVEL_STATIC;
	//CUi_Mgr::Get_Instance()->Add_Clone(CUi_StageResult_Bonus::ObjID, m_iLevelIndex, L"CUi", CUi_StageResult_Bonus::ObjID, &Desc);
}

void CUi_StageResultSystem::Check_FinalScore(_float fTimeDelta)
{
	if (m_bCreateBonus && !m_bCreateFinalScore)
	{
		m_fFinalScoreCreateTime += fTimeDelta;
		if (m_fFinalScoreCreateTime > 0.2f)
		{
			m_bCreateFinalScore = true;
			Create_FinalScore();
		}
	}
}

void CUi_StageResultSystem::Create_FinalScore()
{
	CUi_StageResult_FinalScore::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 300;
	Desc.fSizeY = 30;
	Desc.fX = 30;
	Desc.fY = -200;
	Desc.fZ = 0.7f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.fSpeed = 100;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StageResult_UnderLine";
	Desc.TextureLevel = LEVEL_STATIC;
	CUi_Mgr::Get_Instance()->Add_Clone(CUi_StageResult_FinalScore::ObjID, m_iLevelIndex, L"CUi", CUi_StageResult_FinalScore::ObjID, &Desc);

	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StageResult2");
}

void CUi_StageResultSystem::Check_Rank(_float fTimeDelta)
{
	if (m_bCreateFinalScore && !m_bCreateRank)
	{
		m_fRankCreateTime += fTimeDelta;
		if (m_fRankCreateTime > 1.f)
		{
			m_bCreateRank = true;
			Create_Rank();
		}
	}
}

void CUi_StageResultSystem::Create_Rank()
{
	CUi_Default::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 500;
	Desc.fSizeY = 500;
	Desc.fX = 420;
	Desc.fY = -200;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_Rank";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pRank = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));


	vector<pair<_int, _int>> PairVec = PLAYERMANAGER->Get_CorusScore();
	_uint FinalRank = { 0 };
	for (size_t i = 0; i < PairVec.size(); ++i)
	{
		FinalRank += PairVec[i].second;
	}
	 
	m_pRank->Set_TextureNum((_uint)(FinalRank / PairVec.size()));
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StageResult3");
}

void CUi_StageResultSystem::Adjust_Rank(_float fTimeDelta)
{
	m_pRank->Add_Scale(-fTimeDelta * 600, -fTimeDelta * 600);
	m_pRank->Add_Color(ColorType::A, fTimeDelta * 5);
	if (m_pRank->Get_Scale().x < m_fOriginRankScale.x)
	{
		m_pRank->Set_Scale(m_fOriginRankScale.x, m_fOriginRankScale.y);
	}
	if (m_bCreateRankString)
	{
		m_pRankString->Add_Color(ColorType::A, fTimeDelta * 3);
	}
}

void CUi_StageResultSystem::Check_RankString(_float fTimeDelta)
{
	if (m_bCreateRank && !m_bCreateRankString)
	{
		m_fRankStringCreateTime += fTimeDelta;
		if (m_fRankCreateTime > 0.2f)
		{
			m_bCreateRankString = true;
			Create_RankString();
		}
	}

}

void CUi_StageResultSystem::Create_RankString()
{
	CUi_Default::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 370;
	Desc.fSizeY = 370;
	Desc.fX = 400;
	Desc.fY = -150;
	Desc.fZ = 0.1f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 10;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_FinalResultString";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pRankString = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_StageResultSystem::Adjust_RankString(_float fTimeDelta)
{
}

void CUi_StageResultSystem::Check_Fade(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_RETURN) && !m_bFadeOut)
	{
		m_bFadeOut = true;
		Create_Fade();
	}
}

void CUi_StageResultSystem::Check_Corus(_float fTimeDelta)
{
	m_fCorusCreateGap += fTimeDelta;
	if (m_fCorusCreateGap > 0.2f && !m_bCanCreateBonus)
	{
		if (m_CorusScoreVec.size() > m_CorusCreateCount)
		{
			m_fCorusCreateGap = 0;
			Create_Corus();
			++m_CorusCreateCount;
		}
		else
		{
			m_bCanCreateBonus = true;
		}
	}
}

void CUi_StageResultSystem::Check_Bonus(_float fTimeDelta)
{
	if (m_bCanCreateBonus && !m_bCreateBonus)
	{
		m_fFontCreateTime += fTimeDelta;
		if (m_fFontCreateTime > 0.2f)
		{
			m_bCreateBonus = true;
			//Create_Bonus();
		}
	}
}

void CUi_StageResultSystem::Fade_Out(_float fTimeDelta)
{
	if (m_bFadeOut)
	{
		m_fFadeAlpha += fTimeDelta * 0.8f;
		m_pFade->Set_Color(ColorType::A, m_fFadeAlpha);
		if (m_fFadeAlpha >= 1)
		{
			Dead();
		}
	}
}

void CUi_StageResultSystem::Initialize_Screen()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.95f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_StageResult_Screen";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBackGround = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_StageResultSystem::Initialize_Title()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 250;
	Desc.fSizeY = Desc.fSizeX * 0.5f;
	Desc.fX = -(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.5f + 40;
	Desc.fY = (g_iWinSizeY * 0.5f) - Desc.fSizeY  * 0.5f - 20;
	Desc.fZ = 0.85f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 1;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_StageResult_Title";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pTitle = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_StageResultSystem::Initialize_CorusVec()
{
	m_CorusScoreVec = PLAYERMANAGER->Get_CorusScore();
}

void CUi_StageResultSystem::Dying()
{
	PLAYERMANAGER->Clear_CorusScore();

	for (auto& iter : m_pCorusVec)
		iter->Dead();

	m_pFade->Dead();

	CUi_Mgr::Get_Instance()->Ui_Dead(CUi_StageResult_FinalScore::ObjID);

	//CUi_Mgr::Get_Instance()->Ui_Dead(CUi_StageResult_Bonus::ObjID);

	if(m_pBackGround)
		m_pBackGround->Dead();

	if (m_pRankString)
		m_pRankString->Dead();

	if (m_pRank)
		m_pRank->Dead();

	if (m_pTitle)
		m_pTitle->Dead();

	if (m_iNextLevel != LEVEL_END)
	{
		m_pGameInstance.lock()->Change_Level_Delayed(m_iNextLevel);
	}
	else
	{
		m_pGameInstance.lock()->Change_Level_Delayed(LEVEL_LOGO);
	}
}

shared_ptr<CUi_StageResultSystem> CUi_StageResultSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StageResultSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StageResultSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_StageResultSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StageResultSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StageResultSystem
	{
		MakeSharedEnabler(const CUi_StageResultSystem& rhs) : CUi_StageResultSystem(rhs) { }
	};

	shared_ptr<CUi_StageResultSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}