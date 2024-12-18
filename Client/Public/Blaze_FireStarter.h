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

class CBlaze_FireStarter : public CGameObject
{

public:
	static wstring ObjID;

public:
	enum class FIRESTARTER_LIST{ FIRESTARTER_STRAIGHT, FIRESTARTER_TRACKER, FIRESTARTER_RING, FIRESTARTER_DEATH, FIRESTARTER_END };

public:
	struct Blaze_FireStarter
	{
		_float4x4 matWorld;
		FIRESTARTER_LIST eFireStarterType = FIRESTARTER_LIST::FIRESTARTER_STRAIGHT;
		_int iFireStarterCount = 10;
	};

private:
	CBlaze_FireStarter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlaze_FireStarter(const CBlaze_FireStarter& rhs);

public:
	virtual ~CBlaze_FireStarter();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();

private:
	void State_Machine(_float fTimeDelta);

	void State_Straight(_float fTimeDelta);
	void State_Death(_float fTimeDelta);

private:
	_int m_iFireStarter_Count = 0;
	_int m_iFireStarter_Count_Max = 15;

private:
	_float m_fBlazeFireTime = 0.05f;
	_float m_fBlazeFireTimeMax = 0.05f;

private:
	FIRESTARTER_LIST m_eFireStarterType = FIRESTARTER_LIST::FIRESTARTER_STRAIGHT;

public:
	static shared_ptr<CBlaze_FireStarter> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END