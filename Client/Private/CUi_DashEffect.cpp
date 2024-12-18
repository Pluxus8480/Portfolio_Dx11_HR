#include "CUi_DashEffect.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_DashEffect::ObjID = L"CUi_DashEffect";

CUi_DashEffect::CUi_DashEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Canvas(pDevice, pContext)
{
}

CUi_DashEffect::CUi_DashEffect(const CUi_DashEffect& rhs)
	:CUi_Canvas(rhs)
{
}

CUi_DashEffect::~CUi_DashEffect()
{
}

HRESULT CUi_DashEffect::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_DashEffect", L"../Bin/Resources/Textures/Ui/DashEffect/DashEffect_%d.png", 6)
	);

	return S_OK;
}

HRESULT CUi_DashEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	Create_DashEffect();

	return S_OK;
}

void CUi_DashEffect::Tick(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= m_fIntervalTime)
	{
		m_fElapsedTime = 0.0f;
		m_iCurrentTextureIndex = (m_iCurrentTextureIndex + 1) % 6;

		m_pDashEffect->Set_TextureNum(m_iCurrentTextureIndex);
	}
}

void CUi_DashEffect::Dying()
{
	m_pDashEffect->Dead();
}

void CUi_DashEffect::Set_Active(_bool Active)
{
	m_pDashEffect->Set_Active(Active);
}

void CUi_DashEffect::Create_DashEffect()
{
	CUi_Sprite::Desc   DashEffectDesc{};
	DashEffectDesc.fX = 0.f;
	DashEffectDesc.fY = 0.f;
	DashEffectDesc.eRenderType = RenderType::Priority;
	DashEffectDesc.iPriority = 10;
	DashEffectDesc.fSizeX = 1280.f;
	DashEffectDesc.fSizeY = 720.f;
	DashEffectDesc.fSizeZ = 1.f;
	DashEffectDesc.fSpeed = 0.f;
	DashEffectDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	DashEffectDesc.TextureFileName = L"Prototype_Component_Texture_DashEffect";
	DashEffectDesc.TextureLevel = LEVEL_STATIC;
	DashEffectDesc.iTextureNum = 0;
	DashEffectDesc.ShaderPass = ShaderPass::Blend;
	m_pDashEffect = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, m_iLevelIndex, L"CUi", CUi_Sprite::ObjID, &DashEffectDesc));
}

shared_ptr<CUi_DashEffect> CUi_DashEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_DashEffect
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_DashEffect(pDevice, pContext) { }
	};

	shared_ptr<CUi_DashEffect> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_DashEffect::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_DashEffect
	{
		MakeSharedEnabler(const CUi_DashEffect& rhs) : CUi_DashEffect(rhs) { }
	};

	shared_ptr<CUi_DashEffect> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
