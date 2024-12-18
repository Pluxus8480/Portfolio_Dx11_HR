#include "CUi_TramGauge.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "iostream"

wstring CUi_TramGauge::ObjID = L"CUi_TramGauge";

CUi_TramGauge::CUi_TramGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_TramGauge::CUi_TramGauge(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_TramGauge::~CUi_TramGauge()
{
}

HRESULT CUi_TramGauge::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TramGauge",
			L"../Bin/Resources/Textures/Ui/Tram/LiftBar.png"));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TramGaugeBack",
			L"../Bin/Resources/Textures/Ui/Tram/LiftBarBg.png"));

	return S_OK;
}

HRESULT CUi_TramGauge::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	m_pBoneptr = Arg->pBonePtr;
	m_ePartPos = Arg->PartPos;
	m_pPartHp = Arg->pHp;
	m_PartMaxHp = *m_pPartHp;
	Initialize_Gauge();
	Initialize_Rotation();
	Initialize_Back();


	return S_OK;
}

void CUi_TramGauge::PriorityTick(_float fTimeDelta)
{
}

void CUi_TramGauge::Tick(_float fTimeDelta)
{
	Cal_Percent();
}

void CUi_TramGauge::LateTick(_float fTimeDelta)
{
	Sync_Pos();
	Adjust_Back();
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_TramGauge::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", m_fPercent)))
		assert(false);
	
	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);
	
	Render_Back();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", Get_TextureNum())))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	return S_OK;
}

void CUi_TramGauge::Render_Back()
{
	if (FAILED(m_pBackTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pBackTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", Get_TextureNum())))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)ShaderPass::Default)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);
}

void CUi_TramGauge::Render_String()
{
}

void CUi_TramGauge::Initialize_Gauge()
{
	CUi_2D::Desc Desc;
	Desc.fSizeX = 9;
	Desc.fSizeY = 4;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::TramHpBar;
	Desc.TextureFileName = L"Prototype_Component_Texture_TramGauge";
	Desc.TextureLevel = LEVEL_STATIC;
	if (FAILED(Ready_Components(&Desc)))
		assert(false);

	InitializeUi(Desc.fX, Desc.fY, Desc.fZ, Desc.fSizeX, Desc.fSizeY, Desc.fSizeZ, Desc.fSpeed, Desc.fColor);
}

void CUi_TramGauge::Initialize_Rotation()
{
	switch (m_ePartPos)
	{
	case CBrake_Part::FRONT_LEFT:
		m_pTransformCom->Rotation(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(261));
		break;
	case CBrake_Part::FRONT_RIGHT:
		m_pTransformCom->Rotation(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(81));
		break;
	case CBrake_Part::BACK_LEFT:
		m_pTransformCom->Rotation(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(261));
		break;
	case CBrake_Part::BACK_RIGHT:
		m_pTransformCom->Rotation(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(81));
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_TramGauge::Initialize_Back()
{
	m_pBackTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_TramGaugeBack",
		L"Com_BackTexture"));
	if (nullptr == m_pBackTextureCom)
		assert(false);

	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_pBackTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pBackTransformCom)
		assert(false);
	if (nullptr != Find_Component(L"Com_BackTransform"))
		assert(false);
	m_Components.emplace(L"Com_BackTransform", m_pBackTransformCom);
}

void CUi_TramGauge::Sync_Pos()
{
	_float3 Pos = {};
	if (m_pBoneptr == nullptr)
		assert(false);
	Pos.x = (*m_pBoneptr).m[3][0];
	Pos.y = (*m_pBoneptr).m[3][1];
	Pos.z = (*m_pBoneptr).m[3][2];

	switch (m_ePartPos)
	{
	case CBrake_Part::FRONT_LEFT:
		Pos.x += 4.5f;
		Pos.y += 28;
		break;
	case CBrake_Part::FRONT_RIGHT:
		Pos.x += -4.5f;
		Pos.y += 28;
		break;
	case CBrake_Part::BACK_LEFT:
		Pos.x += 4.5f;
		Pos.y += 28;
		break;
	case CBrake_Part::BACK_RIGHT:
		Pos.x += -4.5f;
		Pos.y += 28;
		break;
	default:
		assert(false);
		break;
	}
	Set_Pos(Pos.x, Pos.y, Pos.z);
}

void CUi_TramGauge::Adjust_Back()
{
	m_pBackTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
	_float3 BackPos;
	XMStoreFloat3(&BackPos, m_pBackTransformCom->Get_Position());

	switch (m_ePartPos)
	{
	case CBrake_Part::FRONT_LEFT:
		BackPos.x += -0.001f;
		break;
	case CBrake_Part::FRONT_RIGHT:
		BackPos.x += 0.001f;
		break;
	case CBrake_Part::BACK_LEFT:
		BackPos.x += -0.001f;
		break;
	case CBrake_Part::BACK_RIGHT:
		BackPos.x += 0.001f;
		break;
	default:
		break;
	}

	m_pBackTransformCom->Set_Position(XMLoadFloat3(&BackPos));
}

void CUi_TramGauge::Cal_Percent()
{
	m_fPercent = *m_pPartHp / _float(m_PartMaxHp);
}

shared_ptr<CUi_TramGauge> CUi_TramGauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TramGauge
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TramGauge(pDevice, pContext) { }
	};

	shared_ptr<CUi_TramGauge> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TramGauge::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TramGauge
	{
		MakeSharedEnabler(const CUi_TramGauge& rhs) : CUi_TramGauge(rhs) { }
	};

	shared_ptr<CUi_TramGauge> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
