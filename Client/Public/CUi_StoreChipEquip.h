#pragma once

#include "Client_Defines.h"
#include "CUi_Store.h"

BEGIN(Client)

class CUi_StoreChipEquip : public CUi_Store
{
public:
	static wstring ObjID;


protected:
	CUi_StoreChipEquip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreChipEquip(const CUi_StoreChipEquip& rhs);


public:
	virtual ~CUi_StoreChipEquip();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	static shared_ptr<CUi_StoreChipEquip> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END