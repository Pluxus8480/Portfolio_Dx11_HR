#include "CUi_StoreChip.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "CUi_Chip.h"
#include "CUi_StoreChipDesc.h"
#include "iostream"
#include "PlayerManager.h"


wstring CUi_StoreChip::ObjID = L"CUi_StoreChip";

CUi_StoreChip::CUi_StoreChip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreChip::CUi_StoreChip(const CUi_StoreChip& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreChip::~CUi_StoreChip()
{
}

HRESULT CUi_StoreChip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreChip::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_ChipVecName();

	Initialize_StoreDesc();
	Initialize_ChipMap();

	Initialize_Chip();
	Initialize_Back();

	return S_OK;
}

void CUi_StoreChip::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreChip::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Check_Has();
		Adjust_Alpha(fTimeDelta);
		Progress_Store(fTimeDelta);

		wstring ChipName = m_ChipVec[m_iFocusIndex]->Get_Name();
		m_StoreChipDesc->Set_Desc(m_ChipMap.find(ChipName)->second);
	}
}

void CUi_StoreChip::LateTick(_float fTimeDelta)
{
}

void CUi_StoreChip::Initialize_StoreDesc()
{
	CUi_StoreChipDesc::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = g_iWinSizeX * 0.47f;
	Desc.fSizeY = g_iWinSizeY * 0.8f;
	Desc.fX = g_iWinSizeX * 0.5f - Desc.fSizeX * 0.5f - 20;
	Desc.fY = 0;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreDesc";
	Desc.TextureLevel = LEVEL_STATIC;
	m_StoreChipDesc = static_pointer_cast<CUi_StoreChipDesc>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_StoreChipDesc::ObjID, m_iLevelIndex, L"CUi", CUi_StoreChipDesc::ObjID, &Desc));
}

void CUi_StoreChip::Dying()
{
	for (auto& iter : m_ChipVec)
		iter->Dead();

	for (auto& iter : m_BackVec)
		iter->Dead();
}

void CUi_StoreChip::Adjust_Alpha(_float fTimeDelta)
{
	m_fAdjustAlpha += fTimeDelta;
	if (m_fAdjustAlpha < 0.5f)
	{
		for (auto& iter : m_ChipVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 3);

		for (auto& iter : m_BackVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 3);

		m_StoreChipDesc->Add_Color(ColorType::A, fTimeDelta * 3);
	}
}

void CUi_StoreChip::Check_Has()
{
	_uint UpGrade = PLAYERMANAGER->Is_UpGradeCount(m_ChipVec[m_iFocusIndex]->Get_Name());
	if (UpGrade == 0)
	{
		m_StoreChipDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::NeedBuy);
	}
	else if(UpGrade == 3)
	{
		m_StoreChipDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::Has);
	}
	else
	{
		m_StoreChipDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::NeedUpGrade);
	}
}

void CUi_StoreChip::Set_Active(_bool Active)
{
	m_bActive = Active;
	for (auto& iter : m_ChipVec)
		iter->Set_Active(Active);

	for (auto& iter : m_BackVec)
		iter->Set_Active(Active);

	if (m_bActive)
	{
		for (auto& iter : m_ChipVec)
			iter->Set_Color(ColorType::A, 0);

		for (auto& iter : m_BackVec)
			iter->Set_Color(ColorType::A, 0);

		m_StoreChipDesc->Set_Color(ColorType::A, 0);
		m_fAdjustAlpha = 0;
	}
}

void CUi_StoreChip::Progress_Store(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_LEFT))
	{
		if (m_iFocusIndex > 0)
		{
			m_ChipVec[m_iFocusIndex]->Set_Focus(false);
			--m_iFocusIndex;
			m_ChipVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_RIGHT))
	{
		if (m_iFocusIndex < m_ChipVec.size() - 1)
		{
			m_ChipVec[m_iFocusIndex]->Set_Focus(false);
			++m_iFocusIndex;
			m_ChipVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_UP))
	{
		if (m_iFocusIndex > 3)
		{
			m_ChipVec[m_iFocusIndex]->Set_Focus(false);
			m_iFocusIndex -= 4;
			m_ChipVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_DOWN))
	{
		if (m_iFocusIndex< m_ChipVec.size() - 4)
		{
			m_ChipVec[m_iFocusIndex]->Set_Focus(false);
			m_iFocusIndex += 4;
			m_ChipVec[m_iFocusIndex]->Set_Focus(true);
		}
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_ESCAPE))
	{
		Return_Menu();
	}

	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_SPACE))
	{
		if (m_ChipVec[m_iFocusIndex]->Get_Price() <= PLAYERMANAGER->Get_PlayerGear())
		{
			m_fBuyTime += fTimeDelta;
			m_StoreChipDesc->Set_BuyBarPercent(m_fBuyTime * 100);
			if (m_fBuyTime > 1)
			{
				m_fBuyTime = 0;
				Select_Bar();
			}
		}
	}
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	//{
	//	Select_Bar();
	//}
	else
	{
		m_fBuyTime = 0;
	}
}

