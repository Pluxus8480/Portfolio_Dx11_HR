#pragma once

#include "Client_Defines.h"
#include "CUi_Default.h"


BEGIN(Client)

class CUi_DefaultBar : public CUi_Default
{
public:
	static wstring ObjID;


protected:
	CUi_DefaultBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_DefaultBar(const CUi_DefaultBar& rhs);


public:
	virtual ~CUi_DefaultBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Percent(_float Percent);
	_float Get_Percent() const { return m_fPercent; }


private:
	_float m_fPercent = { 0 };


public:
	static shared_ptr<CUi_DefaultBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
