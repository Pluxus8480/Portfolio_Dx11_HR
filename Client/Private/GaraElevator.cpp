#include "GaraElevator.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"
#include "Chai.h"
#include "CharacterController.h"
#include "PlayerManager.h"	
#include "GameManager.h"

wstring CGaraElevator::ObjID = TEXT("CGaraElevator");
CGaraElevator::CGaraElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{pDevice, pContext}
{
}

CGaraElevator::CGaraElevator(const CGaraElevator& rhs)
	: CGimmickBase{rhs}
{
}

CGaraElevator::~CGaraElevator()
{
	Free();
}

void CGaraElevator::Execute(_bool bExecute)
{
	m_bExecuted = true;

	//_vector vInitialPos = m_pTransformCom->Get_Position();
	//for (_int i = 0; i < 5; ++i)
	//	m_fPositionY[i] += XMVectorGetY(vInitialPos);
}

HRESULT CGaraElevator::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.4f, 0.4f, 0.4f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Elevator"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_Elevator.FBX"),
			MODEL_TYPE::NONANIM, fMat)
	);

	XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(-90.f)));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_FloorSwitch_Off"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_FloorSwitchA_Off_IZ_01.FBX"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_Catwalk_Bridge1", L"../Bin/Resources/Models/Map/Gimmick/Sound/Catwalk_Bridge1.ogg", SOUND_CHANNEL::SOUND_EFFECT));

	return S_OK;
}

HRESULT CGaraElevator::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();

	m_vLastPos = _float3(pDesc->worldMatrix._41, pDesc->worldMatrix._42, pDesc->worldMatrix._43);

	if (GAMEMANAGER->Get_Current_Level() != LEVEL_MAPTOOL)
		m_pTransformCom->Set_Position(XMVectorSet(m_vLastPos.x, m_vLastPos.y + 11.f, m_vLastPos.z, 1.f));

	return S_OK;
}

void CGaraElevator::PriorityTick(_float fTimeDelta)
{
}

void CGaraElevator::Tick(_float fTimeDelta)
{
	//if (GAMEINSTANCE->Get_KeyDown(DIK_L))
	//	Execute();
	auto pGI = m_pGameInstance.lock();
	m_bCulled = !pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 7.f);
	_bool bRange = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - pGI->Get_CamPosition_Vector())) > 5.f;
	if (m_bExecuted)
	{

		_vector vCurPosition = m_pTransformCom->Get_Position();
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

		m_fTimer += fTimeDelta;
		if (m_bStop)
		{
			if (m_fTimer > fBeat * 0.5f)
			{
				m_bStop = false;
				m_fTimer = 0.f;
				if(!m_bCulled && !m_bRest && bRange)
					pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Catwalk_Bridge1", m_pTransformCom->Get_Position());
			}
		}
		else if (m_bRest)
		{
			if (m_fTimer > fBeat * 2.f)
			{
				if (!m_bCulled && bRange)
					pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Catwalk_Bridge1", m_pTransformCom->Get_Position());
				m_bRest = false;
				m_fTimer = 0.f;
			}
		}
		else if (m_fTimer > fBeat * 0.5f /*&& m_bDown*/)
		{
			m_iCount++;
			m_bStop = true;
			m_fTimer = 0.f;

			if (m_iCount > 1)
			{
				PLAYERMANAGER->Callback_Event("Fade_Elevator_End");
				//m_bDown = false;
				//m_bRest = true;
				m_pTransformCom->Set_Position(XMLoadFloat3(&m_vLastPos));
				CGimmickBase::Done();
			}
		}

		if (!m_bTest)
			 Move(fTimeDelta);
	}

	else
	{
		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 300,-100000.f} });
	}

	if(m_bCollision)
	{
		auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));

		_float3 vVelocity = pCCT->Get_Velocity();
		auto vPlayerPos = pCCT->Get_Controller()->getPosition();

		_float fTopY = XMVectorGetY(m_pTransformCom->Get_Position());

		if (vVelocity.y <= 0)
		{
			if (m_bSwitchCollision)
				fTopY += 0.2f;

			_vector vFootPos = XMVectorSet(vPlayerPos.x, fTopY, vPlayerPos.z, 1.f);

			//pCCT->Set_Position(XMMatrixTranslationFromVector(vFootPos));
			pCCT->Set_FootPosition(XMMatrixTranslationFromVector(vFootPos));
			dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Transform()->Set_Position(vFootPos);
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

	PxTransform transform{ pxWorld };
	m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

	Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
	Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vSwitchCenter.x, m_vSwitchCenter.y, m_vSwitchCenter.z), Transpose);
	Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
	Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
	Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
	XMStoreFloat4x4(&mat, Transpose);
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

	auto Switchtransform = PxTransform{ pxWorld };
	m_pSwitchPhysX->Get_Static_RigidBody()->setGlobalPose(Switchtransform);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pSwitchCollider->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CGaraElevator::LateTick(_float fTimeDelta)
{
	if (m_bCulled)
		return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pSwitchCollider);
}

HRESULT CGaraElevator::Render()
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

	iNumMeshes = m_pSwitchModel->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pSwitchModel->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//	return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pSwitchModel->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();

	if (m_pSwitchCollider)
		m_pSwitchCollider->Render();
