#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)
class CEffect_Union;

class CMirror_Ball : public CGameObject
{
public:
	static wstring ObjID;

public:
	struct MIRROR_BALL
	{
		_float	fLaserActiveTime = 5.f;
		_float4 vOriginPos = { 0.f,0.f,0.f,0.f };
	};

private:
	CMirror_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMirror_Ball(const CMirror_Ball& rhs);

public:
	virtual ~CMirror_Ball();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Init_Variables();

public:
	_bool Get_MirrorBall_State() { return m_bActive; }

private:
	void Time_Calc(_float fTimeDelta);
	void Pos_Calc(_float fTimeDelta);
	void Dead_Check();
	void Laser_Pause(_float fTimeDelta);

private:
	HRESULT Ready_Components();

	// Laser Control Variables
private:
	_float4 m_vCenterPos = { 0.f,0.f,0.f,0.f };

	// Explode Control Variables
private:
	_float m_fPauseCoolTime = 0.f;
	_float m_fPauseCoolTimeMax = 0.5f;

	_bool  m_bMovePause = false;
	_bool  m_bPauseControl = false;
	// Laser LifeCycle Variables
private:
	_float  m_fMirrorBallTimeMax = 5.f;
	_float  m_fMirrorBallTime = 0.f;
	_float  m_fMirrorBallTimePercent = 0.f;

	_bool   m_bActive = false;

	// Laser Control Effect
private:
	vector<shared_ptr<class CRandom_Laser>> m_pLaserEffects;

public:
	static shared_ptr<CMirror_Ball> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END