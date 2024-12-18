#include "Rekka_Generator.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "OctComp.h"

#include "MapObject_Static.h"


wstring CRekka_Generator::ObjID = TEXT("CRekka_Generator");

CRekka_Generator::CRekka_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CRekka_Generator::CRekka_Generator(const CRekka_Generator& rhs)
	: CGameObject(rhs)
{
}

CRekka_Generator::~CRekka_Generator()
{
	Free();
}

HRESULT CRekka_Generator::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Model_Monster_Rekka"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_ChargingAreaA_SH_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Model_Monster_Rekka_After"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_ChargingAreaB_SH_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

	return S_OK;
}

HRESULT CRekka_Generator::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CRekka_Generator::PriorityTick(_float fTimeDelta)
{
}

void CRekka_Generator::Tick(_float fTimeDelta)
{
}

void CRekka_Generator::LateTick(_float fTimeDelta)
{
	if (false == GAMEINSTANCE->isIn_Frustum_WorldSpace(XMVectorSetW(XMLoadFloat3(&m_vMeshCenter), 1.f), m_fCullingRange))
		return;
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_TERRAIN, shared_from_this());
}

HRESULT CRekka_Generator::Render()
{
	if (!m_pModelCom)
		return S_OK;

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	if (true)
	{
		_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_bool bEmissive = { false };
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				bEmissive = false;
			else
				bEmissive = true;

			if (bEmissive)
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);

			m_pModelCom->Render((_uint)i);
		}
	}
	else
	{
		_uint	iNumMeshes = m_pModel_After->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_bool bEmissive = { false };
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				bEmissive = false;
			else
				bEmissive = true;

			if (bEmissive)
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);

			m_pModelCom->Render((_uint)i);
		}
	}

	return S_OK;
}

HRESULT CRekka_Generator::Ready_Components(const MISCOBJ_DESC* pDesc)
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Model_Monster_Rekka"), TEXT("Com_Model")));
	m_pModel_After = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Model_Monster_Rekka_After"), TEXT("Com_Model_After")));
	if (!m_pModelCom || !m_pModel_After || !m_pShaderCom)
		assert(false);

	// Setting Obstacle Info
	if (true == pDesc->bCollision)
	{
		_uint    iNumMeshes = m_pModelCom->Get_NumMeshes();
		string strTag = string(CRekka_Generator::ObjID.begin(), CRekka_Generator::ObjID.end());

		_float3 m_vModelSize = { 4.f, 0.5f, 4.f };
		_float3 m_vModelCenter = { 0.f, -0.5f, 0.f };
		PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
		PxBoxGeometry Box{ BoxDimension };
		auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, strTag.c_str());
		assert(pRigidBody);

		_matrix WorldMat = m_pTransformCom->Get_WorldMatrix_XMMat();
		WorldMat = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), WorldMat);
		WorldMat.r[0] = XMVector3Normalize(WorldMat.r[0]);
		WorldMat.r[1] = XMVector3Normalize(WorldMat.r[1]);
		WorldMat.r[2] = XMVector3Normalize(WorldMat.r[2]);
		_float4x4 mat;
		XMStoreFloat4x4(&mat, WorldMat);
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

	m_pOctCom = static_pointer_cast<COctComp>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Octree"), TEXT("Com_Octree")));
	if (!m_pOctCom)
		assert(false);

	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, TEXT("Model_Monster_Rekka")))->Get_ModelData();
	_float3 vLocalMeshCenter = pModelData->VertexInfo.vCenter;
	//vLocalMeshCenter.z *= -1.f;
	_vector vMeshCenter = XMVectorSetW(XMLoadFloat3(&vLocalMeshCenter), 1.f);
	XMStoreFloat3(&m_vMeshCenter, XMVector3TransformCoord(vMeshCenter, m_pTransformCom->Get_WorldMatrix_XMMat()));
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_fCullingRange = pModelData->Get_MaxOffsetRange() * (max(vScale.x, max(vScale.y, vScale.z)));
	m_fCullingRange *= 1.5f;

	return S_OK;
}

shared_ptr<CRekka_Generator> CRekka_Generator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRekka_Generator
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Generator(pDevice, pContext) {}
	};

	shared_ptr<CRekka_Generator> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_Generator"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CRekka_Generator::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_Generator
	{
		MakeSharedEnabler(const CRekka_Generator& rhs) : CRekka_Generator(rhs) {}
	};

	shared_ptr<CRekka_Generator> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_Generator"));
		assert(false);
	}
	return pInstance;
}

void CRekka_Generator::Free()
{
}
