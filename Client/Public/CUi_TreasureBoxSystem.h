#pragma once

#include "Client_Defines.h"
#include "CUi_MapInterAction.h"

BEGIN(Client)

class CUi_TreasureBoxSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_TreasureBoxSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TreasureBoxSystem(const CUi_TreasureBoxSystem& rhs);


public:
	virtual ~CUi_TreasureBoxSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	void Initialize_TreasureBox();
	void Initialize_TreasureBoxBack();
	void Initialize_BackGround();
	void Initialize_Line();
	void Initialize_Arrow();
	void Pressing_Key();
	void Reduce_Percent(_float fTimeDelta);
	void Reduce_Alpha(_float fTimeDelta);
	void Adjust_LinePos();
	void Apply_Percent();
	virtual void Dying() override;
	void Adjust_Pressing(_float fTimeDelta);


private:
	shared_ptr<class CUi_TreasureBox> m_pBox;
	vector<shared_ptr<class CUi_TreasureBox>> m_pBoxBackVec;
	vector<shared_ptr<class CUi_Default>> m_pBackVec;
	shared_ptr<class CUi_Default> m_pLine;
	shared_ptr<class CUi_Default> m_pArrow;
	_float2 m_fDistance = { 0, 0 };
	_float m_fPercent = { 0 };
	_bool m_bPress = { true };
	_float m_fPressTime = { 0 };
	_float m_fArrowPosY = { 0 };


public:
	static shared_ptr<CUi_TreasureBoxSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
