#include "OctComp.h"
#include "GameInstance.h"

COctComp::COctComp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):
	CComponent(pDevice, pContext)
{
}

COctComp::COctComp(const COctComp& rhs):
	CComponent(rhs)
{
}

COctComp::~COctComp()
{
	Free();
}

HRESULT COctComp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COctComp::Initialize(void* pArg)
{
	return S_OK;
}

void COctComp::Calc_Octree(const _float3& vCenter, _float fRadius)
{
	m_vCenter = vCenter;
	m_fRadius = fRadius;
	auto pGameInstance = m_pGameInstance.lock();
	auto pMotherNode = pGameInstance->Get_Node();
	if (!pMotherNode->IsInOctree(vCenter, fRadius))
	{
		m_bValidity = false;
		return;
	}
	m_bValidity = true;
	pMotherNode->AddOctComp(shared_from_this());
}

void COctComp::Set_Up()
{
	m_bIsIn = false;
}

void COctComp::Update()
{
	m_bIsIn = true;
}

_bool COctComp::IsCulled()
{
	if (!m_bValidity)
		return false;

	return !m_bIsIn;
}

shared_ptr<COctComp> COctComp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public COctComp
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : COctComp(pDevice, pContext) { }
	};
	shared_ptr< COctComp> pInstance = make_shared <MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : COctComp"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> COctComp::Clone(void* pArg)
{
	struct MakeSharedEnabler : public COctComp
	{
		MakeSharedEnabler(const COctComp& rhs) : COctComp(rhs) { }
	};


	shared_ptr<COctComp> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : COctComp"));
		assert(false);
	}

	return pInstance;
}
void COctComp::Free()
{
}
