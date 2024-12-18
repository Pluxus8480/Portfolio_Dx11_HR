#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_FightString : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_FightString(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_FightString(const CUi_2D& rhs);


public:
	virtual ~CUi_FightString();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	_float m_fLifeTime = { 1.5f };


public:
	static shared_ptr<CUi_FightString> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
