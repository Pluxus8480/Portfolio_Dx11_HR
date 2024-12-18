#include "CUi_StoreSkill.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_StoreBar.h"
#include "CUi_Default.h"
#include "PlayerManager.h"
#include "CUi_StoreEdge.h"
#include "CUi_EquipDefault.h"
#include "CUi_Mgr.h"
#include "CUi_StoreSkillEquip.h"


wstring CUi_StoreSkill::ObjID = L"CUi_StoreSkill";

CUi_StoreSkill::CUi_StoreSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreSkill::CUi_StoreSkill(const CUi_StoreSkill& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreSkill::~CUi_StoreSkill()
{
}

HRESULT CUi_StoreSkill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreSkill::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_StoreDesc();
	Initialize_Boundary(250, -100, -250, 450);
	Initialize_Skill();
	Initialize_SkillMap();
	Initialize_Back();
	
	m_fSpeed = 1500;

	return S_OK;
}

void CUi_StoreSkill::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreSkill::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Check_Has();
		Scrolling(fTimeDelta);
		Progress_Store(fTimeDelta);
		Change_Equip(fTimeDelta);
		wstring SkillName = m_StoreBarVec[m_iFocusIndex]->Get_Name();
		m_StoreDesc->Set_Desc((m_SkillMap.find(SkillName)->second));
	}
}

void CUi_StoreSkill::LateTick(_float fTimeDelta)
{
}

void CUi_StoreSkill::Check_Has()
{
	map<wstring, CChai::SkillDesc> PlayerSkillMap = PLAYERMANAGER->Get_PlayerSkillMap();
	auto iter = PlayerSkillMap.find(m_StoreBarVec[m_iFocusIndex]->Get_Name());
	if (iter != PlayerSkillMap.end())
	{
		m_StoreDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::Has);
	}
	else
	{
		m_StoreDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::NeedBuy);
	}
}

