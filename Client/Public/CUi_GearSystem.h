#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_GearSystem : public CUi_2D
{
public:
	static wstring ObjID;

private:
	enum State { APPEAR, ADDSIGN, CAL, ADDFONTDISAPPEAR, DISAPPEAR, End };

protected:
	CUi_GearSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_GearSystem(const CUi_GearSystem& rhs);


public:
	virtual ~CUi_GearSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Initialize_GearAddBack();
	void Initialize_Font();


private:
	void Progress_State(_float fTimeDelta);
	void Check_ChangeGear();
	void Appear(_float fTimeDelta);
	void AddSign(_float fTimeDelta);
	void Cal(_float fTimeDelta);
	void AddFontDisappear(_float fTimeDelta);
	void Disappear(_float fTimeDelta);
	void Adjust_FontPos();
	void Set_State(State eState) { m_eState = eState; }
	void Adjust_BackPos();

	void AddSign_UpDown(_float fTimeDelta);
	virtual void Dying() override;


private:
	shared_ptr<class CUi_Default> m_pAddBack;
	FontDescInfo m_AddFont;
	_bool m_bChanged = { false };
	_uint m_PreviousGear = { 0 };
	_int m_iGetGearCount = { 0 };
	_bool m_bUpDown = { false };
	_float m_fUpDownTime = { 0 };
	State m_eState = { State::DISAPPEAR };
	_float m_fTime[State::End] = { 0, };
	_bool m_bRenderFont = { false };


public:
	static shared_ptr<CUi_GearSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
