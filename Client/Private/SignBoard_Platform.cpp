#include "SignBoard_Platform.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "PlayerManager.h"

#include "SignBoard.h"
#include "MapManager.h"

wstring CSignBoard_Platform::ObjID = TEXT("CSignBoard_Platform");
CSignBoard_Platform::CSignBoard_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{pDevice, pContext}
{
}

CSignBoard_Platform::CSignBoard_Platform(const CSignBoard_Platform& rhs)
	: CGimmickBase{rhs}
{
}

CSignBoard_Platform::~CSignBoard_Platform()
{
	Free();
}

void CSignBoard_Platform::Set_Dead(bool bDead)
{
    m_bIsDead = bDead;

    m_pSignBoard->Set_Dead(bDead);
}

HRESULT CSignBoard_Platform::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SignBoard_Platform"), TEXT("../Bin/Resources/Models/Map/Stage01/SM_St01A2DScroll_SignBoardFrameA_WB_01.FBX"),
            MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"DownStructure1", L"../Bin/Resources/Models/Map/Gimmick/Sound/DownStructure1.ogg", SOUND_CHANNEL::SOUND_EFFECT));

    return S_OK;
}

HRESULT CSignBoard_Platform::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    pDesc->fSpeedPerSec = 3.f;
    m_fSpeedPerSec = 3.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
    m_vOriginPos = { pDesc->worldMatrix._41, pDesc->worldMatrix._42, pDesc->worldMatrix._43 };

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    m_fShakeMinY = m_vOriginPos.y - 0.5f;
    m_fMinPosY = m_vOriginPos.y - 1.f;

	return S_OK;
}

void CSignBoard_Platform::PriorityTick(_float fTimeDelta)
{
    m_pSignBoard->PriorityTick(fTimeDelta);
}

void CSignBoard_Platform::Tick(_float fTimeDelta)
{
    //if (GAMEINSTANCE->Get_KeyDown(DIK_O))
    //    Execute();

    if (m_bExecuted && !m_bDone)
    {
        m_fDelayTime += fTimeDelta;

        if (m_fDelayTime > 0.5f)
            Move(fTimeDelta);
    }

    if (m_bExecuted)
    {
        _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
        Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
        Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
        Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
        Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
        _float4x4 mat;
        XMStoreFloat4x4(&mat, Transpose);
        PxMat44 pxWorld;
        pxWorld.column0.x = mat.m[0][0];
        pxWorld.column0.y = mat.m[0][1];
        pxWorld.column0.z = mat.m[0][2];
        pxWorld.column0.w = mat.m[0][3];
        pxWorld.column1.x = mat.m[1][0];
        pxWorld.column1.y = mat.m[1][1];
        pxWorld.column1.z = mat.m[1][2];
        pxWorld.column1.w = mat.m[1][3];
        pxWorld.column2.x = mat.m[2][0];
        pxWorld.column2.y = mat.m[2][1];
        pxWorld.column2.z = mat.m[2][2];
        pxWorld.column2.w = mat.m[2][3];
        pxWorld.column3.x = mat.m[3][0];
        pxWorld.column3.y = mat.m[3][1];
        pxWorld.column3.z = mat.m[3][2];
        pxWorld.column3.w = mat.m[3][3];

        PxTransform transform{ pxWorld };
        m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

    _matrix partMat = XMLoadFloat4x4(&m_fBoardMat) * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pSignBoard->Get_Transform()->Set_WorldMatrix(partMat);

    m_pSignBoard->Tick(fTimeDelta);
}

void CSignBoard_Platform::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    auto pGI = m_pGameInstance.lock();
    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    pGI->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

    m_pSignBoard->LateTick(fTimeDelta);
}

HRESULT CSignBoard_Platform::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif
    return S_OK;
}

void CSignBoard_Platform::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        if(!m_bDone)
        {

            PLAYERMANAGER->Callback_Event("SignBoard_Platform");
            Execute();
        }
    }
}

