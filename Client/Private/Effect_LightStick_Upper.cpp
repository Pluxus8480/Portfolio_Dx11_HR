#include "Effect_LightStick_Upper.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CEffect_LightStick_Upper::ObjID = TEXT("CEffect_LightStick_Upper");

CEffect_LightStick_Upper::CEffect_LightStick_Upper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_LightStick_Upper::ObjID);
}

CEffect_LightStick_Upper::CEffect_LightStick_Upper(const CEffect_LightStick_Upper& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_LightStick_Upper::~CEffect_LightStick_Upper()
{
	Free();
}

HRESULT CEffect_LightStick_Upper::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Shine_0"), TEXT("../Bin/Resources/Effect/Stage/Shine_0.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CEffect_LightStick_Upper::Initialize(void* pArg)
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
	m_pTransformCom->Set_Scale(_float3{ 1.f,1.f,1.f });
	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.f, 0.f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f, 1.f, 0.f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f, 0.f, 1.f,1.f });
	m_fEffectUvFlowXSpeed = 1.f;
	m_fEffectUvFlowYSpeed = 1.f;
	m_bUseEffectUvFlowX = false;
	m_bUseEffectUvFlowY = false;
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CEffect_LightStick_Upper::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CEffect_LightStick_Upper::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CEffect_LightStick_Upper::LateTick(_float fTimeDelta)
{
	PARENTCHECK
		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());

			if (GetUseGlow())
				m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
			else
				m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		}
}

HRESULT CEffect_LightStick_Upper::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp = m_pTransformCom->Get_WorldMatrix();

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

	_float2 vFlowSpeed = _float2{ m_fUvRandX, m_fUvRandY };

	m_pShaderCom->Bind_RawValue("g_vUVFlow", &vFlowSpeed, sizeof(_float2));

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (m_fState) {
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}
		m_pShaderCom->Begin((_uint)EFFECT_PASS::UVFlow);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CEffect_LightStick_Upper::EffectStartCallBack()
{
	m_fState = !m_fState;
	m_fUvRandX = GAMEINSTANCE->Random_Float(0.f, 1.f);
	m_fUvRandY = GAMEINSTANCE->Random_Float(0.f, 1.f);
}

void CEffect_LightStick_Upper::EffectEndCallBack()
{

}

void CEffect_LightStick_Upper::EffectSoftEnd()
{
}

void CEffect_LightStick_Upper::EffectHardEnd()
{
}

HRESULT CEffect_LightStick_Upper::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Shine_0"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_LightStick_Upper> CEffect_LightStick_Upper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_LightStick_Upper
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_LightStick_Upper(pDevice, pContext)
		{}
	};

	shared_ptr<CEffect_LightStick_Upper> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_LightStick_Upper"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CEffect_LightStick_Upper::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_LightStick_Upper
	{
		MakeSharedEnabler(const CEffect_LightStick_Upper& rhs) : CEffect_LightStick_Upper(rhs) {}
	};

	shared_ptr<CEffect_LightStick_Upper> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_LightStick_Upper"));
		assert(false);
	}

	return pInstance;
}

void CEffect_LightStick_Upper::Free()
{
}