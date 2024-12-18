#include "CUi_Font.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_Font::ObjID = L"CUi_Font";

CUi_Font::CUi_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Sprite(pDevice, pContext)
{
}

CUi_Font::CUi_Font(const CUi_Font& rhs)
	:CUi_Sprite(rhs)
{
}

CUi_Font::~CUi_Font()
{
}

HRESULT CUi_Font::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_Font::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	Set_Font(Arg->Font);

	return S_OK;
}

void CUi_Font::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUi_Font::Render()
{
	if (FAILED(__super::Render()))
		assert(false);

	RenderFont(m_FontDesc);

	return S_OK;
}

void CUi_Font::Set_Font(FontDescInfo Info)
{
	m_FontDesc = Info;
}

void CUi_Font::Set_Text(_wstring InText)
{
	m_FontDesc.String = InText;
}

shared_ptr<CUi_Font> CUi_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_Font
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_Font(pDevice, pContext) { }
	};

	shared_ptr<CUi_Font> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_Font::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_Font
	{
		MakeSharedEnabler(const CUi_Font& rhs) : CUi_Font(rhs) { }
	};

	shared_ptr<CUi_Font> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
