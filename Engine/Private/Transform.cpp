#include "..\Public\Transform.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent{ rhs },
	m_WorldMatrix(rhs.m_WorldMatrix)
{
}

CTransform::~CTransform()
{
	Free();
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{

	if (nullptr == pArg)
	{
		m_fSpeedPerSec = 1.f;
		m_fRotationPerSec = 1.f;
		return S_OK;
	}

	TRANSFORM_DESC* pTransformDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
	m_fRotationPerSec = pTransformDesc->fRotationPerSec;
	Set_State(STATE_POSITION, XMLoadFloat4(&pTransformDesc->vPosition));

	return S_OK;
}

_vector CTransform::Get_State(STATE eState) const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
}

void CTransform::Set_State(STATE eState, _fvector vVec)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vVec);
	if (eState == STATE_POSITION)
	{
		m_WorldMatrix._44 = 1.f;
	}
}

_float3 CTransform::Get_Scale() const
{
	_matrix Mat = XMLoadFloat4x4(&m_WorldMatrix);

	return _float3(XMVectorGetX(XMVector3Length(Mat.r[0])), XMVectorGetX(XMVector3Length(Mat.r[1])), XMVectorGetX(XMVector3Length(Mat.r[2])));
}

void CTransform::Set_Scale(const _float3& vScale)
{
	_matrix Mat = XMLoadFloat4x4(&m_WorldMatrix);
	Set_State(STATE_RIGHT, XMVector3Normalize(Mat.r[0]) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Mat.r[1]) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Mat.r[2]) * vScale.z);

}

void CTransform::Set_Scale(_fvector vScale)
{
	_float3 vfScale;
	XMStoreFloat3(&vfScale, vScale);
	Set_Scale(vfScale);
}


void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);
	vPosition += XMVector3Normalize(vLook) * fTimeDelta * m_fSpeedPerSec;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);
	vPosition -= XMVector3Normalize(vLook) * fTimeDelta * m_fSpeedPerSec;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);
	vPosition -= XMVector3Normalize(vRight) * fTimeDelta * m_fSpeedPerSec;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);
	vPosition += XMVector3Normalize(vRight) * fTimeDelta * m_fSpeedPerSec;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_UP);
	vPosition += XMVector3Normalize(vUp) * fTimeDelta * m_fSpeedPerSec;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_UP);
	vPosition -= XMVector3Normalize(vUp) * fTimeDelta * m_fSpeedPerSec;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Vector(_fvector vDir, _float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	vPosition += XMVector3Normalize(vDir) * fTimeDelta * m_fSpeedPerSec;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fAngle)
{
	_float3 vScale = Get_Scale();
	_vector vRight = XMVectorSet(vScale.x, 0.f, 0.f, 0.f);
	_vector vUp = XMVectorSet(0.f, vScale.y, 0.f, 0.f);
	_vector vLook = XMVectorSet(0.f, 0.f, vScale.z, 0.f);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fAngle);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation_Quaternion(_fvector vRotation)
{
    // Scale 가져오기
    _float3 vScale = Get_Scale();

    // 각 축의 기본 벡터 설정
    _vector vRight = XMVectorSet(vScale.x, 0.f, 0.f, 0.f);
    _vector vUp = XMVectorSet(0.f, vScale.y, 0.f, 0.f);
    _vector vLook = XMVectorSet(0.f, 0.f, vScale.z, 0.f);

    // 각 축에 대해 독립적인 쿼터니언 회전 계산
    _vector vQuatX = XMQuaternionRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMVectorGetX(vRotation));  // X축 회전
    _vector vQuatY = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorGetY(vRotation));  // Y축 회전
    _vector vQuatZ = XMQuaternionRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVectorGetZ(vRotation));  // Z축 회전

    // 각 축 쿼터니언을 조합하여 최종 회전 쿼터니언 계산 (순서 조정 가능)
    _vector vQuatFinal = XMQuaternionMultiply(vQuatZ, XMQuaternionMultiply(vQuatY, vQuatX));

    // 쿼터니언 정규화: 회전이 변형되지 않도록 단위 쿼터니언으로 정규화
    vQuatFinal = XMQuaternionNormalize(vQuatFinal);

    // 최종 회전 쿼터니언으로 회전 행렬 생성
    _matrix RotationMatrix = XMMatrixRotationQuaternion(vQuatFinal);

    // 회전된 방향 벡터 계산
    _vector vNewRight = XMVector3TransformNormal(vRight, RotationMatrix);
    _vector vNewUp = XMVector3TransformNormal(vUp, RotationMatrix);
    _vector vNewLook = XMVector3TransformNormal(vLook, RotationMatrix);

    // 상태 갱신
    Set_State(STATE_RIGHT, vNewRight);
    Set_State(STATE_UP, vNewUp);
    Set_State(STATE_LOOK, vNewLook);
}

void CTransform::Rotation_Override(_fvector vRotation)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	_vector vQuat = XMQuaternionRotationRollPitchYawFromVector(vRotation);
	_matrix RotationMatrix = XMMatrixRotationQuaternion(vQuat);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fTimeDelta * m_fRotationPerSec);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fRadianPersec, _float fTimeDelta)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fTimeDelta * fRadianPersec);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

