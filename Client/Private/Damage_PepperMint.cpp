#include "Damage_PepperMint.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CDamage_PepperMint::ObjID = TEXT("CDamage_PepperMint");

CDamage_PepperMint::CDamage_PepperMint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CDamage_PepperMint::ObjID);
}

CDamage_PepperMint::CDamage_PepperMint(const CDamage_PepperMint& rhs)
	: CEffect_Base(rhs)
{
}

CDamage_PepperMint::~CDamage_PepperMint()
{
	Free();
}

HRESULT CDamage_PepperMint::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamage_PepperMint::Initialize(void* pArg)
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

void CDamage_PepperMint::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CDamage_PepperMint::Tick(_float fTimeDelta)
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

void CDamage_PepperMint::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
			GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PLAYER, m_pColliderCom);
			//	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CDamage_PepperMint::Render()
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

void CDamage_PepperMint::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() != CHAI_COLLIDER_HIT) {
		DamageInfo damageInfo = {};
		damageInfo.eAttackType = EAttackType::Peppermint;

		if (GetUseGlow())
			damageInfo.eAttackType = EAttackType::Macaron;

		if (GetUseDistortion())
			damageInfo.eAttackType = EAttackType::Korsica;


		damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
		damageInfo.fAmountDamage = 3.f;
		pOther->Get_Owner()->TakeDamage(damageInfo);
	}
}

void CDamage_PepperMint::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{

}

void CDamage_PepperMint::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CDamage_PepperMint::EffectStartCallBack()
{

}

void CDamage_PepperMint::EffectEndCallBack()
{

}

void CDamage_PepperMint::EffectSoftEnd()
{

}

void CDamage_PepperMint::EffectHardEnd()
{

}

HRESULT CDamage_PepperMint::Ready_Components()
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

shared_ptr<CDamage_PepperMint> CDamage_PepperMint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CDamage_PepperMint
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDamage_PepperMint(pDevice, pContext)
		{}
	};


	shared_ptr<CDamage_PepperMint> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDamage_PepperMint"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CDamage_PepperMint::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CDamage_PepperMint
	{
		MakeSharedEnabler(const CDamage_PepperMint& rhs) : CDamage_PepperMint(rhs) {}
	};

	shared_ptr<CDamage_PepperMint> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDamage_PepperMint"));
		assert(false);
	}

	return pInstance;
}

void CDamage_PepperMint::Free()
{
}