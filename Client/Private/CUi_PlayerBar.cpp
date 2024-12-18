#include "CUi_PlayerBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"
#include "iostream"
#include "PlayerManager.h"
#include "CUI_Move.h"
#include "CUI_Scale.h"
#include "CUI_Frame.h"
#include "CUI_CallFunction.h"
#include "CUI_Sequence.h "

wstring CUi_PlayerBar::ObjID = L"CUi_PlayerBar";

CUi_PlayerBar::CUi_PlayerBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Canvas(pDevice, pContext)
{
}

CUi_PlayerBar::CUi_PlayerBar(const CUi_PlayerBar& rhs)
	:CUi_Canvas(rhs)
{
}

CUi_PlayerBar::~CUi_PlayerBar()
{
}

HRESULT CUi_PlayerBar::Initialize_Prototype()
{
#pragma region Guiter

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerGuiterLightning", L"../Bin/Resources/Textures/Ui/PlayerBar/Guiter/T_HUD_LifeGauge_GuiterBack_Color_%d.png", 2)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerGuiterBackground", L"../Bin/Resources/Textures/Ui/PlayerBar/Guiter/T_HUD_PlayerGauge_GuiterBackB_%d.png", 2)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerGuiterShadow", L"../Bin/Resources/Textures/Ui/PlayerBar/Guiter/T_HUD_GuiterBody_DropShadow.png")
	);

#pragma endregion Guiter

#pragma region HP

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerHPGauge", L"../Bin/Resources/Textures/Ui/PlayerBar/HP/T_HealthBar_Bar_%d.png", 20)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerHPBar", L"../Bin/Resources/Textures/Ui/PlayerBar/HP/T_HealthBar_Bg_%d.png", 10)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerHPBarShadow", L"../Bin/Resources/Textures/Ui/PlayerBar/HP/T_HealthBar_DropShadow_%d.png", 10)
	);

#pragma endregion HP

#pragma region Reverb

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbGauge", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_PlayerGauge_ReverbBar_A_%d.png", 4)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbBar", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_PlayerGauge_EmptyReverb_A_%d.png", 4)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbOutline", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_PlayerGauge_ReverbOutline_B_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbShadow", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_ReverbBar_DropShadow_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerReverbGlow", L"../Bin/Resources/Textures/Ui/PlayerBar/Reverb/T_HUD_PlayerGauge_ReverbBar_Glow_%d.png", 2)
	);

#pragma endregion Reverb

#pragma region Lock

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PlayerBarLock", L"../Bin/Resources/Textures/Ui/PlayerBar/Lock/T_HUD_PlayerGauge_Lock.png")
	);

#pragma endregion Lock

#pragma region Partner

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerExplosionCircle", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_explosion_cir.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerExplosion", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_explosion.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerReady", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_Word_PartnerGauge_Ready_%d.png", 3)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerGlow", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_glow.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerPeppermintBlack", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_new_Peppermint_0_BW.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerMacaronBlack", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_new_Macaron_0_BW.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerKorsicaBlack", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_new_Korsica_0_BW.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerPeppermint", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_new_Peppermint_%d.png",9)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerMacaron", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_new_Macaron_%d.png", 9)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_PartnerKorsica", L"../Bin/Resources/Textures/Ui/PlayerBar/Partner/T_gauge_partner_new_Korsica_%d.png", 9)
	);

#pragma endregion Partner

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("MakeReverb"), TEXT("../Bin/Resources/Models/Map/Gimmick/Sound/MakeReverb.ogg")));

	return S_OK;
}

HRESULT CUi_PlayerBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	Create_PlayerGuiter();
	Create_PlayerHP();
	Create_PlayerSubHP();
	Create_PlayerReverb();
	Create_PlayerBarLock();
	Create_PlayerPartner();

	Renew_Hp();
	Renew_Reverb();

	/*Set_HpStateTest();
	Set_ReverbStateTest();*/

	m_pLock->SetVisible(false);
	m_pGuiterLightning->SetVisible(false);

	for (auto& iter : m_pReverb)
	{
		iter->SetVisible(false);
	}
	for (auto& iter : m_pReverbBar)
	{
		iter->SetVisible(false);
	}
	for (auto& iter : m_pReverbShadow)
	{
		iter->SetVisible(false);
	}
	for (auto& iter : m_pReverbGlow)
	{
		iter->SetVisible(false);
	}

	return S_OK;
}

void CUi_PlayerBar::Tick(_float fTimeDelta)
{
	/*Renew_Hp();
	Renew_Reverb();*/

	if (m_bFight)
	{
		m_fShadowTimeFight += fTimeDelta;
		if (m_fShadowTimeFight >= 0.4f)
		{
			Move_Shadow(fTimeDelta);
			m_fShadowTimeFight = 0.4f;
			m_fShadowTimeBasic = 0.f;
		}
	}
	else
	{
		m_fShadowTimeBasic += fTimeDelta;
		if (m_fShadowTimeBasic >= 0.4f)
		{
			Move_Shadow(fTimeDelta);
			m_fShadowTimeBasic = 0.4f;
			m_fShadowTimeFight = 0.f;
		}
	}

	auto pGameInstance = m_pGameInstance.lock();
	/*if (pGameInstance->Get_KeyDown(DIK_F))
	{
		Change_Partner();
		PLAYERMANAGER->Change_Assist();
	}*/
	
	/*if (!m_bFight)
	{
		if (pGameInstance->Get_KeyDown(DIK_N))
		{
			Fight_Mode();
		}
	}
	else if (m_bFight)
	{
		if (pGameInstance->Get_KeyDown(DIK_M))
		{
			Basic_Mode();
		}
	}*/
	
	//Test

	/*if (pGameInstance->Get_KeyDown(DIK_9))
	{
		m_fPlayerMaxHp = max(70.f, m_fPlayerMaxHp - 10);
		Set_HpStateTest();

		m_fPlayerMaxReverb = max(100.f, m_fPlayerMaxReverb - 50);
		Set_ReverbStateTest();
	}
	else if (pGameInstance->Get_KeyDown(DIK_0))
	{
		m_fPlayerMaxHp = min(200.f, m_fPlayerMaxHp +10);
		Set_HpStateTest();

		m_fPlayerMaxReverb = min(200.f, m_fPlayerMaxReverb + 50);
		Set_ReverbStateTest();
	}

	if (GAMEINSTANCE->Get_KeyPressing(DIK_7))
	{
		m_fPlayerHp = max(0.0f, m_fPlayerHp - 1);
		cout << m_fPlayerHp << endl;

		m_fPlayerReverb = max(0.0f, m_fPlayerReverb - 1);
		cout << m_fPlayerReverb << endl;
	}
	else if (GAMEINSTANCE->Get_KeyPressing(DIK_8))
	{
		m_fPlayerHp = min(m_fPlayerMaxHp, m_fPlayerHp + 1);
		cout << m_fPlayerHp << endl;

		m_fPlayerReverb = min(m_fPlayerMaxReverb, m_fPlayerReverb + 1);
		cout << m_fPlayerReverb << endl;
	}
	Set_HpStateTest();
	Set_ReverbStateTest();*/
}

void CUi_PlayerBar::Dying()
{
	for (auto& iter : m_pReverb)
	{
		iter->Dead();
	}
	for (auto& iter : m_pReverbBar)
	{
		iter->Dead();
	}
	for (auto& iter : m_pReverbShadow)
	{
		iter->Dead();
	}
	for (auto& iter : m_pReverbGlow)
	{
		iter->Dead();
	}
	/*for (auto& iter : m_pReverbOutline)
	{
		iter->Dead();
	}*/

	m_pGuiterLightning->Dead();
	m_pGuiterBackground->Dead();
	m_pGuiterShadow->Dead();

	m_pHP->Dead();
	m_pHPBar->Dead();
	m_pHPShadow->Dead();

	m_pSubHP->Dead();
	m_pSubHPBar->Dead();
	m_pSubHPShadow->Dead();

	m_pLock->Dead();

	m_pPartnerGlow->Dead();

	for (size_t i = 0; i < PARTNER_END; ++i)
	{
		m_pPartnerBlack[i]->Dead();
		m_pPartnerSprite[i]->Dead();
	}

}

void CUi_PlayerBar::Set_Active(_bool Active)
{
	m_bActive = Active;

	for (auto& iter : m_pReverb)
	{
		iter->Set_Active(Active);
	}
	for (auto& iter : m_pReverbBar)
	{
		iter->Set_Active(Active);
	}
	for (auto& iter : m_pReverbShadow)
	{
		iter->Set_Active(Active);
	}
	for (auto& iter : m_pReverbGlow)
	{
		iter->Set_Active(Active);
	}
	/*for (auto& iter : m_pReverbOutline)
	{
		iter->Set_Active(Active);
	}*/

	m_pGuiterLightning->Set_Active(Active);
	m_pGuiterBackground->Set_Active(Active);
	m_pGuiterShadow->Set_Active(Active);

	m_pHP->Set_Active(Active);
	m_pHPBar->Set_Active(Active);
	m_pHPShadow->Set_Active(Active);

	m_pSubHP->Set_Active(Active);
	m_pSubHPBar->Set_Active(Active);
	m_pSubHPShadow->Set_Active(Active);

	m_pLock->Set_Active(Active);

	m_pPartnerGlow->Set_Active(Active);

	for (size_t i = 0; i < PARTNER_END; ++i)
	{
		m_pPartnerBlack[i]->Set_Active(Active);
		m_pPartnerSprite[i]->Set_Active(Active);
	}

}

