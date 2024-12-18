#include "PickUpItem.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Chai.h"

#include "Beat_Manager.h"

wstring CPickUpItem::ObjID = TEXT("CPickUpItem");

CPickUpItem::CPickUpItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CPickUpItem::CPickUpItem(const CPickUpItem& rhs)
	: CGimmickBase{ rhs }
{
}

CPickUpItem::~CPickUpItem()
{
	Free();
}

void CPickUpItem::Execute(_bool bExecute)
{
	m_bExecuted = true;
	m_fLandingPos = XMVectorGetY(m_pTransformCom->Get_Position());
	m_vSpeed.x = GAMEINSTANCE->Random_Float(-0.2f, 0.2f);
	m_vSpeed.y = GAMEINSTANCE->Random_Float(0.5, 1.f);
	m_vSpeed.z = GAMEINSTANCE->Random_Float(-0.2f, 0.2f);
}

void CPickUpItem::Model_Change(const PICKUP_DESC& desc)
{
	if (m_ePickupType == desc.eType && m_iSize == desc.eSize)
		return;

	m_ePickupType = desc.eType;
	m_iSize = desc.eSize;
	m_Components[TEXT("Model_Com")] = nullptr;
	if (m_ePickupType == GEAR)
	{
		_wstring strTag = _wstring(L"Prototype_Component_Model_PickUpItem_Gear_") + to_wstring(m_iSize);
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, strTag, TEXT("Model_Com")));
	}
	else if (m_ePickupType == HP)
	{
		_wstring strTag = _wstring(L"Prototype_Component_Model_PickUpItem_Med_") + to_wstring(m_iSize);
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, strTag, TEXT("Model_Com")));
	}
	else
	{
		_wstring strTag = _wstring(L"Prototype_Component_Model_PickUpItem_Battery_") + to_wstring(m_iSize);
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, strTag, TEXT("Model_Com")));
	}
}

HRESULT CPickUpItem::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Gear_0"), TEXT("../Bin/Resources/Models/Map/Gimmick/Gear_0.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Gear_1"), TEXT("../Bin/Resources/Models/Map/Gimmick/Gear_1.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Gear_2"), TEXT("../Bin/Resources/Models/Map/Gimmick/Gear_2.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Med_0"), TEXT("../Bin/Resources/Models/Map/Gimmick/Med_0.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Med_1"), TEXT("../Bin/Resources/Models/Map/Gimmick/Med_1.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Med_2"), TEXT("../Bin/Resources/Models/Map/Gimmick/Med_2.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Battery_0"), TEXT("../Bin/Resources/Models/Map/Gimmick/Battery_0.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Battery_1"), TEXT("../Bin/Resources/Models/Map/Gimmick/Battery_1.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_PickUpItem_Battery_2"), TEXT("../Bin/Resources/Models/Map/Gimmick/Battery_2.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Gear_Pickup"), TEXT("../Bin/Resources/Sounds/MapObject/Gear_Pickup.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Hp_Pickup"), TEXT("../Bin/Resources/Sounds/MapObject/Hp_Pickup.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Battery_Pickup"), TEXT("../Bin/Resources/Sounds/MapObject/Battery_Pickup.ogg")));
	

	return S_OK;
}

HRESULT CPickUpItem::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;


	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();

	GAMEINSTANCE->Add_Unique_Sound_Channel("Pickup");


	return S_OK;
}

void CPickUpItem::PriorityTick(_float fTimeDelta)
{
}

void CPickUpItem::Tick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_O))
	{
		Execute();
	}

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();

	m_pColliderCom->Update(Transpose);
	m_pBiggerColliderCom->Update(Transpose);

	if (m_bExecuted)
	{	
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMLoadFloat3(&m_vSpeed));
		m_vSpeed.x -= m_vSpeed.x * fTimeDelta * 0.5f;
		m_vSpeed.z -= m_vSpeed.z * fTimeDelta * 0.5f;
		m_vSpeed.y -= fTimeDelta * 5.f;
		if (m_fLandingPos >= XMVectorGetY(m_pTransformCom->Get_Position()))
		{
			m_bExecuted = false;
			m_pTransformCom->Set_Position(XMVectorSetY(m_pTransformCom->Get_Position(), m_fLandingPos));
		}
	}
	else
	{

	}
}

void CPickUpItem::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pBiggerColliderCom);
}

