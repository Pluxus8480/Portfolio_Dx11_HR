#include "ConveyorBelt.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"
#include "Mesh.h"
#include "Chai.h"
#include "CharacterController.h"

#include "Beat_Manager.h"

wstring CConveyorBelt::ObjID = TEXT("CConveyorBelt");
_bool CConveyorBelt::s_bFirst = false;

CConveyorBelt::CConveyorBelt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CConveyorBelt::CConveyorBelt(const CConveyorBelt& rhs)
	: CGimmickBase{ rhs }
{
}

CConveyorBelt::~CConveyorBelt()
{
	Free();
}

void CConveyorBelt::Execute(_bool bExecute)
{
	m_bExecuted = true;
}

HRESULT CConveyorBelt::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ConveyorBelt"), TEXT("../Bin/Resources/Models/Map/Gimmick/ConveyorBelt.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);

	return S_OK;
}

HRESULT CConveyorBelt::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float3 vScale = m_pTransformCom->Get_Scale();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_pModelCom->Get_Mesh(i)->Cooking_TriangleMesh(m_pTransformCom->Get_WorldMatrix());
		m_pPhysxCollider->createStaticMeshActor(m_pModelCom->Get_Mesh(i)->Get_TriangleMesh(), "");
	}


	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();

	return S_OK;
}

void CConveyorBelt::PriorityTick(_float fTimeDelta)
{
	s_bFirst = true;
}

void CConveyorBelt::Tick(_float fTimeDelta)
{
	_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeatCnt != iCurrBeat)
	{
		m_iPrevBeatCnt = iCurrBeat;
		m_fUvFlow = 1.f;
	}
	if (m_fUvFlow > 0.f)
	{
		m_fUvFlow -= fTimeDelta / CBeat_Manager::Get_Instance()->Get_Beat() * 4.f;
	}
	else
	{
		m_fUvFlow = 0.f;
	}


	//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matOut) * m_pTransformCom->Get_WorldMatrix_XMMat());


	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
	//PxMat44 mat;

	//mat.column0.x = Transpose.r[0].m128_f32[0];
	//mat.column0.y = Transpose.r[0].m128_f32[1];
	//mat.column0.z = Transpose.r[0].m128_f32[2];
	//mat.column0.w = Transpose.r[0].m128_f32[3];
	//mat.column1.x = Transpose.r[1].m128_f32[0];
	//mat.column1.y = Transpose.r[1].m128_f32[1];
	//mat.column1.z = Transpose.r[1].m128_f32[2];
	//mat.column1.w = Transpose.r[1].m128_f32[3];
	//mat.column2.x = Transpose.r[2].m128_f32[0];
	//mat.column2.y = Transpose.r[2].m128_f32[1];
	//mat.column2.z = Transpose.r[2].m128_f32[2];
	//mat.column2.w = Transpose.r[2].m128_f32[3];
	//mat.column3.x = Transpose.r[3].m128_f32[0];
	//mat.column3.y = Transpose.r[3].m128_f32[1];
	//mat.column3.z = Transpose.r[3].m128_f32[2];
	//mat.column3.w = Transpose.r[3].m128_f32[3];

	//PxTransform transform{ mat };
	//m_pPhysxCollider->Get_Static_RigidBody()->setGlobalPose(transform);

	m_pColliderCom->Update(Transpose);


}

void CConveyorBelt::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	//GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CConveyorBelt::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	_float2 vFlow{ 0.f,m_fUvFlow };
	m_pShaderCom->Bind_RawValue("g_vUVFlow", &vFlow, sizeof(_float2));
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if(i)
			m_pShaderCom->Begin((_uint)MESH_PASS::Belt);
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CConveyorBelt::Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr<CCollider> pCaller)
{
}

void CConveyorBelt::Collision(shared_ptr< CCollider> pOther, shared_ptr<CCollider> pCaller)
{
	if (m_fUvFlow > 0.f)
	{
		auto pChai = dynamic_pointer_cast<CChai>(pOther->Get_Owner());
		if (pChai)
		{
			auto pCCT = dynamic_pointer_cast<CCharacterController>(pChai->Find_Component(TEXT("Com_CCT")));
			if (pCCT)
			{
				if(s_bFirst)
				{
					pCCT->Add_Displacement(m_pTransformCom->Get_NormRight() * -13.f);
					s_bFirst = false;
				}
			}
		}
	}
}

void CConveyorBelt::Edit_Attribute()
{


}

void CConveyorBelt::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

}

void CConveyorBelt::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

}

HRESULT CConveyorBelt::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ConveyorBelt"), TEXT("Model_Com")));

	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vExtents = { 6.33f, 1.f, 5.0f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	m_pPhysxCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysxCol_Com")));
	assert(m_pPhysxCollider);
	if (!m_pShaderCom || !m_pModelCom /* || !m_pColliderCom */)
		assert(false);

	return S_OK;
}

HRESULT CConveyorBelt::Bind_ShaderResources()
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

shared_ptr<CConveyorBelt> CConveyorBelt::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CConveyorBelt
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CConveyorBelt(pDevice, pContext)
		{}
	};

	shared_ptr<CConveyorBelt> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CConveyorBelt"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CConveyorBelt::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CConveyorBelt
	{
		MakeSharedEnabler(const CConveyorBelt& rhs) : CConveyorBelt(rhs)
		{}
	};

	shared_ptr<CConveyorBelt> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CConveyorBelt"));
		assert(false);
	}

	return pInstance;
}

void CConveyorBelt::Free()
{
}
