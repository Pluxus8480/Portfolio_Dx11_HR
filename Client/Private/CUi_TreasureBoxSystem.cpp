#include "CUi_TreasureBoxSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_TreasureBox.h"
#include "iostream"
#include "CUi_CommandIcon.h"
#include "CUi_Default.h"


wstring CUi_TreasureBoxSystem::ObjID = L"CUi_TreasureBoxSystem";

CUi_TreasureBoxSystem::CUi_TreasureBoxSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_TreasureBoxSystem::CUi_TreasureBoxSystem(const CUi_TreasureBoxSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_TreasureBoxSystem::~CUi_TreasureBoxSystem()
{
}

HRESULT CUi_TreasureBoxSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_TreasureBox::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	return S_OK;
}

HRESULT CUi_TreasureBoxSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_TreasureBoxBack();
	Initialize_TreasureBox();
	Initialize_BackGround();
	Initialize_Arrow();
	Initialize_Line();
	

	return S_OK;
}

void CUi_TreasureBoxSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_TreasureBoxSystem::Tick(_float fTimeDelta)
{
	Reduce_Percent(fTimeDelta);
	Pressing_Key();
	Adjust_Pressing(fTimeDelta);
	Reduce_Alpha(fTimeDelta);
	if (m_fPercent > 85)
	{
		Dead();
	}
}

void CUi_TreasureBoxSystem::LateTick(_float fTimeDelta)
{
	Apply_Percent();
	Adjust_LinePos();
}

void CUi_TreasureBoxSystem::Initialize_TreasureBox()
{
	CUi_TreasureBox::Desc Desc;
	Desc.fSizeX = 300;
	Desc.fSizeY = 120;
	Desc.fX = 0;
	Desc.fY = 50;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::TreasureBox;
	Desc.TextureFileName = L"Prototype_Component_Texture_TreasureBox";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBox = static_pointer_cast<CUi_TreasureBox>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_TreasureBox::ObjID, m_iLevelIndex, L"CUi", CUi_TreasureBox::ObjID, &Desc));
	m_pBox->Set_Percent(0);
	m_pBox->Set_TextureNum(2);
}

void CUi_TreasureBoxSystem::Initialize_TreasureBoxBack()
{
	CUi_TreasureBox::Desc Desc;
	Desc.fSizeX = 300;
	Desc.fSizeY = 120;
	Desc.fX = 0;
	Desc.fY = 50;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::TreasureBox;
	Desc.TextureFileName = L"Prototype_Component_Texture_TreasureBox";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBoxBackVec.emplace_back(static_pointer_cast<CUi_TreasureBox>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_TreasureBox::ObjID, m_iLevelIndex, L"CUi", CUi_TreasureBox::ObjID, &Desc)));

	Desc.iPriority = -1;
	m_pBoxBackVec.emplace_back(static_pointer_cast<CUi_TreasureBox>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_TreasureBox::ObjID, m_iLevelIndex, L"CUi", CUi_TreasureBox::ObjID, &Desc)));
	

	m_pBoxBackVec[0]->Set_Percent(50);
	m_pBoxBackVec[1]->Set_Percent(100);
	m_pBoxBackVec[1]->Set_TextureNum(1);
}

void CUi_TreasureBoxSystem::Initialize_BackGround()
{
	_float3 Pos = m_pBoxBackVec[0]->Get_Pos();
	CUi_Default::Desc Desc;
	Desc.fSizeX = 30;
	Desc.fSizeY = 30;
	Desc.fX = Pos.x;
	Desc.fY = Pos.y + 70;
	Desc.fZ = 0.6f;
	Desc.fColor = { 0.3f, 0.7f, 0.8f, 1 };
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 1;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_TreasureBoxArrow";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	Desc.fY = Pos.y - 20;
	m_pBackVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	m_pBackVec[1]->Set_TextureNum(1);
}

