#include "CUi_KaleHpBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_KaleHpBar::ObjID = L"CUi_KaleHpBar";

CUi_KaleHpBar::CUi_KaleHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_KaleHpBar::CUi_KaleHpBar(const CUi_KaleHpBar& rhs)
	:CUi_2D(rhs)
{
}

CUi_KaleHpBar::~CUi_KaleHpBar()
{
}

HRESULT CUi_KaleHpBar::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KaleHp",
			L"../Bin/Resources/Textures/Ui/Boss/Kale/KaleHpBarReal%d.png", 2));

	return S_OK;
}

HRESULT CUi_KaleHpBar::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_KaleHpBar::PriorityTick(_float fTimeDelta)
{
}

void CUi_KaleHpBar::Tick(_float fTimeDelta)
{
	if (m_bDamaged)
	{
		m_fDamagedTime += fTimeDelta;
		if (m_fDamagedTime > 0.2f)
		{
			m_bDamaged = false;
			m_fDamagedTime = 0;
		}
	}
}

void CUi_KaleHpBar::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_KaleHpBar::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", (m_fPercent * 0.5f) + 0.25f)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_BossDamaged", &m_bDamaged, sizeof(_bool))))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_BlurStrong", 0.2f)))
		assert(false);
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BossBlurTexture", 1)))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	return S_OK;
}

void CUi_KaleHpBar::Set_Damaged(_bool Damaged)
{
	m_fDamagedTime = 0;
	m_bDamaged = Damaged;
}

shared_ptr<CUi_KaleHpBar> CUi_KaleHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_KaleHpBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_KaleHpBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_KaleHpBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_KaleHpBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_KaleHpBar
	{
		MakeSharedEnabler(const CUi_KaleHpBar& rhs) : CUi_KaleHpBar(rhs) { }
	};

	shared_ptr<CUi_KaleHpBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
