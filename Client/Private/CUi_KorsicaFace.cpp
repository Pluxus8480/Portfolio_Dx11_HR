#include "CUi_KorsicaFace.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"


wstring CUi_KorsicaFace::ObjID = L"CUi_KorsicaFace";

CUi_KorsicaFace::CUi_KorsicaFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_KorsicaFace::CUi_KorsicaFace(const CUi_KorsicaFace& rhs)
	:CUi_2D(rhs)
{
}

CUi_KorsicaFace::~CUi_KorsicaFace()
{
}

HRESULT CUi_KorsicaFace::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ChaiFace",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/ChaiFace.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KorsicaFace",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/KorsicaFace.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FaceBg",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/FaceBg%d.png", 2));
	
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);


	return S_OK;
}

HRESULT CUi_KorsicaFace::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);
	Initialize_BackTexture();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	Initialize_FaceBack();

	m_pBack->Set_TextureNum(Arg->BackTextureNum);
	m_pBackTextureNum = Arg->BackTextureNum;

	return S_OK;
}

void CUi_KorsicaFace::PriorityTick(_float fTimeDelta)
{
}

void CUi_KorsicaFace::Tick(_float fTimeDelta)
{
}

void CUi_KorsicaFace::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_KorsicaFace::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pBackTextureCom->Bind_ShaderResource(m_pShaderCom, "g_FaceBackTexture", m_pBackTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	return S_OK;
}

void CUi_KorsicaFace::Initialize_BackTexture()
{
	m_pBackTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_FaceBg",
		L"Com_SecondTexture"));

	if (nullptr == m_pBackTextureCom)
		assert(false);
}

void CUi_KorsicaFace::Initialize_FaceBack()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = Get_Scale().x;
	Desc.fSizeY = Get_Scale().y;
	Desc.fX = Get_Pos().x;
	Desc.fY = Get_Pos().y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = m_eRenderType;
	Desc.iPriority = m_iRenderPriority - 1;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_FaceBg";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

shared_ptr<CUi_KorsicaFace> CUi_KorsicaFace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_KorsicaFace
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_KorsicaFace(pDevice, pContext) { }
	};

	shared_ptr<CUi_KorsicaFace> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

void CUi_KorsicaFace::Dying()
{
	m_pBackTextureCom = { nullptr };
	m_pBack->Dead();
}

shared_ptr<CGameObject> CUi_KorsicaFace::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_KorsicaFace
	{
		MakeSharedEnabler(const CUi_KorsicaFace& rhs) : CUi_KorsicaFace(rhs) { }
	};

	shared_ptr<CUi_KorsicaFace> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}

void CUi_KorsicaFace::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pBack->Set_Active(Active);
}
