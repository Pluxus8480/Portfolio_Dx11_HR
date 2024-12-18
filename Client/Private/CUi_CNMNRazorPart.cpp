#include "CUi_CNMNRazorPart.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "CUi_DefaultBar.h"
#include "CUi_CommandIcon.h"
#include "Beat_Manager.h"
#include "CUi_CNMNRazer.h"
#include "iostream"


wstring CUi_CNMNRazorPart::ObjID = L"CUi_CNMNRazorPart";

CUi_CNMNRazorPart::CUi_CNMNRazorPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_CNMNRazorPart::CUi_CNMNRazorPart(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_CNMNRazorPart::~CUi_CNMNRazorPart()
{
}

HRESULT CUi_CNMNRazorPart::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_CNMNRazor",
			L"../Bin/Resources/Textures/Ui/CNMNRazor/CNMNRazor%d.png", 2));

	return S_OK;
}

HRESULT CUi_CNMNRazorPart::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
	m_fBeatDuration = Arg->BeatDuration;
	m_fTimeDuration = m_fBeatDuration * Beat;
	m_fWholeTime = m_fTimeDuration;
	m_iKeyInput = Arg->KeyInput;
	Initialize_Bar(Arg->Pos);
	Initialize_Icon(Arg->KeyInput, Arg->Pos);

	return S_OK;
}

void CUi_CNMNRazorPart::PriorityTick(_float fTimeDelta)
{
}

void CUi_CNMNRazorPart::Tick(_float fTimeDelta)
{
	if (m_bJudgeStart)
	{
		KeyPress(fTimeDelta);
	}
}

void CUi_CNMNRazorPart::LateTick(_float fTimeDelta)
{
	if (m_pCommandIcon != nullptr)
	{
		if (m_pCommandIcon->Is_Dead())
		{
			Fail();
		}
	}

	m_pBarVec[1]->Set_Percent(1 - m_fTimeDuration / m_fWholeTime);
}

void CUi_CNMNRazorPart::Dying()
{
	for (auto& iter : m_pInputIconVec)
		iter->Dead();
	m_pInputIconVec.clear();
	for (auto& iter : m_pBarVec)
		iter->Dead();
	m_pBarVec.clear();
}

void CUi_CNMNRazorPart::Set_Active(_bool Active)
{
}

void CUi_CNMNRazorPart::Initialize_Icon(_uint KeyNum, _float2 StartPos)
{
	CUi_Default::Desc IconDesc;
	IconDesc.fSizeX = 50;
	IconDesc.fSizeY = 50;
	IconDesc.fX = StartPos.x + IconDesc.fSizeX * 0.5f;
	IconDesc.fY = StartPos.y;
	IconDesc.fZ = 0.6f;
	IconDesc.eRenderType = RenderType::Priority;
	IconDesc.iPriority = 3;
	IconDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	IconDesc.ShaderPass = ShaderPass::Default;
	IconDesc.TextureFileName = L"Prototype_Component_Texture_KeyBoardInput";
	IconDesc.TextureLevel = LEVEL_STATIC;
	auto pUi = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &IconDesc));
	pUi->Set_TextureNum(KeyNum);
	m_pInputIconVec.emplace_back(pUi);

	IconDesc.fX = StartPos.x + m_pBarVec[0]->Get_Scale().x - IconDesc.fSizeX * 0.5f;
	IconDesc.fY = StartPos.y;
	pUi = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &IconDesc));
	pUi->Set_TextureNum(KeyNum);
	m_pInputIconVec.emplace_back(pUi);


	CUi_CommandIcon::Desc Desc;
	Desc.fX = m_pInputIconVec[0]->Get_Pos().x;
	Desc.fY = m_pInputIconVec[0]->Get_Pos().y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 2;
	Desc.CorrectBeat = 2;
	Desc.SafeCircleSize = 100;
	Desc.DangerCircleSize = 400;
	Desc.bCreateSuccess = false;
	Desc.bCreateWord = false;
	Desc.SpecialIcon = true;
	m_pCommandIcon = static_pointer_cast<CUi_CommandIcon>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_CommandIcon::ObjID, m_iLevelIndex, L"CUi", CUi_CommandIcon::ObjID, &Desc));
}

void CUi_CNMNRazorPart::Initialize_Bar(_float2 StartPos)
{
	CUi_DefaultBar::Desc BarDesc;
	BarDesc.fZ = 0.6f;
	BarDesc.eRenderType = RenderType::Normal;
	BarDesc.iPriority = 0;
	BarDesc.fSizeX = 150 + 50 * m_fBeatDuration;
	BarDesc.fSizeY = 50;
	BarDesc.fX = StartPos.x + BarDesc.fSizeX * 0.5f;
	BarDesc.fY = StartPos.y;
	BarDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	BarDesc.ShaderPass = ShaderPass::FromLeft;
	BarDesc.TextureFileName = L"Prototype_Component_Texture_CNMNRazor";
	BarDesc.TextureLevel = LEVEL_STATIC;
	m_pBarVec.emplace_back(static_pointer_cast<CUi_DefaultBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_DefaultBar::ObjID, m_iLevelIndex, L"CUi", CUi_DefaultBar::ObjID, &BarDesc)));

	BarDesc.eRenderType = RenderType::Priority;
	m_pBarVec.emplace_back(static_pointer_cast<CUi_DefaultBar>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_DefaultBar::ObjID, m_iLevelIndex, L"CUi", CUi_DefaultBar::ObjID, &BarDesc)));

	m_pBarVec[1]->Set_TextureNum(1);
}

