#include "LaserBoard.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Beat_Manager.h"
#include "PodLaser.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CLaserBoard::ObjID = TEXT("CLaserBoard");

CLaserBoard::CLaserBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CLaserBoard::CLaserBoard(const CLaserBoard& rhs)
	: CGimmickBase{ rhs }
{
}

CLaserBoard::~CLaserBoard()
{
	Free();
}

void CLaserBoard::Execute(_bool bExecute)
{
	m_bExecuted = true;
}

HRESULT CLaserBoard::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LaserBoard"), TEXT("../Bin/Resources/Models/Map/Gimmick/LaserBoard.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);

	return S_OK;
}

HRESULT CLaserBoard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;


	if (FAILED(Ready_Components((_float4x4*)pArg)))
		return E_FAIL;



	return S_OK;
}

void CLaserBoard::PriorityTick(_float fTimeDelta)
{

}

void CLaserBoard::Tick(_float fTimeDelta)
{

	if (m_bExecuted)
	{
		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
		}
	}
}

void CLaserBoard::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

}

HRESULT CLaserBoard::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}
		if (bNoEmit)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);

		//m_pShaderCom->Begin(0);
		m_pModelCom->Render((_uint)i);
	}


	return S_OK;
}

void CLaserBoard::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CLaserBoard::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CLaserBoard::Edit_Attribute()
{
}

HRESULT CLaserBoard::Ready_Components(_float4x4* pMat)
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LaserBoard"), TEXT("Model_Com")));
	m_pPhysxCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));
	
	_uint    iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		auto pMesh = m_pModelCom->Get_Mesh((_uint)i);
		_matrix mat = XMLoadFloat4x4(pMat);
		_vector vScale, vRot, vPos;
		XMMatrixDecompose(&vScale, &vRot, &vPos, mat);
		_float3 scale;
		XMStoreFloat3(&scale, vScale);
		PxVec3 pscale;
		pscale.x = scale.x;
		pscale.y = scale.y;
		pscale.z = scale.z;
		pMesh->Cooking_TriangleMesh(*pMat);
		auto pTriangleMesh = pMesh->Get_TriangleMesh();
		if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
			m_pPhysxCollider->createStaticMeshActor(pTriangleMesh, nullptr);
	}


	if (!m_pShaderCom || !m_pModelCom || !m_pPhysxCollider)
		assert(false);

	return S_OK;
}

HRESULT CLaserBoard::Bind_ShaderResources()
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


shared_ptr<CLaserBoard> CLaserBoard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLaserBoard
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLaserBoard(pDevice, pContext)
		{}
	};

	shared_ptr<CLaserBoard> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLaserBoard"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CLaserBoard::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLaserBoard
	{
		MakeSharedEnabler(const CLaserBoard& rhs) : CLaserBoard(rhs)
		{}
	};

	shared_ptr<CLaserBoard> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLaserBoard"));
		assert(false);
	}

	return pInstance;
}

void CLaserBoard::Free()
{
}
