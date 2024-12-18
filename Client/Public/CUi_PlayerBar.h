#pragma once

#include "Client_Defines.h"
#include "CUi_Canvas.h"

BEGIN(Client)

class CUi_PlayerBar : public CUi_Canvas
{
public:
	static wstring ObjID;

public:
	enum PARTNER { PARTNER_PEPPERMINT, PARTNER_MACARON, PARTNER_KORSICA, PARTNER_END };
	enum REVERB { REVERB_FIRST, REVERB_SECOND, REVERB_THIRD, REVERB_FOURTH, REVERB_END};
	enum REVERB_SHADOW { RESHADOW_FIRST, RESHADOW_SECOND, RESHADOW_THIRD, RESHADOW_END };
	enum REVERB_GLOW { REGLOW_FIRST, REGLOW_SECOND, REGLOW_END };

protected:
	CUi_PlayerBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PlayerBar(const CUi_PlayerBar& rhs);

public:
	virtual ~CUi_PlayerBar();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void    Tick(_float fTimeDelta);
	virtual void    Dying() override;
	virtual void    Set_Active(_bool Active) override;

public:
	void Create_PlayerGuiter();
	void Create_PlayerHP();
	void Create_PlayerSubHP();
	void Create_PlayerReverb();
	void Create_PlayerBarLock();
	void Create_PlayerPartner();

	void Move_Shadow(_float fTimeDelta);
	void Fight_Mode();
	void Basic_Mode();
	void Renew_Hp();
	void Renew_Reverb();
	void Enable_Reverb();
	void Set_HpStateTest();
	void Set_ReverbStateTest();

	void Partner_Respawn(PARTNER PartnerType);
	void Change_Partner();
	virtual void ReceiveUIEvent(UI_EVENT InUIEvent) override;

	void PartnerOff_Both();
	void PartnerOff_Korsica();
	void VisbleReverb();

protected:
	_float m_fShadowTimeFight = 0.0f;
	_float m_fShadowTimeBasic = 0.0f;
	_int   m_iNormal = 0;
	_bool  m_bReturning = false;
	_bool  m_bFight = false;
	_bool  m_bReverb = false;

	_float m_fPlayerMaxHp = { 70.f };
	_float m_fPlayerHp = { 70.f };

	_float m_fPlayerMaxReverb = { 100.f };
	_float m_fPlayerReverb = { 100.f };

	_float2 m_OriginShadowGuiterPos = {};
	_float2 m_OriginShadowHpPos = {};
	_float2 m_OriginShadowSubHpPos = {};
	vector<_float2> m_OriginShadowReverbPos = {};

	vector<PARTNER> m_vecPartnerOrder;
	vector<PARTNER> m_CurrentOrder = { PARTNER_PEPPERMINT, PARTNER_MACARON, PARTNER_KORSICA };
	vector<_float3> m_vecPartnerPosition;
	vector<_float3> m_vecPartnerPositionFightMode;
	_int    m_iCurPartnerIndex = { 0 };
	_float4 m_PartnerColor[PARTNER_END];

protected:
	shared_ptr<class CUi_Sprite> m_pGuiterLightning;
	shared_ptr<class CUi_Sprite> m_pGuiterBackground;
	shared_ptr<class CUi_Sprite> m_pGuiterShadow;

	shared_ptr<class CUi_PlayerHp> m_pHP;
	shared_ptr<class CUi_Sprite>   m_pHPBar;
	shared_ptr<class CUi_Sprite>   m_pHPShadow;

	shared_ptr<class CUi_PlayerHp> m_pSubHP;
	shared_ptr<class CUi_Sprite>   m_pSubHPBar;
	shared_ptr<class CUi_Sprite>   m_pSubHPShadow;

	vector<shared_ptr<CUi_PlayerReverb>>  m_pReverb;
	vector<shared_ptr<class CUi_Sprite>>  m_pReverbBar;
	vector<shared_ptr<class CUi_Sprite>>  m_pReverbShadow;
	vector<shared_ptr<class CUi_Sprite>>  m_pReverbGlow;
	//vector<shared_ptr<class CUi_Sprite>>       m_pReverbOutline;

	shared_ptr<class CUi_Sprite> m_pLock;

	shared_ptr<class CUi_Sprite> m_pPartnerGlow;
	shared_ptr<class CUi_Sprite> m_pPartnerBlack[PARTNER_END];
	shared_ptr<class CUi_PlayerPartnerGauge> m_pPartnerSprite[PARTNER_END];

public:
	static shared_ptr<CUi_PlayerBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