HRESULT CSignBoard_Platform::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SignBoard_Platform"), TEXT("Model_Com")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

    m_vModelCenter = { -6.f, 11.9f, 2.f };
    _float3 vModelSize = { 8.7f, 0.1f, 3.6f };

    _float3 vScale = m_pTransformCom->Get_Scale();
    vModelSize = { vModelSize.x * vScale.x, vModelSize.y * vScale.y , vModelSize.z * vScale.z };
    m_vModelCenter = { m_vModelCenter.x * vScale.x, m_vModelCenter.y * vScale.y , m_vModelCenter.z * vScale.z };

    PxVec3 BoxDimension{ vModelSize.x, vModelSize.y, vModelSize.z };
    PxBoxGeometry Box{ BoxDimension };
    string ColliderTag = string(CSignBoard_Platform::ObjID.begin(), CSignBoard_Platform::ObjID.end());
    m_pPhysXCollider->createStaticShapeActor(Box, ColliderTag.c_str());

    _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
    Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
    Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
    Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
    Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
    _float4x4 mat;
    XMStoreFloat4x4(&mat, Transpose);
    PxMat44 pxWorld;
    pxWorld.column0.x = mat.m[0][0];
    pxWorld.column0.y = mat.m[0][1];
    pxWorld.column0.z = mat.m[0][2];
    pxWorld.column0.w = mat.m[0][3];
    pxWorld.column1.x = mat.m[1][0];
    pxWorld.column1.y = mat.m[1][1];
    pxWorld.column1.z = mat.m[1][2];
    pxWorld.column1.w = mat.m[1][3];
    pxWorld.column2.x = mat.m[2][0];
    pxWorld.column2.y = mat.m[2][1];
    pxWorld.column2.z = mat.m[2][2];
    pxWorld.column2.w = mat.m[2][3];
    pxWorld.column3.x = mat.m[3][0];
    pxWorld.column3.y = mat.m[3][1];
    pxWorld.column3.z = mat.m[3][2];
    pxWorld.column3.w = mat.m[3][3];

    PxTransform transform{ pxWorld };
    m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = { -6.f, 13.f, 2.f };
    obbDesc.vExtents = { 8.5f, 2.f, 3.6f };
    obbDesc.vRotation = { 0.f, 0.f, 0.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

    assert(m_pColliderCom);

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
        assert(false);

    return S_OK;
}

HRESULT CSignBoard_Platform::Ready_Parts()
{
    MISCOBJ_DESC miscDesc{};
    miscDesc.worldMatrix = Identity;
    miscDesc.worldMatrix._11 = 1.2f;
    miscDesc.worldMatrix._22 = 1.2f;
    miscDesc.worldMatrix._33 = 1.2f;
    miscDesc.worldMatrix._41 = -6.f;
    miscDesc.worldMatrix._42 = 11.9f;
    miscDesc.worldMatrix._43 = 1.f;

    m_fBoardMat = miscDesc.worldMatrix;

    //m_pSignBoard = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CSignBoard::ObjID, &miscDesc);
    m_pSignBoard = GAMEINSTANCE->Get_Object_Prototype(CSignBoard::ObjID)->Clone(&miscDesc);
    assert(m_pSignBoard);

    static_pointer_cast<CSignBoard>(m_pSignBoard)->Set_TextureIndex(9);

    return S_OK;
}

HRESULT CSignBoard_Platform::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;
    
    return S_OK;
}

