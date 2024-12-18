#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"


class CBattle3rdFloor :  public CJeongGimmicBase
{
public:
	static wstring ObjID;

private:
	CBattle3rdFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattle3rdFloor(const CBattle3rdFloor& rhs);


public:
	~CBattle3rdFloor();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	_bool m_bRender = { true };


public:
	static shared_ptr<CBattle3rdFloor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

