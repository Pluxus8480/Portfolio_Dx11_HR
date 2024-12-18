#include "CUi_Store.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "CUi_Default.h"
#include "CUi_Mgr.h"
#include "CUi_StoreBar.h"
#include "iostream"
#include "CUi_StoreBuyBar.h"
#include "CUi_StoreEdge.h"


wstring CUi_Store::ObjID = L"CUi_Store";

CUi_Store::CUi_Store(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_Store::CUi_Store(const CUi_Store& rhs)
	:CUi_2D(rhs)
{
}

CUi_Store::~CUi_Store()
{
}

HRESULT CUi_Store::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_StoreBar::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_StoreDesc::ObjID);


	return S_OK;
}

HRESULT CUi_Store::Initialize(void* pArg)
{
	Initialize_Transform();
	return S_OK;
}

void CUi_Store::PriorityTick(_float fTimeDelta)
{
}

void CUi_Store::Tick(_float fTimeDelta)
{

}

void CUi_Store::LateTick(_float fTimeDelta)
{

}

void CUi_Store::Progress_Store(_float fTimeDelta)
{
	auto GameInstance = m_pGameInstance.lock();
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
	else if(GameInstance->Get_KeyDown(DIK_ESCAPE))
	{
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
				GameInstance->Stop_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemBuying");
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
		GameInstance->Stop_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"StoreItemBuying");
		m_fBuyTime = 0;
	}
}

void CUi_Store::Adjust_Alpha(_float fTimeDelta)
{
	m_fAdjustAlpha += fTimeDelta;
	if (m_fAdjustAlpha < 0.5f)
	{
		for (auto& iter : m_StoreBarVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 3);

		for (auto& iter : m_BackVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 3);

		m_StoreDesc->Add_Color(ColorType::A, fTimeDelta * 3);

		for (auto& iter : m_pBoundaryVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 3);
	}
}

void CUi_Store::Select_Bar()
{
}

void CUi_Store::Return_Menu()
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_ESCAPE))
	{
		Set_Active(false);
		Dead();
		CUi_Mgr::Get_Instance()->Set_Active(L"CUi_StoreSystem", true);
	}
}


void CUi_Store::Set_Active(_bool Active)
{
	m_bActive = Active;
	for (auto& iter : m_StoreBarVec)
		iter->Set_Active(Active);

	for (auto& iter : m_BackVec)
		iter->Set_Active(Active);

	for (auto& iter : m_pBoundaryVec)
		iter->Set_Active(Active);

	m_StoreDesc->Set_Active(Active);

	if (m_bActive)
	{
		for (auto& iter : m_StoreBarVec)
			iter->Set_Color(ColorType::A, 0);

		for (auto& iter : m_BackVec)
			iter->Set_Color(ColorType::A, 0);

		m_StoreDesc->Set_Color(ColorType::A, 0);

		for(auto& iter : m_pBoundaryVec)
			iter->Set_Color(ColorType::A, 0);
		m_fAdjustAlpha = 0;
	}
}

void CUi_Store::Dying()
{
	for (auto& iter : m_StoreBarVec)
		iter->Dead();

	for (auto& iter : m_BackVec)
		iter->Dead();

	for (auto& iter : m_pBoundaryVec)
		iter->Dead();

	m_StoreDesc->Dead();
}

void CUi_Store::Check_Has()
{
}

void CUi_Store::Scrolling(_float fTimeDelta)
{
	_float Up = m_StoreBarVec[m_iFocusIndex]->Get_UpPercent();
	_float Down = m_StoreBarVec[m_iFocusIndex]->Get_DownPercent();
	if (Up != 0)
	{
		for (auto& iter : m_StoreBarVec)
			iter->Move(MoveDir::Down, fTimeDelta);
	}
	else if(Down != 1)
	{
		for (auto& iter : m_StoreBarVec)
			iter->Move(MoveDir::Up, fTimeDelta);
	}
}


void CUi_Store::Initialize_StoreDesc()
{
	CUi_StoreDesc::Desc Desc;
	Desc.fColor = { 1, 1, 1, 1 };

#ifdef PERFECTION_LOAD
	Desc.fSizeX = g_iWinSizeX * 0.235f;
	Desc.fSizeY = g_iWinSizeY * 0.4f;
	Desc.fX = g_iWinSizeX * 0.25f - Desc.fSizeX * 0.5f - 20;
#else
	Desc.fSizeX = g_iWinSizeX * 0.47f;
	Desc.fSizeY = g_iWinSizeY * 0.8f;
	Desc.fX = g_iWinSizeX * 0.5f - Desc.fSizeX * 0.5f - 20;
#endif // PERFECTION_LOAD

	Desc.fY = 0;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreDesc";
	Desc.TextureLevel = LEVEL_STATIC;
	m_StoreDesc = static_pointer_cast<CUi_StoreDesc>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_StoreDesc::ObjID, m_iLevelIndex, L"CUi", CUi_StoreDesc::ObjID, &Desc));
	m_StoreDesc->Set_Color(ColorType::A, 0);
}

void CUi_Store::Initialize_Boundary(_float Up, _float Down, _float xPos, _float xSize)
{
	CUi_StoreEdge::Desc Desc;
	Desc.fSizeX = xSize;
	Desc.fSizeY = 2;
	Desc.fX = xPos;
	Desc.fY = Up;
	Desc.fZ = 0.6f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreEdge";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	m_pBoundaryVec.emplace_back(static_pointer_cast<CUi_StoreEdge>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_StoreEdge::ObjID, m_iLevelIndex, L"CUi", CUi_StoreEdge::ObjID, &Desc)));

	Desc.fY = Down;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreEdge";
	m_pBoundaryVec.emplace_back(static_pointer_cast<CUi_StoreEdge>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_StoreEdge::ObjID, m_iLevelIndex, L"CUi", CUi_StoreEdge::ObjID, &Desc)));
}

void CUi_Store::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		break;
	default:
		assert(false);
		break;
	}

	for (auto& iter : m_StoreBarVec)
		iter->Add_Color(Type, Color);

	for (auto& iter : m_BackVec)
		iter->Add_Color(Type, Color);

	for (auto& iter : m_pBoundaryVec)
		iter->Add_Color(Type, Color);
	 
	m_StoreDesc->Add_Color(Type, Color);
}

void CUi_Store::Set_Color(ColorType Type, _float Color)
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

	for (auto& iter : m_StoreBarVec)
		iter->Set_Color(Type, Color);

	for (auto& iter : m_BackVec)
		iter->Set_Color(Type, Color);

	for (auto& iter : m_pBoundaryVec)
		iter->Set_Color(Type, Color);

	m_StoreDesc->Set_Color(Type, Color);
}

void CUi_Store::Check_Scroll()
{


}

shared_ptr<CUi_Store> CUi_Store::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_Store
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_Store(pDevice, pContext) { }
	};

	shared_ptr<CUi_Store> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_Store::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_Store
	{
		MakeSharedEnabler(const CUi_Store& rhs) : CUi_Store(rhs) { }
	};

	shared_ptr<CUi_Store> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
