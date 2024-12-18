#include "SectionCullingBox.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "GameManager.h"

wstring CSectionCullingBox::ObjID = TEXT("CSectionCullingBox");
CSectionCullingBox::CSectionCullingBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CSectionCullingBox::CSectionCullingBox(const CSectionCullingBox& rhs)
	: CGameObject{rhs}
{
}

CSectionCullingBox::~CSectionCullingBox()
{
	Free();
}

HRESULT CSectionCullingBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSectionCullingBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SECTIONCULLING_DESC* pDesc = static_cast<SECTIONCULLING_DESC*>(pArg);

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
	aabbDesc.vExtents = { 1.f, 1.f, 1.f };
	aabbDesc.vCenter = { 0.f, 0.f, 0.f };
	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

	assert(m_pColliderCom);

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMat);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

	return S_OK;
}

void CSectionCullingBox::PriorityTick(_float fTimeDelta)
{
}

void CSectionCullingBox::Tick(_float fTimeDelta)
{
	if(CGame_Manager::Get_Instance()->Get_Current_Level() == LEVEL_MAPTOOL)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CSectionCullingBox::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CSectionCullingBox::Render()
{
#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSectionCullingBox::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = true;
}

void CSectionCullingBox::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = true;
}

void CSectionCullingBox::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
		m_bCollision = false;
}

void CSectionCullingBox::Edit_Attribute()
{
	if (ImGui::BeginListBox("##Sections", ImVec2(0.f, 100.f)))
	{
		for (_uint i = 0; i < m_vecRenderIndex.size(); i++)
		{
			_bool bIsSelected = (m_iCurIndex == i);
			string strListElem = "RenderIndex_" + to_string(m_vecRenderIndex[i]);
			if (ImGui::Selectable(strListElem.c_str(), bIsSelected))
				m_iCurIndex = i;

			if (bIsSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	ImGui::InputInt("Render Index", &m_iRenderIndex);

	if (ImGui::Button("Add"))
	{
		if (find(m_vecRenderIndex.begin(), m_vecRenderIndex.end(), m_iRenderIndex) == m_vecRenderIndex.end())
			m_vecRenderIndex.push_back(m_iRenderIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove") && m_vecRenderIndex.size() > 0)
	{
		m_vecRenderIndex.erase(m_vecRenderIndex.begin() + m_iCurIndex);
		m_iCurIndex = max(m_iCurIndex - 1, 0);
	}
}

void CSectionCullingBox::Save_Data(ofstream& os)
{
	size_t vectorSize = m_vecRenderIndex.size();
	write_typed_data(os, vectorSize);

	for (auto& Index : m_vecRenderIndex)
		write_typed_data(os, Index);
}

void CSectionCullingBox::Load_Data(ifstream& is)
{
	size_t size = {};
	read_typed_data(is, size);

	for (size_t i = 0; i < size; ++i)
	{
		_uint iIndex = { };
		read_typed_data(is, iIndex);
		m_vecRenderIndex.push_back(iIndex);
	}
}

shared_ptr<CSectionCullingBox> CSectionCullingBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSectionCullingBox
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSectionCullingBox(pDevice, pContext)
		{}
	};

	shared_ptr<CSectionCullingBox> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSectionCullingBox"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CSectionCullingBox::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSectionCullingBox
	{
		MakeSharedEnabler(const CSectionCullingBox& rhs) : CSectionCullingBox(rhs)
		{}
	};


	shared_ptr<CSectionCullingBox> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSectionCullingBox"));
		assert(false);
	}

	return pInstance;
}

void CSectionCullingBox::Free()
{
}
