#include "CUi_TramTimer.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_TramTimer::ObjID = L"CUi_TramTimer";

CUi_TramTimer::CUi_TramTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_TramTimer::CUi_TramTimer(const CUi_TramTimer& rhs)
	:CUi_2D(rhs)
{
}

CUi_TramTimer::~CUi_TramTimer()
{
}

HRESULT CUi_TramTimer::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TramTimer",
			L"../Bin/Resources/Textures/Ui/Tram/TramTimer.png"));


	return S_OK;
}

HRESULT CUi_TramTimer::Initialize(void* pArg)
{
	m_fTime = reinterpret_cast<Desc*>(pArg)->fTime;
	Initialize_Transform();
	Initialize_Timer();
	Initialize_Font();

	return S_OK;
}

void CUi_TramTimer::PriorityTick(_float fTimeDelta)
{
}

void CUi_TramTimer::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Cal_Time(fTimeDelta);
	}
}

void CUi_TramTimer::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_TramTimer::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", Get_TextureNum())))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	RenderFont(StringDesc);
	RenderFont(m_FontDesc);

	return S_OK;
}

void CUi_TramTimer::Add_Time(_float Time)
{
	m_fTime += Time;
	if (m_fTime > 99)
	{
		m_fTime = 99;
	}
}

void CUi_TramTimer::Dying()
{
}

void CUi_TramTimer::Cal_Time(_float fTimeDelta)
{
	m_fTime -= fTimeDelta;
	if (m_fTime < 0)
		m_fTime = 0;

	
	_int String = m_fTime * 100;

	m_FontDesc.String = to_wstring(String);

	
	if (m_FontDesc.String.size() > 2)
	{
		if (m_fTime < 100 && 10 <= m_fTime)
		{
			m_FontDesc.String.insert(2, L".");
		}
		else if (m_fTime < 10 && 1 <= m_fTime)
		{
			m_FontDesc.String.insert(1, L".");
		}
		else if (m_fTime < 1)
		{
			m_FontDesc.String.insert(0, L"0");
			m_FontDesc.String.insert(1, L".");
			m_FontDesc.String = L"0";
		}
	}
}

void CUi_TramTimer::Initialize_Timer()
{
	CUi_2D::Desc Desc;
	Desc.fSizeX = 300;
	Desc.fSizeY = 100;
	Desc.fX = 220;
	Desc.fY = 220;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 5;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_TramTimer";
	Desc.TextureLevel = LEVEL_STATIC;
	if (FAILED(Ready_Components(&Desc)))
		assert(false);
	InitializeRenderType(Desc.eRenderType, Desc.iPriority);
	InitializeUi(Desc.fX, Desc.fY, Desc.fZ, Desc.fSizeX, Desc.fSizeY, Desc.fSizeZ, Desc.fSpeed, Desc.fColor);
}

void CUi_TramTimer::Initialize_Font()
{
	_int String = m_fTime * 100;
	m_FontDesc.String = to_wstring(String);
	m_FontDesc.String.insert(2, L".");
	m_FontDesc.DirectPos = { Get_Pos().x - Get_Scale().x * 0.2f, Get_Pos().y + 15 };
	m_FontDesc.Size = { 0.9f };
	StringDesc.String = L"TIME UNTIL L.I.F.T STOPS";
	StringDesc.DirectPos = { Get_Pos().x - Get_Scale().x * 0.35f, Get_Pos().y + Get_Scale().y * 0.45f};
	StringDesc.Color = { 1, 1, 0, 1 };
	StringDesc.Size = { 0.4f };
}

shared_ptr<CUi_TramTimer> CUi_TramTimer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TramTimer
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TramTimer(pDevice, pContext) { }
	};

	shared_ptr<CUi_TramTimer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TramTimer::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TramTimer
	{
		MakeSharedEnabler(const CUi_TramTimer& rhs) : CUi_TramTimer(rhs) { }
	};

	shared_ptr<CUi_TramTimer> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
