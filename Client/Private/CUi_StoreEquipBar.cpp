#include "CUi_StoreEquipBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "iostream"
#include "CUi_Default.h"


wstring CUi_StoreEquipBar::ObjID = L"CUi_StoreEquipBar";

CUi_StoreEquipBar::CUi_StoreEquipBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_StoreBar(pDevice, pContext)
{
}

CUi_StoreEquipBar::CUi_StoreEquipBar(const CUi_StoreEquipBar& rhs)
	:CUi_StoreBar(rhs)
{
}

CUi_StoreEquipBar::~CUi_StoreEquipBar()
{
}

HRESULT CUi_StoreEquipBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreEquipBar::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	m_fOriginPos = { Get_Pos().x, Get_Pos().y };
	Initialize_DefaultVec();
	Set_StringFont(Arg->StringFont);
	m_fUpLimitPos = Arg->fUpLimitPos;
	m_fDownLimitPos = Arg->fDownLimitPos;

	m_pDefaultVec[0]->Set_Active(false);
	m_pDefaultVec[1]->Set_Active(false);

	return S_OK;
}

void CUi_StoreEquipBar::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreEquipBar::Tick(_float fTimeDelta)
{
	if (m_bActive)
	{
		Check_Scroll();
		Adjust_Pos(fTimeDelta);
		
	}
}

void CUi_StoreEquipBar::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		_float PosX = Get_Pos().x;
		_float PosY = Get_Pos().y;
		m_StringFont.DirectPos.x = PosX - m_fStringFont_Distance.x;
		m_StringFont.DirectPos.y = PosY - m_fStringFont_Distance.y;
		m_pDefaultVec[0]->Set_Active(false);
		m_pDefaultVec[1]->Set_Pos(Get_Pos().x + Get_Scale().x * 0.4f, Get_Pos().y, 0.5f);
		if (m_bFocus)
		{
			if (PLAYERMANAGER->Get_PlayerEquipSkill().SkillName == Get_Name())
			{
				m_pDefaultVec[1]->Set_Active(true);
			}
		}
		else
		{
			m_pDefaultVec[1]->Set_Active(false);
		}

		if (m_bFocus)
			m_iTextureNum = 1;
		else
			m_iTextureNum = 0;

		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_StoreEquipBar::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_StoreBarScrollUpPercent", m_fUpLimitPercent)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Float("g_StoreBarScrollDownPercent", m_fDownLimitPercent)))
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

	if (Get_Color().w == 1)
	{
		RenderFont(m_StringFont);
	}

	return S_OK;
}

void CUi_StoreEquipBar::Set_Active(_bool Active)
{
	m_bActive = Active;
	if (!m_bActive)
	{
		for (auto& iter : m_pDefaultVec)
		{
			iter->Set_Active(Active);
		}
	}
}

shared_ptr<CUi_StoreEquipBar> CUi_StoreEquipBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreEquipBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreEquipBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreEquipBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreEquipBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreEquipBar
	{
		MakeSharedEnabler(const CUi_StoreEquipBar& rhs) : CUi_StoreEquipBar(rhs) { }
	};

	shared_ptr<CUi_StoreEquipBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
