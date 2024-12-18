#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Client)

class CSwitchFence : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CSwitchFence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwitchFence(const CSwitchFence& rhs);
public:
	~CSwitchFence();

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
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSwitchFence> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END