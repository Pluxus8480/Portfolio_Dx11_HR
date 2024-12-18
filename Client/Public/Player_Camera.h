#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Player : public CCamera
{
public:
	struct CAMERA_FREE_DESC : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = {};
	};
	enum CAM_EFFECT_TYPE
	{
		CE_SHAKE,
		CE_DISSOLVE,
		CE_END,
	};

	enum LENS_EFFECT_TYPE
	{
		LE_REDSCREEN,
		LE_COLORSCREEN,
		LE_END,
	};

	struct CAM_EFFECT
	{
		_float			fTimer;
		_float			fPower;
		_float			fDuration;
	};
	struct LENS_EFFECT
	{
		_float			fTimer;
		_float4			vColor;
		_float			fDuration;
	};

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);

public:
	~CCamera_Player();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_Player(shared_ptr<CGameObject> pPlayer)
	{
		m_pPlayer = pPlayer;
	}

	void Set_Lock_On_Target(shared_ptr<CGameObject> pTarget) { m_pLock_On_Target = pTarget; }
	shared_ptr<CGameObject> Get_Lock_On_Target() { return m_pLock_On_Target; }

	void Set_Gimmic_Lock_On_Target(shared_ptr<CGameObject> pTarget) { m_pGimmic_Lock_On_Target = pTarget; }
	shared_ptr<CGameObject> Get_Gimmic_Lock_On_Target() { return m_pGimmic_Lock_On_Target; }

	void Set_Wire_Target(shared_ptr<class CMagnet> pTarget) { m_list_Wire_Target.push_back(pTarget); }
	list < shared_ptr<class CMagnet>> Get_Wire_Target() { return m_list_Wire_Target; }

	void Start_Following_Bone(_float4x4* pWorldMatrix, _float4x4* pBoneMatrix);
	void Change_Following_Bone(_float4x4* pWorldMatrix, _float4x4* pBoneMatrix);
	void End_Following_Bone();

	void Shake(_float fDuration, _float fPower, _bool bClear = true);

	void Rotation_Targeting(_fvector vLook, _float fPitch, _float fYaw);

	_float Get_DistanceTarget()const { return m_fDistanceTarget; }
	_float Get_Original_DistanceTarget()const { return m_fOriginal_DistanceTarget; }
	void Set_DistanceTarget(_float fDistance) { if(!m_b2DScroll) m_fDistanceTarget = fDistance;  }

	void Start_2DScroll(_fvector vLook, _float fDistance);
	void Start_2DScroll(const _float3& vLook, _float fDistance);

	void End_2DScroll();

	void Camera_Fixed(shared_ptr<CTransform> pFixTransform, _fvector vLookDir, _float fDistance);
	void End_Camera_Fixed();

	_bool Is_Following() const { return m_bFollowBone; };
	void Set_Fixed(_bool bFixed) { m_bFixed = bFixed; }
private:
	void Track_Player(_float fTimeDelta);
	void Follow_Bone(_float fTimeDelta);
	void Interpolate(_float fTimeDelta);
	shared_ptr<CGameObject> Auto_LockOn_Target(list<shared_ptr<CGameObject>> vecTargetList);
	void Apply_Effect();
	void _Shake(_float fTimeDelta);
private:
	_float m_fMouseSensor = {};
	_float m_fDistance = {2.6f};
	_float m_fDistanceTarget = {3.6f};
	_float m_fOriginal_DistanceTarget = {3.6f};
	_float3 m_vPitchYawRoll = {0.f,0.f,0.f};
	_float3 m_vPitchYawRollTarget = {0.f,0.f,0.f};

	_float m_fInputTimer = 0.f;

	_float  m_fInterpolateSpeed = 7.f;

	weak_ptr<CGameObject> m_pPlayer;
	shared_ptr<CGameObject> m_pLock_On_Target = nullptr;
	shared_ptr<CGameObject> m_pGimmic_Lock_On_Target = nullptr;
	list<shared_ptr<class CMagnet>> m_list_Wire_Target = {};
	_float4x4* m_pFollowWorld = nullptr;
	_float4x4* m_pBoneToFollow = nullptr;
	_bool m_bFollowBone = false;
	_float m_fEnteringBoneTimer = 0.f;

	_bool m_b2DScroll = false;
	_float3 m_v2DScrollLook{};
	_float m_f2DDistance = 0.f;

	_bool m_bFixed = false;
	shared_ptr<CTransform> m_pFixedTransform = {};
	_float m_fFixedTimer = 0.f;

	_float4x4 m_EffectOffset{};
	list<CAM_EFFECT> m_arrEffect[CE_END];
	list<LENS_EFFECT> m_arrLensEffect[LE_END];

	_bool m_bLensEffect[LE_END]{};

public:
	void SetTransform(_float4x4 matTrans);


public:
	static shared_ptr<CCamera_Player> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END