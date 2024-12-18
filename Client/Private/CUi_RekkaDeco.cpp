#include "CUi_RekkaDeco.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_RekkaDecoEffect.h"


wstring CUi_RekkaDeco::ObjID = L"CUi_RekkaDeco";

CUi_RekkaDeco::CUi_RekkaDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_RekkaDeco::CUi_RekkaDeco(const CUi_RekkaDeco& rhs)
	:CUi_2D(rhs)
{
}

CUi_RekkaDeco::~CUi_RekkaDeco()
{
}

HRESULT CUi_RekkaDeco::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_RekkaDeco",
			L"../Bin/Resources/Textures/Ui/Boss/Rekka/RekkaBackGround%d.png", 2));
	
	m_List_Owning_ObjectID.push_back(CUi_RekkaDecoEffect::ObjID);

	return S_OK;
}

HRESULT CUi_RekkaDeco::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	Initialize_Effect();


	return S_OK;
}

void CUi_RekkaDeco::PriorityTick(_float fTimeDelta)
{
}

void CUi_RekkaDeco::Tick(_float fTimeDelta)
{
	Adjust_Effect(fTimeDelta);
}

void CUi_RekkaDeco::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_RekkaDeco::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_RekkaBuff", &m_bBuff, sizeof(_bool))))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BossBlurTexture", m_iTextureNum + 1)))
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

void CUi_RekkaDeco::Initialize_Effect()
{
	CUi_RekkaDecoEffect::Desc Desc;
	Desc.fSizeX = Get_Scale().x * 0.85f;
	Desc.fSizeY = Get_Scale().y * 0.4f;
	Desc.fX = Get_Pos().x;
	Desc.fY = Get_Pos().y - 20;
	Desc.fZ = 0.6f;
	Desc.eRenderType = m_eRenderType;
	Desc.iPriority = m_iRenderPriority + 1;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::RekkaDecoEffect;
	Desc.TextureFileName = L"Prototype_Component_Texture_RekkaBackGroundEffect";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pEffect = static_pointer_cast<CUi_RekkaDecoEffect>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_RekkaDecoEffect::ObjID, m_iLevelIndex, L"CUi", CUi_RekkaDecoEffect::ObjID, &Desc));
}

void CUi_RekkaDeco::Adjust_Effect(_float fTimeDelta)
{
	if (m_bBuff)
	{
		m_pEffect->Set_Active(true); 
		m_fEffectTime += fTimeDelta;
		if (m_fEffectTime < 2.f)
		{
			m_pEffect->Set_EffectStartPos(m_fEffectTime * 1.5f);
		}
		else
		{
			m_fEffectTime = 0;
		}
	}
	else
	{
		m_pEffect->Set_Active(false);
		m_fEffectTime = 0;
	}
}

shared_ptr<CUi_RekkaDeco> CUi_RekkaDeco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_RekkaDeco
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_RekkaDeco(pDevice, pContext) { }
	};

	shared_ptr<CUi_RekkaDeco> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_RekkaDeco::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_RekkaDeco
	{
		MakeSharedEnabler(const CUi_RekkaDeco& rhs) : CUi_RekkaDeco(rhs) { }
	};

	shared_ptr<CUi_RekkaDeco> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
