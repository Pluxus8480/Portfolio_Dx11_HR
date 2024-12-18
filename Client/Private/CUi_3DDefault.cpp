#include "CUi_3DDefault.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_3DDefault::ObjID = L"CUi_3DDefault";

CUi_3DDefault::CUi_3DDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_3DDefault::CUi_3DDefault(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_3DDefault::~CUi_3DDefault()
{
}

HRESULT CUi_3DDefault::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_3DDefault::Initialize(void* pArg)
{
	Initialize_Transform();
	if (FAILED(Ready_Components(pArg)))
		assert(false);
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	m_pTransformCom->Set_Speed(1);

	return S_OK;
}

void CUi_3DDefault::PriorityTick(_float fTimeDelta)
{
}

void CUi_3DDefault::Tick(_float fTimeDelta)
{
}

void CUi_3DDefault::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_3DDefault::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
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

	return S_OK;
}

void CUi_3DDefault::Initialize_StunGaugeResource()
{

}

shared_ptr<CUi_3DDefault> CUi_3DDefault::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_3DDefault
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_3DDefault(pDevice, pContext) { }
	};

	shared_ptr<CUi_3DDefault> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_3DDefault::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_3DDefault
	{
		MakeSharedEnabler(const CUi_3DDefault& rhs) : CUi_3DDefault(rhs) { }
	};

	shared_ptr<CUi_3DDefault> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