HRESULT CPickUpItem::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	_float4 vOutlineColor = { 0.f,0.f,0.f,1.f };
	m_pShaderCom->Bind_Vector4("g_vOutlineColor", &vOutlineColor);
	m_pShaderCom->Bind_Float("g_OutlineWidth",0.5f);
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}
		if (bNoEmit)
		{

			m_pShaderCom->Begin(0);

		}
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
		
		m_pShaderCom->Begin((_uint)MESH_PASS::Outline);
		m_pModelCom->Draw((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
	if (m_pBiggerColliderCom)
		m_pBiggerColliderCom->Render();
	
#endif

	return S_OK;
}

void CPickUpItem::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CPickUpItem::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == CHAI_COLLIDER_HIT)
	{
		if (pCaller == m_pBiggerColliderCom)
		{
			_vector vDest = pOther->Get_Owner()->Get_Transform()->Get_Position();
			_vector vPos = m_pTransformCom->Get_Position();
			_vector vDir = XMVector3Normalize(vDest - vPos);
			m_pTransformCom->Set_Position(vPos + vDir * 7.f * GAMEINSTANCE->Get_TimeDelta(L"Timer_60"));
		}
		if (pCaller == m_pColliderCom)
		{
			auto pGI = m_pGameInstance.lock();
			string strTag;
			if (m_ePickupType == GEAR)
			{
				strTag = "Gear";
				pGI->Play_Unique_Sound_Channel("Pickup", SOUND_CHANNEL::SOUND_EFFECT, L"Gear_Pickup");
			}
			else if (m_ePickupType == HP)
			{
				strTag = "Hp";
				pGI->Play_Unique_Sound_Channel("Pickup", SOUND_CHANNEL::SOUND_EFFECT, L"Hp_Pickup");
			}
			else
			{
				strTag = "Battery";
				pGI->Play_Unique_Sound_Channel("Pickup", SOUND_CHANNEL::SOUND_EFFECT, L"Battery_Pickup");
			}
			static_pointer_cast<CChai>(pOther->Get_Owner())->Catch_Item(strTag, m_iSize);
			Dead();
		}
	}
}

void CPickUpItem::Edit_Attribute()
{
	ImGui::InputInt("PickupType-0:Gear,1:Med,3:Battery", &m_ePickupType);
	ImGui::InputInt("0:Small,1:Normal,3:Big", &m_iSize);

}

void CPickUpItem::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
	write_typed_data(os, m_ePickupType);
	write_typed_data(os, m_iSize);


}

void CPickUpItem::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
	read_typed_data(is, m_ePickupType);
	read_typed_data(is, m_iSize);

	if (m_ePickupType == GEAR)
	{
		_wstring strTag = _wstring(L"Prototype_Component_Model_PickUpItem_Gear_") + to_wstring(m_iSize);
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, strTag, TEXT("Model_Com")));
	}
	else if (m_ePickupType == HP)
	{
		_wstring strTag = _wstring(L"Prototype_Component_Model_PickUpItem_Med_") + to_wstring(m_iSize);
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, strTag, TEXT("Model_Com")));
	}
	else
	{
		_wstring strTag = _wstring(L"Prototype_Component_Model_PickUpItem_Battery_") + to_wstring(m_iSize);
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, strTag, TEXT("Model_Com")));
	}
}

void CPickUpItem::TakeDamage(const DamageInfo& damageInfo)
{
	Execute();
}

HRESULT CPickUpItem::Ready_Components(COMMONOBJ_DESC* pDesc)
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));

	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PickUpItem_Gear_0"), TEXT("Model_Com")));

	CBounding_Sphere::BOUNDING_SPHERE_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.fRadius = 0.3f;
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Com"), &obbDesc));
	m_pColliderCom->Set_ColliderTag(L"Pickup_Gain");
	//m_pBiggerColliderCom

	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.fRadius = 12.f;
	m_pBiggerColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Follow_Com"), &obbDesc));
	m_pBiggerColliderCom->Set_ColliderTag(L"Pickup_Follow");

	if (!m_pShaderCom || !m_pModelCom /* || !m_pColliderCom */)
		assert(false);

	return S_OK;
}

HRESULT CPickUpItem::Bind_ShaderResources()
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

shared_ptr<CPickUpItem> CPickUpItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CPickUpItem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPickUpItem(pDevice, pContext)
		{}
	};

	shared_ptr<CPickUpItem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPickUpItem"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CPickUpItem::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CPickUpItem
	{
		MakeSharedEnabler(const CPickUpItem& rhs) : CPickUpItem(rhs)
		{}
	};

	shared_ptr<CPickUpItem> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPickUpItem"));
		assert(false);
	}

	return pInstance;
}

void CPickUpItem::Free()
{
}
