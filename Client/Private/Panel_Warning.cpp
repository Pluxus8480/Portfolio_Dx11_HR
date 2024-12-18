#include "Panel_Warning.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"

wstring CPanel_Warning::ObjID = TEXT("CPanel_Warning");

CPanel_Warning::CPanel_Warning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CPanel_Warning::ObjID);
}

CPanel_Warning::CPanel_Warning(const CPanel_Warning& rhs)
	: CEffect_Base(rhs)
{
}

CPanel_Warning::~CPanel_Warning()
{
	Free();
}

HRESULT CPanel_Warning::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Warn_TC"), TEXT("../Bin/Resources/Effect/Warning/T_VFX_tk_symbol_Arrow_04.dds"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Warn_Light"), TEXT("../Bin/Resources/Effect/Warning/T_VFX_tk_symbol_Arrow_03.dds"), 1));
	Set_PoolSize(10);

	return S_OK;
}

HRESULT CPanel_Warning::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(12.f, 24.f, 1.f));

	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	m_vEffectColorPresets.push_back(_float4{ 0.5f,0.5f,0.1f,1.f });
	m_vEffectColorPresets.push_back(_float4{ 1.0f, 1.0f,0.2f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 1.0f,0.3f,0.1f,1.f });
	m_vEffectBaseColor = _float4{ 1.0f,1.0f,1.0f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	m_bEffectLoop = true;
	m_bUseEffectUvFlowY = true;
	m_fEffectUvFlowYSpeed = -1.f;
	return S_OK;
}

void CPanel_Warning::PriorityTick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime <= 0.f)
	{
		Dead();
	}
}

void CPanel_Warning::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
}

void CPanel_Warning::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CPanel_Warning::Render()
{
	//_float fTimeRatio = m_fEffectTimeAcc / m_fEffectTimeMax;
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	//_float4x4 Temp;
	//XMStoreFloat4x4(&Temp, XMMatrixScaling(fTimeRatio, fTimeRatio, fTimeRatio) * m_pTransformCom->Get_WorldMatrix_XMMat());

	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", sin(m_fEffectTimePercent * XM_PI));
	//m_pShaderCom->Bind_Float("g_fDissolveThreshold",  0.f);

	if (FAILED(m_pLightTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	for(_int i = -2; i< 3; ++i)
	{
		_float4x4 Temp;
		XMStoreFloat4x4(&Temp,
			XMMatrixTranslation(0.f, 1.f * (_float)i, 0.f) * m_pTransformCom->Get_WorldMatrix_XMMat());
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
		if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::Warning)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffer()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Draw()))
			return E_FAIL;
	}

	return S_OK;
}

void CPanel_Warning::Push_From_Pool(void* pArg)
{
	m_fLifeTime = CBeat_Manager::Get_Instance()->Get_Beat() * 4.f;
	m_fEffectTimeAccMultiplier = 1.f / CBeat_Manager::Get_Instance()->Get_Beat();
	EffectStart();
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPanel_Warning::EffectStartCallBack()
{
}

void CPanel_Warning::EffectEndCallBack()
{

}

void CPanel_Warning::EffectSoftEnd()
{
}

void CPanel_Warning::EffectHardEnd()
{
}

HRESULT CPanel_Warning::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pLightTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Warn_Light"), TEXT("Com_Light_Texture")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Warn_TC"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pLightTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CPanel_Warning> CPanel_Warning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CPanel_Warning
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPanel_Warning(pDevice, pContext)
		{}
	};


	shared_ptr<CPanel_Warning> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPanel_Warning"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CPanel_Warning::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CPanel_Warning
	{
		MakeSharedEnabler(const CPanel_Warning& rhs) : CPanel_Warning(rhs) {}
	};

	shared_ptr<CPanel_Warning> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPanel_Warning"));
		assert(false);
	}

	return pInstance;
}

void CPanel_Warning::Free()
{
}