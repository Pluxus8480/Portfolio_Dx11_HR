#include "Tool_Cine_Camera.h"
#include "GameInstance.h"


wstring CTool_Cine_Camera::ObjID = L"Tool_Cine_Camera";

CTool_Cine_Camera::CTool_Cine_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CTool_Cine_Camera::CTool_Cine_Camera(const CTool_Cine_Camera& rhs)
	: CCamera(rhs)
{
}

CTool_Cine_Camera::~CTool_Cine_Camera()
{
	Free();
}

HRESULT CTool_Cine_Camera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTool_Cine_Camera::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTool_Cine_Camera::PriorityTick(_float fTimeDelta)
{
	// Speed °»½Å
	m_pTransformCom->Set_Speed(m_fMoveSpeed);

	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);
	else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);
	else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);
	else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);

	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	if (m_pGameInstance.lock()->Get_KeyPressing(DIK_Q))
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + vUp * fTimeDelta * m_pTransformCom->Get_Speed());
	else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_E))
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() - vUp * fTimeDelta * m_pTransformCom->Get_Speed());

	_long	MouseMove = {};
	if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	{
		if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_X))
		{
			_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}
		if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}

	if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	{
		POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
		ClientToScreen(g_hWnd, &ptWindow);
		SetCursorPos(ptWindow.x, ptWindow.y);
	}

	__super::PriorityTick(fTimeDelta);
}

void CTool_Cine_Camera::Tick(_float fTimeDelta)
{
}

void CTool_Cine_Camera::LateTick(_float fTimeDelta)
{
}

HRESULT CTool_Cine_Camera::Render()
{
	return S_OK;
}

void CTool_Cine_Camera::SetCamInfo(CCine_Sequence::CineCamInfo& camInfo)
{
	Set_Fov(camInfo.fFov);
	m_pTransformCom->Set_WorldMatrix(camInfo.matTrans);
}
void CTool_Cine_Camera::GetCamInfo(CCine_Sequence::CineCamInfo& camInfo)
{
	camInfo.fFov = Get_Fov();
	camInfo.matTrans = m_pTransformCom->Get_WorldMatrix();
}

_float CTool_Cine_Camera::GetSpeed() const
{
	return m_pTransformCom->Get_Speed();
}

shared_ptr<CTool_Cine_Camera> CTool_Cine_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CTool_Cine_Camera
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTool_Cine_Camera(pDevice, pContext) { }
	};

	shared_ptr<CTool_Cine_Camera> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTool_Cine_Camera"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CTool_Cine_Camera::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CTool_Cine_Camera
	{
		MakeSharedEnabler(const CTool_Cine_Camera& rhs) : CTool_Cine_Camera(rhs) { }
	};

	shared_ptr<CTool_Cine_Camera> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTool_Cine_Camera"));
		assert(false);
	}
	return pInstance;
}

void CTool_Cine_Camera::Free()
{
}
