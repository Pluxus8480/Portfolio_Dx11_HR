#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StoreEdge : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_StoreEdge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreEdge(const CUi_2D& rhs);


public:
	virtual ~CUi_StoreEdge();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	virtual void Add_Pos(_float PosX, _float PosY) override;
	virtual void Set_Active(_bool Active) override;
	virtual void Dying() override;



private:
	void Initialize_Part();


private:
	vector<shared_ptr<class CUi_Default>> m_pPartVec;


public:
	static shared_ptr<CUi_StoreEdge> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
