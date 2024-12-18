#include "CUi_PartIcon.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_PartIcon::ObjID = L"CUi_PartIcon";

CUi_PartIcon::CUi_PartIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Default(pDevice, pContext)
{
}

CUi_PartIcon::CUi_PartIcon(const CUi_PartIcon& rhs)
	:CUi_Default(rhs)
{
}

CUi_PartIcon::~CUi_PartIcon()
{
}

HRESULT CUi_PartIcon::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_CommandIcon",
			L"../Bin/Resources/Textures/Ui/CommandIcon/Icon%d.png", 3));

	return S_OK;
}

HRESULT CUi_PartIcon::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	m_bRedIcon = Arg->bRedIcon;

	return S_OK;
}

HRESULT CUi_PartIcon::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bCommandIconRed", &m_bRedIcon, sizeof(_bool))))
		assert(false);
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);


	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	return S_OK;
}

shared_ptr<CUi_PartIcon> CUi_PartIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PartIcon
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PartIcon(pDevice, pContext) { }
	};

	shared_ptr<CUi_PartIcon> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PartIcon::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PartIcon
	{
		MakeSharedEnabler(const CUi_PartIcon& rhs) : CUi_PartIcon(rhs) { }
	};

	shared_ptr<CUi_PartIcon> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
