#include "PhysXCollider.h"
#include "Transform.h"
#include "GameInstance.h"

CPhysXCollider::CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhysXCollider::CPhysXCollider(const CPhysXCollider& rhs)
	: CComponent(rhs),
	m_pPhysics{ rhs.m_pPhysics },
	m_pCurrentScene{ rhs.m_pCurrentScene }
{
}

CPhysXCollider::~CPhysXCollider()
{
	Free();
}

HRESULT CPhysXCollider::Initialize_Prototype()
{
	m_pPhysics = CGameInstance::Get_Instance()->Get_Physics();
	m_pCurrentScene = CGameInstance::Get_Instance()->Get_Scene();

	return S_OK;
}

HRESULT CPhysXCollider::Initialize(void* pArg)
{
	m_pHitReport = new CharacterControllerHitReport();
	return S_OK;
}

void CPhysXCollider::Synchronize_Transform(CTransform* pTransform, _fvector In_vOffset)
{
	if (m_eColliderType == RIGIDBODY) {
		PxVec3 vPos = m_pRigidDynamic->getGlobalPose().p;
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
	}
	else if (m_eColliderType == CCT) {
		PxExtendedVec3 vPos = m_pController->getPosition();
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet((float)vPos.x, (float)vPos.y, (float)vPos.z, 1.f));
	}
}

void CPhysXCollider::Synchronize_Transform_Position(CTransform* pTransform)
{
	if (m_eColliderType == RIGIDBODY) {
		PxVec3 vPos = m_pRigidDynamic->getGlobalPose().p;
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
	}
	else if (m_eColliderType == CCT) {
		PxExtendedVec3 vPos = m_pController->getPosition();
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet((float)vPos.x, (float)vPos.y, (float)vPos.z, 1.f));
	}
	_float3 vPos = {};
	XMStoreFloat3(&vPos, pTransform->Get_State(CTransform::STATE_LOOK));
	PxVec3 vLook = PxVec3(vPos.x, vPos.y, vPos.z);
	Set_LookVector(vLook);

}

void CPhysXCollider::Synchronize_Transform_Rotation(CTransform* pTransform)
{
	//PxQuat vQuat = m_pRigidDynamic->getGlobalPose().q;
	//pTransform->Set_State(CTransform::STATE_QUATERNION, XMVectorSet(vQuat.x, vQuat.y, vQuat.z, vQuat.w));
}

void CPhysXCollider::checkCollisionFlags(PxU32 collisionFlags)
{
	m_bCollisionDown = false;
	m_bCollisionUp = false;
	m_bCollisionSides = false;
	m_bCollisionLeft = false;
	m_bCollisionRight = false;

	if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
		m_bCollisionDown = true;
	}
	if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_UP) {
		m_bCollisionUp = true;
	}
	if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_SIDES) {
		m_bCollisionSides = true;

		if (m_pHitReport->GetLeftHit() == true) {
			m_bCollisionLeft = true;
		}
		else if (m_pHitReport->GetRightHit() == true) {
			m_bCollisionRight = true;
		}

	}

	if (m_bCollisionDown || m_bCollisionUp || m_bCollisionSides)
		m_bCollision = true;
	else
		m_bCollision = false;

}

PxController* CPhysXCollider::createCharacterController(PxControllerManager* manager, const PxVec3& position)
{
	PxCapsuleControllerDesc desc;
	desc.height = 2.0f;
	desc.radius = 0.5f;
	desc.position = PxExtendedVec3(position.x, position.y, position.z);
	desc.material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	desc.reportCallback = m_pHitReport;

	m_pController = manager->createController(desc);

	if (m_pController == nullptr)
	{
		MSG_BOX(TEXT("Failed to create Character Controller"));
		assert(false);
	}
	m_eColliderType = CCT;
	return m_pController;
}

void CPhysXCollider::Add_To_Scene()
{
	if (m_pRigidStatic && !m_pRigidStatic->getScene())
	{
		m_pCurrentScene->addActor(*m_pRigidStatic);
	}
	if (m_pRigidDynamic && !m_pRigidDynamic->getScene())
	{
		m_pCurrentScene->addActor(*m_pRigidDynamic);
	}
}

void CPhysXCollider::Remove_To_Scene()
{
	if (m_pRigidDynamic != nullptr && m_pRigidDynamic->getScene())
	{
		m_pCurrentScene->removeActor(*m_pRigidDynamic);
	}

	if (m_pRigidStatic != nullptr && m_pRigidStatic->getScene())
	{
		m_pCurrentScene->removeActor(*m_pRigidStatic);
	}

}

PxTriangleMesh* CPhysXCollider::CreateTriangleMesh(PxVec3* vertices, PxU32* indices, PxU32 vertexCount, PxU32 triangleCount)
{
	PxCookingParams params = PxCookingParams(PxTolerancesScale());

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vertexCount;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices;
	meshDesc.triangles.count = triangleCount;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	return m_pPhysics->createTriangleMesh(readBuffer);
}

