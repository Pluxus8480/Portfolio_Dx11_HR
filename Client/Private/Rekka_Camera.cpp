#include "Rekka_Camera.h"
#include "GameInstance.h"
#include "iostream"


wstring CRekka_Camera::ObjID = TEXT("RekkaCamera");

CRekka_Camera::CRekka_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CRekka_Camera::CRekka_Camera(const CRekka_Camera& rhs)
    : CCamera(rhs)
{
}

CRekka_Camera::~CRekka_Camera()
{
}

HRESULT CRekka_Camera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRekka_Camera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        assert(false);

    //CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
    //m_pTransformCom->Set_Position(XMLoadFloat4(&pDesc->vPosition));

    return S_OK;
}

void CRekka_Camera::PriorityTick(_float fTimeDelta)
{
    //_Shake(fTimeDelta);
    //Apply_Effect();

    __super::PriorityTick(fTimeDelta);
}

void CRekka_Camera::Tick(_float fTimeDelta)
{
}

void CRekka_Camera::LateTick(_float fTimeDelta)
{
}

HRESULT CRekka_Camera::Render()
{
    return S_OK;
}

void CRekka_Camera::MoveTo(_fvector vTargetPos, _fvector vDir)
{
    m_pTransformCom->Set_Position(vTargetPos);
    m_pTransformCom->LookAt(vTargetPos + vDir);
}

void CRekka_Camera::Start_Following_Bone(_float4x4* pWorldMatrix, _float4x4* pBoneMatrix)
{
    m_pFollowWorld = pWorldMatrix;
    m_pBoneToFollow = pBoneMatrix;
}

void CRekka_Camera::Set_NextBoneFollow(_bool NextFollow, _float4x4 NextMatrix)
{
    m_bNextFollow = NextFollow;
    m_OriginMatrix = m_pTransformCom->Get_WorldMatrix();
    m_pNextCameraPos = { NextMatrix.m[3][0], NextMatrix.m[3][1], NextMatrix.m[3][2] };
}

void CRekka_Camera::Follow_Bone(_float fTimeDelta)
{
    _matrix FollowNoScale = XMLoadFloat4x4(m_pBoneToFollow);
    for (_uint i = 0; i < 3; ++i)
    {
        FollowNoScale.r[i] = XMVector3Normalize(FollowNoScale.r[i]);
    }
    _matrix FollowingBone = XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90.f), XMConvertToRadians(90.f)) * FollowNoScale * XMLoadFloat4x4(m_pFollowWorld);
    m_pTransformCom->Set_WorldMatrix(FollowingBone);
}

void CRekka_Camera::Shake(_float fDuration, _float fPower, _bool bClear)
{
    if (bClear)
        m_arrEffect[CCamera_Player::CE_SHAKE].clear();
    m_arrEffect[CCamera_Player::CE_SHAKE].push_back({ fDuration, fPower, fDuration });
}

void CRekka_Camera::NextFollow_Bone(_float fTimeDelta)
{
	m_fNextFollowTime += fTimeDelta;
	if (m_fNextFollowTime > 0.5f)
	{
		m_bNextFollow = false;
		m_fNextFollowTime = 0;
		cout << "End" << endl;
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

void CRekka_Camera::_Shake(_float fTimeDelta)
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

void CRekka_Camera::Apply_Effect()
{
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_EffectOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
    XMStoreFloat4x4(&m_EffectOffset, XMMatrixIdentity());
}

shared_ptr<CRekka_Camera> CRekka_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler : public CRekka_Camera
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Camera(pDevice, pContext) {}
    };

    shared_ptr<CRekka_Camera> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRekka_Camera"));
        assert(false);
    }
    return pInstance;
}

shared_ptr<CGameObject> CRekka_Camera::Clone(void* pArg)
{
    struct MakeSharedEnabler : public CRekka_Camera
    {
        MakeSharedEnabler(const CRekka_Camera& rhs) : CRekka_Camera(rhs) {}
    };

    shared_ptr<CRekka_Camera> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRekka_Camera"));
        assert(false);
    }
    return pInstance;
}
