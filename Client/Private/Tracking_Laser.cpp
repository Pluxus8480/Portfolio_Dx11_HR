#include "Tracking_Laser.h"
#include "GameInstance.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"

wstring CTracking_Laser::ObjID = TEXT("CTracking_Laser");

CTracking_Laser::CTracking_Laser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTracking_Laser::CTracking_Laser(const CTracking_Laser& rhs)
	: CGameObject(rhs)
{
}

CTracking_Laser::~CTracking_Laser()
{
	Free();
}

HRESULT CTracking_Laser::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Set_PoolSize(10);

	return S_OK;
}

HRESULT CTracking_Laser::Initialize(void* pArg)
{
	Init_Variables();

	TRACKING_LASER pDesc = *static_cast<TRACKING_LASER*>(pArg);
	m_vStartPos = pDesc.vStartPos;
	m_vEndPos = pDesc.vEndPos;
	m_fLaserTimeMax = pDesc.fLaserActiveTime;

	m_vCurrentPos = m_vStartPos;

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Position(pDesc.vOriginPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Tracking_Laser", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);

	return S_OK;
}

void CTracking_Laser::PriorityTick(_float fTimeDelta)
{

}

void CTracking_Laser::Tick(_float fTimeDelta)
{

	Time_Calc(fTimeDelta);
	Dead_Check();
	Pos_Calc(fTimeDelta);
	Laser_Explode(fTimeDelta);
}

void CTracking_Laser::LateTick(_float fTimeDelta)
{

}

HRESULT CTracking_Laser::Render()
{
	return S_OK;
}

void CTracking_Laser::Init_Variables()
{

	m_fLaserTimeMax = 0.5f;
	m_fLaserTime = 0.f;
	m_fLaserTimePercent = 0.f;
	m_bActive = true;
	
	m_fExplodeCoolTime = 0.f;
	m_fExplodeCoolTimeMax = 0.05f;

	m_bExplode = false;

}

void CTracking_Laser::Time_Calc(_float fTimeDelta)
{

	m_fLaserTime += fTimeDelta;
	m_fLaserTimePercent = m_fLaserTime / m_fLaserTimeMax;

}

void CTracking_Laser::Pos_Calc(_float fTimeDelta)
{

	XMStoreFloat4(&m_vCurrentPos, XMVectorLerp(XMLoadFloat4(&m_vStartPos), XMLoadFloat4(&m_vEndPos), m_fLaserTimePercent));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_vCurrentPos));

}

void CTracking_Laser::Dead_Check()
{
	if (m_fLaserTime > m_fLaserTimeMax)
	{
		m_bActive = false;
		Dead();
		m_pEffect->UnionEffectStop();
		m_pEffect = nullptr;
	}
}

void CTracking_Laser::Laser_Explode(_float fTimeDelta)
{
	if (m_bActive) {
		m_fExplodeCoolTime += fTimeDelta;
		if (m_fExplodeCoolTime >= m_fExplodeCoolTimeMax) {
			m_bExplode = true;
			m_fExplodeCoolTime -= m_fExplodeCoolTimeMax;
		}
	}

	if (m_bExplode)
	{
		m_bExplode = false;

		_float4x4 vPos;
		XMStoreFloat4x4(&vPos,DirectX::XMMatrixTranslationFromVector(XMLoadFloat4(&m_vCurrentPos)));
		vPos._42 = 0.f;
		EFFECTMANAGER->Call_Fixed_Union_Effect("Laser_Explode", &vPos, 1.f, true);
	}

}

HRESULT CTracking_Laser::Ready_Components()
{
	return S_OK;
}

shared_ptr<CTracking_Laser> CTracking_Laser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CTracking_Laser
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTracking_Laser(pDevice, pContext)
		{}
	};


	shared_ptr<CTracking_Laser> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTracking_Laser"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CTracking_Laser::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CTracking_Laser
	{
		MakeSharedEnabler(const CTracking_Laser& rhs) : CTracking_Laser(rhs)
		{}
	};


	shared_ptr<CTracking_Laser> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTracking_Laser"));
		assert(false);
	}

	return pInstance;
}

void CTracking_Laser::Free()
{
}
