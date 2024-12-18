#include "CUi_StoreEdge.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"


wstring CUi_StoreEdge::ObjID = L"CUi_StoreEdge";

CUi_StoreEdge::CUi_StoreEdge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StoreEdge::CUi_StoreEdge(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StoreEdge::~CUi_StoreEdge()
{
}

HRESULT CUi_StoreEdge::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreEdge",
			L"../Bin/Resources/Textures/Ui/Store/StoreEdge.png"));

	return S_OK;
}

HRESULT CUi_StoreEdge::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	Initialize_Part();

	return S_OK;
}

void CUi_StoreEdge::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreEdge::Tick(_float fTimeDelta)
{
}

void CUi_StoreEdge::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

void CUi_StoreEdge::Add_Color(ColorType Type, _float Color)
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

	for (auto& iter : m_pPartVec)
		iter->Add_Color(Type, Color);
}

void CUi_StoreEdge::Set_Color(ColorType Type, _float Color)
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

	for (auto& iter : m_pPartVec)
		iter->Set_Color(Type, Color);
}

void CUi_StoreEdge::Set_Active(_bool Active)
{
	m_bActive = Active;
	for (auto& iter : m_pPartVec)
		iter->Set_Active(Active);
}

void CUi_StoreEdge::Dying()
{
	for (auto& iter : m_pPartVec)
		iter->Dead();
}

void CUi_StoreEdge::Add_Pos(_float PosX, _float PosY)
{
	_float3 Pos = Get_Pos();
	Pos.x += PosX;
	Pos.y += PosY;
	Set_Pos(Pos.x, Pos.y, Pos.z);
	for (auto& iter : m_pPartVec)
		iter->Add_Pos(PosX, PosY);
}

void CUi_StoreEdge::Initialize_Part()
{
	CUi_Default::Desc Desc;
	Desc.fColor = { 1, 1, 1, 1 };
	Desc.fSizeX = 15;
	Desc.fSizeY = 15;
	Desc.fX = Get_Pos().x - Get_Scale().x * 0.5f - 20; 
	Desc.fY = Get_Pos().y;
	Desc.fZ = 0.9f;
	Desc.eRenderType = m_eRenderType;
	Desc.iPriority = m_iRenderPriority + 1;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreEdgePart";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pPartVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fX = Get_Pos().x + Get_Scale().x * 0.5f + 20;
	m_pPartVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	m_pPartVec[1]->Set_TextureNum(1);
}

shared_ptr<CUi_StoreEdge> CUi_StoreEdge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreEdge
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreEdge(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreEdge> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreEdge::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreEdge
	{
		MakeSharedEnabler(const CUi_StoreEdge& rhs) : CUi_StoreEdge(rhs) { }
	};

	shared_ptr<CUi_StoreEdge> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
