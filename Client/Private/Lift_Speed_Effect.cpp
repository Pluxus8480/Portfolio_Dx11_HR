#include "Lift_Speed_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

wstring CLift_Speed_Effect::ObjID = TEXT("CLift_Speed_Effect");

CLift_Speed_Effect::CLift_Speed_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CLift_Speed_Effect::ObjID);
}

CLift_Speed_Effect::CLift_Speed_Effect(const CLift_Speed_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CLift_Speed_Effect::~CLift_Speed_Effect()
{
	Free();
}

HRESULT CLift_Speed_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Line"), TEXT("../Bin/Resources/Effect/Line/Swing0_Noise0.png"), 1));



	return S_OK;
}

HRESULT CLift_Speed_Effect::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(24.f, 12.f, 1.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XM_PIDIV2);

	m_vEffectColorPresets.push_back(_float4{ 1.f,1.0f,1.0f,5.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.5f,1.0f, 5.f });
	m_vEffectColorPresets.push_back(_float4{ 0.5f,0.5f,1.0f,5.f });
	m_vEffectBaseColor = _float4{ 1.0f,1.0f,1.0f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	m_vecPositions.resize(50);
	auto pGI = m_pGameInstance.lock();
	for (_uint i = 0; i < 50; ++i)
	{
		_float fAngle = pGI->Random_Float(0.f, XM_PI);
		_float fZPos = pGI->Random_Float(-200.f, 200.f);
		_float fSpeed = pGI->Random_Float(400.f, 800.f);

		_vector vRight = { 40.f, 0.f,0.f,0.f };
		vRight =  XMVector3Rotate(vRight, XMQuaternionRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), fAngle));
		vRight.m128_f32[2] = fZPos;
		vRight.m128_f32[3] = fSpeed;
		XMStoreFloat4(&m_vecPositions[i], vRight);
	}
	return S_OK;
}

void CLift_Speed_Effect::PriorityTick(_float fTimeDelta)
{


}

void CLift_Speed_Effect::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < 50; ++i)
	{
		m_vecPositions[i].z -= m_vecPositions[i].w * fTimeDelta;
		if (m_vecPositions[i].z <= -200.f)
		{
			m_vecPositions[i].z += 400.f;
		}
	}
}

void CLift_Speed_Effect::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
	}
}

HRESULT CLift_Speed_Effect::Render()
{

	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;
	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	for(_uint i = 0; i<50;++i)
	{
		_matrix Temp = World;
		Temp.r[3] += XMLoadFloat4(&m_vecPositions[i]);
		Temp.r[3].m128_f32[3] = 1.f;
		_float4x4 World4x4;
		XMStoreFloat4x4(&World4x4, Temp);
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World4x4);
		//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

		if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::NonBlendMonoColor)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Draw()))
			return E_FAIL;
	}

	return S_OK;
}

void CLift_Speed_Effect::EffectStartCallBack()
{
}

void CLift_Speed_Effect::EffectEndCallBack()
{

}

void CLift_Speed_Effect::EffectSoftEnd()
{
}

void CLift_Speed_Effect::EffectHardEnd()
{
}

HRESULT CLift_Speed_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Line"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CLift_Speed_Effect> CLift_Speed_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLift_Speed_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLift_Speed_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CLift_Speed_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLift_Speed_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CLift_Speed_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLift_Speed_Effect
	{
		MakeSharedEnabler(const CLift_Speed_Effect& rhs) : CLift_Speed_Effect(rhs) {}
	};

	shared_ptr<CLift_Speed_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLift_Speed_Effect"));
		assert(false);
	}

	return pInstance;
}

void CLift_Speed_Effect::Free()
{
}