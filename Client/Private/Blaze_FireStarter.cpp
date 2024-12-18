#include "Blaze_FireStarter.h"
#include "GameInstance.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "Random_Laser.h"
#include "Beat_Manager.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CBlaze_FireStarter::ObjID = TEXT("CBlaze_FireStarter");

CBlaze_FireStarter::CBlaze_FireStarter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBlaze_FireStarter::CBlaze_FireStarter(const CBlaze_FireStarter& rhs)
	: CGameObject(rhs)
{
}

CBlaze_FireStarter::~CBlaze_FireStarter()
{
	Free();
}

HRESULT CBlaze_FireStarter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Set_PoolSize(10);

	return S_OK;
}

HRESULT CBlaze_FireStarter::Initialize(void* pArg)
{

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	Blaze_FireStarter* pDesc = static_cast<Blaze_FireStarter*>(pArg);

	m_pTransformCom->Set_WorldMatrix(pDesc->matWorld);
	m_eFireStarterType = pDesc->eFireStarterType;

	m_iFireStarter_Count = 0;
	m_iFireStarter_Count_Max = 10;

	m_fBlazeFireTime = 0.1f;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBlaze_FireStarter::PriorityTick(_float fTimeDelta)
{

}

void CBlaze_FireStarter::Tick(_float fTimeDelta)
{
	State_Machine(fTimeDelta);
}

void CBlaze_FireStarter::LateTick(_float fTimeDelta)
{

}

HRESULT CBlaze_FireStarter::Render()
{
	return S_OK;
}

HRESULT CBlaze_FireStarter::Ready_Components()
{

	return S_OK;
}

void CBlaze_FireStarter::State_Machine(_float fTimeDelta)
{

	switch (m_eFireStarterType)
	{
	case FIRESTARTER_LIST::FIRESTARTER_STRAIGHT:
		State_Straight(fTimeDelta);
		break;
	case FIRESTARTER_LIST::FIRESTARTER_DEATH:
		State_Death(fTimeDelta);
		break;
	default:
		break;
	}

}

void CBlaze_FireStarter::State_Straight(_float fTimeDelta)
{

	m_fBlazeFireTime += fTimeDelta;
	if (m_fBlazeFireTime >= m_fBlazeFireTimeMax) {

		m_fBlazeFireTime -= m_fBlazeFireTimeMax;

		_vector vPos = m_pTransformCom->Get_Position();

		_vector vRight = m_pTransformCom->Get_NormRight();

		_vector vLook = m_pTransformCom->Get_NormLook();

		_float vDirection = m_iFireStarter_Count % 2 == 0 ? 1.f : -1.f;

		vPos = vPos + (vLook * m_iFireStarter_Count) * 2.f + (vRight * vDirection) * 0.5f;

		CTransform::TRANSFORM_DESC			TransformDesc{};
		TransformDesc.fSpeedPerSec = 1.f;
		TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		XMStoreFloat4(&TransformDesc.vPosition, vPos);

		GAMEINSTANCE->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("CBlaze_Fire"), &TransformDesc);
	
		++m_iFireStarter_Count;
	
		if (m_iFireStarter_Count >= m_iFireStarter_Count_Max) {
			m_eFireStarterType = FIRESTARTER_LIST::FIRESTARTER_DEATH;
		}
	}


}

void CBlaze_FireStarter::State_Death(_float fTimeDelta)
{
	Dead();
}

shared_ptr<CBlaze_FireStarter> CBlaze_FireStarter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBlaze_FireStarter
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBlaze_FireStarter(pDevice, pContext)
		{}
	};

	shared_ptr<CBlaze_FireStarter> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBlaze_FireStarter"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CBlaze_FireStarter::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBlaze_FireStarter
	{
		MakeSharedEnabler(const CBlaze_FireStarter& rhs) : CBlaze_FireStarter(rhs)
		{}
	};


	shared_ptr<CBlaze_FireStarter> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBlaze_FireStarter"));
		assert(false);
	}

	return pInstance;
}

void CBlaze_FireStarter::Free()
{
}
