#include "CUi_CNMNRazer.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "CUi_CommandIcon.h"
#include "CUi_DefaultBar.h"
#include "CUi_CNMNRazorPart.h"
#include "iostream"
#include "Beat_Manager.h"
#include "Cine_Manager.h"


wstring CUi_CNMNRazer::ObjID = L"CUi_CNMNRazer";

CUi_CNMNRazer::CUi_CNMNRazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_CNMNRazer::CUi_CNMNRazer(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_CNMNRazer::~CUi_CNMNRazer()
{
}

HRESULT CUi_CNMNRazer::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_CommandIcon::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_CNMNRazorPart::ObjID);
	
	return S_OK;
}

HRESULT CUi_CNMNRazer::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_FirstPhaseCallback();
	Initialize_SecondPhaseCallback();
	Initialize_ThirdPhaseCallback();
	Initialize_FourthPhaseCallback();
	Initialize_FifthPhaseCallback();

	return S_OK;
}

void CUi_CNMNRazer::PriorityTick(_float fTimeDelta)
{
}

void CUi_CNMNRazer::Tick(_float fTimeDelta)
{
	Check_Fail();
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_A))
	{
		KeyPress(0, fTimeDelta);
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_D))
	{
		KeyPress(1, fTimeDelta);
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_W))
	{
		KeyPress(2, fTimeDelta);
	}
	else if (m_pGameInstance.lock()->Get_KeyDown(DIK_S))
	{
		KeyPress(3, fTimeDelta);
	}
}

void CUi_CNMNRazer::LateTick(_float fTimeDelta)
{
}

void CUi_CNMNRazer::KeyPress(_uint Keypress, _float fTimeDelta)
{
	if (m_pIconVec.size() != 0 && m_pDefaultVec.size() != 0)
	{
		CUi_JudgeWord::WordType Type = m_pIconVec[0]->Cal_Judge();
		m_pIconVec.erase(m_pIconVec.begin());
		if (m_pDefaultVec.size() != 0)
		{
			m_pDefaultVec[0]->Dead();
			m_pDefaultVec.erase(m_pDefaultVec.begin());
		}
	}
	else
	{
		if (m_pPart != nullptr)
		{
			m_pPart->Set_StartJudge();
		}
	}
}

void CUi_CNMNRazer::Create_Icon(_float2 Pos, _uint CorrectBeat, _uint KeyNum)
{
	CUi_Default::Desc IconDesc;
	IconDesc.fX = Pos.x;
	IconDesc.fY = Pos.y;
	IconDesc.fZ = 0.6f;
	IconDesc.eRenderType = RenderType::Priority;
	IconDesc.iPriority = 3;
	IconDesc.fSizeX = 50;
	IconDesc.fSizeY = 50;
	IconDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	IconDesc.ShaderPass = ShaderPass::Default;
	IconDesc.TextureFileName = L"Prototype_Component_Texture_KeyBoardInput";
	IconDesc.TextureLevel = LEVEL_STATIC;
	auto pUi = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &IconDesc));
	pUi->Set_TextureNum(KeyNum);
	m_pDefaultVec.emplace_back(pUi);


	CUi_CommandIcon::Desc Desc;
	Desc.fX = Pos.x;
	Desc.fY = Pos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 2;
	Desc.CorrectBeat = CorrectBeat;
	Desc.SafeCircleSize = 100;
	Desc.DangerCircleSize = 500;
	Desc.bCreateSuccess = false;
	Desc.bCreateWord = false;
	m_pIconVec.emplace_back(static_pointer_cast<CUi_CommandIcon>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_CommandIcon::ObjID, m_iLevelIndex, L"CUi", CUi_CommandIcon::ObjID, &Desc)));
}

