#pragma once

#include "Client_Defines.h"
#include "CUi_Sprite.h"

BEGIN(Client)

class CUi_PlayerReverb : public CUi_Sprite
{
public:
	static wstring ObjID;

protected:
	CUi_PlayerReverb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PlayerReverb(const CUi_PlayerReverb& rhs);

public:
	virtual ~CUi_PlayerReverb();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_PercentReverb(_float fPercent);
	void Cal_DecreaseTimeReverb(_float fTimeDelta);

protected:
	_bool  m_bPlayerReverb = { false };
	_bool  m_bPlayerReverbDecreased = { false };
	_float m_fPercentReverb = { 1 };
	_float m_fDecTimeReverb = { 0 };

public:
	static shared_ptr<CUi_PlayerReverb> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
