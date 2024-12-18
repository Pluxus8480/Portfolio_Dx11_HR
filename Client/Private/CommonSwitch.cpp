#include "CommonSwitch.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "SwitchButton.h"
#include "MapManager.h"


wstring CCommonSwitch::ObjID = TEXT("CCommonSwitch");

CCommonSwitch::CCommonSwitch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{pDevice, pContext}
{
}

CCommonSwitch::CCommonSwitch(const CCommonSwitch& rhs)
	: CGimmickBase{rhs}
{
}

CCommonSwitch::~CCommonSwitch()
{
	Free();
}

void CCommonSwitch::Set_Dead(bool bDead)
{
	m_bIsDead = bDead;

	m_pButtonObj->Set_Dead(bDead);
}

HRESULT CCommonSwitch::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_Owning_ObjectID.push_back(CSwitchButton::ObjID);
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CommonSwitch_Off"), TEXT("../Bin/Resources/Models/Map/Gimmick/CommonSwitch_OFF.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_FloorSwitch1", L"../Bin/Resources/Sounds/MapObject/Button_Push1.ogg", SOUND_CHANNEL::SOUND_EFFECT));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_FloorSwitch2", L"../Bin/Resources/Sounds/MapObject/Button_Push2.ogg", SOUND_CHANNEL::SOUND_EFFECT));

	return S_OK;
}

HRESULT CCommonSwitch::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;
	
	return S_OK;
}

void CCommonSwitch::PriorityTick(_float fTimeDelta)
{
}

void CCommonSwitch::Tick(_float fTimeDelta)
{
	//if (!m_bDone && static_pointer_cast<CSwitchButton>(m_pButtonObj)->Get_SwtichOn() == false)
	//{
	//	static_pointer_cast<CGimmickBase> (m_vecRelatives.back())->Execute();
	//	m_bDone = true;
	//}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CCommonSwitch::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

	if (m_bOn)
		GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CCommonSwitch::Render()
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

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CCommonSwitch::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CCommonSwitch::TakeDamage(const DamageInfo& damageInfo)
{
	if (m_bOn)
	{
		m_bOn = false;
		auto pGI = m_pGameInstance.lock();
		pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_FloorSwitch1", m_pTransformCom->Get_Position());
		pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_FloorSwitch2", m_pTransformCom->Get_Position());

		_float4x4 vWorldMat = m_pTransformCom->Get_WorldMatrix();

		_vector vPos = XMVector4Normalize(GAMEINSTANCE->Get_CamPosition_Vector() - (m_pTransformCom->Get_Position()));
		vWorldMat._41 += XMVectorGetX(vPos) * 2.f;
		vWorldMat._42 += XMVectorGetY(vPos) * 2.f;
		vWorldMat._43 += XMVectorGetZ(vPos) * 2.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect("Font_On_Effect", &vWorldMat, 1.f, true);

		//static_pointer_cast<CGimmickBase> (m_vecRelatives.back())->Execute();

		for(auto& pRelative : m_vecRelatives)
			static_pointer_cast<CGimmickBase>(pRelative)->Execute();

		m_pButtonObj->Set_Dead(true);
	}
}

void CCommonSwitch::Edit_Attribute()
{
}

void CCommonSwitch::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	m_vecRelatives.emplace_back(pRelativeObject);
}

void CCommonSwitch::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
	if (iter != m_vecRelatives.end())
		m_vecRelatives.erase(iter);
}

void CCommonSwitch::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	Save_Relatives(os);
}

void CCommonSwitch::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	Load_Relatives(is);
}

HRESULT CCommonSwitch::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CommonSwitch_Off"), TEXT("Model_Com")));

	CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
	sphereDesc.fRadius = 0.5f;
	sphereDesc.vCenter = { 1.f, 1.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Com"), &sphereDesc));

	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));
	_float3 vWorldSize{};
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix_XMMat();
	vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
	vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
	vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));

	_matrix scaleMatrix = XMMatrixScaling(vWorldSize.x, vWorldSize.y, vWorldSize.z);
	_float4x4 fScaleMat{};
	XMStoreFloat4x4(&fScaleMat, scaleMatrix);
	PxVec3 pscale = { vWorldSize.x, vWorldSize.y, vWorldSize.z };
	string strTag = string(CCommonSwitch::ObjID.begin(), CCommonSwitch::ObjID.end());
	auto pMesh = m_pModelCom->Get_Mesh(0);
	pMesh->Cooking_TriangleMesh(fScaleMat);
	auto pTriangleMesh = pMesh->Get_TriangleMesh();
	if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
		auto pRigid = m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, strTag.c_str());

	if (!m_pShaderCom || !m_pModelCom || !m_pColliderCom || !m_pPhysXCollider)
		assert(false);

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
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
	m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

	return S_OK;
}

HRESULT CCommonSwitch::Ready_Parts()
{
	_uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

	CPartObject::PARTOBJ_DESC partDesc{};
	partDesc.fRotationPerSec = 0.f;
	partDesc.fSpeedPerSec = 0.f;
	partDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	partDesc.pParentTransform = m_pTransformCom;

	m_pButtonObj = GAMEINSTANCE->Add_Clone_Return(iLevelIndex, TEXT("Layer_Map"), CSwitchButton::ObjID, &partDesc);
	
	assert(m_pButtonObj);

	return S_OK;
}

HRESULT CCommonSwitch::Bind_ShaderResources()
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

shared_ptr<CCommonSwitch> CCommonSwitch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CCommonSwitch
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCommonSwitch(pDevice, pContext)
        {}
    };

    shared_ptr<CCommonSwitch> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCommonSwitch"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CCommonSwitch::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CCommonSwitch
    {
        MakeSharedEnabler(const CCommonSwitch& rhs) : CCommonSwitch(rhs)
        {}
    };

    shared_ptr<CCommonSwitch> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCommonSwitch"));
        assert(false);
    }

    return pInstance;
}

void CCommonSwitch::Free()
{
}
