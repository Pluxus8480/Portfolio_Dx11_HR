#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_SuccessSystem : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_SuccessSystemInfo
	{
		_float2 CenterPos = {};
		_bool bEffectMode = { false };
	};


protected:
	CUi_SuccessSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_SuccessSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_SuccessSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual void Dying() override;


private:
	void Initialize_LineVec();
	void Initialize_Circle();
	void Initialize_Light();


private:
	_float m_fLifeTime = { 0.25f };
	_float2 m_fCenterPos = {};
	_bool m_bEffectMode = { false };
	vector<shared_ptr<class CUi_Default>> m_pLineVec;
	shared_ptr<class CUi_Default> m_pLight;
	shared_ptr<class CUi_Default> m_pGreenCircle;
	shared_ptr<class CUi_Default> m_pBlueCircle;
	

public:
	static shared_ptr<CUi_SuccessSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
