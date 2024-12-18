#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_KorsicaHpBar : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_KorsicaHpBarInfo : public CUi_2DInfo
	{
		_bool bKorsica = false;
	};


protected:
	CUi_KorsicaHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_KorsicaHpBar(const CUi_KorsicaHpBar& rhs);


public:
	virtual ~CUi_KorsicaHpBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_HpPercent(_float fPercent);


private:
	void Cal_DamagedTime(_float fTimeDelta);


private:
	_bool m_bKorsica = { false };
	_bool m_bKorsicaDamaged = { false };
	_float m_fPercent = { 1 };
	_float m_fDmgTime = { 0 };


public:
	static shared_ptr<CUi_KorsicaHpBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
