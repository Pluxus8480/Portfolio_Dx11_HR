#include "CUi_KorsicaHpBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"


wstring CUi_KorsicaHpBar::ObjID = L"CUi_KorsicaHpBar";

CUi_KorsicaHpBar::CUi_KorsicaHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_KorsicaHpBar::CUi_KorsicaHpBar(const CUi_KorsicaHpBar& rhs)
	:CUi_2D(rhs)
{
}

CUi_KorsicaHpBar::~CUi_KorsicaHpBar()
{
}

HRESULT CUi_KorsicaHpBar::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KorsicaHp",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/KorsicaHp%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ChaiHp",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/ChaiHp.png"));

	return S_OK;
}

HRESULT CUi_KorsicaHpBar::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	m_bKorsica = Arg->bKorsica;

	return S_OK;
}

void CUi_KorsicaHpBar::PriorityTick(_float fTimeDelta)
{
}

void CUi_KorsicaHpBar::Tick(_float fTimeDelta)
{
	Cal_DamagedTime(fTimeDelta);
}

void CUi_KorsicaHpBar::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_KorsicaHpBar::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bKorsica", &m_bKorsica, sizeof(_bool))))
		assert(false);
	
	if (FAILED(m_pShaderCom->Bind_Float("g_fKorsicaHpPercent", m_fPercent)))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		assert(false);

	if (m_bKorsica)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_KorsicaHpTexture", 1)))
			assert(false);

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bKorsicaDamaged", &m_bKorsicaDamaged, sizeof(_bool))))
			assert(false);
	}

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	return S_OK;
}

void CUi_KorsicaHpBar::Set_HpPercent(_float fPercent)
{
	if (m_fPercent != fPercent)
		m_bKorsicaDamaged = true;

	m_fPercent = fPercent;
}

void CUi_KorsicaHpBar::Cal_DamagedTime(_float fTimeDelta)
{
	if (m_bKorsicaDamaged)
	{
		m_fDmgTime += fTimeDelta;
		if (m_fDmgTime > 0.2f)
		{
			m_fDmgTime = 0;
			m_bKorsicaDamaged = false;
		}
	}
}

shared_ptr<CUi_KorsicaHpBar> CUi_KorsicaHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_KorsicaHpBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_KorsicaHpBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_KorsicaHpBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_KorsicaHpBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_KorsicaHpBar
	{
		MakeSharedEnabler(const CUi_KorsicaHpBar& rhs) : CUi_KorsicaHpBar(rhs) { }
	};

	shared_ptr<CUi_KorsicaHpBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
