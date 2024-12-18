#include "CUi_GearSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "CUi_Default.h"
#include "iostream"


wstring CUi_GearSystem::ObjID = L"CUi_GearSystem";

CUi_GearSystem::CUi_GearSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_GearSystem::CUi_GearSystem(const CUi_GearSystem& rhs)
	:CUi_2D(rhs)
{
}

CUi_GearSystem::~CUi_GearSystem()
{
}

HRESULT CUi_GearSystem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_GearSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc pDesc;
	pDesc.fSizeX = 200;
	pDesc.fSizeY = 1;
	pDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	pDesc.ShaderPass = ShaderPass::Blend;
	pDesc.TextureFileName = L"Prototype_Component_Texture_GearBack";
	pDesc.TextureLevel = LEVEL_STATIC;
	pDesc.fX = 480;
	pDesc.fY = -300;
	pDesc.fSpeed = 1000;
	pArg = &pDesc;

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	Initialize_GearAddBack();
	Initialize_Font();

	return S_OK;
}

void CUi_GearSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_GearSystem::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		
		Progress_State(fTimeDelta);
	}
}

void CUi_GearSystem::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		if (m_bChanged)
		{
			m_pAddBack->Set_Active(true);
			Adjust_BackPos();
			Adjust_FontPos();
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
		}
	}
}

HRESULT CUi_GearSystem::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	if (m_bRenderFont)
	{
		RenderFont(m_FontDesc);
		RenderFont(m_AddFont);
	}
	return S_OK;
}

void CUi_GearSystem::Progress_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case State::APPEAR:
		Appear(fTimeDelta);
		break;
	case State::ADDSIGN:
		AddSign(fTimeDelta);
		break;
	case State::CAL:
		Cal(fTimeDelta);
		break;
	case State::ADDFONTDISAPPEAR:
		AddFontDisappear(fTimeDelta);
		break;
	case State::DISAPPEAR:
		Disappear(fTimeDelta);
		Check_ChangeGear();
		break;
	default:
		assert(false);
		break;
	}
}

void CUi_GearSystem::Initialize_GearAddBack()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 120;
	Desc.fSizeY = 1;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Normal;
	Desc.iPriority = 0;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_GearAddBg";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pAddBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	m_pAddBack->Set_Active(false);
}

void CUi_GearSystem::Initialize_Font()
{
	m_FontDesc.Color = { 0, 0, 0, 1 };
	m_AddFont.Color = { 1, 1, 0, 1 };
	m_AddFont.Size = 0.6f;
}

void CUi_GearSystem::Check_ChangeGear()
{
	if (m_PreviousGear != PLAYERMANAGER->Get_PlayerGear() && !m_bChanged)
	{
		for (_uint i = 0; i < State::End; ++i)
		{
			m_fTime[i] = 0;
		}

		m_iGetGearCount = PLAYERMANAGER->Get_PlayerGear() - m_PreviousGear;
		m_FontDesc.String = to_wstring(m_PreviousGear);
		_float4 WhiteColor = { 70, 70, 70, 1 };
		_float3 Scale = { 200, 1, 1 };

		Set_Color(WhiteColor);
		Set_Scale(Scale.x, Scale.y);
		Set_Pos(480, -300);

		_float3 BackScale = { 60, 1, 1 };
		m_pAddBack->Set_Color(WhiteColor);
		m_pAddBack->Set_Scale(BackScale.x, BackScale.y);
		Set_State(State::APPEAR);
		m_bChanged = true;

		m_FontDesc.Color = { 0, 0, 0, 1 };
		m_AddFont.Color = { 1, 1, 0, 1 };
		m_bRenderFont = false;
	}
}

void CUi_GearSystem::Appear(_float fTimeDelta)
{
	m_fTime[State::APPEAR] += fTimeDelta;
	_float4 OriginColor = { 1, 1, 1, 1 };
	if (m_fTime[State::APPEAR] < 0.5f)
	{
		_float3 AddScale = Get_Scale();
		_float3 BackScale = m_pAddBack->Get_Scale();
		AddScale.y += fTimeDelta * 200;
		BackScale.y += fTimeDelta * 200;
		Set_Scale(AddScale.x, AddScale.y);
		m_pAddBack->Set_Scale(BackScale.x, BackScale.y);
		if (Get_Scale().y >= 50)
		{
			Set_Scale(200, 50);
			m_pAddBack->Set_Scale(120, 50);
		}

		_float4 Color = { -fTimeDelta * 200, -fTimeDelta * 200, -fTimeDelta * 200, 0 };
		Add_Color(Color);
		m_pAddBack->Add_Color(Color);

		if (Get_Color().x <= 1)
		{
			Set_Color(OriginColor);
			m_pAddBack->Set_Color(OriginColor);
			wstring Temp = L"+";
			m_AddFont.String = Temp;
			m_AddFont.String += to_wstring(m_iGetGearCount);
			Set_State(State::ADDSIGN);
		}
		
	}
	else
	{
		Set_Scale(200, 50);
		m_pAddBack->Set_Scale(120, 50);
		Set_Color(OriginColor);
		wstring Temp = L"+";
		m_AddFont.String = Temp;
		m_AddFont.String += to_wstring(m_iGetGearCount);
		Set_State(State::ADDSIGN);
	}
}

void CUi_GearSystem::AddSign(_float fTimeDelta)
{
	m_fTime[State::ADDSIGN] += fTimeDelta;
	m_bRenderFont = true;
	if (m_fTime[State::ADDSIGN] < 2)
	{
		_int iGetGearCount = PLAYERMANAGER->Get_PlayerGear() - m_PreviousGear;
		if (m_iGetGearCount != iGetGearCount)
		{
			m_iGetGearCount = iGetGearCount;
			wstring Temp = L"+";
			m_AddFont.String = Temp;
			m_AddFont.String += to_wstring(m_iGetGearCount);
			m_fTime[State::ADDSIGN] = 0;
			m_fUpDownTime = 0;
			m_bUpDown = true;
		}

		AddSign_UpDown(fTimeDelta);
	}
	else
	{
		Set_State(State::CAL);
	}
}

