#include "WoodenBox.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"
#include "PickUpItem.h"

#include "Beat_Manager.h"

wstring CWoodenBox::ObjID = TEXT("CWoodenBox");

CWoodenBox::CWoodenBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CWoodenBox::CWoodenBox(const CWoodenBox& rhs)
	: CGimmickBase{ rhs }
{
}

CWoodenBox::~CWoodenBox()
{
	Free();
}

void CWoodenBox::Execute(_bool bExecute)
{
	m_bExecuted = true;

	for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
	{
		PxVec3 vForce{ GAMEINSTANCE->Random_Float(-10.0f, 10.f), GAMEINSTANCE->Random_Float(-5.0f, 5.f), GAMEINSTANCE->Random_Float(-10.0f, 10.f) };

		m_vecPhysxCollider[i]->Get_Dynamic_RigidBody()->addForce(vForce, PxForceMode::eVELOCITY_CHANGE);
	}
	_int iNumItem = GAMEINSTANCE->Random_Int(5, 8);
	COMMONOBJ_DESC desc{};
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 0.f;
	XMStoreFloat4(&desc.vPosition, m_pTransformCom->Get_Position());
	XMStoreFloat4x4(&desc.worldMatrix, m_pTransformCom->Get_WorldMatrix_XMMat());
	if (m_ePickupType == GEAR)
	{
		for (_int i = 0; i < iNumItem; ++i)
		{
			auto pPickup = static_pointer_cast<CPickUpItem>( GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(),L"PickUp", CPickUpItem::ObjID, &desc));
			CPickUpItem::PICKUP_DESC PUDESC;
			PUDESC.eSize = (CPickUpItem::SIZE)(rand() % 2);
			PUDESC.eType = CPickUpItem::GEAR;
			pPickup->Model_Change(PUDESC);
			pPickup->Execute();
		}
	}
	else
	{
		for (_int i = 0; i < iNumItem; ++i)
		{
			auto pPickup = static_pointer_cast<CPickUpItem>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"PickUp", CPickUpItem::ObjID, &desc));
			CPickUpItem::PICKUP_DESC PUDESC;
			PUDESC.eSize = (CPickUpItem::SIZE)(rand() % 2);
			PUDESC.eType = CPickUpItem::HP;
			pPickup->Model_Change(PUDESC);
			pPickup->Execute();
		}
	}
	m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"WoodenBoxBroken", m_pTransformCom->Get_Position());
}

HRESULT CWoodenBox::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_WoodenBox_Anim"), TEXT("../Bin/Resources/Models/Map/Gimmick/WoodenBox.fbx"),
			MODEL_TYPE::ANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_WoodenBox_2_Anim"), TEXT("../Bin/Resources/Models/Map/Gimmick/WoodenBox_2.fbx"),
			MODEL_TYPE::ANIM, fMat)
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("WoodenBoxBroken"), TEXT("../Bin/Resources/Sounds/SFX/WoodenBox/BoxBroken.ogg")));
	
	return S_OK;
}

HRESULT CWoodenBox::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components((_float4x4*)pArg)))
		return E_FAIL;


	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();



	return S_OK;
}

void CWoodenBox::PriorityTick(_float fTimeDelta)
{
}

void CWoodenBox::Tick(_float fTimeDelta)
{

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();

	m_pColliderCom->Update(Transpose);


	if (m_bExecuted)
	{
		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
		}
		for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
		{
			PxTransform Pose = m_vecPhysxCollider[i]->Get_Dynamic_RigidBody()->getGlobalPose();
			PxMat44 mat{ Pose };
			_matrix Bone{};
			Bone.r[0].m128_f32[0] = mat.column0.x;
			Bone.r[0].m128_f32[1] = mat.column0.y;
			Bone.r[0].m128_f32[2] = mat.column0.z;
			Bone.r[0].m128_f32[3] = mat.column0.w;
			Bone.r[1].m128_f32[0] = mat.column1.x;
			Bone.r[1].m128_f32[1] = mat.column1.y;
			Bone.r[1].m128_f32[2] = mat.column1.z;
			Bone.r[1].m128_f32[3] = mat.column1.w;
			Bone.r[2].m128_f32[0] = mat.column2.x;
			Bone.r[2].m128_f32[1] = mat.column2.y;
			Bone.r[2].m128_f32[2] = mat.column2.z;
			Bone.r[2].m128_f32[3] = mat.column2.w;
			Bone.r[3].m128_f32[0] = mat.column3.x;
			Bone.r[3].m128_f32[1] = mat.column3.y;
			Bone.r[3].m128_f32[2] = mat.column3.z;
			Bone.r[3].m128_f32[3] = mat.column3.w;
			Bone = Bone * XMMatrixInverse(nullptr, Transpose);
			_float4x4 Temp;
			XMStoreFloat4x4(&Temp, Bone);
			m_pModelCom->Set_BoneMatrixPtr(i + 1, Temp);
		}
		m_fLifeTime -= fTimeDelta;
		if (m_fLifeTime <= 0.f)
		{
			for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
			{
				m_vecPhysxCollider[i]->Get_Dynamic_RigidBody()->putToSleep();
			}
			Dead();
		}
	}
	else
	{
		for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
		{
			_matrix Bone = XMLoadFloat4x4(m_vecBones[i]) * Transpose;
			Bone.r[0] = XMVector3Normalize(Bone.r[0]);
			Bone.r[1] = XMVector3Normalize(Bone.r[1]);
			Bone.r[2] = XMVector3Normalize(Bone.r[2]);
			PxMat44 mat;
			mat.column0.x = Bone.r[0].m128_f32[0];
			mat.column0.y = Bone.r[0].m128_f32[1];
			mat.column0.z = Bone.r[0].m128_f32[2];
			mat.column0.w = Bone.r[0].m128_f32[3];
			mat.column1.x = Bone.r[1].m128_f32[0];
			mat.column1.y = Bone.r[1].m128_f32[1];
			mat.column1.z = Bone.r[1].m128_f32[2];
			mat.column1.w = Bone.r[1].m128_f32[3];
			mat.column2.x = Bone.r[2].m128_f32[0];
			mat.column2.y = Bone.r[2].m128_f32[1];
			mat.column2.z = Bone.r[2].m128_f32[2];
			mat.column2.w = Bone.r[2].m128_f32[3];
			mat.column3.x = Bone.r[3].m128_f32[0];
			mat.column3.y = Bone.r[3].m128_f32[1];
			mat.column3.z = Bone.r[3].m128_f32[2];
			mat.column3.w = Bone.r[3].m128_f32[3];

			PxTransform transform{ mat };
			if (!transform.isValid())
				continue;
			m_vecPhysxCollider[i]->Get_Dynamic_RigidBody()->setGlobalPose(transform);
			m_vecPhysxCollider[i]->Get_Dynamic_RigidBody()->setForceAndTorque({ 0.f,0.f,0.f }, { 0.f,0.f,0.f }, PxForceMode::eVELOCITY_CHANGE);
		}
	}
}

