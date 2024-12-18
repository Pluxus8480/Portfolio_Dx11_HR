#include "Smoke_Cover.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"

#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "CharacterController.h"
#include "Chai.h"
#include "MapManager.h"
#include "Effect_Union.h"

wstring CSmoke_Cover::ObjID = TEXT("CSmoke_Cover");

CSmoke_Cover::CSmoke_Cover(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CSmoke_Cover::CSmoke_Cover(const CSmoke_Cover& rhs)
	: CGimmickBase{ rhs }
{
}

CSmoke_Cover::~CSmoke_Cover()
{
	Free();
}

void CSmoke_Cover::Execute(_bool bExecute)
{
	if (m_bExecuted = bExecute) return;

	m_bExecuted = true;
}

HRESULT CSmoke_Cover::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Smoke_Cover"), TEXT("../Bin/Resources/Models/Map/Gimmick/Smoke_Cover.fbx"),
			MODEL_TYPE::ANIM, fMat)
	);

	return S_OK;
}

HRESULT CSmoke_Cover::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	PxVec3 BoxDimension{ 5.f,0.5f,5.0f };
	_float3 vScale = m_pTransformCom->Get_Scale();
	BoxDimension.x *= vScale.x;
	BoxDimension.y *= vScale.y;
	BoxDimension.z *= vScale.z;
	PxBoxGeometry Box{ BoxDimension };
	m_pPhysxCollider->createStaticShapeActor(Box, "");

	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();

	m_pModelCom->Set_AnimIndex(10, false);
	m_pBone = m_pModelCom->Get_BoneMatrixPtr("root");
	m_iCurrAirTime = m_iAirTime;

	//Execute();

	// For effect handling
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach) {
			auto pEffect = CEffect_Manager::Get_Instance()->Call_Union_Effect_Return(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
			pEffect->SetGlowOff();
		}
		else {
			auto pEffect = CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect_Return(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
			pEffect->SetGlowOff();
		}
		}
	);

	return S_OK;
}

void CSmoke_Cover::PriorityTick(_float fTimeDelta)
{
}

void CSmoke_Cover::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_O))
	//{
	//	Execute();
	//}
	auto pGI = m_pGameInstance.lock();
	_bool bCulled = !pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 3.f);
	if (bCulled)
		return;

	_float4x4 matOut;
	_float fOneBeat = m_pModelCom->Get_Animation_PlayTime() / CBeat_Manager::Get_Instance()->Get_Beat();

	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fOneBeat, 0.1f, matOut);

	if (m_bExecuted)
	{
		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
			if (m_pModelCom->Get_CurrentAnimIndex() == m_iHeight * 3 + 2)
			{
				m_iCurrAirTime -= 1;
				if (m_iCurrAirTime <= 0)
				{
					m_iCurrAirTime = m_iAirTime - 1;
					m_pModelCom->Set_AnimIndex(m_iHeight * 3 + 3, false);
				}
			}
			else if (m_pModelCom->Get_CurrentAnimIndex() == 10)
			{
				m_iCurrAirTime -= 1;
				if (m_iCurrAirTime <= 0)
				{
					m_iCurrAirTime = m_iAirTime;
					m_pModelCom->Set_AnimIndex(0, false);
				}
			}
		}
		if (m_pModelCom->Get_Finished())
		{
			if (m_pModelCom->Get_CurrentAnimIndex() == 10)
			{
				m_pModelCom->Set_AnimIndex(0, false);
			}
			else if (m_pModelCom->Get_CurrentAnimIndex() == 0)
			{
				m_pModelCom->Set_AnimIndex(m_iHeight * 3 + 1, false);
			}
			else if (m_pModelCom->Get_CurrentAnimIndex() == m_iHeight * 3 + 1)
			{
				m_pModelCom->Set_AnimIndex(m_iHeight * 3 + 2, true);
			}
			else
			{
				m_pModelCom->Set_AnimIndex(10, true);
			}
		}
	}

	//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matOut) * m_pTransformCom->Get_WorldMatrix_XMMat());

	_matrix Transpose = XMMatrixTranslation(m_pBone->_41, m_pBone->_42, m_pBone->_43) * m_pTransformCom->Get_WorldMatrix_XMMat();
	Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
	Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
	Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
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

	if (m_bCollision)
	{
		auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));

		_float3 vVelocity = pCCT->Get_Velocity();
		auto vPlayerPos = pCCT->Get_Controller()->getPosition();

		_vector vPos = Transpose.r[3];
		//_vector vPos = m_pTransformCom->Get_Position();
		_float fTopY = XMVectorGetY(vPos);

		if (vVelocity.y <= 0)
		{
			_vector vFootPos = XMVectorSet(vPlayerPos.x, fTopY + 0.25f, vPlayerPos.z, 1.f);

			//pCCT->Set_Position(XMMatrixTranslationFromVector(vFootPos));
			pCCT->Set_FootPosition(XMMatrixTranslationFromVector(vFootPos));
			dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Transform()->Set_Position(vFootPos);
			dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Tick_PartObj(fTimeDelta);
		}
	}

	m_pColliderCom->Update(Transpose);
