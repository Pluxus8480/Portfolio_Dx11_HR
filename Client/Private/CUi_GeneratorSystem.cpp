#include "CUi_GeneratorSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_CommandIcon.h"
#include "Beat_Manager.h"
#include "CUi_Default.h"



wstring CUi_GeneratorSystem::ObjID = L"CUi_GeneratorSystem";

CUi_GeneratorSystem::CUi_GeneratorSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_GeneratorSystem::CUi_GeneratorSystem(const CUi_GeneratorSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_GeneratorSystem::~CUi_GeneratorSystem()
{
}

HRESULT CUi_GeneratorSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_CommandIcon::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);


	return S_OK;
}

HRESULT CUi_GeneratorSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	m_fCenterPosVec.reserve(m_iCreateCount);
	_float2 Pos = { -150, 200 };
	m_fCenterPosVec.emplace_back(Pos);

	Pos = { -150, 0 };
	m_fCenterPosVec.emplace_back(Pos);

	Pos = { -150, -200 };
	m_fCenterPosVec.emplace_back(Pos);

	Pos = { 150, -200 };
	m_fCenterPosVec.emplace_back(Pos);

	m_iCreateBeat = BEATMANAGER->Get_BeatCount() + 1;

	return S_OK;
}

void CUi_GeneratorSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_GeneratorSystem::Tick(_float fTimeDelta)
{
	Judge_CreateTime(fTimeDelta);
	Cal_Dead();
	Adjust_BackSize();
	m_fCreateSuccessTime += fTimeDelta;

	if (m_fCreateSuccessTime > 0.5f)
	{
		if (m_pSuccessVec.size() != 0)
		{
			for (auto& iter : m_pSuccessVec)
				iter->Dead();
			m_pSuccessVec.clear();
		}
	}
}

void CUi_GeneratorSystem::LateTick(_float fTimeDelta)
{
}

void CUi_GeneratorSystem::Dying()
{
	for (auto& iter : m_pCommandIconVec)
		iter->Dead();
	for (auto& iter : m_pBackCircleVec)
		iter->Dead();
	for (auto& iter : m_pBackLightVec)
		iter->Dead();
	for (auto& iter : m_pSuccessVec)
		iter->Dead();
}

_bool CUi_GeneratorSystem::Generator_Judge()
{
	_bool bSuccess = false;
	if (m_pGameInstance.lock()->Get_MouseDown(MOUSEKEYSTATE::DIMK_RBUTTON)
		|| m_pGameInstance.lock()->Get_MouseDown(MOUSEKEYSTATE::DIMK_LBUTTON))
	{
		if (0 < m_pCommandIconVec.size())
		{
			CUi_JudgeWord::WordType type = m_pCommandIconVec[0]->Cal_Judge();
			if (type == CUi_JudgeWord::WordType::Perfect ||
				type == CUi_JudgeWord::WordType::Good)
			{
				bSuccess = true;
				m_iSuccess_Count++;
			}
			else
			{
				m_iSuccess_Count = 0;
				m_bFail = true;
				CUi_Mgr::Get_Instance()->Ui_Dead(CUi_GeneratorSystem::ObjID);
			}
		}
	}
	return bSuccess;
}

void CUi_GeneratorSystem::Create_CommandIcon()
{
	CUi_CommandIcon::Desc Desc;
	Desc.fX = m_fCenterPosVec[m_iCreateCount].x;
	Desc.fY = m_fCenterPosVec[m_iCreateCount].y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 2;
	Desc.CorrectBeat = 4;
	Desc.SafeCircleSize = 100;
	Desc.DangerCircleSize = 500;
	Desc.bCreateSuccess = false;
	m_pCommandIconVec.emplace_back(static_pointer_cast<CUi_CommandIcon>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_CommandIcon::ObjID, m_iLevelIndex, L"CUi", CUi_CommandIcon::ObjID, &Desc)));
}

