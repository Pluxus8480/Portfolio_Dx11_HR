#include "CUi.h"
#include "GameInstance.h"
#include "Beat_Manager.h"


CUi::CUi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CUi::CUi(const CUi& rhs)
	:CGameObject(rhs)
{
}

CUi::~CUi()
{
	Free();
}


void CUi::Set_Scale(_float SizeX, _float SizeY, _float SizeZ)
{
	_float3 Scale = { SizeX, SizeY, SizeZ };
	m_pTransformCom->Set_Scale(XMLoadFloat3(&Scale));
}

void CUi::Set_Pos(_float fX, _float fY, _float fZ)
{
	_float3 Pos = { fX, fY, fZ };
	m_pTransformCom->Set_Position(XMLoadFloat3(&Pos));
}

void CUi::Set_RenderZ(_uint iOverlapCount)
{
	RenderType Type = Get_RenderType();
	_float3 Pos = Get_Pos();
	_float PosZ = { 0 };

	if (100 <= iOverlapCount)
	{
		iOverlapCount = 99;
	}

	switch (Type)
	{
	case RenderType::Priority:
		PosZ = 0.1f - 0.001f * iOverlapCount;
		Set_Pos(Pos.x, Pos.y, PosZ);
		break;
	case RenderType::Normal:
		PosZ = 0.2f - 0.001f * iOverlapCount;
		Set_Pos(Pos.x, Pos.y, PosZ);
		break;
	case RenderType::BackGround:
		PosZ = 0.3 - 0.001f * iOverlapCount;
		Set_Pos(Pos.x, Pos.y, PosZ);
		break;
	case RenderType::RealBackGround:
		PosZ = 0.4 - 0.001f * iOverlapCount;
		Set_Pos(Pos.x, Pos.y, PosZ);
		break;
	case RenderType::Lowest:
		PosZ = 0.5 - 0.001f * iOverlapCount;
		Set_Pos(Pos.x, Pos.y, PosZ);
		break;
	default:
		break;
	}
}

_float3 CUi::Get_Pos()
{
	_float3 Pos = { 0, 0, 0 };
	XMStoreFloat3(&Pos, m_pTransformCom->Get_Position());
	return Pos;
}

_float3 CUi::Get_Scale()
{
	return m_pTransformCom->Get_Scale();
}

_float2 CUi::Get_CUiDirectCursorPos(HWND HWND)
{
	POINT pPt = {};
	GetCursorPos(&pPt);
	ScreenToClient(HWND, &pPt);
	return Change_DirectCoordinate((float)pPt.x, (float)pPt.y);
}

_float2 CUi::Change_DirectCoordinate(_float fX, _float fY)
{
	_float2 DirectPos = { 0, 0 };
	DirectPos.x = fX - g_iWinSizeX * 0.5f;
	DirectPos.y = -fY + g_iWinSizeY * 0.5f;
	return DirectPos;
}

_float2 CUi::Change_APICoordinate(_float2 DirectPos)
{
	_float2 APIPos = { 0, 0 };
#ifndef PERFECTION_LOAD
	APIPos.x = DirectPos.x + g_iWinSizeX * 0.5f;
	APIPos.y = -DirectPos.y + g_iWinSizeY * 0.5f;
#else
	APIPos.x = (DirectPos.x * 2) + g_iWinSizeX * 0.5f;
	APIPos.y = (- DirectPos.y * 2) + g_iWinSizeY * 0.5f;
#endif 

	return APIPos;
}

void CUi::Normalize_DeltaTime(_float& fTimeDelta)
{
	if (fTimeDelta >= 0.06f)
	{
		fTimeDelta = 0.06f;
	}
}

void CUi::Initialize_Transform()
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&TransformDesc)))
		assert(false);
}

void CUi::InitializeUi(_float fX, _float fY, _float fZ, _float fSizeX, _float fSizeY, _float fSizeZ, _float fSpeed, _float4 fColor)
{
	Set_Scale(fSizeX, fSizeY, fSizeZ);
	Set_Pos(fX, fY, fZ);
	m_fSpeed = fSpeed;
	m_fColor = fColor;
}

void CUi::InitializeRenderType(RenderType Type, _int Priority)
{
	if (Type == RenderType::End)
		assert(false);

	m_eRenderType = Type;
	m_iRenderPriority = Priority;
}

_float CUi::BeatPercent()
{
	return CBeat_Manager::Get_Instance()->Get_BeatRate();
}

_float2 CUi::Adjust_RealWorld(_vector WorldPos, _float fDirectX, _float fDirectY)
{
	_vector ProjPos = WorldPos;
	auto pGameInstance = m_pGameInstance.lock();
	ProjPos = XMVector3TransformCoord(ProjPos, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	ProjPos = XMVector3TransformCoord(ProjPos, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));


#ifdef PERFECTION_LOAD
	_float fFinalX = XMVectorGetX(ProjPos) * g_iWinSizeX * 0.25f;
	_float fFinalY = XMVectorGetY(ProjPos) * g_iWinSizeY * 0.25f;
#else
	_float fFinalX = XMVectorGetX(ProjPos) * g_iWinSizeX * 0.5f;
	_float fFinalY = XMVectorGetY(ProjPos) * g_iWinSizeY * 0.5f;
#endif
	fFinalX += fDirectX;
	fFinalY += fDirectY;

	return _float2(fFinalX, fFinalY);
}
