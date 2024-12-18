#include "CUi_PlayerReverb.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_PlayerReverb::ObjID = L"CUi_PlayerReverb";

CUi_PlayerReverb::CUi_PlayerReverb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Sprite(pDevice, pContext)
{
}

CUi_PlayerReverb::CUi_PlayerReverb(const CUi_PlayerReverb& rhs)
	:CUi_Sprite(rhs)
{
}

CUi_PlayerReverb::~CUi_PlayerReverb()
{
}

HRESULT CUi_PlayerReverb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_PlayerReverb::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	return S_OK;
}

void CUi_PlayerReverb::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Cal_DecreaseTimeReverb(fTimeDelta);
}

void CUi_PlayerReverb::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUi_PlayerReverb::Render()
{
	if (FAILED(m_pShaderCom->Bind_Float("g_fPlayerReverbPercent", m_fPercentReverb)))
		assert(false);

	if (FAILED(__super::Render()))
		assert(false);

	return S_OK;
}

void CUi_PlayerReverb::Set_PercentReverb(_float fPercent)
{
	if (fPercent < m_fPercentReverb)
		m_bPlayerReverbDecreased = true;
	else
		m_bPlayerReverbDecreased = false;

	m_fPercentReverb = fPercent;
}

void CUi_PlayerReverb::Cal_DecreaseTimeReverb(_float fTimeDelta)
{
	if (m_bPlayerReverbDecreased)
	{
		m_fDecTimeReverb += fTimeDelta;
		if (m_fDecTimeReverb > 0.2f)
		{
			m_fDecTimeReverb = 0;
			m_bPlayerReverbDecreased = false;
		}
	}
}

shared_ptr<CUi_PlayerReverb> CUi_PlayerReverb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerReverb
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerReverb(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerReverb> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerReverb::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerReverb
	{
		MakeSharedEnabler(const CUi_PlayerReverb& rhs) : CUi_PlayerReverb(rhs) { }
	};

	shared_ptr<CUi_PlayerReverb> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
