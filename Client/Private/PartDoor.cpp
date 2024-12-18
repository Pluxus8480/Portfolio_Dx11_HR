#include "PartDoor.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"

wstring CPartDoor::ObjID = TEXT("CPartDoor");

CPartDoor::CPartDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPartDoor::CPartDoor(const CPartDoor& rhs)
	: CPartObject{ rhs }
{
}

CPartDoor::~CPartDoor()
{
	Free();
}

void CPartDoor::Set_DecalIndex(_int iDecalIndex, _int iTextureIndex)
{
	if (!m_bNumbered) return;
	if (iDecalIndex >= m_vecDecalIndex.size()) return;

	m_vecDecalIndex[iDecalIndex] = iTextureIndex;
}

void CPartDoor::Execute_Door(_bool bExecute)
{
	m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_DoorOpen", m_pTransformCom->Get_Position());
	m_bExecute = bExecute;
}

void CPartDoor::Init_DoorState(_bool bOpen)
{
	m_bOpen = bOpen;
	_vector vInitialPos = m_pTransformCom->Get_Position();

	if (m_bOpen)
	{
		switch (m_eDoorDir)
		{
		case Client::CPartDoor::LEFT:
			m_pTransformCom->Set_Position(XMVectorSetZ(vInitialPos, -m_fMoveDist));
			break;
		case Client::CPartDoor::RIGHT:
			m_pTransformCom->Set_Position(XMVectorSetZ(vInitialPos, +m_fMoveDist));
			break;
		case Client::CPartDoor::UP:
			m_pTransformCom->Set_Position(XMVectorSetY(vInitialPos, +m_fMoveDist));
			break;
		case Client::CPartDoor::DOWN:
			m_pTransformCom->Set_Position(XMVectorSetY(vInitialPos, -m_fMoveDist));
			break;
		}
	}
}

HRESULT CPartDoor::Initialize_Prototype()
{
	//Door_Open
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_DoorOpen", L"../Bin/Resources/Sounds/MapObject/Door_Open.ogg",
		SOUND_CHANNEL::SOUND_EFFECT));


	return S_OK;
}

HRESULT CPartDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	DOOR_PARTDESC* pDesc = static_cast<DOOR_PARTDESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, pDesc->strModelTag))->Get_ModelData();
	m_vModelCenter = pModelData->VertexInfo.vCenter;
	_vector vModelSize = XMLoadFloat3(&pModelData->VertexInfo.vMax) - XMLoadFloat3(&pModelData->VertexInfo.vMin);
	XMStoreFloat3(&m_vModelSize, vModelSize);
	m_vModelSize.x = fabs(m_vModelSize.x);
	m_vModelSize.y = fabs(m_vModelSize.y);
	m_vModelSize.z = fabs(m_vModelSize.z);
	m_fModelRange = pModelData->Get_MaxOffsetRange();

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_bOpen = pDesc->bOpen;
	m_eDoorDir = pDesc->eDir;
	m_fMoveDist = pDesc->fMoveDist;
	m_bNumbered = pDesc->bNumbered;
	
	if(pDesc->iNumDecal)
		m_vecDecalIndex.resize(pDesc->iNumDecal);

	return S_OK;
}

void CPartDoor::PriorityTick(_float fTimeDelta)
{
}

void CPartDoor::Tick(_float fTimeDelta)
{
	if (m_bExecute)
	{
		if (m_bOpen)
			Close_Door(fTimeDelta);

		else
			Open_Door(fTimeDelta);
	}

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
	XMStoreFloat4(&m_vWorldCenter, XMVector3TransformCoord(XMLoadFloat3(&m_vModelCenter), XMLoadFloat4x4(&m_WorldMatrix)));
	m_vWorldCenter.w = 1.f;

	if (m_bExecute || !m_bOpen)
	{
		_matrix Transpose = XMLoadFloat4x4(&m_WorldMatrix);
		//Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
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
		for (auto pRigid : m_vecPxRigid)
			pRigid->setGlobalPose(transform);
		//m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
	}

	else
	{
		for (auto pRigid : m_vecPxRigid)
			pRigid->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 100000,-100000.f} });
		//m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 300,-100000.f} });
	}
}

