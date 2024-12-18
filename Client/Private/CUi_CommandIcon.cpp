#include "CUi_CommandIcon.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"
#include "iostream"
#include "CUi_PartIcon.h"
#include "CUi_SuccessSystem.h"


wstring CUi_CommandIcon::ObjID = L"CUi_CommandIcon";

CUi_CommandIcon::CUi_CommandIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_CommandIcon::CUi_CommandIcon(const CUi_2D& rhs)
	:CUi_2D(rhs)
	{
	}

CUi_CommandIcon::~CUi_CommandIcon()
{
}

HRESULT CUi_CommandIcon::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_PartIcon::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_JudgeWord::ObjID);
	
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Combo_Circle_Success"), TEXT("../Bin/Resources/Sounds/SFX/Player_Sound/Combo_Circle/Player_InstFx_163.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Combo_Circle"), TEXT("../Bin/Resources/Sounds/SFX/Player_Sound/Combo_Circle/Player_InstFx_287.ogg")));
	return S_OK;
}

HRESULT CUi_CommandIcon::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	Initialize_Icon(Arg->fX, Arg->fY, Arg->SafeCircleSize, Arg->DangerCircleSize, Arg->SpecialIcon);
	Initialize_Speed(*Arg);
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Combo_Circle");
	m_bCreateSuccess = Arg->bCreateSuccess;
	m_bCreateWord = Arg->bCreateWord;

	return S_OK;
}

void CUi_CommandIcon::PriorityTick(_float fTimeDelta)
{
}

void CUi_CommandIcon::Tick(_float fTimeDelta)
{
	if (m_pDangerIcon->Get_Scale().x < m_pSafeIcon->Get_Scale().x)
	{
		m_pSafeIcon->Set_TextureNum(1);
	}
	if (m_pDangerIcon->Get_Scale().x < m_pSafeIcon->Get_Scale().x * 0.2f)
	{
		Dead();
	}
	else
	{
		m_pDangerIcon->Add_Scale(-fTimeDelta * m_fScaleDownSpeed, -fTimeDelta * m_fScaleDownSpeed);
	}
}

void CUi_CommandIcon::LateTick(_float fTimeDelta)
{

}

void CUi_CommandIcon::Initialize_Icon(_float fX, _float fY, _float SafeCircleSize, _float DangerCircleSize, _bool SpecialIcon)
{
	auto pGameInstance = m_pGameInstance.lock();
	_float PosX = 0;
	_float PosY = 0;
	if (fX != 0 || fY != 0)
	{
		PosX = fX;
		PosY = fY;
	}
	else
	{

		PosX = pGameInstance->Random_Float(-400, 400);
		PosY = pGameInstance->Random_Float(-150, 200);
	}
	CUi_PartIcon::Desc Desc;
	Desc.fSizeX = SafeCircleSize;
	Desc.fSizeY = SafeCircleSize;
	Desc.fX = PosX;
	Desc.fY = PosY;
	Desc.fZ = 0.1f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 10;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::CommandIconCircle;
	Desc.TextureFileName = L"Prototype_Component_Texture_CommandIcon";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.bRedIcon = false;
	m_pSafeIcon = static_pointer_cast<CUi_PartIcon>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_PartIcon::ObjID, m_iLevelIndex, L"CUi", CUi_PartIcon::ObjID, &Desc));


	Desc.fSizeX = DangerCircleSize;
	Desc.fSizeY = DangerCircleSize;
	Desc.fX = PosX;
	Desc.fY = PosY;
	Desc.fZ = 0.1f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::CommandIconCircle;
	Desc.TextureFileName = L"Prototype_Component_Texture_CommandIcon";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.bRedIcon = true;
	m_pDangerIcon = static_pointer_cast<CUi_PartIcon>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_PartIcon::ObjID, m_iLevelIndex, L"CUi", CUi_PartIcon::ObjID, &Desc));

	if (!SpecialIcon)
	{
		m_pDangerIcon->Set_TextureNum(1);
	}
	else
	{
		m_pDangerIcon->Set_TextureNum(2);
	}
}

void CUi_CommandIcon::Initialize_Speed(Desc Desc)
{
	_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
	m_fScaleDownSpeed = (m_pDangerIcon->Get_Scale().x - m_pSafeIcon->Get_Scale().x) / (Desc.CorrectBeat * Beat);
}

CUi_JudgeWord::WordType CUi_CommandIcon::Cal_Judge()
{
	_float Safe = m_pSafeIcon->Get_Scale().x;
	_float Dgr = m_pDangerIcon->Get_Scale().x;
	CUi_JudgeWord::WordType  eWordType = CUi_JudgeWord::WordType::End;
	CUi_JudgeWord::Desc Desc;
	if (Safe * 0.7f < Dgr && Dgr < Safe * 1.3f)
	{
		if (m_bCreateSuccess)
		{
			CUi_SuccessSystem::Desc Desc;
			Desc.CenterPos = { m_pSafeIcon->Get_Pos().x, m_pSafeIcon->Get_Pos().y };
			CUi_Mgr::Get_Instance()->Add_Clone(CUi_SuccessSystem::ObjID, m_iLevelIndex, L"CUi", CUi_SuccessSystem::ObjID, &Desc);
		}
		m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Combo_Circle_Success");
		eWordType = CUi_JudgeWord::WordType::Perfect;
	}
	else if (Safe * 0.5f < Dgr && Dgr < Safe * 1.5f)
	{
		if (m_bCreateSuccess)
		{
			CUi_SuccessSystem::Desc Desc;
			Desc.CenterPos = { m_pSafeIcon->Get_Pos().x, m_pSafeIcon->Get_Pos().y };
			CUi_Mgr::Get_Instance()->Add_Clone(CUi_SuccessSystem::ObjID, m_iLevelIndex, L"CUi", CUi_SuccessSystem::ObjID, &Desc);
		}
		m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Combo_Circle_Success");
		eWordType = CUi_JudgeWord::WordType::Good;
	}
	else
	{
		eWordType = CUi_JudgeWord::WordType::Miss;
	}

	Desc.eWordType = eWordType;
	Desc.fX = m_pSafeIcon->Get_Pos().x;
	Desc.fY = m_pSafeIcon->Get_Pos().y;
	if (m_bCreateWord)
	{
		m_pGameInstance.lock()->Add_Clone(m_iLevelIndex, L"CUi", CUi_JudgeWord::ObjID, &Desc);
	}

	m_pGameInstance.lock()->Stop_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Combo_Circle");
	Dead();

	return eWordType;
}

void CUi_CommandIcon::Dying()
{
	m_pDangerIcon->Dead();
	m_pSafeIcon->Dead();
}


shared_ptr<CUi_CommandIcon> CUi_CommandIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_CommandIcon
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_CommandIcon(pDevice, pContext) { }
	};

	shared_ptr<CUi_CommandIcon> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_CommandIcon::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_CommandIcon
	{
		MakeSharedEnabler(const CUi_CommandIcon& rhs) : CUi_CommandIcon(rhs) { }
	};

	shared_ptr<CUi_CommandIcon> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
