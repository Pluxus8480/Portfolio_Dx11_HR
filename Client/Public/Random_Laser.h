#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect_Union;

class CRandom_Laser : public CGameObject
{
public:
	static wstring ObjID;

public:
	struct RANDOM_LASER
	{
		_float fLaserActiveTime = 0.5f;
		_float4 vOriginPos = { 0.f,0.f,0.f,0.f };
	};

private:
	CRandom_Laser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRandom_Laser(const CRandom_Laser& rhs);

public:
	virtual ~CRandom_Laser();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();

public:
	_bool Get_LaserState() { return m_bActive; }
	void Init_Variables();

	_bool Get_Pause() { return m_bPause; }
	void Set_Pause(_bool bPause) { m_bPause = bPause; }

	void Set_ParentPos(_float2 vPos) { m_bParentPos = vPos; }

private:
	void Time_Calc(_float fTimeDelta);
	void Pos_Calc(_float fTimeDelta);
	void Dead_Check();
	void Laser_Explode(_float fTimeDelta);

	// Laser Control Variables
private:
	_float4 m_vCenterPos = { 0.f,0.f,0.f,0.f };
	_float4 m_vStartPos = { 0.f,0.f,0.f,0.f };
	_float4 m_vEndPos = { 0.f,0.f,0.f,0.f };

	_float4 m_vCurrentPos = { 0.f,0.f,0.f,0.f };

	// Explode Control Variables
private:
	_float m_fExplodeCoolTime = 0.f;
	_float m_fExplodeCoolTimeMax = 0.05f;

	_bool  m_bExplode = false;

	// Laser LifeCycle Variables
private:
	_float  m_fLaserTimeMax = 0.5f;
	_float  m_fLaserTime = 0.f;
	_float  m_fLaserTimePercent = 0.f;

	_bool   m_bActive = false;
	
	// Lissajous Variables
private:
	_float  m_fLissajousSpeed = { 1.f };
	_float  m_fLissajousLagrangianX = { 0.f };
	_float  m_fLissajousLagrangianY = { 0.f };
	_float  m_fLissajousPhaseDelta = { 0.f };
	_float  m_fLissajousTime = { 0.f };

	_bool   m_bPause = false;

	_float2 m_bParentPos = { 0.f,0.f };
	// Laser Control Effect

private:
	shared_ptr<CEffect_Union> m_pEffect = {};

public:
	static shared_ptr<CRandom_Laser> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END