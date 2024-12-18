#include "CKorsicaCamera.h"
#include "GameInstance.h"
#include "iostream"

wstring CKorsicaCamera::ObjID = L"CKorsicaCamera";

CKorsicaCamera::CKorsicaCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
	
}

CKorsicaCamera::CKorsicaCamera(const CKorsicaCamera& rhs)
	: CCamera(rhs)
{
}

CKorsicaCamera::~CKorsicaCamera()
{
}

HRESULT CKorsicaCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKorsicaCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		assert(false);

	_float4 Pos = { 0.f, 0.f, 10.f, 1.f };
	m_pTransformCom->Set_Position(XMLoadFloat4(&Pos));

	return S_OK;
}

void CKorsicaCamera::PriorityTick(_float fTimeDelta)
{
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

	_long	MouseMove = {};

	if (m_pGameInstance.lock()->Get_MousePressing(DIMK_RBUTTON))
	{
		POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
		ClientToScreen(g_hWnd, &ptWindow);
		SetCursorPos(ptWindow.x, ptWindow.y);
	}
	



	/*if (m_bNextFollow)
	{
		NextFollow_Bone(fTimeDelta);
	}
	else if (m_bFollowBone)
	{
		Follow_Bone(fTimeDelta);
	}*/

	if (m_bFollowBone)
	{
		Follow_Bone(fTimeDelta);
	}

	_Shake(fTimeDelta);
	Apply_Effect();

	__super::PriorityTick(fTimeDelta);
}

void CKorsicaCamera::Tick(_float fTimeDelta)
{
}

void CKorsicaCamera::LateTick(_float fTimeDelta)
{
}

HRESULT CKorsicaCamera::Render()
{
	return S_OK;
}

void CKorsicaCamera::Start_Following_Bone(_float4x4* pWorldMatrix, _float4x4* pBoneMatrix)
{
	m_pFollowWorld = pWorldMatrix;
	m_pBoneToFollow = pBoneMatrix;
}

void CKorsicaCamera::Follow_Bone(_float fTimeDelta)
{
	_matrix FollowNoScale = XMLoadFloat4x4(m_pBoneToFollow);
	for (_uint i = 0; i < 3; ++i)
	{
		FollowNoScale.r[i] = XMVector3Normalize(FollowNoScale.r[i]);
	}
	_matrix FollowingBone = XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90.f), XMConvertToRadians(90.f)) * FollowNoScale * XMLoadFloat4x4(m_pFollowWorld);
	if (m_fLerpTime > 0.f)
	{
		_float fSqrLerp = (m_fLerpTime * 2.f) * (m_fLerpTime * 2.f);
		FollowingBone = XMLoadFloat4x4(&m_OriginMatrix)* (fSqrLerp) + FollowingBone * (1.f - fSqrLerp);
		m_fLerpTime -= fTimeDelta;
	}

	m_pTransformCom->Set_WorldMatrix(FollowingBone);

}

void CKorsicaCamera::_Shake(_float fTimeDelta)
{
	auto& effectVec = m_arrEffect[CCamera_Player::CE_SHAKE];

	auto iter = effectVec.begin();
	if (iter == effectVec.end())
		return;

	_matrix Shake = XMMatrixTranslation(GAMEINSTANCE->Random_Float(-iter->fPower, iter->fPower), GAMEINSTANCE->Random_Float(-iter->fPower, iter->fPower), 0.f);
	XMStoreFloat4x4(&m_EffectOffset, XMLoadFloat4x4(&m_EffectOffset) * Shake);

	if (iter->fTimer <= 0.f)
	{
		iter = effectVec.erase(iter);
		return;
	}
	iter->fTimer -= fTimeDelta;
}

void CKorsicaCamera::Apply_Effect()
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_EffectOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
	XMStoreFloat4x4(&m_EffectOffset, XMMatrixIdentity());
}

void CKorsicaCamera::Set_NextBoneFollow(_bool NextFollow, _float4x4 NextMatrix)
{
	m_bNextFollow = NextFollow;
	m_OriginMatrix = m_pTransformCom->Get_WorldMatrix();
	m_pNextCameraPos = { NextMatrix.m[3][0], NextMatrix.m[3][1], NextMatrix.m[3][2] };
}

void CKorsicaCamera::NextFollow_Bone(_float fTimeDelta)
{
	m_fNextFollowTime += fTimeDelta;
	if (m_fNextFollowTime > 0.5f)
	{
		m_bNextFollow = false;
		m_fNextFollowTime = 0;
		return;
	}
	_float4x4 Matrix = m_pTransformCom->Get_WorldMatrix();
	XMVECTOR CurScale;
	XMVECTOR CurRotation;
	XMVECTOR CurTranslation;
	XMMatrixDecompose(&CurScale, &CurRotation, &CurTranslation, XMLoadFloat4x4(&Matrix));

	XMVECTOR NextScale;
	XMVECTOR NextRotation;
	XMVECTOR NextTranslation;
	XMMatrixDecompose(&NextScale, &NextRotation, &NextTranslation, XMLoadFloat4x4(&m_NextBone));

	XMVECTOR interpolatedRotation = XMQuaternionSlerp(CurRotation, NextRotation, m_fNextFollowTime * 2);
	XMVECTOR interpolatedPosition = XMVectorLerp(CurTranslation, NextTranslation, m_fNextFollowTime * 2);
	XMMATRIX newMatrix = XMMatrixScalingFromVector(NextScale) * XMMatrixRotationQuaternion(interpolatedRotation) * XMMatrixTranslationFromVector(NextTranslation);
	m_pTransformCom->Set_WorldMatrix(newMatrix);
}

void CKorsicaCamera::Shake(_float fDuration, _float fPower, _bool bClear)
{
	if (bClear)
		m_arrEffect[CCamera_Player::CE_SHAKE].clear();
	m_arrEffect[CCamera_Player::CE_SHAKE].push_back({ fDuration, fPower, fDuration });
}

void CKorsicaCamera::Start_Lerp()
{
	XMStoreFloat4x4(&m_OriginMatrix, m_pTransformCom->Get_WorldMatrix_XMMat());
	m_fLerpTime = 0.2f;
}


shared_ptr<CKorsicaCamera> CKorsicaCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CKorsicaCamera
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKorsicaCamera(pDevice, pContext) { }
	};

	shared_ptr<CKorsicaCamera> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKorsicaCamera"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CKorsicaCamera::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CKorsicaCamera
	{
		MakeSharedEnabler(const CKorsicaCamera& rhs) : CKorsicaCamera(rhs) { }
	};

	shared_ptr<CKorsicaCamera> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKorsicaCamera"));
		assert(false);
	}

	return pInstance;
}