#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Client)

class CPlatformSpawner : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CPlatformSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlatformSpawner(const CPlatformSpawner& rhs);
public:
	~CPlatformSpawner();

public:

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

};

END