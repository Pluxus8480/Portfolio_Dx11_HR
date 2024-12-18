#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_KaleSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_KaleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_KaleSystem(const CUi_KaleSystem& rhs);


public:
	virtual ~CUi_KaleSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


public:
	void Set_KaleHp(_float* KaleHp) { m_pKaleHp = KaleHp; }
	void Set_KaleMaxHp(_float* KaleMaxHp) { m_pKaleMaxHp = KaleMaxHp; }
	void Set_KaleGroggy(_int* Groggy);
	virtual void Set_Active(_bool Active) override;
	void Set_Phase3();

private:
	void Initialize_HpBar();
	void Initialize_Name();
	void Initialize_GroggyGage();


private:
	void Move_Shadow(_float fTimeDelta);
	void Renew_KaleHp(_float fTimeDelta);
	void Renew_KaleGroggyGauge(_float fTimeDelta);

	virtual void Dying() override;


private:
	shared_ptr<class CUi_KaleHpBar> m_pHpBar;
	shared_ptr<class CUi_Default> m_pHpBackBar;
	shared_ptr<class CUi_KaleGroggyGage> m_pGroggyGaugeBar;
	shared_ptr<class CUi_Default> m_pGroggyGaugeBackBar;
	shared_ptr<class CUi_Default> m_pHpShadow;
	shared_ptr<class CUi_Default> m_pName;
	_float2 m_OriginPos = { 0, -270 };
	_float2 m_OriginShadowPos = { 0, 0 };
	_float m_fPercent = { 1 };

	_float m_fPreviousKalePercent = { 1 };
	_float m_fDamagedTime = { 0 };

	_int* m_pGroggyGage = { nullptr };
	_int m_pMaxGroggyGage = { 0 };
	_float* m_pKaleHp = { nullptr };
	_float* m_pKaleMaxHp = { nullptr };
	_float m_fMaxHp = { 0 };
	_float m_fHp = { 10 };
	_bool m_bGroggyActive = { false };


public:
	static shared_ptr<CUi_KaleSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
