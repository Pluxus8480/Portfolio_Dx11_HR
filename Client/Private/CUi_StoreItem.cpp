#include "CUi_StoreItem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_StoreBar.h"
#include "CUi_Default.h"
#include "PlayerManager.h"
#include "CUi_StoreEdge.h"


wstring CUi_StoreItem::ObjID = L"CUi_StoreItem";

CUi_StoreItem::CUi_StoreItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreItem::CUi_StoreItem(const CUi_StoreItem& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreItem::~CUi_StoreItem()
{
}

HRESULT CUi_StoreItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreItem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_StoreDesc();
	Initialize_Boundary(250, -100, -250, 450);
	Initialize_Item();
	Initialize_ItemMap();


	return S_OK;
}

void CUi_StoreItem::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreItem::Tick(_float fTimeDelta)
{
	Check_Has();
	Scrolling(fTimeDelta);
	Progress_Store(fTimeDelta);
	wstring SkillName = m_StoreBarVec[m_iFocusIndex]->Get_Name();
	m_StoreDesc->Set_Desc((m_ItemMap.find(SkillName)->second));
}

void CUi_StoreItem::LateTick(_float fTimeDelta)
{
	
}

void CUi_StoreItem::Initialize_Item()
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
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBar";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.Price = 5000;
	Desc.fUpLimitPos = m_pBoundaryVec[0]->Get_Pos().y;
	Desc.fDownLimitPos = m_pBoundaryVec[1]->Get_Pos().y;
	Desc.StringFont = L"라이프 게이지";
	Desc.type = CPlayer_Manager::ThingType::Item;
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));


	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.Price = 10000;
	Desc.StringFont = L"전기 리버브 코어";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.Price = 50000;
	Desc.StringFont = L"체력 탱크";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.Price = 40000;
	Desc.StringFont = L"특수 공격 슬롯 확장";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_StoreBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	m_StoreBarVec[0]->Set_Focus(true);
}

void CUi_StoreItem::Initialize_ItemMap()
{
	CUi_StoreDesc::ItemDesc Desc;
	Desc.PriceFont = to_wstring(5000);
	Desc.TitleFont = L"라이프 게이지";
	Desc.ExplainFont = L"암스트롱 프로젝트 부품과 호환되는 반델레이제&라이프 게이지입니다!&&차이의 최대 체력이 증가합니다!&&대박이죠!?";
	Desc.iEnterCount = 5;
	m_ItemMap.emplace(L"라이프 게이지", Desc);

	Desc.PriceFont = to_wstring(10000);
	Desc.TitleFont = L"전기 리버브 코어";
	Desc.ExplainFont = L"진공관을 기반으로 만든 반델레이제 뉴에이지& 전기 코어입니다.&&기분 좋은 짜릿함을 선사하는 데다, 차이의 최대 리버브 게이지를 하나&증가시킨다는 점이 특징입니다!";
	Desc.iEnterCount = 4;
	m_ItemMap.emplace(L"전기 리버브 코어", Desc);

	Desc.PriceFont = to_wstring(50000);
	Desc.TitleFont = L"체력 탱크";
	Desc.ExplainFont = L"여러분의 수분 보충을 책임집니다! 게다가 전용 앱도 있죠!&&이 체력 탱크는 차이가 체력을 완전히&회복할 수 있을 만큼의 에너지를 저장하며,&차이의 체력이 바닥나면 자동으로 발동됩니다.&&체력이 가득한 상태에서 회복 아이템을 주우면,&탱크에 추가 체력이 저장됩니다.";
	Desc.iEnterCount = 7;
	m_ItemMap.emplace(L"체력 탱크", Desc);

	Desc.PriceFont = to_wstring(40000);
	Desc.TitleFont = L"특수 공격 슬롯 확장";
	Desc.ExplainFont = L"특수 공격 한 개로는 부족합니다!&&슬롯을 업그레이드하면, 차이가 두 개의 특수 공격을 장착하고&그때그때 상황에 맞춰 교체할 수 있게 됩니다!&&특수 공격을 전부 장착한 뒤, 게임을 플레이하는 도중 아무 때나&정해진 키를 누르면 특수 공격을 해제할 수 있습니다.&&리버브 게이지 에너지가 충분한지 꼭 확인하세요!";
	Desc.iEnterCount = 8;
	m_ItemMap.emplace(L"특수 공격 슬롯 확장", Desc);
}

void CUi_StoreItem::Initialize_Back()
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

void CUi_StoreItem::Check_Has()
{
	if (PLAYERMANAGER->Is_HasThing(CPlayer_Manager::ThingType::Item, m_StoreBarVec[m_iFocusIndex]->Get_Name()))
		m_StoreDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::Has);
	else
		m_StoreDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::NeedBuy);
}

void CUi_StoreItem::Select_Bar()
{
	PLAYERMANAGER->Sub_PlayerGear(m_StoreBarVec[m_iFocusIndex]->Get_Price());
	CPlayer_Manager::ItemDesc Desc;
	wstring Name = m_StoreBarVec[m_iFocusIndex]->Get_Name();
	Desc.Test = 1;
	if(Name == L"라이프 게이지")
	{
		_float fMaxHp = CPlayer_Manager::Get_Instance()->Get_MaxHp();
		CPlayer_Manager::Get_Instance()->Set_MaxHp(fMaxHp + 50);
		CPlayer_Manager::Get_Instance()->Set_Hp(fMaxHp + 50);
	}
	else if(Name == L"전기 리버브 코어")
	{
		_float fReverb = CPlayer_Manager::Get_Instance()->Get_MaxReverb();
		CPlayer_Manager::Get_Instance()->Set_MaxReverb(fReverb + 50);
		CPlayer_Manager::Get_Instance()->Set_Reverb(fReverb + 50);
	}
	PLAYERMANAGER->Add_Thing(CPlayer_Manager::ThingType::Item, m_StoreBarVec[m_iFocusIndex]->Get_Name(), &Desc);
	m_StoreDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::Has);
}

shared_ptr<CUi_StoreItem> CUi_StoreItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreItem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreItem(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreItem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreItem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreItem
	{
		MakeSharedEnabler(const CUi_StoreItem& rhs) : CUi_StoreItem(rhs) { }
	};

	shared_ptr<CUi_StoreItem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
