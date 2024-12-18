#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Client)

class CSpectraMeshGate :
    public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CSpectraMeshGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpectraMeshGate(const CSpectraMeshGate& rhs);
public:
	~CSpectraMeshGate();

public:
	virtual void Execute(_bool bExecute = true) override;
	virtual void Set_Dead(bool bDead = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CGameObject> m_pPartDoor = { nullptr };

private:
	HRESULT Ready_Parts();

public:
	static shared_ptr<CSpectraMeshGate> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END