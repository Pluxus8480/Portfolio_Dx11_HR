#include "CUi_SuccessSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"


wstring CUi_SuccessSystem::ObjID = L"CUi_SuccessSystem";

CUi_SuccessSystem::CUi_SuccessSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_SuccessSystem::CUi_SuccessSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_SuccessSystem::~CUi_SuccessSystem()
{
}

HRESULT CUi_SuccessSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	
	return S_OK;
}

HRESULT CUi_SuccessSystem::Initialize(void* pArg)
{
	if (pArg == nullptr)
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	m_fCenterPos = Arg->CenterPos;
	m_bEffectMode = Arg->bEffectMode;
	Initialize_Transform();
	Initialize_LineVec();
	if (!m_bEffectMode)
	{
		Initialize_Circle();
		Initialize_Light();
	}
	return S_OK;
}

void CUi_SuccessSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_SuccessSystem::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime < 0)
	{
		Dead();
	}

	if (!m_bEffectMode)
	{
		m_pLight->Add_Color(ColorType::A, -fTimeDelta);
	}
}

void CUi_SuccessSystem::LateTick(_float fTimeDelta)
{
}

void CUi_SuccessSystem::Dying()
{
	for (auto& iter : m_pLineVec)
		iter->Dead();

	if (!m_bEffectMode)
	{
		m_pLight->Dead();

		m_pGreenCircle->Dead();

		m_pBlueCircle->Dead();
	}
}

void CUi_SuccessSystem::Initialize_LineVec()
{	
	CUi_Default::Desc Desc;

	Desc.fSizeX = 1200;

	Desc.fX = m_fCenterPos.x;
	Desc.fY = m_fCenterPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 100;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::SuccessLine;
	Desc.TextureFileName = L"Prototype_Component_Texture_SuccessBackPartLine";
	Desc.TextureLevel = LEVEL_STATIC;

	for (size_t i = 0; i < 20; i++)
	{
		Desc.fSizeY = m_pGameInstance.lock()->Random_Float(140, 150);
		m_pLineVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	}

	for (size_t i = 0; i < m_pLineVec.size(); ++i)
	{
		_float Angle = XMConvertToRadians(i * 18);

		_float PosY = sin(Angle) * Desc.fSizeX * 0.5f;
		_float PosX = cos(Angle) * Desc.fSizeX * 0.5f;

		_float CircleY = sin(Angle) * 50;
		_float CircleX = cos(Angle) * 50;
		m_pLineVec[i]->Rotation(Angle);
		m_pLineVec[i]->Add_Pos(-PosX - CircleX, -PosY - CircleY);

		if (i % 3 == 0)
		{
			m_pLineVec[i]->Set_TextureNum(1);
		}
	}


}

void CUi_SuccessSystem::Initialize_Circle()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 80;
	Desc.fSizeY = 80;
	Desc.fX = m_fCenterPos.x;
	Desc.fY = m_fCenterPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 102;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::SuccessLine;
	Desc.TextureFileName = L"Prototype_Component_Texture_SuccessMiddleGreen";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pGreenCircle = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));\
}

void CUi_SuccessSystem::Initialize_Light()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 150;
	Desc.fSizeY = 150;
	Desc.fX = m_fCenterPos.x;
	Desc.fY = m_fCenterPos.y;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 101;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_SuccessMiddleBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBlueCircle = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));


	Desc.fSizeX = 300;
	Desc.fSizeY = 300;
	Desc.iPriority = 99;
	Desc.TextureFileName = L"Prototype_Component_Texture_SuccessMiddleBackLight";
	m_pLight = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));

}

shared_ptr<CUi_SuccessSystem> CUi_SuccessSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_SuccessSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_SuccessSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_SuccessSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_SuccessSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_SuccessSystem
	{
		MakeSharedEnabler(const CUi_SuccessSystem& rhs) : CUi_SuccessSystem(rhs) { }
	};

	shared_ptr<CUi_SuccessSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
