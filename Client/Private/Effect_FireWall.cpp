#include "Effect_FireWall.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CEffect_FireWall::ObjID = TEXT("CEffect_FireWall");

CEffect_FireWall::CEffect_FireWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_FireWall::ObjID);
}

CEffect_FireWall::CEffect_FireWall(const CEffect_FireWall& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_FireWall::~CEffect_FireWall()
{
	Free();
}

HRESULT CEffect_FireWall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Effect_FireWall"), TEXT("../Bin/Resources/Models/Effect/FireWall/FireWall.fbx"), MODEL_TYPE::NONANIM, _float4x4{ 0.1f, 0.f,  0.f,  0.f,
																																											0.f,  0.1f, 0.f,  0.f,
																																											0.f,  0.f,  0.1f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("../Bin/Resources/Models/Effect/Fire/T_VFX_tk_scratch_03b.png"), 1));

	return S_OK;
}

HRESULT CEffect_FireWall::Initialize(void* pArg)
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

void CEffect_FireWall::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CEffect_FireWall::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CEffect_FireWall::LateTick(_float fTimeDelta)
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

HRESULT CEffect_FireWall::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = m_fRandScale * m_fEffectTimePercentReverse;
	_float fBaseScale = 1.0f;

	// m_fEffectTimePercentReverse에 따른 스케일 조정
	if (m_fEffectTimePercentReverse >= 0.8f) {
		fBaseScale = 0.2f + (1.0f - 0.2f) * ((m_fEffectTimePercentReverse - 0.8f) / 0.2f); // 0.5에서 1까지 증가
	}
	else {
		fBaseScale = 0.3f + (0.5f - 0.3f) * (m_fEffectTimePercentReverse / 0.8f); // 0.8에서 0까지 0.3까지 감소
	}

	if (GetUseDistortion()) {
		XMStoreFloat4x4(&Temp, XMMatrixScaling(max(fScale, fBaseScale) * 3.f, max(fScale, fBaseScale) * 6.f, max(fScale, fBaseScale) * 1.5f) * m_pTransformCom->Get_WorldMatrix_XMMat());
		XMStoreFloat4x4(&Temp, XMMatrixRotationY(m_fRandRotate) * XMLoadFloat4x4(&Temp));
	}
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

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", sin(XM_2PI * m_fEffectTimePercent) / 5.f + 0.5f);
	m_pShaderCom->Bind_Float("g_fNoiseStrength", 0.1f);
	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");
	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
	m_pShaderCom->Bind_Vector4("g_vCamPos", &m_pGameInstance.lock()->Get_CamLook_Float4());

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;*/

		m_pShaderCom->Begin((_uint)EFFECT_PASS::DistortMonoColor);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CEffect_FireWall::EffectStartCallBack()
{
	m_fRandRotate = GAMEINSTANCE->Random_Float(0.f, XM_2PI);
	m_fRandScale = GAMEINSTANCE->Random_Float(0.7f, 1.2f);
	m_fTimeMultiflier = GAMEINSTANCE->Random_Float(0.5f, 2.5f);
}

void CEffect_FireWall::EffectEndCallBack()
{
	m_fRandRotate = 0.f;
	m_fRandScale = 1.f;
	m_fTimeMultiflier = 1.f;
}

void CEffect_FireWall::EffectSoftEnd()
{
}

void CEffect_FireWall::EffectHardEnd()
{
}

HRESULT CEffect_FireWall::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_HammerOn"), TEXT("Com_Model")));

	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Cyclone_Scratch"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_FireWall> CEffect_FireWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_FireWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_FireWall(pDevice, pContext)
		{}
	};

	shared_ptr<CEffect_FireWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_FireWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CEffect_FireWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_FireWall
	{
		MakeSharedEnabler(const CEffect_FireWall& rhs) : CEffect_FireWall(rhs) {}
	};

	shared_ptr<CEffect_FireWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_FireWall"));
		assert(false);
	}

	return pInstance;
}

void CEffect_FireWall::Free()
{
}