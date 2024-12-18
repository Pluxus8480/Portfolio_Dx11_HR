#include "CUi_SpecialComboAttackSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "PlayerManager.h"
#include "Beat_Manager.h"
#include "iostream"
#include "CUi_JudgeWord.h"
#include "CUi_Mgr.h"
#include "CUi_MImosaMusicBattleSystem.h"
#include "CUi_DefaultLife.h"

wstring CUi_SpecialComboAttackSystem::ObjID = L"CUi_SpecialComboAttackSystem";

CUi_SpecialComboAttackSystem::CUi_SpecialComboAttackSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_SpecialComboAttackSystem::CUi_SpecialComboAttackSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_SpecialComboAttackSystem::~CUi_SpecialComboAttackSystem()
{
}

HRESULT CUi_SpecialComboAttackSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_DefaultLife::ObjID);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TestJudgeBeat",
			L"../Bin/Resources/Textures/NullWhite.png"));

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Rythm_Fail"), TEXT("../Bin/Resources/Sounds/Ui/Mimosa_Rythm_Fail.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Rythm_Success"), TEXT("../Bin/Resources/Sounds/Ui/Mimosa_Rythm_Success.ogg")));

	return S_OK;
}

HRESULT CUi_SpecialComboAttackSystem::Initialize(void* pArg)
{
	CUi_Mgr::Get_Instance()->Ui_AllActive(false);
	Initialize_Transform();
	Initialize_Back();
	Initialize_ComboDot();
	m_InputKeyVec.reserve(50);
	m_JudgeBeatVec.reserve(20);
	m_RhythmCollectionList = PLAYERMANAGER->Get_Dance_Rhythm();
	m_FirstBeatList = PLAYERMANAGER->Get_Dance_Beats();

	SPComboAttack* eDesc = static_cast<SPComboAttack*>(pArg);
	m_eSPCombo_Type = eDesc->eSPCombo_Type;

	GAMEINSTANCE->Add_Unique_Sound_Channel("Guitar_Rythm");

	return S_OK;
}

void CUi_SpecialComboAttackSystem::PriorityTick(_float fTimeDelta)
{
	
}

void CUi_SpecialComboAttackSystem::Tick(_float fTimeDelta)
{
	Progress_State(fTimeDelta);

	if (m_eSPCombo_Type == SPCombo_Type::Mimosa)
		Dance_Battle_KeyPress(fTimeDelta);
	else
		KeyPress(fTimeDelta);
}

void CUi_SpecialComboAttackSystem::LateTick(_float fTimeDelta)
{

}

void CUi_SpecialComboAttackSystem::Progress_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case State::Start:
		Progress_Start(fTimeDelta);
		break;
	case State::Process:
		Progress_Process(fTimeDelta);
		break;
	default:
		break;
	}

}

void CUi_SpecialComboAttackSystem::Progress_Start(_float fTimeDelta)
{
	Set_State(State::Process);
}

void CUi_SpecialComboAttackSystem::Progress_Process(_float fTimeDelta)
{
	for (auto& iter : m_pComboDotVec)
		iter->Move(MoveDir::Left, fTimeDelta);
	for (auto& iter : m_InputKeyVec)
		iter->Move(MoveDir::Left, fTimeDelta);

	if (m_InputKeyVec.size() == 0)
	{
		m_fNextStartTime += fTimeDelta;
		_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
		if (m_fNextStartTime >= Beat * 2.2f)
		{
			m_fNextStartTime = 0;
			Set_State(State::Start);
		}
	}
	else if (m_InputKeyVec.back()->Get_Pos().x <= m_fJudgePointX - 450)
	{
		Set_State(State::Start);
	}	
}

void CUi_SpecialComboAttackSystem::Set_State(State eState)
{
	switch (eState)
	{
	case State::Start:
		Set_Start();
		break;
	case State::Process:
		Set_Process();
		break;
	default:
		break;
	}
}

void CUi_SpecialComboAttackSystem::Set_Process()
{
	m_eState = State::Process;
	Create_Rhythm();
	for (size_t i = 0; i < m_pComboDotVec.size(); ++i)
	{
		m_pComboDotVec[i]->Set_Pos(-300 + i * m_fDotInterval, m_DotStartPos.y);
	}
	m_fStartBeat = CBeat_Manager::Get_Instance()->Get_BeatCount() + BeatPercent();
}

