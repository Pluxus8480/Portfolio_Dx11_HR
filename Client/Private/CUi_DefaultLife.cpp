#include "CUi_DefaultLife.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_DefaultLife::ObjID = L"CUi_DefaultLife";

CUi_DefaultLife::CUi_DefaultLife(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Default(pDevice, pContext)
{
}

CUi_DefaultLife::CUi_DefaultLife(const CUi_DefaultLife& rhs)
	:CUi_Default(rhs)
{
}

CUi_DefaultLife::~CUi_DefaultLife()
{
}

HRESULT CUi_DefaultLife::Initialize_Prototype()
{
	Initialize_SpecialComboAttackResource();

	return S_OK;
}

HRESULT CUi_DefaultLife::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	m_fLifeTime = Arg->fLifeTime;
	m_fAlphaSpeed = Arg->fAlphaSpeed;
	m_fScalingSpeed = Arg->fScalingSpeed;
	m_fAlphaTime = Arg->fAlphaTime;
	m_fScaleTime = Arg->fScaleTime;

	return S_OK;
}

void CUi_DefaultLife::PriorityTick(_float fTimeDelta)
{
}

void CUi_DefaultLife::Tick(_float fTimeDelta)
{
	Cal_LifeTime(fTimeDelta);
	Adjust_AlphaTime(fTimeDelta);
	Adjust_ScaleTime(fTimeDelta);
}

void CUi_DefaultLife::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

void CUi_DefaultLife::Initialize_SpecialComboAttackResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SpecialComboStar",
			L"../Bin/Resources/Textures/Ui/SpecialComboAttack/SpecialComboStar.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SpecialComboCircle",
			L"../Bin/Resources/Textures/Ui/SpecialComboAttack/SpecialComboCircle.png"));
}

void CUi_DefaultLife::Cal_LifeTime(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime < 0)
	{
		Dead();
	}
}

void CUi_DefaultLife::Adjust_AlphaTime(_float fTimeDelta)
{
	m_fAlphaTime -= fTimeDelta;
	if (m_fAlphaTime > 0)
	{
		Add_Color(ColorType::A, m_fAlphaSpeed * fTimeDelta);
	}

}

void CUi_DefaultLife::Adjust_ScaleTime(_float fTimeDelta)
{
	m_fScaleTime -= fTimeDelta;
	if (m_fScaleTime > 0)
	{
		Add_Scale(m_fScalingSpeed * fTimeDelta, m_fScalingSpeed * fTimeDelta);
	}
}

shared_ptr<CUi_DefaultLife> CUi_DefaultLife::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_DefaultLife
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_DefaultLife(pDevice, pContext) { }
	};

	shared_ptr<CUi_DefaultLife> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_DefaultLife::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_DefaultLife
	{
		MakeSharedEnabler(const CUi_DefaultLife& rhs) : CUi_DefaultLife(rhs) { }
	};

	shared_ptr<CUi_DefaultLife> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
