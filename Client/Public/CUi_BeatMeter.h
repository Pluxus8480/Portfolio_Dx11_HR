#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_BeatMeter : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_BeatMeter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_BeatMeter(const CUi_2D& rhs);


public:
	virtual ~CUi_BeatMeter();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Percent(_float Percent) { m_fPercent = Percent; }


private:
	_float m_fPercent = { 0 };


public:
	static shared_ptr<CUi_BeatMeter> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
