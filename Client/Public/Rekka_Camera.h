#pragma once

#include "Client_Defines.h"
#include "Player_Camera.h"


BEGIN(Client)

class CRekka_Camera : public CCamera
{
public:
	static wstring ObjID;

public:


private:
	CRekka_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Camera(const CRekka_Camera& rhs);

public:
	~CRekka_Camera();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void MoveTo(_fvector vTargetPos, _fvector vDir);


public:
	void Start_Following_Bone(_float4x4* pWorldMatrix, _float4x4* pBoneMatrix);
	void Set_Follow(_bool Follow) { m_bFollowBone = Follow; }
	void Set_NextBoneFollow(_bool NextFollow, _float4x4 NextMatrix = {});
	void Follow_Bone(_float fTimeDelta);
	void Shake(_float fDuration, _float fPower, _bool bClear = true);


private:
	void NextFollow_Bone(_float fTimeDelta);
	void _Shake(_float fTimeDelta);
	void Apply_Effect();

private:
	shared_ptr<CGameObject> m_pPlayer = nullptr;
	_float4x4* m_pFollowWorld = nullptr;
	_float4x4* m_pBoneToFollow = nullptr;
	_bool m_bFollowBone = { true };
	_float4x4 m_NextBone = {};
	_float4x4 m_OriginMatrix = {};
	_float4x4 m_DistanceMatrix = {};
	_bool m_bNextFollow = { false };
	_float m_fNextFollowTime = { 0 };
	_float3 m_pNextCameraPos = {};


	_float4x4 m_EffectOffset{};
	list<CCamera_Player::CAM_EFFECT> m_arrEffect[CCamera_Player::CE_END];


public:
	static shared_ptr<CRekka_Camera> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END