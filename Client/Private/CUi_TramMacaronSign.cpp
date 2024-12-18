#include "CUi_TramMacaronSign.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"


wstring CUi_TramMacaronSign::ObjID = L"CUi_TramMacaronSign";

CUi_TramMacaronSign::CUi_TramMacaronSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_TramMacaronSign::CUi_TramMacaronSign(const CUi_TramMacaronSign& rhs)
	:CUi_2D(rhs)
{
}

CUi_TramMacaronSign::~CUi_TramMacaronSign()
{
}

HRESULT CUi_TramMacaronSign::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TriggerMacaron",
			L"../Bin/Resources/Textures/Ui/Tram/TriggerMacaron.png"));

	return S_OK;
}

HRESULT CUi_TramMacaronSign::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_Sign();

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	m_pBoneptr = Arg->pBonePtr;
	m_ePartPos = Arg->PartPos;
	m_fAwareDistance = Arg->AwareDistance;

	return S_OK;
}

void CUi_TramMacaronSign::PriorityTick(_float fTimeDelta)
{
}

void CUi_TramMacaronSign::Tick(_float fTimeDelta)
{
}

void CUi_TramMacaronSign::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Adjust_Pos();
		if (Judge_InFrustum_Distance())
		{
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
		}
	}
}

void CUi_TramMacaronSign::Initialize_Sign()
{
	CUi_2D::Desc Desc;
	Desc.fSizeX = 50;
	Desc.fSizeY = 50;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 5;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_TriggerMacaron";
	Desc.TextureLevel = LEVEL_STATIC;

	if (FAILED(Ready_Components(&Desc)))
		assert(false);

	InitializeUi(Desc.fX, Desc.fY, Desc.fZ, Desc.fSizeX, Desc.fSizeY, Desc.fSizeZ, Desc.fSpeed, Desc.fColor);
}

void CUi_TramMacaronSign::Adjust_Pos()
{
	_float3 Pos = {};
	Pos.x = m_pBoneptr->_41;
	Pos.y = m_pBoneptr->_42;
	Pos.z = m_pBoneptr->_43;
	switch (m_ePartPos)
	{
	case CBrake_Part::FRONT_LEFT:
		Pos.x += 4.5f;
		Pos.y += 25.f;
		break;
	case CBrake_Part::FRONT_RIGHT:
		Pos.x -= 4.5f;
		Pos.y += 25.f;
		break;
	case CBrake_Part::BACK_LEFT:
		Pos.x += 4.5f;
		Pos.y += 25.f;
		break;
	case CBrake_Part::BACK_RIGHT:
		Pos.x -= 4.5f;
		Pos.y += 25.f;
		break;
	case CBrake_Part::BRAKE_END:
		break;
	default:
		break;
	}
	_float2 RealPos = Adjust_RealWorld(XMLoadFloat3(&Pos), 0, 70);
	Set_Pos(RealPos.x, RealPos.y);
}

_bool CUi_TramMacaronSign::Judge_InFrustum_Distance()
{
	_float3 Pos = {};
	Pos.x = m_pBoneptr->_41;
	Pos.y = m_pBoneptr->_42;
	Pos.z = m_pBoneptr->_43;
	Pos.x += 4.5f;
	Pos.y += 25.f;
	if (m_pGameInstance.lock()->isIn_Frustum_WorldSpace(XMLoadFloat3(&Pos), 1.f))
	{
		if (PLAYERMANAGER->Get_DistancePlayer(Pos) < m_fAwareDistance)
		{
			return true;
		}
	}
	return false;
}

shared_ptr<CUi_TramMacaronSign> CUi_TramMacaronSign::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TramMacaronSign
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TramMacaronSign(pDevice, pContext) { }
	};

	shared_ptr<CUi_TramMacaronSign> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TramMacaronSign::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TramMacaronSign
	{
		MakeSharedEnabler(const CUi_TramMacaronSign& rhs) : CUi_TramMacaronSign(rhs) { }
	};

	shared_ptr<CUi_TramMacaronSign> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
