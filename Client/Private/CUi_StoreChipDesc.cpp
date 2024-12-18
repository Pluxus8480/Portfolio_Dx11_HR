#include "CUi_StoreChipDesc.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "PlayerManager.h"


wstring CUi_StoreChipDesc::ObjID = L"CUi_StoreChipDesc";

CUi_StoreChipDesc::CUi_StoreChipDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_StoreDesc(pDevice, pContext)
{
}

CUi_StoreChipDesc::CUi_StoreChipDesc(const CUi_StoreChipDesc& rhs)
	:CUi_StoreDesc(rhs)
{
}

CUi_StoreChipDesc::~CUi_StoreChipDesc()
{
}

HRESULT CUi_StoreChipDesc::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreChipDesc::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	Initialize_Font();
	Initialize_BuyBar();


	return S_OK;
}

void CUi_StoreChipDesc::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreChipDesc::Tick(_float fTimeDelta)
{
	Adjust_Alpha(fTimeDelta);
}

void CUi_StoreChipDesc::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_PriceFont.String = to_wstring(123);
		m_ChipPrice_Font.String = to_wstring(4321);
		
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_StoreChipDesc::Render()
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
	RenderFont(m_ChipPrice_Font);
	for (auto& iter : m_UpGrade_Font)
		RenderFont(iter);
	
	return S_OK;
}

void CUi_StoreChipDesc::Set_Desc(ChipDesc ChipInfo)
{
	m_PriceFont.String = ChipInfo.PriceFont;
	m_ExplainFont.String = ChipInfo.ExplainFont;
	m_TitleFont.String = ChipInfo.TitleFont;
	m_ChipPrice_Font.String = ChipInfo.ChipPrice;
	m_iExplainFontEnterCount = ChipInfo.iEnterCount;
	m_UpGradeGap = ChipInfo.UpGrade_Gap;
}

void CUi_StoreChipDesc::Initialize_Font()
{
	m_TitleFont.DirectPos = { Get_Pos().x - Get_Scale().x * 0.5f + 40, Get_Pos().y + Get_Scale().y * 0.47f };
	m_TitleFont.Size = 0.7f;
	m_TitleFont.String = L"제목";
	m_TitleFont.Color = { 1, 1, 1, 0 };

	m_PriceFont.DirectPos = { Get_Pos().x + 40, Get_Pos().y + Get_Scale().y * 0.35f };
	m_PriceFont.Size = 0.5f;
	m_PriceFont.String = to_wstring(4321);
	m_PriceFont.Color = { 1, 1, 1, 0 };

	m_ChipPrice_Font.DirectPos = { Get_Pos().x + 40, Get_Pos().y + Get_Scale().y * 0.35f - 20 };
	m_ChipPrice_Font.Size = 0.5f;
	m_ChipPrice_Font.String = to_wstring(123);
	m_ChipPrice_Font.Color = { 1, 1, 1, 0 };

	m_ExplainFont.DirectPos = { Get_Pos().x - Get_Scale().x * 0.5f + 35, Get_Pos().y - 20 };
	m_ExplainFont.Size = 0.35f;
	m_ExplainFont.String = L"설명";
	m_ExplainFont.Color = {1, 1, 1, 0};

	_float2 Pos = { Get_Pos().x - Get_Scale().x * 0.5f + 15, Get_Pos().y + 20 };
	_float Size = { 0.35f };
	FontDescInfo UpGradeFont;
	for (size_t i = 0; i < 3; ++i)
	{
		Pos.x += i * 50;
		UpGradeFont.Color = { 1, 1, 1, 0 };
		UpGradeFont.DirectPos = Pos;
		UpGradeFont.String = to_wstring(i+1);
		UpGradeFont.Size = Size;
		m_UpGrade_Font.emplace_back(UpGradeFont);
	}
}

void CUi_StoreChipDesc::Initialize_DefaultVec()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 40;
	Desc.fSizeY = 40;
	Desc.fX = 585 - m_PriceFont.String.size() * 15.f - 20;
	Desc.fY = 325;
	Desc.fZ = 0.5f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 10;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_Gear";
	Desc.TextureLevel = LEVEL_STATIC;
	m_DefaultVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fSizeX = 40;
	Desc.fSizeY = 40;
	Desc.fX = 585 - m_PriceFont.String.size() * 15.f - 20;
	Desc.fY = 300;
	Desc.fZ = 0.5f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 10;
	Desc.TextureFileName = L"Prototype_Component_Texture_StorePauseChip";
	m_DefaultVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
}

void CUi_StoreChipDesc::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		m_TitleFont.Color.x += Color;
		m_ExplainFont.Color.x += Color;
		m_PriceFont.Color.x += Color;
		m_ChipPrice_Font.Color.x += Color;
		for(auto& iter : m_UpGrade_Font)
			iter.Color.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		m_TitleFont.Color.y += Color;
		m_ExplainFont.Color.y += Color;
		m_PriceFont.Color.y += Color;
		m_ChipPrice_Font.Color.y += Color;
		for (auto& iter : m_UpGrade_Font)
			iter.Color.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		m_TitleFont.Color.z += Color;
		m_ExplainFont.Color.z += Color;
		m_PriceFont.Color.z += Color;
		m_ChipPrice_Font.Color.z += Color;
		for (auto& iter : m_UpGrade_Font)
			iter.Color.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		m_TitleFont.Color.w += Color;
		m_ExplainFont.Color.w += Color;
		m_PriceFont.Color.w += Color;
		m_ChipPrice_Font.Color.w += Color;
		for (auto& iter : m_UpGrade_Font)
			iter.Color.w += Color;
		break;
	default:
		assert(false);
		break;
	}
	m_pBuyBar->Add_Color(Type, Color);
}

shared_ptr<CUi_StoreChipDesc> CUi_StoreChipDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreChipDesc
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreChipDesc(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreChipDesc> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreChipDesc::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreChipDesc
	{
		MakeSharedEnabler(const CUi_StoreChipDesc& rhs) : CUi_StoreChipDesc(rhs) { }
	};

	shared_ptr<CUi_StoreChipDesc> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
