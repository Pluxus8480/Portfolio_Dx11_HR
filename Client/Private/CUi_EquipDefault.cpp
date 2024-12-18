#include "CUi_EquipDefault.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "iostream"


wstring CUi_EquipDefault::ObjID = L"CUi_EquipDefault";

CUi_EquipDefault::CUi_EquipDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Default(pDevice, pContext)
{
}

CUi_EquipDefault::CUi_EquipDefault(const CUi_EquipDefault& rhs)
	:CUi_Default(rhs)
{
}

CUi_EquipDefault::~CUi_EquipDefault()
{
}

HRESULT CUi_EquipDefault::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SkillEquip",
			L"../Bin/Resources/Textures/Ui/Store/SkillEquip/SkillEquip%d.png", 2));

	return S_OK;
}

HRESULT CUi_EquipDefault::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	Initialize_GaugeVec(Arg->Big);
	Initialize_Font(Arg->Big);
	m_OriginPos = { Get_Pos().x, Get_Pos().y };
	m_bBig = Arg->Big;
	
	return S_OK;
}

void CUi_EquipDefault::PriorityTick(_float fTimeDelta)
{
}

void CUi_EquipDefault::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		_float3 Pos = Get_Pos();
		_float3 Scale = Get_Scale();
		_float3 VecScale = m_GaugeVec[0]->Get_Scale();
		_float3 VecPos = m_GaugeVec[0]->Get_Pos();
		if (m_bBig)
		{
			for (size_t i = 0; i < m_GaugeVec.size(); ++i)
				m_GaugeVec[i]->Set_Pos((Pos.x) + (VecScale.x * i) + 65, Pos.y - Scale.y * 0.2f);

		}
		else
		{
			for (size_t i = 0; i < m_GaugeVec.size(); ++i)
				m_GaugeVec[i]->Set_Pos((Pos.x) + (VecScale.x * i) + 45, Pos.y - Scale.y * 0.2f);
		}

		for (auto& iter : m_GaugeVec)
			iter->Set_OwnActive(false);

		for (size_t i = 0; i < m_iGaugeCount; ++i)
		{
			m_GaugeVec[i]->Set_OwnActive(true);
		}
	}
}

void CUi_EquipDefault::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		_float3 Pos = Get_Pos();
		_float3 Scale = Get_Scale();
		if (m_bBig)
		{
			m_FontDesc.DirectPos = { Pos.x - Scale.x * 0.3f, Pos.y + Scale.y * 0.3f };
			m_FontString.DirectPos = { Pos.x - Scale.x * 0.35f, Pos.y - Scale.y * 0.15f };
		}
		else
		{
			m_FontDesc.DirectPos = { Pos.x - Scale.x * 0.3f, Pos.y + Scale.y * 0.27f };
			m_FontString.DirectPos = { Pos.x - Scale.x * 0.25f, Pos.y - Scale.y * 0.15f };
		}
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_EquipDefault::Render()
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

	RenderFont(m_FontDesc);
	RenderFont(m_FontString);

	return S_OK;
}

void CUi_EquipDefault::Initialize_GaugeVec(_bool Big)
{
	m_GaugeVec.reserve(4);
	CUi_Default::Desc Desc;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.fZ = 0.5f;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_EquipReverbe";
	Desc.TextureLevel = LEVEL_STATIC;
	if (Big)
	{
		Desc.fSizeX = 50;
		Desc.fSizeY = 20;
		for (_uint i = 0; i < 4; ++i)
		{
			m_GaugeVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
				->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
		}
	}
	else
	{
		Desc.fSizeX = 35;
		Desc.fSizeY = 14;
		for (_uint i = 0; i < 4; ++i)
		{
			m_GaugeVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
				->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
		}
	}
}

void CUi_EquipDefault::Initialize_Font(_bool Big)
{
	if (Big)
	{
		m_FontDesc.String = L"파워 코드";
		m_FontDesc.Size = 0.7f;
		m_FontString.String = L"필요 리버브 게이지";
		m_FontString.Size = 0.5f;
	}
	else
	{
		m_FontDesc.String = L"파워 코드";
		m_FontDesc.Size = 0.5f;
		m_FontString.String = L"필요 리버브 게이지";
		m_FontString.Size = 0.3f;
	}
}

void CUi_EquipDefault::Add_Color(ColorType Type, _float Color)
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
	for (auto& iter : m_GaugeVec)
		iter->Add_Color(Type, Color);

	m_FontDesc.Color.x += Color;
	m_FontDesc.Color.y += Color;
	m_FontDesc.Color.z += Color;
	m_FontDesc.Color.w += Color;

	m_FontString.Color.x += Color;
	m_FontString.Color.y += Color;
	m_FontString.Color.z += Color;
	m_FontString.Color.w += Color;
}

void CUi_EquipDefault::Set_Color(ColorType Type, _float Color)
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

	for (auto& iter : m_GaugeVec)
		iter->Set_Color(Type, Color);

	m_FontDesc.Color.x = Color;
	m_FontDesc.Color.y = Color;
	m_FontDesc.Color.z = Color;
	m_FontDesc.Color.w = Color;

	m_FontString.Color.x = Color;
	m_FontString.Color.y = Color;
	m_FontString.Color.z = Color;
	m_FontString.Color.w = Color;
}

void CUi_EquipDefault::Set_Color(_float4 Color)
{
	m_fColor = Color;
}

void CUi_EquipDefault::Add_Pos(_float PosX, _float PosY)
{
	_float3 Pos = Get_Pos();
	Pos.x += PosX;
	Pos.y += PosY;
	Set_Pos(Pos.x, Pos.y, Pos.z);
}

void CUi_EquipDefault::Set_Active(_bool Active)
{
	m_bActive = Active;
	for (auto& iter : m_GaugeVec)
		iter->Set_Active(Active);
}

void CUi_EquipDefault::Set_Skill(CChai::SkillDesc Skill)
{
	m_FontDesc.String = Skill.SkillName;
	if (Skill.ConsumeReverb > 0)
	{
		m_iGaugeCount = Skill.ConsumeReverb;
	}
	else
	{
		cout << "GaugeError" << endl;
		m_iGaugeCount = 0;
	}
	if (m_iGaugeCount >= 5)
	{
		assert(false);
	}
}

wstring CUi_EquipDefault::Get_SkillName() const
{
	return 	m_FontDesc.String;
}

shared_ptr<CUi_EquipDefault> CUi_EquipDefault::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_EquipDefault
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_EquipDefault(pDevice, pContext) { }
	};

	shared_ptr<CUi_EquipDefault> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_EquipDefault::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_EquipDefault
	{
		MakeSharedEnabler(const CUi_EquipDefault& rhs) : CUi_EquipDefault(rhs) { }
	};

	shared_ptr<CUi_EquipDefault> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
