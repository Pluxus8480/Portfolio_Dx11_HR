#include "BeamCylinder.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CBeamCylinder::ObjID = TEXT("CBeamCylinder");

CBeamCylinder::CBeamCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CBeamCylinder::ObjID);
}

CBeamCylinder::CBeamCylinder(const CBeamCylinder& rhs)
	: CEffect_Base(rhs)
{
}

CBeamCylinder::~CBeamCylinder()
{
	Free();
}

HRESULT CBeamCylinder::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_Cylinder"), TEXT("../Bin/Resources/Models/Effect/Cylinder/Cylinder.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_BeamCylinder_Scratch"), TEXT("../Bin/Resources/Models/Effect/Cylinder/T_VFX_tk_scratch_06.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_BeamCylinder"), TEXT("../Bin/Resources/Models/Effect/Cylinder/T_VFX_tk_noise_06.dds"), 1));


	return S_OK;
}

HRESULT CBeamCylinder::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);

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
	return S_OK;
}

void CBeamCylinder::PriorityTick(_float fTimeDelta)
{
}

void CBeamCylinder::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
	m_fTimer += fTimeDelta * m_fEffectTimeAccMultiplier / (m_fEffectTimeMax - m_fEffectTimeMin);

}

void CBeamCylinder::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		if (m_bUseGlow)	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		else m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
	}
}

HRESULT CBeamCylinder::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	//_float fAngle = pow(m_fEffectTimePercent, 0.25f) * XM_2PI;
	XMStoreFloat4x4(&Temp,  m_pTransformCom->Get_WorldMatrix_XMMat());
	//m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	


	if (m_pParentWorld != nullptr) {

		_float4x4 ParentMultipleWorld;
		auto pParent = m_pParentUnion.lock();

		if (pParent)
		{
			_float fParentPer = pParent->GetUnionTimePer();
			if (fParentPer <= 0.1f)
			{
				XMStoreFloat4x4(&Temp, XMMatrixScaling(fParentPer * 10.f, fParentPer * 10.f, fParentPer * 10.f)* XMLoadFloat4x4(&Temp));
			}
			else if (fParentPer >= 0.9f)
			{
				_float fScale = 1.f - fParentPer;
				XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale * 10.f, fScale * 10.f, fScale * 10.f) * XMLoadFloat4x4(&Temp));
			}
		}
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

	m_pShaderCom->Bind_Float("g_fEffectTimeAcc", m_fTimer);
	
	//m_pShaderCom->Bind_Float("g_fNoiseStrength", GAMEINSTANCE->Random_Float(0.02f, 0.04f));

	//m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", abs((m_fEffectTimePercent - 0.5f) * 2.f));

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_PerlinNoise_Texture", 0);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "", (_uint)i, aiTextureType_SPECULAR)))
		//	return E_FAIL;
		m_pShaderCom->Begin((_uint)EFFECT_PASS::DrakkenLaser);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CBeamCylinder::EffectStartCallBack()
{
}

void CBeamCylinder::EffectEndCallBack()
{

}

void CBeamCylinder::EffectSoftEnd()
{
}

void CBeamCylinder::EffectHardEnd()
{
}

HRESULT CBeamCylinder::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Cylinder"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_BeamCylinder_Scratch"), TEXT("Com_ScratchTexture")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_BeamCylinder"), TEXT("Com_Texture")));


	if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CBeamCylinder> CBeamCylinder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBeamCylinder
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBeamCylinder(pDevice, pContext)
		{}
	};


	shared_ptr<CBeamCylinder> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBeamCylinder"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CBeamCylinder::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBeamCylinder
	{
		MakeSharedEnabler(const CBeamCylinder& rhs) : CBeamCylinder(rhs) {}
	};

	shared_ptr<CBeamCylinder> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBeamCylinder"));
		assert(false);
	}

	return pInstance;
}

void CBeamCylinder::Free()
{
}
