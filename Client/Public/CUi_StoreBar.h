#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "PlayerManager.h"

BEGIN(Client)

class CUi_StoreBar : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_StoreBarInfo : public CUi_2DInfo
	{
		wstring StringFont;
		_uint Price = {};
		_float fUpLimitPos = { 0 };
		_float fDownLimitPos = { 0 };
		CPlayer_Manager::ThingType type = { CPlayer_Manager::ThingType::End };
	};

protected:
	CUi_StoreBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreBar(const CUi_StoreBar& rhs);


public:
	virtual ~CUi_StoreBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	wstring Get_Name() const { return m_StringFont.String; }
	CPlayer_Manager::ThingType Get_Type() const { return m_eThingType; }
	_uint Get_Price() const { return m_iPrice; }
	void Set_Focus(_bool Focus) { m_bFocus = Focus; }
	virtual void Set_Active(_bool Active) override;
	virtual void Dying() override;
	void Set_StringFont(wstring str);
	void Set_PriceFont(_uint Price);
	void Adjust_Alpha(_float fTimeDelta);
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	_float Get_UpPercent() const { return m_fUpLimitPercent; }
	_float Get_DownPercent() const { return m_fDownLimitPercent; }
	virtual void Add_Pos(_float PosX, _float PosY) override;


protected:
	void Initialize_DefaultVec();
	void Adjust_Pos(_float fTimeDelta);
	void Check_Scroll();


protected:
	FontDescInfo m_StringFont;
	_float2 m_fStringFont_Distance = {};
	_bool m_bFocus = { false };
	_float2 m_fOriginPos = { 0, 0 };
	vector<shared_ptr<class CUi_Default>> m_pDefaultVec;
	_float m_fAlphaTime = { 0 };
	_float m_fUpLimitPercent = { 0 };
	_float m_fDownLimitPercent = { 0 };
	_float m_fUpLimitPos = { 0 };
	_float m_fDownLimitPos = { 0 };

private:
	CPlayer_Manager::ThingType m_eThingType = {};
	_float2 m_fPriceFont_Distance = {};
	_uint m_iPrice = { 0 };



public:
	static shared_ptr<CUi_StoreBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
