#include "CUi_MimosaBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_MimosaBar::ObjID = L"CUi_MimosaBar";

CUi_MimosaBar::CUi_MimosaBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_MimosaBar::CUi_MimosaBar(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_MimosaBar::~CUi_MimosaBar()
{
}

HRESULT CUi_MimosaBar::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaHealthBar",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaHealthBar%d.png", 6));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaGroggyGauge",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaGroggyGauge.png"));

	return S_OK;
}

HRESULT CUi_MimosaBar::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_MimosaBar::PriorityTick(_float fTimeDelta)
{
}

void CUi_MimosaBar::Tick(_float fTimeDelta)
{
}

void CUi_MimosaBar::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_MimosaBar::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", m_fPercent)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_BossDamaged", &m_bDmged, sizeof(_bool))))
		assert(false);

	if (m_bDmged)
	{
		_float4 Color = { 0.f, 0.f, 0.f, 1.f };
		if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &Color)))
			assert(false);

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BossBlurTexture", m_iTextureNum + 3)))
			assert(false);

		if (FAILED(m_pShaderCom->Bind_Float("g_BlurStrong", 1)))
			assert(false);
	}

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

shared_ptr<CUi_MimosaBar> CUi_MimosaBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_MimosaBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_MimosaBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_MimosaBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_MimosaBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_MimosaBar
	{
		MakeSharedEnabler(const CUi_MimosaBar& rhs) : CUi_MimosaBar(rhs) { }
	};

	shared_ptr<CUi_MimosaBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