void CUi_StoreSkill::Progress_Store(_float fTimeDelta)
{
	auto GameInstance = m_pGameInstance.lock();
	if (!m_bEquip)
	{
		if (GameInstance->Get_KeyDown(DIK_UP))
		{
			if (m_iFocusIndex != 0)
			{
				GameInstance->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemSelect");
				m_StoreBarVec[m_iFocusIndex]->Set_Focus(false);
				--m_iFocusIndex;
				m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
			}
		}
		else if (GameInstance->Get_KeyDown(DIK_DOWN))
		{
			if (m_iFocusIndex != m_StoreBarVec.size() - 1)
			{
				GameInstance->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemSelect");
				m_StoreBarVec[m_iFocusIndex]->Set_Focus(false);

				++m_iFocusIndex;

				m_StoreBarVec[m_iFocusIndex]->Set_Focus(true);
			}
		}
		else if (GameInstance->Get_KeyDown(DIK_E))
		{
			m_bEquip = !m_bEquip;
			static_pointer_cast<CUi_StoreSkillEquip>(CUi_Mgr::Get_Instance()->Find_UI(CUi_StoreSkillEquip::ObjID))->Initialize_SkillBar();
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

		}
		else if (GameInstance->Get_KeyDown(DIK_ESCAPE))
		{
			CUi_Mgr::Get_Instance()->Ui_Dead(CUi_StoreSkillEquip::ObjID);
			Return_Menu();
		}
		if (GameInstance->Get_KeyPressing(DIK_SPACE))
		{
			if (m_StoreBarVec[m_iFocusIndex]->Get_Price() <= PLAYERMANAGER->Get_PlayerGear())
			{
				m_fBuyTime += fTimeDelta;
				m_StoreDesc->Set_BuyBarPercent(m_fBuyTime * 100);
				if (m_fBuyTime > 1)
				{
					m_fBuyTime = 0;
					GameInstance->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemBuy0");
					Select_Bar();
				}
				if (!GameInstance->Is_Sound_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemBuying"))
				{
					GameInstance->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemBuying");
				}
			}
		}
		else
		{
			m_fBuyTime = 0;
		}
	}
	else
	{
		if(GameInstance->Get_KeyDown(DIK_E))
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

void CUi_StoreSkill::Set_Active(_bool Active)
{
	m_bActive = Active;
	for (auto& iter : m_StoreBarVec)
		iter->Set_Active(Active);

	m_StoreDesc->Set_Active(Active);

	for (auto& iter : m_pBoundaryVec)
		iter->Set_Active(Active);

	for (auto& iter : m_BackVec)
		iter->Set_Active(Active);
}

void CUi_StoreSkill::Select_Bar()
{
	PLAYERMANAGER->Sub_PlayerGear(m_StoreBarVec[m_iFocusIndex]->Get_Price());
	wstring strSkill = m_StoreBarVec[m_iFocusIndex]->Get_Name();
	CChai::SkillDesc Desc;

	if (strSkill == L"Pickmeup!")
	{
		Desc.ConsumeReverb = 1;
		Desc.SkillName = L"Pickmeup!";
		Desc.SkillType = CChai::SkillType::Recover;
	}
	else if (strSkill == L"Overdrive!")
	{
		Desc.ConsumeReverb = 4;
		Desc.SkillName = L"Overdrive!";
		Desc.SkillType = CChai::SkillType::Attack;
	}
	else if (strSkill == L"DoubleRiff!")
	{
		Desc.ConsumeReverb = 1;
		Desc.SkillName = L"DoubleRiff!";
		Desc.SkillType = CChai::SkillType::Attack;
	}
	else if (strSkill == L"TwinAssault!")
	{
		Desc.ConsumeReverb = 3;
		Desc.SkillName = L"TwinAssault!";
		Desc.SkillType = CChai::SkillType::Attack;
	}
	PLAYERMANAGER->Add_PlayerSkill(Desc.SkillName, Desc);
	m_StoreDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::Has);
}

void CUi_StoreSkill::Initialize_Skill()
{
	CUi_StoreBar::Desc Desc;
	Desc.fSizeX = 440;
	Desc.fSizeY = 50;
	Desc.fSpeed = 300;
	Desc.fX = -255;
	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBar";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.Price = 5000;
	Desc.StringFont = L"Pickmeup!";
	Desc.fUpLimitPos = m_pBoundaryVec[0]->Get_Pos().y;
	Desc.fDownLimitPos = m_pBoundaryVec[1]->Get_Pos().y;
	Desc.type = CPlayer_Manager::ThingType::Skill;
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fSpeed = 300;
	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.Price = 10000;
	Desc.StringFont = L"DoubleRiff!";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fSpeed = 300;
	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.Price = 10000;
	Desc.StringFont = L"TwinAssault!";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));


	Desc.fSpeed = 300;
	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.Price = 20000;
	Desc.StringFont = L"Overdrive!";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	m_StoreBarVec[0]->Set_Focus(true);
}

void CUi_StoreSkill::Initialize_SkillMap()
{
	CUi_StoreDesc::SkillDesc Desc;
	Desc.PriceFont = to_wstring(5000);
	Desc.TitleFont = L"Pickmeup!";
	Desc.ExplainFont = L"차이는 전투 도중에 궁지에 몰리면 예비 에너지 드링크를 꺼내 자신의&체력을 회복합니다. 에너지 드링크를 마실 때 비트 히트를 발동시키면 체력&회복량이 증가합니다!";
	Desc.iEnterCount = 2;
	m_SkillMap.emplace(L"Pickmeup!", Desc);

	Desc.PriceFont = to_wstring(10000);
	Desc.TitleFont = L"DoubleRiff!";
	Desc.ExplainFont = L"페퍼민트와 협력해 적을 날려버리세요. 페퍼민트의 무기와 차이의&뮤지컬 에너지로 전방에 연발 공격을 날리고 무엇이든 경로에 있는 것에&피해를 줍니다.&배리어가 있는 적에게 특히 효과적이며, 아머가 있는 적에게도 피해를&줍니다. ";
	Desc.iEnterCount = 3;
	m_SkillMap.emplace(L"DoubleRiff!", Desc);

	Desc.PriceFont = to_wstring(10000);
	Desc.TitleFont = L"TwinAssault!";
	Desc.ExplainFont = L"코르시카를 소환해 막대로 공격하고 콤보 공격으로 적을 마무리합니다!&공격 범위가 넓기 때문에 다수의 적을 끌어들일 가능성이 높습니다.";
	Desc.iEnterCount = 1;
	m_SkillMap.emplace(L"TwinAssault!", Desc);

	Desc.PriceFont = to_wstring(20000);
	Desc.TitleFont = L"Overdrive!";
	Desc.ExplainFont = L"차이가 리듬에 맞춰 에너지를 충전한 다음 여러 차례 참격을 날립니다.&비트 히트로 공격을 마무리하면 전방에 참격을 가해 고통에 찬 비명을&만들어 내죠! 에너지를 충분히 충전해야 최대 피해를 줄 수 있으니&주의하세요!";
	Desc.iEnterCount = 3;
	m_SkillMap.emplace(L"Overdrive!", Desc);
}

