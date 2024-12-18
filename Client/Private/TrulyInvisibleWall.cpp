#include "TrulyInvisibleWall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Texture.h"
#include "Beat_Manager.h"
#include "VIBuffer_Rect.h"
#include "CombatManager.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CTrulyInvisibleWall::ObjID = TEXT("CTrulyInvisibleWall");

CTrulyInvisibleWall::CTrulyInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CTrulyInvisibleWall::CTrulyInvisibleWall(const CTrulyInvisibleWall& rhs)
	: CGimmickBase{ rhs }
{
}

CTrulyInvisibleWall::~CTrulyInvisibleWall()
{
	Free();
}

void CTrulyInvisibleWall::Execute(_bool bExecute)
{
	m_bExecuted = bExecute;
	if (m_bExecuted)
	{
		_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();

		_matrix Transpose = World;
		for (_uint j = 0; j < 3; ++j)
		{
			Transpose.r[j] = XMVector3Normalize(Transpose.r[j]);
		}
		_float4x4 mat;
		XMStoreFloat4x4(&mat, Transpose);
		PxMat44 pxWorld;
		pxWorld.column0.x = mat.m[0][0];
		pxWorld.column0.y = mat.m[0][1];
		pxWorld.column0.z = mat.m[0][2];
		pxWorld.column0.w = mat.m[0][3];
		pxWorld.column1.x = mat.m[1][0];
		pxWorld.column1.y = mat.m[1][1];
		pxWorld.column1.z = mat.m[1][2];
		pxWorld.column1.w = mat.m[1][3];
		pxWorld.column2.x = mat.m[2][0];
		pxWorld.column2.y = mat.m[2][1];
		pxWorld.column2.z = mat.m[2][2];
		pxWorld.column2.w = mat.m[2][3];
		pxWorld.column3.x = mat.m[3][0];
		pxWorld.column3.y = mat.m[3][1];
		pxWorld.column3.z = mat.m[3][2];
		pxWorld.column3.w = mat.m[3][3];

		PxTransform transform{ pxWorld };

		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
	}
	else
	{
		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f,-100000.f} });
	}
	__super::m_bDone = true;
}

HRESULT CTrulyInvisibleWall::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTrulyInvisibleWall::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();

	_matrix Transpose = World;
	for (_uint j = 0; j < 3; ++j)
	{
		Transpose.r[j] = XMVector3Normalize(Transpose.r[j]);
	}
	_float4x4 mat;
	XMStoreFloat4x4(&mat, Transpose);
	PxMat44 pxWorld;
	pxWorld.column0.x = mat.m[0][0];
	pxWorld.column0.y = mat.m[0][1];
	pxWorld.column0.z = mat.m[0][2];
	pxWorld.column0.w = mat.m[0][3];
	pxWorld.column1.x = mat.m[1][0];
	pxWorld.column1.y = mat.m[1][1];
	pxWorld.column1.z = mat.m[1][2];
	pxWorld.column1.w = mat.m[1][3];
	pxWorld.column2.x = mat.m[2][0];
	pxWorld.column2.y = mat.m[2][1];
	pxWorld.column2.z = mat.m[2][2];
	pxWorld.column2.w = mat.m[2][3];
	pxWorld.column3.x = mat.m[3][0];
	pxWorld.column3.y = mat.m[3][1];
	pxWorld.column3.z = mat.m[3][2];
	pxWorld.column3.w = mat.m[3][3];

	PxTransform transform{ pxWorld };

	m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
	return S_OK;
}

void CTrulyInvisibleWall::PriorityTick(_float fTimeDelta)
{
}

void CTrulyInvisibleWall::Tick(_float fTimeDelta)
{
	
}

void CTrulyInvisibleWall::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
	if (m_bExecuted || m_pGameInstance.lock()->Get_Current_LevelID() == LEVEL_MAPTOOL)
		GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
#endif
}

HRESULT CTrulyInvisibleWall::Render()
{

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CTrulyInvisibleWall::Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CTrulyInvisibleWall::Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CTrulyInvisibleWall::Edit_Attribute()
{
}

HRESULT CTrulyInvisibleWall::Ready_Components()
{

	_float3 vScale = m_pTransformCom->Get_Scale();

	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"),
	wstring(TEXT("Com_PhsyXCollider"))));
	string strTag = m_strGimmickID;
	_float3 vModelSize = { vScale.x, vScale.y, vScale.z };

	PxVec3 BoxDimension{ vModelSize.x, vModelSize.y, vModelSize.z };
	PxBoxGeometry Box{ BoxDimension };
	auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, strTag.c_str());

	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vExtents = { vScale.x, vScale.y, vScale.z };
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));


	if (!m_pPhysXCollider)
		assert(false);

	return S_OK;
}

HRESULT CTrulyInvisibleWall::Bind_ShaderResources()
{
	return S_OK;
}

void CTrulyInvisibleWall::Move(_float fTimeDelta)
{
}

shared_ptr<CTrulyInvisibleWall> CTrulyInvisibleWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CTrulyInvisibleWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTrulyInvisibleWall(pDevice, pContext)
		{}
	};

	shared_ptr<CTrulyInvisibleWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTrulyInvisibleWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CTrulyInvisibleWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CTrulyInvisibleWall
	{
		MakeSharedEnabler(const CTrulyInvisibleWall& rhs) : CTrulyInvisibleWall(rhs)
		{}
	};

	shared_ptr<CTrulyInvisibleWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTrulyInvisibleWall"));
		assert(false);
	}

	return pInstance;
}

void CTrulyInvisibleWall::Free()
{
}
