#include "CUi_RekkaDecoEffect.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_RekkaDecoEffect::ObjID = L"CUi_RekkaDecoEffect";

CUi_RekkaDecoEffect::CUi_RekkaDecoEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_RekkaDecoEffect::CUi_RekkaDecoEffect(const CUi_RekkaDecoEffect& rhs)
	:CUi_2D(rhs)
{
}

CUi_RekkaDecoEffect::~CUi_RekkaDecoEffect()
{
}

HRESULT CUi_RekkaDecoEffect::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_RekkaBackGroundEffect",
			L"../Bin/Resources/Textures/Ui/Boss/Rekka/RekkaBackGroundEffect.png"));

	return S_OK;
}

HRESULT CUi_RekkaDecoEffect::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_RekkaDecoEffect::PriorityTick(_float fTimeDelta)
{
}

void CUi_RekkaDecoEffect::Tick(_float fTimeDelta)
{
}

void CUi_RekkaDecoEffect::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_RekkaDecoEffect::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_RekkaEffectStartPos", m_fStartPos)))
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

shared_ptr<CUi_RekkaDecoEffect> CUi_RekkaDecoEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_RekkaDecoEffect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_RekkaDecoEffect(pDevice, pContext) { }
	};

	shared_ptr<CUi_RekkaDecoEffect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_RekkaDecoEffect::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_RekkaDecoEffect
	{
		MakeSharedEnabler(const CUi_RekkaDecoEffect& rhs) : CUi_RekkaDecoEffect(rhs) { }
	};

	shared_ptr<CUi_RekkaDecoEffect> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
