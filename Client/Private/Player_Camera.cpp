#include "Player_Camera.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "PlayerManager.h"
#include "CombatManager.h"
CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs)
	: CCamera(rhs)
{
}

CCamera_Player::~CCamera_Player()
{
	Free();
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayer = CPlayer_Manager::Get_Instance()->Get_Player();

	return S_OK;
}

void CCamera_Player::PriorityTick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	//{
	//	POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	//	ClientToScreen(g_hWnd, &ptWindow);
	//	SetCursorPos(ptWindow.x, ptWindow.y);
	//}
	Track_Player(fTimeDelta);
	Follow_Bone(fTimeDelta);
	_Shake(fTimeDelta);
	Apply_Effect();
	__super::PriorityTick(fTimeDelta);
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	/*if (m_pGameInstance.lock()->Get_MouseDown(DIMK_LBUTTON))
	{
		_uint res = m_pGameInstance->Pixel_Picking_Tool(TEXT("Target_PixelPicking"), g_iWinSizeX, g_iWinSizeY, g_hWnd);
		if (res == 0)
			return;

		CImGui_Manager::Get_Instance()->Select_Tool_Object(g_ObjectMap[res]);

	}*/
}

void CCamera_Player::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

void CCamera_Player::Start_Following_Bone(_float4x4* pWorldMatrix,_float4x4* pBoneMatrix)
{
	m_fEnteringBoneTimer = 0.f;
	m_bFollowBone = true;
	m_pFollowWorld = pWorldMatrix;
	m_pBoneToFollow = pBoneMatrix;
}
void CCamera_Player::Change_Following_Bone(_float4x4* pWorldMatrix, _float4x4* pBoneMatrix)
{
	m_fEnteringBoneTimer = 0.1f;
	m_bFollowBone = true;
	m_pFollowWorld = pWorldMatrix;
	m_pBoneToFollow = pBoneMatrix;
}
void CCamera_Player::End_Following_Bone()
{
	if (!m_bFollowBone)
		return;
	//m_fEnteringBoneTimer = 0.1f;
	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	auto m_pTargetTransform = m_pPlayer.lock()->Get_Transform();
	_vector vOffset = XMVector3Normalize(World.r[3] - m_pTargetTransform->Get_Position());
	_float fYaw = atan2(vOffset.m128_f32[0], vOffset.m128_f32[2]);

	_float fXZLen = sqrtf(vOffset.m128_f32[0] * vOffset.m128_f32[0] + vOffset.m128_f32[2] * vOffset.m128_f32[2]);
	_float fPitch = atan2(-vOffset.m128_f32[1], fXZLen);
	m_vPitchYawRollTarget.x = fPitch;
	m_vPitchYawRoll.x = fPitch;
	m_vPitchYawRollTarget.y = fYaw;
	m_vPitchYawRoll.y = fYaw;
	m_bFollowBone = false;
}

void CCamera_Player::Shake(_float fDuration, _float fPower, _bool bClear)
{
	if (bClear)
		m_arrEffect[CE_SHAKE].clear();
	m_arrEffect[CE_SHAKE].push_back({ fDuration, fPower, fDuration });

}

void CCamera_Player::Rotation_Targeting(_fvector vLook, _float fPitch, _float fYaw)
{
	m_vPitchYawRollTarget.x = fPitch;
	m_vPitchYawRollTarget.y = atan2(-XMVectorGetX(vLook), -XMVectorGetZ(vLook)) + fYaw;
}

void CCamera_Player::Start_2DScroll(_fvector vLook, _float fDistance)
{
	XMStoreFloat3(&m_v2DScrollLook, XMVector3Normalize(vLook));
	m_b2DScroll = true;
	m_f2DDistance = fDistance;
	m_fDistanceTarget = m_f2DDistance;
	_float fYaw = atan2(-m_v2DScrollLook.x, -m_v2DScrollLook.z);
	_float fXZLen = sqrtf(m_v2DScrollLook.x * m_v2DScrollLook.x + m_v2DScrollLook.z * m_v2DScrollLook.z);
	_float fPitch = atan2(m_v2DScrollLook.y, fXZLen);
	m_vPitchYawRollTarget.x = fPitch;
	m_vPitchYawRollTarget.y = fYaw;
}

void CCamera_Player::Start_2DScroll(const _float3& vLook, _float fDistance)
{
	Start_2DScroll(XMLoadFloat3(&vLook), fDistance);
}

void CCamera_Player::End_2DScroll()
{
	m_b2DScroll = false;
	m_fDistanceTarget = m_fOriginal_DistanceTarget;
}

