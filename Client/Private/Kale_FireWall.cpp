#include "Kale_FireWall.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "Beat_Manager.h"

wstring CKale_FireWall::ObjID = TEXT("CKale_FireWall");

CKale_FireWall::CKale_FireWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CKale_FireWall::CKale_FireWall(const CKale_FireWall& rhs)
	: CGameObject(rhs)
{
}

CKale_FireWall::~CKale_FireWall()
{
	Free();
}

HRESULT CKale_FireWall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	Set_PoolSize(12);
	return S_OK;
}

HRESULT CKale_FireWall::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CKale_FireWall::PriorityTick(_float fTimeDelta)
{
	_int iCurrBeat = (_int)BEATMANAGER->Get_BeatCount();
	if (iCurrBeat != m_iPrevBeat)
	{
		m_iPrevBeat = iCurrBeat;
		--m_iShootTimer;
		if (m_iShootTimer < -10)
		{
			Dead();
		}
	}
}

void CKale_FireWall::Tick(_float fTimeDelta)
{
	if (m_iShootTimer < 0 ||(m_iShootTimer == 0 && m_fShootTimerLeft <= BEATMANAGER->Get_Timer()))
	{
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + m_pTransformCom->Get_NormLook() * fTimeDelta * 12.f);
	}
}

void CKale_FireWall::LateTick(_float fTimeDelta)
{

}

HRESULT CKale_FireWall::Render()
{
	return S_OK;
}

void CKale_FireWall::Push_From_Pool(void* pArg)
{
	EFFECTMANAGER->Call_Union_Effect("FireWall", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
	auto pBM = BEATMANAGER;
	m_iShootTimer = *(_int*)(pArg);
	m_fShootTimerLeft = pBM->Get_Timer();
	m_iPrevBeat = pBM->Get_BeatCount();
}

void CKale_FireWall::Dying()
{
}

HRESULT CKale_FireWall::Ready_Components()
{
	return S_OK;
}

shared_ptr<CKale_FireWall> CKale_FireWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CKale_FireWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKale_FireWall(pDevice, pContext)
		{}
	};


	shared_ptr<CKale_FireWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKale_FireWall"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CKale_FireWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CKale_FireWall
	{
		MakeSharedEnabler(const CKale_FireWall& rhs) : CKale_FireWall(rhs)
		{}
	};


	shared_ptr<CKale_FireWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKale_FireWall"));
		assert(false);
	}

	return pInstance;
}

void CKale_FireWall::Free()
{
}
