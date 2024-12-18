#include "Particle_Anime_Spark.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Point_Instance.h"
#include "EffectData.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"

wstring CParticle_Anime_Spark::ObjID = TEXT("CParticle_Anime_Spark");

CParticle_Anime_Spark::CParticle_Anime_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CParticle_Anime_Spark::ObjID);
}

CParticle_Anime_Spark::CParticle_Anime_Spark(const CParticle_Anime_Spark& rhs)
	: CEffect_Base(rhs)
{
}

CParticle_Anime_Spark::~CParticle_Anime_Spark()
{
	Free();
}

HRESULT CParticle_Anime_Spark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Particle_Spark"), TEXT("../Bin/Resources/Effect/Particle/ParticleSpark%d.png"), 2));

	return S_OK;
}

HRESULT CParticle_Anime_Spark::Initialize(void* pArg)
{
	CVIBuffer_Point_Instance::INSTANCE_DESC InstanceDesc{};
	InstanceDesc.iNumInstance = 20;
	InstanceDesc.vScale = _float2(1.f, 2.f);
	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//InstanceDesc.vCenter = _float3(0.f,0.f,0.f);
	InstanceDesc.vRange = _float3(5.f, 5.f, 5.f);
	InstanceDesc.vSpeed = _float2(1.f, 3.f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(1.f, 1.f);
	InstanceDesc.vTorque = _float3(1.f, 1.f, 1.f);
	InstanceDesc.bLoop = true;


	_wstring strPrototypeTag = TEXT("Prototype_Component_VIBuffer_PointInstance_Particle_AnimSpark_Base");
	strPrototypeTag += to_wstring(m_iObjectID);

	if (FAILED(GAMEINSTANCE->Add_Prototype(LEVEL_STATIC, strPrototypeTag,
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

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

void CParticle_Anime_Spark::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CParticle_Anime_Spark::Tick(_float fTimeDelta)
{
	PARENTCHECK
		if (!GetEffectTimeFreeze())
			m_pVIBufferCom->Update_PtInstance_Style(fTimeDelta);

	//m_pVIBufferCom->Update_Spread(fTimeDelta);

	EffectTick(fTimeDelta);
}

void CParticle_Anime_Spark::LateTick(_float fTimeDelta)
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

HRESULT CParticle_Anime_Spark::Render()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::UVFlow)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)INSTANCE_PASS::ParticleSparkPtInstance)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Anime_Spark::EffectStartCallBack()
{
	if (m_pVIBufferCom != nullptr) {
		if (m_pVIBufferCom->Get_IsRandomize()) {
			m_pVIBufferCom->Speed_Renewal(m_pVIBufferCom->Get_Speed().x, m_pVIBufferCom->Get_Speed().y);
			m_pVIBufferCom->Range_Renewal(m_pVIBufferCom->Get_Range());
		}
		m_pVIBufferCom->Reset();
	}
}

void CParticle_Anime_Spark::EffectEndCallBack()
{
}

void CParticle_Anime_Spark::EffectSoftEnd()
{
}

void CParticle_Anime_Spark::EffectHardEnd()
{
}

_float3 CParticle_Anime_Spark::Get_Pivot()
{
	if (m_pVIBufferCom == nullptr)
		return _float3(0.f, 0.f, 0.f);

	return m_pVIBufferCom->Get_Pivot();
}

void CParticle_Anime_Spark::Set_Pivot(_float3 vPivot)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Set_Pivot(vPivot);
}

_float2 CParticle_Anime_Spark::Get_Speed()
{
	if (m_pVIBufferCom == nullptr)
		return _float2(0.f, 0.f);

	return m_pVIBufferCom->Get_Speed();
}

void CParticle_Anime_Spark::Set_Speed(_float2 vSpeed)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Speed_Renewal(vSpeed.x, vSpeed.y);
}

_float3 CParticle_Anime_Spark::Get_Range()
{
	if (m_pVIBufferCom == nullptr)
		return _float3(0.f, 0.f, 0.f);

	return m_pVIBufferCom->Get_Range();
}

void CParticle_Anime_Spark::Set_Range(_float3 vRange)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Range_Renewal(vRange);
}

_bool CParticle_Anime_Spark::Get_IsRandomize()
{
	if (m_pVIBufferCom == nullptr)
		return false;

	return m_pVIBufferCom->Get_IsRandomize();
}

void CParticle_Anime_Spark::Set_IsRandomize(_bool bIsRandomize)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Set_IsRandomize(bIsRandomize);
}

_float2 CParticle_Anime_Spark::Get_Scale()
{
	if (m_pVIBufferCom == nullptr)
		return _float2(0.f, 0.f);

	return m_pVIBufferCom->Get_Scale();
}

void CParticle_Anime_Spark::Set_Scale(_float2 vScale)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Scale_Renewal(vScale);
}

CVIBuffer_Instance::INSTANCE_STYLE CParticle_Anime_Spark::Get_Style()
{
	if (m_pVIBufferCom == nullptr)
		assert(false);

	return m_pVIBufferCom->Get_Style();
}

void CParticle_Anime_Spark::Set_Style(CVIBuffer_Instance::INSTANCE_STYLE eStyle)
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Set_Style(eStyle);
}

HRESULT CParticle_Anime_Spark::Ready_Components()
{
	/* For.Com_Shader */
	//m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader")));

	_wstring strPrototypeTag = TEXT("Prototype_Component_VIBuffer_PointInstance_Particle_AnimSpark_Base");
	strPrototypeTag += to_wstring(m_iObjectID);

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Point_Instance>(Add_Component(LEVEL_STATIC, strPrototypeTag, TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Spark"), TEXT("Com_Texture")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BigSlash_Effect_Slash_01_Scratch"), TEXT("Com_Noise_Texture")));

	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CParticle_Anime_Spark> CParticle_Anime_Spark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CParticle_Anime_Spark
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CParticle_Anime_Spark(pDevice, pContext)
		{}
	};

	shared_ptr<CParticle_Anime_Spark> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Anime_Spark"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CParticle_Anime_Spark::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CParticle_Anime_Spark
	{
		MakeSharedEnabler(const CParticle_Anime_Spark& rhs) : CParticle_Anime_Spark(rhs) {}
	};

	shared_ptr<CParticle_Anime_Spark> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Anime_Spark"));
		assert(false);
	}

	return pInstance;
}

void CParticle_Anime_Spark::Free()
{
}