_vector CTransform::Get_NormRight()
{
	return XMVector3Normalize(Get_State(STATE_RIGHT));
}

_vector CTransform::Get_NormUp()
{
	return XMVector3Normalize(Get_State(STATE_UP));
}

_vector CTransform::Get_NormLook()
{
	return XMVector3Normalize(Get_State(STATE_LOOK));
}

void CTransform::LookAt(_fvector vAt)
{
	_float3 vScale = Get_Scale();
	_vector vLook = vAt - Get_State(STATE_POSITION);
	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f || (fabs(XMVectorGetX(vLook)) < 0.0001f && fabs(XMVectorGetZ(vLook)) < 0.0001f))
		return;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);

}

void CTransform::LookAt_Horizontal(_fvector vAt)
{
	_float3 vScale = Get_Scale();
	_vector vLook = vAt - Get_State(STATE_POSITION);
	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f || (fabs(XMVectorGetX(vLook)) < 0.0001f && fabs(XMVectorGetZ(vLook)) < 0.0001f))
		return;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vUp, vLook);
	vLook = XMVector3Cross(vRight, vUp);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Vertical(_fvector vAt)
{
	_float3 vScale = Get_Scale();
	_vector vLook = vAt - Get_State(STATE_POSITION);

	// vLook의 y축만 카메라의 y축으로 세팅
	vLook = XMVectorSetY(vLook, XMVectorGetY(vAt - Get_State(STATE_POSITION)));
	vLook = XMVector3Normalize(vLook);

	_vector vUp = XMVector3Normalize(Get_State(STATE_UP));

	// vLook 벡터가 너무 작은 경우, 처리를 멈춘다.
	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
		return;

	// vRight와 vLook을 이용하여 Up 벡터를 계산
	_vector vRight = XMVector3Cross(vLook, vUp);

	// 정규화 및 축별 스케일 적용
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Vertical_Up(_fvector vAt)
{
	_float3 vScale = Get_Scale();
	_vector vLook = vAt - Get_State(STATE_POSITION);

	// vLook의 y축만 카메라의 y축으로 세팅
	vLook = XMVectorSetY(vLook, XMVectorGetY(vAt - Get_State(STATE_POSITION)));
	vLook = XMVector3Normalize(vLook);

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	// vLook 벡터가 너무 작은 경우, 처리를 멈춘다.
	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
		return;

	// vRight와 vLook을 이용하여 Up 벡터를 계산
	_vector vRight = XMVector3Cross(vLook, vUp);

	// 정규화 및 축별 스케일 적용
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Horizontal_With_Speed(_fvector vAt, _float fTimeDelta)
{
	if (fTimeDelta > 1.f)
		fTimeDelta = 1.f;

	_float3		vScaled = Get_Scale();

	_vector		vTargetLook = XMVector3Normalize(XMVectorSetY(vAt - Get_State(STATE_POSITION), 0.f));
	_vector		vCurLook = XMVector3Normalize(XMVectorSetY(Get_State(STATE_LOOK), 0.f));

	if (XMVectorGetX(XMVector3LengthSq(vTargetLook - vCurLook)) < 0.0001f)
		return;

	XMVECTOR rotationAxis = XMVector3Normalize(XMVector3Cross(vCurLook, vTargetLook));
	if (XMVectorGetX(XMVector3LengthSq(rotationAxis)) < 0.0001f)
		return;

	float dotProduct = XMVectorGetX(XMVector3Dot(vTargetLook, vCurLook));
	float fAngle = acos(dotProduct) * fTimeDelta;
	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(rotationAxis, fAngle);

	_vector		vLook = XMVector3Normalize(XMVector3Rotate(vCurLook, rotationQuaternion));
	if (XMVector4IsNaN(vLook) || XMVectorGetX(XMVector3LengthSq(vLook)) < FLT_MIN)
		return;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

HRESULT CTransform::Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

_float4x4 CTransform::Get_WorldMatrix_Inversed() const
{
	auto Matrix = XMLoadFloat4x4(&m_WorldMatrix);
	Matrix = XMMatrixInverse(nullptr, Matrix);
	_float4x4 MatRet;
	XMStoreFloat4x4(&MatRet, Matrix);
	return MatRet;
}

void CTransform::Set_WorldMatrix(_fmatrix Matrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, Matrix);
}

void CTransform::Billboard()
{
	_float3 vScale = Get_Scale();
	_vector vLook = GAMEINSTANCE->Get_MainCamera()->Get_Transform()->Get_State(STATE_POSITION) - Get_State(STATE_POSITION);
	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
		return;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vUp, vLook);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);

}


shared_ptr<CTransform> CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CTransform
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTransform(pDevice, pContext) { }
	};

	shared_ptr<CTransform> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		return nullptr;
		//Safe_Release(pInstance);
	}

	return pInstance;
}


shared_ptr<CComponent> CTransform::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CTransform
	{
		MakeSharedEnabler(const CTransform& rhs) : CTransform(rhs) { }
	};

	shared_ptr<CTransform> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		return nullptr;
		//Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
}
