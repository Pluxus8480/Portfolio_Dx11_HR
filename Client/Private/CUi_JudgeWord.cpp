#include "CUi_JudgeWord.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_JudgeWord::ObjID = L"CUi_JudgeWord";

CUi_JudgeWord::CUi_JudgeWord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Default(pDevice, pContext)
{
}

CUi_JudgeWord::CUi_JudgeWord(const CUi_JudgeWord& rhs)
	:CUi_Default(rhs)
{
}

CUi_JudgeWord::~CUi_JudgeWord()
{
}

HRESULT CUi_JudgeWord::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_CommandIconWord",
			L"../Bin/Resources/Textures/Ui/CommandIcon/IconWord%d.png", 3));

	return S_OK;
}

HRESULT CUi_JudgeWord::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc OriginDesc;
	OriginDesc.fSizeX = 100;
	OriginDesc.fSizeY = 40;
	OriginDesc.eRenderType = RenderType::Priority;
	OriginDesc.iPriority = 10;
	OriginDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	OriginDesc.ShaderPass = ShaderPass::Blend;
	OriginDesc.TextureFileName = L"Prototype_Component_Texture_CommandIconWord";
	OriginDesc.TextureLevel = LEVEL_STATIC;

	if (FAILED(Ready_Components(&OriginDesc)))
		assert(false);

	InitializeRenderType(OriginDesc.eRenderType, OriginDesc.iPriority);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, OriginDesc.fSizeX, OriginDesc.fSizeY, OriginDesc.fSizeZ, Arg->fSpeed, Arg->fColor);
	switch (Arg->eWordType)
	{
	case WordType::Perfect:
		Set_TextureNum(_uint(WordType::Perfect));
		break;
	case WordType::Good:
		Set_TextureNum(_uint(WordType::Good));
		break;
	case WordType::Miss:
		Set_TextureNum(_uint(WordType::Miss));
		break;
	default:
		assert(false);
		break;
	}

	m_iWordDir = m_pGameInstance.lock()->Random_Int(0, 2);


	return S_OK;
}

void CUi_JudgeWord::PriorityTick(_float fTimeDelta)
{
}

void CUi_JudgeWord::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	if (m_fTime < 0.2f)
	{
		Add_Scale(fTimeDelta * 500, fTimeDelta * 200);
		if (m_fOriginScale.x < Get_Scale().x)
		{
			Set_Scale(m_fOriginScale.x, m_fOriginScale.y);
		}
		Set_Speed(1500);

		switch (m_iWordDir)
		{
		case 0:
			Move(MoveDir::Right, fTimeDelta);
			Move(MoveDir::Up, fTimeDelta * 0.25f);
			break;
		case 1:
			Move(MoveDir::Right, fTimeDelta);
			break;
		case 2:
			Move(MoveDir::Right, fTimeDelta);
			Move(MoveDir::Down, fTimeDelta * 0.25f);
			break;
		default:
			assert(false);
			break;
		}
	}
	else if (m_fTime < 0.6f)
	{
		Add_Color(ColorType::A, -fTimeDelta * 3);
	}
	else
	{
		Dead();
	}
}

void CUi_JudgeWord::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

shared_ptr<CUi_JudgeWord> CUi_JudgeWord::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_JudgeWord
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_JudgeWord(pDevice, pContext) { }
	};

	shared_ptr<CUi_JudgeWord> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_JudgeWord::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_JudgeWord
	{
		MakeSharedEnabler(const CUi_JudgeWord& rhs) : CUi_JudgeWord(rhs) { }
	};

	shared_ptr<CUi_JudgeWord> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
