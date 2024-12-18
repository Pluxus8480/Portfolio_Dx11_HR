#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_MImosaMusicBattleSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_MImosaMusicBattleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_MImosaMusicBattleSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_MImosaMusicBattleSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	void Set_SuccessPercent(_float Percent);
	void Initialize_Back();
	void Initialize_Bar();


public:
	virtual void Dying() override;
	virtual void Set_Active(_bool Active) override;
	void Set_Turn(_bool ChaiTurn);


private:
	void Renew_Percent();
	void Turn_Check();


private:
	_float m_fChaiPercent = { 0.5f };
	vector<shared_ptr<class CUi_Default>> m_DecoVec;
	vector<shared_ptr<class CUi_DefaultBar>> m_BarVec;
	shared_ptr<class CUi_SpecialComboAttackSystem> m_pSpecialCombo;
	_bool m_bChaiTurn = { false };


public:
	static shared_ptr<CUi_MImosaMusicBattleSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
