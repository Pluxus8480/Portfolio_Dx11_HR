#include "HammerOn_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CHammerOn_Effect::ObjID = TEXT("CHammerOn_Effect");

CHammerOn_Effect::CHammerOn_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CHammerOn_Effect::ObjID);
}

CHammerOn_Effect::CHammerOn_Effect(const CHammerOn_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CHammerOn_Effect::~CHammerOn_Effect()
{
	Free();
}

HRESULT CHammerOn_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_HammerOn"), TEXT("../Bin/Resources/Models/Effect/HammerOn/HammerOn.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CHammerOn_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	m_pTransformCom->Set_Scale(_float3{ 48.f,48.f,48.f });
	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.5f, 0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.9f, 0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	m_fEffectUvFlowXSpeed = 1.f;
	m_fEffectUvFlowYSpeed = 1.f;
	m_bUseEffectUvFlowX = true;
	m_bUseEffectUvFlowY = true;
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CHammerOn_Effect::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CHammerOn_Effect::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CHammerOn_Effect::LateTick(_float fTimeDelta)
{
	PARENTCHECK
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		 m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CHammerOn_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = min(pow(m_fEffectTimePercent * 4.f, 3.f) + 0.5f, 1.f);

	if (GetUseDistortion()) fScale = m_fEffectTimePercent;
	if (GetUseGlow()) {
		if (m_fEffectTimePercent < 0.2f) fScale = 0.2f;
		if (m_fEffectTimePercent > 0.2f) fScale = 0.2f;
		if (m_fEffectTimePercent > 0.4f) fScale = 0.4f;
		if (m_fEffectTimePercent > 0.6f) fScale = 0.6f;
		if (m_fEffectTimePercent > 0.8f) fScale = 1.f;
	}
	XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale, fScale, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());

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


	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", abs((m_fEffectTimePercent - 0.5f) * 2.f));
	if (GetUseGlow()) m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	//m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0);
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Effect_Texture_01", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "", (_uint)i, aiTextureType_SPECULAR)))
		//	return E_FAIL;
		m_pShaderCom->Begin((_uint)EFFECT_PASS::MonoColorMesh);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CHammerOn_Effect::EffectStartCallBack()
{
}

void CHammerOn_Effect::EffectEndCallBack()
{

}

void CHammerOn_Effect::EffectSoftEnd()
{
}

void CHammerOn_Effect::EffectHardEnd()
{
}

HRESULT CHammerOn_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_HammerOn"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	//m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("Com_Texture")));


	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CHammerOn_Effect> CHammerOn_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CHammerOn_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CHammerOn_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CHammerOn_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHammerOn_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CHammerOn_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CHammerOn_Effect
	{
		MakeSharedEnabler(const CHammerOn_Effect& rhs) : CHammerOn_Effect(rhs) {}
	};

	shared_ptr<CHammerOn_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHammerOn_Effect"));
		assert(false);
	}

	return pInstance;
}

void CHammerOn_Effect::Free()
{
}
