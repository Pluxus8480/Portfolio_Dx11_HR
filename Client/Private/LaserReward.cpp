#include "LaserReward.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "CUi_PlayerBar.h"
#include "CUi_TalkSystem.h"

#include "Beat_Manager.h"

wstring CLaserReward::ObjID = TEXT("CLaserReward");

CLaserReward::CLaserReward(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CLaserReward::CLaserReward(const CLaserReward& rhs)
	: CGimmickBase{ rhs }
{
}

CLaserReward::~CLaserReward()
{
	Free();
}

void CLaserReward::Execute(_bool bExecute)
{
	m_bExecuted = true;
}

HRESULT CLaserReward::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LaserReward_Anim"), TEXT("../Bin/Resources/Models/Map/Gimmick/LaserReward.fbx"),
			MODEL_TYPE::ANIM, fMat)
	);
	XMStoreFloat4x4(&fMat, XMMatrixScaling(1.f, 1.f, 1.f));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LaserReward2"), TEXT("../Bin/Resources/Models/Map/Gimmick/LaserReward2.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);

	//m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>());
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("ST1LaserSpeaker2_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/Stage1/ST1LaserSpeaker2_1.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("ST1LaserSpeaker2_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/Stage1/ST1LaserSpeaker2_2.ogg")));

	return S_OK;
}

HRESULT CLaserReward::Initialize(void* pArg)
{
	//COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	//pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	//m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	PxVec3 BoxDimension{ 1.5f,0.15f,1.5f };
	_float3 vScale = m_pTransformCom->Get_Scale();
	BoxDimension.x *= vScale.x;
	BoxDimension.y *= vScale.y;
	BoxDimension.z *= vScale.z;
	PxBoxGeometry Box{ BoxDimension };
	m_pPhysxCollider->createStaticShapeActor(Box, "");

	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();

	m_pBoneMat = m_pModelCom->Get_BoneMatrixPtr("elevater");
	m_pModelCom->Set_AnimIndex(2, false);

	GAMEINSTANCE->Add_Unique_Sound_Channel("ST1_Laser_2");

	return S_OK;
}

void CLaserReward::PriorityTick(_float fTimeDelta)
{
}

void CLaserReward::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_O))
	//{
	//	Execute();
	//}

	_float4x4 matOut;
	_float fOneBeat = m_pModelCom->Get_Animation_PlayTime() / CBeat_Manager::Get_Instance()->Get_Beat();

	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fOneBeat, 0.1f, matOut);
	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
	PxMat44 mat;

	mat.column0.x = Transpose.r[0].m128_f32[0];
	mat.column0.y = Transpose.r[0].m128_f32[1];
	mat.column0.z = Transpose.r[0].m128_f32[2];
	mat.column0.w = Transpose.r[0].m128_f32[3];
	mat.column1.x = Transpose.r[1].m128_f32[0];
	mat.column1.y = Transpose.r[1].m128_f32[1];
	mat.column1.z = Transpose.r[1].m128_f32[2];
	mat.column1.w = Transpose.r[1].m128_f32[3];
	mat.column2.x = Transpose.r[2].m128_f32[0];
	mat.column2.y = Transpose.r[2].m128_f32[1];
	mat.column2.z = Transpose.r[2].m128_f32[2];
	mat.column2.w = Transpose.r[2].m128_f32[3];
	mat.column3.x = Transpose.r[3].m128_f32[0];
	mat.column3.y = Transpose.r[3].m128_f32[1];
	mat.column3.z = Transpose.r[3].m128_f32[2];
	mat.column3.w = Transpose.r[3].m128_f32[3];

	PxTransform transform{ mat };
	m_pPhysxCollider->Get_Static_RigidBody()->setGlobalPose(transform);


	if (m_bExecuted)
	{
		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
		}
		if (m_pModelCom->Get_CurrentAnimIndex() == 2)
		{
			m_pModelCom->Set_AnimIndex(0, false);
		}

		m_pColliderCom->Update(Transpose);
	}
	else
	{
		m_pModelCom->Set_AnimIndexNonCancle(2, false);
	}
}

void CLaserReward::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	if (m_bExecuted)
	{
		GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
	}
}

HRESULT CLaserReward::Render()
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
	if(!m_bExhausted)
	{
		iNumMeshes = m_pRewardModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_bool bNoEmit = false;

			if (FAILED(m_pRewardModelCom->Bind_Material(m_pNonAnimShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pRewardModelCom->Bind_Material(m_pNonAnimShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			{
				bNoEmit = true;
			}
			if (bNoEmit)
				m_pNonAnimShaderCom->Begin((_uint)MESH_PASS::CCW);
			else
				m_pNonAnimShaderCom->Begin((_uint)MESH_PASS::Emission);
			m_pRewardModelCom->Render((_uint)i);
		}
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CLaserReward::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (!m_bReverb)
	{
		static_pointer_cast<CUi_PlayerBar>(CUi_Mgr::Get_Instance()->Find_UI(CUi_PlayerBar::ObjID))->Enable_Reverb();
		m_bReverb = true;
	}
	if (!m_bExhausted)
	{
		DamageInfo dmg{};
		dmg.eAttackType = EAttackType::ELECTRIC;
		dmg.fAmountDamage = 1.f;
		dmg.pObjectOwner = shared_from_this();
		pOther->Get_Owner()->TakeDamage(dmg);
	}
	m_bExhausted = true;
}

void CLaserReward::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CLaserReward::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (!m_bTalk)
	{
		m_bTalk = true;
		m_isTalkOver = false;

		auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
		if (pTalkSystem == nullptr)
		{
			CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
			TalkDesc.endCallback = [&]() {
				m_isTalkOver = true;
				};

			pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_TEST, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
		}

		auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
		if (PRequiredTalkSystem != nullptr)
		{
			PRequiredTalkSystem->LoadTalkFile(L"ST1_LaserSpeaker Talk_File_2.csv");

			CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
		}
	}
}

void CLaserReward::Edit_Attribute()
{


}

void CLaserReward::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
}

void CLaserReward::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
}

HRESULT CLaserReward::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	m_pNonAnimShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com2")));

	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LaserReward_Anim"), TEXT("Model_Com")));
	m_pRewardModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LaserReward2"), TEXT("Model_Com2")));

	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vExtents = { 1.5f, 3.0f, 1.5f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));
	

	assert(m_pNonAnimShaderCom);
	assert(m_pRewardModelCom);

	m_pPhysxCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysxCol_Com")));
	assert(m_pPhysxCollider);
	if (!m_pShaderCom || !m_pModelCom /* || !m_pColliderCom */)
		assert(false);

	return S_OK;
}

HRESULT CLaserReward::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//    return E_FAIL;
	_float4x4 Mat;
	XMStoreFloat4x4(&Mat, XMMatrixTranslationFromVector(XMLoadFloat4x4(m_pBoneMat).r[3]) * m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pNonAnimShaderCom->Bind_Matrix("g_WorldMatrix", &Mat);


	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	m_pNonAnimShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint));
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	return S_OK;
}

shared_ptr<CLaserReward> CLaserReward::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLaserReward
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLaserReward(pDevice, pContext)
		{}
	};

	shared_ptr<CLaserReward> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLaserReward"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CLaserReward::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLaserReward
	{
		MakeSharedEnabler(const CLaserReward& rhs) : CLaserReward(rhs)
		{}
	};

	shared_ptr<CLaserReward> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLaserReward"));
		assert(false);
	}

	return pInstance;
}

void CLaserReward::Free()
{
}
