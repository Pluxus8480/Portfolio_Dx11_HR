#include "Kale_BigSword.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"


wstring CKale_BigSword::ObjID = TEXT("CKale_BigSword");

CKale_BigSword::CKale_BigSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CKale_BigSword::CKale_BigSword(const CKale_BigSword& rhs)
	: CGameObject(rhs)
{
}

CKale_BigSword::~CKale_BigSword()
{
	Free();
}

HRESULT CKale_BigSword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	Set_PoolSize(12);
	return S_OK;
}

HRESULT CKale_BigSword::Initialize(void* pArg)
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

void CKale_BigSword::PriorityTick(_float fTimeDelta)
{
	_int iCurrBeat = (_int)BEATMANAGER->Get_BeatCount();
	if (iCurrBeat != m_iPrevBeat)
	{
		m_iPrevBeat = iCurrBeat;
	}
}

void CKale_BigSword::Tick(_float fTimeDelta)
{
	auto pBM = BEATMANAGER;
	m_fShootTimerLeft -= fTimeDelta;
	if (m_fShootTimerLeft >= pBM->Get_Beat() * 0.5f)
	{
		m_pTransformCom->Set_Position(PLAYERMANAGER->Get_PlayerPos());
	}
	if (!m_bShoot && m_fShootTimerLeft <= 0.f)
	{
		m_bShoot = true;
		if (m_bExtended) {
			EFFECTMANAGER->Call_Union_Effect("Kale_Judgement_Extend", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
			EFFECTMANAGER->Call_Union_Effect("Kale_Judgement_Fire", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
		}
		else {
			EFFECTMANAGER->Call_Union_Effect("Kale_Judgement", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
		}
	}
	if (m_fShootTimerLeft <= -0.6f)
	{
		Dead();
	}

}

void CKale_BigSword::LateTick(_float fTimeDelta)
{

}

HRESULT CKale_BigSword::Render()
{
	return S_OK;
}

void CKale_BigSword::Push_From_Pool(void* pArg)
{
	auto pBM = BEATMANAGER;
	EFFECTMANAGER->Call_Union_Effect("Kale_Judgement_Warning", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f / pBM->Get_Beat() / 3.f, false);
	m_bShoot = false;
	m_fShootTimerLeft = pBM->Get_Beat() * 3.f;
	m_iPrevBeat = pBM->Get_BeatCount();
}

void CKale_BigSword::Dying()
{
}

HRESULT CKale_BigSword::Ready_Components()
{
	return S_OK;
}

shared_ptr<CKale_BigSword> CKale_BigSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CKale_BigSword
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKale_BigSword(pDevice, pContext)
		{}
	};


	shared_ptr<CKale_BigSword> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKale_BigSword"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CKale_BigSword::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CKale_BigSword
	{
		MakeSharedEnabler(const CKale_BigSword& rhs) : CKale_BigSword(rhs)
		{}
	};


	shared_ptr<CKale_BigSword> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKale_BigSword"));
		assert(false);
	}

	return pInstance;
}

void CKale_BigSword::Free()
{
}
