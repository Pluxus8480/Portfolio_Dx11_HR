#include "Effect_Kale_Judgement.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CEffect_Kale_Judgement::ObjID = TEXT("CEffect_Kale_Judgement");

CEffect_Kale_Judgement::CEffect_Kale_Judgement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_Kale_Judgement::ObjID);
}

CEffect_Kale_Judgement::CEffect_Kale_Judgement(const CEffect_Kale_Judgement& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_Kale_Judgement::~CEffect_Kale_Judgement()
{
	Free();
}

HRESULT CEffect_Kale_Judgement::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_FireWall"), TEXT("../Bin/Resources/Models/Effect/FireWall/FireWall.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	return S_OK;
}

HRESULT CEffect_Kale_Judgement::Initialize(void* pArg)
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
	m_vEffectColorPresets.push_back(_float4{ 0.41f, 0.8f, 0.99f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f, 0.5f, 1.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	m_fEffectUvFlowXSpeed = 1.f;
	m_fEffectUvFlowYSpeed = 1.f;
	m_bUseEffectUvFlowX = true;
	m_bUseEffectUvFlowY = true;
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CEffect_Kale_Judgement::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CEffect_Kale_Judgement::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CEffect_Kale_Judgement::LateTick(_float fTimeDelta)
{
	PARENTCHECK
		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());

			if (GetUseGlow())
				m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
			else
				m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_DISTORTION, shared_from_this());
		}
}

HRESULT CEffect_Kale_Judgement::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	XMStoreFloat4x4(&Temp, m_pTransformCom->Get_WorldMatrix_XMMat());

	if (m_fEffectTimePercent < 0.3f)
		Temp._42 += m_fEffectTimePercent * 20.f;
	else
		Temp._42 += 6.f;
	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);

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

	_float fDissolveTime = 0.f;

	if (m_fEffectTimePercent < 0.5f)
		fDissolveTime = 0.f;
	else
		fDissolveTime = 0.3f * (m_fEffectTimePercent / 5.f) * 5.f;

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", fDissolveTime);
	//m_pShaderCom->Bind_Float("g_fNoiseStrength", 0.1f);
	//m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");
	//_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	//m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
	//m_pShaderCom->Bind_Vector4("g_vCamPos", &m_pGameInstance.lock()->Get_CamLook_Float4());

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)EFFECT_PASS::UVFlow);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CEffect_Kale_Judgement::EffectStartCallBack()
{

}

void CEffect_Kale_Judgement::EffectEndCallBack()
{
	m_fRandRotate = 0.f;
	m_fRandScale = 1.f;
	m_fTimeMultiflier = 1.f;
}

void CEffect_Kale_Judgement::EffectSoftEnd()
{
}

void CEffect_Kale_Judgement::EffectHardEnd()
{
}

HRESULT CEffect_Kale_Judgement::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_FireWall"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("TransitionDissolve"), TEXT("Com_Texture")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("TransitionDissolve"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_Kale_Judgement> CEffect_Kale_Judgement::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Kale_Judgement
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Kale_Judgement(pDevice, pContext)
		{}
	};

	shared_ptr<CEffect_Kale_Judgement> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Kale_Judgement"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CEffect_Kale_Judgement::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Kale_Judgement
	{
		MakeSharedEnabler(const CEffect_Kale_Judgement& rhs) : CEffect_Kale_Judgement(rhs) {}
	};

	shared_ptr<CEffect_Kale_Judgement> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Kale_Judgement"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Kale_Judgement::Free()
{
}