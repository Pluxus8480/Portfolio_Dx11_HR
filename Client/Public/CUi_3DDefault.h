#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_3DDefault : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_3DDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_3DDefault(const CUi_2D& rhs);


public:
	virtual ~CUi_3DDefault();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Initialize_StunGaugeResource();


public:
	static shared_ptr<CUi_3DDefault> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
