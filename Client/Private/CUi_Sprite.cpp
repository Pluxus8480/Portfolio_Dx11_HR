#include "CUi_Sprite.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUI_Animation.h"

wstring CUi_Sprite::ObjID = L"CUi_Sprite";

CUi_Sprite::CUi_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_Sprite::CUi_Sprite(const CUi_Sprite& rhs)
	:CUi_2D(rhs)
{
}

CUi_Sprite::~CUi_Sprite()
{
}

HRESULT CUi_Sprite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_Sprite::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	m_iTextureNum = Arg->iTextureNum;
	m_iMaxFrame = Arg->iMaxFrame;

	return S_OK;
}

void CUi_Sprite::Tick(_float fTimeDelta)
{
	AnimationTick(fTimeDelta);
}

void CUi_Sprite::LateTick(_float fTimeDelta)
{
	if (m_bIsVisible && m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

void CUi_Sprite::PlayAnimation(shared_ptr<class CUI_Animation> InAnimation)
{
	m_AnimationList.emplace_back(InAnimation);

	InAnimation->OnPlayAnimationEvent(shared_from_this());
}

void CUi_Sprite::StopAnimation()
{
	m_AnimationList.clear();
}

void CUi_Sprite::AnimationTick(_float fTimeDelta)
{
	for (auto iter = m_AnimationList.begin(); iter != m_AnimationList.end(); )
	{
		if (!(*iter)->Tick(fTimeDelta))
		{
			iter = m_AnimationList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

shared_ptr<CUi_Sprite> CUi_Sprite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_Sprite
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_Sprite(pDevice, pContext) { }
	};

	shared_ptr<CUi_Sprite> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_Sprite::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_Sprite
	{
		MakeSharedEnabler(const CUi_Sprite& rhs) : CUi_Sprite(rhs) { }
	};

	shared_ptr<CUi_Sprite> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
