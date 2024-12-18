#include "CUi_StoreSkillEquip.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "CUi_StoreBar.h"
#include "CUi_StoreEdge.h"
#include "CUi_Default.h"
#include "CUi_EquipDefault.h"
#include "CUi_StoreEquipBar.h"
#include "iostream"
#include "CUi_StoreSkill.h"
#include "CUi_Mgr.h"


wstring CUi_StoreSkillEquip::ObjID = L"CUi_StoreSkillEquip";

CUi_StoreSkillEquip::CUi_StoreSkillEquip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreSkillEquip::CUi_StoreSkillEquip(const CUi_StoreSkillEquip& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreSkillEquip::~CUi_StoreSkillEquip()
{
}

HRESULT CUi_StoreSkillEquip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreSkillEquip::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_Boundary(250, -100, -250, 450);
	Initialize_SkillBar();
	Initialize_EquipBack();
	Initialize_EquipDefault();

	m_fSpeed = 1500;

	return S_OK;
}

void CUi_StoreSkillEquip::PriorityTick(_float fTimeDelta)
{
	m_EquipWindowVec[0]->Set_Skill(PLAYERMANAGER->Get_PlayerEquipSkill());
	m_EquipWindowVec[1]->Set_Skill(PLAYERMANAGER->Get_PlayerNonEquipSkill());
}

void CUi_StoreSkillEquip::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Change_Equip(fTimeDelta);
		if (m_bReadyChange)
		{
			Check_Skill();
		}
		else
		{
			Progress_Store(fTimeDelta);
		}
	}

}

void CUi_StoreSkillEquip::LateTick(_float fTimeDelta)
{
}

