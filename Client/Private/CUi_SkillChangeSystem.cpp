#include "CUi_SkillChangeSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_DefaultFont.h"
#include "PlayerManager.h"

wstring CUi_SkillChangeSystem::ObjID = L"CUi_SkillChangeSystem";

CUi_SkillChangeSystem::CUi_SkillChangeSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_SkillChangeSystem::CUi_SkillChangeSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_SkillChangeSystem::~CUi_SkillChangeSystem()
{
}

HRESULT CUi_SkillChangeSystem::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SkillChange",
			L"../Bin/Resources/Textures/Ui/SkillChange/SkillChange%d.png", 8));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SkillChangeBar",
			L"../Bin/Resources/Textures/Ui/SkillChange/SkillChangeBar.png"));

	return S_OK;
}

HRESULT CUi_SkillChangeSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_SkillBar();
	Initialize_SkillBack();
	auto pMgr = PLAYERMANAGER;
	Set_Change(pMgr->Get_PlayerEquipSkill(), pMgr->Get_PlayerNonEquipSkill());
	m_fTime = 9999;

	return S_OK;
}

void CUi_SkillChangeSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_SkillChangeSystem::Tick(_float fTimeDelta)
{
	if (m_bAdjustPos)
	{
		Adjust_Pos(fTimeDelta);
	}
	if (m_bAdjustColor)
	{
		Adjust_Color(fTimeDelta);
	}
}

void CUi_SkillChangeSystem::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_fTime += fTimeDelta;
		if (m_fTime >= 3)
		{
			m_pEquipSkillBar->Set_Active(false);
			m_pEquipSkillBack->Set_Active(false);
			m_pNonEquipSkillBar->Set_Active(false);
			m_pNonEquipSkillBack->Set_Active(false);
		}
	}
}

void CUi_SkillChangeSystem::Set_Change(CChai::SkillDesc EquipSkill, CChai::SkillDesc NonEquipSkill)
{
	m_fTime = 0;
	m_bAdjustPos = true;
	m_bAdjustColor = true;
	m_pEquipSkillBar->Set_Pos(m_NoneEquipPos.x, m_NoneEquipPos.y);
	if (EquipSkill.SkillType == CChai::SkillType::Attack)
	{
		m_pEquipSkillBar->Set_TextureNum(EquipSkill.ConsumeReverb - 1);	
	}
	else if(EquipSkill.SkillType == CChai::SkillType::Recover)
	{
		m_pEquipSkillBar->Set_TextureNum(EquipSkill.ConsumeReverb + 3);
	}

	m_pEquipSkillBack->Set_Pos(m_NoneEquipPos.x, m_NoneEquipPos.y);
	m_pEquipSkillBack->Set_FontString(EquipSkill.SkillName);
	m_pEquipSkillBack->Set_Color(m_NonEquipColor);


	if (NonEquipSkill.SkillType == CChai::SkillType::Attack)
	{
		m_pNonEquipSkillBar->Set_TextureNum(NonEquipSkill.ConsumeReverb - 1);	
	}
	else if (NonEquipSkill.SkillType == CChai::SkillType::Recover)
	{
		m_pNonEquipSkillBar->Set_TextureNum(NonEquipSkill.ConsumeReverb + 3);
	}
	m_pNonEquipSkillBar->Set_Pos(m_EquipPos.x, m_EquipPos.y, 0.9f);
	m_pNonEquipSkillBack->Set_Pos(m_EquipPos.x, m_EquipPos.y);
	m_pNonEquipSkillBack->Set_FontString(NonEquipSkill.SkillName);
	m_pNonEquipSkillBack->Set_Color(m_EquipColor);

	m_pEquipSkillBar->Set_Active(true);
	m_pEquipSkillBack->Set_Active(true);
	m_pNonEquipSkillBar->Set_Active(true);
	m_pNonEquipSkillBack->Set_Active(true);
}