void CUi_PlayerBar::Create_PlayerGuiter()
{
	CUi_Sprite::Desc GuiterLightningDesc{};
	GuiterLightningDesc.fX = -550.f;
	GuiterLightningDesc.fY = 300.f;
	GuiterLightningDesc.eRenderType = RenderType::Normal;
	GuiterLightningDesc.iPriority = m_iNormal + 4;
	GuiterLightningDesc.fSizeX = 125.f;
	GuiterLightningDesc.fSizeY = 125.f;
	GuiterLightningDesc.fSizeZ = 1.f;
	GuiterLightningDesc.fSpeed = 0.f;
	GuiterLightningDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	GuiterLightningDesc.TextureFileName = L"Prototype_Component_Texture_PlayerGuiterLightning";
	GuiterLightningDesc.TextureLevel = LEVEL_STATIC;
	GuiterLightningDesc.iTextureNum = 0;
	GuiterLightningDesc.ShaderPass = ShaderPass::Blend;
	m_pGuiterLightning = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &GuiterLightningDesc));
	m_pGuiterLightning->Set_TestName(L"Guitar");

	CUi_Sprite::Desc GuiterBackDesc{};
	GuiterBackDesc.fX = -550.f;
	GuiterBackDesc.fY = 300.f;
	GuiterBackDesc.eRenderType = RenderType::Normal;
	GuiterBackDesc.iPriority = m_iNormal + 3;
	GuiterBackDesc.fSizeX = 125.f;
	GuiterBackDesc.fSizeY = 125.f;
	GuiterBackDesc.fSizeZ = 1.f;
	GuiterBackDesc.fSpeed = 0.f;
	GuiterBackDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	GuiterBackDesc.TextureFileName = L"Prototype_Component_Texture_PlayerGuiterBackground";
	GuiterBackDesc.TextureLevel = LEVEL_STATIC;
	GuiterBackDesc.iTextureNum = 0;
	GuiterBackDesc.ShaderPass = ShaderPass::Default;
	m_pGuiterBackground = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &GuiterBackDesc));

	CUi_Sprite::Desc GuiterShadowDesc{};
	GuiterShadowDesc.fX = -550.f;
	GuiterShadowDesc.fY = 295.f;
	m_OriginShadowGuiterPos.x = GuiterShadowDesc.fX;
	m_OriginShadowGuiterPos.y = GuiterShadowDesc.fY;
	GuiterShadowDesc.eRenderType = RenderType::Normal;
	GuiterShadowDesc.iPriority = m_iNormal + 2;
	GuiterShadowDesc.fSizeX = 125.f;
	GuiterShadowDesc.fSizeY = 125.f;
	GuiterShadowDesc.fSizeZ = 1.f;
	GuiterShadowDesc.fSpeed = 0.f;
	GuiterShadowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	GuiterShadowDesc.TextureFileName = L"Prototype_Component_Texture_PlayerGuiterShadow";
	GuiterShadowDesc.TextureLevel = LEVEL_STATIC;
	GuiterShadowDesc.iTextureNum = 0;
	GuiterShadowDesc.ShaderPass = ShaderPass::Default;
	m_pGuiterShadow = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &GuiterShadowDesc));
}

void CUi_PlayerBar::Create_PlayerHP()
{
	CUi_PlayerHp::Desc  HPDesc;
	HPDesc.fX = -432.;
	HPDesc.fY = 307;
	HPDesc.iMaxFrame = 19;
	HPDesc.eRenderType = RenderType::Normal;
	HPDesc.iPriority = m_iNormal + 6;
	HPDesc.fSizeX = 250;
	HPDesc.fSizeY = 300;
	HPDesc.fSizeZ = 1.f;
	HPDesc.fSpeed = 0.f;
	HPDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	HPDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPGauge";
	HPDesc.TextureLevel = LEVEL_STATIC;
	HPDesc.iTextureNum = 6;
	HPDesc.ShaderPass = ShaderPass::PlayerBarHp;
	m_pHP = static_pointer_cast<CUi_PlayerHp>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerHp::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerHp::ObjID, &HPDesc));
	m_pHP->Rotation(XMConvertToRadians(6.5f));

	CUi_Sprite::Desc HPBarDesc{};
	HPBarDesc.fX = -432.f;
	HPBarDesc.fY = 307.f;
	HPBarDesc.iMaxFrame = 9;
	HPBarDesc.eRenderType = RenderType::Normal;
	HPBarDesc.iPriority = m_iNormal + 5;
	HPBarDesc.fSizeX = 250.f;
	HPBarDesc.fSizeY = 300.f;
	HPBarDesc.fSizeZ = 1.f;
	HPBarDesc.fSpeed = 0.f;
	HPBarDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	HPBarDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPBar";
	HPBarDesc.TextureLevel = LEVEL_STATIC;
	HPBarDesc.iTextureNum = 6;
	HPBarDesc.ShaderPass = ShaderPass::Default;
	m_pHPBar = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &HPBarDesc));
	m_pHPBar->Rotation(XMConvertToRadians(6.5f));

	CUi_Sprite::Desc HPShadowDesc{};
	HPShadowDesc.fX = -432.f;
	HPShadowDesc.fY = 303.f;
	m_OriginShadowHpPos.x = HPShadowDesc.fX;
	m_OriginShadowHpPos.y = HPShadowDesc.fY;
	HPShadowDesc.iMaxFrame = 9;
	HPShadowDesc.eRenderType = RenderType::Normal;
	HPShadowDesc.iPriority = m_iNormal + 1;
	HPShadowDesc.fSizeX = 250.f;
	HPShadowDesc.fSizeY = 300.f;
	HPShadowDesc.fSizeZ = 1.f;
	HPShadowDesc.fSpeed = 0.f;
	HPShadowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	HPShadowDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPBarShadow";
	HPShadowDesc.TextureLevel = LEVEL_STATIC;
	HPShadowDesc.iTextureNum = 6;
	HPShadowDesc.ShaderPass = ShaderPass::Default;
	m_pHPShadow = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &HPShadowDesc));
	m_pHPShadow->Rotation(XMConvertToRadians(6.5f));
}

void CUi_PlayerBar::Create_PlayerSubHP()
{
	CUi_PlayerHp::Desc  SubHPDesc;
	SubHPDesc.fX = -414.f;
	SubHPDesc.fY = 293.f;
	SubHPDesc.iMaxFrame = 19;
	SubHPDesc.eRenderType = RenderType::Priority;
	SubHPDesc.iPriority = 3;
	SubHPDesc.fSizeX = 205;
	SubHPDesc.fSizeY = 245;
	SubHPDesc.fSizeZ = 1.f;
	SubHPDesc.fSpeed = 0.f;
	SubHPDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	SubHPDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPGauge";
	SubHPDesc.TextureLevel = LEVEL_STATIC;
	SubHPDesc.iTextureNum = 0;
	SubHPDesc.ShaderPass = ShaderPass::PlayerBarHp;
	m_pSubHP = static_pointer_cast<CUi_PlayerHp>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerHp::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerHp::ObjID, &SubHPDesc));
	m_pSubHP->Rotation(XMConvertToRadians(6.5f));

	CUi_Sprite::Desc SubHPBarDesc{};
	SubHPBarDesc.fX = -414.f;
	SubHPBarDesc.fY = 293.f;
	SubHPBarDesc.iMaxFrame = 9;
	SubHPBarDesc.eRenderType = RenderType::Priority;
	SubHPBarDesc.iPriority = 2;
	SubHPBarDesc.fSizeX = 205.f;
	SubHPBarDesc.fSizeY = 245.f;
	SubHPBarDesc.fSizeZ = 1.f;
	SubHPBarDesc.fSpeed = 0.f;
	SubHPBarDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	SubHPBarDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPBar";
	SubHPBarDesc.TextureLevel = LEVEL_STATIC;
	SubHPBarDesc.iTextureNum = 0;
	SubHPBarDesc.ShaderPass = ShaderPass::Default;
	m_pSubHPBar = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &SubHPBarDesc));
	m_pSubHPBar->Rotation(XMConvertToRadians(6.5f));

	CUi_Sprite::Desc SubHPShadowDesc{};
	SubHPShadowDesc.fX = -414.f;
	SubHPShadowDesc.fY = 289.f;
	SubHPShadowDesc.iMaxFrame = 9;
	m_OriginShadowSubHpPos.x = SubHPShadowDesc.fX;
	m_OriginShadowSubHpPos.y = SubHPShadowDesc.fY;
	SubHPShadowDesc.eRenderType = RenderType::Normal;
	SubHPShadowDesc.iPriority = m_iNormal + 2;
	SubHPShadowDesc.fSizeX = 205.f;
	SubHPShadowDesc.fSizeY = 245.f;
	SubHPShadowDesc.fSizeZ = 1.f;
	SubHPShadowDesc.fSpeed = 0.f;
	SubHPShadowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	SubHPShadowDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPBarShadow";
	SubHPShadowDesc.TextureLevel = LEVEL_STATIC;
	SubHPShadowDesc.iTextureNum = 0;
	SubHPShadowDesc.ShaderPass = ShaderPass::Default;
	m_pSubHPShadow = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &SubHPShadowDesc));
	m_pSubHPShadow->Rotation(XMConvertToRadians(6.5f));
}

