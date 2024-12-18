#include "CUi_DefaultBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_DefaultBar::ObjID = L"CUi_DefaultBar";

CUi_DefaultBar::CUi_DefaultBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Default(pDevice, pContext)
{
}

CUi_DefaultBar::CUi_DefaultBar(const CUi_DefaultBar& rhs)
	:CUi_Default(rhs)
{
}

CUi_DefaultBar::~CUi_DefaultBar()
{
}

HRESULT CUi_DefaultBar::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaMusicChaiHp",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaMusicChaiHp.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaMusicMimosaHp",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaMusicMimosaHp.png"));

	return S_OK;
}

HRESULT CUi_DefaultBar::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_DefaultBar::PriorityTick(_float fTimeDelta)
{
}

void CUi_DefaultBar::Tick(_float fTimeDelta)
{
}

void CUi_DefaultBar::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_DefaultBar::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", m_fPercent)))
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

void CUi_DefaultBar::Set_Percent(_float Percent)
{
	m_fPercent = Percent;
}

shared_ptr<CUi_DefaultBar> CUi_DefaultBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_DefaultBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_DefaultBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_DefaultBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_DefaultBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_DefaultBar
	{
		MakeSharedEnabler(const CUi_DefaultBar& rhs) : CUi_DefaultBar(rhs) { }
	};

	shared_ptr<CUi_DefaultBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
