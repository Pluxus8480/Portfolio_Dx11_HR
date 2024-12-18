#include "CUi_Chip.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"


wstring CUi_Chip::ObjID = L"CUi_Chip";

CUi_Chip::CUi_Chip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_Chip::CUi_Chip(const CUi_Chip& rhs)
	:CUi_2D(rhs)
{
}

CUi_Chip::~CUi_Chip()
{
}

HRESULT CUi_Chip::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(
		L"Prototype_Component_Texture_StoreChip", 
		L"../Bin/Resources/Textures/Ui/Store/Chip/ChipBase%d.png", 2));


	return S_OK;
}

HRESULT CUi_Chip::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	m_ChipName = Arg->ChipName;
	m_iPrice = Arg->iPrice;
	m_iOriginPrice = m_iPrice;


	return S_OK;
}

void CUi_Chip::PriorityTick(_float fTimeDelta)
{
}

void CUi_Chip::Tick(_float fTimeDelta)
{
	Check_UpGrade();
}

void CUi_Chip::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		if (m_bFocus)
			m_iTextureNum = 1;
		else
			m_iTextureNum = 0;

		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}

}

void CUi_Chip::Check_UpGrade()
{
	CPlayer_Manager::ChipDesc Desc;
	Desc = PLAYERMANAGER->Get_PlayerChip(Get_Name());
	switch (Desc.iUpGrade)
	{
	case 0:
		m_iPrice = m_iOriginPrice;
		break;
	case 1:
		m_iPrice = _uint(m_iOriginPrice * 1.5f);
		break;
	case 2:
		m_iPrice = _uint(m_iOriginPrice * 2.f);
		break;
	default:
		break;
	} 
}

shared_ptr<CUi_Chip> CUi_Chip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_Chip
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_Chip(pDevice, pContext) { }
	};

	shared_ptr<CUi_Chip> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_Chip::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_Chip
	{
		MakeSharedEnabler(const CUi_Chip& rhs) : CUi_Chip(rhs) { }
	};

	shared_ptr<CUi_Chip> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
