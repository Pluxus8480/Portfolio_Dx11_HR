#include "Rekka_Pipe_L.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"

#include "Bounding_Sphere.h"

#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "Effect_Manager.h"


wstring CRekka_Pipe_L::ObjID = TEXT("Rekka_Pipe_L");

CRekka_Pipe_L::CRekka_Pipe_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CRekka_Pipe_L::CRekka_Pipe_L(const CRekka_Pipe_L& rhs)
    : CPartObject(rhs)
{
}

CRekka_Pipe_L::~CRekka_Pipe_L()
{
    Free();
}

HRESULT CRekka_Pipe_L::Initialize_Prototype()
{
    _matrix		LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Rekka_Pipe_L"), TEXT("../Bin/Resources/Models/Monsters/Rekka/Pipe_R.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );

    return S_OK;
}

HRESULT CRekka_Pipe_L::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    m_pBoneMatrix = pDesc->pBoneMatrix;
    m_iLevelIndex = pDesc->iLevelIndex;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matEffect, 1.f);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matEffect, 1.f);
        }
    );

    return S_OK;
}

void CRekka_Pipe_L::PriorityTick(_float fTimeDelta)
{
}

void CRekka_Pipe_L::Tick(_float fTimeDelta)
{
    // Play Anim
    switch (m_curState)
    {
        case EPipeState::Attack: case EPipeState::PreAttack: case EPipeState::FinalAttack:
        {
            m_pModelCom->Set_AnimIndex("em2105_atk-pipe_300", true);

            m_isAttackOn = true;
            m_pCollider_Attack->Update(XMLoadFloat4x4(&m_WorldMatrix));
            break;
        }
        default :
        {
            m_pModelCom->Set_AnimIndex("em2105_idle-pipe_000", false);
            m_isAttackOn = false;
            break;
        }
    }
    _float4x4 fmatResult{};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);

    // Logic
    switch (m_curState)
    {
        case EPipeState::Default: case EPipeState::Disappear:
        {
            _matrix matRotation = XMMatrixRotationZ(XM_PI * 0.5f);
            _matrix matTransform = matRotation * XMMatrixTranslationFromVector(XMLoadFloat4(&m_vDefaultPos));
            XMStoreFloat4x4(&m_WorldMatrix, matTransform);
            break;
        }
        case EPipeState::PreAttack:
        {
            _vector vRekkaPos = m_pParentTransform->Get_Position();
            _vector vCurPos = XMLoadFloat4x4(&m_WorldMatrix).r[3];
            _vector vDirMove = XMVectorSetY(XMVectorSetW(XMVector3Normalize(vCurPos - vRekkaPos), 0.f), 0.f);
                
            _matrix matCurWorld = XMLoadFloat4x4(&m_WorldMatrix);
            _vector vResultPos = matCurWorld.r[3] + vDirMove * fTimeDelta * 4.f;

            auto vTargetRot = XMQuaternionRotationMatrix(XMMatrixRotationX(XM_PIDIV2));
            auto vCurRot = XMQuaternionRotationMatrix(matCurWorld);
            auto vResultRot = XMQuaternionSlerp(vCurRot, vTargetRot, fTimeDelta * 5.f);

            matCurWorld = XMMatrixRotationQuaternion(vResultRot);
            matCurWorld.r[3] = vResultPos;
            XMStoreFloat4x4(&m_WorldMatrix, matCurWorld);
            break;
        }
        case EPipeState::Attack:
        {
            _vector vPlayerPos = PLAYERMANAGER->Get_PlayerPos();
            _vector vCurPos = XMLoadFloat4x4(&m_WorldMatrix).r[3];
            XMStoreFloat4(&m_vDirLauch, XMVectorSetY(XMVectorSetW(XMVector3Normalize(vPlayerPos - vCurPos), 0.f), 0.f));

            if (m_iCurBeatCnt != CBeat_Manager::Get_Instance()->Get_BeatCount())
            {
                m_iCurBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();
                m_iTotalBeatCnt++;
            }
            if (m_iTotalBeatCnt == 2)
                m_curState = EPipeState::FinalAttack;

            // TODO: Effect Rendering

            break;
        }
        case EPipeState::FinalAttack:
        {
            _matrix matCurWorld = XMLoadFloat4x4(&m_WorldMatrix);
            matCurWorld.r[3] += XMLoadFloat4(&m_vDirLauch) * fTimeDelta * m_fLaunchSpeed;
            XMStoreFloat4x4(&m_WorldMatrix, matCurWorld);

            break;
        }
        default:
        {
            _matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);
            for (size_t i = 0; i < 3; i++)
                BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
            SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);
            break;
        }
    }

    // Call all reserved effects
    _matrix matRotation = XMMatrixRotationX(XM_PI * 0.5f);
    _matrix matScale = XMMatrixScaling(1.4f, 1.f, 1.4f);
    _matrix matEffect = matScale * matRotation * XMLoadFloat4x4(&m_WorldMatrix);
    XMStoreFloat4x4(&m_matEffect, matEffect);
    m_pModelCom->PlayReservedCallbacks();
}

void CRekka_Pipe_L::LateTick(_float fTimeDelta)
{
    if (m_isAttackOn)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRekka_Pipe_L::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        if (FAILED(m_pModelCom->Bind_Buffers(i)))
            return E_FAIL;

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_isAttackOn)
        m_pCollider_Attack->Render();
#endif

    return S_OK;
}

void CRekka_Pipe_L::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack)
    {
        DamageInfo damageInfo{};
        damageInfo.eAttackType = EAttackType::Base;
        damageInfo.fAmountDamage = 10.f;
        damageInfo.pObjectOwner = shared_from_this();
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

HRESULT CRekka_Pipe_L::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Rekka_Pipe_L"), TEXT("Com_Model")));

    CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
    sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
    sphereDesc.fRadius = 2.f;
    m_pCollider_Attack =
        dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Attack"), &sphereDesc));
    m_pCollider_Attack->Set_Owner(shared_from_this());
    m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));

    return S_OK;
}

void CRekka_Pipe_L::AttachOn()
{
    m_curState = EPipeState::Attach;
}

void CRekka_Pipe_L::PrepareAttack()
{
    m_curState = EPipeState::PreAttack;
}

void CRekka_Pipe_L::AttackSpecial()
{
    m_curState = EPipeState::Attack;

    m_iCurBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();
    m_iTotalBeatCnt = 0;
    if (CBeat_Manager::Get_Instance()->Get_Timer() > CBeat_Manager::Get_Instance()->Get_BeatCount() * 0.95f)
        m_iTotalBeatCnt -= 1;
}

shared_ptr<CRekka_Pipe_L> CRekka_Pipe_L::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRekka_Pipe_L
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Pipe_L(pDevice, pContext) {}
    };

    shared_ptr<CRekka_Pipe_L> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRekka_Pipe_L"));
        assert(false);
    }
    return pInstance;
}

shared_ptr<CGameObject> CRekka_Pipe_L::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRekka_Pipe_L
    {
        MakeSharedEnabler(const CRekka_Pipe_L& rhs) : CRekka_Pipe_L(rhs) {}
    };

    shared_ptr<CRekka_Pipe_L> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRekka_Pipe_L"));
        assert(false);
    }
    return pInstance;
}

void CRekka_Pipe_L::Free()
{
}
