#include "Particle_Mimosa_Panel.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Point_Instance.h"
#include "EffectData.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"

wstring CParticle_Mimosa_Panel::ObjID = TEXT("CParticle_Mimosa_Panel");

CParticle_Mimosa_Panel::CParticle_Mimosa_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CParticle_Mimosa_Panel::ObjID);
}

CParticle_Mimosa_Panel::CParticle_Mimosa_Panel(const CParticle_Mimosa_Panel& rhs)
	: CEffect_Base(rhs)
{
}

CParticle_Mimosa_Panel::~CParticle_Mimosa_Panel()
{
	Free();
}

HRESULT CParticle_Mimosa_Panel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_T_VFX_tc_tk_em5000_PerfectGuard_02"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Barrier/T_VFX_tc_tk_em5000_PerfectGuard_02.png")));

	CVIBuffer_Point_Instance::INSTANCE_DESC InstanceDesc{};
	InstanceDesc.iNumInstance = 10;
	InstanceDesc.vScale = _float2(1.f, 2.f);
	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//InstanceDesc.vCenter = _float3(0.f,0.f,0.f);
	InstanceDesc.vRange = _float3(5.f, 5.f, 5.f);
	InstanceDesc.vSpeed = _float2(1.f, 3.f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(1.f, 1.f);
	InstanceDesc.vTorque = _float3(1.f, 1.f, 1.f);
	InstanceDesc.bLoop = true;

	if (FAILED(GAMEINSTANCE->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance_Particle_MimosaPanel_Base"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	//m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_BigSlash_Effect_Slash_01_Scratch"), TEXT("../Bin/Resources/Effect/Slash/1/T_VFX_ym_swing_03.png"), 1));

	return S_OK;
}

HRESULT CParticle_Mimosa_Panel::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(4.f, 4.f, 4.f));

	_float4 vPos = _float4(0.f, 2.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	m_eEffectType = EFFECT_TYPE::EFFECT_PARTICLE;
	m_pVIBufferCom->Set_Style(CVIBuffer_Point_Instance::STYLE_SPREAD);
	EffectInit();

	return S_OK;
}

void CParticle_Mimosa_Panel::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CParticle_Mimosa_Panel::Tick(_float fTimeDelta)
{
	PARENTCHECK
		if (!GetEffectTimeFreeze())
			m_pVIBufferCom->Update_PtInstance_Style(fTimeDelta);

	//m_pVIBufferCom->Update_Spread(fTimeDelta);

	EffectTick(fTimeDelta);
}

void CParticle_Mimosa_Panel::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			if (GetUseGlow())
				m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
			else
				m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CParticle_Mimosa_Panel::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

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

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0);

	/*if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;*/

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::UVFlow)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)INSTANCE_PASS::ParticleSparkPtInstance)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Mimosa_Panel::EffectStartCallBack()
{
}

void CParticle_Mimosa_Panel::EffectEndCallBack()
{
	if (m_pVIBufferCom != nullptr) {
		if (m_pVIBufferCom->Get_IsRandomize()) {
			m_pVIBufferCom->Speed_Renewal(m_pVIBufferCom->Get_Speed().x, m_pVIBufferCom->Get_Speed().y);
			m_pVIBufferCom->Range_Renewal(m_pVIBufferCom->Get_Range());
		}
		m_pVIBufferCom->Reset();
	}
}

void CParticle_Mimosa_Panel::EffectSoftEnd()
{
}

void CParticle_Mimosa_Panel::EffectHardEnd()
{
}

_float3 CParticle_Mimosa_Panel::Get_Pivot()
{
	if (m_pVIBufferCom == nullptr)
		return _float3(0.f, 0.f, 0.f);

	return m_pVIBufferCom->Get_Pivot();
}

void CParticle_Mimosa_Panel::Set_Pivot(_float3 vPivot)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Set_Pivot(vPivot);
}

_float2 CParticle_Mimosa_Panel::Get_Speed()
{
	if (m_pVIBufferCom == nullptr)
		return _float2(0.f, 0.f);

	return m_pVIBufferCom->Get_Speed();
}

void CParticle_Mimosa_Panel::Set_Speed(_float2 vSpeed)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Speed_Renewal(vSpeed.x, vSpeed.y);
}

_float3 CParticle_Mimosa_Panel::Get_Range()
{
	if (m_pVIBufferCom == nullptr)
		return _float3(0.f, 0.f, 0.f);

	return m_pVIBufferCom->Get_Range();
}

void CParticle_Mimosa_Panel::Set_Range(_float3 vRange)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Range_Renewal(vRange);
}

_bool CParticle_Mimosa_Panel::Get_IsRandomize()
{
	if (m_pVIBufferCom == nullptr)
		return false;

	return m_pVIBufferCom->Get_IsRandomize();
}

void CParticle_Mimosa_Panel::Set_IsRandomize(_bool bIsRandomize)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Set_IsRandomize(bIsRandomize);
}

_float2 CParticle_Mimosa_Panel::Get_Scale()
{
	if (m_pVIBufferCom == nullptr)
		return _float2(0.f, 0.f);

	return m_pVIBufferCom->Get_Scale();
}

void CParticle_Mimosa_Panel::Set_Scale(_float2 vScale)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Scale_Renewal(vScale);
}

CVIBuffer_Instance::INSTANCE_STYLE CParticle_Mimosa_Panel::Get_Style()
{
	if (m_pVIBufferCom == nullptr)
		assert(false);

	return m_pVIBufferCom->Get_Style();
}

void CParticle_Mimosa_Panel::Set_Style(CVIBuffer_Instance::INSTANCE_STYLE eStyle)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Set_Style(eStyle);
}

HRESULT CParticle_Mimosa_Panel::Ready_Components()
{
	/* For.Com_Shader */
	//m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader")));

	//m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));
	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Point_Instance>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance_Particle_MimosaPanel_Base"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_T_VFX_tc_tk_em5000_PerfectGuard_02"), TEXT("Com_Texture")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BigSlash_Effect_Slash_01_Scratch"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CParticle_Mimosa_Panel> CParticle_Mimosa_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CParticle_Mimosa_Panel
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CParticle_Mimosa_Panel(pDevice, pContext)
		{}
	};

	shared_ptr<CParticle_Mimosa_Panel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Mimosa_Panel"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CParticle_Mimosa_Panel::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CParticle_Mimosa_Panel
	{
		MakeSharedEnabler(const CParticle_Mimosa_Panel& rhs) : CParticle_Mimosa_Panel(rhs) {}
	};

	shared_ptr<CParticle_Mimosa_Panel> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Mimosa_Panel"));
		assert(false);
	}

	return pInstance;
}

void CParticle_Mimosa_Panel::Free()
{
}