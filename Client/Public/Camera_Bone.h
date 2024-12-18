#pragma once
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END
class CCamera_Bone
{
public:
	CCamera_Bone();

	_bool Update(_float fTimeDelta);
	void Initialize(_float fTimerLimit, _float fCamDistance, shared_ptr<CTransform> pTransform);
private:
	void LookAt(_fvector vAt);

public:
	_float m_fTimer = 0.f;
	_float m_fTimerLimit = 0.f;
	_float m_fDistance = 0.f;
	_float m_fYaw = 0.f;
	_float4x4 m_BoneMatrix = Identity;
	_float4x4 m_RotateOffsetInverse = Identity;
	weak_ptr<CTransform> m_pFollow;
};

