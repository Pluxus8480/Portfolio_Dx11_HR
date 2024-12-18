#include "Procedural_Cyclone_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CProcedural_Cyclone_Effect::ObjID = TEXT("CProcedural_Cyclone_Effect");

CProcedural_Cyclone_Effect::CProcedural_Cyclone_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CProcedural_Cyclone_Effect::ObjID);
}

CProcedural_Cyclone_Effect::CProcedural_Cyclone_Effect(const CProcedural_Cyclone_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CProcedural_Cyclone_Effect::~CProcedural_Cyclone_Effect()
{
	Free();
}

HRESULT CProcedural_Cyclone_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_Cylinder"), TEXT("../Bin/Resources/Models/Effect/Cylinder/Cylinder.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Cyclone_Scratch"), TEXT("../Bin/Resources/Models/Effect/Cylinder/T_VFX_tk_noise_07.dds"), 1));


	return S_OK;
}

HRESULT CProcedural_Cyclone_Effect::Initialize(void* pArg)
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
	m_vEffectColorPresets.push_back(_float4{ 0.3f, 1.0f, 0.2f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.1f, 0.7f, 0.1f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	m_fEffectUvFlowXSpeed = 1.f;
	m_fEffectUvFlowYSpeed = 1.f;
	m_bUseEffectUvFlowX = true;
	m_bUseEffectUvFlowY = true;
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	m_vecTransforms.resize(10);
	m_iNumCycle = rand() % 3 + 8;
	for (_int i = 0; i < m_iNumCycle; ++i)
	{
		_float fScale = m_pGameInstance.lock()->Random_Float(0.5f + (_float)(i) / (_float)m_iNumCycle - 0.5f / (_float)m_iNumCycle, 0.5f + (_float)(i) / (_float)m_iNumCycle + 0.5f / (_float)m_iNumCycle);
		_float fRotationX = m_pGameInstance.lock()->Random_Float(-XM_PIDIV4 * 0.5f, XM_PIDIV4 * 0.5f);
		_float fRotationZ = m_pGameInstance.lock()->Random_Float(-XM_PIDIV4 * 0.5f, XM_PIDIV4 * 0.5f);
		;
		XMStoreFloat4x4(&m_vecTransforms[i], XMMatrixAffineTransformation(XMVectorSet(fScale, 1.f, fScale, 0.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(fRotationX, 0.f, fRotationZ),
			XMVectorSet(0.f, (_float)(i - m_iNumCycle / 2) * 0.05f, 0.f, 1.f)));

	}
	return S_OK;
}

void CProcedural_Cyclone_Effect::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CProcedural_Cyclone_Effect::Tick(_float fTimeDelta)
{
	PARENTCHECK
	EffectTick(fTimeDelta);
}

void CProcedural_Cyclone_Effect::LateTick(_float fTimeDelta)
{
	PARENTCHECK
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CProcedural_Cyclone_Effect::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);





	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);


	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));

	//m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0);

	for (_int i = 0; i < m_iNumCycle; ++i)
	{
		_float4x4 World;
		_float fAngle = pow(min((m_fEffectTimePercent * 2.f - (_float)i / (_float)m_iNumCycle), 1.f), 0.25f) * XM_2PI * 2.f;
		XMStoreFloat4x4(&World, XMMatrixRotationY(fAngle) * m_pTransformCom->Get_WorldMatrix_XMMat());
		if (m_pParentWorld != nullptr) 
		{
			_float4x4 ParentMultipleWorld;
			XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(&m_vecTransforms[i]) * XMLoadFloat4x4(&World), XMLoadFloat4x4(m_pParentWorld)));
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
				return E_FAIL;
		}
		else 
		{
			_float4x4 Temp;
			XMStoreFloat4x4(&Temp, XMLoadFloat4x4(&m_vecTransforms[i]) * XMLoadFloat4x4(&World));
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp)))
				return E_FAIL;
		}
		m_pShaderCom->Bind_Float("g_fDissolveThreshold", abs((max((m_fEffectTimePercent * 2.f - (_float)i / (_float)m_iNumCycle), 0.f) - 0.5f) * 2.f));
		_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin((_uint)EFFECT_PASS::Fire);

			m_pModelCom->Render((_uint)i);
		}
	}


	return S_OK;
}

void CProcedural_Cyclone_Effect::EffectStartCallBack()
{
	m_iNumCycle = rand() % 3 + 8;
	for (_int i = 0; i < m_iNumCycle; ++i)
	{
		_float fScale = m_pGameInstance.lock()->Random_Float(0.5f + (_float)(i) / (_float)m_iNumCycle - 0.5f / (_float)m_iNumCycle, 0.5f + (_float)(i) / (_float)m_iNumCycle + 0.5f / (_float)m_iNumCycle);
		_float fRotationX = m_pGameInstance.lock()->Random_Float(-XM_PIDIV4 * 0.5f, XM_PIDIV4 * 0.5f);
		_float fRotationZ = m_pGameInstance.lock()->Random_Float(-XM_PIDIV4 * 0.5f, XM_PIDIV4 * 0.5f);
		;
		XMStoreFloat4x4(&m_vecTransforms[i], XMMatrixAffineTransformation(XMVectorSet(fScale, 1.f, fScale, 0.f),
			XMVectorSet(0.f,0.f,0.f,1.f), XMQuaternionRotationRollPitchYaw(fRotationX, 0.f, fRotationZ),
			XMVectorSet(0.f, (_float)(i - m_iNumCycle / 2) * 0.05f, 0.f, 1.f)));

	}

}

void CProcedural_Cyclone_Effect::EffectEndCallBack()
{

}

void CProcedural_Cyclone_Effect::EffectSoftEnd()
{
}

void CProcedural_Cyclone_Effect::EffectHardEnd()
{
}

HRESULT CProcedural_Cyclone_Effect::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Cylinder"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Cyclone_Scratch"), TEXT("Com_Texture")));


	if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CProcedural_Cyclone_Effect> CProcedural_Cyclone_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CProcedural_Cyclone_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CProcedural_Cyclone_Effect(pDevice, pContext)
		{}
	};


	shared_ptr<CProcedural_Cyclone_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CProcedural_Cyclone_Effect"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CProcedural_Cyclone_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CProcedural_Cyclone_Effect
	{
		MakeSharedEnabler(const CProcedural_Cyclone_Effect& rhs) : CProcedural_Cyclone_Effect(rhs) {}
	};

	shared_ptr<CProcedural_Cyclone_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CProcedural_Cyclone_Effect"));
		assert(false);
	}

	return pInstance;
}

void CProcedural_Cyclone_Effect::Free()
{
}
