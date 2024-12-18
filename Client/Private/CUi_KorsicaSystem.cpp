#include "CUi_KorsicaSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "iostream"
#include "CUi_KorsicaFace.h"
#include "CUi_KorsicaHpBar.h"
#include "PlayerManager.h"
#include "Beat_Manager.h"


wstring CUi_KorsicaSystem::ObjID = L"CUi_KorsicaSystem";

CUi_KorsicaSystem::CUi_KorsicaSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_KorsicaSystem::CUi_KorsicaSystem(const CUi_KorsicaSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_KorsicaSystem::~CUi_KorsicaSystem()
{
}

HRESULT CUi_KorsicaSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_KorsicaFace::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_KorsicaHpBar::ObjID);
	

	return S_OK;
}

HRESULT CUi_KorsicaSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	m_fKorsicaHp = Arg->CorsicaHp;
	Initialize_FaceVec();
	Initialize_HpBack();
	Initialize_Hp();
	Initialize_Shadow();


	return S_OK;
}

void CUi_KorsicaSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_KorsicaSystem::Tick(_float fTimeDelta)
{
	_float KorsicaHpPercent = *m_fKorsicaHp / m_fKorsicaMaxHp;
	m_pKorsicaHp->Set_HpPercent(KorsicaHpPercent);
	Renew_Hp();
	Shadow_Move(fTimeDelta);	
}

void CUi_KorsicaSystem::LateTick(_float fTimeDelta)
{

}

void CUi_KorsicaSystem::Initialize_FaceVec()
{
	CUi_KorsicaFace::Desc Desc;
	Desc.fSizeX = 200;
	Desc.fSizeY = 150;
	Desc.fX = -510;
	Desc.fY = 260;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::KorsicaFace;
	Desc.TextureFileName = L"Prototype_Component_Texture_ChaiFace";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.BackTextureNum = 0;
	m_pFaceVec.emplace_back(static_pointer_cast<CUi_KorsicaFace>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_KorsicaFace::ObjID, m_iLevelIndex, L"CUi", CUi_KorsicaFace::ObjID, &Desc)));
	

	Desc.fX = 510;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.BackTextureNum = 1;
	Desc.TextureFileName = L"Prototype_Component_Texture_KorsicaFace";
	m_pFaceVec.emplace_back(static_pointer_cast<CUi_KorsicaFace>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_KorsicaFace::ObjID, m_iLevelIndex, L"CUi", CUi_KorsicaFace::ObjID, &Desc)));
}

void CUi_KorsicaSystem::Initialize_HpBack()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 400;
	Desc.fSizeY = 85;
	Desc.fX = -315;
	Desc.fY = 270;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ChaiHpBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pChaiHpBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pChaiHpBack->Rotation(XMConvertToRadians(7));


	Desc.fX = 315;
	Desc.TextureFileName = L"Prototype_Component_Texture_KorsicaHpBack";
	m_pKorsicaHpBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pKorsicaHpBack->Rotation(XMConvertToRadians(-7));
}

void CUi_KorsicaSystem::Initialize_Hp()
{
	CUi_KorsicaHpBar::Desc Desc;
	Desc.fSizeX = 320;
	Desc.fSizeY = 65;
	Desc.fX = -340;
	Desc.fY = 260;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::KorsicaHpBar;
	Desc.TextureFileName = L"Prototype_Component_Texture_ChaiHp";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.bKorsica = false;
	m_pChaiHp = static_pointer_cast<CUi_KorsicaHpBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_KorsicaHpBar::ObjID, m_iLevelIndex, L"CUi", CUi_KorsicaHpBar::ObjID, &Desc));
	m_pChaiHp->Rotation(XMConvertToRadians(7));


	Desc.fX = 340;
	Desc.fY = 260;
	Desc.bKorsica = true;
	Desc.TextureFileName = L"Prototype_Component_Texture_KorsicaHp";
	m_pKorsicaHp = static_pointer_cast<CUi_KorsicaHpBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_KorsicaHpBar::ObjID, m_iLevelIndex, L"CUi", CUi_KorsicaHpBar::ObjID, &Desc));
	m_pKorsicaHp->Rotation(XMConvertToRadians(-7));
}

