#include "..\Public\Octree.h"
#include "GameInstance.h"
#include "OctComp.h"

void UpdateNode(shared_ptr<COctree::Node> pNode)
{
	_bool bInFrustum = CGameInstance::Get_Instance()->isIn_Frustum_WorldSpace(XMVectorSetW(XMLoadFloat3(&pNode->Center), 1.f), pNode->fOuterRadius);
	_bool bInFull = CGameInstance::Get_Instance()->isIn_Frustum_WorldSpaceEX(XMVectorSetW(XMLoadFloat3(&pNode->Center), 1.f), pNode->fOuterRadius);
	pNode->Update(bInFrustum, bInFull);
}


COctree::COctree()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{

}

COctree::~COctree()
{
	Free();
}

HRESULT COctree::Initialize(_float fUnitScale, _int iMaxRecursion, const _float3& vCenter)
{
	if(!m_pOctree)
		m_pOctree = make_shared<Node>();
	m_pOctree->Clear();
	m_pOctree->Initialize(fUnitScale, iMaxRecursion, vCenter);
	



	return S_OK;
}

void COctree::Set_Up()
{
	if (!m_bCull)
		return;
	if(m_pOctree)
	m_pOctree->Set_Up();
}

void COctree::Update()
{

	if (!m_pOctree || !m_bCull)
		return;
	vector<thread> t;
	for (_uint i = 0; i < 8; ++i)
	{
		t.push_back(thread(UpdateNode, m_pOctree->Children[i]));
	}
	for (_uint i = 0; i < 8; ++i)
	{
		t[i].join();
	}
}

void COctree::Clear()
{
	if (m_pOctree)
		m_pOctree->Clear();
}

void COctree::Start()
{
	m_bCull = true;
}

void COctree::Stop()
{
	m_bCull = false;
}


shared_ptr<COctree> COctree::Create()
{
	MAKE_SHARED_ENABLER(COctree)
	{}
	shared_ptr<COctree> pInstance = make_shared<MakeSharedEnabler>();

	//if (FAILED(pInstance->Initialize(fUnitScale, iMaxRecursion,vCenter)))
	//{
	//	MSG_BOX(TEXT("Failed to Create : COctree"));
	//	assert(false);
	//}

	return pInstance;
}

void COctree::Free()
{

}

void COctree::Node::Update(_bool _bInFrustum, _bool _bInFull)
{
	if (!_bInFrustum)
	{
		bInFrustum = _bInFrustum;
		if(iTopology)
		{
			for (_uint i = 0; i < 8; ++i)
			{
				if (Children[i])
				Children[i]->Update(false, false);
			}
		}

		return;
	}
	if (_bInFull)
	{
		bInFrustum = true;
		bInFull = true;
		if (iTopology)
		{
			for (_uint i = 0; i < 8; ++i)
			{
				if (Children[i])
				Children[i]->Update(true, true);
			}
		}
		for (auto& Obj : vecObjects)
		{
			Obj->Update();
		}
		return;
	}

	bInFrustum = CGameInstance::Get_Instance()->isIn_Frustum_WorldSpace(XMVectorSetW(XMLoadFloat3(&Center), 1.f), fOuterRadius);
	if (bInFrustum)
	{
		for (auto& Obj : vecObjects)
		{
			Obj->Update();
		}
	}
	bInFull = CGameInstance::Get_Instance()->isIn_Frustum_WorldSpaceEX(XMVectorSetW(XMLoadFloat3(&Center), 1.f), fOuterRadius);
	if (iTopology > 0)
	{
		for (_uint i = 0; i < 8; ++i)
		{
			if (Children[i])
			Children[i]->Update(bInFrustum, bInFull);
		}
	}
	return;
}

void COctree::Node::Set_Up()
{

	for (auto& Obj : vecObjects)
	{
		Obj->Set_Up();
	}
	if (iTopology > 0)
	{
		for (_uint i = 0; i < 8; ++i)
		{
			if (Children[i])
				Children[i]->Set_Up();
		}
	}
}

_bool COctree::Node::AddOctComp(shared_ptr<COctComp> pComp)
{
	_bool bIsIn = IsInOctree(pComp->m_vCenter, pComp->m_fRadius);
	_bool bIsLast = true;
	if (bIsIn)
	{
		if (iTopology > 0)
		{
			for (_uint i = 0; i < 8; ++i)
			{
				if (Children[i])
				{
					if (Children[i]->AddOctComp(pComp))
					{
						bIsLast = false;
					}
				}
			}
		}
	}
	if(bIsLast && bIsIn)
		vecObjects.push_back(pComp);
	return bIsIn;
}