void CUi_SpecialComboAttackSystem::Set_Start()
{
	m_eState = State::Start;
	if (m_RhythmCollectionList.size() == 0)
	{
		Dead();
	}
}

void CUi_SpecialComboAttackSystem::Initialize_Back()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 1350;
	Desc.fSizeY = 200;
	Desc.fX = 0;
	Desc.fY = m_DotStartPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_SpecialComboBG";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_SpecialComboAttackSystem::Initialize_ComboDot()
{
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
	m_pComboDotVec.reserve(50);
	CUi_Default::Desc Desc;
	Desc.fSizeY = 100;
	Desc.fY = -230;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_SpecialComboDot";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.fSpeed = m_fDotInterval * 2 / fBeat;
	for (size_t i = 0; i < 50; i++)
	{
		Desc.fX = 300 + i * m_fDotInterval;
		if (i % 2 == 0)
		{
			Desc.fSizeX = 30;
			auto Dot = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
				->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
			m_pComboDotVec.emplace_back(Dot);
		}
		else
		{
			Desc.fSizeX = 15;
			auto Dot = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
				->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
			m_pComboDotVec.emplace_back(Dot);
			Dot->Set_TextureNum(1);
		}
	}
}

void CUi_SpecialComboAttackSystem::Create_Rhythm()
{
	for (auto& iter : m_InputKeyVec)
		iter->Dead();
	m_InputKeyVec.clear();
	m_JudgeBeatVec.clear();

	Set_RhythmList();
	Set_KeyboardInputStartPos();
	Create_KeyboardInput();
	Set_JudgeBeat();
	Initialize_Count((_uint)m_RhythmList.size());
	m_RhythmList.clear();
	m_fChaiTurnBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	m_fChaiTurnBeat += m_fRealBeatJudgeInterval - 1;
}

_bool CUi_SpecialComboAttackSystem::Cal_Judge(_uint iPressKey)
{
	for (size_t i = 0; i < m_InputKeyVec.size(); ++i)
	{
		if (abs(m_InputKeyVec[i]->Get_Pos().x - m_fJudgePointX) < 100)
		{
			if (m_InputKeyVec[i]->Get_TextureNum() == iPressKey)
			{
				_float3 Pos = m_InputKeyVec[i]->Get_Pos();
				Create_Judge(true, _float2(Pos.x - 20, Pos.y));

				m_InputKeyVec[i]->Dead();
				m_InputKeyVec.erase(m_InputKeyVec.begin() + i);

				GAMEINSTANCE->Play_Unique_Sound_Channel("Guitar_Rythm", SOUND_CHANNEL::SOUND_EFFECT, L"Rythm_Success");

				return true;
			}
		}
	}

	GAMEINSTANCE->Play_Unique_Sound_Channel("Guitar_Rythm", SOUND_CHANNEL::SOUND_EFFECT, L"Rythm_Fail");
	Create_Judge(false, _float2(m_fJudgePointX - 10, -230));
	return false;
}

void CUi_SpecialComboAttackSystem::Dying()
{
	if (m_eSPCombo_Type != SPCombo_Type::Kale_Finish)
		CUi_Mgr::Get_Instance()->Ui_AllActive(true);
	m_pBack->Dead();
	for (auto& iter : m_pComboDotVec)
		iter->Dead();
	for (auto& iter : m_InputKeyVec)
		iter->Dead();

	for (auto& iter : m_RhythmCollectionList)
		iter.clear();
	m_RhythmCollectionList.clear();

	auto pUI = CUi_Mgr::Get_Instance()->Find_UI(CUi_MImosaMusicBattleSystem::ObjID);
	if (pUI)
	{
		pUI->Dead();
	}

	m_JudgeBeatVec.clear();
}

void CUi_SpecialComboAttackSystem::KeyPress(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	if (pGI->Get_KeyDown(DIK_A))
	{
		if (Cal_Judge(0))
		{
			++m_iSuccessCount;
		}
	}
	else if (pGI->Get_KeyDown(DIK_D))
	{
		if (Cal_Judge(1))
		{
			++m_iSuccessCount;
		}
	}
	else if (pGI->Get_KeyDown(DIK_W))
	{
		if (Cal_Judge(2))
		{
			++m_iSuccessCount;
		}
	}
	else if (pGI->Get_KeyDown(DIK_S))
	{
		if (Cal_Judge(3))
		{
			++m_iSuccessCount;
		}
	}
}

