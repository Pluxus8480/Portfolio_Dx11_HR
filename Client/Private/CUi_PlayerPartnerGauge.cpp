#include "CUi_PlayerPartnerGauge.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUI_Move.h"
#include "CUI_Scale.h"
#include "CUI_Frame.h"
#include "CUI_Sequence.h "
#include "PlayerManager.h"
#include "CUI_CallFunction.h"

wstring CUi_PlayerPartnerGauge::ObjID = L"CUi_PlayerPartnerGauge";

CUi_PlayerPartnerGauge::CUi_PlayerPartnerGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Sprite(pDevice, pContext)
{
}

CUi_PlayerPartnerGauge::CUi_PlayerPartnerGauge(const CUi_PlayerPartnerGauge& rhs)
	:CUi_Sprite(rhs)
{
}

CUi_PlayerPartnerGauge::~CUi_PlayerPartnerGauge()
{
}

HRESULT CUi_PlayerPartnerGauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_PlayerPartnerGauge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	m_iPartnerIndex = Arg->iPartnerIndex;

	CUi_Sprite::Desc PartnerCirDesc{};
	PartnerCirDesc.fX = Get_Pos().x -25.f;
	PartnerCirDesc.fY = Get_Pos().y;
	PartnerCirDesc.eRenderType = RenderType::Priority;
	PartnerCirDesc.iPriority = m_iRenderPriority;
	PartnerCirDesc.fSizeX = 130.f;
	PartnerCirDesc.fSizeY = 130.f;
	PartnerCirDesc.fSizeZ = 1.f;
	PartnerCirDesc.fSpeed = 0.f;
	PartnerCirDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PartnerCirDesc.TextureFileName = L"Prototype_Component_Texture_PartnerExplosionCircle";
	PartnerCirDesc.TextureLevel = LEVEL_STATIC;
	PartnerCirDesc.iTextureNum = 0;
	PartnerCirDesc.ShaderPass = ShaderPass::Default;
	m_pPartnerCir = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PartnerCirDesc));

	CUi_Sprite::Desc PartnerExpDesc{};
	PartnerExpDesc.fX = Get_Pos().x -25.f;
	PartnerExpDesc.fY = Get_Pos().y;
	PartnerExpDesc.eRenderType = RenderType::Priority;
	PartnerExpDesc.iPriority = m_iRenderPriority;
	PartnerExpDesc.fSizeX = 1.f;
	PartnerExpDesc.fSizeY = 1.f;
	PartnerExpDesc.fSizeZ = 1.f;
	PartnerExpDesc.fSpeed = 0.f;
	PartnerExpDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PartnerExpDesc.TextureFileName = L"Prototype_Component_Texture_PartnerExplosion";
	PartnerExpDesc.TextureLevel = LEVEL_STATIC;
	PartnerExpDesc.iTextureNum = 0;
	PartnerExpDesc.ShaderPass = ShaderPass::Default;
	m_pPartnerExp = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PartnerExpDesc));

	CUi_Sprite::Desc PartnerReadyDesc{};
	PartnerReadyDesc.fX = Get_Pos().x -25.f;
	PartnerReadyDesc.fY = Get_Pos().y;
	PartnerReadyDesc.eRenderType = RenderType::Priority;
	PartnerReadyDesc.iPriority = m_iRenderPriority;
	PartnerReadyDesc.fSizeX = 130.f;
	PartnerReadyDesc.fSizeY = 130.f;
	PartnerReadyDesc.fSizeZ = 1.f;
	PartnerReadyDesc.fSpeed = 0.f;
	PartnerReadyDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PartnerReadyDesc.TextureFileName = L"Prototype_Component_Texture_PartnerReady";
	PartnerReadyDesc.TextureLevel = LEVEL_STATIC;
	PartnerReadyDesc.iTextureNum = 0;
	PartnerReadyDesc.ShaderPass = ShaderPass::Default;
	m_pPartnerReady = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &PartnerReadyDesc));

	if (m_pPartnerCir != nullptr)
	{
		m_pPartnerCir->SetVisible(false);
	}

	if (m_pPartnerExp != nullptr)
	{
		m_pPartnerExp->SetVisible(false);
	}

	if (m_pPartnerReady != nullptr)
	{
		m_pPartnerReady->Set_TextureNum(Arg->iReadyTextureNum);
		m_pPartnerReady->SetVisible(false);
	}

	return S_OK;
}

void CUi_PlayerPartnerGauge::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Partner_Respawn(fTimeDelta);

	if (m_pPartnerCir != nullptr)
	{
		m_pPartnerCir->Set_Pos(Get_Pos().x - 25.f, Get_Pos().y);
	}

	if (m_pPartnerExp != nullptr)
	{
		m_pPartnerExp->Set_Pos(Get_Pos().x - 25.f, Get_Pos().y);
	}

	if (m_pPartnerReady != nullptr)
	{
		m_pPartnerReady->Set_Pos(Get_Pos().x - 25.f, Get_Pos().y);
	}
}

