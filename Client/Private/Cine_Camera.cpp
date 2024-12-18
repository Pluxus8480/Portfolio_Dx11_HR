#include "Cine_Camera.h"
#include "GameInstance.h"


wstring CCine_Camera::ObjID = L"Cine_Camera";

CCine_Camera::CCine_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCine_Camera::CCine_Camera(const CCine_Camera& rhs)
	: CCamera(rhs)
{
}

CCine_Camera::~CCine_Camera()
{
	Free();
}

HRESULT CCine_Camera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCine_Camera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCine_Camera::PriorityTick(_float fTimeDelta)
{
	__super::PriorityTick(fTimeDelta);
}

void CCine_Camera::Tick(_float fTimeDelta)
{
}

void CCine_Camera::LateTick(_float fTimeDelta)
{
}

HRESULT CCine_Camera::Render()
{
	return S_OK;
}

void CCine_Camera::SetCamInfo(CCine_Sequence::CineCamInfo& camInfo)
{
	Set_Fov(camInfo.fFov);
	m_pTransformCom->Set_WorldMatrix(camInfo.matTrans);
}
void CCine_Camera::GetCamInfo(CCine_Sequence::CineCamInfo& camInfo)
{
	camInfo.fFov = Get_Fov();
	camInfo.matTrans = m_pTransformCom->Get_WorldMatrix();
}

shared_ptr<CCine_Camera> CCine_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CCine_Camera
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCine_Camera(pDevice, pContext) { }
	};

	shared_ptr<CCine_Camera> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCine_Camera"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CCine_Camera::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CCine_Camera
	{
		MakeSharedEnabler(const CCine_Camera& rhs) : CCine_Camera(rhs) { }
	};

	shared_ptr<CCine_Camera> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCine_Camera"));
		assert(false);
	}
	return pInstance;
}

void CCine_Camera::Free()
{
}