#endif

	return S_OK;
}

void CGaraElevator::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (!m_bDone && pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		Execute();
	}
}

void CGaraElevator::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		m_bCollision = true;		

		if (pCaller == m_pSwitchCollider)
			m_bSwitchCollision = true;
	}
}

void CGaraElevator::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		m_bCollision = false;

		if (pCaller == m_pSwitchCollider)
			m_bSwitchCollision = false;
	}
}

void CGaraElevator::Edit_Attribute()
{
}

HRESULT CGaraElevator::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Elevator"), TEXT("Model_Com")));
	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
	aabbDesc.vExtents = { 4.f, 5.f, 4.f };
	aabbDesc.vCenter = { 0.f, 0.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

	//const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Elevator"))->Get_ModelData();
	//m_vModelCenter = pModelData.get()->VertexInfo.vCenter;
	//_vector vModelSize = XMLoadFloat3(&pModelData->VertexInfo.vMax) - XMLoadFloat3(&pModelData->VertexInfo.vMin);
	//XMStoreFloat3(&m_vModelSize, vModelSize);
	//m_vModelSize.x *= 0.01f;
	//m_vModelSize.y *= 0.01f;
	//m_vModelSize.z *= 0.01f;
	//m_vModelCenter.x *= 0.01f;
	//m_vModelCenter.y *= 0.01f;
	//m_vModelCenter.z *= 0.01f;
	
	string strTag = m_strGimmickID;
	m_vModelSize = { 4.f, 0.2f, 4.f };
	m_vModelCenter = { 0.f, -0.1f, 0.f };
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
	PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
	PxBoxGeometry Box{ BoxDimension };
	auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, strTag.c_str());

	assert(pRigidBody);

	//_matrix mat = m_pTransformCom->Get_WorldMatrix_XMMat();
	//_float4x4 fMat{};
	//XMStoreFloat4x4(&fMat, mat);
	//_uint    iNumMeshes = m_pModelCom->Get_NumMeshes();
	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	string tmp = strTag + to_string(i);
	//	auto pMesh = m_pModelCom->Get_Mesh((_uint)i);
	//	_vector vScale, vRot, vPos;
	//	XMMatrixDecompose(&vScale, &vRot, &vPos, mat);
	//	_float3 scale;
	//	XMStoreFloat3(&scale, vScale);
	//	PxVec3 pscale;
	//	pscale.x = scale.x;
	//	pscale.y = scale.y;
	//	pscale.z = scale.z;
	//	pMesh->Cooking_TriangleMesh(fMat);
	//	auto pTriangleMesh = pMesh->Get_TriangleMesh();
	//	if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
	//		m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
	//}

	if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
		assert(false);

	m_pSwitchModel = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FloorSwitch_Off"), TEXT("SwitchModel_Com")));
	assert(m_pSwitchModel);

	m_pSwitchPhysX = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_SwitchPhysX")));
	strTag = m_strGimmickID + "Switch";
	_float3 vSwitchSize = { 1.5f, 0.1f, 1.5f };
	m_vSwitchCenter = { 0.f, +0.1f, 0.f };
	vSwitchSize.x *= vWorldSize.x;
	vSwitchSize.y *= vWorldSize.y;
	vSwitchSize.z *= vWorldSize.z;
	m_vSwitchCenter.x *= vWorldSize.x;
	m_vSwitchCenter.y *= vWorldSize.y;
	m_vSwitchCenter.z *= vWorldSize.z;
	BoxDimension = PxVec3{ vSwitchSize.x, vSwitchSize.y, vSwitchSize.z };
	PxBoxGeometry SwitchBox{ BoxDimension };
	auto pSwitchRigid= m_pSwitchPhysX->createStaticShapeActor(SwitchBox, strTag.c_str());

	assert(pSwitchRigid);

	aabbDesc.vExtents = { 1.5f, 3.f, 1.5f };
	aabbDesc.vCenter = { 0.f, 3.f, 0.f };

	m_pSwitchCollider = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("SwitchCollider_Com"), &aabbDesc));

	return S_OK;
}

HRESULT CGaraElevator::Bind_ShaderResources()
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

void CGaraElevator::Move(_float fTimeDelta)
{
	if (m_bStop || m_bRest)
		return;
	_vector vCurPosition = m_pTransformCom->Get_Position();
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

	if (m_bDown)
		m_pTransformCom->Go_Down(fTimeDelta / fBeat);
	//else
	//	m_pTransformCom->Go_Up(fTimeDelta / fBeat);
}

shared_ptr<CGaraElevator> CGaraElevator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CGaraElevator
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGaraElevator(pDevice, pContext)
		{}
	};

	shared_ptr<CGaraElevator> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGaraElevator"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CGaraElevator::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CGaraElevator
	{
		MakeSharedEnabler(const CGaraElevator& rhs) : CGaraElevator(rhs)
		{}
	};

	shared_ptr<CGaraElevator> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGaraElevator"));
		assert(false);
	}

	return pInstance;
}

void CGaraElevator::Free()
{
}
