#include "Camera_Void_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CCamera_Void_Effect::ObjID = TEXT("CCamera_Void_Effect");

CCamera_Void_Effect::CCamera_Void_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CCamera_Void_Effect::ObjID);
}

CCamera_Void_Effect::CCamera_Void_Effect(const CCamera_Void_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CCamera_Void_Effect::~CCamera_Void_Effect()
{
	Free();
}

HRESULT CCamera_Void_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Void_Effect::Initialize(void* pArg)
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
	m_pTransformCom->Set_Scale(_float3{ 3.f,3.f,3.f });
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

void CCamera_Void_Effect::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CCamera_Void_Effect::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CCamera_Void_Effect::LateTick(_float fTimeDelta)
{
	PARENTCHECK
		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_LENS, shared_from_this());
		}
}

HRESULT CCamera_Void_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = 1.f;
	XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale, fScale, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());

	_float4 vCamPos = m_pGameInstance.lock()->Get_CamPosition_Float4();

	Temp._41 = vCamPos.x;
	Temp._42 = vCamPos.y;
	Temp._43 = vCamPos.z;
	Temp._44 = 1.f;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fNoiseStrength", 0.2f);
	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");
	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
	m_pShaderCom->Bind_Vector4("g_vCamPos", &m_pGameInstance.lock()->Get_CamLook_Float4());

	_float fDissolveThreshold = 0.f;

	if (GetUseGlow())		fDissolveThreshold = m_fEffectTimePercentReverse;
	if (GetUseDistortion())		fDissolveThreshold = m_fEffectTimePercent;

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", fDissolveThreshold);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if(FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Effect_Texture_01", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;*/

			//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "", (_uint)i, aiTextureType_SPECULAR)))
			//	return E_FAIL;
		m_pShaderCom->Begin((_uint)EFFECT_PASS::ColorReverseEx);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CCamera_Void_Effect::EffectStartCallBack()
{
}

void CCamera_Void_Effect::EffectEndCallBack()
{

}

void CCamera_Void_Effect::EffectSoftEnd()
{
}

void CCamera_Void_Effect::EffectHardEnd()
{
}

HRESULT CCamera_Void_Effect::Ready_Components()
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

shared_ptr<CCamera_Void_Effect> CCamera_Void_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CCamera_Void_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCamera_Void_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CCamera_Void_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Void_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CCamera_Void_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CCamera_Void_Effect
	{
		MakeSharedEnabler(const CCamera_Void_Effect& rhs) : CCamera_Void_Effect(rhs) {}
	};

	shared_ptr<CCamera_Void_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Void_Effect"));
		assert(false);
	}

	return pInstance;
}

void CCamera_Void_Effect::Free()
{
}
