#include "Effect_Mimosa_Warning.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"

wstring CEffect_Mimosa_Warning::ObjID = TEXT("CEffect_Mimosa_Warning");

CEffect_Mimosa_Warning::CEffect_Mimosa_Warning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_Mimosa_Warning::ObjID);
}

CEffect_Mimosa_Warning::CEffect_Mimosa_Warning(const CEffect_Mimosa_Warning& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_Mimosa_Warning::~CEffect_Mimosa_Warning()
{
	Free();
}

HRESULT CEffect_Mimosa_Warning::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Decal"), TEXT("../Bin/Resources/Effect/Decal/Decal_%d.dds"), 4));

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Decal_Scratch"), TEXT("../Bin/Resources/Effect/Decal/Scratch_%d.png"), 4));

	return S_OK;
}

HRESULT CEffect_Mimosa_Warning::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(1.f, 0.2f, 4.0f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	EffectInit();
	SetColorPresetRGB(_float4{ 0.2f,1.0f,0.3f,1.f }, _float4{ 1.f,0.2f,0.1f,1.f }, _float4{ 0.0f,0.0f,0.0f,1.f });
	m_fEffectAlpha = 0.8f;
	m_fEffectColorBlendRate = 0.1f;
	return S_OK;
}

void CEffect_Mimosa_Warning::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Mimosa_Warning::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_fEffectUvFlowX = vScale.x;
	m_fEffectUvFlowY = vScale.z;

	if (m_fEffectTimePercent < 0.25f)
	{
		if (m_fThreshold < 0.25f)
		{
			m_fThreshold = (m_fEffectTimePercent * 4.f) * (m_fEffectTimePercent * 4.f) * 0.25f;
		}
		else
		{
			m_fThreshold = 0.25f;
		}
	}
	else if (m_fEffectTimePercent < 0.5f)
	{
		if (m_fThreshold < 0.5f)
		{
			m_fThreshold = 0.25f + ((m_fEffectTimePercent - .25f) * 4.f) * ((m_fEffectTimePercent - .25f) * 4.f) * 0.25f;
		}
		else
		{
			m_fThreshold = 0.5f;
		}
	}
	else if (m_fEffectTimePercent < 0.75f)
	{
		if (m_fThreshold < 0.75f)
		{
			m_fThreshold = 0.5f + ((m_fEffectTimePercent - .5f) * 4.f) * ((m_fEffectTimePercent - .5f) * 4.f) * 0.25f;
		}
		else
		{
			m_fThreshold = 0.75f;
		}
	}
	else
	{
		if (m_fThreshold < 1.f)
		{
			m_fThreshold = 0.75f + ((m_fEffectTimePercent - .75f) * 4.f) * ((m_fEffectTimePercent - .75f) * 4.f) * 0.25f;
		}
		else
		{
			m_fThreshold = 1.f;
		}
	}
}

void CEffect_Mimosa_Warning::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_DECAL, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CEffect_Mimosa_Warning::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);


	m_pShaderCom->Bind_Float("g_fDissolveThreshold", m_fThreshold);

	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_Position", m_pShaderCom, "g_PositionTexture");

	_float4x4 WorldInv = m_pTransformCom->Get_WorldMatrix_Inversed();
	if (m_pParentWorld)
	{
		_matrix Parent = m_pTransformCom->Get_WorldMatrix_XMMat() * XMLoadFloat4x4(m_pParentWorld);
		_float4x4 Temp;
		XMStoreFloat4x4(&Temp, Parent);
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);

		XMStoreFloat4x4(&WorldInv, XMMatrixInverse(nullptr, Parent));
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}
	m_pShaderCom->Bind_Matrix("g_WorldInvMatrix", &WorldInv);

	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));

	//if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 3)))
	//	return E_FAIL;

	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2D", 1)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)CUBE_PASS::SSD_WARNING_CIRCLE_CLIP)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;*/

		//if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		//	return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Mimosa_Warning::EffectStartCallBack()
{
}

void CEffect_Mimosa_Warning::EffectEndCallBack()
{

}

void CEffect_Mimosa_Warning::EffectSoftEnd()
{

}

void CEffect_Mimosa_Warning::EffectHardEnd()
{

}

HRESULT CEffect_Mimosa_Warning::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Cube>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Decal"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Decal_Scratch"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_Mimosa_Warning> CEffect_Mimosa_Warning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Mimosa_Warning
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Mimosa_Warning(pDevice, pContext)
		{}
	};


	shared_ptr<CEffect_Mimosa_Warning> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Mimosa_Warning"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEffect_Mimosa_Warning::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Mimosa_Warning
	{
		MakeSharedEnabler(const CEffect_Mimosa_Warning& rhs) : CEffect_Mimosa_Warning(rhs) {}
	};

	shared_ptr<CEffect_Mimosa_Warning> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Mimosa_Warning"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Mimosa_Warning::Free()
{
}