void CUi_SkillChangeSystem::Initialize_SkillBar()
{
	CUi_DefaultFont::Desc Desc;
	Desc.fSizeX = 280;
	Desc.fSizeY = 65;
	Desc.fX = m_EquipPos.x;
	Desc.fY = m_EquipPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.fSpeed = 250;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_SkillChange";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pEquipSkillBar = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));

	Desc.fX = m_NoneEquipPos.x;
	Desc.fY = m_NoneEquipPos.y;

	m_pNonEquipSkillBar = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_SkillChangeSystem::Initialize_SkillBack()
{
	CUi_DefaultFont::Desc Desc;
	Desc.fSizeX = 280;
	Desc.fSizeY = 65;
	Desc.fX = m_EquipPos.x;
	Desc.fY = m_EquipPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.fSpeed = 300;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_SkillChangeBar";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pEquipSkillBack = static_pointer_cast<CUi_DefaultFont>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_DefaultFont::ObjID, m_iLevelIndex, L"CUi", CUi_DefaultFont::ObjID, &Desc));

	Desc.fX = m_NoneEquipPos.x;
	Desc.fY = m_NoneEquipPos.y;
	m_pNonEquipSkillBack = static_pointer_cast<CUi_DefaultFont>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_DefaultFont::ObjID, m_iLevelIndex, L"CUi", CUi_DefaultFont::ObjID, &Desc));

	FontDescInfo FontDesc;
	FontDesc.Color = { 1, 1, 1, 1 };
	FontDesc.RelativePos = { -20, 20 };
	FontDesc.Size = { 0.5f };
	FontDesc.String = L"Test";
	m_pEquipSkillBack->Set_FontDesc(FontDesc);
	m_pNonEquipSkillBack->Set_FontDesc(FontDesc);
}

void CUi_SkillChangeSystem::Adjust_Pos(_float fTimeDelta)
{
	m_pEquipSkillBar->Move(MoveDir::Left, fTimeDelta);
	m_pEquipSkillBar->Move(MoveDir::Up, fTimeDelta * 1.4f);

	m_pEquipSkillBack->Move(MoveDir::Left, fTimeDelta);
	m_pEquipSkillBack->Move(MoveDir::Up, fTimeDelta * 1.4f);

	m_pNonEquipSkillBar->Move(MoveDir::Right, fTimeDelta);
	m_pNonEquipSkillBar->Move(MoveDir::Down, fTimeDelta * 1.4f);

	m_pNonEquipSkillBack->Move(MoveDir::Right, fTimeDelta);
	m_pNonEquipSkillBack->Move(MoveDir::Down, fTimeDelta * 1.4f);

	if (m_EquipPos.y <= m_pEquipSkillBar->Get_Pos().y || 
		m_pNonEquipSkillBar->Get_Pos().y <= m_NoneEquipPos.y)
	{
		m_pEquipSkillBar->Set_Pos(m_EquipPos.x, m_EquipPos.y);
		m_pEquipSkillBack->Set_Pos(m_EquipPos.x, m_EquipPos.y);

		m_pNonEquipSkillBar->Set_Pos(m_NoneEquipPos.x, m_NoneEquipPos.y);
		m_pNonEquipSkillBack->Set_Pos(m_NoneEquipPos.x, m_NoneEquipPos.y);

		m_bAdjustPos = false;
	}

}

void CUi_SkillChangeSystem::Adjust_Color(_float fTimeDelta)
{
	_float4 m_EquipColor = { 1, 0.8f, 0, 1 };
	_float4 m_NonEquipColor = { 0.3f, 0.3f, 0.3f, 1 };

	_float4 EquipAddColor = { fTimeDelta * 4, fTimeDelta * 4, -fTimeDelta * 2, 0 };
	m_pEquipSkillBack->Add_Color(EquipAddColor);

	_float4 NonEquipAddColor = { -fTimeDelta * 4, -fTimeDelta * 4, fTimeDelta * 2, 0 };
	m_pNonEquipSkillBack->Add_Color(NonEquipAddColor);

	if (m_pEquipSkillBack->Get_Color().z < 0)
	{
		m_pEquipSkillBack->Set_Color(m_EquipColor);
		m_pNonEquipSkillBack->Set_Color(m_NonEquipColor);
		m_bAdjustColor = false;
	}
}

shared_ptr<CUi_SkillChangeSystem> CUi_SkillChangeSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_SkillChangeSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_SkillChangeSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_SkillChangeSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_SkillChangeSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_SkillChangeSystem
	{
		MakeSharedEnabler(const CUi_SkillChangeSystem& rhs) : CUi_SkillChangeSystem(rhs) { }
	};

	shared_ptr<CUi_SkillChangeSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
