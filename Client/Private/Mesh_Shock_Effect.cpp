#include "Mesh_Shock_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CMesh_Shock_Effect::ObjID = TEXT("CMesh_Shock_Effect");

CMesh_Shock_Effect::CMesh_Shock_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CMesh_Shock_Effect::ObjID);
}

CMesh_Shock_Effect::CMesh_Shock_Effect(const CMesh_Shock_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CMesh_Shock_Effect::~CMesh_Shock_Effect()
{
	Free();
}

HRESULT CMesh_Shock_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_Shock"), TEXT("../Bin/Resources/Models/Effect/Shock/CannonShot.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CMesh_Shock_Effect::Initialize(void* pArg)
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

void CMesh_Shock_Effect::PriorityTick(_float fTimeDelta)
{
}

void CMesh_Shock_Effect::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
}

void CMesh_Shock_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CMesh_Shock_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = 1.f;//pow(min(m_fEffectTimePercent * 2.f, 1.f), 4.f);
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

		if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
			return E_FAIL;
		
		m_pShaderCom->Begin((_uint)EFFECT_PASS::UVFlow);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CMesh_Shock_Effect::EffectStartCallBack()
{
}

void CMesh_Shock_Effect::EffectEndCallBack()
{

}

void CMesh_Shock_Effect::EffectSoftEnd()
{
}

void CMesh_Shock_Effect::EffectHardEnd()
{
}

HRESULT CMesh_Shock_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Shock"), TEXT("Com_Model")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Shock_Scratch"), TEXT("Com_Noise_Texture")));
	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CMesh_Shock_Effect> CMesh_Shock_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMesh_Shock_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMesh_Shock_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CMesh_Shock_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh_Shock_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CMesh_Shock_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMesh_Shock_Effect
	{
		MakeSharedEnabler(const CMesh_Shock_Effect& rhs) : CMesh_Shock_Effect(rhs) {}
	};

	shared_ptr<CMesh_Shock_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh_Shock_Effect"));
		assert(false);
	}

	return pInstance;
}

void CMesh_Shock_Effect::Free()
{
}