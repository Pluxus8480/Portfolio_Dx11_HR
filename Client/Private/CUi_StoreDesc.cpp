#include "CUi_StoreDesc.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"

wstring CUi_StoreDesc::ObjID = L"CUi_StoreDesc";

CUi_StoreDesc::CUi_StoreDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StoreDesc::CUi_StoreDesc(const CUi_StoreDesc& rhs)
	:CUi_2D(rhs)
{
}

CUi_StoreDesc::~CUi_StoreDesc()
{
}

HRESULT CUi_StoreDesc::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreDesc",
			L"../Bin/Resources/Textures/Ui/Store/StoreDesc.png"));

	return S_OK;
}

HRESULT CUi_StoreDesc::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	Initialize_Font();
	Initialize_DefaultVec();
	Initialize_BuyBar();
	Initialize_Png();
	return S_OK;
}

void CUi_StoreDesc::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreDesc::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Adjust_Alpha(fTimeDelta);
		Sync_Png();
	}
}

void CUi_StoreDesc::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{		
		m_Itempng->Set_Pos(Get_Pos().x - Get_Scale().x * 0.25f, Get_Pos().y + Get_Scale().y * 0.2f);
		m_DefaultVec[0]->Set_Pos(m_PriceFont.DirectPos.x - 10, m_PriceFont.DirectPos.y - 10);
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_StoreDesc::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	RenderFont(m_TitleFont, 0);
	RenderFont(m_PriceFont, 0);
	RenderFont(m_ExplainFont, 20, m_iExplainFontEnterCount);

	return S_OK;
}

void CUi_StoreDesc::Set_Desc(AttackDesc AttackInfo)
{
	m_PriceFont.String = AttackInfo.PriceFont;
	m_ExplainFont.String = AttackInfo.ExplainFont;
	m_TitleFont.String = AttackInfo.TitleFont;
	m_iExplainFontEnterCount = AttackInfo.iEnterCount;
}

void CUi_StoreDesc::Set_Desc(SkillDesc SkillInfo)
{
	m_PriceFont.String = SkillInfo.PriceFont;
	m_ExplainFont.String = SkillInfo.ExplainFont;
	m_TitleFont.String = SkillInfo.TitleFont;
	m_iExplainFontEnterCount = SkillInfo.iEnterCount;
}

void CUi_StoreDesc::Set_Desc(ItemDesc ItemInfo)
{
	m_PriceFont.String = ItemInfo.PriceFont;
	m_ExplainFont.String = ItemInfo.ExplainFont;
	m_TitleFont.String = ItemInfo.TitleFont;
	m_iExplainFontEnterCount = ItemInfo.iEnterCount;
}

void CUi_StoreDesc::Add_Color(ColorType Type, _float Color)
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
	m_TitleFont.Color.x += Color;
	m_ExplainFont.Color.x += Color;
	m_PriceFont.Color.x += Color;

	m_TitleFont.Color.y += Color;
	m_ExplainFont.Color.y += Color;
	m_PriceFont.Color.y += Color;

	m_TitleFont.Color.z += Color;
	m_ExplainFont.Color.z += Color;
	m_PriceFont.Color.z += Color;

	m_TitleFont.Color.w += Color;
	m_ExplainFont.Color.w += Color;
	m_PriceFont.Color.w += Color;

	m_pBuyBar->Add_Color(Type, Color);

	for (auto& iter : m_DefaultVec)
		iter->Add_Color(Type, Color);

	m_Itempng->Add_Color(Type, Color);
}

void CUi_StoreDesc::Set_Color(ColorType Type, _float Color)
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
	m_TitleFont.Color.x = Color;
	m_ExplainFont.Color.x = Color;
	m_PriceFont.Color.x = Color;

	m_TitleFont.Color.y = Color;
	m_ExplainFont.Color.y = Color;
	m_PriceFont.Color.y = Color;

	m_TitleFont.Color.z = Color;
	m_ExplainFont.Color.z = Color;
	m_PriceFont.Color.z = Color;

	m_TitleFont.Color.w = Color;
	m_ExplainFont.Color.w = Color;
	m_PriceFont.Color.w = Color;

	m_pBuyBar->Set_Color(Type, Color);

	for (auto& iter : m_DefaultVec)
		iter->Set_Color(Type, Color);

	m_Itempng->Set_Color(Type, Color);
}

void CUi_StoreDesc::Adjust_Alpha(_float fTimeDelta)
{
	m_fAlphaTime += fTimeDelta;
	if (m_fAlphaTime < 0.5f)
	{
		Add_Color(ColorType::A, fTimeDelta * 4);
		for (auto& iter : m_DefaultVec)
			iter->Add_Color(ColorType::A, fTimeDelta * 4);
		m_Itempng->Add_Color(ColorType::A, fTimeDelta * 4);
	}
}

