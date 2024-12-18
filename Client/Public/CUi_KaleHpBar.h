#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_KaleHpBar : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_KaleHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_KaleHpBar(const CUi_KaleHpBar& rhs);


public:
	virtual ~CUi_KaleHpBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Percent(_float Percent) { m_fPercent = Percent; }
	void Set_Damaged(_bool Damaged);


private:
	_float m_fPercent = { 0 };
	_bool m_bDamaged = { false };
	_float m_fDamagedTime = { 0 };


public:
	static shared_ptr<CUi_KaleHpBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
