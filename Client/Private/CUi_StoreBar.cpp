#include "CUi_StoreBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "iostream"

wstring CUi_StoreBar::ObjID = L"CUi_StoreBar";

CUi_StoreBar::CUi_StoreBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StoreBar::CUi_StoreBar(const CUi_StoreBar& rhs)
	:CUi_2D(rhs)
{
}

CUi_StoreBar::~CUi_StoreBar()
{
}

HRESULT CUi_StoreBar::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBar",
			L"../Bin/Resources/Textures/Ui/Store/Store_Bar%d.png", 2));


	return S_OK;
}

HRESULT CUi_StoreBar::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	m_fOriginPos = { Get_Pos().x, Get_Pos().y };

	Set_StringFont(Arg->StringFont);
	Set_PriceFont(Arg->Price);
	m_iPrice = Arg->Price;
	m_eThingType = Arg->type;
	m_fUpLimitPos = Arg->fUpLimitPos;
	m_fDownLimitPos = Arg->fDownLimitPos;

	Initialize_DefaultVec();

	return S_OK;
}

void CUi_StoreBar::PriorityTick(_float fTimeDelta)
{
	if (m_bActive)
	{

	}
}

void CUi_StoreBar::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Check_Scroll();
		Adjust_Pos(fTimeDelta);
	}
}

void CUi_StoreBar::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		if (m_fUpLimitPercent != 1 || m_fDownLimitPercent != 0)
		{
			for (auto& iter : m_pDefaultVec)
				iter->Set_Active(false);
		}
		else
		{
			for (auto& iter : m_pDefaultVec)
				iter->Set_Active(true);
		}
		_float PosX = Get_Pos().x;
		_float PosY = Get_Pos().y;
		m_StringFont.DirectPos.x = PosX - m_fStringFont_Distance.x;
		m_StringFont.DirectPos.y = PosY - m_fStringFont_Distance.y;
		m_FontDesc.DirectPos.x = PosX - m_fPriceFont_Distance.x;
		m_FontDesc.DirectPos.y = PosY - m_fPriceFont_Distance.y;
		m_pDefaultVec[0]->Set_Pos(m_FontDesc.DirectPos.x - 25, m_FontDesc.DirectPos.y - 12);
		m_pDefaultVec[1]->Set_Active(false);

		if (PLAYERMANAGER->Is_HasThing(m_eThingType, Get_Name()))
		{
   			m_FontDesc.String = L"";
   			m_pDefaultVec[1]->Set_Active(true);
			m_pDefaultVec[1]->Set_Pos(Get_Pos().x + Get_Scale().x * 0.4f, Get_Pos().y, 0.5f);
		}
		if (m_bFocus)
			m_iTextureNum = 1;
		else
			m_iTextureNum = 0;
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_StoreBar::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_StoreBarScrollUpPercent", m_fUpLimitPercent)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_StoreBarScrollDownPercent", m_fDownLimitPercent)))
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

	if (m_fUpLimitPercent == 0 && m_fDownLimitPercent == 1)
	{
		if (m_fColor.w >= 0.1f)
		{
			RenderFont(m_FontDesc);
			RenderFont(m_StringFont);
		}
	}

	return S_OK;
}

void CUi_StoreBar::Set_Active(_bool Active)
{
	m_bActive = Active; 

	for (auto& iter : m_pDefaultVec)
		iter->Set_Active(Active);

	if (m_bActive)
	{
		Set_Color(ColorType::A, 0);
		m_fAlphaTime = 0;
	}
}

void CUi_StoreBar::Dying()
{
	for (auto& iter : m_pDefaultVec)
		iter->Dead();
}

void CUi_StoreBar::Set_StringFont(wstring str)
{
	m_fStringFont_Distance.x = Get_Scale().x * 0.5f - 20;
	m_fStringFont_Distance.y = -Get_Scale().y * 0.3f;
	m_StringFont.DirectPos = { Get_Pos().x - m_fStringFont_Distance.x, 
		Get_Pos().y - m_fStringFont_Distance.y };
	m_StringFont.String = str;
	m_FontDesc.Size = 0.45f;
}

void CUi_StoreBar::Set_PriceFont(_uint Price)
{
	_uint Digit = Num_Quotation(m_FontDesc, Price);
	m_fPriceFont_Distance.x = -Get_Scale().x * 0.5f + 50 + Digit * 10.f;
	m_fPriceFont_Distance.y = -Get_Scale().y * 0.25f;
	m_FontDesc.DirectPos = { Get_Pos().x - m_fPriceFont_Distance.x,
		Get_Pos().y - m_fPriceFont_Distance.y };
	m_FontDesc.Size = 0.5f;	
}

