#pragma once

#include "Client_Defines.h"
#include "CUi_StoreDesc.h"

BEGIN(Client)

class CUi_StoreChipDesc : public CUi_StoreDesc
{
public:
	static wstring ObjID;


public:
	using ChipDesc = struct ChipExplainInfo
	{
		wstring TitleFont = L"";
		wstring PriceFont = L"";
		wstring ExplainFont = L"";
		wstring ChipPrice = L"";
		_uint UpGrade_Gap = { 0 };
		_uint iEnterCount = { 0 };
	};

protected:
	CUi_StoreChipDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreChipDesc(const CUi_StoreChipDesc& rhs);


public:
	virtual ~CUi_StoreChipDesc();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Desc(ChipDesc ChipInfo);
	virtual void Add_Color(ColorType Type, _float Color) override;


protected:
	virtual void Initialize_Font() override;
	virtual void Initialize_DefaultVec() override;


private:
	vector<FontDescInfo> m_UpGrade_Font;
	FontDescInfo m_ChipPrice_Font;
	_uint m_UpGradeGap = { 0 };


public:
	static shared_ptr<CUi_StoreChipDesc> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
