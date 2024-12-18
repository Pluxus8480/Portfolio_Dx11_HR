#include "Rekka_Logo.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "OctComp.h"

#include "MapObject_Static.h"

wstring CRekka_Logo::ObjID = TEXT("CRekka_Logo");

CRekka_Logo::CRekka_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CRekka_Logo::CRekka_Logo(const CRekka_Logo& rhs)
    : CGameObject{rhs}
{
}

CRekka_Logo::~CRekka_Logo()
{
    Free();
}

HRESULT CRekka_Logo::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f)));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RekkaLogo"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_REKKALogo_SH_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

    return S_OK;
}

HRESULT CRekka_Logo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	
	//m_pOctCom->Calc_Octree(m_vMeshCenter, m_fCullingRange);

    return S_OK;
}

void CRekka_Logo::PriorityTick(_float fTimeDelta)
{
}

void CRekka_Logo::Tick(_float fTimeDelta)
{
}

void CRekka_Logo::LateTick(_float fTimeDelta)
{
	//if (m_pOctCom->IsCulled())
	//	return;
	if (false == GAMEINSTANCE->isIn_Frustum_WorldSpace(XMVectorSetW(XMLoadFloat3(&m_vMeshCenter), 1.f), m_fCullingRange)) return;

	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_TERRAIN, shared_from_this());
}

HRESULT CRekka_Logo::Render()
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

		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

    return S_OK;
}

HRESULT CRekka_Logo::Ready_Components(const MISCOBJ_DESC* pDesc)
{	
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RekkaLogo"), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);

	if (true == pDesc->bCollision)
	{
		_uint    iNumMeshes = m_pModelCom->Get_NumMeshes();
		string strTag = string(CRekka_Logo::ObjID.begin(), CRekka_Logo::ObjID.end());
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			string tmp = strTag + string("_") + to_string(i);
			auto pMesh = m_pModelCom->Get_Mesh((_uint)i);
			_matrix mat = XMLoadFloat4x4(&pDesc->worldMatrix);
			_vector vScale, vRot, vPos;
			XMMatrixDecompose(&vScale, &vRot, &vPos, mat);
			_float3 scale;
			XMStoreFloat3(&scale, vScale);
			PxVec3 pscale;
			pscale.x = scale.x;
			pscale.y = scale.y;
			pscale.z = scale.z;
			pMesh->Cooking_TriangleMesh(pDesc->worldMatrix);
			auto pTriangleMesh = pMesh->Get_TriangleMesh();
			if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
				m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
		}

		if (!m_pPhysXCollider)
			assert(false);
	}

	m_pOctCom = static_pointer_cast<COctComp>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Octree"), TEXT("Com_Octree")));

	if (!m_pShaderCom || !m_pOctCom)
		assert(false);

	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RekkaLogo")))->Get_ModelData();
	_float3 vLocalMeshCenter = pModelData->VertexInfo.vCenter;
	//vLocalMeshCenter.z *= -1.f;
	_vector vMeshCenter = XMVectorSetW(XMLoadFloat3(&vLocalMeshCenter), 1.f);
	XMStoreFloat3(&m_vMeshCenter, XMVector3TransformCoord(vMeshCenter, m_pTransformCom->Get_WorldMatrix_XMMat()));
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_fCullingRange = pModelData->Get_MaxOffsetRange() * (max(vScale.x, max(vScale.y, vScale.z)));
	m_fCullingRange *= 1.5f;

    return S_OK;
}

shared_ptr<CRekka_Logo> CRekka_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRekka_Logo
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Logo(pDevice, pContext)
		{}
	};


	shared_ptr<CRekka_Logo> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_Logo"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CRekka_Logo::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_Logo
	{
		MakeSharedEnabler(const CRekka_Logo& rhs) : CRekka_Logo(rhs)
		{}
	};

	shared_ptr<CRekka_Logo> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_Logo"));
		assert(false);
	}

	return pInstance;
}

void CRekka_Logo::Free()
{
}
