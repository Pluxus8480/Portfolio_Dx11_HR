#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_TramTimer : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_TramTimerInfo
	{
		_float fTime = { 99 };
	};

protected:
	CUi_TramTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TramTimer(const CUi_TramTimer& rhs);


public:
	virtual ~CUi_TramTimer();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Add_Time(_float Time);


private:
	virtual void Dying() override;


private:
	void Cal_Time(_float fTimeDelta);
	void Initialize_Timer();
	void Initialize_Font();


private:
	FontDescInfo StringDesc;
	_float m_fTime = { 0 };



public:
	static shared_ptr<CUi_TramTimer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