void CUi_StoreSkillEquip::Progress_Store(_float fTimeDelta)
{
	auto GameInstance = m_pGameInstance.lock();
	if (m_bEquip && !m_bReadyChange)
	{
		if (m_pGameInstance.lock()->Get_KeyDown(DIK_UP))
		{
			if (m_iFocusIndex != 0 && m_StoreBarVec.size() > 1)
			{
				GameInstance->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemSelect");
				m_StoreBarVec[m_iFocusIndex]->Set_Focus(false);
				--m_iFocusIndex;
				m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
			}
		}
		else if (m_pGameInstance.lock()->Get_KeyDown(DIK_DOWN))
		{
			if (m_iFocusIndex != m_StoreBarVec.size() - 1)
			{
				GameInstance->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemSelect");
				m_StoreBarVec[m_iFocusIndex]->Set_Focus(false);

				++m_iFocusIndex;

				m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
			}
		}
		else if (m_pGameInstance.lock()->Get_KeyDown(DIK_ESCAPE))
		{
			CUi_Mgr::Get_Instance()->Ui_Dead(CUi_StoreSkill::ObjID);
			Return_Menu();
		}
		else if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
		{
			m_bEquip = !m_bEquip;
			if (!m_bEquip)
			{
				for (auto& iter : m_StoreBarVec)
					iter->Set_Active(false);
			}
			else
			{
				for (auto& iter : m_StoreBarVec)
					iter->Set_Active(true);
			}

			m_iFocusIndex = 0;
		}
		if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
		{
			Set_ReadyChange(true);
		}
	}
	else
	{
		if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
		{
			m_bEquip = !m_bEquip;
			for (auto& iter : m_StoreBarVec)
				iter->Set_Active(true);

			for (auto& iter : m_StoreBarVec)
				iter->Set_Focus(false);

			m_iFocusIndex = 0;
			m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	
}

void CUi_StoreSkillEquip::Select_Bar()
{
	if (m_EquipWindowVec[0]->Get_SkillName() != m_StoreBarVec[m_iFocusIndex]->Get_Name()
		&& m_EquipWindowVec[1]->Get_SkillName() != m_StoreBarVec[m_iFocusIndex]->Get_Name())
	{	
		CChai::SkillDesc Desc = PLAYERMANAGER->Find_PlayerSkill(m_StoreBarVec[m_iFocusIndex]->Get_Name());
		if (m_iChangeFocusIndex == 0)
		{
			PLAYERMANAGER->Set_PlayerEquipSkill(Desc);
		}
		else
		{
			PLAYERMANAGER->Set_PlayerNonEquipSkill(Desc);
		}
	}
}

void CUi_StoreSkillEquip::Move(MoveDir Dir, _float fTimeDelta)
{
	switch (Dir)
	{
	case MoveDir::Right:
		m_pTransformCom->Go_Right(fTimeDelta * m_fSpeed);

		for (auto& iter : m_StoreBarVec)
			iter->Add_Pos(fTimeDelta * m_fSpeed, 0);

		for(auto& iter : m_EquipWindowVec)
			iter->Add_Pos(fTimeDelta * m_fSpeed, 0);

		for (auto& iter : m_BackVec)
			iter->Add_Pos(fTimeDelta * m_fSpeed, 0);

		for (auto& iter : m_pBoundaryVec)
			iter->Add_Pos(fTimeDelta * m_fSpeed, 0);
		break;
	case MoveDir::Left:
		m_pTransformCom->Go_Left(fTimeDelta * m_fSpeed);

		for (auto& iter : m_StoreBarVec)
			iter->Add_Pos(-fTimeDelta * m_fSpeed, 0);

		for (auto& iter : m_EquipWindowVec)
			iter->Add_Pos(-fTimeDelta * m_fSpeed, 0);

		for (auto& iter : m_BackVec)
			iter->Add_Pos(-fTimeDelta * m_fSpeed, 0);

		for (auto& iter : m_pBoundaryVec)
			iter->Add_Pos(-fTimeDelta * m_fSpeed, 0);

		break;
	case MoveDir::Down:
		m_pTransformCom->Go_Down(fTimeDelta * m_fSpeed);
		break;
	case MoveDir::Up:
		m_pTransformCom->Go_Up(fTimeDelta * m_fSpeed);
		break;
	default:
		assert(false);
		break;
	}

	m_iFocusIndex = 0;
	m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
}

void CUi_StoreSkillEquip::Add_Color(ColorType Type, _float Color)
{
}

void CUi_StoreSkillEquip::Set_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x = Color;
		break;
	case ColorType::G:
		m_fColor.y = Color;
		break;
	case ColorType::B:
		m_fColor.z = Color;
		break;
	case ColorType::A:
		m_fColor.w = Color;
		break;
	default:
		assert(false);
		break;
	}

}

void CUi_StoreSkillEquip::Add_Pos(_float PosX, _float PosY)
{
}

void CUi_StoreSkillEquip::Set_Active(_bool Active)
{
	m_bActive = Active;
	for (auto& iter : m_StoreBarVec)
		iter->Set_Active(Active);

	for (auto& iter : m_pBoundaryVec)
		iter->Set_Active(Active);

	for (auto& iter : m_BackVec)
		iter->Set_Active(Active);

	for (auto& iter : m_EquipWindowVec)
		iter->Set_Active(Active);

	if (m_bActive)
	{
		Initialize_SkillBar();
	}
}

void CUi_StoreSkillEquip::Dying()
{
	for (auto& iter : m_StoreBarVec)
		iter->Dead();

	for (auto& iter : m_pBoundaryVec)
		iter->Dead();

	for (auto& iter : m_BackVec)
		iter->Dead();

	for (auto& iter : m_EquipWindowVec)
		iter->Dead();
}

