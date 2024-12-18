#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "GameInstance.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLight::~CLight()
{
	Free();
}

HRESULT CLight::Initialize(void* pArg)
{
	LIGHT_DESC* pLightDesc = reinterpret_cast<LIGHT_DESC*>(pArg);
	memcpy(&m_LightDesc, pLightDesc, sizeof(LIGHT_DESC));


	CTransform::TRANSFORM_DESC tTransformDesc{};
	tTransformDesc.vPosition = pLightDesc->vPosition;


	if (FAILED(__super::Initialize(&tTransformDesc)))
		return E_FAIL;
	m_pTransformCom->LookAt(m_pTransformCom->Get_Position() + XMLoadFloat4(&pLightDesc->vDirection));
	Ready_Components();

	//Set_MainTag("Light");
	//Set_SubTag("Light");

	//m_pTransformCom->Set_Scale(_float3{ 0.3f, 0.6f, 0.3f });
	//m_pTransformCom->Set_Scale({ 0.7f,0.2f,3.f });

	return S_OK;
}

void CLight::Set_Desc(const LIGHT_DESC desc)
{
	m_LightDesc = desc;
}

HRESULT CLight::Ready_Components()
{
	return S_OK;
}

HRESULT CLight::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	auto pGameInstance = m_pGameInstance.lock();
	if(pGameInstance)
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Vector4("g_vCamPosition", &pGameInstance->Get_CamPosition_Float4())))
			return E_FAIL;
	}

	return S_OK;
}

void CLight::PriorityTick(_float fTimeDelta)
{
}

void CLight::Tick(_float fTimeDelta)
{
	XMStoreFloat4(&m_LightDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&m_LightDesc.vDirection, m_pTransformCom->Get_NormLook());
}

void CLight::LateTick(_float fTimeDelta)
{
}

HRESULT CLight::Render()
{
	return S_OK;
}

shared_ptr<CLight> CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* LightDesc)
{
	struct MakeSharedEnabler : public CLight
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLight(pDevice, pContext)
		{
		}
	};
	shared_ptr<CLight> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CLight"));
		assert(false);
	}

	return pInstance;
}

void CLight::Free()
{

}