void CPartDoor::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	if (!pGI->isIn_Frustum_WorldSpace(XMLoadFloat4(&m_vWorldCenter), m_fModelRange)) return;
	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CPartDoor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		_bool bNumberedDecal = { false };
		if (m_bNumbered)
		{
			if (i < m_vecDecalIndex.size())
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_vecDecalIndex[i], sizeof(_int))))
					return E_FAIL;

				bNumberedDecal = true;
			}
		}

		if (bNumberedDecal)
			m_pShaderCom->Begin((_uint)MESH_PASS::Frame);
		else
			m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CPartDoor::Ready_Components(DOOR_PARTDESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, pDesc->strModelTag, TEXT("Com_Model")));

	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));
	
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
	_float3 vWorldSize{};
	_matrix matWorld = XMLoadFloat4x4(&m_WorldMatrix);
	vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
	vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
	vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));
	//m_vModelSize.x *= vWorldSize.x;
	//m_vModelSize.y *= vWorldSize.y;
	//m_vModelSize.z *= vWorldSize.z;
	//m_vModelCenter.x *= vWorldSize.x;
	//m_vModelCenter.y *= vWorldSize.y;
	//m_vModelCenter.z *= vWorldSize.z;
	m_fModelRange *= max(vWorldSize.x, max(vWorldSize.y, vWorldSize.z));
	//m_vModelSize = { m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
	////XMStoreFloat3(&m_vModelSize, XMVector3Dot(XMLoadFloat3(&m_vModelSize), XMLoadFloat3(&vWorldSize)));
	////XMStoreFloat3(&m_vModelCenter, XMVector3TransformCoord(XMLoadFloat3(&m_vModelCenter), XMLoadFloat4x4(&m_WorldMatrix)));
	//PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
	//PxBoxGeometry Box{ BoxDimension };
	//m_pPhysXCollider->createStaticShapeActor(Box, "Test");

	_matrix scaleMatrix = XMMatrixScaling(vWorldSize.x, vWorldSize.y, vWorldSize.z);
	_float4x4 fScaleMat{};
	XMStoreFloat4x4(&fScaleMat, scaleMatrix);
	PxVec3 pscale = { vWorldSize.x, vWorldSize.y, vWorldSize.z };
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	string strTag = string(CPartDoor::ObjID.begin(), CPartDoor::ObjID.end());
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		string tmp = strTag + string("_") + to_string(m_iObjectID) + string("_") + to_string(i);
		auto pMesh = m_pModelCom->Get_Mesh((_uint)i);
		pMesh->Cooking_TriangleMesh(fScaleMat);
		auto pTriangleMesh = pMesh->Get_TriangleMesh();
		if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
		{
			auto pRigid = m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
			if (pRigid)
				m_vecPxRigid.emplace_back(pRigid);
		}
	}

	if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
		assert(false);

	return S_OK;
}

HRESULT CPartDoor::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

void CPartDoor::Open_Door(_float fTimeDelta)
{
	_vector vCurPos = m_pTransformCom->Get_Position();

	switch (m_eDoorDir)
	{
	case Client::CPartDoor::LEFT:
	{
		if (XMVectorGetZ(vCurPos) < -m_fMoveDist
			|| fabs(fabs(XMVectorGetZ(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetZ(vCurPos, -m_fMoveDist));
		}
		else
			m_pTransformCom->Go_Backward(fTimeDelta);
	
		break;
	}
	case Client::CPartDoor::RIGHT:
	{
		if (XMVectorGetZ(vCurPos) > +m_fMoveDist
			|| fabs(fabs(XMVectorGetZ(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetZ(vCurPos, +m_fMoveDist));
		}
		else
			m_pTransformCom->Go_Straight(fTimeDelta);
	
		break;
	}
	case Client::CPartDoor::UP:
	{
		if (XMVectorGetY(vCurPos) > +m_fMoveDist
			|| fabs(fabs(XMVectorGetY(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetY(vCurPos, +m_fMoveDist));
		}
		else
			m_pTransformCom->Go_Up(fTimeDelta);

		break;
	}
	case Client::CPartDoor::DOWN:
	{
		if (XMVectorGetY(vCurPos) < -m_fMoveDist
			|| fabs(fabs(XMVectorGetY(vCurPos)) - m_fMoveDist) < 0.1f)
		{
			m_bOpen = true;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetY(vCurPos, -m_fMoveDist));
		}
		else
			m_pTransformCom->Go_Down(fTimeDelta);

		break;
	}
	}
}

void CPartDoor::Close_Door(_float fTimeDelta)
{
	_vector vCurPos = m_pTransformCom->Get_Position();

	switch (m_eDoorDir)
	{
	case Client::CPartDoor::LEFT:
	{
		if (XMVectorGetZ(vCurPos) > 0
			|| fabs(XMVectorGetZ(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetZ(vCurPos, 0));
		}
		else
			m_pTransformCom->Go_Straight(fTimeDelta);

		break;
	}
	case Client::CPartDoor::RIGHT:
	{
		if (XMVectorGetZ(vCurPos) < 0
			|| fabs(XMVectorGetZ(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetZ(vCurPos, 0));
		}
		else
			m_pTransformCom->Go_Backward(fTimeDelta);

		break;
	}
	case Client::CPartDoor::UP:
	{
		if (XMVectorGetY(vCurPos) > 0
			|| fabs(XMVectorGetY(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetY(vCurPos, 0));
		}
		else
			m_pTransformCom->Go_Down(fTimeDelta);

		break;
	}
	case Client::CPartDoor::DOWN:
	{
		if (XMVectorGetY(vCurPos) < 0
			|| fabs(XMVectorGetY(vCurPos)) < 0.1f)
		{
			m_bOpen = false;
			m_bExecute = false;
			m_bDone = true;

			m_pTransformCom->Set_Position(XMVectorSetY(vCurPos, 0));
		}
		else
			m_pTransformCom->Go_Up(fTimeDelta);

		break;
	}
	}

}

shared_ptr<CPartDoor> CPartDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CPartDoor
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartDoor(pDevice, pContext)
		{}
	};

	shared_ptr<CPartDoor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPartDoor"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CPartDoor::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CPartDoor
	{
		MakeSharedEnabler(const CPartDoor& rhs) : CPartDoor(rhs)
		{}
	};


	shared_ptr<CPartDoor> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPartDoor"));
		assert(false);
	}

	return pInstance;
}

void CPartDoor::Free()
{
	for (auto& Rigid : m_vecPxRigid)
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