void CUi_StoreSkillEquip::Initialize_SkillBar()
{
	for (auto& iter : m_StoreBarVec)
		iter->Dead();
	m_StoreBarVec.clear();

	map<wstring, CChai::SkillDesc> SkillMap = PLAYERMANAGER->Get_PlayerSkillMap();
	CUi_StoreEquipBar::Desc Desc;
	Desc.fSizeX = 440;
	Desc.fSizeY = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBar";
	Desc.ShaderPass = ShaderPass::StoreScrollBar;
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 10;
	for (auto& iter : SkillMap)
	{
		Desc.fSpeed = 300;
		Desc.fX = -255;
		Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
		Desc.fZ = 0.1f;
		Desc.Price = 0;
		Desc.StringFont = (iter.second).SkillName;
		Desc.fUpLimitPos = m_pBoundaryVec[0]->Get_Pos().y;
		Desc.fDownLimitPos = m_pBoundaryVec[1]->Get_Pos().y;
		m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreEquipBar>
			(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreEquipBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreEquipBar::ObjID, &Desc)));
	}

	for (auto& iter : m_StoreBarVec)
		iter->Set_Color(ColorType::A, 0);
}

void CUi_StoreSkillEquip::Initialize_EquipBack()
{
	CUi_Default::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 220;
	Desc.fSizeY = 60;
	Desc.fX = -_float(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.5f + 30;
	Desc.fY = g_iWinSizeY * 0.5f - Desc.fSizeY * 0.5f - 20;
	Desc.fZ = 0.9f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_TitleEquip";
	Desc.TextureLevel = LEVEL_STATIC;
	m_BackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));


	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 250;
	Desc.fSizeY = 500;
	Desc.fX = -_float(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.5f - 30;
	Desc.fY = -_float(g_iWinSizeY * 0.5f) + Desc.fSizeY * 0.5f;
	Desc.TextureFileName = L"Prototype_Component_Texture_EquipChaiBack";
	m_BackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));


	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY * 0.8f;
	Desc.fX = g_iWinSizeX * 0.6f;
	Desc.fY = 0;
	Desc.fZ = 0.9f;
	Desc.TextureFileName = L"Prototype_Component_Texture_EquipListBack";
	m_BackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_Default::ObjID,m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	for (auto& iter : m_BackVec)
		iter->Add_Pos(-650, 0);

	for (auto& iter : m_BackVec)
		iter->Set_Color(ColorType::A, 0);
}

void CUi_StoreSkillEquip::Initialize_EquipDefault()
{
	CUi_EquipDefault::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 550.4f;
	Desc.fSizeY = 140;
	Desc.fX = -200;
	Desc.fY = 100;
	Desc.fZ = 0.7f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_SkillEquip";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.Big = true;
	m_EquipWindowVec.emplace_back(static_pointer_cast<CUi_EquipDefault>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_EquipDefault::ObjID, m_iLevelIndex, L"CUi", CUi_EquipDefault::ObjID, &Desc)));

	Desc.fY = -200;
	m_EquipWindowVec.emplace_back(static_pointer_cast<CUi_EquipDefault>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_EquipDefault::ObjID, m_iLevelIndex, L"CUi", CUi_EquipDefault::ObjID, &Desc)));

	Desc.Big = false;
	Desc.fSizeX = 385;
	Desc.fSizeY = 98;
	Desc.fX = -200;
	Desc.fY = 200;
	m_EquipWindowVec.emplace_back(static_pointer_cast<CUi_EquipDefault>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_EquipDefault::ObjID, m_iLevelIndex, L"CUi", CUi_EquipDefault::ObjID, &Desc)));

	Desc.fY = -100;
	m_EquipWindowVec.emplace_back(static_pointer_cast<CUi_EquipDefault>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_EquipDefault::ObjID, m_iLevelIndex, L"CUi", CUi_EquipDefault::ObjID, &Desc)));

	for (auto& iter : m_EquipWindowVec)
		iter->Add_Pos(-650, 0);

	for (auto& iter : m_EquipWindowVec)
		iter->Set_Color(ColorType::A, 0);

	m_EquipWindowVec[2]->Set_TextureNum(1);
	m_EquipWindowVec[3]->Set_TextureNum(1);

	m_EquipWindowVec[2]->Set_Active(false);
	m_EquipWindowVec[3]->Set_Active(false);
}