void CUi_StoreSkill::Initialize_Back()
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
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreTitle";
	Desc.TextureLevel = LEVEL_STATIC;
	m_BackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
}

void CUi_StoreSkill::Change_Equip(_float fTimeDelta)
{
	if (m_bEquip)
	{
		if (Get_Pos().x < m_fEquipPos.x)
		{
			Move(MoveDir::Right, fTimeDelta);

			m_StoreDesc->Add_Color(ColorType::A, -fTimeDelta * 3);
			for (auto& iter : m_BackVec)
				iter->Add_Color(ColorType::A, -fTimeDelta * 3);

			for (auto& iter : m_StoreBarVec)
				iter->Set_Color(ColorType::A, 0);

			if (m_StoreDesc->Get_Color().w < 0)
			{
				m_StoreDesc->Set_Color(ColorType::A, 0);

				for (auto& iter : m_StoreBarVec)
					iter->Set_Color(ColorType::A, 0);

				for (auto& iter : m_BackVec)
					iter->Set_Color(ColorType::A, 0);
			}
		}
	}
	else
	{
		if (m_fNonEquipPos.x < Get_Pos().x)
		{
			Move(MoveDir::Left, fTimeDelta);

			m_StoreDesc->Add_Color(ColorType::A, fTimeDelta * 3);

			for (auto& iter : m_StoreBarVec)
				iter->Add_Color(ColorType::A, fTimeDelta * 3);

			for (auto& iter : m_BackVec)
				iter->Add_Color(ColorType::A, fTimeDelta * 3);

			if (1 < m_StoreDesc->Get_Color().w)
			{
				m_StoreDesc->Set_Color(ColorType::A, 1);

				for (auto& iter : m_StoreBarVec)
					iter->Set_Color(ColorType::A, 1);

				for (auto& iter : m_BackVec)
					iter->Set_Color(ColorType::A, 1);
			}
		}
	}
}

void CUi_StoreSkill::Move(MoveDir Dir, _float fTimeDelta)
{
	switch (Dir)
	{
	case MoveDir::Right:
		m_pTransformCom->Go_Right(fTimeDelta * m_fSpeed);

		for (auto& iter : m_StoreBarVec)
			iter->Add_Pos(fTimeDelta * m_fSpeed, 0);

		m_StoreDesc->Add_Pos(fTimeDelta * m_fSpeed, 0);

		for (auto& iter : m_pBoundaryVec)
			iter->Add_Pos(fTimeDelta * m_fSpeed, 0);
		break;
	case MoveDir::Left:
		m_pTransformCom->Go_Left(fTimeDelta * m_fSpeed);

		for (auto& iter : m_StoreBarVec)
			iter->Add_Pos(-fTimeDelta * m_fSpeed, 0);

		m_StoreDesc->Add_Pos(-fTimeDelta * m_fSpeed, 0);

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
}

shared_ptr<CUi_StoreSkill> CUi_StoreSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreSkill
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreSkill(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreSkill> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreSkill::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreSkill
	{
		MakeSharedEnabler(const CUi_StoreSkill& rhs) : CUi_StoreSkill(rhs) { }
	};

	shared_ptr<CUi_StoreSkill> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