#ifdef _DEBUG
#endif

	// Call all reserved effects
	m_pModelCom->PlayReservedCallbacks();
}

void CSmoke_Cover::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CSmoke_Cover::Render()
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

void CSmoke_Cover::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CSmoke_Cover::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		auto pCCT = dynamic_pointer_cast<CCharacterController>(PLAYERMANAGER->Get_Player()->Find_Component(L"Com_CCT"));

		_float3 vVelocity = pCCT->Get_Velocity();

		if (dynamic_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Is_OnAir())
			m_bCollision = false;
		else 
			m_bCollision = true;

	}
}

void CSmoke_Cover::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		m_bCollision = false;
	}
}

void CSmoke_Cover::Edit_Attribute()
{
	ImGui::Checkbox("Wait?", &m_bWait);

	ImGui::InputInt("Height", &m_iHeight);
	ImGui::InputInt("Delay", &m_iDelay);
	ImGui::InputInt("AirTime", &m_iAirTime);
}

void CSmoke_Cover::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
	write_typed_data(os, m_iHeight);
	write_typed_data(os, m_iDelay);
	write_typed_data(os, m_iAirTime);

	write_typed_data(os, m_bWait);
}

void CSmoke_Cover::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
	read_typed_data(is, m_iHeight);
	read_typed_data(is, m_iDelay);
	read_typed_data(is, m_iAirTime);

	read_typed_data(is, m_bWait);

	if (m_bWait == false)
		Execute();
}

HRESULT CSmoke_Cover::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Smoke_Cover"), TEXT("Model_Com")));

#ifdef _DEBUG
#endif
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 1.2f, 0.f };
	obbDesc.vExtents = { 2.f, 3.2f, 2.f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };
	
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	m_pPhysxCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysxCol_Com")));
	assert(m_pPhysxCollider);
	if (!m_pShaderCom || !m_pModelCom /* || !m_pColliderCom */)
		assert(false);

	return S_OK;
}

HRESULT CSmoke_Cover::Bind_ShaderResources()
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

shared_ptr<CSmoke_Cover> CSmoke_Cover::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSmoke_Cover
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSmoke_Cover(pDevice, pContext)
		{}
	};

	shared_ptr<CSmoke_Cover> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSmoke_Cover"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CSmoke_Cover::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSmoke_Cover
	{
		MakeSharedEnabler(const CSmoke_Cover& rhs) : CSmoke_Cover(rhs)
		{}
	};

	shared_ptr<CSmoke_Cover> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSmoke_Cover"));
		assert(false);
	}

	return pInstance;
}

void CSmoke_Cover::Free()
{
}
