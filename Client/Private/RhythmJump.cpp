#include "RhythmJump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "Chai.h"
#include "CharacterController.h"
#include "PlayerManager.h"	

wstring CRhythmJump::ObjID = TEXT("CRhythmJump");
CRhythmJump::CRhythmJump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CRhythmJump::CRhythmJump(const CRhythmJump& rhs)
	: CGimmickBase{ rhs }
{
}

CRhythmJump::~CRhythmJump()
{
	Free();
}

void CRhythmJump::Execute(_bool bExecute)
{
	if (m_bExecuted == bExecute) return;

	m_bExecuted = bExecute;
	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();

	m_MovingFloorMatrix._42 = 0.f;
	XMStoreFloat4x4(&m_MovingFloorWorldMat, XMLoadFloat4x4(&m_MovingFloorMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat());
}

HRESULT CRhythmJump::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RhythmJumpBase"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_RhythmJumpBaseA_IZ_01.FBX"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RhythmJumpJoint"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_RhythmJumpBaseAJoint_IZ_01.FBX"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RhythmJumpMoving"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_RhythmJumpBaseAMoving_IZ_01.FBX"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RhythmJumpDisplay"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_RhythmJumpDisplayA_IZ_01.FBX"),
			MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_RhythmJumpDisplay_DMG"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_DMG_Rhythm_Jump.dds"))
	);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_Catwalk_Bridge1", L"../Bin/Resources/Models/Map/Gimmick/Sound/Catwalk_Bridge1.ogg", SOUND_CHANNEL::SOUND_EFFECT));

	return S_OK;
}

HRESULT CRhythmJump::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 7.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
	m_MovingFloorWorldMat = pDesc->worldMatrix;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();

	return S_OK;
}

void CRhythmJump::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		m_fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
		m_fSecPerFrame = m_fBeat * 4.f / m_iMaxFrame;

		m_bInit = true;
	}
}

void CRhythmJump::Tick(_float fTimeDelta)
{
	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime >= m_fSecPerFrame)
	{
		m_fFrameTime -= m_fFrameTime;
		++m_iCurFrameIndex;
		m_iCurFrameIndex %= m_iMaxFrame;
	}

	auto pGI = m_pGameInstance.lock();
	m_bCulled = !pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 3.f);

	_bool bRange = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - pGI->Get_CamPosition_Vector())) > 5.f;

	if (m_bExecuted)
	{
		_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (iCurBeat != m_iPrevBeatCnt)
		{
			m_iPrevBeatCnt = iCurBeat;
			if (m_bRest)
				m_bRest = false;
		}

		Move(fTimeDelta);

		_matrix Transpose = XMLoadFloat4x4(&m_MovingFloorMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat();
		XMStoreFloat4x4(&m_MovingFloorWorldMat, Transpose);
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

		PxTransform px = m_pPhysXCollider->Get_Static_RigidBody()->getGlobalPose();
		PxTransform transform{ pxWorld };
		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

		if (m_bCollision)
		{
			auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));

			_float3 vVelocity = pCCT->Get_Velocity();
			auto vPlayerPos = pCCT->Get_Controller()->getPosition();

			_vector vPos = Transpose.r[3];
			//_vector vPos = m_pTransformCom->Get_Position();
			_float fTopY = XMVectorGetY(vPos);

			if (vVelocity.y <= 0)
			{
				_vector vFootPos = XMVectorSet(vPlayerPos.x, fTopY + 0.25f, vPlayerPos.z, 1.f);

				//pCCT->Set_Position(XMMatrixTranslationFromVector(vFootPos));
				pCCT->Set_FootPosition(XMMatrixTranslationFromVector(vFootPos));
				dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Transform()->Set_Position(vFootPos);
				dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Tick_PartObj(fTimeDelta);
			}
		}
	}
}