void CUi_KorsicaSystem::Initialize_Shadow()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 470;
	Desc.fSizeY = 90;
	Desc.fX = -365;
	Desc.fY = 240;
	Desc.fZ = 0.6f;
	Desc.fSpeed = 50;
	Desc.fColor = { 1, 1, 1, 0.5f };
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = -5;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_ChaiHpShadow";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pChaiShadow = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));

	m_pChaiShadow->Rotation(XMConvertToRadians(5));


	Desc.fX = 365;
	Desc.fY = 240;
	Desc.TextureFileName = L"Prototype_Component_Texture_KorsicaHpShadow";
	m_pKorsicaShadow = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pKorsicaShadow->Rotation(XMConvertToRadians(-5));
}

void CUi_KorsicaSystem::Renew_Hp()
{
	_float ChaiHp = PLAYERMANAGER->Get_Hp();
	_float ChaiMaxHp = PLAYERMANAGER->Get_MaxHp();
	_float ChaiHpPercent = ChaiHp / ChaiMaxHp;
	m_pChaiHp->Set_HpPercent(ChaiHpPercent);
}

void CUi_KorsicaSystem::Shadow_Move(_float fTimeDelta)
{
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
	_float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
	_float fQuarterBeat = fBeat / 4.f;

	if (fBeat - fTimer < fQuarterBeat)
	{
		m_pKorsicaShadow->Set_Speed(120);
		m_pChaiShadow->Set_Speed(120);

		m_pKorsicaShadow->Move(MoveDir::Down, fTimeDelta);
		m_pChaiShadow->Move(MoveDir::Down, fTimeDelta);
		if (m_pChaiShadow->Get_Pos().y < m_fShadowOriginY - 120 * fQuarterBeat)
		{
			m_pKorsicaShadow->Set_Pos(m_pKorsicaShadow->Get_Pos().x, m_fShadowOriginY - 120 * fQuarterBeat);
			m_pChaiShadow->Set_Pos(m_pChaiShadow->Get_Pos().x, m_fShadowOriginY - 120 * fQuarterBeat);
		}
	}
	else
	{
		m_pKorsicaShadow->Set_Speed(40);
		m_pChaiShadow->Set_Speed(40);

		m_pKorsicaShadow->Move(MoveDir::Up, fTimeDelta);
		m_pChaiShadow->Move(MoveDir::Up, fTimeDelta);

		if (m_fShadowOriginY < m_pChaiShadow->Get_Pos().y)
		{
			m_pKorsicaShadow->Set_Pos(m_pKorsicaShadow->Get_Pos().x, m_fShadowOriginY);
			m_pChaiShadow->Set_Pos(m_pChaiShadow->Get_Pos().x, m_fShadowOriginY);
		}
	}
}

void CUi_KorsicaSystem::Dying()
{
	for (auto& iter : m_pFaceVec)
		iter->Dead();

	m_pChaiHpBack->Dead();
	m_pKorsicaHpBack->Dead();
	m_pChaiHp->Dead();
	m_pKorsicaHp->Dead();
	m_pKorsicaShadow->Dead();
	m_pChaiShadow->Dead();
}

void CUi_KorsicaSystem::Set_Active(_bool Active)
{
	m_bActive = Active;

	for (auto& iter : m_pFaceVec)
		iter->Set_Active(Active);

	m_pChaiHpBack->Set_Active(Active);
	m_pKorsicaHpBack->Set_Active(Active);
	m_pChaiHp->Set_Active(Active);
	m_pKorsicaHp->Set_Active(Active);
	m_pKorsicaShadow->Set_Active(Active);
	m_pChaiShadow->Set_Active(Active);
}

shared_ptr<CUi_KorsicaSystem> CUi_KorsicaSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_KorsicaSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_KorsicaSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_KorsicaSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_KorsicaSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_KorsicaSystem
	{
		MakeSharedEnabler(const CUi_KorsicaSystem& rhs) : CUi_KorsicaSystem(rhs) { }
	};

	shared_ptr<CUi_KorsicaSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
