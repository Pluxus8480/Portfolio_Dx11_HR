#include "FadeElevator.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"

#include "GameManager.h"
#include "CharacterController.h"

wstring CFadeElevator::ObjID = TEXT("CFadeElevator");
CFadeElevator::CFadeElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CFadeElevator::CFadeElevator(const CFadeElevator& rhs)
    : CGimmickBase{rhs} 
{
}

CFadeElevator::~CFadeElevator()
{
    Free();
}

void CFadeElevator::Done()
{
	m_bExecuted = false; 
	m_bDone = true;

	if (CGame_Manager::Get_Instance()->Get_Current_Level() == LEVEL_TEST)
	{
		PLAYERMANAGER->Callback_Event("Fade_Elevator");
	}
}

HRESULT CFadeElevator::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LargeElevator"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_Elevator_WB_01.FBX"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_Mach_Init", L"../Bin/Resources/Sounds/MapObject/Mach_Init.ogg", SOUND_CHANNEL::SOUND_EFFECT));

	return S_OK;
}

HRESULT CFadeElevator::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 1.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
	XMStoreFloat3(&m_vInitialPosition, m_pTransformCom->Get_Position());

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CFadeElevator::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		if (m_pOther.lock())
		{
			_matrix matWorld = m_pOther.lock()->Get_Transform()->Get_WorldMatrix_XMMat();
			_matrix InvWorld = XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix_XMMat());
			XMStoreFloat4x4(&m_ChildMat, XMMatrixMultiply(matWorld, InvWorld));
		}

		m_vFinalPosition = m_vInitialPosition;
		m_vFinalPosition.y = (m_bDown) ? (m_vFinalPosition.y - m_fMoveDistance) : (m_vFinalPosition.y + m_fMoveDistance);

		m_bInit = true;
	}
}

void CFadeElevator::Tick(_float fTimeDelta)
{
	if (m_bExecuted)
	{
		
		//m_fTimer += fTimeDelta;
		//if (m_fTimer < BEATMANAGER->Get_Beat() * 3.f)
		//	_int a = 10;
		//else
		//{
		//	if (!m_bEventTrigger)
		//	{
		//		m_bEventTrigger = true;
		//		PLAYERMANAGER->Callback_Event("Fade_Elevator");
		//	}
		//
		//	Move(fTimeDelta);
		//
		//	if (m_pOther.lock())	// Floor Switch
		//		m_pOther.lock()->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&m_ChildMat) * m_pTransformCom->Get_WorldMatrix_XMMat());
		//}

		Move(fTimeDelta);

		if (m_pOther.lock())	// Floor Switch
			m_pOther.lock()->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&m_ChildMat) * m_pTransformCom->Get_WorldMatrix_XMMat());

		if (m_bCollision)
		{
			auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));

			_float3 vVelocity = pCCT->Get_Velocity();
			auto vPlayerPos = pCCT->Get_Controller()->getPosition();

			_float fTopY = XMVectorGetY(m_pTransformCom->Get_Position());

			if (vVelocity.y <= 0)
			{
				_vector vFootPos = XMVectorSet(vPlayerPos.x, fTopY, vPlayerPos.z, 1.f);

				//pCCT->Set_Position(XMMatrixTranslationFromVector(vFootPos));
				pCCT->Set_FootPosition(XMMatrixTranslationFromVector(vFootPos));
				dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Transform()->Set_Position(vFootPos);
			}
		}

	}

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
	Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
	Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
	Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
	Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
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

	PxTransform px = m_pPhysXCollider->Get_Static_RigidBody()->getGlobalPose();
	PxTransform transform{ pxWorld };
	m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CFadeElevator::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f))
		return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	pGI->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CFadeElevator::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//	return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

    return S_OK;
}

void CFadeElevator::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = true;
}

void CFadeElevator::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = true;
}

void CFadeElevator::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		//auto pCCT = dynamic_pointer_cast<CCharacterController>(pOther->Get_Owner()->Find_Component(L"Com_CCT"));
		//
		//_float3 vVelocity = pCCT->Get_Velocity();
		//auto vPlayerPos = pCCT->Get_Controller()->getPosition();
		//
		////_vector vPlayerPos = pOther->Get_Owner()->Get_Transform()->Get_Position();
		//_float fTopY = XMVectorGetY(m_pTransformCom->Get_Position());
		//
		//if (vVelocity.y <= 0)
		//{
		//	_vector vFootPos = XMVectorSet(vPlayerPos.x, fTopY, vPlayerPos.z, 1.f);
		//
		//	//pCCT->Set_Position(XMMatrixTranslationFromVector(vFootPos));
		//	pCCT->Set_FootPosition(XMMatrixTranslationFromVector(vFootPos));
		//}

		m_bCollision = false;
	}
}