void CWoodenBox::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CWoodenBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}
		if (bNoEmit)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CWoodenBox::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CWoodenBox::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CWoodenBox::Edit_Attribute()
{
	ImGui::InputInt("PickupType", &m_ePickupType);

}

void CWoodenBox::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
	write_typed_data(os, m_ePickupType);


}

void CWoodenBox::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
	read_typed_data(is, m_ePickupType);
	m_Components[TEXT("Model_Com")] = nullptr;
	if (m_ePickupType == GEAR)
	{
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WoodenBox_Anim"), TEXT("Model_Com")));
	}
	else
	{
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WoodenBox_2_Anim"), TEXT("Model_Com")));
	}	
	_float4x4 matOut;
	m_pModelCom->Play_Animation(0.1f, 0.1f, 0.1f, matOut);
}

void CWoodenBox::Push_From_Pool(void* pArg)
{
	for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
	{
		m_vecPhysxCollider[i]->Add_To_Scene();
	}
}

void CWoodenBox::Dying()
{
	for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
	{
		m_vecPhysxCollider[i]->Remove_To_Scene();
	}
}

void CWoodenBox::TakeDamage(const DamageInfo& damageInfo)
{
	Execute();
}

HRESULT CWoodenBox::Ready_Components(_float4x4* pMat)
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	if (m_ePickupType == GEAR)
	{
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WoodenBox_Anim"), TEXT("Model_Com")));
	}
	else
	{
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WoodenBox_2_Anim"), TEXT("Model_Com")));
	}
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, .5f, 0.f };
	obbDesc.vExtents = { .5f, .5f, .5f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));


	if (!m_pShaderCom || !m_pModelCom /* || !m_pColliderCom */)
		assert(false);


	m_pModelCom->Set_AnimIndex(0, false);
	_float4x4 matOut;
	m_pModelCom->Play_Animation(0.1f, 0.1f, 0.1f, matOut);
	_uint iNumBone = m_pModelCom->Get_NumBones();
	for (_uint i = 1; i < iNumBone; ++i)
	{
		m_vecBones.push_back(m_pModelCom->Get_BoneMatrixPtr(i));
	}


	PxVec3 BoxDimension{ 0.1f,0.1f,0.1f };
	_float3 vScale = m_pTransformCom->Get_Scale();
	BoxDimension.x *= vScale.x;
	BoxDimension.y *= vScale.y;
	BoxDimension.z *= vScale.z;
	PxBoxGeometry Box{ BoxDimension };
	m_vecPhysxCollider.resize(m_vecBones.size());
	for (_uint i = 0; i < m_vecBones.size(); ++i)
	{
		m_vecPhysxCollider[i] = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), _wstring(TEXT("PhysxCol_Com")) + to_wstring(i)));
		m_vecPhysxCollider[i]->createDynamicShapeActor(Box, nullptr);
	}

	//m_pPhysxCollider->createStaticShapeActor(Box, "");


	return S_OK;
}

HRESULT CWoodenBox::Bind_ShaderResources()
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

shared_ptr<CWoodenBox> CWoodenBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CWoodenBox
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CWoodenBox(pDevice, pContext)
		{}
	};

	shared_ptr<CWoodenBox> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWoodenBox"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CWoodenBox::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CWoodenBox
	{
		MakeSharedEnabler(const CWoodenBox& rhs) : CWoodenBox(rhs)
		{}
	};

	shared_ptr<CWoodenBox> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWoodenBox"));
		assert(false);
	}

	return pInstance;
}

void CWoodenBox::Free()
{
}
