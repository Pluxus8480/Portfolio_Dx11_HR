#include "CUi_EnemySpecialAttackPart.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"
#include "iostream"
#include "CUi_PartIcon.h"
#include "CUi_SuccessSystem.h"


wstring CUi_EnemySpecialAttackPart::ObjID = L"CUi_EnemySpecialAttackPart";

CUi_EnemySpecialAttackPart::CUi_EnemySpecialAttackPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_EnemySpecialAttackPart::CUi_EnemySpecialAttackPart(const CUi_2D& rhs)
	:CUi_2D(rhs)
	{
	}

CUi_EnemySpecialAttackPart::~CUi_EnemySpecialAttackPart()
{
}

HRESULT CUi_EnemySpecialAttackPart::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EnemySpecialAttackPart",
			L"../Bin/Resources/Textures/Ui/EnemySpecialAttack/EnemySpecialAttackPart%d.png", 2));

	m_List_Owning_ObjectID.push_back(CUi_PartIcon::ObjID);

	return S_OK;
}

HRESULT CUi_EnemySpecialAttackPart::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	Initialize_Icon(Arg->SafeCircleSize, Arg->DangerCircleSize);
	Initialize_Speed();
	m_iActiveBeat = Arg->ActiveBeat;

	m_pDangerIcon->Set_Active(false);
	m_pSafeIcon->Set_Active(false);

	return S_OK;
}

void CUi_EnemySpecialAttackPart::PriorityTick(_float fTimeDelta)
{
}

void CUi_EnemySpecialAttackPart::Tick(_float fTimeDelta)
{
	_float BeatCount = CBeat_Manager::Get_Instance()->Get_BeatCount() + BeatPercent();
	if (m_iActiveBeat <= BeatCount)
	{
		m_pSafeIcon->Set_Active(true);
		m_pDangerIcon->Set_Active(true);
		if (m_pDangerIcon->Get_Scale().x > m_pSafeIcon->Get_Scale().x)
		{
			Dead();
		}
		else
		{
			m_pDangerIcon->Add_Scale(fTimeDelta * m_fScaleUpSpeed, fTimeDelta * m_fScaleUpSpeed);
		}
	}
}

void CUi_EnemySpecialAttackPart::LateTick(_float fTimeDelta)
{

}

void CUi_EnemySpecialAttackPart::Initialize_Icon(_float SafeCircleSize, _float DangerCircleSize)
{
	auto pGameInstance = m_pGameInstance.lock();
	_float PosX = 0;
	_float PosY = 0;
	CUi_PartIcon::Desc Desc;
	Desc.fSizeX = SafeCircleSize;
	Desc.fSizeY = SafeCircleSize;
	Desc.fX = PosX;
	Desc.fY = PosY;
	Desc.fZ = 0.1f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 10;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_EnemySpecialAttackPart";
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
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_EnemySpecialAttackPart";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.bRedIcon = true;
	m_pDangerIcon = static_pointer_cast<CUi_PartIcon>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_PartIcon::ObjID, m_iLevelIndex, L"CUi", CUi_PartIcon::ObjID, &Desc));
	m_pDangerIcon->Set_TextureNum(1);
}

void CUi_EnemySpecialAttackPart::Initialize_Speed()
{
	_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
	m_fScaleUpSpeed = (m_pSafeIcon->Get_Scale().x - m_pDangerIcon->Get_Scale().x) / (2 * Beat);
}

void CUi_EnemySpecialAttackPart::Dying()
{
	m_pDangerIcon->Dead();
	m_pSafeIcon->Dead();
}


shared_ptr<CUi_EnemySpecialAttackPart> CUi_EnemySpecialAttackPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_EnemySpecialAttackPart
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_EnemySpecialAttackPart(pDevice, pContext) { }
	};

	shared_ptr<CUi_EnemySpecialAttackPart> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_EnemySpecialAttackPart::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_EnemySpecialAttackPart
	{
		MakeSharedEnabler(const CUi_EnemySpecialAttackPart& rhs) : CUi_EnemySpecialAttackPart(rhs) { }
	};

	shared_ptr<CUi_EnemySpecialAttackPart> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
