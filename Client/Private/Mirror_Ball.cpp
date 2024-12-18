#include "Mirror_Ball.h"
#include "GameInstance.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "Random_Laser.h"
#include "Beat_Manager.h"

wstring CMirror_Ball::ObjID = TEXT("CMirror_Ball");

CMirror_Ball::CMirror_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMirror_Ball::CMirror_Ball(const CMirror_Ball& rhs)
	: CGameObject(rhs)
{
}

CMirror_Ball::~CMirror_Ball()
{
	Free();
}

HRESULT CMirror_Ball::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Set_PoolSize(10);

	return S_OK;
}

HRESULT CMirror_Ball::Initialize(void* pArg)
{
	Init_Variables();

	MIRROR_BALL pDesc = *static_cast<MIRROR_BALL*>(pArg);
	m_fMirrorBallTimeMax = pDesc.fLaserActiveTime;

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Position(pDesc.vOriginPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CRandom_Laser::RANDOM_LASER tLaser;
	tLaser.vOriginPos = pDesc.vOriginPos;

	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));
	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));
	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));
	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));
	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));
	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));
	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));
	m_pLaserEffects.push_back(static_pointer_cast<CRandom_Laser>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CRandom_Laser::ObjID, &tLaser)));

	for (auto pEffect : m_pLaserEffects)
	{
		pEffect->Set_ParentPos({ XMVectorGetX(m_pTransformCom->Get_Position()), XMVectorGetZ(m_pTransformCom->Get_Position()) });
	}

	GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Loop", true);

	return S_OK;
}

void CMirror_Ball::PriorityTick(_float fTimeDelta)
{

}

void CMirror_Ball::Tick(_float fTimeDelta)
{

	Time_Calc(fTimeDelta);
	Dead_Check();
	Laser_Pause(fTimeDelta);
}

void CMirror_Ball::LateTick(_float fTimeDelta)
{

}

HRESULT CMirror_Ball::Render()
{
	return S_OK;
}

void CMirror_Ball::Init_Variables()
{

	m_fMirrorBallTimeMax = 5.f;
	m_fMirrorBallTime = 0.f;
	m_fMirrorBallTimePercent = 0.f;
	m_bActive = true;

	m_fPauseCoolTime = 0.f;
	m_fPauseCoolTimeMax = BEATMANAGER->Get_Beat();

	m_bMovePause = false;

}

void CMirror_Ball::Time_Calc(_float fTimeDelta)
{

	m_fMirrorBallTime += fTimeDelta;
	m_fMirrorBallTimePercent = m_fMirrorBallTime / m_fMirrorBallTimeMax;

}

void CMirror_Ball::Pos_Calc(_float fTimeDelta)
{
	//m_pTransformCom->LookAt(XMLoadFloat4(&m_vCurrentPos));
}

void CMirror_Ball::Dead_Check()
{
	if (m_fMirrorBallTime > m_fMirrorBallTimeMax)
	{
		GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Loop");
		m_bActive = false;
		Dead();

		for (auto pEffect : m_pLaserEffects)
		{
			pEffect = nullptr;
		}

	}
}

void CMirror_Ball::Laser_Pause(_float fTimeDelta)
{
	if(!m_bActive)
		return;

	m_fPauseCoolTime += fTimeDelta;
	if (m_fPauseCoolTime >= m_fPauseCoolTimeMax) {
		m_bMovePause = !m_bMovePause;
		m_bPauseControl = true;
		m_fPauseCoolTime -= m_fPauseCoolTimeMax;
	}

	if (!m_bPauseControl)
		return;

	if (m_bMovePause)
	{
		for (auto pEffect : m_pLaserEffects)
		{
			pEffect->Set_Pause(true);
		}
		m_bPauseControl = false;
	}
	else
	{
		for (auto pEffect : m_pLaserEffects)
		{
			pEffect->Set_Pause(false);
		}
		m_bPauseControl = false;
	}

}

HRESULT CMirror_Ball::Ready_Components()
{
	return S_OK;
}

shared_ptr<CMirror_Ball> CMirror_Ball::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMirror_Ball
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMirror_Ball(pDevice, pContext)
		{}
	};

	shared_ptr<CMirror_Ball> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMirror_Ball"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMirror_Ball::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMirror_Ball
	{
		MakeSharedEnabler(const CMirror_Ball& rhs) : CMirror_Ball(rhs)
		{}
	};


	shared_ptr<CMirror_Ball> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMirror_Ball"));
		assert(false);
	}

	return pInstance;
}

void CMirror_Ball::Free()
{
}
