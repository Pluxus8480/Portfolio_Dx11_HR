#include "CUI_MimosaCutin.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"


wstring CUI_MimosaCutin::ObjID = L"CUI_MimosaCutin";

CUI_MimosaCutin::CUI_MimosaCutin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUI_MimosaCutin::CUI_MimosaCutin(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUI_MimosaCutin::~CUI_MimosaCutin()
{
}

HRESULT CUI_MimosaCutin::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);


	return S_OK;
}

HRESULT CUI_MimosaCutin::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_CUtinObj();
	//m_fLifeTime = reinterpret_cast<Desc*>(pArg)->fLifeTime;

	return S_OK;
}

void CUI_MimosaCutin::PriorityTick(_float fTimeDelta)
{
}

void CUI_MimosaCutin::Tick(_float fTimeDelta)
{
	Progress_State(fTimeDelta);
}

void CUI_MimosaCutin::LateTick(_float fTimeDelta)
{
}

void CUI_MimosaCutin::Progress_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case State::MimosaAppear:
		Progress_MimosaAppear(fTimeDelta);
		break;
	case State::ChaiAppear:
		Progress_ChaiAppear(fTimeDelta);
		break;
	case State::Slow:
		Progress_Slow(fTimeDelta);
		break;
	case State::Disappear:
		Progress_Disappear(fTimeDelta);
		break;
	default:
		break;
	}
}

void CUI_MimosaCutin::Progress_MimosaAppear(_float fTimeDelta)
{
	m_pMimosaCutin->Add_Pos(4000 * fTimeDelta, 0);

	if (m_pMimosaCutin->Get_Pos().x > 100)
	{
		m_eState = State::ChaiAppear;
	}
}

void CUI_MimosaCutin::Progress_ChaiAppear(_float fTimeDelta)
{
	m_fDelayTime -= fTimeDelta;
	m_pMimosaCutin->Add_Pos(50 * fTimeDelta, 0);
	if (m_fDelayTime < 0)
	{
		m_pChaiCutin->Add_Pos(-4000 * fTimeDelta, 0);
	}
	if (m_pChaiCutin->Get_Pos().x < -100)
	{
		m_eState = State::Slow;
	}
}

void CUI_MimosaCutin::Progress_Slow(_float fTimeDelta)
{
	m_fSlowTime -= fTimeDelta;
	if (m_fSlowTime < 0)
	{
		m_eState = State::Disappear;
	}
	m_pChaiCutin->Add_Pos(-50 * fTimeDelta, 0);
	m_pMimosaCutin->Add_Pos(50 * fTimeDelta, 0);
}

void CUI_MimosaCutin::Progress_Disappear(_float fTimeDelta)
{
	m_pChaiCutin->Add_Pos(-7000 * fTimeDelta, 0);
	m_pMimosaCutin->Add_Pos(7000 * fTimeDelta, 0);

	if (m_pChaiCutin->Get_Pos().x < -2000)
	{
		Dead();
	}
}

void CUI_MimosaCutin::Initialize_CUtinObj()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 850;
	Desc.fSizeY = 170;
	Desc.fX = 1500;
	Desc.fY = -100;
	Desc.fZ = 0.1f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.fSpeed = 500;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ChaiCutIn";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pChaiCutin = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	
	Desc.fX = -1500;
	Desc.fY = 100;
	Desc.TextureFileName = L"Prototype_Component_Texture_MimosaCutIn";
	m_pMimosaCutin = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUI_MimosaCutin::Dying()
{
	m_pChaiCutin->Dead();
	m_pMimosaCutin->Dead();
}

shared_ptr<CUI_MimosaCutin> CUI_MimosaCutin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUI_MimosaCutin
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI_MimosaCutin(pDevice, pContext) { }
	};

	shared_ptr<CUI_MimosaCutin> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUI_MimosaCutin::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUI_MimosaCutin
	{
		MakeSharedEnabler(const CUI_MimosaCutin& rhs) : CUI_MimosaCutin(rhs) { }
	};

	shared_ptr<CUI_MimosaCutin> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
