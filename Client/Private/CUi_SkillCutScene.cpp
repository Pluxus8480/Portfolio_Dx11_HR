#include "CUi_SkillCutScene.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"


wstring CUi_SkillCutScene::ObjID = L"CUi_SkillCutScene";

CUi_SkillCutScene::CUi_SkillCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_SkillCutScene::CUi_SkillCutScene(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_SkillCutScene::~CUi_SkillCutScene()
{
}

HRESULT CUi_SkillCutScene::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	return S_OK;
}

HRESULT CUi_SkillCutScene::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	Initialize_CutScene(Arg->CutScenetype);
	Initialize_Back();
	Initialize_Word(Arg->Wordtype);

	return S_OK;
}

void CUi_SkillCutScene::PriorityTick(_float fTimeDelta)
{
}

void CUi_SkillCutScene::Tick(_float fTimeDelta)
{
	m_fDieTime += fTimeDelta;
	if (m_fDieTime > 3)
	{
		m_pCutScene->Dead();
		m_pBack->Dead();
		m_pWord->Dead();
		Dead();
	}
	Moving(fTimeDelta);
}

void CUi_SkillCutScene::LateTick(_float fTimeDelta)
{
	_float3 Pos = m_pCutScene->Get_Pos();
	m_pWord->Set_Pos(Pos.x - 200, Pos.y - 150, 0.1f);

	m_pBack->Set_Pos(Pos.x, Pos.y - 100, 0.3f);
}

void CUi_SkillCutScene::Initialize_Back()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 800;
	Desc.fSizeY = 400;
	Desc.fX = 2000;
	Desc.fY = 2000;
	Desc.fZ = 0.3f;
	Desc.eRenderType = RenderType::RealBackGround;
	Desc.iPriority = 7;
	Desc.fSpeed = 1000;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_CutSceneBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_SkillCutScene::Initialize_CutScene(CutSceneType Scene)
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = g_iWinSizeX * 0.3f;
	Desc.fSizeY = g_iWinSizeY * 0.7f;
	Desc.fX = m_fOriginPos.x;
	Desc.fY = m_fOriginPos.y;
	Desc.fZ = 0.2f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 8;
	Desc.fSpeed = 3000;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_CUtScene";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pCutScene =static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pCutScene->Set_TextureNum((_uint)Scene);
}

void CUi_SkillCutScene::Initialize_Word(WordType Word)
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 500;
	Desc.fSizeY = 200;
	Desc.fX = 2000;
	Desc.fY = 2000;
	Desc.fZ = 0.1f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 10;
	Desc.fSpeed = 1000;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_SkillWord";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pWord = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pWord->Set_TextureNum((_uint)Word);
}

void CUi_SkillCutScene::Moving(_float fTimeDelta)
{
	m_pCutScene->Move(MoveDir::Left, fTimeDelta);
	m_pCutScene->Move(MoveDir::Down, fTimeDelta * 0.5f);
	if (m_pCutScene->Get_Pos().x < m_fDestPos.x)
	{
		m_pCutScene->Set_Pos(m_fDestPos.x, m_fDestPos.y, 0.2f);
	}
}

void CUi_SkillCutScene::Dying()
{
}

shared_ptr<CUi_SkillCutScene> CUi_SkillCutScene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_SkillCutScene
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_SkillCutScene(pDevice, pContext) { }
	};

	shared_ptr<CUi_SkillCutScene> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_SkillCutScene::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_SkillCutScene
	{
		MakeSharedEnabler(const CUi_SkillCutScene& rhs) : CUi_SkillCutScene(rhs) { }
	};

	shared_ptr<CUi_SkillCutScene> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
