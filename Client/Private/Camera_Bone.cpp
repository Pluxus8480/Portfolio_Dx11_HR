#include "Camera_Bone.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "Player_Camera.h"

CCamera_Bone::CCamera_Bone()
{
	XMStoreFloat4x4(&m_RotateOffsetInverse, XMMatrixInverse(nullptr, XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90.f), XMConvertToRadians(90.f))));
}

_bool CCamera_Bone::Update(_float fTimeDelta)
{
	if (m_fTimerLimit <= m_fTimer)
	{
		return true;
	}
	_float fLerp = m_fTimer / m_fTimerLimit;
	m_fYaw = XM_PI * 0.25f * fLerp - XM_PI * 0.25f * (1.f - fLerp);
	
	_matrix Bone = XMLoadFloat4x4(&Identity);

	Bone.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vOffset = XMVector3Rotate(XMVectorSet(0.f, 0.f, m_fDistance, 0.f), XMQuaternionRotationRollPitchYaw(-XM_PIDIV4 * 0.5f, m_fYaw, 0.f));
	_vector vTarget = Bone.r[3];
	_vector vNormOff = XMVector3Normalize(vOffset);
	Bone.r[3] += vOffset;
	XMStoreFloat4x4(&m_BoneMatrix, Bone);
	LookAt(vTarget);

	m_fTimer += fTimeDelta;

	if (m_fTimerLimit <= m_fTimer)
	{
		PLAYERMANAGER->Get_Player()->Get_Player_Camera()->End_Following_Bone();
		return true;
	}

	if (!PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Is_Following() && m_pFollow.lock())
	{
		PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Change_Following_Bone(m_pFollow.lock()->Get_WorldMatrix_Ptr(), &m_BoneMatrix);
	}
	return false;
}

void CCamera_Bone::Initialize(_float fTimerLimit, _float fCamDistance, shared_ptr<CTransform> pTransform)
{
	m_fTimer = 0.f;
	m_fTimerLimit = fTimerLimit;
	PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Change_Following_Bone(pTransform->Get_WorldMatrix_Ptr(), &m_BoneMatrix);
	m_fDistance = fCamDistance;
	m_pFollow = pTransform;
	m_fYaw = XM_PI * 0.75f;

}

void CCamera_Bone::LookAt(_vector vAt)
{
	_matrix Bone = XMLoadFloat4x4(&m_BoneMatrix);
	_float3 vScale{ XMVector3Length(Bone.r[0]).m128_f32[0], XMVector3Length(Bone.r[1]).m128_f32[0], XMVector3Length(Bone.r[2]).m128_f32[0] };
	
	_vector vLook = vAt - Bone.r[3];
	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
		return;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Bone.r[0] = XMVector3Normalize(vRight) * vScale.x;
	Bone.r[1] = XMVector3Normalize(vUp) * vScale.y;
	Bone.r[2] = XMVector3Normalize(vLook) * vScale.z;
	XMStoreFloat4x4(&m_BoneMatrix, XMLoadFloat4x4(&m_RotateOffsetInverse) * Bone);
}
