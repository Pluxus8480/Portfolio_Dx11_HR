#include "ExpandingCylinder.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CExpandingCylinder::ObjID = TEXT("CExpandingCylinder");

CExpandingCylinder::CExpandingCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CExpandingCylinder::ObjID);
}

CExpandingCylinder::CExpandingCylinder(const CExpandingCylinder& rhs)
	: CEffect_Base(rhs)
{
}

CExpandingCylinder::~CExpandingCylinder()
{
	Free();
}

HRESULT CExpandingCylinder::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_Cylinder"), TEXT("../Bin/Resources/Models/Effect/Cylinder/Cylinder.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CExpandingCylinder::Initialize(void* pArg)
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

void CExpandingCylinder::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CExpandingCylinder::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CExpandingCylinder::LateTick(_float fTimeDelta)
{
	PARENTCHECK
		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_DISTORTION, shared_from_this());
		}
}

HRESULT CExpandingCylinder::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = m_fEffectTimePercent;

	if (GetUseDistortion()) fScale = min(pow(m_fEffectTimePercent * 4.f, 3.f) + 0.5f, 1.f);
	XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale, 1.f, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());

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

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 1.f);

	m_pShaderCom->Bind_Float("g_fNoiseStrength", 0.02f);
	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");
	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
	m_pShaderCom->Bind_Vector4("g_vCamPos", &m_pGameInstance.lock()->Get_CamLook_Float4());

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	//m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0);
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Effect_Texture_01", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "", (_uint)i, aiTextureType_SPECULAR)))
		//	return E_FAIL;
		m_pShaderCom->Begin((_uint)EFFECT_PASS::DistortMonoColor);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CExpandingCylinder::EffectStartCallBack()
{
}

void CExpandingCylinder::EffectEndCallBack()
{

}

void CExpandingCylinder::EffectSoftEnd()
{
}

void CExpandingCylinder::EffectHardEnd()
{
}

HRESULT CExpandingCylinder::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Cylinder"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	//m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("Com_Texture")));


	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CExpandingCylinder> CExpandingCylinder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CExpandingCylinder
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CExpandingCylinder(pDevice, pContext)
		{}
	};


	shared_ptr<CExpandingCylinder> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CExpandingCylinder"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CExpandingCylinder::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CExpandingCylinder
	{
		MakeSharedEnabler(const CExpandingCylinder& rhs) : CExpandingCylinder(rhs) {}
	};

	shared_ptr<CExpandingCylinder> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CExpandingCylinder"));
		assert(false);
	}

	return pInstance;
}

void CExpandingCylinder::Free()
{
}