void CUi_PlayerBar::Create_PlayerReverb()
{
	m_pReverb.resize(REVERB_END);
	m_pReverbBar.resize(REVERB_END);
	m_pReverbShadow.resize(RESHADOW_END);
	m_pReverbGlow.resize(REGLOW_END);
	m_OriginShadowReverbPos.resize(3);

	CUi_PlayerReverb::Desc  ReverbDesc;
	ReverbDesc.fX = -495.f;
	ReverbDesc.fY = 321.f;
	ReverbDesc.eRenderType = RenderType::Priority;
	ReverbDesc.iPriority = 3;
	ReverbDesc.fSizeX = 130.f;
	ReverbDesc.fSizeY = 140.f;
	ReverbDesc.fSizeZ = 1.f;
	ReverbDesc.fSpeed = 0.f;
	ReverbDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverbDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbGauge";
	ReverbDesc.TextureLevel = LEVEL_STATIC;
	ReverbDesc.iTextureNum = 0;
	ReverbDesc.ShaderPass = ShaderPass::PlayerBarReverb;
	m_pReverb[REVERB_FIRST] = (static_pointer_cast<CUi_PlayerReverb>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerReverb::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerReverb::ObjID, &ReverbDesc)));
	m_pReverb[REVERB_FIRST]->Rotation(XMConvertToRadians(7.4f));

	ReverbDesc.fX = -428.f;
	ReverbDesc.fY = 325.f;
	ReverbDesc.fSizeX = 130.f;
	ReverbDesc.fSizeY = 140.f;
	ReverbDesc.iTextureNum = 1;
	m_pReverb[REVERB_SECOND] = (static_pointer_cast<CUi_PlayerReverb>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerReverb::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerReverb::ObjID, &ReverbDesc)));
	m_pReverb[REVERB_SECOND]->Rotation(XMConvertToRadians(7.4f));

	ReverbDesc.fX = -359.f;
	ReverbDesc.fY = 327.f;
	ReverbDesc.fSizeX = 130.f;
	ReverbDesc.fSizeY = 140.f;
	ReverbDesc.iTextureNum = 2;
	m_pReverb[REVERB_THIRD] = (static_pointer_cast<CUi_PlayerReverb>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerReverb::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerReverb::ObjID, &ReverbDesc)));
	m_pReverb[REVERB_THIRD]->Rotation(XMConvertToRadians(7.4f));

	ReverbDesc.fX = -304.f;
	ReverbDesc.fY = 327.f;
	ReverbDesc.fSizeX = 130.f;
	ReverbDesc.fSizeY = 140.f;
	ReverbDesc.iTextureNum = 3;
	ReverbDesc.eRenderType = RenderType::Priority;
	ReverbDesc.iPriority = 1;
	m_pReverb[REVERB_FOURTH] = (static_pointer_cast<CUi_PlayerReverb>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerReverb::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerReverb::ObjID, &ReverbDesc)));
	m_pReverb[REVERB_FOURTH]->Rotation(XMConvertToRadians(7.4f));

	CUi_Sprite::Desc ReverbBarDesc{};
	ReverbBarDesc.fX = -495.f;
	ReverbBarDesc.fY = 321.f;
	ReverbBarDesc.eRenderType = RenderType::Priority;
	ReverbBarDesc.iPriority = 2;
	ReverbBarDesc.fSizeX = 130.f;
	ReverbBarDesc.fSizeY = 140.f;
	ReverbBarDesc.fSizeZ = 1.f;
	ReverbBarDesc.fSpeed = 0.f;
	ReverbBarDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverbBarDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbBar";
	ReverbBarDesc.TextureLevel = LEVEL_STATIC;
	ReverbBarDesc.iTextureNum = 0;
	ReverbBarDesc.ShaderPass = ShaderPass::Default;
	m_pReverbBar[REVERB_FIRST] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbBarDesc)));
	m_pReverbBar[REVERB_FIRST]->Rotation(XMConvertToRadians(7.4f));

	ReverbBarDesc.fX = -428.f;
	ReverbBarDesc.fY = 325.f;
	ReverbBarDesc.fSizeX = 130.f;
	ReverbBarDesc.fSizeY = 140.f;
	ReverbBarDesc.iTextureNum = 1;
	m_pReverbBar[REVERB_SECOND] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbBarDesc)));
	m_pReverbBar[REVERB_SECOND]->Rotation(XMConvertToRadians(7.4f));

	ReverbBarDesc.fX = -359.f;
	ReverbBarDesc.fY = 327.f;
	ReverbBarDesc.fSizeX = 130.f;
	ReverbBarDesc.fSizeY = 140.f;
	ReverbBarDesc.iTextureNum = 2;
	m_pReverbBar[REVERB_THIRD] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbBarDesc)));
	m_pReverbBar[REVERB_THIRD]->Rotation(XMConvertToRadians(7.4f));

	ReverbBarDesc.fX = -304.f;
	ReverbBarDesc.fY = 327.f;
	ReverbBarDesc.fSizeX = 130.f;
	ReverbBarDesc.fSizeY = 140.f;
	ReverbBarDesc.iTextureNum = 3;
	ReverbBarDesc.eRenderType = RenderType::Priority;
	ReverbBarDesc.iPriority = 0;
	m_pReverbBar[REVERB_FOURTH] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbBarDesc)));
	m_pReverbBar[REVERB_FOURTH]->Rotation(XMConvertToRadians(7.4f));

	CUi_Sprite::Desc  ReverbShadowDesc;
	ReverbShadowDesc.fX = -461.f;
	ReverbShadowDesc.fY = 308.f;
	m_OriginShadowReverbPos[0].x = ReverbShadowDesc.fX;
	m_OriginShadowReverbPos[0].y = ReverbShadowDesc.fY;
	ReverbShadowDesc.eRenderType = RenderType::Normal;
	ReverbShadowDesc.iPriority = m_iNormal;
	ReverbShadowDesc.fSizeX = 502.f;
	ReverbShadowDesc.fSizeY = 502.f;
	ReverbShadowDesc.fSizeZ = 1.f;
	ReverbShadowDesc.fSpeed = 0.f;
	ReverbShadowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverbShadowDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbShadow";
	ReverbShadowDesc.TextureLevel = LEVEL_STATIC;
	ReverbShadowDesc.iTextureNum = 0;
	ReverbShadowDesc.ShaderPass = ShaderPass::Default;
	m_pReverbShadow[RESHADOW_FIRST] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbShadowDesc)));
	m_pReverbShadow[RESHADOW_FIRST]->Rotation(XMConvertToRadians(7.4f));

	ReverbShadowDesc.fX = -425.5f;
	ReverbShadowDesc.fY = 311.f;
	m_OriginShadowReverbPos[1].x = ReverbShadowDesc.fX;
	m_OriginShadowReverbPos[1].y = ReverbShadowDesc.fY;
	ReverbShadowDesc.fSizeX = 508.f;
	ReverbShadowDesc.fSizeY = 508.f;
	ReverbShadowDesc.iTextureNum = 1;
	m_pReverbShadow[RESHADOW_SECOND] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbShadowDesc)));
	m_pReverbShadow[RESHADOW_SECOND]->Rotation(XMConvertToRadians(7.2f));

	ReverbShadowDesc.fX = -399.f;
	ReverbShadowDesc.fY = 316.f;
	m_OriginShadowReverbPos[2].x = ReverbShadowDesc.fX;
	m_OriginShadowReverbPos[2].y = ReverbShadowDesc.fY;
	ReverbShadowDesc.fSizeX = 512.f;
	ReverbShadowDesc.fSizeY = 512.f;
	ReverbShadowDesc.iTextureNum = 2;
	m_pReverbShadow[RESHADOW_THIRD] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbShadowDesc)));
	m_pReverbShadow[RESHADOW_THIRD]->Rotation(XMConvertToRadians(7.2f));

	CUi_Sprite::Desc  ReverGlowbDesc;
	ReverGlowbDesc.fX = -205.f;
	ReverGlowbDesc.fY = 329.f;
	ReverGlowbDesc.eRenderType = RenderType::Priority;
	ReverGlowbDesc.iPriority = 3;
	ReverGlowbDesc.fSizeX = 195.f;
	ReverGlowbDesc.fSizeY = 210.f;
	ReverGlowbDesc.fSizeZ = 1.f;
	ReverGlowbDesc.fSpeed = 0.f;
	ReverGlowbDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverGlowbDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbGlow";
	ReverGlowbDesc.TextureLevel = LEVEL_STATIC;
	ReverGlowbDesc.iTextureNum = 0;
	ReverGlowbDesc.ShaderPass = ShaderPass::Blend;
	m_pReverbGlow[REGLOW_FIRST] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverGlowbDesc)));
	m_pReverbGlow[REGLOW_FIRST]->Rotation(XMConvertToRadians(7.4f));

	ReverGlowbDesc.fX = -104.f;
	ReverGlowbDesc.fY = 335.f;
	ReverGlowbDesc.fSizeX = 195.f;
	ReverGlowbDesc.fSizeY = 210.f;
	ReverGlowbDesc.iTextureNum = 1;
	m_pReverbGlow[REGLOW_SECOND] = (static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverGlowbDesc)));
	m_pReverbGlow[REGLOW_SECOND]->Rotation(XMConvertToRadians(7.4f));

	/*CUi_Sprite::Desc  ReverbOutLineDesc;
	ReverbOutLineDesc.fX = -395.f;
	ReverbOutLineDesc.fY = 296.f;
	ReverbOutLineDesc.eRenderType = RenderType::Normal;
	ReverbOutLineDesc.iPriority = m_iNormal + 8;
	ReverbOutLineDesc.fSizeX = 779.f;
	ReverbOutLineDesc.fSizeY = 787.f;
	ReverbOutLineDesc.fSizeZ = 1.f;
	ReverbOutLineDesc.fSpeed = 0.f;
	ReverbOutLineDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverbOutLineDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbOutline";
	ReverbOutLineDesc.TextureLevel = LEVEL_STATIC;
	ReverbOutLineDesc.iTextureNum = 0;
	ReverbOutLineDesc.ShaderPass = ShaderPass::Blend;
	m_pReverbOutline.emplace_back(static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbOutLineDesc)));
	m_pReverbOutline.back()->Rotation(XMConvertToRadians(7.2f));

	ReverbOutLineDesc.fX = -348.5f;
	ReverbOutLineDesc.fY = 300.f;
	ReverbOutLineDesc.fSizeX = 757.5f;
	ReverbOutLineDesc.fSizeY = 782.f;
	ReverbOutLineDesc.iTextureNum = 1;
	ReverbOutLineDesc.ShaderPass = ShaderPass::Blend;
	m_pReverbOutline.emplace_back(static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &ReverbOutLineDesc)));
	m_pReverbOutline.back()->Rotation(XMConvertToRadians(7.4f));*/
}

void CUi_PlayerBar::Create_PlayerBarLock()
{
	CUi_Sprite::Desc LockDesc{};
	LockDesc.fX = -553.f;
	LockDesc.fY = 302.f;
	LockDesc.eRenderType = RenderType::Priority;
	LockDesc.iPriority = 10;
	LockDesc.fSizeX = 45.f;
	LockDesc.fSizeY = 45.f;
	LockDesc.fSpeed = 0.f;
	LockDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	LockDesc.TextureFileName = L"Prototype_Component_Texture_PlayerBarLock";
	LockDesc.TextureLevel = LEVEL_STATIC;
	LockDesc.iTextureNum = 0;
	LockDesc.ShaderPass = ShaderPass::Default;
	m_pLock = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &LockDesc));
}

