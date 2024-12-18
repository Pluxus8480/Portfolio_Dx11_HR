#include "CUi_BeatMeter.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "iostream"


wstring CUi_BeatMeter::ObjID = L"CUi_BeatMeter";

CUi_BeatMeter::CUi_BeatMeter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_BeatMeter::CUi_BeatMeter(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_BeatMeter::~CUi_BeatMeter()
{
}

HRESULT CUi_BeatMeter::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Beatmeter",
			L"../Bin/Resources/Textures/Ui/BeatMeter/Beatmeter%d.png", 5));

	return S_OK;
}

HRESULT CUi_BeatMeter::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_BeatMeter::PriorityTick(_float fTimeDelta)
{
}

void CUi_BeatMeter::Tick(_float fTimeDelta)
{
}

void CUi_BeatMeter::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_BeatMeter::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", m_fPercent * 0.01f * XM_2PI)))
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

	return S_OK;
}

shared_ptr<CUi_BeatMeter> CUi_BeatMeter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_BeatMeter
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_BeatMeter(pDevice, pContext) { }
	};

	shared_ptr<CUi_BeatMeter> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_BeatMeter::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_BeatMeter
	{
		MakeSharedEnabler(const CUi_BeatMeter& rhs) : CUi_BeatMeter(rhs) { }
	};

	shared_ptr<CUi_BeatMeter> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
