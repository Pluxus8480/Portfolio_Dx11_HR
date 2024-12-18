#include "Random_Laser.h"
#include "GameInstance.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "PlayerManager.h"

wstring CRandom_Laser::ObjID = TEXT("CRandom_Laser");

CRandom_Laser::CRandom_Laser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CRandom_Laser::CRandom_Laser(const CRandom_Laser& rhs)
	: CGameObject(rhs)
{
}

CRandom_Laser::~CRandom_Laser()
{
	Free();
}

HRESULT CRandom_Laser::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Set_PoolSize(10);

	return S_OK;
}

HRESULT CRandom_Laser::Initialize(void* pArg)
{
	Init_Variables();

	RANDOM_LASER pDesc = *static_cast<RANDOM_LASER*>(pArg);

	m_vCurrentPos = m_vStartPos;

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Position(pDesc.vOriginPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pEffect = EFFECTMANAGER->Call_Union_Effect_Return("Tracking_Laser", m_pTransformCom->Get_WorldMatrix_Ptr(), 0.05f);

	return S_OK;
}

void CRandom_Laser::PriorityTick(_float fTimeDelta)
{

}

void CRandom_Laser::Tick(_float fTimeDelta)
{

	Time_Calc(fTimeDelta);
	Dead_Check();
	Pos_Calc(fTimeDelta);
	Laser_Explode(fTimeDelta);
}

void CRandom_Laser::LateTick(_float fTimeDelta)
{

}

HRESULT CRandom_Laser::Render()
{
	return S_OK;
}

void CRandom_Laser::Init_Variables()
{
	m_fLissajousLagrangianX = (float)GAMEINSTANCE->Random_Int(1, 5);
	m_fLissajousLagrangianY = (float)GAMEINSTANCE->Random_Int(1, 5);
	m_fLissajousPhaseDelta = (float)GAMEINSTANCE->Random_Int(1, 4);
	m_fLissajousSpeed = GAMEINSTANCE->Random_Float(0.5, 1);

	m_fLaserTimeMax = 5.f;
	m_fLaserTime = 0.f;
	m_fLaserTimePercent = 0.f;
	m_bActive = true;

	m_fExplodeCoolTime = 0.f;
	m_fExplodeCoolTimeMax = 0.05f;

	m_bParentPos = { 0.f,0.f };

	m_bExplode = false;

}

void CRandom_Laser::Time_Calc(_float fTimeDelta)
{

	m_fLaserTime += fTimeDelta;
	m_fLaserTimePercent = m_fLaserTime / m_fLaserTimeMax;

}

void CRandom_Laser::Pos_Calc(_float fTimeDelta)
{
	if(m_bPause)
		return;

	_float2 fLissajous = GAMEINSTANCE->Lissajous_Curve(fTimeDelta, m_fLissajousTime, 10, 10, m_fLissajousLagrangianX, m_fLissajousLagrangianY, m_fLissajousPhaseDelta, m_fLissajousSpeed);
	m_vCurrentPos = { fLissajous.x + m_bParentPos.x, PLAYERMANAGER->Get_FootPosY(), fLissajous.y + m_bParentPos.y, 1.f };
	m_pTransformCom->LookAt(XMLoadFloat4(&m_vCurrentPos));
}

void CRandom_Laser::Dead_Check()
{
	if (m_fLaserTime > m_fLaserTimeMax)
	{
		m_bActive = false;
		Dead();
		m_pEffect->UnionEffectStop();
		m_pEffect = nullptr;
	}
}

void CRandom_Laser::Laser_Explode(_float fTimeDelta)
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
		XMStoreFloat4x4(&vPos, DirectX::XMMatrixTranslationFromVector(XMLoadFloat4(&m_vCurrentPos)));
		EFFECTMANAGER->Call_Fixed_Union_Effect("Laser_Explode", &vPos, 1.f, true);
	}

}

HRESULT CRandom_Laser::Ready_Components()
{
	return S_OK;
}

shared_ptr<CRandom_Laser> CRandom_Laser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRandom_Laser
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRandom_Laser(pDevice, pContext)
		{}
	};

	shared_ptr<CRandom_Laser> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRandom_Laser"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CRandom_Laser::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRandom_Laser
	{
		MakeSharedEnabler(const CRandom_Laser& rhs) : CRandom_Laser(rhs)
		{}
	};

	shared_ptr<CRandom_Laser> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRandom_Laser"));
		assert(false);
	}

	return pInstance;
}

void CRandom_Laser::Free()
{
}