void CUi_PlayerBar::Create_PlayerPartner()
{
	CUi_Sprite::Desc GlowDesc{};
	GlowDesc.fX = -547.5f;
	GlowDesc.fY = 250.f;
	GlowDesc.eRenderType = RenderType::Normal;
	GlowDesc.iPriority = m_iNormal + 5;
	GlowDesc.fSizeX = 84.f;
	GlowDesc.fSizeY = 94.f;
	GlowDesc.fSpeed = 0.f;
	GlowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	GlowDesc.TextureFileName = L"Prototype_Component_Texture_PartnerGlow";
	GlowDesc.TextureLevel = LEVEL_STATIC;
	GlowDesc.iTextureNum = 0;
	GlowDesc.fColor = { 0.019f, 0.86f, 0.941f, 1.f };
	GlowDesc.ShaderPass = ShaderPass::Blend;
	m_pPartnerGlow = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &GlowDesc));
	m_pPartnerGlow->Rotation(XMConvertToRadians(358.f));

	CUi_Sprite::Desc PepperBlackDesc{};
	PepperBlackDesc.fX = -528.f;
	PepperBlackDesc.fY = 235.f;
	PepperBlackDesc.eRenderType = RenderType::Normal;
	PepperBlackDesc.iPriority = m_iNormal + 6;
	PepperBlackDesc.fSizeX = 200.f;
	PepperBlackDesc.fSizeY = 200.f;
	PepperBlackDesc.fSizeZ = 1.f;
	PepperBlackDesc.fSpeed = 0.f;
	PepperBlackDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PepperBlackDesc.TextureFileName = L"Prototype_Component_Texture_PartnerPeppermintBlack";
	PepperBlackDesc.TextureLevel = LEVEL_STATIC;
	PepperBlackDesc.iTextureNum = 0;
	PepperBlackDesc.ShaderPass = ShaderPass::Default;
	m_pPartnerBlack[PARTNER_PEPPERMINT] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PepperBlackDesc));

	CUi_PlayerPartnerGauge::Desc PepperDesc{};
	PepperDesc.fX = -528.f;
	PepperDesc.fY = 235.f;
	PepperDesc.iMaxFrame = 8;
	PepperDesc.eRenderType = RenderType::Normal;
	PepperDesc.iPriority = m_iNormal + 7;
	PepperDesc.fSizeX = 200.f;
	PepperDesc.fSizeY = 200.f;
	PepperDesc.fSizeZ = 1.f;
	PepperDesc.fSpeed = 0.f;
	PepperDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PepperDesc.TextureFileName = L"Prototype_Component_Texture_PartnerPeppermint";
	PepperDesc.TextureLevel = LEVEL_STATIC;
	PepperDesc.iTextureNum = 0;
	PepperDesc.ShaderPass = ShaderPass::PlayerBarPartner;
	PepperDesc.iReadyTextureNum = 0;
	PepperDesc.iPartnerIndex = 0;
	m_pPartnerSprite[PARTNER_PEPPERMINT] = static_pointer_cast<CUi_PlayerPartnerGauge>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerPartnerGauge::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerPartnerGauge::ObjID, &PepperDesc));

	CUi_Sprite::Desc MacaronBlackDesc{};
	MacaronBlackDesc.fX = -560.5f;
	MacaronBlackDesc.fY = 247.f;
	MacaronBlackDesc.eRenderType = RenderType::Normal;
	MacaronBlackDesc.iPriority = m_iNormal + 6;
	MacaronBlackDesc.fSizeX = 130.f;
	MacaronBlackDesc.fSizeY = 130.f;
	MacaronBlackDesc.fSizeZ = 1.f;
	MacaronBlackDesc.fSpeed = 0.f;
	MacaronBlackDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MacaronBlackDesc.TextureFileName = L"Prototype_Component_Texture_PartnerMacaronBlack";
	MacaronBlackDesc.TextureLevel = LEVEL_STATIC;
	MacaronBlackDesc.iTextureNum = 0;
	MacaronBlackDesc.ShaderPass = ShaderPass::Default;
	m_pPartnerBlack[PARTNER_MACARON] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &MacaronBlackDesc));

	CUi_PlayerPartnerGauge::Desc MacaronDesc{};
	MacaronDesc.fX = -560.5f;
	MacaronDesc.fY = 247.f;
	MacaronDesc.iMaxFrame = 8;
	MacaronDesc.eRenderType = RenderType::Normal;
	MacaronDesc.iPriority = m_iNormal + 7;
	MacaronDesc.fSizeX = 130.f;
	MacaronDesc.fSizeY = 130.f;
	MacaronDesc.fSizeZ = 1.f;
	MacaronDesc.fSpeed = 0.f;
	MacaronDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	MacaronDesc.TextureFileName = L"Prototype_Component_Texture_PartnerMacaron";
	MacaronDesc.TextureLevel = LEVEL_STATIC;
	MacaronDesc.iTextureNum = 0;
	MacaronDesc.ShaderPass = ShaderPass::PlayerBarPartner;
	MacaronDesc.iReadyTextureNum = 1;
	MacaronDesc.iPartnerIndex = 1;
	m_pPartnerSprite[PARTNER_MACARON] = static_pointer_cast<CUi_PlayerPartnerGauge>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerPartnerGauge::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerPartnerGauge::ObjID, &MacaronDesc));

	CUi_Sprite::Desc KorsicaBlackDesc{};
	KorsicaBlackDesc.fX = -582.f;
	KorsicaBlackDesc.fY = 248.f;
	KorsicaBlackDesc.eRenderType = RenderType::Normal;
	KorsicaBlackDesc.iPriority = m_iNormal + 6;
	KorsicaBlackDesc.fSizeX = 130.f;
	KorsicaBlackDesc.fSizeY = 130.f;
	KorsicaBlackDesc.fSizeZ = 1.f;
	KorsicaBlackDesc.fSpeed = 0.f;
	KorsicaBlackDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	KorsicaBlackDesc.TextureFileName = L"Prototype_Component_Texture_PartnerKorsicaBlack";
	KorsicaBlackDesc.TextureLevel = LEVEL_STATIC;
	KorsicaBlackDesc.iTextureNum = 0;
	KorsicaBlackDesc.ShaderPass = ShaderPass::Default;
	m_pPartnerBlack[PARTNER_KORSICA] = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &KorsicaBlackDesc));

	CUi_PlayerPartnerGauge::Desc KorsicaDesc{};
	KorsicaDesc.fX = -582.f;
	KorsicaDesc.fY = 248.f;
	KorsicaDesc.iMaxFrame = 8;
	KorsicaDesc.eRenderType = RenderType::Normal;
	KorsicaDesc.iPriority = m_iNormal + 7;
	KorsicaDesc.fSizeX = 130.f;
	KorsicaDesc.fSizeY = 130.f;
	KorsicaDesc.fSizeZ = 1.f;
	KorsicaDesc.fSpeed = 0.f;
	KorsicaDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	KorsicaDesc.TextureFileName = L"Prototype_Component_Texture_PartnerKorsica";
	KorsicaDesc.TextureLevel = LEVEL_STATIC;
	KorsicaDesc.iTextureNum = 0;
	KorsicaDesc.ShaderPass = ShaderPass::PlayerBarPartner;
	KorsicaDesc.iReadyTextureNum = 2;
	KorsicaDesc.iPartnerIndex = 2;
	m_pPartnerSprite[PARTNER_KORSICA] = static_pointer_cast<CUi_PlayerPartnerGauge>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_PlayerPartnerGauge::ObjID, m_iLevelIndex, L"CUi", CUi_PlayerPartnerGauge::ObjID, &KorsicaDesc));

	m_vecPartnerOrder.emplace_back(PARTNER_PEPPERMINT);
	m_vecPartnerOrder.emplace_back(PARTNER_MACARON);
	m_vecPartnerOrder.emplace_back(PARTNER_KORSICA);

	m_vecPartnerPosition.emplace_back(_float3(-528.f, 235.f, 0.7f));
	m_vecPartnerPosition.emplace_back(_float3(-560.5f, 247.f, 0.7f));
	m_vecPartnerPosition.emplace_back(_float3(-582.f, 248.f, 0.7f));

	m_vecPartnerPositionFightMode.emplace_back(_float3(-489.5f, 179.f, 0.7f));
	m_vecPartnerPositionFightMode.emplace_back(_float3(-537.5f, 201.f, 0.7f));
	m_vecPartnerPositionFightMode.emplace_back(_float3(-570.f, 202.f, 0.7f));

	m_PartnerColor[PARTNER_PEPPERMINT] = _float4{0.019f, 0.86f, 0.941f, 1.f};
	m_PartnerColor[PARTNER_MACARON] = _float4{0.043f, 0.898f, 0.325f, 1.f};
	m_PartnerColor[PARTNER_KORSICA] = _float4{0.8f, 0.223f, 0.372f, 1.f};
}

void CUi_PlayerBar::Move_Shadow(_float fTimeDelta)
{
	if (0.66f < BeatPercent())
	{
		m_bReturning = true;
	}
	else
	{
		m_bReturning = false;
	}
	if (m_bReturning)
	{
		m_pGuiterShadow->Set_Speed(50);
		m_pHPShadow->Set_Speed(50);
		m_pSubHPShadow->Set_Speed(50);
		m_pReverbShadow[RESHADOW_FIRST]->Set_Speed(50);
		m_pReverbShadow[RESHADOW_SECOND]->Set_Speed(50);
		m_pReverbShadow[RESHADOW_THIRD]->Set_Speed(50);

		m_pGuiterShadow->Move(MoveDir::Down, fTimeDelta * 2.5f);
		m_pGuiterShadow->Move(MoveDir::Right, fTimeDelta);

		m_pHPShadow->Move(MoveDir::Down, fTimeDelta * 2.5f);
		m_pHPShadow->Move(MoveDir::Right, fTimeDelta);

		m_pSubHPShadow->Move(MoveDir::Down, fTimeDelta * 2.5f);
		m_pSubHPShadow->Move(MoveDir::Right, fTimeDelta);

		m_pReverbShadow[RESHADOW_FIRST]->Move(MoveDir::Down, fTimeDelta * 2.5f);
		m_pReverbShadow[RESHADOW_FIRST]->Move(MoveDir::Right, fTimeDelta);

		m_pReverbShadow[RESHADOW_SECOND]->Move(MoveDir::Down, fTimeDelta * 2.5f);
		m_pReverbShadow[RESHADOW_SECOND]->Move(MoveDir::Right, fTimeDelta);

		m_pReverbShadow[RESHADOW_THIRD]->Move(MoveDir::Down, fTimeDelta * 2.5f);
		m_pReverbShadow[RESHADOW_THIRD]->Move(MoveDir::Right, fTimeDelta);
			
		// if right 50 => down 50 * 2.5 => 125 
		_float3 Pos = m_pGuiterShadow->Get_Pos();
		if (m_OriginShadowGuiterPos.x + 5 < Pos.x)
		{
			m_pGuiterShadow->Set_Pos(m_OriginShadowGuiterPos.x + 5, m_OriginShadowGuiterPos.y - 12.5f);
			m_pHPShadow->Set_Pos(m_OriginShadowHpPos.x + 5, m_OriginShadowHpPos.y - 12.5f);
			m_pSubHPShadow->Set_Pos(m_OriginShadowSubHpPos.x + 5, m_OriginShadowSubHpPos.y - 12.5f);
			m_pReverbShadow[RESHADOW_FIRST]->Set_Pos(m_OriginShadowReverbPos[0].x + 5, m_OriginShadowReverbPos[0].y - 12.5f);
			m_pReverbShadow[RESHADOW_SECOND]->Set_Pos(m_OriginShadowReverbPos[1].x + 5, m_OriginShadowReverbPos[1].y - 12.5f);
			m_pReverbShadow[RESHADOW_THIRD]->Set_Pos(m_OriginShadowReverbPos[2].x + 5, m_OriginShadowReverbPos[2].y - 12.5f);
		}
	}
	else
	{
		m_pGuiterShadow->Set_Speed(25);
		m_pHPShadow->Set_Speed(25);
		m_pGuiterShadow->Set_Speed(25);
		m_pReverbShadow[RESHADOW_FIRST]->Set_Speed(25);
		m_pReverbShadow[RESHADOW_SECOND]->Set_Speed(25);
		m_pReverbShadow[RESHADOW_THIRD]->Set_Speed(25);

		m_pGuiterShadow->Move(MoveDir::Up, fTimeDelta * 2.5f);
		m_pGuiterShadow->Move(MoveDir::Left, fTimeDelta);

		m_pHPShadow->Move(MoveDir::Up, fTimeDelta * 2.5f);
		m_pHPShadow->Move(MoveDir::Left, fTimeDelta);

		m_pSubHPShadow->Move(MoveDir::Up, fTimeDelta * 2.5f);
		m_pSubHPShadow->Move(MoveDir::Left, fTimeDelta);

		m_pReverbShadow[RESHADOW_FIRST]->Move(MoveDir::Up, fTimeDelta * 2.5f);
		m_pReverbShadow[RESHADOW_FIRST]->Move(MoveDir::Left, fTimeDelta);

		m_pReverbShadow[RESHADOW_SECOND]->Move(MoveDir::Up, fTimeDelta * 2.5f);
		m_pReverbShadow[RESHADOW_SECOND]->Move(MoveDir::Left, fTimeDelta);

		m_pReverbShadow[RESHADOW_THIRD]->Move(MoveDir::Up, fTimeDelta * 2.5f);
		m_pReverbShadow[RESHADOW_THIRD]->Move(MoveDir::Left, fTimeDelta);

		_float3 Pos = m_pGuiterShadow->Get_Pos();
		if (Pos.x < m_OriginShadowGuiterPos.x)
		{
			m_pGuiterShadow->Set_Pos(m_OriginShadowGuiterPos.x, m_OriginShadowGuiterPos.y);
			m_pHPShadow->Set_Pos(m_OriginShadowHpPos.x, m_OriginShadowHpPos.y);
			m_pSubHPShadow->Set_Pos(m_OriginShadowSubHpPos.x, m_OriginShadowSubHpPos.y);
			m_pReverbShadow[RESHADOW_FIRST]->Set_Pos(m_OriginShadowReverbPos[0].x, m_OriginShadowReverbPos[0].y);
			m_pReverbShadow[RESHADOW_SECOND]->Set_Pos(m_OriginShadowReverbPos[1].x, m_OriginShadowReverbPos[1].y);
			m_pReverbShadow[RESHADOW_THIRD]->Set_Pos(m_OriginShadowReverbPos[2].x, m_OriginShadowReverbPos[2].y);
		}
	}
}

