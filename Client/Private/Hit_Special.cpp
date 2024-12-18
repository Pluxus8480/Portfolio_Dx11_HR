#include "Hit_Special.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CHit_Special::ObjID = TEXT("CHit_Special");

CHit_Special::CHit_Special(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CHit_Special::ObjID);
}

CHit_Special::CHit_Special(const CHit_Special& rhs)
	: CEffect_Base(rhs)
{
}

CHit_Special::~CHit_Special()
{
	Free();
}

HRESULT CHit_Special::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHit_Special::Initialize(void* pArg)
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

void CHit_Special::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CHit_Special::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);

	if (m_pParentWorld != nullptr) {
		m_pColliderCom->Update(XMMatrixMultiply(m_pTransformCom->Get_WorldMatrix_XMMat(), XMLoadFloat4x4(m_pParentWorld)));
	}
	else {
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
	}
}

void CHit_Special::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
			GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_MONSTER, m_pColliderCom);
			//	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CHit_Special::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	//#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
	//#endif

	return S_OK;
}

void CHit_Special::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == CHAI_COLLIDER_HIT) {
		DamageInfo damageInfo = {};
		damageInfo.eAttackType = EAttackType::Special;
		damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
		damageInfo.fAmountDamage = max(30.f, m_fEffectAlpha);
		if (GetUseGlow()) damageInfo.fAmountDamage = m_fEffectAlpha;
		damageInfo.pObjectOwner = shared_from_this();
		pOther->Get_Owner()->TakeDamage(damageInfo);
	}
}

void CHit_Special::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CHit_Special::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CHit_Special::EffectStartCallBack()
{
}

void CHit_Special::EffectEndCallBack()
{
}

void CHit_Special::EffectSoftEnd()
{
}

void CHit_Special::EffectHardEnd()
{
}

HRESULT CHit_Special::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vExtents = { 0.5f, 0.5f, 0.5f };
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };

	m_pColliderCom = dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CHit_Special> CHit_Special::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CHit_Special
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CHit_Special(pDevice, pContext)
		{}
	};

	shared_ptr<CHit_Special> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHit_Special"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CHit_Special::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CHit_Special
	{
		MakeSharedEnabler(const CHit_Special& rhs) : CHit_Special(rhs) {}
	};

	shared_ptr<CHit_Special> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHit_Special"));
		assert(false);
	}

	return pInstance;
}

void CHit_Special::Free()
{
}