void CUi_StoreDesc::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pBuyBar->Set_Active(Active);
	for (auto& iter : m_DefaultVec)
		iter->Set_Active(Active);

	m_Itempng->Set_Active(Active);

	if (m_bActive)
	{
		Set_Color(ColorType::A, 0);
		for (auto& iter : m_DefaultVec)
			iter->Set_Color(ColorType::A, 0);

		m_fAlphaTime = 0;
	}
}

void CUi_StoreDesc::Add_Pos(_float PosX, _float PosY)
{
	_float3 Pos = Get_Pos();
	Pos.x += PosX;
	Pos.y += PosY;
	Set_Pos(Pos.x, Pos.y, Pos.z);

	for (auto& iter : m_DefaultVec)
		iter->Add_Pos(PosX, PosY);

	m_pBuyBar->Add_Pos(PosX, PosY);
	m_PriceFont.DirectPos.x += PosX;
	m_PriceFont.DirectPos.y += PosY;
	m_ExplainFont.DirectPos.x += PosX;
	m_ExplainFont.DirectPos.y += PosY;
	m_TitleFont.DirectPos.y += PosY;
	m_TitleFont.DirectPos.x += PosX;
}

void CUi_StoreDesc::Set_BuyBarPercent(_float fPercent)
{
	if(m_pBuyBar)
		m_pBuyBar->Set_Percent(fPercent);
}

void CUi_StoreDesc::Set_BuyBarType(CUi_StoreBuyBar::Type Type)
{
	m_pBuyBar->Set_Type(Type);
}

void CUi_StoreDesc::Initialize_Font()
{
	m_TitleFont.DirectPos = { Get_Pos().x - Get_Scale().x * 0.5f + 40, Get_Pos().y + Get_Scale().y * 0.47f};
	m_TitleFont.Size = 0.7f;
	m_TitleFont.String = L"제목";

	m_PriceFont.DirectPos = { Get_Pos().x + 40, Get_Pos().y + Get_Scale().y * 0.35f };
	m_PriceFont.Size = 0.5f;
	m_PriceFont.String = L"가격";

	m_ExplainFont.DirectPos = { Get_Pos().x - Get_Scale().x * 0.5f + 35, Get_Pos().y - 20 };
	m_ExplainFont.Size = 0.35f;
	m_ExplainFont.String = L"설명";
}

void CUi_StoreDesc::Initialize_DefaultVec()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 40;
	Desc.fSizeY = 40;
	Desc.fX = 585 - m_PriceFont.String.size() * 15.f - 20;
	Desc.fY = 325;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 1;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_Gear";
	Desc.TextureLevel = LEVEL_STATIC;
	m_DefaultVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
}

void CUi_StoreDesc::Initialize_BuyBar()
{
	CUi_StoreBuyBar::Desc Desc;
	Desc.fSizeX = 230;
	Desc.fSizeY = 30;
	Desc.fX = Get_Pos().x + 100;
	Desc.fY = Get_Pos().y + Get_Scale().y * 0.3f - 30;
	Desc.fZ = Get_Pos().z - 0.2f;
	Desc.eRenderType = m_eRenderType;
	Desc.iPriority = m_iRenderPriority + 2;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::StoreBuyBar;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBuyBar";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBuyBar = static_pointer_cast<CUi_StoreBuyBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_StoreBuyBar::ObjID, m_iLevelIndex, L"CUi", CUi_StoreBuyBar::ObjID, &Desc));
}

void CUi_StoreDesc::Dying()
{
	for (auto& iter : m_DefaultVec)
		iter->Dead();
	m_Itempng->Dead();
}

void CUi_StoreDesc::Initialize_Png()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 100;
	Desc.fSizeY = 100;
	Desc.fX = Get_Pos().x - Get_Scale().x * 0.4f;
	Desc.fY = Get_Pos().y + Get_Scale().y * 0.3f;
	Desc.fZ = 0.5f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 10;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreItem";
	Desc.TextureLevel = LEVEL_STATIC;
	m_Itempng = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_StoreDesc::Sync_Png()
{
	if (m_TitleFont.String == L"라이프 게이지")
	{
		m_Itempng->Set_TextureNum(ItemOrder::LifeGauge);
	}
	else if (m_TitleFont.String == L"전기 리버브 코어")
	{
		m_Itempng->Set_TextureNum(ItemOrder::Reverb);
	}
	else if (m_TitleFont.String == L"체력 탱크")
	{
		m_Itempng->Set_TextureNum(ItemOrder::HpTank);
	}
	else
	{
		m_Itempng->Set_TextureNum(ItemOrder::None);
	}

}

shared_ptr<CUi_StoreDesc> CUi_StoreDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreDesc
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreDesc(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreDesc> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreDesc::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreDesc
	{
		MakeSharedEnabler(const CUi_StoreDesc& rhs) : CUi_StoreDesc(rhs) { }
	};

	shared_ptr<CUi_StoreDesc> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