void CUi_PlayerBar::Fight_Mode()
{
	if (m_bFight) 
		return;

	m_bFight = true;

	m_pGuiterLightning->PlayAnimation(CUI_Move::Create(_float3(-527.f, 282.f, 1.f), 0.2f));
	m_pGuiterLightning->PlayAnimation(CUI_Scale::Create(_float3(187.5f, 187.5f, 1.f), 0.2f));
	
	m_pGuiterBackground->PlayAnimation(CUI_Move::Create(_float3(-527.f, 282.f, 1.f), 0.2f));
	m_pGuiterBackground->PlayAnimation(CUI_Scale::Create(_float3(187.5f, 187.5f, 1.f), 0.2f));
	
	m_pGuiterShadow->PlayAnimation(CUI_Move::Create(_float3(-527.f, 277.f, 1.f), 0.2f));
	m_pGuiterShadow->PlayAnimation(CUI_Scale::Create(_float3(187.5f, 187.5f, 1.f), 0.2f));
	m_OriginShadowGuiterPos.x = m_OriginShadowGuiterPos.x + 23.f;
	m_OriginShadowGuiterPos.y = m_OriginShadowGuiterPos.y - 18.f;

	m_pHP->PlayAnimation(CUI_Move::Create(_float3(-352.f, 289.f, 1.f), 0.2f));
	m_pHP->PlayAnimation(CUI_Scale::Create(_float3(375.f, 450.f, 1.f), 0.2f));

	m_pHPBar->PlayAnimation(CUI_Move::Create(_float3(-352.f, 289.f, 1.f), 0.2f));
	m_pHPBar->PlayAnimation(CUI_Scale::Create(_float3(375.f, 450.f, 1.f), 0.2f));

	m_pHPShadow->PlayAnimation(CUI_Move::Create(_float3(-352.f, 285.f, 1.f), 0.2f));
	m_pHPShadow->PlayAnimation(CUI_Scale::Create(_float3(375.f, 450.f, 1.f), 0.2f));
	m_OriginShadowHpPos.x = m_OriginShadowHpPos.x + 80.f;
	m_OriginShadowHpPos.y = m_OriginShadowHpPos.y - 18.f;

	m_pSubHP->PlayAnimation(CUI_Move::Create(_float3(-324.f, 267.f, 1.f), 0.2f));
	m_pSubHP->PlayAnimation(CUI_Scale::Create(_float3(307.5f, 367.5f, 1.f), 0.2f));

	m_pSubHPBar->PlayAnimation(CUI_Move::Create(_float3(-324.f, 267.f, 1.f), 0.2f));
	m_pSubHPBar->PlayAnimation(CUI_Scale::Create(_float3(307.5f, 367.5f, 1.f), 0.2f));

	m_pSubHPShadow->PlayAnimation(CUI_Move::Create(_float3(-324.f, 263.f, 1.f), 0.2f));
	m_pSubHPShadow->PlayAnimation(CUI_Scale::Create(_float3(307.5f, 367.5f, 1.f), 0.2f));
	m_OriginShadowSubHpPos.x = m_OriginShadowSubHpPos.x + 90.f;
	m_OriginShadowSubHpPos.y = m_OriginShadowSubHpPos.y - 26.f;

	m_pReverb[REVERB_FIRST]->PlayAnimation(CUI_Move::Create(_float3(-445.f, 309.f, 1.f), 0.2f));
	m_pReverb[REVERB_FIRST]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverb[REVERB_SECOND]->PlayAnimation(CUI_Move::Create(_float3(-344.f, 315.f, 1.f), 0.2f));
	m_pReverb[REVERB_SECOND]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverb[REVERB_THIRD]->PlayAnimation(CUI_Move::Create(_float3(-241.f, 318.f, 1.f), 0.2f));
	m_pReverb[REVERB_THIRD]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverb[REVERB_FOURTH]->PlayAnimation(CUI_Move::Create(_float3(-158.5f, 318.f, 1.f), 0.2f));
	m_pReverb[REVERB_FOURTH]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_FIRST]->PlayAnimation(CUI_Move::Create(_float3(-445.f, 309.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_FIRST]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_SECOND]->PlayAnimation(CUI_Move::Create(_float3(-344.f, 315.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_SECOND]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_THIRD]->PlayAnimation(CUI_Move::Create(_float3(-241.f, 318.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_THIRD]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_FOURTH]->PlayAnimation(CUI_Move::Create(_float3(-158.5f, 318.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_FOURTH]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 210.f, 1.f), 0.2f));

	m_pReverbShadow[RESHADOW_FIRST]->PlayAnimation(CUI_Move::Create(_float3(-395.f, 292.f, 1.f), 0.2f));
	m_pReverbShadow[RESHADOW_FIRST]->PlayAnimation(CUI_Scale::Create(_float3(753.f, 753.f, 1.f), 0.2f));
	m_OriginShadowReverbPos[0].x = m_OriginShadowReverbPos[0].x + 66.f;
	m_OriginShadowReverbPos[0].y = m_OriginShadowReverbPos[0].y - 16.f;

	m_pReverbShadow[RESHADOW_SECOND]->PlayAnimation(CUI_Move::Create(_float3(-342.f, 295.f, 1.f), 0.2f));
	m_pReverbShadow[RESHADOW_SECOND]->PlayAnimation(CUI_Scale::Create(_float3(762.f, 762.f, 1.f), 0.2f));
	m_OriginShadowReverbPos[1].x = m_OriginShadowReverbPos[1].x + 82.5f;
	m_OriginShadowReverbPos[1].y = m_OriginShadowReverbPos[1].y - 16.f;

	m_pReverbShadow[RESHADOW_THIRD]->PlayAnimation(CUI_Move::Create(_float3(-301.f, 300.f, 1.f), 0.2f));
	m_pReverbShadow[RESHADOW_THIRD]->PlayAnimation(CUI_Scale::Create(_float3(768.f, 768.f, 1.f), 0.2f));
	m_OriginShadowReverbPos[2].x = m_OriginShadowReverbPos[2].x + 97.f;
	m_OriginShadowReverbPos[2].y = m_OriginShadowReverbPos[2].y - 16.f;

	m_pPartnerGlow->PlayAnimation(CUI_Move::Create(_float3(-519.5f, 200.f, 1.f), 0.2f));
	m_pPartnerGlow->PlayAnimation(CUI_Scale::Create(_float3(126.f, 141.f, 1.f), 0.2f));

	for (int i = 0; i < m_CurrentOrder.size(); ++i)
	{
		if (m_pPartnerSprite[m_CurrentOrder[i]] != nullptr)
		{
			m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPositionFightMode[i], 0.2f));
			m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPositionFightMode[i], 0.2f));

			if (i == 0)
			{
				m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(300.f, 300.f, 1.f), 0.2f));
				m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(300.f, 300.f, 1.f), 0.2f));
			}
			else
			{
				m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 195.f, 1.f), 0.2f));
				m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 195.f, 1.f), 0.2f));
			}
		}
	}
}

