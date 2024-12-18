#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"


class CBattle3rdAfter :  public CJeongGimmicBase
{
public:
	static wstring ObjID;

private:
	CBattle3rdAfter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattle3rdAfter(const CBattle3rdAfter& rhs);


public:
	~CBattle3rdAfter();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	_bool m_bRender = { false };
	_bool m_isTalkOver = { true };
	_bool m_bTalk = { false };


public:
	static shared_ptr<CBattle3rdAfter> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

