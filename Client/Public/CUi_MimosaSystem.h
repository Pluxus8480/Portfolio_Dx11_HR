#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_MimosaSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_MimosaSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_MimosaSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_MimosaSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	void Set_Hp(_float* Hp);
	void Set_Groggy(_float* Groggy);


private:
	void Renew_Hp(_float fTimeDelta);
	void Renew_Groggy();
	virtual void Dying() override;
	virtual void Set_Active(_bool Active) override;
	void Initialize_Deco();
	void Initialize_HpBar();
	void Initialize_Groggy();


private:
	_float* m_pCurHp = { nullptr };
	_float m_fMaxHp = { 100 };
	_float* m_pGroggy = { nullptr };
	_float m_fMaxGroggy = { 40 };
	_float m_fPreviousPercent = { 1 };
	_float m_fDmgTime = { 0 };


private:
	vector<shared_ptr<class CUi_Default>> m_HealthBarBackVec;
	vector<shared_ptr<class CUi_MimosaBar>> m_HealthBarVec;
	vector<shared_ptr<class CUi_Default>> m_DecoVec;
	vector<shared_ptr<class CUi_MimosaBar>> m_GroggyGaugeVec;


public:
	static shared_ptr<CUi_MimosaSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
