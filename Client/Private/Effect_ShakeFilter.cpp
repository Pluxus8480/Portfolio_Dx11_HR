#include "Effect_ShakeFilter.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "PlayerManager.h"
#include "Player_Camera.h"

wstring CEffect_ShakeFilter::ObjID = TEXT("CEffect_ShakeFilter");

CEffect_ShakeFilter::CEffect_ShakeFilter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CEffect_ShakeFilter::ObjID);
}

CEffect_ShakeFilter::CEffect_ShakeFilter(const CEffect_ShakeFilter& rhs)
	: CEffect_Base(rhs)
{
}

CEffect_ShakeFilter::~CEffect_ShakeFilter()
{
	Free();
}

HRESULT CEffect_ShakeFilter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_ShakeFilter::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	SetColorPresetRGB(_float4{ 1.f,0.1f,0.1f,1.f }, _float4{ 1.f,0.8f,0.1f,1.f }, _float4{ 0.8f,0.5f,1.0f,1.f });

	EffectInit();
	return S_OK;
}

void CEffect_ShakeFilter::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CEffect_ShakeFilter::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CEffect_ShakeFilter::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		}
}

HRESULT CEffect_ShakeFilter::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	return S_OK;
}

void CEffect_ShakeFilter::EffectStartCallBack()
{
	if(PLAYERMANAGER->Get_Player() != nullptr)
		PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Shake(0.5f, 0.2f, true);
}

void CEffect_ShakeFilter::EffectEndCallBack()
{

}

void CEffect_ShakeFilter::EffectSoftEnd()
{

}

void CEffect_ShakeFilter::EffectHardEnd()
{

}

HRESULT CEffect_ShakeFilter::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CEffect_ShakeFilter> CEffect_ShakeFilter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_ShakeFilter
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_ShakeFilter(pDevice, pContext)
		{}
	};


	shared_ptr<CEffect_ShakeFilter> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_ShakeFilter"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEffect_ShakeFilter::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_ShakeFilter
	{
		MakeSharedEnabler(const CEffect_ShakeFilter& rhs) : CEffect_ShakeFilter(rhs) {}
	};

	shared_ptr<CEffect_ShakeFilter> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_ShakeFilter"));
		assert(false);
	}

	return pInstance;
}

void CEffect_ShakeFilter::Free()
{
}