void CUi_GeneratorSystem::Create_BackGround()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 200;
	Desc.fSizeY = 200;
	Desc.fX = m_fCenterPosVec[m_iCreateCount].x;
	Desc.fY = m_fCenterPosVec[m_iCreateCount].y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_GeneratorBackCircle";
	Desc.TextureLevel = LEVEL_STATIC;

	auto BackCircle = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pBackCircleVec.emplace_back(BackCircle);

	_float4 CircleColor = { 0, 0, 0, 0 };
	if (m_iCreateCount % 2 == 0)
	{
		CircleColor = { 1.f, 0.7f, 0, 0.5f };
	}
	else
	{
		CircleColor = { 0, 0.7f, 1, 0.5f };
	}

	BackCircle->Set_Color(CircleColor);

	Desc.fSizeX = 100;
	Desc.fSizeY = 100;
	Desc.fX = m_fCenterPosVec[m_iCreateCount].x;
	Desc.fY = m_fCenterPosVec[m_iCreateCount].y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 1;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_GeneratorBackLight";
	Desc.TextureLevel = LEVEL_STATIC;
	auto BackLight = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pBackLightVec.emplace_back(BackLight);

	_float4 BackLightColor = { 0, 0, 0, 0 };
	if (m_iCreateCount % 2 == 0)
	{
		BackLightColor = { 1.f, 0.7f, 0, 0.8f };
	}
	else
	{
		BackLightColor = { 0, 0.7f, 1, 0.8f };
	}

	BackLight->Set_Color(BackLightColor);
}

void CUi_GeneratorSystem::Cal_Dead()
{
	if (0 < m_pCommandIconVec.size())
	{
		if (m_pCommandIconVec[0]->Is_Dead())
		{
			m_pCommandIconVec[0] = nullptr;
			m_pCommandIconVec.erase(m_pCommandIconVec.begin());

			m_pBackCircleVec[0]->Dead();
			m_pBackCircleVec[0] = nullptr;
			m_pBackCircleVec.erase(m_pBackCircleVec.begin());

			m_pBackLightVec[0]->Dead();
			m_pBackLightVec[0] = nullptr;
			m_pBackLightVec.erase(m_pBackLightVec.begin());
		}
	}
}

void CUi_GeneratorSystem::Judge_CreateTime(_float fTimeDelta)
{
	if (m_iCreateBeat == BEATMANAGER->Get_BeatCount())
	{
		switch (m_iCreateCount)
		{
		case 0:
			Create_CommandIcon();
			Create_BackGround();
			m_iCreateBeat += 4;
			break;
		case 1:
			Create_CommandIcon();
			Create_BackGround();
			m_iCreateBeat += 4;
			break;
		case 2:
			Create_CommandIcon();
			Create_BackGround();
			m_iCreateBeat += 2;
			break;
		case 3:
			Create_CommandIcon();
			Create_BackGround();
			m_iCreateBeat += 6;
			break;
		default:
			Dead();
			break;
		}

		m_iCreateCount += 1;
	}
}

void CUi_GeneratorSystem::Adjust_BackSize()
{
	_float Size = { 0 };
	for (size_t i = 0; i < m_pCommandIconVec.size(); ++i)
	{
		Size = m_pCommandIconVec[i]->Get_DangerIconSize() * 0.8f;
		if (Size < m_pBackCircleVec[i]->Get_Scale().x)
		{
			m_pBackCircleVec[i]->Set_Scale(Size, Size);
		}
	}

}

void CUi_GeneratorSystem::Create_Success()
{
	//CUi_Default::Desc Desc;
	//Desc.fSizeX = 250;
	//Desc.fSizeY = 250;
	//Desc.fX = 0;
	//Desc.fY = 0;
	//Desc.fZ = 0.6f;
	//Desc.eRenderType = RenderType::Priority;
	//Desc.iPriority = 0;
	//Desc.fSpeed = 50;
	//Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	//Desc.ShaderPass = ShaderPass::Blend;
	//Desc.TextureFileName = L"Prototype_Component_Texture_GeneratorSuccess";
	//Desc.TextureLevel = LEVEL_STATIC;
	//m_pSuccessVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
	//	->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	//m_pSuccessVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
	//	->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	//m_pSuccessVec[1]->Set_TextureNum(1);

	//m_pSuccessVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
	//	->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	//m_pSuccessVec[2]->Set_TextureNum(2);

	//for (auto& iter : m_pSuccessVec)
	//{
	//	iter->Set_Pos(m_fCenterPosVec[m_iCreateCount - 1].x, m_fCenterPosVec[m_iCreateCount - 1].y);
	//	iter->Set_Color(ColorType::A, 0.5f);
	//}
}

shared_ptr<CUi_GeneratorSystem> CUi_GeneratorSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_GeneratorSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_GeneratorSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_GeneratorSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_GeneratorSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_GeneratorSystem
	{
		MakeSharedEnabler(const CUi_GeneratorSystem& rhs) : CUi_GeneratorSystem(rhs) { }
	};

	shared_ptr<CUi_GeneratorSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