void CUi_CNMNRazer::Create_SpecialIcon(_float2 Pos, _uint KeyInput, _float BeatDuration)
{
	CUi_CNMNRazorPart::Desc Desc;
	Desc.BeatDuration = BeatDuration;
	Desc.KeyInput = KeyInput;
	Desc.Pos = { Pos.x, Pos.y };
	m_pPart = static_pointer_cast<CUi_CNMNRazorPart>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CNMNRazorPart::ObjID, m_iLevelIndex,
		L"CUi", CUi_CNMNRazorPart::ObjID, &Desc));
}

void CUi_CNMNRazer::Check_Fail()
{
	if (m_pIconVec.size() != 0)
	{
		if (m_pIconVec[0]->Is_Dead())
		{
			if (m_pIconVec.size() == m_pDefaultVec.size())
			{
				m_pDefaultVec[0]->Dead();
				m_pDefaultVec.erase(m_pDefaultVec.begin());
			}
			m_pIconVec.erase(m_pIconVec.begin());
		}
	}
}

void CUi_CNMNRazer::Initialize_FirstPhaseCallback()
{
	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 0, [&](_float4, _float4, _float3)
		{	
			cout << "FirstPhase" << endl;
			Create_Icon(_float2(-400, 200), 2, 0);
		});
	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 1, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(0, 200), 2, 1);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 2, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(-400, -200), 2, 2);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 3, [&](_float4, _float4, _float3)
		{
			Create_SpecialIcon(_float2(-250, -50), 0, 3);
		});
}

void CUi_CNMNRazer::Initialize_SecondPhaseCallback()
{
	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 4, [&](_float4, _float4, _float3)
		{
			cout << "SecondPhase" << endl;
			Create_Icon(_float2(400, 150), 2, 3);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 5, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(0, 250), 2, 1);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 6, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(200, 50), 2, 2);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 7, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(0, 250), 2, 1);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 8, [&](_float4, _float4, _float3)
		{
			Create_SpecialIcon(_float2(250, 150), 1, 3);
		});
}

void CUi_CNMNRazer::Initialize_ThirdPhaseCallback()
{
	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 9, [&](_float4, _float4, _float3)
		{
			cout << "ThirdPhase" << endl;
			Create_Icon(_float2(0, 1), 2, 1);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 10, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(-300, 250), 2, 0);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 11, [&](_float4, _float4, _float3)
		{
			Create_SpecialIcon(_float2(100, -50), 2, 3);
		});
}

void CUi_CNMNRazer::Initialize_FourthPhaseCallback()
{
	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 12, [&](_float4, _float4, _float3)
		{
			cout << "FourthPhase" << endl;
			Create_Icon(_float2(300, 250), 2, 3);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 13, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(-300, -250), 2, 1);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 14, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(-300, -250), 2, 2);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 15, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(100, -30), 2, 2);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 16, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(-100, 100), 2, 0);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 17, [&](_float4, _float4, _float3)
		{
			Create_SpecialIcon(_float2(-200, 100), 2, 3);
		});
}

void CUi_CNMNRazer::Initialize_FifthPhaseCallback()
{
	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 18, [&](_float4, _float4, _float3)
		{
			cout << "FifthPhase" << endl;
			Create_Icon(_float2(-150, 70), 2, 3);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 19, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(200, -150), 2, 1);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 20, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(-350, -170), 2, 2);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 21, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(350, -170), 2, 2);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 22, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(0, -100), 1, 3);
		});

	CCine_Manager::Get_Instance()->AddSequenceCallback(
		"EV1190", 0, 23, [&](_float4, _float4, _float3)
		{
			Create_Icon(_float2(0, 150), 1, 2);
		});
}

shared_ptr<CUi_CNMNRazer> CUi_CNMNRazer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_CNMNRazer
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_CNMNRazer(pDevice, pContext) { }
	};

	shared_ptr<CUi_CNMNRazer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_CNMNRazer::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_CNMNRazer
	{
		MakeSharedEnabler(const CUi_CNMNRazer& rhs) : CUi_CNMNRazer(rhs) { }
	};

	shared_ptr<CUi_CNMNRazer> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