void CUi_PlayerBar::Basic_Mode()
{
	if (!m_bFight)
		return;

	m_bFight = false;

	m_pGuiterLightning->PlayAnimation(CUI_Move::Create(_float3(-550.f, 300.f, 1.f), 0.2f));
	m_pGuiterLightning->PlayAnimation(CUI_Scale::Create(_float3(125.f, 125.f, 1.f), 0.2f));

	m_pGuiterBackground->PlayAnimation(CUI_Move::Create(_float3(-550.f, 300.f, 1.f), 0.2f));
	m_pGuiterBackground->PlayAnimation(CUI_Scale::Create(_float3(125.f, 125.f, 1.f), 0.2f));

	m_pGuiterShadow->PlayAnimation(CUI_Move::Create(_float3(-550.f, 295.f, 1.f), 0.2f));
	m_pGuiterShadow->PlayAnimation(CUI_Scale::Create(_float3(125.f, 125.f, 1.f), 0.2f));
	m_OriginShadowGuiterPos.x = m_OriginShadowGuiterPos.x - 23.f;
	m_OriginShadowGuiterPos.y = m_OriginShadowGuiterPos.y + 18.f;

	m_pHP->PlayAnimation(CUI_Move::Create(_float3(-432.f, 307.f, 1.f), 0.2f));
	m_pHP->PlayAnimation(CUI_Scale::Create(_float3(250.f, 300.f, 1.f), 0.2f));

	m_pHPBar->PlayAnimation(CUI_Move::Create(_float3(-432.f, 307.f, 1.f), 0.2f));
	m_pHPBar->PlayAnimation(CUI_Scale::Create(_float3(250.f, 300.f, 1.f), 0.2f));

	m_pHPShadow->PlayAnimation(CUI_Move::Create(_float3(-432.f, 303.f, 1.f), 0.2f));
	m_pHPShadow->PlayAnimation(CUI_Scale::Create(_float3(250.f, 300.f, 1.f), 0.2f));
	m_OriginShadowHpPos.x = m_OriginShadowHpPos.x - 80.f;
	m_OriginShadowHpPos.y = m_OriginShadowHpPos.y + 18.f;

	m_pSubHP->PlayAnimation(CUI_Move::Create(_float3(-414.f, 293.f, 1.f), 0.2f));
	m_pSubHP->PlayAnimation(CUI_Scale::Create(_float3(205.f, 245.f, 1.f), 0.2f));

	m_pSubHPBar->PlayAnimation(CUI_Move::Create(_float3(-414.f, 293.f, 1.f), 0.2f));
	m_pSubHPBar->PlayAnimation(CUI_Scale::Create(_float3(205.f, 245.f, 1.f), 0.2f));

	m_pSubHPShadow->PlayAnimation(CUI_Move::Create(_float3(-414.f, 289.f, 1.f), 0.2f));
	m_pSubHPShadow->PlayAnimation(CUI_Scale::Create(_float3(205.f, 245.f, 1.f), 0.2f));
	m_OriginShadowSubHpPos.x = m_OriginShadowSubHpPos.x - 90.f;
	m_OriginShadowSubHpPos.y = m_OriginShadowSubHpPos.y + 26.f;

	m_pReverb[REVERB_FIRST]->PlayAnimation(CUI_Move::Create(_float3(-495.f, 321.f, 1.f), 0.2f));
	m_pReverb[REVERB_FIRST]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverb[REVERB_SECOND]->PlayAnimation(CUI_Move::Create(_float3(-428.f, 325.f, 1.f), 0.2f));
	m_pReverb[REVERB_SECOND]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverb[REVERB_THIRD]->PlayAnimation(CUI_Move::Create(_float3(-359.f, 327.f, 1.f), 0.2f));
	m_pReverb[REVERB_THIRD]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverb[REVERB_FOURTH]->PlayAnimation(CUI_Move::Create(_float3(-304.f, 327.f, 1.f), 0.2f));
	m_pReverb[REVERB_FOURTH]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_FIRST]->PlayAnimation(CUI_Move::Create(_float3(-495.f, 321.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_FIRST]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_SECOND]->PlayAnimation(CUI_Move::Create(_float3(-428.f, 325.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_SECOND]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_THIRD]->PlayAnimation(CUI_Move::Create(_float3(-359.f, 327.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_THIRD]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverbBar[REVERB_FOURTH]->PlayAnimation(CUI_Move::Create(_float3(-304.f, 327.f, 1.f), 0.2f));
	m_pReverbBar[REVERB_FOURTH]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 140.f, 1.f), 0.2f));

	m_pReverbShadow[RESHADOW_FIRST]->PlayAnimation(CUI_Move::Create(_float3(-461.f, 308.f, 1.f), 0.2f));
	m_pReverbShadow[RESHADOW_FIRST]->PlayAnimation(CUI_Scale::Create(_float3(502.f, 502.f, 1.f), 0.2f));
	m_OriginShadowReverbPos[0].x = m_OriginShadowReverbPos[0].x - 66.f;
	m_OriginShadowReverbPos[0].y = m_OriginShadowReverbPos[0].y + 16.f;

	m_pReverbShadow[RESHADOW_SECOND]->PlayAnimation(CUI_Move::Create(_float3(-425.5f, 311.f, 1.f), 0.2f));
	m_pReverbShadow[RESHADOW_SECOND]->PlayAnimation(CUI_Scale::Create(_float3(508.f, 508.f, 1.f), 0.2f));
	m_OriginShadowReverbPos[1].x = m_OriginShadowReverbPos[1].x - 82.5f;
	m_OriginShadowReverbPos[1].y = m_OriginShadowReverbPos[1].y + 16.f;

	m_pReverbShadow[RESHADOW_THIRD]->PlayAnimation(CUI_Move::Create(_float3(-399.f, 316.f, 1.f), 0.2f));
	m_pReverbShadow[RESHADOW_THIRD]->PlayAnimation(CUI_Scale::Create(_float3(512.f, 512.f, 1.f), 0.2f));
	m_OriginShadowReverbPos[2].x = m_OriginShadowReverbPos[2].x - 97.f;
	m_OriginShadowReverbPos[2].y = m_OriginShadowReverbPos[2].y + 16.f;

	m_pPartnerGlow->PlayAnimation(CUI_Move::Create(_float3(-547.5f, 250.f, 1.f), 0.2f));
	m_pPartnerGlow->PlayAnimation(CUI_Scale::Create(_float3(84.f, 94.f, 1.f), 0.2f));

	for (int i = 0; i < m_CurrentOrder.size(); ++i)
	{
		if (m_pPartnerSprite[m_CurrentOrder[i]] != nullptr)
		{
			m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPosition[i], 0.2f));
			m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPosition[i], 0.2f));

			if (i == 0)
			{
				m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(200.f, 200.f, 1.f), 0.2f));
				m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(200.f, 200.f, 1.f), 0.2f));
			}
			else
			{
				m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 130.f, 1.f), 0.2f));
				m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 130.f, 1.f), 0.2f));
			}
		}
	}
}

void CUi_PlayerBar::Renew_Hp()
{
	_float PlayerHp = PLAYERMANAGER->Get_Hp();
	_float PlayerMaxHp = PLAYERMANAGER->Get_MaxHp();

	_float MainHpPercent = 0.0f;
	_float SubHpPercent = 0.0f;

	if (PlayerMaxHp <= 100)
	{
		MainHpPercent = PlayerHp / PlayerMaxHp;
		m_pHP->Set_PercentHp(MainHpPercent);
		m_pSubHP->Set_PercentHp(0.0f);
	}
	else
	{
		if (PlayerHp <= 100)
		{
			MainHpPercent = PlayerHp / 100.0f;
			m_pHP->Set_PercentHp(MainHpPercent);
			m_pSubHP->Set_PercentHp(0.0f);
		}
		else
		{
			MainHpPercent = 1.0f;
			m_pHP->Set_PercentHp(MainHpPercent);

			SubHpPercent = (PlayerHp - 100) / (PlayerMaxHp - 100);
			m_pSubHP->Set_PercentHp(SubHpPercent);
		}
	}

	if (PlayerMaxHp <= 100)
	{
		m_pHP->SetVisible(true);
		m_pHPBar->SetVisible(true);
		m_pHPShadow->SetVisible(true);

		m_pSubHP->SetVisible(false);
		m_pSubHPBar->SetVisible(false);
		m_pSubHPShadow->SetVisible(false);
	}
	else
	{
		m_pHP->SetVisible(true);
		m_pHPBar->SetVisible(true);
		m_pHPShadow->SetVisible(true);

		m_pSubHP->SetVisible(true);
		m_pSubHPBar->SetVisible(true);
		m_pSubHPShadow->SetVisible(true);
	}

	if (PlayerMaxHp <= 70)
	{
		m_pHP->Set_TextureNum(6);
		m_pHPBar->Set_TextureNum(6);
		m_pHPShadow->Set_TextureNum(6);
	}
	else if (PlayerMaxHp <= 80)
	{
		m_pHP->Set_TextureNum(7);
		m_pHPBar->Set_TextureNum(7);
		m_pHPShadow->Set_TextureNum(7);
	}
	else if (PlayerMaxHp <= 90)
	{
		m_pHP->Set_TextureNum(8);
		m_pHPBar->Set_TextureNum(8);
		m_pHPShadow->Set_TextureNum(8);
	}
	else if (PlayerMaxHp <= 100)
	{
		m_pHP->Set_TextureNum(9);
		m_pHPBar->Set_TextureNum(9);
		m_pHPShadow->Set_TextureNum(9);
	}
	else if (PlayerMaxHp <= 110)
	{
		m_pSubHP->Set_TextureNum(0);
		m_pSubHPBar->Set_TextureNum(0);
		m_pSubHPShadow->Set_TextureNum(0);
	}
	else if (PlayerMaxHp <= 120)
	{
		m_pSubHP->Set_TextureNum(1);
		m_pSubHPBar->Set_TextureNum(1);
		m_pSubHPShadow->Set_TextureNum(1);
	}
	else if (PlayerMaxHp <= 130)
	{
		m_pSubHP->Set_TextureNum(2);
		m_pSubHPBar->Set_TextureNum(2);
		m_pSubHPShadow->Set_TextureNum(2);
	}
	else if (PlayerMaxHp <= 140)
	{
		m_pSubHP->Set_TextureNum(3);
		m_pSubHPBar->Set_TextureNum(3);
		m_pSubHPShadow->Set_TextureNum(3);
	}
	else if (PlayerMaxHp <= 150)
	{
		m_pSubHP->Set_TextureNum(4);
		m_pSubHPBar->Set_TextureNum(4);
		m_pSubHPShadow->Set_TextureNum(4);
	}
	else if (PlayerMaxHp <= 160)
	{
		m_pSubHP->Set_TextureNum(5);
		m_pSubHPBar->Set_TextureNum(5);
		m_pSubHPShadow->Set_TextureNum(5);
	}
	else if (PlayerMaxHp <= 170)
	{
		m_pSubHP->Set_TextureNum(6);
		m_pSubHPBar->Set_TextureNum(6);
		m_pSubHPShadow->Set_TextureNum(6);
	}
	else if (PlayerMaxHp <= 180)
	{
		m_pSubHP->Set_TextureNum(7);
		m_pSubHPBar->Set_TextureNum(7);
		m_pSubHPShadow->Set_TextureNum(7);
	}
	else if (PlayerMaxHp <= 190)
	{
		m_pSubHP->Set_TextureNum(8);
		m_pSubHPBar->Set_TextureNum(8);
		m_pSubHPShadow->Set_TextureNum(8);
	}
	else if (PlayerMaxHp <= 200)
	{
		m_pSubHP->Set_TextureNum(9);
		m_pSubHPBar->Set_TextureNum(9);
		m_pSubHPShadow->Set_TextureNum(9);
	}
}

