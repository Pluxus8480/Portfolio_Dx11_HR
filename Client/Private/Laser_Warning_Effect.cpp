#include "Laser_Warning_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Camera.h"

wstring CLaser_Warning_Effect::ObjID = TEXT("CLaser_Warning_Effect");

CLaser_Warning_Effect::CLaser_Warning_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CLaser_Warning_Effect::ObjID);
}

CLaser_Warning_Effect::CLaser_Warning_Effect(const CLaser_Warning_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CLaser_Warning_Effect::~CLaser_Warning_Effect()
{
	Free();
}

HRESULT CLaser_Warning_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Laser_Warning_Effect"), TEXT("../Bin/Resources/Effect/Beam/T_VFX_tk_beam_tc_03.dds"), 1));

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Laser_Warning_Effect_Scratch"), TEXT("../Bin/Resources/Effect/Beam/T_VFX_tk_beam_02.png"), 1));

	return S_OK;
}

HRESULT CLaser_Warning_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(48.f, 4.f, 1.f));

	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));


	m_vEffectColorPresets.push_back({ 1.0f,0.0f,0.f,1.f });
	m_vEffectColorPresets.push_back({ 0.f,1.f,0.0f,1.f });
	m_vEffectColorPresets.push_back({ 1.f,1.f,1.f,1.f });
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CLaser_Warning_Effect::PriorityTick(_float fTimeDelta)
{

}

void CLaser_Warning_Effect::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CLaser_Warning_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CLaser_Warning_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//_float4x4 matTemp;
	//XMStoreFloat4x4(&matTemp, XMLoadFloat4x4(&m_matRotation) * m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matTemp);

	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	//m_pTransformCom->LookAt_Vertical(GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_Position());

	_float4x4 Temp;
	_float fScale = m_fEffectTimePercentReverse * 2.f - floor(m_fEffectTimePercentReverse * 2.f) * 0.8f + 0.2f;//m_fEffectTimePercent * 0.5f + 0.5f;
	XMStoreFloat4x4(&Temp, XMMatrixScaling(1.f, fScale, 1.f) * m_pTransformCom->Get_WorldMatrix_XMMat());

	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(&Temp), XMLoadFloat4x4(m_pParentWorld)));
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp)))
			return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };

	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &m_vEffectBaseColor, sizeof(_float4));

	_float fThreshold = ((m_fEffectTimePercent * 2.f) - floor(m_fEffectTimePercent * 2.f));

	if (m_fEffectTimePercent >= 0.5f)
	{
		_float4 vColor;
		XMStoreFloat4(&vColor, XMLoadFloat4(&m_vEffectColorPresets[0]) * XMLoadFloat4(&m_vEffectColorPresets[2]));
		m_pShaderCom->Bind_RawValue("g_vEffectBaseColorR", &vColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[0], sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[0], sizeof(_float4));
	}
	else
	{
		_float4 vColor;
		XMStoreFloat4(&vColor, XMLoadFloat4(&m_vEffectColorPresets[1]) * XMLoadFloat4(&m_vEffectColorPresets[2]));
		m_pShaderCom->Bind_RawValue("g_vEffectBaseColorR", &vColor, sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vEffectBaseColorG", &m_vEffectColorPresets[1], sizeof(_float4));
		m_pShaderCom->Bind_RawValue("g_vEffectBaseColorB", &m_vEffectColorPresets[1], sizeof(_float4));
	}


	m_pShaderCom->Bind_RawValue("g_vEffectColorBlendRate", &m_fEffectColorBlendRate, sizeof(_float));

	m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimePercent, sizeof(_float));
	_float2 vFlowSpeed = _float2{ m_fEffectUvFlowXSpeed, m_fEffectUvFlowYSpeed };
	m_pShaderCom->Bind_RawValue("g_vUVFlowSpeed", &vFlowSpeed, sizeof(_float2));

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", fThreshold * 0.2f);

	_float2 vUvMultiplier = _float2{ m_fEffectUvSizeXMultiplier, m_fEffectUvSizeYMultiplier };

	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUvMultiplier, sizeof(_float2));

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::WeightBlend)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CLaser_Warning_Effect::EffectStartCallBack()
{
}

void CLaser_Warning_Effect::EffectEndCallBack()
{

}

void CLaser_Warning_Effect::EffectSoftEnd()
{
}

void CLaser_Warning_Effect::EffectHardEnd()
{
}

HRESULT CLaser_Warning_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Laser_Warning_Effect"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Laser_Warning_Effect_Scratch"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CLaser_Warning_Effect> CLaser_Warning_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLaser_Warning_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLaser_Warning_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CLaser_Warning_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLaser_Warning_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CLaser_Warning_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLaser_Warning_Effect
	{
		MakeSharedEnabler(const CLaser_Warning_Effect& rhs) : CLaser_Warning_Effect(rhs) {}
	};

	shared_ptr<CLaser_Warning_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLaser_Warning_Effect"));
		assert(false);
	}

	return pInstance;
}

void CLaser_Warning_Effect::Free()
{
}