void CUi_PlayerPartnerGauge::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUi_PlayerPartnerGauge::Render()
{
	if (FAILED(m_pShaderCom->Bind_Float("g_fPlayerPartnerPercent", m_fProgress)))
		assert(false);

	if (FAILED(__super::Render()))
		assert(false);

	return S_OK;
}

void CUi_PlayerPartnerGauge::Dying()
{
	m_pPartnerCir->Dead();
	m_pPartnerExp->Dead();
	m_pPartnerReady->Dead();
}

void CUi_PlayerPartnerGauge::Set_Active(_bool Active)
{
	m_bActive = Active;
	m_pPartnerCir->Set_Active(Active);
	m_pPartnerExp->Set_Active(Active);
	m_pPartnerReady->Set_Active(Active);
}

void CUi_PlayerPartnerGauge::Partner_Respawn(_float fTimeDelta)
{
	if (PLAYERMANAGER->Get_Assist_Struct(m_iPartnerIndex).bIs_CoolTime && m_bRespawn)
	{
			m_fProgress = 0.f;
			m_fRespawnTime = PLAYERMANAGER->Get_Assist_Struct(m_iPartnerIndex).fCurr_CoolTime;
			m_InitCoolTime = PLAYERMANAGER->Get_Assist_Struct(m_iPartnerIndex).fInitCoolTime;

			m_fRespawnTime += fTimeDelta;
			m_fProgress = m_fRespawnTime / m_InitCoolTime;

			if (m_fProgress >= 1.0f)
			{
				m_fProgress = 1.0f;
				m_fRespawnTime = 0.0f;
				m_bRespawn = false;

				PlayAnimation(CUI_Frame::Create(0, 8, 1.f));

				if (m_pPartnerCir != nullptr)
				{
					m_pPartnerCir->SetVisible(true);
					m_pPartnerCir->Set_RenderPriority(m_iRenderPriority + 1);
					m_pPartnerCir->PlayAnimation(CUI_Sequence::Create(CUI_Scale::Create(_float3(1.f, 1.f, 1.f), 0.25f),
						                                              CUI_Scale::Create(_float3(130.f, 130.f, 130.f), 0.1f),
						                                              CUI_SequenceEnd::Create()));
					m_pPartnerCir->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pPartnerCir->SetVisible(false);
						}, 0.25f));
						                                            
				}

				if (m_pPartnerExp != nullptr)
				{
					m_pPartnerExp->Set_RenderPriority(m_iRenderPriority + 1);
					m_pPartnerExp->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pPartnerExp->SetVisible(true);
						}, 0.25f));

					m_pPartnerExp->PlayAnimation(CUI_Sequence::Create(CUI_Move::Create(_float3(Get_Pos().x - 25.f, Get_Pos().y, 1.f), 0.25f),
						                         CUI_Scale::Create(_float3(130.f, 130.f, 1.f), 0.25f),
						                         CUI_Scale::Create(_float3(1.f, 1.f, 1.f), 0.1f),
						                         CUI_SequenceEnd::Create()));
		

					m_pPartnerExp->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pPartnerExp->SetVisible(false);
						}, 0.5f));

				}

				if (m_pPartnerReady != nullptr)
				{
					m_pPartnerReady->Set_RenderPriority(m_iRenderPriority + 1);
					m_pPartnerReady->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pPartnerReady->SetVisible(true);
						}, 0.25f));

					m_pPartnerReady->PlayAnimation(CUI_CallFunction::Create([=]()
						{
							m_pPartnerReady->SetVisible(false);
						}, 1.f));
				}
			}
	}
}

void CUi_PlayerPartnerGauge::OnPartnerChange(int Index, _float3 InPosition, _float3 InScale, float InTime)
{
	PlayAnimation(CUI_Move::Create(InPosition, InTime));
	PlayAnimation(CUI_Scale::Create(InScale, InTime));

	if (m_pPartnerReady != nullptr)
	{
		m_pPartnerReady->PlayAnimation(CUI_Scale::Create(Index == 0 ? _float3(130, 130, 1) : _float3(90, 90, 1), InTime));
	}
}

shared_ptr<CUi_PlayerPartnerGauge> CUi_PlayerPartnerGauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerPartnerGauge
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerPartnerGauge(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerPartnerGauge> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerPartnerGauge::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerPartnerGauge
	{
		MakeSharedEnabler(const CUi_PlayerPartnerGauge& rhs) : CUi_PlayerPartnerGauge(rhs) { }
	};

	shared_ptr<CUi_PlayerPartnerGauge> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
