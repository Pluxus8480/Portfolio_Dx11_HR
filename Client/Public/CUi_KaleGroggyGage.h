#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_KaleGroggyGage : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_KaleGroggyGage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_KaleGroggyGage(const CUi_KaleGroggyGage& rhs);


public:
	virtual ~CUi_KaleGroggyGage();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Percent(_float GroggyGaugePercent) { m_fPercent = GroggyGaugePercent; }
	void Set_OwnActive(_bool Active) { m_bOwnActive = Active; }


private:
	_float m_fPercent = { 0 };
	_bool m_bOwnActive = { true };


public:
	static shared_ptr<CUi_KaleGroggyGage> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