void CUi_CNMNRazorPart::KeyPress(_float fTimeDelta)
{
	switch (m_iKeyInput)
	{
	case 0:
		if (m_pGameInstance.lock()->Get_KeyDown(DIK_A))
		{
			if (Cal_CommandIcon(0))
			{
				Progress_KeyDown();
			}	
		}
		else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_A))
		{
			Progress_KeyPressing(fTimeDelta);
		}
		else if (m_pGameInstance.lock()->Get_KeyUp(DIK_A))
		{
			if (Cal_CommandIcon(0))
			{
				Progress_KeyUp();
			}
			else
			{
				Fail();
			}
		}
		break;
	case 1:
		if (m_pGameInstance.lock()->Get_KeyDown(DIK_D))
		{
			if (Cal_CommandIcon(1))
			{
				Progress_KeyDown();
			}
		}
		else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_D))
		{
			Progress_KeyPressing(fTimeDelta);
		}
		else if (m_pGameInstance.lock()->Get_KeyUp(DIK_D))
		{
			if (Cal_CommandIcon(1))
			{
				Progress_KeyUp();
			}
			else
			{
				Fail();
			}
		}
		break;
	case 2:
		if (m_pGameInstance.lock()->Get_KeyDown(DIK_W))
		{
			if (Cal_CommandIcon(2))
			{
				Progress_KeyDown();
			}
		}
		else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_W))
		{
			Progress_KeyPressing(fTimeDelta);
		}
		else if (m_pGameInstance.lock()->Get_KeyUp(DIK_W))
		{
			if (Cal_CommandIcon(2))
			{
				Progress_KeyUp();
			}
			else
			{
				Fail();
			}
		}
		break;
	case 3:
		if (m_pGameInstance.lock()->Get_KeyDown(DIK_S))
		{
			if (Cal_CommandIcon(3))
			{
				Progress_KeyDown();
			}
		}
		else if (m_pGameInstance.lock()->Get_KeyPressing(DIK_S))
		{
			Progress_KeyPressing(fTimeDelta);
		}
		else if (m_pGameInstance.lock()->Get_KeyUp(DIK_S))
		{
			if (Cal_CommandIcon(3))
			{
				Progress_KeyUp();
			}
			else
			{
				Fail();
			}
		}
		break;
	default:
		break;
	}
}

void CUi_CNMNRazorPart::Progress_KeyDown()
{
	Create_AfterIcon();
}

void CUi_CNMNRazorPart::Progress_KeyPressing(_float fTimeDelta)
{
	m_fTimeDuration -= fTimeDelta;
}

void CUi_CNMNRazorPart::Progress_KeyUp()
{
	Dead();
}

_bool CUi_CNMNRazorPart::Cal_CommandIcon(_uint PressKeyNum)
{
	if (m_pCommandIcon == nullptr)
	{
		Dead();
		return false;
	}
	else
	{
		if (m_pInputIconVec.size() != 0 && PressKeyNum == m_pInputIconVec[0]->Get_TextureNum()
			&& !m_pCommandIcon->Is_Dead())
		{
			CUi_JudgeWord::WordType Type = m_pCommandIcon->Cal_Judge();
			if (Type != CUi_JudgeWord::WordType::Miss)
			{
				return true;
			}
		}
		return false;
	}
}

void CUi_CNMNRazorPart::Fail()
{
	Dead();
}

void CUi_CNMNRazorPart::Create_AfterIcon()
{
	_float3 Pos = m_pInputIconVec[1]->Get_Pos();
	CUi_CommandIcon::Desc Desc;
	Desc.fX = Pos.x;
	Desc.fY = Pos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 5;
	Desc.CorrectBeat = m_fBeatDuration;
	Desc.SafeCircleSize = 100;
	Desc.DangerCircleSize = 300;
	Desc.bCreateSuccess = false;
	Desc.SpecialIcon = true;
	Desc.bCreateWord = false;
	m_pCommandIcon = static_pointer_cast<CUi_CommandIcon>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_CommandIcon::ObjID, m_iLevelIndex, L"CUi", CUi_CommandIcon::ObjID, &Desc));
}

shared_ptr<CUi_CNMNRazorPart> CUi_CNMNRazorPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_CNMNRazorPart
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_CNMNRazorPart(pDevice, pContext) { }
	};

	shared_ptr<CUi_CNMNRazorPart> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_CNMNRazorPart::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_CNMNRazorPart
	{
		MakeSharedEnabler(const CUi_CNMNRazorPart& rhs) : CUi_CNMNRazorPart(rhs) { }
	};

	shared_ptr<CUi_CNMNRazorPart> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
