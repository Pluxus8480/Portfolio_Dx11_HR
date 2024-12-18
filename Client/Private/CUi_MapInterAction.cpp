#include "CUi_MapInterAction.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "PlayerManager.h"
#include "iostream"

wstring CUi_MapInterAction::ObjID = L"CUi_MapInterAction";

CUi_MapInterAction::CUi_MapInterAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_MapInterAction::CUi_MapInterAction(const CUi_MapInterAction& rhs)
	:CUi_2D(rhs)
{
}

CUi_MapInterAction::~CUi_MapInterAction()
{

}

HRESULT CUi_MapInterAction::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
	make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_InterActionSign",
		L"../Bin/Resources/Textures/Ui/MapInterAction/MapInterAction%d.png", 6));

	return S_OK;
}

HRESULT CUi_MapInterAction::Initialize(void* pArg)
{
	if (pArg == nullptr)
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	Initialize_Arg(Arg);
	Initialize_Transform();
	Initialize_Component();
	InitializeUi(0, 0, 0.3f, Arg->Size.x, Arg->Size.y);

	return S_OK;
}

void CUi_MapInterAction::PriorityTick(_float fTimeDelta)
{
}

void CUi_MapInterAction::Tick(_float fTimeDelta)
{
	Check_Target();
	//Maybe_PlayerUse();
}

void CUi_MapInterAction::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	if (!pGI->isIn_Frustum_WorldSpace(m_pTargetTransform->Get_Position(), 1.5f))
		return;
	if (m_bActive && m_bOwnActive && m_bPhaseActive)
	{
		_float2 Pos;
		if (m_pTargetTransform)
		{
			Pos = Adjust_RealWorld(m_pTargetTransform->Get_Position(), m_RelativeDistance.x, m_RelativeDistance.y);
		}
		else
		{
			Pos = Adjust_RealWorld(XMLoadFloat3(&m_FixPos), m_RelativeDistance.x, m_RelativeDistance.y);		
		}
		Set_Pos(Pos.x, Pos.y);
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}


void CUi_MapInterAction::Check_Target()
{
	_vector Pos = m_pTargetTransform->Get_Position();
	_float Distance = PLAYERMANAGER->Get_DistancePlayer(m_pTargetTransform);

	if (Distance <= m_fAwareDistance && m_fWorkDistance < Distance )
	{
		m_bOwnActive = true;
		Set_TextureNum(0);
	}
	else if (Distance <= m_fWorkDistance && m_fInactiveDistance <= Distance)
	{
		m_bOwnActive = true;
		Set_TextureNum((_uint)m_iWorkTextureIndex);
	}
	else
	{
		m_bOwnActive = false;
	}
}

void CUi_MapInterAction::Initialize_Arg(Desc* Arg)
{
	if (Arg->OwnerTransform)
	{
		m_pTargetTransform = Arg->OwnerTransform;
	}
	else
	{
		m_FixPos = Arg->FixPosition;
	}
	m_RelativeDistance = Arg->RelativeDistance;
	m_fAwareDistance = Arg->fAwareDistance;
	m_fWorkDistance = Arg->fWorkDistance;
	m_iWorkTextureIndex = Arg->iWorkTextureIndex;
	m_fInactiveDistance = Arg->fInactiveDistance;
	if (m_iWorkTextureIndex == WorkTexture::Wire)
	{
		m_fInactiveDistance = 5;
	}
}

void CUi_MapInterAction::Initialize_Component()
{
	CUi_2D::Desc Desc;
	Desc.fSizeX = 100;
	Desc.fSizeY = 100;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_InterActionSign";
	Desc.TextureLevel = LEVEL_STATIC;

	if (FAILED(Ready_Components(&Desc)))
		assert(false);
}

//void CUi_MapInterAction::Maybe_PlayerUse(_bool PlayerUseActive)
//{
//	_vector Pos = m_pTargetTransform->Get_Position();
//	_float Distance = PLAYERMANAGER->Get_DistancePlayer(m_pTargetTransform);
//	if (Distance < 3)
//	{
//		m_bOwnActive = false;
//		Set_Active(false);
//	}
//}

void CUi_MapInterAction::Phase_Active(_bool PhaseActive)
{
	m_bPhaseActive = PhaseActive;
}

shared_ptr<CUi_MapInterAction> CUi_MapInterAction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_MapInterAction
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_MapInterAction(pDevice, pContext) { }
	};

	shared_ptr<CUi_MapInterAction> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_MapInterAction::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_MapInterAction
	{
		MakeSharedEnabler(const CUi_MapInterAction& rhs) : CUi_MapInterAction(rhs) { }
	};

	shared_ptr<CUi_MapInterAction> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
