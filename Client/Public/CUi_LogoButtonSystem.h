#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_LogoButtonSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_LogoButtonSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_LogoButtonSystem(const CUi_LogoButtonSystem& rhs);


public:
	virtual ~CUi_LogoButtonSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	LEVEL Get_LevelIndex() const { return (LEVEL)m_iCurIndex; }


private:
	void Initialize_Font();


private:
	FontDescInfo m_LevelFontDesc[LEVEL_END];
	_int m_iCurIndex = { 0 };


public:
	static shared_ptr<CUi_LogoButtonSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
