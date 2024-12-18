#pragma once

#include "Client_Defines.h"
#include "CUi_Canvas.h"

BEGIN(Client)

class CUi_DashEffect : public CUi_Canvas
{
public:
	static wstring ObjID;

protected:
	CUi_DashEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_DashEffect(const CUi_DashEffect& rhs);

public:
	virtual ~CUi_DashEffect();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void    Tick(_float fTimeDelta);
	virtual void    Dying() override;
	virtual void    Set_Active(_bool Active) override;

public:
	void Create_DashEffect();

protected:
	_float m_fElapsedTime = { 0.f };
	_float m_fIntervalTime = { 0.1f };
	_int   m_iCurrentTextureIndex = { 0 };

protected:
	shared_ptr<class CUi_Sprite> m_pDashEffect;

public:
	static shared_ptr<CUi_DashEffect> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
