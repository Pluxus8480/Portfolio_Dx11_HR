#pragma once

#include "Client_Defines.h"
#include "CUi_Sprite.h"

BEGIN(Client)

class CUi_PlayerPartnerGauge : public CUi_Sprite
{
public:
	static wstring ObjID;

	using Desc = struct CUi_PlayerPartnerGaugeInfo : public CUi_SpriteInfo
	{
		_int iReadyTextureNum;
		_int iPartnerIndex;
	};

protected:
	CUi_PlayerPartnerGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PlayerPartnerGauge(const CUi_PlayerPartnerGauge& rhs);

public:
	virtual ~CUi_PlayerPartnerGauge();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void    Dying() override;
	virtual void    Set_Active(_bool Active) override;

public:
	_bool IsRespawnComplete() const { return m_fProgress >= 1.0f; }
	void  Partner_Respawn(_float fTimeDelta);

	_bool IsRespawning() const { return m_bRespawn; }
	void  SetRespawn(bool bRespawn) { m_bRespawn = bRespawn; }

	_float GetProgress() const { return m_fProgress; }
	void   SetProgress(_float fProgress) { m_fProgress = fProgress; }

	void OnPartnerChange(int Index, _float3 InPosition, _float3 InScale, float InTime);

protected:
	_float m_bRespawn = false;
	_float m_fProgress = 1.f;
	_float m_fRespawnTime = 0.0f;
	_float m_InitCoolTime = 0.0f;
	_int   m_iPartnerIndex = 0 ;

	std::function<void()> m_RespawnCompleteCallback = nullptr;

protected:
	shared_ptr<class CUi_Sprite> m_pPartnerCir;
	shared_ptr<class CUi_Sprite> m_pPartnerExp;
	shared_ptr<class CUi_Sprite> m_pPartnerReady;

public:
	static shared_ptr<CUi_PlayerPartnerGauge> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
