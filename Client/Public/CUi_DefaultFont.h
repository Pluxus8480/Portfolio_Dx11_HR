#pragma once

#include "Client_Defines.h"
#include "CUi_Default.h"


BEGIN(Client)

class CUi_DefaultFont : public CUi_Default
{
public:
	static wstring ObjID;


protected:
	CUi_DefaultFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_DefaultFont(const CUi_DefaultFont& rhs);


public:
	virtual ~CUi_DefaultFont();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;
	void Set_FontDesc(FontDescInfo Font);
	void Set_FontString(wstring FontString);


private:
	void Adjust_FontPos();


public:
	static shared_ptr<CUi_DefaultFont> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
