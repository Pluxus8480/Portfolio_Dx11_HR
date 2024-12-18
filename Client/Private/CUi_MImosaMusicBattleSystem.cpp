#include "CUi_MImosaMusicBattleSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_SpecialComboAttackSystem.h"
#include "CUi_DefaultBar.h"
#include "iostream"
#include "Beat_Manager.h"


wstring CUi_MImosaMusicBattleSystem::ObjID = L"CUi_MImosaMusicBattleSystem";

CUi_MImosaMusicBattleSystem::CUi_MImosaMusicBattleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_MImosaMusicBattleSystem::CUi_MImosaMusicBattleSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_MImosaMusicBattleSystem::~CUi_MImosaMusicBattleSystem()
{
}

HRESULT CUi_MImosaMusicBattleSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_DefaultBar::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);


	return S_OK;
}

HRESULT CUi_MImosaMusicBattleSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_Back();
	Initialize_Bar();
	m_pSpecialCombo = static_pointer_cast<CUi_SpecialComboAttackSystem>
		(CUi_Mgr::Get_Instance()->Find_UI(CUi_SpecialComboAttackSystem::ObjID));
	Set_Turn(false);

	return S_OK;
}

void CUi_MImosaMusicBattleSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_MImosaMusicBattleSystem::Tick(_float fTimeDelta)
{
	Renew_Percent();
	Turn_Check();
}

void CUi_MImosaMusicBattleSystem::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
	}
}

void CUi_MImosaMusicBattleSystem::Set_SuccessPercent(_float Percent)
{
	if (0.5f < Percent)
	{
		m_fChaiPercent += 0.1f;
	}
	else if (0.25f < Percent && Percent <= 0.5f)
	{
		m_fChaiPercent += 0.05f;
	}
	else
	{
		m_fChaiPercent -= 0.05f;
	}
}

void CUi_MImosaMusicBattleSystem::Initialize_Back()
{
	m_DecoVec.reserve(5);
	CUi_Default::Desc Desc;
	Desc.fSizeX = 640;
	Desc.fSizeY = 70;
	Desc.fX = 0;
	Desc.fY = 280;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaMusicBase";
	Desc.TextureLevel = LEVEL_STATIC;
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));


	Desc.fSizeX = 100;
	Desc.fSizeY = 70;
	Desc.fX = -255;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 5;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaMusicChai";
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaMusicBack";
	Desc.iPriority = 0;
	auto pGi = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	pGi->Set_TextureNum(1);
	m_DecoVec.emplace_back(pGi);


	Desc.fX = 255;
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	Desc.iPriority = 5;

	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaMusicMimosa";
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
}

void CUi_MImosaMusicBattleSystem::Initialize_Bar()
{
	m_BarVec.reserve(2);

	CUi_DefaultBar::Desc Desc;
	Desc.fSizeX = 400;
	Desc.fSizeY = 15;
	Desc.fX = 0;
	Desc.fY = 280;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::FromRight;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaMusicChaiHp";
	Desc.TextureLevel = LEVEL_STATIC;
	m_BarVec.emplace_back(static_pointer_cast<CUi_DefaultBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_DefaultBar::ObjID, m_iLevelIndex, L"CUi", CUi_DefaultBar::ObjID, &Desc)));

	Desc.ShaderPass = ShaderPass::FromLeft;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaMusicMimosaHp";
	Desc.TextureLevel = LEVEL_STATIC;
	m_BarVec.emplace_back(static_pointer_cast<CUi_DefaultBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_DefaultBar::ObjID, m_iLevelIndex, L"CUi", CUi_DefaultBar::ObjID, &Desc)));	
}

void CUi_MImosaMusicBattleSystem::Dying()
{
	for (auto& iter : m_DecoVec)
		iter->Dead();
	for (auto& iter : m_BarVec)
		iter->Dead();
}

void CUi_MImosaMusicBattleSystem::Set_Active(_bool Active)
{
	for (auto& iter : m_DecoVec)
		iter->Set_Active(Active);
	for (auto& iter : m_BarVec)
		iter->Set_Active(Active);
}

void CUi_MImosaMusicBattleSystem::Set_Turn(_bool ChaiTurn)
{
	m_bChaiTurn = ChaiTurn;
	if (ChaiTurn)
	{
		m_DecoVec[1]->Set_TextureNum(1);
		m_DecoVec[4]->Set_TextureNum(0);
	}
	else
	{
		m_DecoVec[4]->Set_TextureNum(1);
		m_DecoVec[1]->Set_TextureNum(0);
	}
}

void CUi_MImosaMusicBattleSystem::Renew_Percent()
{
	m_BarVec[0]->Set_Percent(m_fChaiPercent);
	m_BarVec[1]->Set_Percent(m_fChaiPercent);
}

void CUi_MImosaMusicBattleSystem::Turn_Check()
{
	if (m_pSpecialCombo->Get_ChaiTurnBeat() <= BEATMANAGER->Get_BeatCount()
		&& m_bChaiTurn == false)
	{
		Set_Turn(true);

	}
	else if(m_pSpecialCombo->Get_ChaiTurnBeat() > BEATMANAGER->Get_BeatCount()
		&& m_bChaiTurn == true)
	{
		Set_SuccessPercent(m_pSpecialCombo->Get_SuccessPercent());
		Set_Turn(false);
	}
}

shared_ptr<CUi_MImosaMusicBattleSystem> CUi_MImosaMusicBattleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_MImosaMusicBattleSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_MImosaMusicBattleSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_MImosaMusicBattleSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_MImosaMusicBattleSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_MImosaMusicBattleSystem
	{
		MakeSharedEnabler(const CUi_MImosaMusicBattleSystem& rhs) : CUi_MImosaMusicBattleSystem(rhs) { }
	};

	shared_ptr<CUi_MImosaMusicBattleSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
