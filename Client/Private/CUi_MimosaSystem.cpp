#include "CUi_MimosaSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "iostream"
#include "CUi_MimosaBar.h"

wstring CUi_MimosaSystem::ObjID = L"CUi_MimosaSystem";

CUi_MimosaSystem::CUi_MimosaSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_MimosaSystem::CUi_MimosaSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_MimosaSystem::~CUi_MimosaSystem()
{
}

HRESULT CUi_MimosaSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_MimosaBar::ObjID);
	

	return S_OK;
}

HRESULT CUi_MimosaSystem::Initialize(void* pArg)
{
	m_DecoVec.reserve(5);
	Initialize_Transform();
	Initialize_Deco();
	Initialize_HpBar();
	Initialize_Groggy();


	return S_OK;
}

void CUi_MimosaSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_MimosaSystem::Tick(_float fTimeDelta)
{
	Renew_Hp(fTimeDelta);
	Renew_Groggy();
}

void CUi_MimosaSystem::LateTick(_float fTimeDelta)
{
}

void CUi_MimosaSystem::Set_Hp(_float* Hp)
{
	m_pCurHp = Hp;
	m_fMaxHp = *m_pCurHp;
}

void CUi_MimosaSystem::Set_Groggy(_float* Groggy)
{
	m_pGroggy = Groggy;
	m_fMaxGroggy = *m_pGroggy;
}

void CUi_MimosaSystem::Renew_Hp(_float fTimeDelta)
{
	m_fDmgTime += fTimeDelta;
	_float Percent = *m_pCurHp / m_fMaxHp;
	_float DeletePercent = { 0 };
	if (m_fPreviousPercent != Percent)
	{
		m_fDmgTime = 0;
		m_fPreviousPercent = Percent;
		if (Percent > 0.66f)
		{
			DeletePercent = 1 - Percent;
			m_HealthBarVec[0]->Set_Percent(DeletePercent * 3.f);
		}
		else if (Percent > 0.33f)
		{
			m_HealthBarBackVec[0]->Set_TextureNum(3);
			DeletePercent = 0.66f - Percent;
			m_HealthBarVec[0]->Set_Percent(1);
			m_HealthBarVec[1]->Set_Percent(DeletePercent * 3.f);
		}
		else if (Percent > 0.01f)
		{
			m_HealthBarBackVec[1]->Set_TextureNum(4);
			DeletePercent = 0.33f - Percent;
			m_HealthBarVec[1]->Set_Percent(1);
			m_HealthBarVec[2]->Set_Percent(DeletePercent * 3.f);
		}
		else
		{
			for (_uint i = 0; i < (_uint)m_HealthBarBackVec.size(); ++i)
			{
				m_HealthBarBackVec[i]->Set_TextureNum(i+3);
			}
			for (auto& iter : m_HealthBarVec)
				iter->Set_Percent(1);
		}
	}

	if (m_fDmgTime > 0.4f)
	{
		for(auto& iter : m_HealthBarVec)
			iter->Set_Damaged(false);
	}
	else
	{
		for (auto& iter : m_HealthBarVec)
			iter->Set_Damaged(true);
	}
}

void CUi_MimosaSystem::Renew_Groggy()
{
	_float Percent = *m_pGroggy / m_fMaxGroggy;
	m_GroggyGaugeVec[0]->Set_Percent(1 - Percent);
	m_GroggyGaugeVec[1]->Set_Percent(Percent);
}

void CUi_MimosaSystem::Dying()
{
}

void CUi_MimosaSystem::Set_Active(_bool Active)
{
}

void CUi_MimosaSystem::Initialize_Deco()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 200;
	Desc.fSizeY = 50;
	Desc.fX = 0;
	Desc.fY = -320;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 5;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaName";
	Desc.TextureLevel = LEVEL_STATIC;
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fSizeX = 75;
	Desc.fSizeY = 75;
	Desc.fX = -360;
	Desc.fY = -290;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaDeco";
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fX = 360;
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	m_DecoVec[2]->Set_TextureNum(1);
}

void CUi_MimosaSystem::Initialize_HpBar()
{
	CUi_2D::Desc Desc;
	Desc.fSizeX = 240;
	Desc.fSizeY = 60;
	Desc.fY = -290;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::BossHp;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaHealthBar";
	Desc.TextureLevel = LEVEL_STATIC;
	for (_int i = 0; i < 3; ++i)
	{
		Desc.fX = -240 + i * 240;
		m_HealthBarVec.emplace_back(static_pointer_cast<CUi_MimosaBar>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_MimosaBar::ObjID, m_iLevelIndex, L"CUi", CUi_MimosaBar::ObjID, &Desc)));
		m_HealthBarVec[i]->Set_TextureNum(i);
	}

	Desc.eRenderType = RenderType::RealBackGround;
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaHealthBarBack";
	for (_int i = 0; i < 3; ++i)
	{
		Desc.fX = -240 + i * 240;
		m_HealthBarBackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
		m_HealthBarBackVec[i]->Set_TextureNum(i);
	}
	
}

void CUi_MimosaSystem::Initialize_Groggy()
{
	CUi_2D::Desc Desc;
	Desc.fSizeX = 300;
	Desc.fSizeY = 20;
	Desc.fX = -200;
	Desc.fY = -330;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaGroggyGaugeBg";
	Desc.TextureLevel = LEVEL_STATIC;
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fX = 200;
	m_DecoVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));


	Desc.eRenderType = RenderType::BackGround;
	Desc.fSizeX -= 10;
	Desc.fSizeY -= 5;
	Desc.fX = -200;
	Desc.ShaderPass = ShaderPass::FromLeft;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaGroggyGauge";
	m_GroggyGaugeVec.emplace_back(static_pointer_cast<CUi_MimosaBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_MimosaBar::ObjID, m_iLevelIndex, L"CUi", CUi_MimosaBar::ObjID, &Desc)));

	Desc.fX = 200;
	Desc.ShaderPass = ShaderPass::FromRight;
	m_GroggyGaugeVec.emplace_back(static_pointer_cast<CUi_MimosaBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_MimosaBar::ObjID, m_iLevelIndex, L"CUi", CUi_MimosaBar::ObjID, &Desc)));
}

shared_ptr<CUi_MimosaSystem> CUi_MimosaSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_MimosaSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_MimosaSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_MimosaSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_MimosaSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_MimosaSystem
	{
		MakeSharedEnabler(const CUi_MimosaSystem& rhs) : CUi_MimosaSystem(rhs) { }
	};

	shared_ptr<CUi_MimosaSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
