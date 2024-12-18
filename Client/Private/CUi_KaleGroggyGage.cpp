#include "CUi_KaleGroggyGage.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_KaleGroggyGage::ObjID = L"CUi_KaleGroggyGage";

CUi_KaleGroggyGage::CUi_KaleGroggyGage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_KaleGroggyGage::CUi_KaleGroggyGage(const CUi_KaleGroggyGage& rhs)
	:CUi_2D(rhs)
{
}

CUi_KaleGroggyGage::~CUi_KaleGroggyGage()
{
}

HRESULT CUi_KaleGroggyGage::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KaleGroggy",
			L"../Bin/Resources/Textures/Ui/Boss/Kale/KaleStunGauge%d.png", 2));

	return S_OK;
}

HRESULT CUi_KaleGroggyGage::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_KaleGroggyGage::PriorityTick(_float fTimeDelta)
{
}

void CUi_KaleGroggyGage::Tick(_float fTimeDelta)
{
}

void CUi_KaleGroggyGage::LateTick(_float fTimeDelta)
{
	if (m_bActive && m_bOwnActive)
	{
		if (m_fPercent == 0)
			Set_TextureNum(1);
		else
			Set_TextureNum(0);

		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_KaleGroggyGage::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);
	if (m_fPercent == 0)
	{
		if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", 0)))
			assert(false);
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", 1 - m_fPercent)))
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

shared_ptr<CUi_KaleGroggyGage> CUi_KaleGroggyGage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_KaleGroggyGage
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_KaleGroggyGage(pDevice, pContext) { }
	};

	shared_ptr<CUi_KaleGroggyGage> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_KaleGroggyGage::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_KaleGroggyGage
	{
		MakeSharedEnabler(const CUi_KaleGroggyGage& rhs) : CUi_KaleGroggyGage(rhs) { }
	};

	shared_ptr<CUi_KaleGroggyGage> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
