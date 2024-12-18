#include "CUi_RekkaHp.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "iostream"


wstring CUi_RekkaHp::ObjID = L"CUi_RekkaHp";

CUi_RekkaHp::CUi_RekkaHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_RekkaHp::CUi_RekkaHp(const CUi_RekkaHp& rhs)
	:CUi_2D(rhs)
{
}

CUi_RekkaHp::~CUi_RekkaHp()
{
}

HRESULT CUi_RekkaHp::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_RekkaHp",
			L"../Bin/Resources/Textures/Ui/Boss/Rekka/RekkaHp%d.png", 8));

	return S_OK;
}

HRESULT CUi_RekkaHp::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_RekkaHp::PriorityTick(_float fTimeDelta)
{
}

void CUi_RekkaHp::Tick(_float fTimeDelta)
{
}

void CUi_RekkaHp::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_RekkaHp::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", m_fDeletePercent)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_BossDamaged", &m_bDamaged, sizeof(_bool))))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BossBlurTexture", m_iTextureNum + 1)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_BlurStrong", 0.2f)))
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

shared_ptr<CUi_RekkaHp> CUi_RekkaHp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_RekkaHp
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_RekkaHp(pDevice, pContext) { }
	};

	shared_ptr<CUi_RekkaHp> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_RekkaHp::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_RekkaHp
	{
		MakeSharedEnabler(const CUi_RekkaHp& rhs) : CUi_RekkaHp(rhs) { }
	};

	shared_ptr<CUi_RekkaHp> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