void CSignBoard_Platform::Move(_float fTimeDelta)
{
    _vector vCurPos = m_pTransformCom->Get_Position();
    _float fCurY = XMVectorGetY(vCurPos);

    switch (m_eFallSQ)
    {
    case Client::CSignBoard_Platform::SHAKE: {    
        if (m_bDown && !m_bLastShake) {
            if (fabs(fCurY - m_fShakeMinY) < 0.2f) {
                m_pTransformCom->Set_Position(XMVectorSetY(vCurPos, m_fShakeMinY));
                m_bDown = false;
                m_bLastShake = true;
            }
            else {
                m_fSpeedPerSec += 0.2f;
                m_pTransformCom->Set_Speed(m_fSpeedPerSec);
                m_pTransformCom->Go_Down(fTimeDelta);
            }
        }
        else {
            if (fabs(fCurY - m_vOriginPos.y) < 0.2f) {
                m_pTransformCom->Set_Position(XMVectorSetY(vCurPos, m_vOriginPos.y));
                //m_bDown = true;
                m_bLastShake = false;
                m_eFallSQ = FALL;
                m_fSpeedPerSec = 3.f;
                m_pTransformCom->Set_Speed(m_fSpeedPerSec);
            }
            else {
                m_fSpeedPerSec -= 0.2f;
                m_pTransformCom->Set_Speed(m_fSpeedPerSec);
                m_pTransformCom->Go_Up(fTimeDelta);
            }
        }
        break;
    }
    case Client::CSignBoard_Platform::FALL: {
        //m_fRadPerSec += XMConvertToRadians(0.5f);
        //
        //if (XMConvertToRadians(30.f) < m_fCurRotation || fabs(XMConvertToRadians(30.f) - m_fCurRotation) <= XMConvertToRadians(1.f))
        //{
        //    m_pTransformCom->Rotation_Override(m_pTransformCom->Get_NormLook() * (XMConvertToRadians(30.f) - m_fCurRotation));
        //    //m_bDone = true;
        //    m_eFallSQ = REBOUND;
        //    m_fRadPerSec *= 0.5f;
        //}
        //else if (fabs(XMConvertToRadians(30.f) - m_fCurRotation) > XMConvertToRadians(1.f))
        //{
        //    m_fCurRotation += m_fRadPerSec * fTimeDelta;
        //    m_pTransformCom->Turn(m_pTransformCom->Get_NormLook(), m_fRadPerSec, fTimeDelta);
        //}
        
        if (fabs(fCurY - m_fMinPosY) < 0.2f) {
            m_pTransformCom->Set_Position(XMVectorSetY(vCurPos, m_fMinPosY));
            m_eFallSQ = REBOUND;
        }
        else {
            m_fSpeedPerSec += 0.2f;
            m_pTransformCom->Set_Speed(m_fSpeedPerSec);
            m_pTransformCom->Go_Down(fTimeDelta);
        }

        break;
    }
    case Client::CSignBoard_Platform::REBOUND: {
        //if (!m_bDown && !m_bLastShake) {
        //    if (XMConvertToRadians(20.f) > m_fCurRotation || fabs(XMConvertToRadians(20.f) - m_fCurRotation) <= XMConvertToRadians(1.f)) {
        //        m_bDown = true;
        //        m_bLastShake = true;
        //    }
        //    else {
        //        m_fRadPerSec -= XMConvertToRadians(0.5f);
        //        m_fRadPerSec = max(m_fRadPerSec, XMConvertToRadians(8.f));
        //        m_fCurRotation -= m_fRadPerSec * fTimeDelta;
        //        m_pTransformCom->Turn(m_pTransformCom->Get_NormLook(), -m_fRadPerSec, fTimeDelta);
        //    }
        //}
        //else {
        //    if (XMConvertToRadians(30.f) < m_fCurRotation || fabs(XMConvertToRadians(30.f) - m_fCurRotation) <= XMConvertToRadians(1.f)) {
        //        m_eFallSQ = SQ_END;
        //        CGimmickBase::Done();
        //    }
        //    else {
        //        m_fRadPerSec += XMConvertToRadians(0.5f);
        //        m_fCurRotation += m_fRadPerSec * fTimeDelta;
        //        m_pTransformCom->Turn(m_pTransformCom->Get_NormLook(), +m_fRadPerSec, fTimeDelta);
        //    }
        //}

        m_fRadPerSec += XMConvertToRadians(1.f);
        if (XMConvertToRadians(5.f) < m_fCurRotation || fabs(XMConvertToRadians(5.f) - m_fCurRotation) <= XMConvertToRadians(1.f))
        {
            if (!m_bSoundPlayed)
            {
                m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"DownStructure1", m_pTransformCom->Get_Position());
                m_bSoundPlayed = true;
            }
            m_pTransformCom->Rotation_Override(m_pTransformCom->Get_NormLook() * (XMConvertToRadians(5.f) - m_fCurRotation));
            m_eFallSQ = SQ_END;
            CGimmickBase::Done();
        }
        else if (fabs(XMConvertToRadians(5.f) - m_fCurRotation) > XMConvertToRadians(1.f))
        {
            m_fCurRotation += m_fRadPerSec * fTimeDelta;
            m_pTransformCom->Turn(m_pTransformCom->Get_NormLook(), m_fRadPerSec, fTimeDelta);
        }

        break;
    }
    }
}

shared_ptr<CSignBoard_Platform> CSignBoard_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CSignBoard_Platform
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSignBoard_Platform(pDevice, pContext)
        {}
    };

    shared_ptr<CSignBoard_Platform> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSignBoard_Platform"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CSignBoard_Platform::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CSignBoard_Platform
    {
        MakeSharedEnabler(const CSignBoard_Platform& rhs) : CSignBoard_Platform(rhs)
        {}
    };

    shared_ptr<CSignBoard_Platform> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSignBoard_Platform"));
        assert(false);
    }

    return pInstance;
}

void CSignBoard_Platform::Free()
{
}
