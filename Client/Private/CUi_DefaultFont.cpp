#include "CUi_DefaultFont.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_DefaultFont::ObjID = L"CUi_DefaultFont";

CUi_DefaultFont::CUi_DefaultFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Default(pDevice, pContext)
{
}

CUi_DefaultFont::CUi_DefaultFont(const CUi_DefaultFont& rhs)
	:CUi_Default(rhs)
{
}

CUi_DefaultFont::~CUi_DefaultFont()
{
}

HRESULT CUi_DefaultFont::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUi_DefaultFont::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_DefaultFont::PriorityTick(_float fTimeDelta)
{
}

void CUi_DefaultFont::Tick(_float fTimeDelta)
{
}

void CUi_DefaultFont::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Adjust_FontPos();
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_DefaultFont::Render()
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

	RenderFont(m_FontDesc);

	return S_OK;
}

void CUi_DefaultFont::Set_FontDesc(FontDescInfo Font)
{
	m_FontDesc = Font;
}

void CUi_DefaultFont::Set_FontString(wstring FontString)
{
	m_FontDesc.String = FontString;
}

void CUi_DefaultFont::Adjust_FontPos()
{
	_float3 Pos = Get_Pos();
	m_FontDesc.DirectPos.x = m_FontDesc.RelativePos.x + Pos.x;
	m_FontDesc.DirectPos.y = m_FontDesc.RelativePos.y + Pos.y;
}

shared_ptr<CUi_DefaultFont> CUi_DefaultFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_DefaultFont
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_DefaultFont(pDevice, pContext) { }
	};

	shared_ptr<CUi_DefaultFont> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_DefaultFont::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_DefaultFont
	{
		MakeSharedEnabler(const CUi_DefaultFont& rhs) : CUi_DefaultFont(rhs) { }
	};

	shared_ptr<CUi_DefaultFont> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
