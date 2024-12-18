#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_Chip : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_ChipInfo : public CUi_2DInfo
	{
		_uint iPrice = { 0 };
		wstring ChipName = {};
		wstring TextureName = {};
	};



protected:
	CUi_Chip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_Chip(const CUi_Chip& rhs);


public:
	virtual ~CUi_Chip();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


public:
	_uint Get_Price() const { return m_iPrice; }
	void Set_Focus(_bool Focus) { m_bFocus = Focus; }
	wstring Get_Name() const { return m_ChipName; }


private:
	void Check_UpGrade();
	

private:
	_bool m_bFocus = { false };
	wstring m_ChipName = L"";
	_uint m_iPrice = { 0 };
	_uint m_iOriginPrice = { 0 };


public:
	static shared_ptr<CUi_Chip> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