void CUi_SpecialComboAttackSystem::Dance_Battle_KeyPress(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	if (pGI->Get_KeyDown(DIK_A))
	{
		if (Cal_Judge(0))
		{
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(0);
			++m_iSuccessCount;
		}
		else
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(4);
	}
	else if (pGI->Get_KeyDown(DIK_D))
	{
		if (Cal_Judge(1))
		{
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(1);
			++m_iSuccessCount;
		}
		else
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(4);
	}
	else if (pGI->Get_KeyDown(DIK_W))
	{
		if (Cal_Judge(2))
		{
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(2);
			++m_iSuccessCount;
		}
		else
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(4);
	}
	else if (pGI->Get_KeyDown(DIK_S))
	{
		if (Cal_Judge(3))
		{
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(3);
			++m_iSuccessCount;
		}
		else
			PLAYERMANAGER->Get_Player()->Set_DanceBattle_Anim(4);
	}
}

void CUi_SpecialComboAttackSystem::Initialize_Count(_uint Count)
{
	m_fPreviousSuccessPercent = m_iSuccessCount / (_float)m_iWholeCount;
	Set_WholeCount(Count);
	m_iSuccessCount = 0;
}

void CUi_SpecialComboAttackSystem::Set_RhythmList()
{
	m_RhythmList = m_RhythmCollectionList.front();
	m_fRealBeatJudgeInterval = m_FirstBeatList.front();
	m_FirstBeatList.pop_front();
	m_RhythmCollectionList.pop_front();
}

void CUi_SpecialComboAttackSystem::Set_KeyboardInputStartPos()
{
	m_DotStartPos.x = m_fJudgePointX + (m_fDotInterval * 2 * m_fRealBeatJudgeInterval);
}

void CUi_SpecialComboAttackSystem::Create_KeyboardInput()
{
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
	CUi_Default::Desc Desc;
	Desc.fSizeX = 50;
	Desc.fSizeY = 50;
	Desc.fY = m_DotStartPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_KeyBoardInput";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.fSpeed = (m_fDotInterval * 2) / fBeat;

	_float BeatAcc = { 0 };

	for (auto& iter : m_RhythmList)
	{
		BeatAcc += iter.first;
		Desc.fX = m_DotStartPos.x + BeatAcc * m_fDotInterval * 2;
		auto Rhythm = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
		Rhythm->Set_TextureNum(iter.second);
		m_InputKeyVec.emplace_back(Rhythm);
	}
}

void CUi_SpecialComboAttackSystem::Set_JudgeBeat()
{
	_float BeatAcc = { 0 };
	for (auto& iter : m_RhythmList)
	{
		BeatAcc += iter.first;
		m_JudgeBeatVec.emplace_back(BeatAcc + m_fRealBeatJudgeInterval);
	}
}

void CUi_SpecialComboAttackSystem::Create_Judge(_bool bSuccess, _float2 Pos)
{
	CUi_DefaultLife::Desc Desc;
	Desc.fLifeTime = 0.3f;
	Desc.fScalingSpeed = 700;
	Desc.fScaleTime = 0.15f;
	Desc.fAlphaTime = 0.3f;
	Desc.fAlphaSpeed = -2.5f;
	Desc.fX = Pos.x;
	Desc.fY = Pos.y;
	Desc.fZ = 0.1f;
	Desc.fColor = { 1, 1, 1, 1.2f };
	Desc.fSizeX = 10;
	Desc.fSizeY = 10;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_SpecialComboCircle";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pGameInstance.lock()->Add_Clone(m_iLevelIndex, L"CUi", CUi_DefaultLife::ObjID, &Desc);
	if (bSuccess)
	{
		Desc.fColor = { 1, 1, 0, 1.2f };
		Desc.TextureFileName = L"Prototype_Component_Texture_SpecialComboStar";
		m_pGameInstance.lock()->Add_Clone(m_iLevelIndex, L"CUi", CUi_DefaultLife::ObjID, &Desc);
	}
}

shared_ptr<CUi_SpecialComboAttackSystem> CUi_SpecialComboAttackSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_SpecialComboAttackSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_SpecialComboAttackSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_SpecialComboAttackSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_SpecialComboAttackSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_SpecialComboAttackSystem
	{
		MakeSharedEnabler(const CUi_SpecialComboAttackSystem& rhs) : CUi_SpecialComboAttackSystem(rhs) { }
	};

	shared_ptr<CUi_SpecialComboAttackSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