void CCamera_Player::Camera_Fixed(shared_ptr<CTransform> pFixTransform, _fvector vLookDir , _float fDistance)
{
	m_bFixed = true;
	m_pFixedTransform = pFixTransform;
	_float3 fDir = {};
	
	XMStoreFloat3(&fDir, vLookDir);
	Start_2DScroll(fDir, fDistance);
}

void CCamera_Player::End_Camera_Fixed()
{
	m_b2DScroll = false;
	m_bFixed = false;
	m_fDistanceTarget = m_fOriginal_DistanceTarget;
}

void CCamera_Player::Track_Player(_float fTimeDelta)
{
	m_fInputTimer -= fTimeDelta;
	if(!m_b2DScroll && !m_bFixed)
	{
		_long MouseMoveX = m_pGameInstance.lock()->Get_DIMouseMove(DIMM_X);
		if (MouseMoveX != 0)
		{
			m_vPitchYawRollTarget.y += fTimeDelta * MouseMoveX * m_fMouseSensor * 10.f;
			m_fInputTimer = 1.5f;
		}
		m_vPitchYawRollTarget.x -= fTimeDelta * m_pGameInstance.lock()->Get_DIMouseMove(DIMM_Y) * m_fMouseSensor * 10.f;
	}


	if (m_vPitchYawRollTarget.x < -XM_PIDIV2 + 0.01f)
	{
		m_vPitchYawRollTarget.x = -XM_PIDIV2 + 0.01f;
	}
	if (m_vPitchYawRollTarget.x > XM_PIDIV2 - 0.01f)
	{
		m_vPitchYawRollTarget.x = XM_PIDIV2 - 0.01f;
	}
	//m_fDistance += fTimeDelta * m_pGameInstance.lock()->Get_DIMouseMove(DIMM_WHEEL) * m_fMouseSensor;
	if (m_fDistanceTarget <= 1.f)
	{
		m_fDistanceTarget = 1.f;
	}
	if (m_fDistanceTarget >= 40.f)
	{
		m_fDistanceTarget = 40.f;
	}
	Interpolate(fTimeDelta);
	shared_ptr<CTransform> m_pTargetTransform = nullptr;
	_matrix Matrix;
	
	if (!m_bFixed)
	{
		m_pTargetTransform = m_pPlayer.lock()->Get_Transform();
		Matrix = m_pTargetTransform->Get_WorldMatrix_XMMat();
	}
	else
	{
		m_pTargetTransform = m_pFixedTransform;
		Matrix = m_pTargetTransform->Get_WorldMatrix_XMMat();

		for (size_t i = 0; i < 3; i++)
			Matrix.r[i] = XMVector3Normalize(Matrix.r[i]);
		_matrix mPlayerMat = m_pPlayer.lock()->Get_Transform()->Get_WorldMatrix_XMMat();
		for (size_t i = 0; i < 3; i++)
			mPlayerMat.r[i] = XMVector3Normalize(mPlayerMat.r[i]);

		m_fFixedTimer += fTimeDelta;
		if (m_fFixedTimer > 1.f)
			m_fFixedTimer = 1.f;
		Matrix = mPlayerMat * (1.f - m_fFixedTimer) + Matrix * m_fFixedTimer;

		for (size_t i = 0; i < 3; i++)
			Matrix.r[i] = XMVector3Normalize(Matrix.r[i]);
	}

	if (m_pTargetTransform)
	{

		Matrix.r[3] += XMVectorSet(0, 1.f, 0, 0);
		_vector vOffset = XMVector3Rotate(XMVectorSet(0.f, 0.f, m_fDistance, 0.f), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&m_vPitchYawRoll)));
		_vector vTarget = Matrix.r[3];
		_vector vNormOff = XMVector3Normalize(vOffset);
		_float3 vfPos;
		XMStoreFloat3(&vfPos, vTarget + vNormOff * 2.f);
		if(!m_b2DScroll && !m_bFixed)
		{
			PxSweepBuffer Hit;
			PxTransform Start{ PxVec3{vfPos.x,vfPos.y,vfPos.z} };
			_float3 vDirection;
			XMStoreFloat3(&vDirection, vNormOff);
			PxVec3 pvDist = PxVec3{ vDirection.x,vDirection.y,vDirection.z };
			pvDist.normalize();
			if (m_pGameInstance.lock()->PxSweep(Start, pvDist, m_fDistance, 1.f, Hit))
			{
				_float fDistance = Hit.block.distance;
				vOffset = vNormOff * (fDistance + 1.f);
			}
		}
		Matrix.r[3] += vOffset;
		m_pTransformCom->Set_WorldMatrix(Matrix);
		m_pTransformCom->LookAt(vTarget);

		if(!m_b2DScroll && !m_bFixed)
		{
			if (m_pLock_On_Target)
			{
				if (m_pLock_On_Target->Is_Dead())
					m_pLock_On_Target = nullptr;
				else
				{
					if (XMVector3Length(m_pLock_On_Target->Get_Transform()->Get_Position() - m_pTargetTransform->Get_Position()).m128_f32[0] > 10.f)
					{
						m_pLock_On_Target = nullptr;
					}
				}
			}
			if(!m_pLock_On_Target)
			{
				m_pLock_On_Target = Auto_LockOn_Target(CCombat_Manager::Get_Instance()->Get_Enemy_List());
				if (m_pLock_On_Target && XMVector3Length(m_pLock_On_Target->Get_Transform()->Get_Position() - m_pTargetTransform->Get_Position()).m128_f32[0] > 8.f)
				{
					m_pLock_On_Target = nullptr;
				}
			}
			

			if (m_pLock_On_Target && m_fInputTimer <= 0.f)
			{
				auto pLockOnTargetTransform = m_pLock_On_Target->Get_Transform();
				_matrix LockOnMatrix = pLockOnTargetTransform->Get_WorldMatrix_XMMat();

				_vector vLockOnDir = (vTarget - LockOnMatrix.r[3]);
				if (XMVector3Length(vLockOnDir).m128_f32[0] < 0.1f)
					return;
				_float fX = XMVectorGetX(vLockOnDir);
				_float fZ = XMVectorGetZ(vLockOnDir);
				_float fAngle = atan2(fX, fZ);
				_float fDiff = fAngle - m_vPitchYawRollTarget.y;
				if (fDiff < 0.f)
				{
					while (fDiff < -XM_PI)
					{
						fAngle += XM_2PI;
						fDiff = fAngle - m_vPitchYawRollTarget.y;
					}
				}
				if (fDiff > 0.f)
				{
					while (fDiff > XM_PI)
					{
						fAngle -= XM_2PI;
						fDiff = fAngle - m_vPitchYawRollTarget.y;
					}
				}
				m_vPitchYawRollTarget.y = fAngle;
			}
		}
	}
}

