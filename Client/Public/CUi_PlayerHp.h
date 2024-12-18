#pragma once

#include "Client_Defines.h"
#include "CUi_Sprite.h"

BEGIN(Client)

class CUi_PlayerHp : public CUi_Sprite
{
public:
	static wstring ObjID;

protected:
	CUi_PlayerHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PlayerHp(const CUi_PlayerHp& rhs);

public:
	virtual ~CUi_PlayerHp();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _uint   Get_TextureNum() const;

public:
	void Set_PercentHp(_float fPercent);
	void Cal_DecreaseTimeHp(_float fTimeDelta);
	_float GetHpPercent();

protected:
	_bool  m_bPlayerHp = { false };
	_bool  m_bPlayerHpDecreased = { false };
	_float m_fPercentHp = { 1 };
	_float m_fDecTimeHp = { 0 };

public:
	static shared_ptr<CUi_PlayerHp> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
