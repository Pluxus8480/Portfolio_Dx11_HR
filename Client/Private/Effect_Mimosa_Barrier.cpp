#include "Effect_Mimosa_Barrier.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CEffect_Mimosa_Barrier::ObjID = TEXT("CEffect_Mimosa_Barrier");

CEffect_Mimosa_Barrier::CEffect_Mimosa_Barrier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_Mimosa_Barrier::ObjID);
}

CEffect_Mimosa_Barrier::CEffect_Mimosa_Barrier(const CEffect_Mimosa_Barrier& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_Mimosa_Barrier::~CEffect_Mimosa_Barrier()
{
	Free();
}

HRESULT CEffect_Mimosa_Barrier::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_Barrier"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Barrier/Mimosa_Barrier.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_T_VFX_tc_tk_em5000_PerfectGuard_01"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Barrier/T_VFX_tc_tk_em5000_PerfectGuard_01.png")));

	return S_OK;
}

HRESULT CEffect_Mimosa_Barrier::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4 vPos = _float4(0.f, 1.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.8f, 0.8f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.4f, 0.2f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CEffect_Mimosa_Barrier::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Mimosa_Barrier::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
}

void CEffect_Mimosa_Barrier::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CEffect_Mimosa_Barrier::Render()
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

	_float fAlpha = 1.f;

	if (m_fEffectTimePercent < 0.2f)
		fAlpha = m_fEffectTimePercent * 5.f;
	else if (m_fEffectTimePercent > 0.8f)
		fAlpha = 1.f - (m_fEffectTimePercent - 0.8f) * 5.f;

	m_pShaderCom->Bind_Float("g_fEffectAlpha", fAlpha);
	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", m_fEffectTimePercent);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Effect_Texture_01", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)EFFECT_PASS::UVFlow);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CEffect_Mimosa_Barrier::EffectStartCallBack()
{
}

void CEffect_Mimosa_Barrier::EffectEndCallBack()
{

}

void CEffect_Mimosa_Barrier::EffectSoftEnd()
{
}

void CEffect_Mimosa_Barrier::EffectHardEnd()
{
}

HRESULT CEffect_Mimosa_Barrier::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Barrier"), TEXT("Com_Model")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_T_VFX_tc_tk_em5000_PerfectGuard_01"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_Mimosa_Barrier> CEffect_Mimosa_Barrier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Mimosa_Barrier
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Mimosa_Barrier(pDevice, pContext)
		{}
	};


	shared_ptr<CEffect_Mimosa_Barrier> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Mimosa_Barrier"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEffect_Mimosa_Barrier::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Mimosa_Barrier
	{
		MakeSharedEnabler(const CEffect_Mimosa_Barrier& rhs) : CEffect_Mimosa_Barrier(rhs) {}
	};

	shared_ptr<CEffect_Mimosa_Barrier> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Mimosa_Barrier"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Mimosa_Barrier::Free()
{
}
