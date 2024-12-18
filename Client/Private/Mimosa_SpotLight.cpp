#include "Mimosa_SpotLight.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Manager.h"
#include "Beat_Manager.h"
wstring CMimosa_SpotLight::ObjID = TEXT("CMimosa_SpotLight");

CMimosa_SpotLight::CMimosa_SpotLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMimosa_SpotLight::CMimosa_SpotLight(const CMimosa_SpotLight& rhs)
	: CGameObject(rhs)
{
}

CMimosa_SpotLight::~CMimosa_SpotLight()
{
	Free();
}

HRESULT CMimosa_SpotLight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMimosa_SpotLight::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.vPosition = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vPosition;
	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_SpotLight = EFFECTMANAGER->Call_Union_Effect_Return("SpotLight_Enemy", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
	m_fColor = { 0.f,0.8f,1.f , 0.9f };

	return S_OK;
}

void CMimosa_SpotLight::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_SpotLight::Tick(_float fTimeDelta)
{
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
	if (m_bColor_Change)
	{
		m_fColor.y -= fBeat * 2.f * fTimeDelta * 0.8f;
		m_fColor.x += fBeat * 2.f * fTimeDelta * 0.8f;

		if (m_fColor.y < 0.f)
		{
			m_fColor.y = 0.f;
			m_fColor.x = 0.8f;
			m_bColor_Change = false;
		}

		if(m_SpotLight != nullptr)
			m_SpotLight->SetColorAll(m_fColor);
	}
}

void CMimosa_SpotLight::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
}

HRESULT CMimosa_SpotLight::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	 
	return S_OK;
}

void CMimosa_SpotLight::Dying()
{
	if (m_SpotLight)
		m_SpotLight->UnionEffectEnd();

	m_SpotLight->SetColorAll({ 0.f,0.8f,1.f , 0.9f });
}

HRESULT CMimosa_SpotLight::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CMimosa_SpotLight> CMimosa_SpotLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_SpotLight
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_SpotLight(pDevice, pContext)
		{}
	};


	shared_ptr<CMimosa_SpotLight> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_SpotLight"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CMimosa_SpotLight::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_SpotLight
	{
		MakeSharedEnabler(const CMimosa_SpotLight& rhs) : CMimosa_SpotLight(rhs)
		{}
	};


	shared_ptr<CMimosa_SpotLight> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_SpotLight"));
		assert(false);
	}

	return pInstance;
}

void CMimosa_SpotLight::Free()
{
	if(m_SpotLight)
		m_SpotLight->UnionEffectStop();
}
