#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_RekkaDeco : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_RekkaDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_RekkaDeco(const CUi_RekkaDeco& rhs);


public:
	virtual ~CUi_RekkaDeco();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Buff(bool bBuff) { m_bBuff = bBuff; }


private:
	void Initialize_Effect();
	void Adjust_Effect(_float fTimeDelta);


private:
	_bool m_bBuff = { false };
	_float m_fEffectTime = { 0 };
	shared_ptr<class CUi_RekkaDecoEffect> m_pEffect;


public:
	static shared_ptr<CUi_RekkaDeco> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
