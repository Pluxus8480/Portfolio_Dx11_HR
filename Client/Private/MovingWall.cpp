#include "MovingWall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "PlayerManager.h"

#include "MapManager.h"

wstring CMovingWall::ObjID = TEXT("CMovingWall");

CMovingWall::CMovingWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CMovingWall::CMovingWall(const CMovingWall& rhs)
    : CGimmickBase{rhs}
{
}

CMovingWall::~CMovingWall()
{
    Free();
}

void CMovingWall::Execute(_bool bExecute)
{
	m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_MovingWall", m_pTransformCom->Get_Position());

	m_bExecuted = true;
}

HRESULT CMovingWall::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_MovingWall"), TEXT("../Bin/Resources/Models/Map/Gimmick/MovingWall.fbx"), 
			MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_MovingWall", L"../Bin/Resources/Sounds/MapObject/MovingWall.ogg",
		SOUND_CHANNEL::SOUND_EFFECT));


    return S_OK;
}

HRESULT CMovingWall::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

    return S_OK;
}

void CMovingWall::PriorityTick(_float fTimeDelta)
{
}

void CMovingWall::Tick(_float fTimeDelta)
{
	if (m_bExecuted && !m_bDone)
		Move(fTimeDelta);

	if (m_bDone)
	{
		_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
		Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
		//Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
		//Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
		//Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
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
		//PxTransform px = m_pPhysXCollider->Get_Static_RigidBody()->getGlobalPose();
		//int a = 0;
	}
	else
	{
		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 100000,-100000.f} });
	}
}

void CMovingWall::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	auto pGI = m_pGameInstance.lock();
	if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMovingWall::Render()
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

	return S_OK;
}

void CMovingWall::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CMovingWall::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CMovingWall::Edit_Attribute()
{
}

HRESULT CMovingWall::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MovingWall"), TEXT("Model_Com")));
	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	//const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_MovingWall"))->Get_ModelData();
	//m_vModelCenter = pModelData.get()->VertexInfo.vCenter;
	//_vector vModelSize = XMLoadFloat3(&pModelData->VertexInfo.vMax) - XMLoadFloat3(&pModelData->VertexInfo.vMin);
	//XMStoreFloat3(&m_vModelSize, vModelSize);
	//
	string strTag = m_strGimmickID;
	//_float3 vWorldSize{};
	//_matrix matWorld = m_pTransformCom->Get_WorldMatrix_XMMat();
	//vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
	//vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
	//vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));
	//m_vModelSize.x *= vWorldSize.x;
	//m_vModelSize.y *= vWorldSize.y;
	//m_vModelSize.z *= vWorldSize.z;
	//m_vModelCenter.x *= vWorldSize.x;
	//m_vModelCenter.y *= vWorldSize.y;
	//m_vModelCenter.z *= vWorldSize.z;
	m_vModelSize = { 0.2f, 2.f, 2.5f };
	m_vModelCenter = { m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
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
	//			m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
	//}

	if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
		assert(false);

	return S_OK;
}

HRESULT CMovingWall::Bind_ShaderResources()
{
	auto pGameInstance = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

void CMovingWall::Move(_float fTimeDelta)
{
	_float fRadPerSec = XMConvertToRadians(360.f);

	if (XMConvertToRadians(90.f) < m_fCurRotation)
	{
		m_pTransformCom->Rotation_Override(m_pTransformCom->Get_NormLook() * (XMConvertToRadians(90.f) - m_fCurRotation));
		m_bDone = true;
	}

	else if (fabs(XMConvertToRadians(90.f) - m_fCurRotation) <= XMConvertToRadians(5.f))
	{
		m_pTransformCom->Rotation_Override(m_pTransformCom->Get_NormLook() * (XMConvertToRadians(90.f) - m_fCurRotation));
		m_bDone = true;
	}

	else if (fabs(XMConvertToRadians(90.f) - m_fCurRotation) > XMConvertToRadians(5.f))
	{
		m_fCurRotation += fRadPerSec * fTimeDelta;
		m_pTransformCom->Turn(m_pTransformCom->Get_NormLook(), fRadPerSec, fTimeDelta);
	}
}

shared_ptr<CMovingWall> CMovingWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMovingWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMovingWall(pDevice, pContext)
		{}
	};

	shared_ptr<CMovingWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMovingWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMovingWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMovingWall
	{
		MakeSharedEnabler(const CMovingWall& rhs) : CMovingWall(rhs)
		{}
	};

	shared_ptr<CMovingWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMovingWall"));
		assert(false);
	}

	return pInstance;
}

void CMovingWall::Free()
{
}