void CUi_PlayerBar::Renew_Reverb()
{
	if (m_bReverb)
	{
		_float PlayerReverb = PLAYERMANAGER->Get_Reverb();
		_float PlayerMaxReverb = PLAYERMANAGER->Get_MaxReverb();
		_float PlayerReverbPercent = PlayerReverb / PlayerMaxReverb;

		_float Reverb1Percent = 0.0f;
		_float Reverb2Percent = 0.0f;
		_float Reverb3Percent = 0.0f;
		_float Reverb4Percent = 0.0f;

		if (PlayerReverb >= 100)
		{
			m_pGuiterLightning->SetVisible(true);
			/*m_pReverbOutline[0]->SetVisible(true);*/
		}
		else
		{
			m_pGuiterLightning->SetVisible(false);
			/*m_pReverbOutline[0]->SetVisible(false);*/
		}

		if (PlayerMaxReverb <= 100)
		{
			if (PlayerReverbPercent > 0.5f)
			{
				Reverb2Percent = (PlayerReverbPercent - 0.5f) * 2.0f;
				Reverb1Percent = 1.0f;
			}
			else
			{
				Reverb2Percent = 0.0f;
				Reverb1Percent = PlayerReverbPercent * 2.0f;
			}

			m_pReverb[REVERB_FIRST]->Set_PercentReverb(Reverb1Percent);
			m_pReverb[REVERB_SECOND]->Set_PercentReverb(Reverb2Percent);

			m_pReverb[REVERB_FIRST]->SetVisible(true);
			m_pReverbBar[REVERB_FIRST]->SetVisible(true);
			m_pReverb[REVERB_SECOND]->SetVisible(true);
			m_pReverbBar[REVERB_SECOND]->SetVisible(true);
			m_pReverbShadow[RESHADOW_FIRST]->SetVisible(true);

			m_pReverb[REVERB_THIRD]->SetVisible(false);
			m_pReverbBar[REVERB_THIRD]->SetVisible(false);
			m_pReverbShadow[RESHADOW_SECOND]->SetVisible(false);
			m_pReverb[REVERB_FOURTH]->SetVisible(false);
			m_pReverbBar[REVERB_FOURTH]->SetVisible(false);
			m_pReverbShadow[RESHADOW_THIRD]->SetVisible(false);
		}
		else if (PlayerMaxReverb <= 150)
		{
			if (PlayerReverbPercent > 2.0f / 3.0f)
			{
				Reverb3Percent = (PlayerReverbPercent - 2.0f / 3.0f) * 3.0f;
				Reverb2Percent = 1.0f;
				Reverb1Percent = 1.0f;
			}
			else if (PlayerReverbPercent > 1.0f / 3.0f)
			{
				Reverb3Percent = 0.0f;
				Reverb2Percent = (PlayerReverbPercent - 1.0f / 3.0f) * 3.0f;
				Reverb1Percent = 1.0f;
			}
			else
			{
				Reverb3Percent = 0.0f;
				Reverb2Percent = 0.0f;
				Reverb1Percent = PlayerReverbPercent * 3.0f;
			}

			m_pReverb[REVERB_FIRST]->Set_PercentReverb(Reverb1Percent);
			m_pReverb[REVERB_SECOND]->Set_PercentReverb(Reverb2Percent);
			m_pReverb[REVERB_THIRD]->Set_PercentReverb(Reverb3Percent);

			m_pReverb[REVERB_FIRST]->SetVisible(true);
			m_pReverbBar[REVERB_FIRST]->SetVisible(true);
			m_pReverb[REVERB_SECOND]->SetVisible(true);
			m_pReverbBar[REVERB_SECOND]->SetVisible(true);
			m_pReverb[REVERB_THIRD]->SetVisible(true);
			m_pReverbBar[REVERB_THIRD]->SetVisible(true);
			m_pReverbShadow[RESHADOW_SECOND]->SetVisible(true);

			m_pReverbShadow[RESHADOW_FIRST]->SetVisible(false);
			m_pReverb[REVERB_FOURTH]->SetVisible(false);
			m_pReverbBar[REVERB_FOURTH]->SetVisible(false);
			m_pReverbShadow[RESHADOW_THIRD]->SetVisible(false);
		}
		else if (PlayerMaxReverb <= 200)
		{
			if (PlayerReverbPercent > 3.0f / 4.0f)
			{
				Reverb4Percent = (PlayerReverbPercent - 3.0f / 4.0f) * 4.0f;
				Reverb3Percent = 1.0f;
				Reverb2Percent = 1.0f;
				Reverb1Percent = 1.0f;
			}
			else if (PlayerReverbPercent > 2.0f / 4.0f)
			{
				Reverb4Percent = 0.0f;
				Reverb3Percent = (PlayerReverbPercent - 2.0f / 4.0f) * 4.0f;
				Reverb2Percent = 1.0f;
				Reverb1Percent = 1.0f;
			}
			else if (PlayerReverbPercent > 1.0f / 4.0f)
			{
				Reverb4Percent = 0.0f;
				Reverb3Percent = 0.0f;
				Reverb2Percent = (PlayerReverbPercent - 1.0f / 4.0f) * 4.0f;
				Reverb1Percent = 1.0f;
			}
			else
			{
				Reverb4Percent = 0.0f;
				Reverb3Percent = 0.0f;
				Reverb2Percent = 0.0f;
				Reverb1Percent = PlayerReverbPercent * 4.0f;
			}

			m_pReverb[REVERB_FIRST]->Set_PercentReverb(Reverb1Percent);
			m_pReverb[REVERB_SECOND]->Set_PercentReverb(Reverb2Percent);
			m_pReverb[REVERB_THIRD]->Set_PercentReverb(Reverb3Percent);
			m_pReverb[REVERB_FOURTH]->Set_PercentReverb(Reverb4Percent);

			m_pReverb[REVERB_FIRST]->SetVisible(true);
			m_pReverbBar[REVERB_FIRST]->SetVisible(true);
			m_pReverb[REVERB_SECOND]->SetVisible(true);
			m_pReverbBar[REVERB_SECOND]->SetVisible(true);
			m_pReverb[REVERB_THIRD]->SetVisible(true);
			m_pReverbBar[REVERB_THIRD]->SetVisible(true);
			m_pReverb[REVERB_FOURTH]->SetVisible(true);
			m_pReverbBar[REVERB_FOURTH]->SetVisible(true);
			m_pReverbShadow[RESHADOW_THIRD]->SetVisible(true);

			m_pReverbShadow[RESHADOW_SECOND]->SetVisible(false);
			m_pReverbShadow[RESHADOW_FIRST]->SetVisible(false);
		}
	}
}

void CUi_PlayerBar::Enable_Reverb()
{
	Fight_Mode();

	if (m_pReverbGlow[REGLOW_FIRST] != nullptr && m_pReverbGlow[REGLOW_SECOND] != nullptr)
	{
		m_pReverbGlow[REGLOW_FIRST]->PlayAnimation(CUI_CallFunction::Create([=]()
			{
				m_pReverbGlow[REGLOW_FIRST]->SetVisible(true);

			}, 0.5f));

		m_pReverbGlow[REGLOW_SECOND]->PlayAnimation(CUI_CallFunction::Create([=]()
			{
				m_pReverbGlow[REGLOW_SECOND]->SetVisible(true);

			}, 0.5f));

		m_pReverbGlow[REGLOW_FIRST]->PlayAnimation(CUI_CallFunction::Create([=]()
			{
				m_pReverbGlow[REGLOW_FIRST]->PlayAnimation(CUI_Move::Create(_float3(-445.f, 309.f, 1.f), 0.1f));
				m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"MakeReverb");

			}, 0.5f));

		m_pReverbGlow[REGLOW_SECOND]->PlayAnimation(CUI_CallFunction::Create([=]()
			{
				m_pReverbGlow[REGLOW_SECOND]->PlayAnimation(CUI_Move::Create(_float3(-344.f, 315.f, 1.f), 0.1f));

			}, 0.5f));


		m_pReverbGlow[REGLOW_FIRST]->PlayAnimation(CUI_CallFunction::Create([=]()
			{
				m_pReverbGlow[REGLOW_FIRST]->SetVisible(false);

			}, 1.1f));

		m_pReverbGlow[REGLOW_SECOND]->PlayAnimation(CUI_CallFunction::Create([=]()
			{
				m_pReverbGlow[REGLOW_SECOND]->SetVisible(false);
				m_bReverb = true;
				Basic_Mode();

			}, 1.1f));
		
	}
}

#pragma region Test

void CUi_PlayerBar::Set_HpStateTest()
{
	_float MainHpPercent = 0.0f;
	_float SubHpPercent = 0.0f;

	if (m_fPlayerMaxHp <= 100)
	{
		MainHpPercent = m_fPlayerHp / m_fPlayerMaxHp;
		m_pHP->Set_PercentHp(MainHpPercent);
		m_pSubHP->Set_PercentHp(0.0f);
	}
	else
	{
		if (m_fPlayerHp <= 100)
		{
			MainHpPercent = m_fPlayerHp / 100.0f;
			m_pHP->Set_PercentHp(MainHpPercent);
			m_pSubHP->Set_PercentHp(0.0f);
		}
		else
		{
			MainHpPercent = 1.0f;
			m_pHP->Set_PercentHp(MainHpPercent);

			SubHpPercent = (m_fPlayerHp - 100) / (m_fPlayerMaxHp - 100);
			m_pSubHP->Set_PercentHp(SubHpPercent);
		}
	}

	if (m_fPlayerMaxHp <= 100)
	{
		m_pHP->SetVisible(true);
		m_pHPBar->SetVisible(true);  
		m_pHPShadow->SetVisible(true);
		
		m_pSubHP->SetVisible(false);
		m_pSubHPBar->SetVisible(false);
		m_pSubHPShadow->SetVisible(false);
	}
	else
	{
		m_pHP->SetVisible(true);
		m_pHPBar->SetVisible(true);
		m_pHPShadow->SetVisible(true);

		m_pSubHP->SetVisible(true);
		m_pSubHPBar->SetVisible(true);
		m_pSubHPShadow->SetVisible(true);
	}

	if (m_fPlayerMaxHp <= 70)
	{
		m_pHP->Set_TextureNum(6);
		m_pHPBar->Set_TextureNum(6);
		m_pHPShadow->Set_TextureNum(6);
	}
	else if (m_fPlayerMaxHp <= 80)
	{
		m_pHP->Set_TextureNum(7);
		m_pHPBar->Set_TextureNum(7);
		m_pHPShadow->Set_TextureNum(7);
	}
	else if (m_fPlayerMaxHp <= 90)
	{
		m_pHP->Set_TextureNum(8);
		m_pHPBar->Set_TextureNum(8);
		m_pHPShadow->Set_TextureNum(8);
	}
	else if (m_fPlayerMaxHp <= 100)
	{
		m_pHP->Set_TextureNum(9);
		m_pHPBar->Set_TextureNum(9);
		m_pHPShadow->Set_TextureNum(9);
	}
	else if (m_fPlayerMaxHp <= 110)
	{
		m_pSubHP->Set_TextureNum(0);
		m_pSubHPBar->Set_TextureNum(0);
		m_pSubHPShadow->Set_TextureNum(0);
	}
	else if (m_fPlayerMaxHp <= 120)
	{
		m_pSubHP->Set_TextureNum(1);
		m_pSubHPBar->Set_TextureNum(1);
		m_pSubHPShadow->Set_TextureNum(1);
	}
	else if (m_fPlayerMaxHp <= 130)
	{
		m_pSubHP->Set_TextureNum(2);
		m_pSubHPBar->Set_TextureNum(2);
		m_pSubHPShadow->Set_TextureNum(2);
	}
	else if (m_fPlayerMaxHp <= 140)
	{
		m_pSubHP->Set_TextureNum(3);
		m_pSubHPBar->Set_TextureNum(3);
		m_pSubHPShadow->Set_TextureNum(3);
	}
	else if (m_fPlayerMaxHp <= 150)
	{
		m_pSubHP->Set_TextureNum(4);
		m_pSubHPBar->Set_TextureNum(4);
		m_pSubHPShadow->Set_TextureNum(4);
	}
	else if (m_fPlayerMaxHp <= 160)
	{
		m_pSubHP->Set_TextureNum(5);
		m_pSubHPBar->Set_TextureNum(5);
		m_pSubHPShadow->Set_TextureNum(5);
	}
	else if (m_fPlayerMaxHp <= 170)
	{
		m_pSubHP->Set_TextureNum(6);
		m_pSubHPBar->Set_TextureNum(6);
		m_pSubHPShadow->Set_TextureNum(6);
	}
	else if (m_fPlayerMaxHp <= 180)
	{
		m_pSubHP->Set_TextureNum(7);
		m_pSubHPBar->Set_TextureNum(7);
		m_pSubHPShadow->Set_TextureNum(7);
	}
	else if (m_fPlayerMaxHp <= 190)
	{
		m_pSubHP->Set_TextureNum(8);
		m_pSubHPBar->Set_TextureNum(8);
		m_pSubHPShadow->Set_TextureNum(8);
	}
	else if (m_fPlayerMaxHp <= 200)
	{
		m_pSubHP->Set_TextureNum(9);
		m_pSubHPBar->Set_TextureNum(9);
		m_pSubHPShadow->Set_TextureNum(9);
	}
}

