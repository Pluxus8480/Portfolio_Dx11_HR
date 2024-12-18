#include "CUi_ScoreSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "CUi_Default.h"


wstring CUi_ScoreSystem::ObjID = L"CUi_ScoreSystem";

CUi_ScoreSystem::CUi_ScoreSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_ScoreSystem::CUi_ScoreSystem(const CUi_ScoreSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_ScoreSystem::~CUi_ScoreSystem()
{
}

HRESULT CUi_ScoreSystem::Initialize_Prototype()
{
	if (FAILED(m_pGameInstance.lock()->Add_Font
	(L"Nexon", L"../Bin/Resources/Textures/Ui/Font/Nexon.spritefont")))
		assert(false);


	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);


	return S_OK;
}

HRESULT CUi_ScoreSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_ScoreBack();
	Initialize_Font();

	return S_OK;
}

void CUi_ScoreSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_ScoreSystem::Tick(_float fTimeDelta)
{
	Check_PlayerScore();

	if (m_bChange)
	{
		StringMove(fTimeDelta);
	}
}

void CUi_ScoreSystem::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_ScoreSystem::Render()
{
	RenderFont(m_FontDesc);
	RenderFont(m_StringFontDesc);

	return S_OK;
}

void CUi_ScoreSystem::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pScoreBack->Set_Active(Active);
}

void CUi_ScoreSystem::Check_PlayerScore()
{
	_uint Score = PLAYERMANAGER->Get_PlayerScore();
	if (m_iPrevScore != Score)
	{
		m_iPrevScore = Score;
		m_bChange = true;
		m_FontDesc.String = to_wstring(Score);
		m_FontDesc.DirectPos.x = m_fOriginPos.x;
		m_FontDesc.DirectPos.x -= m_FontDesc.String.size() * 15;
	}
}

void CUi_ScoreSystem::StringMove(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	if (m_fMoveTime <= 0.04f)
	{
		m_FontDesc.DirectPos.y += m_fSpeed * fTimeDelta;
	}
	else if (0.04f < m_fMoveTime && m_fMoveTime < 0.08f)
	{
		m_FontDesc.DirectPos.y -= m_fSpeed * fTimeDelta;
	}
	else
	{
		m_fMoveTime = 0;
		m_FontDesc.DirectPos.y = m_fOriginPos.y;
		m_bChange = false;
	}
}

void CUi_ScoreSystem::Initialize_ScoreBack()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 200;
	Desc.fSizeY = 25;
	Desc.fX = m_fOriginPos.x - 20;
	Desc.fY = m_fOriginPos.y + 30;
	Desc.fZ = 0.9f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ScoreBack";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	m_pScoreBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_ScoreSystem::Dying()
{
	if (m_pScoreBack)
		m_pScoreBack->Dead();
}

void CUi_ScoreSystem::Initialize_Font()
{
	m_StringFontDesc.String = L"Á¡¼ö";
	m_StringFontDesc.DirectPos.x = m_fOriginPos.x - 90;
	m_StringFontDesc.DirectPos.y = m_fOriginPos.y + 40;
	m_StringFontDesc.Size = 0.3f;
	m_StringFontDesc.Color = { 1, 1, 0, 1 };
	m_FontDesc.DirectPos.y = m_fOriginPos.y;
	m_FontDesc.DirectPos.x = m_fOriginPos.x;
}

shared_ptr<CUi_ScoreSystem> CUi_ScoreSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_ScoreSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_ScoreSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_ScoreSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_ScoreSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_ScoreSystem
	{
		MakeSharedEnabler(const CUi_ScoreSystem& rhs) : CUi_ScoreSystem(rhs) { }
	};

	shared_ptr<CUi_ScoreSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
