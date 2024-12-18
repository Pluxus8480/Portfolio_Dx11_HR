#include "CUi_FightString.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Mgr.h"


wstring CUi_FightString::ObjID = L"CUi_FightString";

CUi_FightString::CUi_FightString(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_FightString::CUi_FightString(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_FightString::~CUi_FightString()
{
}

HRESULT CUi_FightString::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FightString",
			L"../Bin/Resources/Textures/Ui/FightString/FightString.png"));

	auto GameInstance = m_pGameInstance.lock();
	GameInstance->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"FightString", L"../Bin/Resources/Sounds/Ui/FightString/FightString.ogg");

	return S_OK;
}

HRESULT CUi_FightString::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc Fix;
	Fix.eRenderType = RenderType::Priority;
	Fix.fColor = { 100, 100, 100, 1.f };
	Fix.fX = 0;
	Fix.fY = 0;
	Fix.fZ = 0.7f;
	Fix.fSizeX = 1.f;
	Fix.fSizeY = 1.f;
	Fix.fSizeZ = 1;
	Fix.iPriority = 10;
	Fix.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Fix.ShaderPass = ShaderPass::Blend;
	Fix.TextureFileName = L"Prototype_Component_Texture_FightString";
	Fix.TextureLevel = LEVEL_STATIC;

	if (FAILED(Ready_Components(&Fix)))
		assert(false);

	InitializeRenderType(Fix.eRenderType, Fix.iPriority);
	InitializeUi(Fix.fX, Fix.fY, Fix.fZ, Fix.fSizeX, Fix.fSizeY, Fix.fSizeZ, Fix.fSpeed, Fix.fColor);

	CUi_Mgr::Get_Instance()->Battle_Start();
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"FightString");

	return S_OK;
}

void CUi_FightString::PriorityTick(_float fTimeDelta)
{
}

void CUi_FightString::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime < 0.7f)
	{
		_float3 Pos = Get_Pos();
		Pos.z -= fTimeDelta;
		Set_Pos(0, 0, Pos.z);
		Add_Color(ColorType::A, -fTimeDelta * 0.5f);
	}

	if (m_fLifeTime <= 0)
	{
		Dead();
	}
}

void CUi_FightString::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_FightString::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
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

	return S_OK;
}

shared_ptr<CUi_FightString> CUi_FightString::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_FightString
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_FightString(pDevice, pContext) { }
	};

	shared_ptr<CUi_FightString> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_FightString::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_FightString
	{
		MakeSharedEnabler(const CUi_FightString& rhs) : CUi_FightString(rhs) { }
	};

	shared_ptr<CUi_FightString> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
