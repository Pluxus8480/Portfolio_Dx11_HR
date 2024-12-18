#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "CUi_StoreBuyBar.h"


BEGIN(Client)

class CUi_StoreDesc : public CUi_2D
{
private:
	enum ItemOrder { None, LifeGauge, Reverb, HpTank, Extension, End };
public:
	static wstring ObjID;


public:
	using AttackDesc = struct AttackExplainInfo
	{
		wstring TitleFont = L"";
		wstring PriceFont = L"";
		wstring ExplainFont = L"";
		_uint iEnterCount = { 0 };
	};

	using ItemDesc = struct ItemExplainInfo
	{
		wstring TitleFont = L"";
		wstring PriceFont = L"";
		wstring ExplainFont = L"";
		_uint iEnterCount = { 0 };
	};

	using SkillDesc = struct SkillExplainInfo
	{
		wstring TitleFont = L"";
		wstring PriceFont = L"";
		wstring ExplainFont = L"";
		_uint iEnterCount = { 0 };
	};

protected:
	CUi_StoreDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreDesc(const CUi_StoreDesc& rhs);


public:
	virtual ~CUi_StoreDesc();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Desc(AttackDesc AttackInfo);
	void Set_Desc(SkillDesc SkillInfo);
	void Set_Desc(ItemDesc ItemInfo);
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	void Adjust_Alpha(_float fTimeDelta);
	virtual void Set_Active(_bool Active) override;
	virtual void Add_Pos(_float PosX, _float PosY);


public:
	void Set_BuyBarPercent(_float fPercent);
	void Set_BuyBarType(CUi_StoreBuyBar::Type Type);


protected:
	virtual void Initialize_Font();
	virtual void Initialize_DefaultVec();
	virtual void Initialize_BuyBar();
	virtual void Dying() override;


private:
	void Initialize_Png();
	void Sync_Png();


protected:
	vector<shared_ptr<class CUi_Default>> m_DefaultVec;
	shared_ptr<class CUi_Default> m_Itempng;
	FontDescInfo m_TitleFont;
	FontDescInfo m_ExplainFont;
	FontDescInfo m_PriceFont;
	_uint m_iExplainFontEnterCount = { 0 };
	_float m_fAlphaTime = { 0 };
	shared_ptr<class CUi_StoreBuyBar> m_pBuyBar;


public:
	static shared_ptr<CUi_StoreDesc> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
