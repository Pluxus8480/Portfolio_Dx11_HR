#pragma once

#include "Client_Defines.h"
#include "CUi_StoreBar.h"

BEGIN(Client)

class CUi_StoreEquipBar : public CUi_StoreBar
{
public:
	static wstring ObjID;


protected:
	CUi_StoreEquipBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreEquipBar(const CUi_StoreEquipBar& rhs);


public:
	virtual ~CUi_StoreEquipBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual void Set_Active(_bool Active) override;


public:
	static shared_ptr<CUi_StoreEquipBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