void CUi_StoreChip::Select_Bar()
{
	PLAYERMANAGER->Sub_PlayerGear(m_ChipVec[m_iFocusIndex]->Get_Price());

	CPlayer_Manager::ChipDesc Desc;
	Desc.iUpGrade = 1;
	PLAYERMANAGER->Add_Thing(CPlayer_Manager::ThingType::Chip, m_ChipVec[m_iFocusIndex]->Get_Name(), &Desc);

	_uint UpGrade = PLAYERMANAGER->Is_UpGradeCount(m_ChipVec[m_iFocusIndex]->Get_Name());
	if (UpGrade == 3)
	{
		m_StoreChipDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::Has);
	}
	else if (UpGrade == 0)
	{
		m_StoreChipDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::NeedBuy);
	}
	else
	{
		m_StoreChipDesc->Set_BuyBarType(CUi_StoreBuyBar::Type::NeedUpGrade);
	}
}

void CUi_StoreChip::Initialize_Back()
{
	CUi_Default::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 220;
	Desc.fSizeY = 60;
	Desc.fX = -_float(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.5f + 30;
	Desc.fY = g_iWinSizeY * 0.5f - Desc.fSizeY * 0.5f - 20;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreTitle";
	Desc.TextureLevel = LEVEL_STATIC;
	m_BackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 220;
	Desc.fSizeY = 60;
	Desc.fX = -_float(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.5f + 30;
	Desc.fY = -(_float)g_iWinSizeY * 0.5f + Desc.fSizeY * 0.5f + 20;
	Desc.fZ = 0.9f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.TextureFileName = L"Prototype_Component_Texture_TitleEquip";
	m_BackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
}

void CUi_StoreChip::Initialize_Chip()
{
	CUi_Chip::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.fSizeX = 80;
	Desc.fSizeY = 80;
	Desc.fZ = 0.7f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreChip";
	Desc.TextureLevel = LEVEL_STATIC;
	for (_int i = 0; i < 20; ++i)
	{
		Desc.fX = _float(-470 + (i % 4) * 100);
		Desc.fY = _float(230 - _int(i / 4) * 100);
		Desc.ChipName = m_ChipNameVec[i];
		Desc.iPrice = i * 100;
		
		m_ChipVec.emplace_back(static_pointer_cast<CUi_Chip>(CUi_Mgr::Get_Instance()->
			Add_Clone_Return(CUi_Chip::ObjID, m_iLevelIndex, L"CUi", CUi_Chip::ObjID, &Desc)));
	}

	m_ChipVec[0]->Set_Focus(true);
}

void CUi_StoreChip::Initialize_ChipMap()
{
	CUi_StoreChipDesc::ChipDesc Desc;
	for (int i = 0; i < 20; ++i)
	{
		Desc.UpGrade_Gap = 1;
		Desc.ChipPrice = m_ChipNameVec[i];
		Desc.PriceFont = m_ChipNameVec[i];
		Desc.TitleFont = m_ChipNameVec[i];
		Desc.ExplainFont = L"Ό³Έν";
		Desc.iEnterCount = 0;
		m_ChipMap.emplace(m_ChipNameVec[i], Desc);
	}
}

void CUi_StoreChip::Initialize_ChipVecName()
{
	for (int i = 0; i < 20; ++i)
	{
		m_ChipNameVec.emplace_back(to_wstring(i));
	}
}

shared_ptr<CUi_StoreChip> CUi_StoreChip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreChip
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreChip(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreChip> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreChip::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreChip
	{
		MakeSharedEnabler(const CUi_StoreChip& rhs) : CUi_StoreChip(rhs) { }
	};

	shared_ptr<CUi_StoreChip> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
