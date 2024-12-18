#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_RekkaSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_RekkaSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_RekkaSystem(const CUi_RekkaSystem& rhs);


public:
	virtual ~CUi_RekkaSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	void SetRekka(shared_ptr<class CMonster_Rekka> pRekka) { m_pRekka = pRekka; };

private:
	void Initialize_Hp();
	void Initialize_HpBack();
	void Initialize_Deco();


private:
	void Renew_RekkaHp();
	void RekkaDamaged(_float fTimeDelta);
	void Check_RekkaBuff();
	virtual void Dying() override;
	virtual void Set_Active(_bool Active) override;


private:
	shared_ptr<class CMonster_Rekka> m_pRekka;
	vector<shared_ptr<class CUi_RekkaHp>> m_pHpVec;
	vector<shared_ptr<class CUi_Default>> m_pHpBackVec;
	shared_ptr<class CUi_RekkaDeco> m_pDeco;
	_float2 m_fHpSize = { 200, 80 };

	_bool m_bDamaged = { false };
	_float m_fPreviousRekkaPercent = { 1 };
	_float m_fDamagedTime = { 0 };

	_bool m_bRekkaBuff = { false };


public:
	static shared_ptr<CUi_RekkaSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
