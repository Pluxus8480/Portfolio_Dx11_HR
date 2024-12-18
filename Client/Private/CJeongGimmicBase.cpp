#include "CJeongGimmicBase.h"
#include "GimmickManager.h"
#include "PhysXCollider.h"
#include "Beat_Manager.h"


CJeongGimmicBase::CJeongGimmicBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase(pDevice, pContext)
{
}

CJeongGimmicBase::CJeongGimmicBase(const CJeongGimmicBase& rhs)
	: CGimmickBase(rhs)
{
	m_strGimmickID = rhs.m_strGimmickID;
}


CJeongGimmicBase::~CJeongGimmicBase()
{
	Free();
}

HRESULT CJeongGimmicBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJeongGimmicBase::Initialize(void* pArg)
{
	return S_OK;
}

void CJeongGimmicBase::PriorityTick(_float fTimeDelta)
{
}

void CJeongGimmicBase::Tick(_float fTimeDelta)
{
}

void CJeongGimmicBase::LateTick(_float fTimeDelta)
{
}

HRESULT CJeongGimmicBase::Render()
{
	if (FAILED(Bind_ShaderResources()))
		assert(false);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			assert(false);*/

		_bool bEmi = { false };
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			bEmi = false;
		else
			bEmi = true;

		if (bEmi) {
			_float4 vColor = { 1.f,0.2f, 0.f, 1.f };
			m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
			m_pShaderCom->Begin((_uint)MESH_PASS::DiffuseColor);
		}
		else
			m_pShaderCom->Begin(m_iShaderPassIndex);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CJeongGimmicBase::Bind_ShaderResources()
{
	auto pGI = m_pGameInstance.lock();
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	 if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		 assert(false);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		assert(false);

	return S_OK;
}

void CJeongGimmicBase::InitializeBasicComponent(wstring ModelName, wstring ShaderName, wstring PhysicsName)
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, ShaderName, L"Shader_Com"));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, ModelName, L"Model_Com"));
	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, PhysicsName, L"Com_PhsyXCollider"));
    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
        assert(false);
}


void CJeongGimmicBase::Use_PhysX()
{
    _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
	Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
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
	if (m_vecPxRigid.size() != 0)
	{
		for (auto& iter : m_vecPxRigid)
		{
			iter->setGlobalPose(transform);
		}
	}
	else
	{
		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
	}
}

void CJeongGimmicBase::Follow_BonePhysX(const _float4x4 Bone)
{
	_matrix BoneMatrix;
	BoneMatrix = XMLoadFloat4x4(&Bone);
	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);
	_matrix Transpose = XMMatrixMultiply(BoneMatrix, m_pTransformCom->Get_WorldMatrix_XMMat());
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

void CJeongGimmicBase::Initialize_Transform()
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		assert(false);
}

_float3 CJeongGimmicBase::Get_Pos()
{
	_float3 Pos;
	XMStoreFloat3(&Pos, m_pTransformCom->Get_Position());
	return Pos;
}

void CJeongGimmicBase::Set_Pos(_float3 Pos)
{
	m_pTransformCom->Set_Position(XMLoadFloat3(&Pos));
}

void CJeongGimmicBase::Set_Pos(_float PosX, _float PosY, _float PosZ)
{
	_float3 Pos = { PosX, PosY, PosZ };
	m_pTransformCom->Set_Position(XMLoadFloat3(&Pos));
}

void CJeongGimmicBase::Argument_Setting(Desc* pDesc)
{
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
	Set_Pos(pDesc->Pos);
	m_pTransformCom->Set_Scale(pDesc->Scale);
	m_eRenderGroup = pDesc->eRenderGroup;
	m_iShaderPassIndex = pDesc->ShaderPass;
}

void CJeongGimmicBase::wstringtostring(wstring ObjID, string& GimmicId)
{
	m_strGimmickID.assign(ObjID.begin(), ObjID.end());
}

void CJeongGimmicBase::Initialize_CookPhysX(wstring ModelName, _float3 Center)
{
	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>
		(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, ModelName))->Get_ModelData();

	m_vModelCenter = Center;
	_vector vModelSize = XMLoadFloat3(&pModelData->VertexInfo.vMax) - XMLoadFloat3(&pModelData->VertexInfo.vMin);
	m_fModelRange = pModelData->Get_MaxOffsetRange();

	_float3 vWorldSize = m_pTransformCom->Get_Scale();
	_matrix scaleMatrix = XMMatrixScaling(vWorldSize.x, vWorldSize.y, vWorldSize.z);
	_float4x4 fScaleMat{};
	XMStoreFloat4x4(&fScaleMat, scaleMatrix);
	PxVec3 pscale = { vWorldSize.x, vWorldSize.y, vWorldSize.z };

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		string tmp = "dmddo";
		auto pMesh = m_pModelCom->Get_Mesh((_uint)i);
		pMesh->Cooking_TriangleMesh(fScaleMat);
		auto pTriangleMesh = pMesh->Get_TriangleMesh();
		if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
		{
			auto pRigid = m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
			if (pRigid)
			{
				m_vecPxRigid.emplace_back(pRigid);
			}
			else
			{
				assert(false);
			}
		}
	}
}

void CJeongGimmicBase::Free()
{
	__super::Free();

	for(auto& Rigid : m_vecPxRigid)
	{
		if (Rigid != nullptr)
		{
			auto Scene = Rigid->getScene();
			if (Scene)
				Scene->removeActor(*Rigid);
		}
	}
	m_vecPxRigid.clear();
}
