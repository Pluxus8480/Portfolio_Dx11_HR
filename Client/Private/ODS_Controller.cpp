#include "ODS_Controller.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"
#include "Beat_Manager.h"

wstring CODS_Controller::ObjID = TEXT("CODS_Controller");

CODS_Controller::CODS_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CODS_Controller::CODS_Controller(const CODS_Controller& rhs)
	: CGameObject(rhs)
{
}

CODS_Controller::~CODS_Controller()
{
	Free();
}

HRESULT CODS_Controller::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	Set_PoolSize(12);
	return S_OK;
}

HRESULT CODS_Controller::Initialize(void* pArg)
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

void CODS_Controller::PriorityTick(_float fTimeDelta)
{
	_int iCurrBeat = (_int)BEATMANAGER->Get_BeatCount();
	if (iCurrBeat != m_iPrevBeat)
	{
		m_iPrevBeat = iCurrBeat;
		--m_iShootTimer;
		if (m_iShootTimer < -20)
		{
			Dead();
		}
	}
}

void CODS_Controller::Tick(_float fTimeDelta)
{
	if (m_iShootTimer < 0 || (m_iShootTimer == 0 && m_fShootTimerLeft <= BEATMANAGER->Get_Timer()))
	{
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + m_pTransformCom->Get_NormLook() * fTimeDelta * 50.f);
	}
}

void CODS_Controller::LateTick(_float fTimeDelta)
{

}

HRESULT CODS_Controller::Render()
{
	return S_OK;
}

void CODS_Controller::Push_From_Pool(void* pArg)
{

	auto pBM = BEATMANAGER;
	EFFECTMANAGER->Call_Union_Effect("O_D_Slash", m_pTransformCom->Get_WorldMatrix_Ptr(), 2.f / pBM->Get_Beat(), false);
	m_iShootTimer = *(_int*)(pArg);
	m_fShootTimerLeft = pBM->Get_Timer();
	m_iPrevBeat = pBM->Get_BeatCount();
}

void CODS_Controller::Dying()
{
}

HRESULT CODS_Controller::Ready_Components()
{
	return S_OK;
}

shared_ptr<CODS_Controller> CODS_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CODS_Controller
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CODS_Controller(pDevice, pContext)
		{}
	};


	shared_ptr<CODS_Controller> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CODS_Controller"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CODS_Controller::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CODS_Controller
	{
		MakeSharedEnabler(const CODS_Controller& rhs) : CODS_Controller(rhs)
		{}
	};


	shared_ptr<CODS_Controller> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CODS_Controller"));
		assert(false);
	}

	return pInstance;
}

void CODS_Controller::Free()
{
}