void CCamera_Player::Follow_Bone(_float fTimeDelta)
{
	if(m_bFollowBone)
	{
		if (m_fEnteringBoneTimer < 0.1f)
			m_fEnteringBoneTimer += fTimeDelta;
		else
			m_fEnteringBoneTimer = 0.1f;
	}
	else
	{
		if (m_fEnteringBoneTimer > 0.0f)
			m_fEnteringBoneTimer -= fTimeDelta * 0.5f;
		else
			m_fEnteringBoneTimer = 0.0f;

	}
	if (m_fEnteringBoneTimer <= 0.f || !m_pBoneToFollow || !m_pFollowWorld)
		return;
	_matrix FollowNoScale = XMLoadFloat4x4(m_pBoneToFollow);
	for (_uint i = 0; i < 3; ++i)
	{
		FollowNoScale.r[i] = XMVector3Normalize(FollowNoScale.r[i]);
	}

	_matrix FollowingBone = XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90.f), XMConvertToRadians(90.f)) * FollowNoScale * XMLoadFloat4x4(m_pFollowWorld);
	_float fLerp = m_fEnteringBoneTimer / 0.1f;
	_matrix LerpedMatrix = m_pTransformCom->Get_WorldMatrix_XMMat() * (1.f - fLerp) + FollowingBone * fLerp;
	m_pTransformCom->Set_WorldMatrix(LerpedMatrix);

}