void CUi_GearSystem::Cal(_float fTimeDelta)
{
	m_fTime[State::CAL] += fTimeDelta;
	_int Number = m_pGameInstance.lock()->Random_Int(1, 20);
	m_iGetGearCount -= Number;

	if (m_iGetGearCount <= 0)
	{
		m_iGetGearCount = 0;
		m_PreviousGear = PLAYERMANAGER->Get_PlayerGear();
		m_FontDesc.String = to_wstring(m_PreviousGear);
		m_AddFont.String = to_wstring(m_iGetGearCount);
		Set_State(State::ADDFONTDISAPPEAR);
	}
	else
	{
		m_PreviousGear += Number;
		m_FontDesc.String = to_wstring(m_PreviousGear);
		wstring Temp = L"+";
		m_AddFont.String = Temp;
		m_AddFont.String += to_wstring(m_iGetGearCount);

		if (m_fTime[State::CAL] > 2)
		{
			m_iGetGearCount = 0;
			m_PreviousGear = PLAYERMANAGER->Get_PlayerGear();
			m_FontDesc.String = to_wstring(m_PreviousGear);
			m_AddFont.String = to_wstring(m_iGetGearCount);
			Set_State(State::ADDFONTDISAPPEAR);
		}
	}
}

void CUi_GearSystem::AddFontDisappear(_float fTimeDelta)
{
	m_fTime[State::ADDFONTDISAPPEAR] += fTimeDelta;
	m_AddFont.Color.x -= fTimeDelta * 10;
	m_AddFont.Color.y -= fTimeDelta * 10;
	m_AddFont.Color.z -= fTimeDelta * 10;
	m_AddFont.Color.w -= fTimeDelta * 10;

	m_pAddBack->Add_Color(ColorType::A, -fTimeDelta * 5);
	if (m_pAddBack->Get_Scale().x <= 40)
	{
		m_pAddBack->Set_Scale(1, 1, 1);
	}
	else
	{
		m_pAddBack->Add_Scale(-fTimeDelta * 200, -fTimeDelta * 100);
	}

	if (m_fTime[State::ADDFONTDISAPPEAR] > 1.5f)
	{
		Set_State(State::DISAPPEAR);
	}
}

void CUi_GearSystem::Disappear(_float fTimeDelta)
{
	m_fTime[State::DISAPPEAR] += fTimeDelta;
	Move(MoveDir::Right, fTimeDelta);
	Add_Color(ColorType::A, -fTimeDelta * 5);
	if (m_fTime[State::DISAPPEAR] > 1)
	{
		m_bChanged = false;
	}

	if (m_PreviousGear != PLAYERMANAGER->Get_PlayerGear() && m_bChanged)
	{
		_float4 OriginColor = { 1, 1, 1, 1 };

		Set_Color(OriginColor);
		Set_Pos(480, -300);


		m_pAddBack->Set_Scale(120, 50);
		m_pAddBack->Set_Color(OriginColor);

		_float4 AddFontColor = { 1, 1, 0, 1 };
		_float4 FontColor = { 0, 0, 0, 1 };

		m_FontDesc.Color = FontColor;
		m_AddFont.Color = AddFontColor;

		for (_int i = 0; i < State::End; ++i)
		{
			if (i == State::APPEAR)
			{
			}
			else
			{
				m_fTime[i] = 0;
			}
		}

		Set_State(State::APPEAR);
	}
}

void CUi_GearSystem::Adjust_FontPos()
{
	_float3 Pos = Get_Pos();
	_float3 Scale = Get_Scale();
	_float3 AddPos = m_pAddBack->Get_Pos();
	_float3 AddScale = m_pAddBack->Get_Scale();
	m_FontDesc.DirectPos = { Pos.x - Scale.x * 0.1f, Pos.y + Scale.y * 0.35f };
	m_AddFont.DirectPos = { AddPos.x - AddScale.x * 0.35f, AddPos.y + AddScale.y * 0.3f };
}

void CUi_GearSystem::Adjust_BackPos()
{
	_float3 Pos = Get_Pos();
	_float3 Scale = Get_Scale();
	m_pAddBack->Set_Pos(Pos.x + Scale.x * 0.2f, Pos.y + Scale.y * 0.7f);
}

void CUi_GearSystem::AddSign_UpDown(_float fTimeDelta)
{	
	if (m_bUpDown)
	{
		m_fUpDownTime += fTimeDelta;
		if (m_fUpDownTime < 0.15f)
		{
			m_pAddBack->Add_Pos(0, fTimeDelta * 100);
		}
		else if (m_fUpDownTime < 0.3f)
		{
			m_pAddBack->Add_Pos(0, -fTimeDelta * 100);
		}
		else
		{
			m_fUpDownTime = 0;
			m_bUpDown = false;
		}
	}
	else
	{
		_float3 Pos = Get_Pos();
		m_pAddBack->Set_Pos(Pos.x, Pos.y + 50);
	}
}

void CUi_GearSystem::Dying()
{
	m_pAddBack->Dead();
}

shared_ptr<CUi_GearSystem> CUi_GearSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_GearSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_GearSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_GearSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_GearSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_GearSystem
	{
		MakeSharedEnabler(const CUi_GearSystem& rhs) : CUi_GearSystem(rhs) { }
	};

	shared_ptr<CUi_GearSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
