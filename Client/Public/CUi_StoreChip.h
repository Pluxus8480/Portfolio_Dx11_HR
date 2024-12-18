#pragma once

#include "Client_Defines.h"
#include "CUi_Store.h"
#include "CUi_StoreChipDesc.h"


BEGIN(Client)

class CUi_StoreChip : public CUi_Store
{
public:
	static wstring ObjID;


protected:
	CUi_StoreChip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreChip(const CUi_StoreChip& rhs);


public:
	virtual ~CUi_StoreChip();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


public:
	virtual void Initialize_StoreDesc() override;
	virtual void Dying() override;
	virtual void Adjust_Alpha(_float fTimeDelta) override;
	virtual void Check_Has() override;
	virtual void Set_Active(_bool Active) override;


protected:
	virtual void Progress_Store(_float fTimeDelta) override;
	virtual void Select_Bar() override;


private:
	void Initialize_Back();
	void Initialize_Chip();
	void Initialize_ChipMap();
	void Initialize_ChipVecName();


private:
	vector<shared_ptr<class CUi_Chip>> m_ChipVec;
	map<wstring, CUi_StoreChipDesc::ChipDesc> m_ChipMap;
	shared_ptr<CUi_StoreChipDesc> m_StoreChipDesc;
	vector<wstring> m_ChipNameVec;


public:
	static shared_ptr<CUi_StoreChip> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