void CUi_StoreBar::Adjust_Alpha(_float fTimeDelta)
{
	m_fAlphaTime += fTimeDelta;
	if(m_fAlphaTime < 0.5f)
	{
		Add_Color(ColorType::A, fTimeDelta * 4);
	}
}

void CUi_StoreBar::Add_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x += Color;
		m_StringFont.Color.x += Color;
		m_FontDesc.Color.x += Color;
		break;
	case ColorType::G:
		m_fColor.y += Color;
		m_StringFont.Color.y += Color;
		m_FontDesc.Color.y += Color;
		break;
	case ColorType::B:
		m_fColor.z += Color;
		m_StringFont.Color.z += Color;
		m_FontDesc.Color.z += Color;
		break;
	case ColorType::A:
		m_fColor.w += Color;
		m_StringFont.Color.w += Color;
		m_FontDesc.Color.w += Color;
		break;
	default:
		assert(false);
		break;
	}

	for (auto& iter : m_pDefaultVec)
		iter->Add_Color(Type, Color);
}

void CUi_StoreBar::Set_Color(ColorType Type, _float Color)
{
	switch (Type)
	{
	case ColorType::R:
		m_fColor.x = Color;
		m_StringFont.Color.x = Color;
		m_FontDesc.Color.x = Color;
		break;
	case ColorType::G:
		m_fColor.y = Color;
		m_StringFont.Color.y = Color;
		m_FontDesc.Color.y = Color;
		break;
	case ColorType::B:
		m_fColor.z = Color;
		m_StringFont.Color.z = Color;
		m_FontDesc.Color.z = Color;
		break;
	case ColorType::A:
		m_fColor.w = Color;
		m_StringFont.Color.w = Color;
		m_FontDesc.Color.w = Color;
		break;
	default:
		assert(false);
		break;
	}

	for(auto& iter : m_pDefaultVec)
		iter->Set_Color(Type, Color);
}

void CUi_StoreBar::Add_Pos(_float PosX, _float PosY)
{
	_float3 Pos = Get_Pos();
	Pos.x += PosX;
	Pos.y += PosY;
	m_fOriginPos.x += PosX;
	m_fOriginPos.y += PosY;
	Set_Pos(Pos.x, Pos.y, Pos.z);
	for (auto& iter : m_pDefaultVec)
		iter->Add_Pos(PosX, PosY);

	m_FontDesc.DirectPos.x += PosX;
	m_FontDesc.DirectPos.y += PosY;

	m_StringFont.DirectPos.x += PosX;
	m_StringFont.DirectPos.y += PosY;
}

void CUi_StoreBar::Initialize_DefaultVec()
{
	CUi_Default::Desc Desc;
	Desc.fColor = { 1, 1, 1, 0.f };
	Desc.fSizeX = 40;
	Desc.fSizeY = 40;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 10;
	Desc.fSpeed = m_fSpeed;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_Gear";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pDefaultVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fSizeX = 40;
	Desc.fSizeY = Get_Scale().y * 0.7f;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.4f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.fSpeed = m_fSpeed;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreHasSkill";
	m_pDefaultVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	m_pDefaultVec[1]->Set_Active(false);
}

void CUi_StoreBar::Adjust_Pos(_float fTimeDelta)
{
	if (m_bFocus)
	{
		if (Get_Pos().x < m_fOriginPos.x + 15)
		{
			Move(MoveDir::Right, fTimeDelta);
		}
		else
		{
			Set_Pos(m_fOriginPos.x + 15, Get_Pos().y);
		}
	}
	else
	{
		if (Get_Pos().x > m_fOriginPos.x)
		{
			Move(MoveDir::Left, fTimeDelta);
		}
		else
		{
			Set_Pos(m_fOriginPos.x, Get_Pos().y);
		}
	}
}

void CUi_StoreBar::Check_Scroll()
{
	if (Get_Pos().y + Get_Scale().y * 0.5f > m_fUpLimitPos)
	{
		_float Pass = Get_Pos().y + Get_Scale().y * 0.5f - m_fUpLimitPos;
		m_fUpLimitPercent = Pass / Get_Scale().y;
	}
	else
		m_fUpLimitPercent = 0;

	if (Get_Pos().y - Get_Scale().y * 0.5f < m_fDownLimitPos)
	{
		_float Pass = m_fDownLimitPos - (Get_Pos().y - Get_Scale().y * 0.5f);
		m_fDownLimitPercent = 1 - Pass / Get_Scale().y;
	}
	else
		m_fDownLimitPercent = 1;
}

shared_ptr<CUi_StoreBar> CUi_StoreBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreBar
	{
		MakeSharedEnabler(const CUi_StoreBar& rhs) : CUi_StoreBar(rhs) { }
	};

	shared_ptr<CUi_StoreBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
