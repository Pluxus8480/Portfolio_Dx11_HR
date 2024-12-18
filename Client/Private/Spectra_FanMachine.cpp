#include "Spectra_FanMachine.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_OBB.h"

#include "Spectra_FanBlade.h"
#include "MapManager.h"

wstring CSpectra_FanMachine::ObjID = TEXT("CSpectra_FanMachine");
CSpectra_FanMachine::CSpectra_FanMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CSpectra_FanMachine::CSpectra_FanMachine(const CSpectra_FanMachine& rhs)
    : CGameObject{rhs}
{
}

CSpectra_FanMachine::~CSpectra_FanMachine()
{
    Free();
}

void CSpectra_FanMachine::Set_Dead(bool bDead)
{
	m_bIsDead = bDead;

	m_pBlade[0]->Set_Dead(bDead);
	m_pBlade[1]->Set_Dead(bDead);
}

HRESULT CSpectra_FanMachine::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Spectra_FanBody"), TEXT("../Bin/Resources/Models/Map/Stage_Kale/SM_FanMachineA_IZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	m_List_Owning_ObjectID.push_back(CSpectra_FanBlade::ObjID);
	
    return S_OK;
}

HRESULT CSpectra_FanMachine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

    return S_OK;
}

void CSpectra_FanMachine::PriorityTick(_float fTimeDelta)
{
	m_pBlade[0]->PriorityTick(fTimeDelta);
	m_pBlade[1]->PriorityTick(fTimeDelta);
}

void CSpectra_FanMachine::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
#endif

	m_pBlade[0]->Tick(fTimeDelta);
	m_pBlade[1]->Tick(fTimeDelta);
}

void CSpectra_FanMachine::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

	m_pBlade[0]->LateTick(fTimeDelta);
	m_pBlade[1]->LateTick(fTimeDelta);
}

HRESULT CSpectra_FanMachine::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		m_pShaderCom->Begin(0);
		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CSpectra_FanMachine::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Spectra_FanBody"), TEXT("Model_Com")));
	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	_float3 vModelCenter = { 0.f, 1.f, 0.f };
	_float3 vModelSize = { 2.5f, 1.f, 1.5f };

	_float3 vScale = m_pTransformCom->Get_Scale();
	vModelSize = { vModelSize.x * vScale.x, vModelSize.y * vScale.y , vModelSize.z * vScale.z };
	vModelCenter = { vModelCenter.x * vScale.x, vModelCenter.y * vScale.y , vModelCenter.z * vScale.z };

	PxVec3 BoxDimension{ vModelSize.x, vModelSize.y, vModelSize.z };
	PxBoxGeometry Box{ BoxDimension };
	string ColliderTag = string(CSpectra_FanMachine::ObjID.begin(), CSpectra_FanMachine::ObjID.end()) + to_string(m_iObjectID);
	m_pPhysXCollider->createStaticShapeActor(Box, ColliderTag.c_str());

	_matrix WorldMat = m_pTransformCom->Get_WorldMatrix_XMMat();
	WorldMat = XMMatrixMultiply(XMMatrixTranslation(vModelCenter.x, vModelCenter.y, vModelCenter.z), WorldMat);
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

#ifdef _DEBUG
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vExtents = vModelSize;
	obbDesc.vCenter = vModelCenter;
	obbDesc.vRotation = { 0.f, 0.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	assert(m_pColliderCom);
#endif

	if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
		assert(false);

	return S_OK;
}

HRESULT CSpectra_FanMachine::Ready_Parts()
{
	CPartObject::PARTOBJ_DESC partDesc{};
	partDesc.pParentTransform = m_pTransformCom;
	partDesc.vPosition = { -1.4f, 1.2f, -0.5f, 1.f };

	auto pPrototype = GAMEINSTANCE->Get_Object_Prototype(CSpectra_FanBlade::ObjID);
	m_pBlade[0] = pPrototype->Clone(&partDesc);

	partDesc.vPosition = { 1.4f, 1.2f, -0.5f, 1.f };
	m_pBlade[1] = pPrototype->Clone(&partDesc);
	
	assert(m_pBlade[0]);
	assert(m_pBlade[1]);

	return S_OK;
}

HRESULT CSpectra_FanMachine::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

    return S_OK;
}

shared_ptr<CSpectra_FanMachine> CSpectra_FanMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSpectra_FanMachine
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSpectra_FanMachine(pDevice, pContext)
		{}
	};

	shared_ptr<CSpectra_FanMachine> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpectra_FanMachine"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CSpectra_FanMachine::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSpectra_FanMachine
	{
		MakeSharedEnabler(const CSpectra_FanMachine& rhs) : CSpectra_FanMachine(rhs)
		{}
	};

	shared_ptr<CSpectra_FanMachine> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpectra_FanMachine"));
		assert(false);
	}

	return pInstance;
}

void CSpectra_FanMachine::Free()
{
}
