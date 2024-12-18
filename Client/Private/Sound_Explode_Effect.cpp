#include "Sound_Explode_Effect.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CSound_Explode_Effect::ObjID = TEXT("CSound_Explode_Effect");

CSound_Explode_Effect::CSound_Explode_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CSound_Explode_Effect::ObjID);
}

CSound_Explode_Effect::CSound_Explode_Effect(const CSound_Explode_Effect& rhs)
	: CEffect_Base(rhs)
{
}

CSound_Explode_Effect::~CSound_Explode_Effect()
{
	Free();
}

HRESULT CSound_Explode_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(L"Sound_Enemy_Explode", L"../Bin/Resources/Sounds/SFX/enemy_explode.ogg", SOUND_CHANNEL::SOUND_EFFECT));

	return S_OK;
}

HRESULT CSound_Explode_Effect::Initialize(void* pArg)
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

void CSound_Explode_Effect::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CSound_Explode_Effect::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);

}

void CSound_Explode_Effect::LateTick(_float fTimeDelta)
{
	PARENTCHECK

	if (GetEffectIsView()) {
	}
}

HRESULT CSound_Explode_Effect::Render()
{

	return S_OK;
}

void CSound_Explode_Effect::Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CSound_Explode_Effect::Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CSound_Explode_Effect::Collision_Exit(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CSound_Explode_Effect::EffectStartCallBack()
{
	m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Enemy_Explode", m_fEffectAlpha);
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Enemy_Explode");
}

void CSound_Explode_Effect::EffectEndCallBack()
{
}

void CSound_Explode_Effect::EffectSoftEnd()
{
}

void CSound_Explode_Effect::EffectHardEnd()
{
}

HRESULT CSound_Explode_Effect::Ready_Components()
{
	return S_OK;
}

shared_ptr<CSound_Explode_Effect> CSound_Explode_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSound_Explode_Effect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSound_Explode_Effect(pDevice, pContext)
		{}
	};

	shared_ptr<CSound_Explode_Effect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Explode_Effect"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CSound_Explode_Effect::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSound_Explode_Effect
	{
		MakeSharedEnabler(const CSound_Explode_Effect& rhs) : CSound_Explode_Effect(rhs) {}
	};

	shared_ptr<CSound_Explode_Effect> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSound_Explode_Effect"));
		assert(false);
	}

	return pInstance;
}

void CSound_Explode_Effect::Free()
{
}