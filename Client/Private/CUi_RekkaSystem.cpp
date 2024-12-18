#include "CUi_RekkaSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "Monster_Rekka.h"
#include "iostream"
#include "CUi_RekkaHp.h"
#include "CUi_RekkaDeco.h"


wstring CUi_RekkaSystem::ObjID = L"CUi_RekkaSystem";

CUi_RekkaSystem::CUi_RekkaSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_RekkaSystem::CUi_RekkaSystem(const CUi_RekkaSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_RekkaSystem::~CUi_RekkaSystem()
{
}

HRESULT CUi_RekkaSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_RekkaHp::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_RekkaDeco::ObjID);
	

	return S_OK;
}

HRESULT CUi_RekkaSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_Hp();
	Initialize_HpBack();
	Initialize_Deco();


	return S_OK;
}

void CUi_RekkaSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_RekkaSystem::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Check_RekkaBuff();
		Renew_RekkaHp();
		RekkaDamaged(fTimeDelta);
	}
}

void CUi_RekkaSystem::LateTick(_float fTimeDelta)
{
	
}

void CUi_RekkaSystem::Initialize_Hp()
{
	CUi_RekkaHp::Desc Desc;
	Desc.fSizeX = m_fHpSize.x;
	Desc.fSizeY = m_fHpSize.y;
	Desc.fY = -230;
	Desc.fZ = 0.6f;
	Desc.fColor = { 1, 1, 1, 1 };
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 1;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::BossHp;
	Desc.TextureFileName = L"Prototype_Component_Texture_RekkaHp";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pHpVec.reserve(4);
	for (_uint i = 0; i < 4; ++i)
	{
		Desc.fX = (-1.5f + i) * m_fHpSize.x;
		m_pHpVec.emplace_back(static_pointer_cast<CUi_RekkaHp>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_RekkaHp::ObjID, m_iLevelIndex, L"CUi", CUi_RekkaHp::ObjID, &Desc)));
	}
	for (_uint i = 0; i < 4; ++i)
	{
		m_pHpVec[i]->Set_TextureNum(i * 2);
	}
}

void CUi_RekkaSystem::Initialize_HpBack()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = m_fHpSize.x;
	Desc.fSizeY = m_fHpSize.y;
	Desc.fY = -230;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Lowest;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_RekkaHpBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pHpBackVec.reserve(4);
	for (_uint i = 0; i < 4; ++i)
	{
		Desc.fX = (-1.5f + i) * m_fHpSize.x;
		m_pHpBackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	}
	for (_uint i = 0; i < 4; ++i)
	{
		m_pHpBackVec[i]->Set_TextureNum(i*2);
	}
}

void CUi_RekkaSystem::Initialize_Deco()
{
	CUi_RekkaDeco::Desc Desc;
	Desc.fSizeX = m_fHpSize.x * 5.f;
	Desc.fSizeY = Desc.fSizeX * 0.25f;
	Desc.fX = 0;
	Desc.fY = -210;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 2;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::RekkaDeco;
	Desc.TextureFileName = L"Prototype_Component_Texture_RekkaDeco";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pDeco = static_pointer_cast<CUi_RekkaDeco>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_RekkaDeco::ObjID, m_iLevelIndex, L"CUi", CUi_RekkaDeco::ObjID, &Desc));
}

void CUi_RekkaSystem::Renew_RekkaHp()
{
	_float RekkaMaxHp = m_pRekka->Get_MaxHp();
	_float RekkaCurHp = m_pRekka->Get_Hp();
	_float Percent = RekkaCurHp / RekkaMaxHp;
 
	_float DeletePercent = { 0 };
	if (m_fPreviousRekkaPercent != Percent)
	{
		if (Percent > 0.75f)
		{
			DeletePercent = 1 - Percent;
			m_pHpVec[0]->Set_DeletePercent(DeletePercent * 4.f);
		}
		else if (Percent > 0.5f)
		{
			m_pHpBackVec[0]->Set_TextureNum(1);
			DeletePercent = 0.75f - Percent;
			m_pHpVec[0]->Set_DeletePercent(100);
			m_pHpVec[1]->Set_DeletePercent(DeletePercent * 4.f);
		}
		else if (Percent > 0.25f)
		{
			m_pHpBackVec[0]->Set_TextureNum(1);
			m_pHpBackVec[1]->Set_TextureNum(3);
			DeletePercent = 0.5f - Percent;
			m_pHpVec[0]->Set_DeletePercent(100);
			m_pHpVec[1]->Set_DeletePercent(100);
			m_pHpVec[2]->Set_DeletePercent(DeletePercent * 4.f);
		}
		else if (Percent > 0.f)
		{
			m_pHpBackVec[0]->Set_TextureNum(1);
			m_pHpBackVec[1]->Set_TextureNum(3);
			m_pHpBackVec[2]->Set_TextureNum(5);
			DeletePercent = 0.25f - Percent;
			m_pHpVec[0]->Set_DeletePercent(100);
			m_pHpVec[1]->Set_DeletePercent(100);
			m_pHpVec[2]->Set_DeletePercent(100);
			m_pHpVec[3]->Set_DeletePercent(DeletePercent * 4.f);
		}
		else
		{
			m_pHpBackVec[0]->Set_TextureNum(1);
			m_pHpBackVec[1]->Set_TextureNum(3);
			m_pHpBackVec[2]->Set_TextureNum(5);
			m_pHpBackVec[3]->Set_TextureNum(7);

			m_pHpVec[0]->Set_DeletePercent(100);
			m_pHpVec[1]->Set_DeletePercent(100);
			m_pHpVec[2]->Set_DeletePercent(100);
			m_pHpVec[3]->Set_DeletePercent(100);
		}
		m_fPreviousRekkaPercent = Percent;
		m_bDamaged = true;
	}
}

void CUi_RekkaSystem::RekkaDamaged(_float fTimeDelta)
{
	if (m_bDamaged)
	{
		m_fDamagedTime += fTimeDelta;
		if (m_fDamagedTime < 0.15f)
		{
			for (size_t i = 0; i < m_pHpVec.size(); ++i)
			{
				m_pHpVec[i]->Set_Damaged(true);
			}
		}
		else
		{
			m_bDamaged = false;
			for (size_t i = 0; i < m_pHpVec.size(); ++i)
			{
				m_pHpVec[i]->Set_Damaged(false);
			}
			m_fDamagedTime = 0;
		}
	}
}

void CUi_RekkaSystem::Check_RekkaBuff()
{
	m_bRekkaBuff = m_pRekka->Get_Buff();
	m_pDeco->Set_Buff(m_bRekkaBuff);
}

void CUi_RekkaSystem::Dying()
{
	for (auto& iter : m_pHpVec)
		iter->Dead();

	for (auto& iter : m_pHpBackVec)
		iter->Dead();

	m_pDeco->Dead();

	m_pRekka = nullptr;
}

void CUi_RekkaSystem::Set_Active(_bool Active)
{
	m_bActive = Active;

	for (auto& iter : m_pHpVec)
		iter->Set_Active(Active);

	for (auto& iter : m_pHpBackVec)
		iter->Set_Active(Active);

	m_pDeco->Set_Active(Active);
}

shared_ptr<CUi_RekkaSystem> CUi_RekkaSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_RekkaSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_RekkaSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_RekkaSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_RekkaSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_RekkaSystem
	{
		MakeSharedEnabler(const CUi_RekkaSystem& rhs) : CUi_RekkaSystem(rhs) { }
	};

	shared_ptr<CUi_RekkaSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