void CUi_PlayerBar::Set_ReverbStateTest()
{
	_float PlayerReverbPercent = m_fPlayerReverb / m_fPlayerMaxReverb;
	_float Reverb1Percent = 0.0f;
	_float Reverb2Percent = 0.0f;
	_float Reverb3Percent = 0.0f;
	_float Reverb4Percent = 0.0f;

	if (m_fPlayerReverb >= 100)
	{
		m_pGuiterLightning->SetVisible(true);
		/*m_pReverbOutline[0]->SetVisible(true);*/
	}
	else
	{
		m_pGuiterLightning->SetVisible(false);
		/*m_pReverbOutline[0]->SetVisible(false);*/
	}

	if (m_fPlayerMaxReverb <= 100)
	{
		if (PlayerReverbPercent > 0.5f)
		{
			Reverb2Percent = (PlayerReverbPercent - 0.5f) * 2.0f;
			Reverb1Percent = 1.0f;
		}
		else
		{
			Reverb2Percent = 0.0f;
			Reverb1Percent = PlayerReverbPercent * 2.0f;
		}

		m_pReverb[REVERB_FIRST]->Set_PercentReverb(Reverb1Percent);
		m_pReverb[REVERB_SECOND]->Set_PercentReverb(Reverb2Percent);

		m_pReverb[REVERB_FIRST]->SetVisible(true);
		m_pReverbBar[REVERB_FIRST]->SetVisible(true);
		m_pReverb[REVERB_SECOND]->SetVisible(true);
		m_pReverbBar[REVERB_SECOND]->SetVisible(true);
		m_pReverbShadow[RESHADOW_FIRST]->SetVisible(true);

		m_pReverb[REVERB_THIRD]->SetVisible(false);
		m_pReverbBar[REVERB_THIRD]->SetVisible(false);
		m_pReverbShadow[RESHADOW_SECOND]->SetVisible(false);
		m_pReverb[REVERB_FOURTH]->SetVisible(false);
		m_pReverbBar[REVERB_FOURTH]->SetVisible(false);
		m_pReverbShadow[RESHADOW_THIRD]->SetVisible(false);
	}
	else if (m_fPlayerMaxReverb <= 150)
	{
		if (PlayerReverbPercent > 2.0f / 3.0f)
		{
			Reverb3Percent = (PlayerReverbPercent - 2.0f / 3.0f) * 3.0f;
			Reverb2Percent = 1.0f;
			Reverb1Percent = 1.0f;
		}
		else if (PlayerReverbPercent > 1.0f / 3.0f)
		{
			Reverb3Percent = 0.0f;
			Reverb2Percent = (PlayerReverbPercent - 1.0f / 3.0f) * 3.0f;
			Reverb1Percent = 1.0f;
		}
		else
		{
			Reverb3Percent = 0.0f;
			Reverb2Percent = 0.0f;
			Reverb1Percent = PlayerReverbPercent * 3.0f;
		}

		m_pReverb[REVERB_FIRST]->Set_PercentReverb(Reverb1Percent);
		m_pReverb[REVERB_SECOND]->Set_PercentReverb(Reverb2Percent);
		m_pReverb[REVERB_THIRD]->Set_PercentReverb(Reverb3Percent);

		m_pReverb[REVERB_THIRD]->SetVisible(true);
		m_pReverbBar[REVERB_THIRD]->SetVisible(true);
		m_pReverbShadow[RESHADOW_SECOND]->SetVisible(true);

		m_pReverbShadow[RESHADOW_FIRST]->SetVisible(false);
		m_pReverb[REVERB_FOURTH]->SetVisible(false);
		m_pReverbBar[REVERB_FOURTH]->SetVisible(false);
		m_pReverbShadow[RESHADOW_THIRD]->SetVisible(false);
	}
	else if (m_fPlayerMaxReverb <= 200)
	{
		if (PlayerReverbPercent > 3.0f / 4.0f)
		{
			Reverb4Percent = (PlayerReverbPercent - 3.0f / 4.0f) * 4.0f;
			Reverb3Percent = 1.0f;
			Reverb2Percent = 1.0f;
			Reverb1Percent = 1.0f;
		}
		else if (PlayerReverbPercent > 2.0f / 4.0f)
		{
			Reverb4Percent = 0.0f;
			Reverb3Percent = (PlayerReverbPercent - 2.0f / 4.0f) * 4.0f;
			Reverb2Percent = 1.0f;
			Reverb1Percent = 1.0f;
		}
		else if (PlayerReverbPercent > 1.0f / 4.0f)
		{
			Reverb4Percent = 0.0f;
			Reverb3Percent = 0.0f;
			Reverb2Percent = (PlayerReverbPercent - 1.0f / 4.0f) * 4.0f;
			Reverb1Percent = 1.0f;
		}
		else
		{
			Reverb4Percent = 0.0f;
			Reverb3Percent = 0.0f;
			Reverb2Percent = 0.0f;
			Reverb1Percent = PlayerReverbPercent * 4.0f;
		}

		m_pReverb[REVERB_FIRST]->Set_PercentReverb(Reverb1Percent);
		m_pReverb[REVERB_SECOND]->Set_PercentReverb(Reverb2Percent);
		m_pReverb[REVERB_THIRD]->Set_PercentReverb(Reverb3Percent);
		m_pReverb[REVERB_FOURTH]->Set_PercentReverb(Reverb4Percent);

		m_pReverb[REVERB_FOURTH]->SetVisible(true);
		m_pReverbBar[REVERB_FOURTH]->SetVisible(true);
		m_pReverbShadow[RESHADOW_THIRD]->SetVisible(true);

		m_pReverbShadow[RESHADOW_FIRST]->SetVisible(false);
		m_pReverbShadow[RESHADOW_SECOND]->SetVisible(false);
	}
}

#pragma endregion 

void CUi_PlayerBar::Partner_Respawn(PARTNER PartnerType)
{
	if (m_pPartnerSprite[PartnerType] != nullptr)
	{
		m_pPartnerSprite[PartnerType]->PlayAnimation(CUI_CallFunction::Create([=]()
			{
				auto pPartnerSprite = m_pPartnerSprite[PartnerType];

				if (!pPartnerSprite->IsRespawning())
				{
					pPartnerSprite->SetRespawn(true);
				}
			}, 0.f));
	}
}

void CUi_PlayerBar::Change_Partner()
{
	// m_vecPartnerOrder : 0, 1, 2

	m_iCurPartnerIndex = (++m_iCurPartnerIndex % m_vecPartnerOrder.size()); // 2

	m_CurrentOrder.clear();

	for (int i = m_iCurPartnerIndex; i < m_vecPartnerOrder.size(); ++i)
	{
		m_CurrentOrder.emplace_back(m_vecPartnerOrder[i]); // m_CurrentOrder 2
	}
	for (int i = 0; i < m_iCurPartnerIndex; ++i)
	{
		m_CurrentOrder.emplace_back(m_vecPartnerOrder[i]); // m_CurrentOrder 2, 0, 1
	}

	if (!m_bFight)
	{
		for (int i = 0; i < m_CurrentOrder.size(); ++i)
		{
			if (m_pPartnerSprite[m_CurrentOrder[i]] != nullptr && i < m_vecPartnerPosition.size())
			{
				m_pPartnerSprite[m_CurrentOrder[i]]->OnPartnerChange(i, m_vecPartnerPosition[i], i == 0 ? _float3(200.f, 200.f, 1.f) : _float3(130.f, 130.f, 1.f), 0.2f);

				m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPosition[i], 0.2f));
				/*m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPosition[i], 0.5f));*/

				if (i == 0)
				{
					m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(200.f, 200.f, 1.f), 0.2f));
					/*m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(200.f, 200.f, 1.f), 0.5f));*/

					m_pPartnerGlow->Set_Color(m_PartnerColor[m_CurrentOrder[i]]);
				}
				else if (i == (m_CurrentOrder.size() - 1))
				{
					m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 130.f, 1.f), 0.2f));
					/*m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(130.f, 130.f, 1.f), 0.5f));*/
				}

				m_pPartnerBlack[m_CurrentOrder[i]]->Set_RenderPriority((_int)m_CurrentOrder.size() - i + 5);
				m_pPartnerSprite[m_CurrentOrder[i]]->Set_RenderPriority((_int)m_CurrentOrder.size() - i + 6);
			}
		}
	}
	else
	{
		for (int i = 0; i < m_CurrentOrder.size(); ++i)
		{
			if (m_pPartnerSprite[m_CurrentOrder[i]] != nullptr && i < m_vecPartnerPositionFightMode.size())
			{
				m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPositionFightMode[i], 0.2f));
				m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Move::Create(m_vecPartnerPositionFightMode[i], 0.2f));

				if (i == 0)
				{
					m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(300.f, 300.f, 1.f), 0.2f));
					m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(300.f, 300.f, 1.f), 0.2f));

					m_pPartnerGlow->Set_Color(m_PartnerColor[m_CurrentOrder[i]]);
				}
				else if (i == (m_CurrentOrder.size() - 1))
				{
					m_pPartnerBlack[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 195.f, 1.f), 0.2f));
					m_pPartnerSprite[m_CurrentOrder[i]]->PlayAnimation(CUI_Scale::Create(_float3(195.f, 195.f, 1.f), 0.2f));
				}

				m_pPartnerBlack[m_CurrentOrder[i]]->Set_RenderPriority((_int)m_CurrentOrder.size() - i + 5);
				m_pPartnerSprite[m_CurrentOrder[i]]->Set_RenderPriority((_int)m_CurrentOrder.size() - i + 6);
			}
		}
	}
}

void CUi_PlayerBar::ReceiveUIEvent(UI_EVENT InUIEvent)
{
	switch (InUIEvent)
	{
	case UI_EVENT_CHANGE_PARTNER:
	{
		Change_Partner();
	}
	break;

	case UI_EVENT_CHANGE_HP:
	case UI_EVENT_CHANGE_MAX_HP:
	{
		Renew_Hp();
	}
	break;

	case UI_EVENT_CHANGE_REVERB:
	case UI_EVENT_CHANGE_MAX_REVERB:
	{
		Renew_Reverb();
	}
	break;

	case UI_EVENT_FIGHT_MODE:
	{
		Fight_Mode();
	}
	break;

	case UI_EVENT_ENABLE_REVERB:
	{
		Enable_Reverb();
	}
	break;

	default:break;
	}
}

void CUi_PlayerBar::PartnerOff_Both()
{
	m_pPartnerSprite[PARTNER_MACARON]->SetVisible(false);
	m_pPartnerSprite[PARTNER_KORSICA]->SetVisible(false);
}

void CUi_PlayerBar::PartnerOff_Korsica()
{
	m_pPartnerSprite[PARTNER_KORSICA]->SetVisible(false);
}

void CUi_PlayerBar::VisbleReverb()
{
	m_bReverb = true;
}

shared_ptr<CUi_PlayerBar> CUi_PlayerBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerBar
	{
		MakeSharedEnabler(const CUi_PlayerBar& rhs) : CUi_PlayerBar(rhs) { }
	};

	shared_ptr<CUi_PlayerBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
