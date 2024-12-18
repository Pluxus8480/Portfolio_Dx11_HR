#include "TriggerBox_Level.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"
#include "PickUpItem.h"
#include "PlayerManager.h"
#include "Player_Camera.h"

#include "Beat_Manager.h"


wstring CTriggerBox_Level::ObjID = TEXT("CTriggerBox_Level");

CTriggerBox_Level::CTriggerBox_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CTriggerBox_Level::CTriggerBox_Level(const CTriggerBox_Level& rhs)
	: CGimmickBase{ rhs }
{
}

CTriggerBox_Level::~CTriggerBox_Level()
{
	Free();
}

void CTriggerBox_Level::Execute(_bool bExecute)
{
	m_bExecuted = bExecute;
}

HRESULT CTriggerBox_Level::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));


	return S_OK;
}

HRESULT CTriggerBox_Level::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CTriggerBox_Level::PriorityTick(_float fTimeDelta)
{
}

void CTriggerBox_Level::Tick(_float fTimeDelta)
{

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();

	m_pColliderCom->Update(Transpose);
}

void CTriggerBox_Level::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CTriggerBox_Level::Render()
{
#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif
	return S_OK;
}

void CTriggerBox_Level::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == CHAI_COLLIDER_HIT && !m_bUsedUp)
	{
		m_bUsedUp = true;
		m_pGameInstance.lock()->Change_Level_Delayed(m_iNextLevel);
	}
}

void CTriggerBox_Level::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CTriggerBox_Level::Edit_Attribute()
{
	const char* items1[] = {
	"LEVEL_STATIC",
	"LEVEL_LOADING",
	"LEVEL_LOGO",
	"Level_Test",
	"LEVEL_EFFECTTOOL",
	"LEVEL_ANIMTOOL",
	"LEVEL_MAPTOOL",
	"LEVEL_UI",
	"LEVEL_CINETOOL",
	"LEVEL_LIFT",
	"LEVEL_KORSICA",
	"LEVEL_MODELTEST",
	"LEVEL_KALEBOSS",
	"LEVEL_MIMOSA",
	"LEVEL_GIMMICTEST",
	"LEVEL_KALESTAGE" };
	ImGui::ListBox("NextLevel", &m_iSignedLevel ,items1, IM_ARRAYSIZE(items1));

}

void CTriggerBox_Level::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
	write_typed_data(os, m_iSignedLevel);
}

void CTriggerBox_Level::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
	read_typed_data(is, m_iSignedLevel);
	m_iNextLevel = (_uint)m_iSignedLevel;
}

void CTriggerBox_Level::Push_From_Pool(void* pArg)
{

}

void CTriggerBox_Level::Dying()
{

}

void CTriggerBox_Level::TakeDamage(const DamageInfo& damageInfo)
{
}

HRESULT CTriggerBox_Level::Ready_Components()
{
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vExtents = { .5f, .5f, .5f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	assert(m_pColliderCom);


	return S_OK;
}

HRESULT CTriggerBox_Level::Bind_ShaderResources()
{
	return S_OK;
}

shared_ptr<CTriggerBox_Level> CTriggerBox_Level::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CTriggerBox_Level
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTriggerBox_Level(pDevice, pContext)
		{}
	};

	shared_ptr<CTriggerBox_Level> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTriggerBox_Level"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CTriggerBox_Level::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CTriggerBox_Level
	{
		MakeSharedEnabler(const CTriggerBox_Level& rhs) : CTriggerBox_Level(rhs)
		{}
	};

	shared_ptr<CTriggerBox_Level> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTriggerBox_Level"));
		assert(false);
	}

	return pInstance;
}

void CTriggerBox_Level::Free()
{
}
