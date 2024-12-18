#include "Radial_Blur.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CRadial_Blur::ObjID = TEXT("CRadial_Blur");

CRadial_Blur::CRadial_Blur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CRadial_Blur::ObjID);
}

CRadial_Blur::CRadial_Blur(const CRadial_Blur& rhs)
	: CEffect_Base(rhs)
{
}

CRadial_Blur::~CRadial_Blur()
{
	Free();
}

HRESULT CRadial_Blur::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRadial_Blur::Initialize(void* pArg)
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

void CRadial_Blur::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CRadial_Blur::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);

}

void CRadial_Blur::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			_vector vPosition = m_pTransformCom->Get_Position();
			_float2 vPos{ vPosition.m128_f32[0], vPosition.m128_f32[1] };
			_bool  bGlow = GetUseGlow();
			_bool  bDistort = GetUseDistortion();
			if (bGlow && !bDistort)
			{
				m_pGameInstance.lock()->Blur_On(vPos, m_fEffectAlpha * powf(m_fEffectTimePercentReverse, m_fEffectColorBlendRate));
			}
			if (bDistort && !bGlow)
			{
				m_pGameInstance.lock()->Blur_On(vPos, m_fEffectAlpha * powf(sin(m_fEffectTimePercent * XM_PI), m_fEffectColorBlendRate));
			}
			if (bDistort && bGlow)
			{
				m_pGameInstance.lock()->Blur_On(vPos, m_fEffectAlpha * powf(m_fEffectTimePercent, m_fEffectColorBlendRate));
			}
			if (!bDistort && !bGlow)
			{
				m_pGameInstance.lock()->Blur_On(vPos, m_fEffectAlpha);
			}
		}
}

HRESULT CRadial_Blur::Render()
{

	return S_OK;
}

void CRadial_Blur::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CRadial_Blur::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CRadial_Blur::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CRadial_Blur::EffectStartCallBack()
{
}

void CRadial_Blur::EffectEndCallBack()
{
}

void CRadial_Blur::EffectSoftEnd()
{
}

void CRadial_Blur::EffectHardEnd()
{
}

HRESULT CRadial_Blur::Ready_Components()
{
	return S_OK;
}

shared_ptr<CRadial_Blur> CRadial_Blur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRadial_Blur
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRadial_Blur(pDevice, pContext)
		{}
	};

	shared_ptr<CRadial_Blur> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRadial_Blur"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CRadial_Blur::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRadial_Blur
	{
		MakeSharedEnabler(const CRadial_Blur& rhs) : CRadial_Blur(rhs) {}
	};

	shared_ptr<CRadial_Blur> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRadial_Blur"));
		assert(false);
	}

	return pInstance;
}

void CRadial_Blur::Free()
{
}