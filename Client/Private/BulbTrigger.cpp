#include "BulbTrigger.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"

#include "Beat_Manager.h"
#include "MovingWall.h"
#include "SmallElevator.h"
#include "CUi_Mgr.h"
#include "CUi_MapInterAction.h"

#include "MapManager.h"
#include "GameManager.h"

wstring CBulbTrigger::ObjID = TEXT("CBulbTrigger");

CBulbTrigger::CBulbTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CBulbTrigger::CBulbTrigger(const CBulbTrigger& rhs)
    : CGimmickBase{rhs}
{
}

CBulbTrigger::~CBulbTrigger()
{
    Free();
}

HRESULT CBulbTrigger::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_BulbTrigger"), TEXT("../Bin/Resources/Models/Map/Gimmick/BulbTrigger.fbx"), MODEL_TYPE::ANIM, fMat)
	);

    return S_OK;
}

HRESULT CBulbTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	m_pModelCom->Set_AnimIndex(3, true);

	// For effect handling
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach)
			CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
		else
			CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
		}
	);

	if(GAMEMANAGER->Get_Current_Level() != LEVEL_MAPTOOL)
		Initialize_Sign();

	return S_OK;
}

void CBulbTrigger::PriorityTick(_float fTimeDelta)
{
}

void CBulbTrigger::Tick(_float fTimeDelta)
{
	//if (GAMEINSTANCE->Get_KeyDown(DIK_O))
	//{
	//	m_bReady = false;
	//
	//	m_eState = SPIN_BEGIN;
	//	m_pModelCom->Set_AnimIndex(0, false);
	//
	//	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	//}

	if (m_bAnimDone && !m_bDone)
		Move_Platforms();

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

	_float4x4 fMat = {};
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);

	Set_Animation();

	
	// Call all reserved effects
	m_pModelCom->PlayReservedCallbacks();
}

void CBulbTrigger::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

	if (!m_bDone)
		GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLIDER_TYPE::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CBulbTrigger::Render()
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
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

		m_pShaderCom->Begin(0);
		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

    return S_OK;
}

void CBulbTrigger::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (m_bReady && pOther->Get_ColliderTag() == TEXT("Player_Attack"))
	{
		//m_bReady = false;
		//
		//m_eState = SPIN_BEGIN;
		//m_pModelCom->Set_AnimIndex(0, false);
	}
}

void CBulbTrigger::TakeDamage(const DamageInfo& damageInfo)
{
	if (m_bReady)
	{
		m_bReady = false;

		m_eState = SPIN_BEGIN;
		m_pModelCom->Set_AnimIndex(0, false);
	}
}

void CBulbTrigger::Edit_Attribute()
{
}

void CBulbTrigger::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	m_vecRelatives.emplace_back(pRelativeObject);
}

void CBulbTrigger::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
	if (iter != m_vecRelatives.end())
		m_vecRelatives.erase(iter);
}

void CBulbTrigger::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	Save_Relatives(os);
}

void CBulbTrigger::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	Load_Relatives(is);
}

void CBulbTrigger::Initialize_Sign()
{
	CUi_MapInterAction::Desc Desc;
	Desc.fAwareDistance = 10;
	Desc.fWorkDistance = 5;
	Desc.OwnerTransform = m_pTransformCom;
	Desc.RelativeDistance = { 0, 60 };
	Desc.iWorkTextureIndex = CUi_MapInterAction::WorkTexture::Left;
	m_pSign = static_pointer_cast<CUi_MapInterAction>
		(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_MapInterAction::ObjID, m_iLevelIndex, L"CUi", CUi_MapInterAction::ObjID, &Desc));
}

HRESULT CBulbTrigger::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_BulbTrigger"), TEXT("Model_Com")));

	CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
	sphereDesc.fRadius = 0.5f;
	sphereDesc.vCenter = { 1.f, 1.5f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Com"), &sphereDesc));

	if (!m_pShaderCom || !m_pModelCom || !m_pColliderCom)
		assert(false);

    return S_OK;
}

HRESULT CBulbTrigger::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	auto pGameInstance = m_pGameInstance.lock();
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

void CBulbTrigger::Set_Animation()
{
	switch (m_eState)
	{
	case Client::CBulbTrigger::SPIN_BEGIN:
		if (m_pModelCom->Get_Finished())
		{
			m_pSign->Dead();
			m_pSign = nullptr;
			m_eState = SPINNING;
			m_pModelCom->Set_AnimIndex(1, false);
		}
		break;
	case Client::CBulbTrigger::SPINNING:
		if (m_pModelCom->Get_Finished())
		{
			m_eState = SPIN_END;
			m_pModelCom->Set_AnimIndex(2, false);
		}
		break;
	case Client::CBulbTrigger::SPIN_END:
		if (m_pModelCom->Get_Finished())
		{
			m_eState = ST_END;
			m_bAnimDone = true;

			m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
			static_pointer_cast<CElevator>(m_vecRelatives.back())->Execute();
		}
		break;
	}
}

void CBulbTrigger::Move_Platforms()
{
	_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurBeat)
	{
		m_iPrevBeat = iCurBeat;

		static_pointer_cast<CMovingWall>(m_vecRelatives[m_iCurPlatformIndex])->Execute();
		++m_iCurPlatformIndex;

		if (m_iCurPlatformIndex == ELEVATOR)
		{
			m_bDone = true;
			m_bExecuted = false;
		}
	}
}

shared_ptr<CBulbTrigger> CBulbTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBulbTrigger
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBulbTrigger(pDevice, pContext)
		{}
	};

	shared_ptr<CBulbTrigger> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBulbTrigger"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CBulbTrigger::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBulbTrigger
	{
		MakeSharedEnabler(const CBulbTrigger& rhs) : CBulbTrigger(rhs)
		{}
	};

	shared_ptr<CBulbTrigger> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBulbTrigger"));
		assert(false);
	}

	return pInstance;
}

void CBulbTrigger::Free()
{
}
