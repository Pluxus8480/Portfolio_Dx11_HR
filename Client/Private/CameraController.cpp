#include "CameraController.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"
#include "PickUpItem.h"
#include "PlayerManager.h"
#include "Player_Camera.h"

#include "Beat_Manager.h"

wstring CCameraController::ObjID = TEXT("CCameraController");

CCameraController::CCameraController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CCameraController::CCameraController(const CCameraController& rhs)
	: CGimmickBase{ rhs }
{
}

CCameraController::~CCameraController()
{
	Free();
}

void CCameraController::Execute(_bool bExecute)
{
	m_bExecuted = bExecute;
}

HRESULT CCameraController::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));



	return S_OK;
}

HRESULT CCameraController::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CCameraController::PriorityTick(_float fTimeDelta)
{
}

void CCameraController::Tick(_float fTimeDelta)
{

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();

	m_pColliderCom->Update(Transpose);
}

void CCameraController::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CCameraController::Render()
{
#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif
	return S_OK;
}

void CCameraController::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == CHAI_COLLIDER_HIT && !m_bUsedUp)
	{
		m_bUsedUp = true;
		if (m_bEnder)
		{
			CPlayer_Manager::Get_Instance()->End_2DScroll();
		}
		else
		{
			CPlayer_Manager::Get_Instance()->Start_2DScroll(m_vLook, m_fDistance);
		}
	}
}

void CCameraController::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CCameraController::Edit_Attribute()
{
	ImGui::InputFloat3("Look", &m_vLook.x);
	ImGui::InputFloat("Distance", &m_fDistance);
	ImGui::InputInt("Ender", &m_bEnder);
}

void CCameraController::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
	write_typed_data(os, m_vLook);
	write_typed_data(os, m_fDistance);
	write_typed_data(os, m_bEnder);
}

void CCameraController::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
	read_typed_data(is, m_vLook);
	read_typed_data(is, m_fDistance);
	read_typed_data(is, m_bEnder);
}

void CCameraController::Push_From_Pool(void* pArg)
{

}

void CCameraController::Dying()
{

}

void CCameraController::TakeDamage(const DamageInfo& damageInfo)
{
}

HRESULT CCameraController::Ready_Components()
{
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vExtents = { .5f, .5f, .5f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	assert(m_pColliderCom);


	return S_OK;
}

HRESULT CCameraController::Bind_ShaderResources()
{
	return S_OK;
}

shared_ptr<CCameraController> CCameraController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CCameraController
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCameraController(pDevice, pContext)
		{}
	};

	shared_ptr<CCameraController> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCameraController"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CCameraController::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CCameraController
	{
		MakeSharedEnabler(const CCameraController& rhs) : CCameraController(rhs)
		{}
	};

	shared_ptr<CCameraController> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCameraController"));
		assert(false);
	}

	return pInstance;
}

void CCameraController::Free()
{
}
