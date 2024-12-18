#include "CUi_TreasureBox.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "iostream"


wstring CUi_TreasureBox::ObjID = L"CUi_TreasureBox";

CUi_TreasureBox::CUi_TreasureBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_TreasureBox::CUi_TreasureBox(const CUi_TreasureBox& rhs)
	:CUi_2D(rhs)
{
}

CUi_TreasureBox::~CUi_TreasureBox()
{
}

HRESULT CUi_TreasureBox::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TreasureBox",
			L"../Bin/Resources/Textures/Ui/MapInterAction/TreasureBox/TreasureBox%d.png", 4));

	return S_OK;
}

HRESULT CUi_TreasureBox::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);
	
	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	
	return S_OK;
}

void CUi_TreasureBox::PriorityTick(_float fTimeDelta)
{
}

void CUi_TreasureBox::Tick(_float fTimeDelta)
{

}

void CUi_TreasureBox::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

HRESULT CUi_TreasureBox::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_TreasurePercent", m_fPercent * 0.01f)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TreasureGlow", &m_bGlow, sizeof(_bool))))
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


shared_ptr<CUi_TreasureBox> CUi_TreasureBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TreasureBox
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TreasureBox(pDevice, pContext) { }
	};

	shared_ptr<CUi_TreasureBox> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TreasureBox::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TreasureBox
	{
		MakeSharedEnabler(const CUi_TreasureBox& rhs) : CUi_TreasureBox(rhs) { }
	};

	shared_ptr<CUi_TreasureBox> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
