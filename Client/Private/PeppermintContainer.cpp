#include "PeppermintContainer.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Bounding_AABB.h"
#include "PlayerManager.h"
#include "Peppermint.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Chai.h"
#include "Player_Camera.h"
#include "CUi_Mgr.h"
#include "CUi_TalkSystem.h"
wstring CPeppermintContainer::ObjID = TEXT("CPeppermintContainer");
CPeppermintContainer::CPeppermintContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CPeppermintContainer::CPeppermintContainer(const CPeppermintContainer& rhs)
	: CGimmickBase{ rhs }
{
}

CPeppermintContainer::~CPeppermintContainer()
{
	Free();
}

void CPeppermintContainer::Execute(_bool bExecute)
{
	//if (m_bExecuted == bExecute) return;

	m_bExecuted = bExecute;

	m_iAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_iAnimIndex, false); 

	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	m_iFirstBeat = m_iPrevBeat;
	auto pPeppermint = static_pointer_cast<CPeppermint>(PLAYERMANAGER->Get_Assist(L"Peppermint"));
	if (pPeppermint)
	{
		pPeppermint->Play_Animation("EV1105_90_000_ch1000_00", false);
		pPeppermint->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
	}

}

HRESULT CPeppermintContainer::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PeppermintContainer"), TEXT("../Bin/Resources/Models/Map/Gimmick/PeppermintContainer.fbx"), MODEL_TYPE::ANIM, fMat)
	);


	return S_OK;
}

HRESULT CPeppermintContainer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_iAnimIndex, false);
	_float4x4 fMat{};
	// For effect handling
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach)
			CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fPlaySpeed, isBillBoard);
		else
			CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fPlaySpeed, isBillBoard);
		}
	);
	m_pModelCom->Play_Animation(0.00001f, 0.00001f, 0.1f, fMat);

	return S_OK;
}

void CPeppermintContainer::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		_float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;
		_float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

		m_fPlaySpeed = fPlayTime / fBeat / fTotalBeat;
	}
}

void CPeppermintContainer::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
	//	Execute();

	if (m_bExecuted)
	{
		_float4x4 fMat{};

		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fPlaySpeed, 0.1f, fMat);

		if (m_pModelCom->Get_Finished())
		{
			Done();
		}
	}

	m_pColliderCom->Update(XMLoadFloat4x4(&m_ColliderOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
	m_pModelCom->PlayReservedCallbacks();
}

void CPeppermintContainer::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	auto pGI = m_pGameInstance.lock();
	if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 50.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	
	if (!m_bExecuted && !m_bDone)
		pGI->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CPeppermintContainer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);

		m_pModelCom->Render((_uint)i);
	}
#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif
	return S_OK;
}

void CPeppermintContainer::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	m_vecRelatives.emplace_back(pRelativeObject);
}

void CPeppermintContainer::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
	if (iter != m_vecRelatives.end())
		m_vecRelatives.erase(iter);
}

void CPeppermintContainer::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		PLAYERMANAGER->Callback_Event("PeppermintContainer");
		Execute();

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
				wstring strTalkTag = wstring(L"STKale Talk_File_1.csv");
				PRequiredTalkSystem->LoadTalkFile(strTalkTag);

				CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
			}
		}

	}
}

void CPeppermintContainer::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CPeppermintContainer::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CPeppermintContainer::Edit_Attribute()
{
	_vector vScale, vRot, vPos;
	XMMatrixDecompose(&vScale, &vRot, &vPos, XMLoadFloat4x4(&m_ColliderOffset));

	if (ImGui::InputFloat3("pos1", vPos.m128_f32))
	{
		XMStoreFloat4x4(&m_ColliderOffset, XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vPos));
	}
}

void CPeppermintContainer::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	Save_Relatives(os);

	_float4 vPos{ m_ColliderOffset._41, m_ColliderOffset._42, m_ColliderOffset._43, 1.f };
	write_typed_data(os, vPos);
}

void CPeppermintContainer::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	Load_Relatives(is);

	_float4 vPos;
	read_typed_data(is, vPos);
	m_ColliderOffset._41 = vPos.x;
	m_ColliderOffset._42 = vPos.y;
	m_ColliderOffset._43 = vPos.z;
	m_ColliderOffset._44 = vPos.w;
}

HRESULT CPeppermintContainer::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PeppermintContainer"), TEXT("Model_Com")));

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
	aabbDesc.vExtents = { 5.0f, 8.0f, 5.0f };
	aabbDesc.vCenter = { 0.f, 0.0f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CPeppermintContainer::Bind_ShaderResources()
{
	auto pGameInstance = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CPeppermintContainer> CPeppermintContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CPeppermintContainer
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPeppermintContainer(pDevice, pContext)
		{}
	};

	shared_ptr<CPeppermintContainer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPeppermintContainer"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CPeppermintContainer::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CPeppermintContainer
	{
		MakeSharedEnabler(const CPeppermintContainer& rhs) : CPeppermintContainer(rhs)
		{}
	};

	shared_ptr<CPeppermintContainer> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPeppermintContainer"));
		assert(false);
	}

	return pInstance;
}

void CPeppermintContainer::Free()
{
}
