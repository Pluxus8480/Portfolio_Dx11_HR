#include "CUi_StunGaugeSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_3DDefault.h"
#include "iostream"
#include "PlayerManager.h"


wstring CUi_StunGaugeSystem::ObjID = L"CUi_StunGaugeSystem";

CUi_StunGaugeSystem::CUi_StunGaugeSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StunGaugeSystem::CUi_StunGaugeSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StunGaugeSystem::~CUi_StunGaugeSystem()
{
}

HRESULT CUi_StunGaugeSystem::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StunGaugeBar",
			L"../Bin/Resources/Textures/Ui/StunGauge/StunGaugeBar%d.png", 2));
	
	m_List_Owning_ObjectID.push_back(CUi_3DDefault::ObjID);

	return S_OK;
}

HRESULT CUi_StunGaugeSystem::Initialize(void* pArg)
{
	if (pArg == nullptr)
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	m_fMaxStunGauge = Arg->fMaxStunGauge;
	m_pCurStunGauge = Arg->pCurStunGauge;
	m_pTargetTransform = Arg->pTargetTransform;
	m_RelativeDistance = Arg->RelativeDistance;
	m_iStarBackTextureNum = Arg->iStar;
	Initialize_Transform();
	Initiailize_Body();
	Initialize_Back();
	Initialize_Star();
	Initialize_StarBack();
	m_pTransformCom->Set_Speed(1);
	m_bSort = false;


	return S_OK;
}

void CUi_StunGaugeSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_StunGaugeSystem::Tick(_float fTimeDelta)
{
	Progress_State(fTimeDelta);
}

void CUi_StunGaugeSystem::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		auto pGI = m_pGameInstance.lock();
		if (pGI->isIn_Frustum_WorldSpace(m_pTargetTransform->Get_Position(), 3))
		{
			Adjust_Pos();
			Adjust_Scale();
			
			pGI->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
			m_pStar->Set_Active(true);
			m_pStarBack->Set_Active(true);
			m_pBack->Set_Active(true);
		}
		else
		{
			m_pStar->Set_Active(false);
			m_pStarBack->Set_Active(false);
			m_pBack->Set_Active(false);
		}
	}
}

HRESULT CUi_StunGaugeSystem::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_fProgress", m_fPercent * 0.25f + 0.25f)))
		assert(false);
	
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

void CUi_StunGaugeSystem::Progress_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case State::Default:
		if (*m_pCurStunGauge == 0)
			Set_State(State::Stun);
		Cal_Percent();
		break;
	case State::Stun:
		if (!(*m_pCurStunGauge == 0))
			Set_State(State::StuntoDefault);
		break;
	case State::StuntoDefault:
		m_fPercent -= fTimeDelta;
		if (m_fPercent <= 0)
			Set_State(State::Default);
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_StunGaugeSystem::Set_State(State eState)
{
	switch (eState)
	{
	case State::Default:
		m_pBack->Set_Active(true);
		break;
	case State::Stun:
		m_pBack->Set_Active(false);
		break;
	case State::StuntoDefault:
		m_pBack->Set_Active(true);
		m_fPercent = 1;
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_StunGaugeSystem::Initiailize_Body()
{
	CUi_3DDefault::Desc Desc;
	Desc.fSizeX = m_OriginSize.x;
	Desc.fSizeY = m_OriginSize.y;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 10;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::StunGaugeBar;
	Desc.TextureFileName = L"Prototype_Component_Texture_StunGaugeBar";
	Desc.TextureLevel = LEVEL_STATIC;

	if (FAILED(Ready_Components(&Desc)))
		assert(false);

	InitializeRenderType(Desc.eRenderType, Desc.iPriority);
	InitializeUi(Desc.fX, Desc.fY, Desc.fZ, Desc.fSizeX, Desc.fSizeY, Desc.fSizeZ, Desc.fSpeed, Desc.fColor);
	m_bSort = false;
}

void CUi_StunGaugeSystem::Initialize_Back()
{
	CUi_3DDefault::Desc Desc;
	Desc.fSizeX = m_OriginSize.x;
	Desc.fSizeY = m_OriginSize.y;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StunGaugeBarBack";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBack = static_pointer_cast<CUi_3DDefault>(m_pGameInstance.lock()->
		Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_3DDefault::ObjID, &Desc));
}

void CUi_StunGaugeSystem::Initialize_Star()
{
	CUi_3DDefault::Desc Desc;
	Desc.fSizeX = m_OriginSize.x;
	Desc.fSizeY = m_OriginSize.y;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 5;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StunGaugeStar";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pStar = static_pointer_cast<CUi_3DDefault>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_3DDefault::ObjID, &Desc));
	m_pStar->Set_TextureNum(m_iStarBackTextureNum-1);
}