void CCamera_Player::Interpolate(_float fTimeDelta)
{

	while (m_vPitchYawRollTarget.y >= XM_2PI)
	{
		m_vPitchYawRollTarget.y -= XM_2PI;
	}
	while (m_vPitchYawRollTarget.y < 0.f)
	{
		m_vPitchYawRollTarget.y += XM_2PI;
	}
	while (m_vPitchYawRoll.y >= XM_2PI)
	{
		m_vPitchYawRoll.y -= XM_2PI;
	}
	while (m_vPitchYawRoll.y < 0.f)
	{
		m_vPitchYawRoll.y += XM_2PI;
	}

	if (m_fInterpolateSpeed * fTimeDelta >= 1.f)
	{
		m_vPitchYawRoll.x = m_vPitchYawRollTarget.x;
		m_vPitchYawRoll.y = m_vPitchYawRollTarget.y;
		m_vPitchYawRoll.z = m_vPitchYawRollTarget.z;
		m_fDistance = m_fDistanceTarget;
	}
	else
	{
		_float fYawDiff = m_vPitchYawRollTarget.x - m_vPitchYawRoll.x;
		if (abs(fYawDiff) > 0.01f)
		{
			m_vPitchYawRoll.x += (fYawDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_vPitchYawRoll.x = m_vPitchYawRollTarget.x;
		}
		_float fPitchDiff = m_vPitchYawRollTarget.y - m_vPitchYawRoll.y;
		//_float fPitchDiffRev = XM_2PI - fPitchDiff;
		if (abs(fPitchDiff) > 0.01f)
		{
			if (fPitchDiff < 0.f)
			{
				if (abs(fPitchDiff) > abs(XM_2PI + fPitchDiff))
				{
					fPitchDiff = XM_2PI + fPitchDiff;
				}
			}
			else
			{
				if (abs(fPitchDiff) > XM_2PI - fPitchDiff)
				{
					fPitchDiff = -XM_2PI + fPitchDiff;
				}
			}
			m_vPitchYawRoll.y += (fPitchDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_vPitchYawRoll.y = m_vPitchYawRollTarget.y;
		}

		_float fRollDiff = m_vPitchYawRollTarget.z - m_vPitchYawRoll.z;
		if (abs(fPitchDiff) > 0.01f)
		{
			m_vPitchYawRoll.z += (fPitchDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_vPitchYawRoll.z = m_vPitchYawRollTarget.z;
		}

		_float fDistDiff = m_fDistanceTarget - m_fDistance;
		if (abs(fDistDiff) > 0.01f)
		{
			m_fDistance += (fDistDiff)*m_fInterpolateSpeed * fTimeDelta;
		}
		else
		{
			m_fDistance = m_fDistanceTarget;
		}
	}

}

shared_ptr<CGameObject> CCamera_Player::Auto_LockOn_Target(list<shared_ptr<CGameObject>> vecTargetList)
{
	auto playerTransform = m_pPlayer.lock()->Get_Transform();
	_float fDistance = FLT_MAX;
	shared_ptr<CGameObject> lockOnTarget = nullptr;
	for (auto& iter : vecTargetList)
	{
		auto targetTransform = iter->Get_Transform();
	
		_float length = XMVectorGetX(XMVector3Length(targetTransform->Get_Position() - playerTransform->Get_Position()));
	
		if (fDistance > length)
		{
			fDistance = length;
			lockOnTarget = iter;
		}
	}
	return lockOnTarget;
}

void CCamera_Player::Apply_Effect()
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_EffectOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
	XMStoreFloat4x4(&m_EffectOffset, XMMatrixIdentity());
}

void CCamera_Player::_Shake(_float fTimeDelta)
{
	auto& effectVec = m_arrEffect[CE_SHAKE];

	auto iter = effectVec.begin();
	if (iter == effectVec.end())
		return;

	_matrix Shake = XMMatrixTranslation(GAMEINSTANCE->Random_Float(-iter->fPower, iter->fPower), GAMEINSTANCE->Random_Float(-iter->fPower, iter->fPower), 0.f);
	XMStoreFloat4x4(&m_EffectOffset, XMLoadFloat4x4(&m_EffectOffset) * Shake);

	if (iter->fTimer <= 0.f)
	{
		iter = effectVec.erase(iter);
		return;
	}
	iter->fTimer -= fTimeDelta;
}

void CCamera_Player::SetTransform(_float4x4 matTrans)
{
	m_pTransformCom->Set_WorldMatrix(matTrans);

	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	auto m_pTargetTransform = m_pPlayer.lock()->Get_Transform();
	_vector vOffset = World.r[3] - m_pTargetTransform->Get_Position();
	m_fDistance = XMVectorGetX(XMVector3Length(vOffset));
	vOffset = XMVector3Normalize(vOffset);
	_float fYaw = atan2(vOffset.m128_f32[0], vOffset.m128_f32[2]);

	_float fXZLen = sqrtf(vOffset.m128_f32[0] * vOffset.m128_f32[0] + vOffset.m128_f32[2] * vOffset.m128_f32[2]);
	_float fPitch = atan2(-vOffset.m128_f32[1], fXZLen);
	m_vPitchYawRollTarget.x = fPitch;
	m_vPitchYawRoll.x = fPitch;
	m_vPitchYawRollTarget.y = fYaw;
	m_vPitchYawRoll.y = fYaw;
	m_bFollowBone = false;
}

shared_ptr<CCamera_Player> CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CCamera_Player
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCamera_Player(pDevice, pContext) { }
	};

	shared_ptr<CCamera_Player> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CCamera_Player::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CCamera_Player
	{
		MakeSharedEnabler(const CCamera_Player& rhs) : CCamera_Player(rhs) { }
	};

	shared_ptr<CCamera_Player> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		assert(false);
	}

	return pInstance;
}

void CCamera_Player::Free()
{

}
