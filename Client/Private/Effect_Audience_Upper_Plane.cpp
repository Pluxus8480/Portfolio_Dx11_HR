#include "Effect_Audience_Upper_Plane.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"
#include "Beat_Manager.h"

wstring CEffect_Audience_Upper_Plane::ObjID = TEXT("CEffect_Audience_Upper_Plane");

CEffect_Audience_Upper_Plane::CEffect_Audience_Upper_Plane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_Audience_Upper_Plane::ObjID);
}

CEffect_Audience_Upper_Plane::CEffect_Audience_Upper_Plane(const CEffect_Audience_Upper_Plane& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_Audience_Upper_Plane::~CEffect_Audience_Upper_Plane()
{
	Free();
}

HRESULT CEffect_Audience_Upper_Plane::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_PlaneAudience_0"), TEXT("../Bin/Resources/Effect/Stage/PlaneAudience_0.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CEffect_Audience_Upper_Plane::Initialize(void* pArg)
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

void CEffect_Audience_Upper_Plane::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CEffect_Audience_Upper_Plane::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CEffect_Audience_Upper_Plane::LateTick(_float fTimeDelta)
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

HRESULT CEffect_Audience_Upper_Plane::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = max(BEATRATE * 0.5f + 0.5f, 0.75f);
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

	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

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

void CEffect_Audience_Upper_Plane::EffectStartCallBack()
{
	m_fState = !m_fState;
}

void CEffect_Audience_Upper_Plane::EffectEndCallBack()
{

}

void CEffect_Audience_Upper_Plane::EffectSoftEnd()
{
}

void CEffect_Audience_Upper_Plane::EffectHardEnd()
{
}

HRESULT CEffect_Audience_Upper_Plane::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_PlaneAudience_0"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom )
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_Audience_Upper_Plane> CEffect_Audience_Upper_Plane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Audience_Upper_Plane
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Audience_Upper_Plane(pDevice, pContext)
		{}
	};

	shared_ptr<CEffect_Audience_Upper_Plane> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Audience_Upper_Plane"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CEffect_Audience_Upper_Plane::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Audience_Upper_Plane
	{
		MakeSharedEnabler(const CEffect_Audience_Upper_Plane& rhs) : CEffect_Audience_Upper_Plane(rhs) {}
	};

	shared_ptr<CEffect_Audience_Upper_Plane> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Audience_Upper_Plane"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Audience_Upper_Plane::Free()
{
}