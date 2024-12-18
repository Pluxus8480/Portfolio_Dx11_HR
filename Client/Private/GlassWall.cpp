#include "GlassWall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"

#include "Beat_Manager.h"

wstring CGlassWall::ObjID = TEXT("CGlassWall");

CGlassWall::CGlassWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CGlassWall::CGlassWall(const CGlassWall& rhs)
	: CGimmickBase{ rhs }
{
}

CGlassWall::~CGlassWall()
{
	Free();
}

void CGlassWall::Execute(_bool bExecute)
{
	m_bExecuted = true;
	m_pPhysxCollider->Remove_To_Scene();

	_vector vDir = m_pTransformCom->Get_NormLook() * -30.f;

	for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
	{
		PxVec3 vForce{vDir.m128_f32[0] * GAMEINSTANCE->Random_Float(1.0f, 2.f), (vDir.m128_f32[1] + 1.f) * GAMEINSTANCE->Random_Float(1.0f, 2.f), vDir.m128_f32[2] * GAMEINSTANCE->Random_Float(1.0f, 2.f) };
		
		m_vecPhysxCollider[i]->Get_Dynamic_RigidBody()->addForce(vForce, PxForceMode::eVELOCITY_CHANGE);
	}
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Glass_Crush");
}

HRESULT CGlassWall::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_GlassWall_Anim"), TEXT("../Bin/Resources/Models/Map/Gimmick/GlassWall.fbx"),
			MODEL_TYPE::ANIM, fMat)
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Sound_Glass_Crush"), TEXT("../Bin/Resources/Sounds/MapObject/Glass_Crush.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Sound_Glass_Breaks"), TEXT("../Bin/Resources/Sounds/MapObject/Glass_Breaks.ogg")));
	
	return S_OK;
}

HRESULT CGlassWall::Initialize(void* pArg)
{
	//COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	//pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	//m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components((_float4x4*)pArg)))
		return E_FAIL;


	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();



	return S_OK;
}

void CGlassWall::PriorityTick(_float fTimeDelta)
{
}

void CGlassWall::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_O))
	//{
	//	Execute();
	//}

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();

	m_pColliderCom->Update(Transpose);


	if (m_bExecuted)
	{
		m_fLifeTime -= fTimeDelta;
		if (m_fLifeTime <= 0.f && !m_bBreak)
		{
			m_bBreak = true;
			m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Glass_Breaks");
			Dead();
		}
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

	}
	else
	{
		{
			_matrix Bone = XMMatrixTranslation(0.f, 3.f, -5.f) * Transpose;
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
			m_pPhysxCollider->Get_Static_RigidBody()->setGlobalPose(transform);
		}

		for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
		{
			_matrix Bone = XMLoadFloat4x4(m_vecBones[i]) * Transpose;
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

void CGlassWall::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_DISTORTION, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CGlassWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pShaderCom->Bind_Float("g_fNoiseStrength", 0.2f);
	m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");
	_float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
	m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
	m_pShaderCom->Bind_Float("g_fTime", 0.f);
	_float4 vColor = { 0.4f,0.2f,0.f,0.5f };
	m_pShaderCom->Bind_Vector4("g_vDistortionColor", &vColor);
	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Noise_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Distortion);
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

void CGlassWall::TakeDamage(const DamageInfo& damageInfo)
{
	if (damageInfo.eAttackType == EAttackType::SP)
		Execute();

}

void CGlassWall::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CGlassWall::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CGlassWall::Edit_Attribute()
{


}

void CGlassWall::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
}

void CGlassWall::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
}

void CGlassWall::Dying()
{
	for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
	{
		m_vecPhysxCollider[i]->Remove_To_Scene();
	}
	m_pPhysxCollider->Remove_To_Scene();
}

void CGlassWall::Push_From_Pool(void* pArg)
{
	for (_uint i = 0; i < m_vecPhysxCollider.size(); ++i)
	{
		m_vecPhysxCollider[i]->Add_To_Scene();
	}
	m_pPhysxCollider->Add_To_Scene();
}

HRESULT CGlassWall::Ready_Components(_float4x4* pMat)
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GlassWall_Anim"), TEXT("Model_Com")));

	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 3.f, -5.f };
	obbDesc.vExtents = { 1.0f, 6.0f, 10.0f };
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
	m_pPhysxCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), _wstring(TEXT("PhysxColMain_Com"))));
	BoxDimension = { 0.1f, 6.f, 10.f };
	Box = PxBoxGeometry{ BoxDimension };
	m_pPhysxCollider->createStaticShapeActor(Box, "");


	return S_OK;
}

HRESULT CGlassWall::Bind_ShaderResources()
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

shared_ptr<CGlassWall> CGlassWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CGlassWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGlassWall(pDevice, pContext)
		{}
	};

	shared_ptr<CGlassWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGlassWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CGlassWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CGlassWall
	{
		MakeSharedEnabler(const CGlassWall& rhs) : CGlassWall(rhs)
		{}
	};

	shared_ptr<CGlassWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGlassWall"));
		assert(false);
	}

	return pInstance;
}

void CGlassWall::Free()
{
}
