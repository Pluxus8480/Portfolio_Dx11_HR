#include "Flat_Wave_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Model.h"

wstring CFlat_Wave_Effect::ObjID = TEXT("CFlat_Wave_Effect");

CFlat_Wave_Effect::CFlat_Wave_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CFlat_Wave_Effect::ObjID);
}

CFlat_Wave_Effect::CFlat_Wave_Effect(const CFlat_Wave_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CFlat_Wave_Effect::~CFlat_Wave_Effect()
{
	Free();
}

HRESULT CFlat_Wave_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("../Bin/Resources/Models/Effect/Fire/T_VFX_tk_scratch_03b.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Fire_Noise"), TEXT("../Bin/Resources/Models/Effect/Fire/T_VFX_tk_noise_02.png"), 1));


	return S_OK;
}

HRESULT CFlat_Wave_Effect::Initialize(void* pArg)
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

void CFlat_Wave_Effect::PriorityTick(_float fTimeDelta)
{
}

void CFlat_Wave_Effect::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
}

void CFlat_Wave_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CFlat_Wave_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//_float4x4 Temp;
	//_float fAngle = -pow(m_fEffectTimePercent, 0.25f) * XM_2PI;
	//XMStoreFloat4x4(&Temp, XMMatrixRotationY(fAngle) * m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()), XMLoadFloat4x4(m_pParentWorld)));
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);


	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));

	//m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", abs((m_fEffectTimePercent - 0.5f) * 2.f));

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0);
	m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::Fire);
	m_pVIBufferCom->Render();
	

	return S_OK;
}

void CFlat_Wave_Effect::EffectStartCallBack()
{
}

void CFlat_Wave_Effect::EffectEndCallBack()
{

}

void CFlat_Wave_Effect::EffectSoftEnd()
{
}

void CFlat_Wave_Effect::EffectHardEnd()
{
}

HRESULT CFlat_Wave_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	/* For.Com_Model */

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Fire_Noise"), TEXT("Com_Texture")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("Com_ScratchTexture")));
	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	if (!m_pShaderCom || !m_pNoiseTextureCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CFlat_Wave_Effect> CFlat_Wave_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFlat_Wave_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFlat_Wave_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CFlat_Wave_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFlat_Wave_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CFlat_Wave_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFlat_Wave_Effect
	{
		MakeSharedEnabler(const CFlat_Wave_Effect& rhs) : CFlat_Wave_Effect(rhs) {}
	};

	shared_ptr<CFlat_Wave_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFlat_Wave_Effect"));
		assert(false);
	}

	return pInstance;
}

void CFlat_Wave_Effect::Free()
{
}