void CRhythmJump::LateTick(_float fTimeDelta)
{
	if (m_bCulled)
		return;
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CRhythmJump::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pBaseModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmi = { false };
		if (FAILED(m_pBaseModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pBaseModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			bEmi = false;
		else
		{
			bEmi = true;
		}

		if (!bEmi)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);

		m_pBaseModelCom->Render((_uint)i);
	}

	iNumMeshes = m_pJointModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmi = { false };
		if (FAILED(m_pJointModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pJointModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			bEmi = false;
		else
		{
			bEmi = true;
		}

		if (!bEmi)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);

		m_pJointModelCom->Render((_uint)i);
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_MovingFloorWorldMat)))
		return E_FAIL;
	iNumMeshes = m_pMovingModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmi = { false };
		if (FAILED(m_pMovingModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pMovingModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			bEmi = false;
		else
		{
			bEmi = true;
		}

		if (!bEmi)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);

		m_pMovingModelCom->Render((_uint)i);
	}
	_float4x4 World;
	XMStoreFloat4x4(&World, XMLoadFloat4x4(&m_DisplayOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
		return E_FAIL;
	iNumMeshes = m_pDisplayModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmi = { false };		
		if (i == 1)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iCurFrameIndex, sizeof(_int))))
				return E_FAIL;
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
				return E_FAIL;
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
				return E_FAIL;

			m_pShaderCom->Begin((_uint)MESH_PASS::Frame);
		}
		else
		{
			if (FAILED(m_pDisplayModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				continue;

			if (SUCCEEDED(m_pDisplayModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			{
				bEmi = true;
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
			}
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}		

		//if (!bEmi)
		//	m_pShaderCom->Begin(0);
		//else
		//	m_pShaderCom->Begin((_uint)MESH_PASS::Emission);

		m_pDisplayModelCom->Render((_uint)i);
	}
#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CRhythmJump::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));

		_float3 vVelocity = pCCT->Get_Velocity();

		if (dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Is_OnAir())
			m_bCollision = false;
		else
		{
			m_bCollision = true;
			Execute();
		}

	}
}

void CRhythmJump::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		if (!dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Is_OnAir())
		{
			PLAYERMANAGER->Callback_Event("RhythmJump");
			m_bCollision = true;
			Execute();
		}
		
	}
}

void CRhythmJump::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		PLAYERMANAGER->Callback_Event("RhythmJump_End");
		m_bCollision = false;

		Execute(false);
	}
}

void CRhythmJump::Edit_Attribute()
{
}

HRESULT CRhythmJump::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));

	m_pBaseModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RhythmJumpBase"), TEXT("BaseModel_Com")));
	m_pJointModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RhythmJumpJoint"), TEXT("JointModel_Com")));
	m_pMovingModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RhythmJumpMoving"), TEXT("MovingModel_Com")));
	m_pDisplayModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RhythmJumpDisplay"), TEXT("DisplayModel_Com")));

	m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RhythmJumpDisplay_DMG"), TEXT("Texture_Com")));

	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
	aabbDesc.vExtents = { 1.5f, 1.0f, 1.5f };
	aabbDesc.vCenter = { 0.f, 1.f, 0.f };
	
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_RhythmJumpBase"))->Get_ModelData();
	m_vModelCenter = pModelData->VertexInfo.vCenter;
	_vector vModelSize = XMLoadFloat3(&pModelData->VertexInfo.vMax) - XMLoadFloat3(&pModelData->VertexInfo.vMin);
	XMStoreFloat3(&m_vModelSize, vModelSize);
	
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
	m_vModelSize = { 1.5f, 0.17f, 1.5f };
	m_vModelCenter = { 0.f, 0.17f, 0.f };
	PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
	PxBoxGeometry Box{ BoxDimension };
	auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, strTag.c_str());

	assert(pRigidBody);

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

	PxTransform px = m_pPhysXCollider->Get_Static_RigidBody()->getGlobalPose();
	PxTransform transform{ pxWorld };
	m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

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

	if (!m_pShaderCom || !m_pPhysXCollider)
		assert(false);

	return S_OK;
}

HRESULT CRhythmJump::Bind_ShaderResources()
{
	auto pGI = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

	XMINT2 vDivide = { 4, 2 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &vDivide, sizeof(XMINT2))))
		return E_FAIL;

	return S_OK;
}

void CRhythmJump::Move(_float fTimeDelta)
{
	if (m_bRest)
		return;
	
	if (!m_bDown)
	{
		m_MovingFloorMatrix._42 = 0.1f;
		m_bRest = true;
		m_bDown = true;
	}

	else
	{
		if (m_MovingFloorMatrix._42 <= 0)
		{
			m_MovingFloorMatrix._42 = 0.f;
			m_bRest = true;
			m_bDown = false;
		}
		else
			m_MovingFloorMatrix._42 -= fTimeDelta;
	}
}

shared_ptr<CRhythmJump> CRhythmJump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRhythmJump
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRhythmJump(pDevice, pContext)
		{}
	};

	shared_ptr<CRhythmJump> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRhythmJump"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CRhythmJump::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRhythmJump
	{
		MakeSharedEnabler(const CRhythmJump& rhs) : CRhythmJump(rhs)
		{}
	};

	shared_ptr<CRhythmJump> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRhythmJump"));
		assert(false);
	}

	return pInstance;
}

void CRhythmJump::Free()
{
}
