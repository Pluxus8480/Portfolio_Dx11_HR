#include "PipeLine.h"
#include "Camera.h"
#include "GameInstance.h"
CPipeLine::CPipeLine()
{
	XMStoreFloat4(&m_vShadowDir, XMVector3Normalize(XMVectorSet(1.f, -2.f, 1.f, 0.f)));
}

CPipeLine::~CPipeLine()
{
	Free();
}

void CPipeLine::Update()
{
	if (m_pMainCamera)
	{
		m_pMainCamera->Set_Transform();
	}

	_matrix ViewInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[TS_VIEW]));
	XMStoreFloat4x4(&m_TransformationMatrices[TS_VIEW_INV], ViewInv);
	XMStoreFloat4x4(&m_TransformationMatrices[TS_PROJ_INV], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[TS_PROJ])));
	XMStoreFloat4(&m_vCamPosition, ViewInv.r[3]);
	XMStoreFloat4(&m_vCamLook, XMVector3Normalize(ViewInv.r[2]));
	Calc_Shadow_Matrix();
}

void CPipeLine::Set_ShadowDir(_fvector vDir)
{
	XMStoreFloat4(&m_vShadowDir, XMVector3Normalize(vDir));
}

void CPipeLine::Set_ShadowDir(const _float4& vDir)
{
	Set_ShadowDir(XMLoadFloat4(&vDir));
}

void CPipeLine::Set_Transform(PIPELINE_STATE eState, _fmatrix Matrix)
{
	XMStoreFloat4x4(&m_TransformationMatrices[eState], Matrix);
}

void CPipeLine::Set_Transform(PIPELINE_STATE eState, const _float4x4& Matrix)
{
	m_TransformationMatrices[eState] = Matrix;
}

_fmatrix CPipeLine::Get_LightView_Matrix() const
{
	return XMLoadFloat4x4(&m_LightView);
}

const _float4x4& CPipeLine::Get_LightView_Float4x4() const
{
	return m_LightView;
}

_fmatrix CPipeLine::Get_LightProj_Matrix() const
{
	return XMLoadFloat4x4(&m_LightProjection);
}

const _float4x4& CPipeLine::Get_LightProj_Float4x4() const
{
	return m_LightProjection;
}

_fmatrix CPipeLine::Get_Transform_Matrix(PIPELINE_STATE eState) const
{
	return XMLoadFloat4x4(&m_TransformationMatrices[eState]);
}

const _float4x4& CPipeLine::Get_Transform_Float4x4(PIPELINE_STATE eState) const
{
	return m_TransformationMatrices[eState];
}

const _float4& CPipeLine::Get_CamPosition_Float4() const
{
	return m_vCamPosition;
}

_fvector CPipeLine::Get_CamPosition_Vector() const
{
	return XMLoadFloat4(&m_vCamPosition);
}

const _float4& CPipeLine::Get_CamLook_Float4() const
{
	return m_vCamLook;
}

_fvector CPipeLine::Get_CamLook_Vector() const
{
	return XMLoadFloat4(&m_vCamLook);
}

void CPipeLine::Add_Camera(const _wstring& strTag, shared_ptr<CCamera> pCamera)
{
	if (m_Cameras.count(strTag))
		return;
	m_Cameras[strTag] = pCamera;
}

shared_ptr<CCamera> CPipeLine::Get_Camera(const _wstring& strTag)
{
	if (!m_Cameras.count(strTag))
		return nullptr;
	return m_Cameras[strTag];
}

shared_ptr<CCamera> CPipeLine::Get_MainCamera()
{
	return m_pMainCamera;
}

void CPipeLine::Set_MainCamera(const _wstring& strTag)
{
	m_pMainCamera = Get_Camera(strTag);
	if (m_pMainCamera)
	{
		m_strMainCamTag = strTag;
	}
	else
	{
		m_strMainCamTag = L"";
	}
}

void CPipeLine::Remove_Camera(const _wstring& strTag)
{
	if (!m_Cameras.count(strTag))
		return;
	if (strTag == m_strMainCamTag)
	{
		m_pMainCamera = nullptr;
	}
	m_Cameras.erase(strTag);
}

void CPipeLine::Clear_Camera()
{
	m_pMainCamera = nullptr;
	m_Cameras.clear();
}

void CPipeLine::Calc_Shadow_Matrix()
{
	_vector vCamPos = XMLoadFloat4(&m_vCamPosition);
	_vector vLightDir = XMLoadFloat4(&m_vShadowDir);
	_vector lightPosition = XMVectorAdd(vCamPos, XMVectorScale(vLightDir, -30.0f));


	_vector lightTarget = vCamPos;

	_vector lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	_matrix lightViewMatrix = XMMatrixLookAtLH(lightPosition, lightTarget, lightUp);

	XMStoreFloat4x4(&m_LightView, lightViewMatrix);


	_float2 vResolution = GAMEINSTANCE->Get_Resolution();
	_matrix lightProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), vResolution.x / vResolution.y, 0.1f, 3000.f);
	XMStoreFloat4x4(&m_LightProjection, lightProjectionMatrix);

}


shared_ptr<CPipeLine> CPipeLine::Create()
{
	MAKE_SHARED_ENABLER(CPipeLine)

	return make_shared<MakeSharedEnabler>();
}

void CPipeLine::Free()
{
	Clear_Camera();
}
