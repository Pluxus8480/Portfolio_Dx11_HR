#include "UI_Event_Cut.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "GameManager.h"


wstring CUI_Event_Cut::ObjID = L"CUI_Event_Cut";

CUI_Event_Cut::CUI_Event_Cut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUI_Event_Cut::CUI_Event_Cut(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUI_Event_Cut::~CUI_Event_Cut()
{
}

HRESULT CUI_Event_Cut::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EventCutScene",
			L"../Bin/Resources/Textures/Event/CutScene%d.png", 11));

	return S_OK;
}

HRESULT CUI_Event_Cut::Initialize(void* pArg)
{
	Initialize_Transform();
	CUi_2D::Desc DDesc;
#ifdef PERFECTION_LOAD
	DDesc.fSizeX = g_iWinSizeX * 0.5f;
	DDesc.fSizeY = g_iWinSizeY * 0.5f;
#else
	DDesc.fSizeX = g_iWinSizeX;
	DDesc.fSizeY = g_iWinSizeY;
#endif
	DDesc.fX = 0;
	DDesc.fY = 0;
	DDesc.fZ = 0.6f;
	DDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	DDesc.ShaderPass = ShaderPass::Default;
	DDesc.TextureFileName = L"Prototype_Component_Texture_EventCutScene";
	DDesc.TextureLevel = LEVEL_STATIC;
	if (FAILED(Ready_Components(&DDesc)))
		assert(false);
	InitializeUi(DDesc.fX, DDesc.fY, DDesc.fZ, DDesc.fSizeX, DDesc.fSizeY, DDesc.fSizeZ, DDesc.fSpeed, DDesc.fColor);

	if (pArg == nullptr)
		assert(false);

	Set_TextureNum(_uint(reinterpret_cast<Desc*>(pArg)->eCutSceneType));

	return S_OK;
}

void CUI_Event_Cut::PriorityTick(_float fTimeDelta)
{
}

void CUI_Event_Cut::Tick(_float fTimeDelta)
{
}

void CUI_Event_Cut::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

shared_ptr<CUI_Event_Cut> CUI_Event_Cut::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUI_Event_Cut
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI_Event_Cut(pDevice, pContext) { }
	};

	shared_ptr<CUI_Event_Cut> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUI_Event_Cut::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUI_Event_Cut
	{
		MakeSharedEnabler(const CUI_Event_Cut& rhs) : CUI_Event_Cut(rhs) { }
	};

	shared_ptr<CUI_Event_Cut> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
