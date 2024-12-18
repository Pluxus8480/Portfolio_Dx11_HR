#include "LandJumpImpact_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CLandJumpImpact_Effect::ObjID = TEXT("CLandJumpImpact_Effect");

CLandJumpImpact_Effect::CLandJumpImpact_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CLandJumpImpact_Effect::ObjID);
}

CLandJumpImpact_Effect::CLandJumpImpact_Effect(const CLandJumpImpact_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CLandJumpImpact_Effect::~CLandJumpImpact_Effect()
{
	Free();
}

HRESULT CLandJumpImpact_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_LandImpact"), TEXT("../Bin/Resources/Models/Effect/LandImpact/LandImpact.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_BigSlash_Effect_Slash_01_Scratch"), TEXT("../Bin/Resources/Effect/Slash/1/T_VFX_ym_swing_03.png"), 1));


	return S_OK;
}

HRESULT CLandJumpImpact_Effect::Initialize(void* pArg)
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

	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.8f, 0.8f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.4f, 0.2f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CLandJumpImpact_Effect::PriorityTick(_float fTimeDelta)
{
}

void CLandJumpImpact_Effect::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
}

void CLandJumpImpact_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CLandJumpImpact_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = pow(min(m_fEffectTimePercent * 2.f, 1.f), 4.f);
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
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);


	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Effect_Texture_01", (_uint)i, aiTextureType_NORMALS)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)EFFECT_PASS::UVFlow);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CLandJumpImpact_Effect::EffectStartCallBack()
{
}

void CLandJumpImpact_Effect::EffectEndCallBack()
{

}

void CLandJumpImpact_Effect::EffectSoftEnd()
{
}

void CLandJumpImpact_Effect::EffectHardEnd()
{
}

HRESULT CLandJumpImpact_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_LandImpact"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CLandJumpImpact_Effect> CLandJumpImpact_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLandJumpImpact_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLandJumpImpact_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CLandJumpImpact_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLandJumpImpact_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CLandJumpImpact_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLandJumpImpact_Effect
	{
		MakeSharedEnabler(const CLandJumpImpact_Effect& rhs) : CLandJumpImpact_Effect(rhs) {}
	};

	shared_ptr<CLandJumpImpact_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLandJumpImpact_Effect"));
		assert(false);
	}

	return pInstance;
}

void CLandJumpImpact_Effect::Free()
{
}