void CUi_StunGaugeSystem::Initialize_StarBack()
{
	CUi_3DDefault::Desc Desc;
	Desc.fSizeX = m_OriginSize.x;
	Desc.fSizeY = m_OriginSize.y;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 3;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_StunGaugeBarBase";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pStarBack = static_pointer_cast<CUi_3DDefault>(m_pGameInstance.lock()
		->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_3DDefault::ObjID, &Desc));
}

void CUi_StunGaugeSystem::Cal_Percent()
{
	if (m_fMaxStunGauge == 0 || m_pCurStunGauge == nullptr)
		return;

	m_fPercent = 1 - *m_pCurStunGauge / m_fMaxStunGauge;
}

void CUi_StunGaugeSystem::Adjust_Pos()
{
	_float3 Pos;
	XMStoreFloat3(&Pos, m_pTargetTransform->Get_Position());

	XMVECTOR scale, rotationQuat, translation;
	XMMatrixDecompose(&scale, &rotationQuat, &translation, m_pTargetTransform->Get_WorldMatrix_XMMat());

	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);
	XMMATRIX YRotation = XMMatrixRotationY(XMConvertToRadians(180));
	XMMATRIX newMatrix = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	rotationMatrix *= YRotation;
	newMatrix = rotationMatrix * newMatrix;


	m_pTransformCom->Set_WorldMatrix(newMatrix);
	m_pTransformCom->Go_Right(0.8f + m_RelativeDistance.x);
	m_pTransformCom->Go_Up(1.5f+ m_RelativeDistance.y);

	XMMATRIX OwnerMatrix = m_pTransformCom->Get_WorldMatrix_XMMat();
	m_pStar->Get_Transform()->Set_WorldMatrix(OwnerMatrix);
	m_pStarBack->Get_Transform()->Set_WorldMatrix(OwnerMatrix);
	m_pBack->Get_Transform()->Set_WorldMatrix(OwnerMatrix);

	_float3 Look;
	XMStoreFloat3(&Look, m_pTransformCom->Get_NormLook());
	Pos = m_pStarBack->Get_Pos();
	if (Look.z > 0)
	{
		Pos.z += 0.01f;
		m_pStarBack->Set_Pos(Pos.x, Pos.y, Pos.z);
		m_pBack->Set_Pos(Pos.x, Pos.y, Pos.z);
	}
	else
	{
		Pos.z -= 0.01f;
		m_pStarBack->Set_Pos(Pos.x, Pos.y, Pos.z);
		m_pBack->Set_Pos(Pos.x, Pos.y, Pos.z);
	}


//	m_pStar->Get_Transform()->Go_Right(0.8f);
//	m_pStarBack->Get_Transform()->Go_Right(0.8f);
//	m_pBack->Get_Transform()->Go_Right(0.8f);
//
//
//	m_pStar->Get_Transform()->Go_Up(1.5f);
//	m_pStarBack->Get_Transform()->Go_Up(1.5f);
//	m_pBack->Get_Transform()->Go_Up(1.5f);
}

void CUi_StunGaugeSystem::Adjust_Scale()
{
	_float fDistance = PLAYERMANAGER->Get_DistancePlayer(m_pTargetTransform);
	_float fMul = 1 - fDistance * 0.1f;
	if (fMul <= 0.01)
		fMul = 0.01f;

	//Set_Scale(m_OriginSize.x * fMul, m_OriginSize.y * fMul);
	//m_pStar->Set_Scale(m_OriginSize.x * fMul, m_OriginSize.y * fMul);
	//m_pStarBack->Set_Scale(m_OriginSize.x * fMul, m_OriginSize.y * fMul);
	//m_pBack->Set_Scale(m_OriginSize.x * fMul, m_OriginSize.y * fMul);
}

void CUi_StunGaugeSystem::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pStar->Set_Active(Active);
	m_pStarBack->Set_Active(Active);
	m_pBack->Set_Active(Active);
}

void CUi_StunGaugeSystem::Dying()
{
	if(m_pStar)
		m_pStar->Dead();
	if(m_pStarBack)
		m_pStarBack->Dead();
	if (m_pBack)
		m_pBack->Dead();
}

shared_ptr<CUi_StunGaugeSystem> CUi_StunGaugeSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StunGaugeSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StunGaugeSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_StunGaugeSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StunGaugeSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StunGaugeSystem
	{
		MakeSharedEnabler(const CUi_StunGaugeSystem& rhs) : CUi_StunGaugeSystem(rhs) { }
	};

	shared_ptr<CUi_StunGaugeSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