void CUi_TreasureBoxSystem::Initialize_Line()
{
	_float3 Pos = m_pBoxBackVec[0]->Get_Pos();
	CUi_Default::Desc Desc;
	Desc.fSizeX = 100;
	Desc.fSizeY = 100;
	Desc.fX = 0;
	Desc.fY = Pos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 1;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.fColor = { 1, 1, 1, 0 };
	Desc.TextureFileName = L"Prototype_Component_Texture_TreasureBoxLine";
	m_pLine = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_TreasureBoxSystem::Initialize_Arrow()
{
	_float3 Pos = m_pBoxBackVec[0]->Get_Pos();
	CUi_Default::Desc Desc;
	Desc.fSizeX = 60;
	Desc.fSizeY = 60;
	Desc.fX = Pos.x;
	Desc.fY = Pos.y - 90;
	m_fArrowPosY = Desc.fY;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 1;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.TextureFileName = L"Prototype_Component_Texture_ButtonArrow";
	m_pArrow = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_TreasureBoxSystem::Pressing_Key()
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
	//{
	//	m_fPercent += 5;
	//	m_pLine->Set_Color(ColorType::A, 1);
	//	if (m_fPercent > 50)
	//	{
	//		m_pBox->Set_TextureNum(3);
	//		m_pBox->Set_Glow(true);
	//	}
	//	else
	//	{
	//		m_pBox->Set_Glow(false);
	//	}

	//	m_bPress = true;
	//	m_fPressTime = 0;
	//}
}

void CUi_TreasureBoxSystem::Reduce_Percent(_float fTimeDelta)
{
	m_fPercent -= fTimeDelta * 10;
	if (m_fPercent < 0)
	{
		m_fPercent = 0;
	}
}

void CUi_TreasureBoxSystem::Reduce_Alpha(_float fTimeDelta)
{
	m_pLine->Add_Color(ColorType::A, -fTimeDelta * 3);
	if (m_pLine->Get_Color().w < 0)
		m_pLine->Set_Color(ColorType::A, 0);
}

void CUi_TreasureBoxSystem::Adjust_LinePos()
{
	_float3 Pos = m_pBoxBackVec[0]->Get_Pos();
	_float3 Scale = m_pBoxBackVec[0]->Get_Scale();

	m_pLine->Set_Pos(Pos.x + Scale.x * (m_fPercent * 0.01f - 0.5f), 
		Pos.y + 30 + Scale.y * -(abs(m_fPercent * 0.01f - 0.5f)));
}

void CUi_TreasureBoxSystem::Apply_Percent()
{
	m_pBox->Set_Percent(m_fPercent);
}

void CUi_TreasureBoxSystem::Dying()
{
	for (auto& iter : m_pBoxBackVec)
		iter->Dead();

	for (auto& iter : m_pBackVec)
		iter->Dead();

	m_pBox->Dead();
	m_pLine->Dead();
	m_pArrow->Dead();

	CUi_CommandIcon::Desc Desc;
	Desc.eRenderType = RenderType::Priority;
	Desc.CorrectBeat = 4;
	Desc.fX = 0;
	Desc.fY = 50;
	CUi_Mgr::Get_Instance()->Add_Clone(CUi_CommandIcon::ObjID, m_iLevelIndex, L"CUi", CUi_CommandIcon::ObjID, &Desc);
}

void CUi_TreasureBoxSystem::Adjust_Pressing(_float fTimeDelta)
{
	if (m_bPress)
	{
		m_fPressTime += fTimeDelta;

		if (m_fPressTime < 0.1f)
		{
			m_pArrow->Add_Pos(0, -fTimeDelta * 700);
		}
		else
		{
			m_pArrow->Add_Pos(0, fTimeDelta * 500);
		}
		if (m_pArrow->Get_Pos().y < m_fArrowPosY - 20)
		{
			m_pArrow->Set_Pos(m_pArrow->Get_Pos().x, m_fArrowPosY - 20);
		}
		else if (m_pArrow->Get_Pos().y > m_fArrowPosY)
		{
			m_pArrow->Set_Pos(m_pArrow->Get_Pos().x, m_fArrowPosY);
		}

		if (m_fPressTime > 0.25f)
		{
			m_pArrow->Set_Pos(m_pArrow->Get_Pos().x, m_fArrowPosY);
			m_bPress = false;
		}
	}
}


shared_ptr<CUi_TreasureBoxSystem> CUi_TreasureBoxSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TreasureBoxSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TreasureBoxSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_TreasureBoxSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TreasureBoxSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TreasureBoxSystem
	{
		MakeSharedEnabler(const CUi_TreasureBoxSystem& rhs) : CUi_TreasureBoxSystem(rhs) { }
	};

	shared_ptr<CUi_TreasureBoxSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
