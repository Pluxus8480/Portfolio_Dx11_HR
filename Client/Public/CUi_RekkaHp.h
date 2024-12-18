#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_RekkaHp : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_RekkaHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_RekkaHp(const CUi_RekkaHp& rhs);


public:
	virtual ~CUi_RekkaHp();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_DeletePercent(_float Percent) { m_fDeletePercent = Percent; }
	void Set_Damaged(_bool bDamaged) { m_bDamaged = bDamaged; }


private:
	_float m_fDeletePercent = { 0 };
	_bool m_bDamaged = { false };


public:
	static shared_ptr<CUi_RekkaHp> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
