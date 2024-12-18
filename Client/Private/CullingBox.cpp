#include "CullingBox.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

#include "MapManager.h"

wstring CCullingBox::ObjID = TEXT("CCullingBox");
CCullingBox::CCullingBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CCullingBox::CCullingBox(const CCullingBox& rhs)
    : CGameObject{rhs}
{
}

CCullingBox::~CCullingBox()
{
    Free();
}

HRESULT CCullingBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCullingBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CULLING_DESC* pDesc = static_cast<CULLING_DESC*>(pArg);
	m_bBoarder = pDesc->bBoarder;

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
	aabbDesc.vExtents = { 1.f, 1.f, 1.f };
	aabbDesc.vCenter = { 0.f, 0.f, 0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

	assert(m_pColliderCom);

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMat);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

    return S_OK;
}

void CCullingBox::PriorityTick(_float fTimeDelta)
{
}

void CCullingBox::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CCullingBox::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CCullingBox::Render()
{
#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

    return S_OK;
}

void CCullingBox::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = true;
}

void CCullingBox::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = true;
}

void CCullingBox::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = false;
}

_bool CCullingBox::IsIn_Bounding(_fvector vPosition)
{
	return m_pColliderCom->PointIntersect(vPosition);
}

shared_ptr<CCullingBox> CCullingBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CCullingBox
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCullingBox(pDevice, pContext)
		{}
	};

	shared_ptr<CCullingBox> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCullingBox"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CCullingBox::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CCullingBox
	{
		MakeSharedEnabler(const CCullingBox& rhs) : CCullingBox(rhs)
		{}
	};


	shared_ptr<CCullingBox> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCullingBox"));
		assert(false);
	}

	return pInstance;
}

void CCullingBox::Free()
{
}
