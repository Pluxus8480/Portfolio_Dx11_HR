#include "CUi_StoreBuyBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"


wstring CUi_StoreBuyBar::ObjID = L"CUi_StoreBuyBar";

CUi_StoreBuyBar::CUi_StoreBuyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StoreBuyBar::CUi_StoreBuyBar(const CUi_StoreBuyBar& rhs)
	:CUi_2D(rhs)
{
}

CUi_StoreBuyBar::~CUi_StoreBuyBar()
{
}

HRESULT CUi_StoreBuyBar::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBuyBar",
			L"../Bin/Resources/Textures/Ui/Store/StoreBuyBar.png"));

	return S_OK;
}

HRESULT CUi_StoreBuyBar::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	Initialize_Font();
	Initialize_Bar();

	return S_OK;
}

void CUi_StoreBuyBar::PriorityTick(_float fTimeDelta)
{
	m_fPercent = 0;
}

void CUi_StoreBuyBar::Tick(_float fTimeDelta)
{
}

void CUi_StoreBuyBar::LateTick(_float fTimeDelta)
{
	if (m_bActive && m_eType != Type::Has)
	{
		m_pBack->Set_Pos(Get_Pos().x, Get_Pos().y, m_pBack->Get_Pos().z);
		Adjust_FontPos();
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}

}

HRESULT CUi_StoreBuyBar::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_StoreBuyBarPercent", m_fPercent)))
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

	RenderFont(m_FontDesc);

	return S_OK;
}

void CUi_StoreBuyBar::Set_Percent(_float Percent)
{
	m_fPercent = Percent * 0.01f;
}

void CUi_StoreBuyBar::Dying()
{
	m_pBack->Dead();
}

void CUi_StoreBuyBar::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		m_FontDesc.Color.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		m_FontDesc.Color.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		m_FontDesc.Color.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		m_FontDesc.Color.w += Color;
		break;
	default:
		assert(false);
		break;
	}
	m_pBack->Add_Color(Type, Color);
}

void CUi_StoreBuyBar::Set_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x = Color;
		m_FontDesc.Color.x = Color;
		break;
	case ColorType::G:
		m_fColor.y = Color;
		m_FontDesc.Color.y = Color;
		break;
	case ColorType::B:
		m_fColor.z = Color;
		m_FontDesc.Color.z = Color;
		break;
	case ColorType::A:
		m_fColor.w = Color;
		m_FontDesc.Color.w = Color;
		break;
	default:
		assert(false);
		break;
	}
	m_pBack->Set_Color(Type, Color);
}

void CUi_StoreBuyBar::Set_Type(Type eType)
{
	m_eType = eType;
	switch (m_eType)
	{
	case Type::NeedBuy:
		m_FontDesc.String = L"구입(길게 누르기)";
		m_FontDesc.DirectPos = { Get_Pos().x - Get_Scale().x * 0.35f, Get_Pos().y + 10 };
		m_pBack->Set_Active(true);
		break;
	case Type::NeedUpGrade:
		m_FontDesc.String = L"업그레이드(길게 누르기)";
		m_FontDesc.DirectPos = { Get_Pos().x - Get_Scale().x * 0.45f, Get_Pos().y + 10 };
		m_pBack->Set_Active(true);
		break;
	case Type::Has:
		m_FontDesc.String = L"";
		m_pBack->Set_Active(false);
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_StoreBuyBar::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pBack->Set_Active(Active);
}

void CUi_StoreBuyBar::Add_Pos(_float PosX, _float PosY)
{
	_float3 Pos = Get_Pos();
	Pos.x += PosX;
	Pos.y += PosY;
	Set_Pos(Pos.x, Pos.y, Pos.z);
	m_pBack->Add_Pos(PosX, PosY);
	m_FontDesc.DirectPos.x += PosX;
	m_FontDesc.DirectPos.y += PosY;
}

void CUi_StoreBuyBar::Initialize_Font()
{
	m_FontDesc.DirectPos = { Get_Pos().x, Get_Pos().y + 10 };
	m_FontDesc.Size = 0.4f;
	m_FontDesc.String = L"구입(길게 누르기)";
}

void CUi_StoreBuyBar::Initialize_Bar()
{
	CUi_Default::Desc Desc;
	_float3 Scale = Get_Scale();
	_float3 Pos = Get_Pos();
	Desc.fSizeX = Scale.x;
	Desc.fSizeY = Scale.y;
	Desc.fX = Pos.x;
	Desc.fY = Pos.y;
	Desc.fZ = Pos.z + 0.1f;
	Desc.eRenderType = m_eRenderType;
	Desc.iPriority = m_iRenderPriority - 1;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBuyBarBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_StoreBuyBar::Adjust_FontPos()
{
	switch (m_eType)
	{
	case Type::NeedBuy:
		m_FontDesc.DirectPos = { Get_Pos().x - Get_Scale().x * 0.25f, Get_Pos().y + 10};
		break;
	case Type::NeedUpGrade:
		m_FontDesc.DirectPos = { Get_Pos().x - Get_Scale().x * 0.3f, Get_Pos().y + 10 };
		break;
	default:
		break;
	}
}

shared_ptr<CUi_StoreBuyBar> CUi_StoreBuyBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreBuyBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreBuyBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreBuyBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreBuyBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreBuyBar
	{
		MakeSharedEnabler(const CUi_StoreBuyBar& rhs) : CUi_StoreBuyBar(rhs) { }
	};

	shared_ptr<CUi_StoreBuyBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
