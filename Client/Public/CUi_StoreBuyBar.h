#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StoreBuyBar : public CUi_2D
{
public:
	static wstring ObjID;

public:
	enum class Type { NeedBuy, NeedUpGrade, Has, End};

protected:
	CUi_StoreBuyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreBuyBar(const CUi_StoreBuyBar& rhs);


public:
	virtual ~CUi_StoreBuyBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Percent(_float Percent);
	virtual void Dying() override;
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	void Set_Type(Type eType);
	virtual void Set_Active(_bool Active) override;
	virtual void Add_Pos(_float PosX, _float PosY);


private:
	void Initialize_Font();
	void Initialize_Bar();
	void Adjust_FontPos();


private:
	shared_ptr<class CUi_Default> m_pBack;
	_float m_fPercent = { 0 };
	Type m_eType = { Type::NeedBuy };


public:
	static shared_ptr<CUi_StoreBuyBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
