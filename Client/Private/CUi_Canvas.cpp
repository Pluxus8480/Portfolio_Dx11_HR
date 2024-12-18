#include "CUi_Canvas.h"

wstring CUi_Canvas::ObjID = L"CUi_Canvas";

CUi_Canvas::CUi_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_Canvas::CUi_Canvas(const CUi_Canvas& rhs)
	:CUi_2D(rhs)
{
}

CUi_Canvas::~CUi_Canvas()
{
}

HRESULT CUi_Canvas::Initialize(void* pArg)
{
	Initialize_Transform();

	auto pCanvasSharedThis = dynamic_pointer_cast<CUi_Canvas>(shared_from_this());
	if (pCanvasSharedThis != nullptr)
	{
		CUi_Mgr::Get_Instance()->AddCanvase(pCanvasSharedThis);
	}

	return S_OK;
}