shared_ptr<CPhysXCollider> CPhysXCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedPtr : public CPhysXCollider
	{
		MakeSharedPtr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPhysXCollider(pDevice, pContext)
		{};
	};
	shared_ptr<CPhysXCollider> pInstance = make_shared<MakeSharedPtr>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CComponent> CPhysXCollider::Clone(void* pArg)
{
	struct MakeSharedPtr : public CPhysXCollider
	{
		MakeSharedPtr(const CPhysXCollider& rhs) : CPhysXCollider(rhs)
		{};
	};

	shared_ptr<CPhysXCollider> pInstance = make_shared<MakeSharedPtr>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModel"));
		assert(false);
	}

	return pInstance;
}

PxRigidStatic* CPhysXCollider::createStaticMeshActor(PxTriangleMesh* triangleMesh, const char* PhysXTag)
{
	PxMaterial* material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	PxTriangleMeshGeometry meshGeometry(triangleMesh);
	if(!m_pRigidStatic)
		m_pRigidStatic = m_pPhysics->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));

	PxShape* shape = m_pPhysics->createShape(meshGeometry, *material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	m_pRigidStatic->attachShape(*shape);
	m_pRigidStatic->setName(PhysXTag);
	m_pCurrentScene->addActor(*m_pRigidStatic);
	return m_pRigidStatic;
}

PxRigidStatic* CPhysXCollider::createStaticShapeActor(const PxGeometry& geometry, const char* PhysXTag)
{
	PxMaterial* material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	if (!m_pRigidStatic)
		m_pRigidStatic = m_pPhysics->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
	
	PxShape* shape = m_pPhysics->createShape(geometry, *material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	m_pRigidStatic->attachShape(*shape);
	m_pCurrentScene->addActor(*m_pRigidStatic);
	return m_pRigidStatic;
}

PxRigidStatic* CPhysXCollider::createStaticMeshActor(PxTriangleMesh* triangleMesh, PxVec3 Scale, const char* PhysXTag)
{
	PxMaterial* material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	PxTriangleMeshGeometry meshGeometry(triangleMesh);
	if(!m_pRigidStatic)
		m_pRigidStatic = m_pPhysics->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
	meshGeometry.scale = Scale;
	PxShape* shape = m_pPhysics->createShape(meshGeometry, *material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	m_pRigidStatic->attachShape(*shape);
	m_pRigidStatic->setName(PhysXTag);
	m_pCurrentScene->addActor(*m_pRigidStatic);
	return m_pRigidStatic;
}

PxRigidDynamic* CPhysXCollider::createDynamicMeshActor(PxTriangleMesh* triangleMesh, const char* PhysXTag)
{
	PxMaterial* material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	PxTriangleMeshGeometry meshGeometry(triangleMesh);
	if (!m_pRigidDynamic)
		m_pRigidDynamic = m_pPhysics->createRigidDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
	PxShape* shape = m_pPhysics->createShape(meshGeometry, *material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	m_pRigidDynamic->attachShape(*shape);
	m_pRigidDynamic->setName(PhysXTag);
	m_pCurrentScene->addActor(*m_pRigidDynamic);
	return m_pRigidDynamic;
}

PxRigidDynamic* CPhysXCollider::createDynamicShapeActor(const PxGeometry& geometry, const char* PhysXTag)
{
	PxMaterial* material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	if (!m_pRigidDynamic)
		m_pRigidDynamic = m_pPhysics->createRigidDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
	PxShape* shape = m_pPhysics->createShape(geometry, *material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	m_pRigidDynamic->attachShape(*shape);
	m_pRigidDynamic->setName(PhysXTag);
	m_pCurrentScene->addActor(*m_pRigidDynamic);
	return m_pRigidDynamic;
}

PxRigidDynamic* CPhysXCollider::createDynamicActor(const char* PhysXTag)
{
	PxMaterial* material = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	PxRigidDynamic* player = m_pPhysics->createRigidDynamic(PxTransform(PxVec3(60.f, 20.f, -55.f)));
	PxShape* shape = m_pPhysics->createShape(PxCapsuleGeometry(0.5f, 1.8f), *material);
	//shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	player->attachShape(*shape);
	player->setMass(1.0f);
	player->setName(PhysXTag);
	m_pCurrentScene->addActor(*player);

	m_pRigidDynamic = player;
	m_eColliderType = RIGIDBODY;
	return player;
}


void CPhysXCollider::Free()
{
	if (m_pRigidDynamic != nullptr)
		m_pCurrentScene->removeActor(*m_pRigidDynamic);

	if (m_pRigidStatic != nullptr)
		m_pCurrentScene->removeActor(*m_pRigidStatic);

	if(m_pController)
		m_pController->release();
	Safe_Delete(m_pHitReport);
	//m_pController->release();
}