void CFadeElevator::Edit_Attribute()
{
	ImGui::InputFloat("Move Distance", &m_fMoveDistance);
	ImGui::InputFloat("Move Speed", &m_fMoveSpeed);
	ImGui::Checkbox("Down?", &m_bDown);
}

void CFadeElevator::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	write_typed_data(os, m_bDown);
	write_typed_data(os, m_fMoveDistance);
	write_typed_data(os, m_fMoveSpeed);
}

void CFadeElevator::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	read_typed_data(is, m_bDown);
	read_typed_data(is, m_fMoveDistance);
	read_typed_data(is, m_fMoveSpeed);
}

void CFadeElevator::Execute(_bool bExecute)
{
	__super::Execute(bExecute);
	m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Mach_Init", m_pTransformCom->Get_Position());
}

HRESULT CFadeElevator::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LargeElevator"), TEXT("Model_Com")));
	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
	aabbDesc.vExtents = { 5.f, 5.f, 5.f };
	aabbDesc.vCenter = { 0.f, 0.5f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_LargeElevator"))->Get_ModelData();
	m_vModelCenter = pModelData.get()->VertexInfo.vCenter;
	_vector vModelSize = XMLoadFloat3(&pModelData->VertexInfo.vMax) - XMLoadFloat3(&pModelData->VertexInfo.vMin);
	XMStoreFloat3(&m_vModelSize, vModelSize);
	
	string strTag = m_strGimmickID;
	_float3 vWorldSize{};
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix_XMMat();
	vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
	vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
	vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));
	m_vModelSize.x *= vWorldSize.x;
	m_vModelSize.y *= vWorldSize.y;
	m_vModelSize.z *= vWorldSize.z;
	m_vModelCenter.x *= vWorldSize.x;
	m_vModelCenter.y *= vWorldSize.y;
	m_vModelCenter.z *= vWorldSize.z;
	m_vModelSize = { 5.f, 0.3f, 5.f };
	m_vModelCenter = { 0.f, -0.3f, 0.f };
	PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
	PxBoxGeometry Box{ BoxDimension };
	auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, strTag.c_str());

	//assert(pRigidBody);

	//_matrix mat = m_pTransformCom->Get_WorldMatrix_XMMat();
	//_float3 fScale = m_pTransformCom->Get_Scale();
	//_float4x4 fMat{};
	//XMStoreFloat4x4(&fMat, XMMatrixScaling(fScale.x, fScale.y, fScale.z));
	//
	//auto pMesh = m_pModelCom->Get_Mesh((_uint)2);
	//
	//_vector vScale, vRot, vPos;
	//XMMatrixDecompose(&vScale, &vRot, &vPos, mat);
	//_float3 scale;
	//XMStoreFloat3(&scale, vScale);
	//PxVec3 pscale;
	//pscale.x = scale.x;
	//pscale.y = scale.y;
	//pscale.z = scale.z;
	//pMesh->Cooking_TriangleMesh(fMat);
	//auto pTriangleMesh = pMesh->Get_TriangleMesh();
	//if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
	//	m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, strTag.c_str());

	if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
		assert(false);

    return S_OK;
}

HRESULT CFadeElevator::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//    return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

    return S_OK;
}

void CFadeElevator::Move(_float fTimeDelta)
{
	_vector vCurPosition = m_pTransformCom->Get_Position();

	if (m_bDown)
	{
		if (m_vFinalPosition.y > XMVectorGetY(vCurPosition)
			|| fabs(m_vFinalPosition.y - XMVectorGetY(vCurPosition)) < 0.5f)
		{
			m_pTransformCom->Set_Position(XMLoadFloat3(&m_vFinalPosition));
			Done();
		}

		else
			m_pTransformCom->Go_Down(fTimeDelta * m_fMoveSpeed);
	}

	else
	{
		if (m_vFinalPosition.y < XMVectorGetY(vCurPosition)
			|| fabs(m_vFinalPosition.y - XMVectorGetY(vCurPosition)) < 0.5f)
		{
			m_pTransformCom->Set_Position(XMLoadFloat3(&m_vFinalPosition));
			Done();
		}

		else
			m_pTransformCom->Go_Up(fTimeDelta * m_fMoveSpeed);
	}
}

shared_ptr<CFadeElevator> CFadeElevator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFadeElevator
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFadeElevator(pDevice, pContext)
		{}
	};

	shared_ptr<CFadeElevator> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFadeElevator"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CFadeElevator::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFadeElevator
	{
		MakeSharedEnabler(const CFadeElevator& rhs) : CFadeElevator(rhs)
		{}
	};

	shared_ptr<CFadeElevator> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFadeElevator"));
		assert(false);
	}

	return pInstance;
}

void CFadeElevator::Free()
{
}
