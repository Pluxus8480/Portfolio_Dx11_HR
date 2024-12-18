#include "CUi_KaleSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "CUi_KaleHpBar.h"
#include "Beat_Manager.h"
#include "iostream"
#include "CUi_KaleGroggyGage.h"

wstring CUi_KaleSystem::ObjID = L"CUi_KaleSystem";

CUi_KaleSystem::CUi_KaleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_KaleSystem::CUi_KaleSystem(const CUi_KaleSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_KaleSystem::~CUi_KaleSystem()
{
}

HRESULT CUi_KaleSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_KaleHpBar::ObjID);
 	m_List_Owning_ObjectID.push_back(CUi_KaleGroggyGage::ObjID);

	
	return S_OK;
}

HRESULT CUi_KaleSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_HpBar();
	Initialize_Name();
	Initialize_GroggyGage();

	return S_OK;
}

void CUi_KaleSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_KaleSystem::Tick(_float fTimeDelta)
{
	Normalize_DeltaTime(fTimeDelta);
	Renew_KaleHp(fTimeDelta);
	Renew_KaleGroggyGauge(fTimeDelta);
	Move_Shadow(fTimeDelta);
}

void CUi_KaleSystem::LateTick(_float fTimeDelta)
{
}

void CUi_KaleSystem::Set_KaleGroggy(_int* Groggy)
{
	m_pGroggyGage = Groggy;
	m_pMaxGroggyGage = *m_pGroggyGage;
}

void CUi_KaleSystem::Initialize_HpBar()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 90 * 15;
	Desc.fSizeY = 90;
	Desc.fX = m_OriginPos.x;
	Desc.fY = m_OriginPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::RealBackGround;
	Desc.iPriority = 3;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_KaleHpBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pHpBackBar = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));


	Desc.eRenderType = RenderType::BackGround;
	Desc.ShaderPass = ShaderPass::BossHp;
	Desc.TextureFileName = L"Prototype_Component_Texture_KaleHp";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pHpBar = static_pointer_cast<CUi_KaleHpBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_KaleHpBar::ObjID, m_iLevelIndex, L"CUi", CUi_KaleHpBar::ObjID, &Desc));
	m_pHpBar->Set_TestName(L"CUi_KaleHpBar");

	m_OriginShadowPos.x = m_OriginPos.x;
	m_OriginShadowPos.y = m_OriginPos.y - 10;
	Desc.fY = m_OriginShadowPos.y;
	Desc.ShaderPass = ShaderPass::Default;
	Desc.eRenderType = RenderType::Lowest;
	Desc.TextureFileName = L"Prototype_Component_Texture_KaleHpShadow";
	Desc.fSpeed = 80;
	m_pHpShadow = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_KaleSystem::Initialize_Name()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 160;
	Desc.fSizeY = 60;
	Desc.fX = 0;
	Desc.fY = m_OriginPos.y - 30;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 5;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_KaleName";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pName = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pName->Set_TestName(L"CUi_KaleName");
}

void CUi_KaleSystem::Initialize_GroggyGage()
{
	CUi_2D::Desc Desc;
	Desc.fSizeX = 90 * 5;
	Desc.fSizeY = 30;
	Desc.fX = m_OriginPos.x;
	Desc.fY = m_OriginPos.y + 50;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Lowest;
	Desc.iPriority = 0;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_KaleStunGaugeBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pGroggyGaugeBackBar = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));


	Desc.iPriority = 10;
	Desc.eRenderType = RenderType::RealBackGround;
	Desc.ShaderPass = ShaderPass::FromLeft;
	Desc.TextureFileName = L"Prototype_Component_Texture_KaleGroggy";
	m_pGroggyGaugeBar = static_pointer_cast<CUi_KaleGroggyGage>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_KaleGroggyGage::ObjID, m_iLevelIndex, L"CUi", CUi_KaleGroggyGage::ObjID, &Desc));
}

void CUi_KaleSystem::Move_Shadow(_float fTimeDelta)
{
	_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
	_float Timer = CBeat_Manager::Get_Instance()->Get_Timer();

	if (0.66f < BeatPercent())
	{
		m_pHpShadow->Move(MoveDir::Down, fTimeDelta * 2.f);
		if (m_pHpShadow->Get_Pos().y < m_OriginShadowPos.y - 25)
		{
			m_pHpShadow->Set_Pos(m_pHpShadow->Get_Pos().x, m_OriginShadowPos.y - 25);
		}
	}
	else
	{
		m_pHpShadow->Move(MoveDir::Up, fTimeDelta);
		if (m_OriginShadowPos.y< m_pHpShadow->Get_Pos().y)
		{
			m_pHpShadow->Set_Pos(m_pHpShadow->Get_Pos().x, m_OriginShadowPos.y);
		}
	}
}

void CUi_KaleSystem::Renew_KaleHp(_float fTimeDelta)
{
	_float Percent = *m_pKaleHp / *m_pKaleMaxHp;
	if (m_fPreviousKalePercent != 1 - Percent)
	{
		m_fPreviousKalePercent = 1 - Percent;
		m_pHpBar->Set_Percent(m_fPreviousKalePercent);
		m_pHpBar->Set_Damaged(true);
	}
}

void CUi_KaleSystem::Renew_KaleGroggyGauge(_float fTimeDelta)
{
	_float GroggyPercent = *m_pGroggyGage /(_float)m_pMaxGroggyGage;
	m_pGroggyGaugeBar->Set_Percent(GroggyPercent);
	if (GroggyPercent == 0 && m_bGroggyActive)
	{
		m_bGroggyActive = false;
		m_pGroggyGaugeBackBar->Set_OwnActive(false);
	}
	else if (!m_bGroggyActive && GroggyPercent != 0)
	{
		m_bGroggyActive = true;
		m_pGroggyGaugeBackBar->Set_OwnActive(true);
	}
}

void CUi_KaleSystem::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pHpBar->Set_Active(Active);
	m_pHpBackBar->Set_Active(Active);
	m_pGroggyGaugeBackBar->Set_Active(Active);
	m_pGroggyGaugeBar->Set_Active(Active);
	m_pHpShadow->Set_Active(Active);
	m_pName->Set_Active(Active);
}

void CUi_KaleSystem::Set_Phase3()
{
	m_pHpBackBar->Set_TextureNum(1);
}

void CUi_KaleSystem::Dying()
{
	m_pHpBar->Dead();
	m_pHpBackBar->Dead();
	m_pGroggyGaugeBackBar->Dead();
	m_pGroggyGaugeBar->Dead();
	m_pHpShadow->Dead();
	m_pName->Dead();
}

shared_ptr<CUi_KaleSystem> CUi_KaleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_KaleSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_KaleSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_KaleSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_KaleSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_KaleSystem
	{
		MakeSharedEnabler(const CUi_KaleSystem& rhs) : CUi_KaleSystem(rhs) { }
	};

	shared_ptr<CUi_KaleSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