void CUi_StoreSkillEquip::Change_Equip(_float fTimeDelta)
{
	if (m_bEquip)
	{
		if (Get_Pos().x < m_fEquipPos.x)
		{
			Move(MoveDir::Right, fTimeDelta);

			for (auto& iter : m_BackVec)
				iter->Add_Color(ColorType::A, fTimeDelta * 3);

			for (auto& iter : m_StoreBarVec)
				iter->Add_Color(ColorType::A, fTimeDelta * 3);

			for (auto& iter : m_EquipWindowVec)
				iter->Add_Color(ColorType::A, fTimeDelta * 3);

			if (m_BackVec[0]->Get_Color().w > 1)
			{
				for (auto& iter : m_BackVec)
					iter->Set_Color(ColorType::A, 1);

				for (auto& iter : m_StoreBarVec)
					iter->Set_Color(ColorType::A, 1);

				for(auto& iter : m_EquipWindowVec)
					iter->Set_Color(ColorType::A, 1);
			}
		}
	}
	else
	{
		if (m_fNonEquipPos.x < Get_Pos().x)
		{
			Move(MoveDir::Left, fTimeDelta);

			for (auto& iter : m_BackVec)
				iter->Add_Color(ColorType::A, -fTimeDelta * 3);

			for (auto& iter : m_StoreBarVec)
				iter->Add_Color(ColorType::A, -fTimeDelta * 3);

			for (auto& iter : m_EquipWindowVec)
				iter->Add_Color(ColorType::A, -fTimeDelta * 3);


			if (m_BackVec[0]->Get_Color().w < 0)
			{
				for (auto& iter : m_BackVec)
					iter->Set_Color(ColorType::A, 0);

				for (auto& iter : m_StoreBarVec)
					iter->Set_Color(ColorType::A, 0);

				for (auto& iter : m_EquipWindowVec)
					iter->Set_Color(ColorType::A, 0);
			}
		}
	}
}

void CUi_StoreSkillEquip::Check_Skill()
{
	CChai::PlayerSkillInfo Desc = PLAYERMANAGER->Find_PlayerSkill(m_StoreBarVec[m_iFocusIndex]->Get_Name());
	m_EquipWindowVec[2]->Set_Skill(Desc);
	m_EquipWindowVec[3]->Set_Skill(Desc);
	if (0 == m_iChangeFocusIndex)
	{
		m_EquipWindowVec[2]->Set_Active(true);
		m_EquipWindowVec[3]->Set_Active(false);
	}
	else if (1 == m_iChangeFocusIndex)
	{
		m_EquipWindowVec[3]->Set_Active(true);
		m_EquipWindowVec[2]->Set_Active(false);
	}

	if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	{
		Select_Bar();
		Set_ReadyChange(false);
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_BACKSPACE))
	{
		Set_ReadyChange(false);
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_DOWN))
	{
		if (m_iChangeFocusIndex == 0)
		{
			++m_iChangeFocusIndex;
			cout << m_iChangeFocusIndex << endl;
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_UP))
	{
		if (m_iChangeFocusIndex >= 1)
		{
			--m_iChangeFocusIndex;
			cout << m_iChangeFocusIndex << endl;
		}
	}
}

void CUi_StoreSkillEquip::Set_ReadyChange(_bool Change)
{
	m_bReadyChange = Change;
	cout << m_bReadyChange << endl;
	m_iChangeFocusIndex = 0;
	m_EquipWindowVec[2]->Set_Active(false);
	m_EquipWindowVec[3]->Set_Active(false);

	if (m_bReadyChange)
	{
		for (auto& iter : m_StoreBarVec)
			iter->Set_Focus(false);
	}
	else
	{
		m_iFocusIndex = 0;
		m_StoreBarVec[0]->Set_Focus(true);
	}
}

shared_ptr<CUi_StoreSkillEquip> CUi_StoreSkillEquip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreSkillEquip
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreSkillEquip(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreSkillEquip> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreSkillEquip::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreSkillEquip
	{
		MakeSharedEnabler(const CUi_StoreSkillEquip& rhs) : CUi_StoreSkillEquip(rhs) { }
	};

	shared_ptr<CUi_StoreSkillEquip> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
