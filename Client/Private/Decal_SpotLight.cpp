#include "Decal_SpotLight.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"

wstring CDecal_SpotLight::ObjID = TEXT("CDecal_SpotLight");

CDecal_SpotLight::CDecal_SpotLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CDecal_SpotLight::ObjID);
}

CDecal_SpotLight::CDecal_SpotLight(const CDecal_SpotLight& rhs)
	: CEffect_Base(rhs)
{
}

CDecal_SpotLight::~CDecal_SpotLight()
{
	Free();
}

HRESULT CDecal_SpotLight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Decal"), TEXT("../Bin/Resources/Effect/Decal/Decal_%d.dds"), 4));

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Decal_Scratch"), TEXT("../Bin/Resources/Effect/Decal/Scratch_%d.png"), 4));

	return S_OK;
}

HRESULT CDecal_SpotLight::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(4.f, 0.25f, 4.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	SetColorPresetRGB(_float4{ 2.f,1.0f,0.0f,1.f }, _float4{ 0.f,0.0f,0.0f,1.f }, _float4{ 0.0f,0.0f,0.0f,1.f });
	m_vEffectBaseColor = _float4{ 7.f,0.5f,0.f,1.f };
	m_fEffectAlpha = 1.f;
	EffectInit();
	return S_OK;
}

void CDecal_SpotLight::PriorityTick(_float fTimeDelta)
{
}

void CDecal_SpotLight::Tick(_float fTimeDelta)
{
	EffectTick(fTimeDelta);
}

void CDecal_SpotLight::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_DECAL_ALL, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CDecal_SpotLight::Render()
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

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);

	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_Position", m_pShaderCom, "g_PositionTexture");

	//_float fScale = m_fEffectTimePercent;
	//XMStoreFloat4x4(&World, XMMatrixScaling(fScale, 1.f, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());
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


	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 1)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2D", 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)CUBE_PASS::SSD_SPOTLIGHT)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;*/

		//if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		//	return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CDecal_SpotLight::EffectStartCallBack()
{

}

void CDecal_SpotLight::EffectEndCallBack()
{

}

void CDecal_SpotLight::EffectSoftEnd()
{

}

void CDecal_SpotLight::EffectHardEnd()
{

}

HRESULT CDecal_SpotLight::Ready_Components()
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

shared_ptr<CDecal_SpotLight> CDecal_SpotLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CDecal_SpotLight
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDecal_SpotLight(pDevice, pContext)
		{}
	};


	shared_ptr<CDecal_SpotLight> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDecal_SpotLight"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CDecal_SpotLight::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CDecal_SpotLight
	{
		MakeSharedEnabler(const CDecal_SpotLight& rhs) : CDecal_SpotLight(rhs) {}
	};

	shared_ptr<CDecal_SpotLight> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDecal_SpotLight"));
		assert(false);
	}

	return pInstance;
}

void CDecal_SpotLight::Free()
{
}