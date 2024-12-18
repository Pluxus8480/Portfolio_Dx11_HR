#include "CUi_PlayerHp.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_PlayerHp::ObjID = L"CUi_PlayerHp";

CUi_PlayerHp::CUi_PlayerHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Sprite(pDevice, pContext)
{
}

CUi_PlayerHp::CUi_PlayerHp(const CUi_PlayerHp& rhs)
	:CUi_Sprite(rhs)
{
}

CUi_PlayerHp::~CUi_PlayerHp()
{
}

HRESULT CUi_PlayerHp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_PlayerHp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	return S_OK;
}

void CUi_PlayerHp::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Cal_DecreaseTimeHp(fTimeDelta);
}

void CUi_PlayerHp::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUi_PlayerHp::Render()
{
	if (FAILED(m_pShaderCom->Bind_Float("g_fPlayerHpPercent", GetHpPercent())))
		assert(false);

	if (m_bPlayerHpDecreased)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_PlayerTexture", Get_TextureNum())))
			assert(false);
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bPlayerDecreased", &m_bPlayerHpDecreased, sizeof(_bool))))
		assert(false);

	if (FAILED(__super::Render()))
		assert(false);

	return S_OK;
}

_uint CUi_PlayerHp::Get_TextureNum() const
{
	auto iTextureNum = __super::Get_TextureNum();

	if (m_bPlayerHpDecreased)
	{
		iTextureNum += 10;
	}

	return iTextureNum;
}

void CUi_PlayerHp::Set_PercentHp(_float fPercent)
{
	if (fPercent < m_fPercentHp)
		m_bPlayerHpDecreased = true;
	else
		m_bPlayerHpDecreased = false;

	m_fPercentHp = fPercent;
}

void CUi_PlayerHp::Cal_DecreaseTimeHp(_float fTimeDelta)
{
	if (m_bPlayerHpDecreased)
	{
		m_fDecTimeHp += fTimeDelta;
		if (m_fDecTimeHp > 0.2f)
		{
			m_fDecTimeHp = 0;
			m_bPlayerHpDecreased = false;
		}
	}
}

_float CUi_PlayerHp::GetHpPercent()
{
	auto iTextureNum = __super::Get_TextureNum();

	float fPercentMaxPos = 0.f;

	vector<_float> vecPosition;

	vecPosition.emplace_back(55.f);
	vecPosition.emplace_back(140.f);
	vecPosition.emplace_back(225.f);
	vecPosition.emplace_back(317.f);
	vecPosition.emplace_back(391.f);
	vecPosition.emplace_back(478.f);
	vecPosition.emplace_back(561.f);
	vecPosition.emplace_back(634.f);
	vecPosition.emplace_back(723.f);
	vecPosition.emplace_back(800.f);
	vecPosition.emplace_back(877.f);

	fPercentMaxPos = (((vecPosition[iTextureNum + 1] - vecPosition[0]) * m_fPercentHp + vecPosition[0]) / 1024.f) * 1.03f;

	return fPercentMaxPos;
}

shared_ptr<CUi_PlayerHp> CUi_PlayerHp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerHp
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerHp(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerHp> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerHp::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerHp
	{
		MakeSharedEnabler(const CUi_PlayerHp& rhs) : CUi_PlayerHp(rhs) { }
	};

	shared_ptr<CUi_PlayerHp> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
