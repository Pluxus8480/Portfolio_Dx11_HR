#include "ZipLine_Obstacle.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"

#include "MapManager.h"
#include "Cine_Manager.h"

wstring CZipLine_Obstacle::ObjID = TEXT("CZipLine_Obstacle");
CZipLine_Obstacle::CZipLine_Obstacle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{pDevice, pContext}
{
}

CZipLine_Obstacle::CZipLine_Obstacle(const CZipLine_Obstacle& rhs)
    : CPartObject{rhs}
{
}

CZipLine_Obstacle::~CZipLine_Obstacle()
{
    Free();
}

CZipLine_Obstacle::ZIPOBS_INFO CZipLine_Obstacle::Get_ObstacleInfo()
{
    ZIPOBS_INFO tInfo; 
    tInfo.eDir = m_eDirection; 
    tInfo.vWorldPosition = { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };

    return tInfo;
}

HRESULT CZipLine_Obstacle::Initialize_Prototype()
{
    _float4x4 fMat = {};
    //XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(+90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RailObstacle_L"), 
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_MagnetRailObstacle_Container_L_KZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RailObstacle_R"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_MagnetRailObstacle_Container_R_KZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_RailObstacle_Front"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_MagnetRailObstacle_Container_QTE_KZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
    );

    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ShortMagnet"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_MagnetRailObstacle_ShortMagnet_KZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("RailContainer"), TEXT("../Bin/Resources/Sounds/MapObject/RailContainer.ogg")));

    return S_OK;
}

HRESULT CZipLine_Obstacle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    ZIPOBS_DESC* pDesc = static_cast<ZIPOBS_DESC*>(pArg);
    m_eDirection = pDesc->eDir;
    m_pTransformCom->Set_WorldMatrix(pDesc->localMat);
    m_iRailIndex = pDesc->iRailIndex;

    Ready_Components();

    return S_OK;
}

void CZipLine_Obstacle::PriorityTick(_float fTimeDelta)
{
}

void CZipLine_Obstacle::Tick(_float fTimeDelta)
{
    SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
    XMStoreFloat4x4(&m_ContainerWorld, XMLoadFloat4x4(&m_ContainerMatrix) * XMLoadFloat4x4(&m_WorldMatrix));

    m_pColliderCom->Update(XMLoadFloat4x4(&m_ContainerWorld));
}

void CZipLine_Obstacle::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_GIMMICK, m_pColliderCom);
    //if(pGI->isIn_Frustum_WorldSpace())

    if(m_bRender)
        pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CZipLine_Obstacle::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom[MAGNET]->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom[MAGNET]->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin((_uint)MESH_PASS::Default);
        m_pModelCom[MAGNET]->Render((_uint)i);
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ContainerWorld)))
        return E_FAIL;

    iNumMeshes = m_pModelCom[CONTAINER]->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom[CONTAINER]->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin((_uint)MESH_PASS::Default);
        m_pModelCom[CONTAINER]->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CZipLine_Obstacle::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"RailContainer");
    m_pGameInstance.lock()->Set_Sound_Position(SOUND_CHANNEL::SOUND_EFFECT, L"RailContainer", 0.35f);
    m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, L"RailContainer", 0.8f);
}

void CZipLine_Obstacle::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        switch (m_eDirection)
        {
        case Client::CZipLine_Obstacle::OBS_DIR::LEFT:
        {
            if (GAMEINSTANCE->Get_KeyDown(DIK_A))
                m_bEvade = true;
            
            break;
        }
        case Client::CZipLine_Obstacle::OBS_DIR::RIGHT:
        {
            if (GAMEINSTANCE->Get_KeyDown(DIK_D))
                m_bEvade = true;

            break;
        }
        case Client::CZipLine_Obstacle::OBS_DIR::FRONT:
        {
            if (GAMEINSTANCE->Get_KeyDown(DIK_SPACE))
                m_bEvade = true;

            break;
        }
        }
    }
}

void CZipLine_Obstacle::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        if(!m_bEvade)
        {
            DamageInfo damageInfo = {};
            damageInfo.pObjectOwner = shared_from_this();
            damageInfo.eAttackType = EAttackType::ZiplineHit;
            damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
            damageInfo.fAmountDamage = 10.f;
            pOther->Get_Owner()->TakeDamage(damageInfo);
        }
    }
}


void CZipLine_Obstacle::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom[MAGNET] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShortMagnet"), TEXT("Model00_Com")));

    switch (m_eDirection)
    {
    case Client::CZipLine_Obstacle::OBS_DIR::LEFT:
        m_pModelCom[CONTAINER] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RailObstacle_L"), TEXT("Model01_Com")));
        break;
    case Client::CZipLine_Obstacle::OBS_DIR::RIGHT:
        m_pModelCom[CONTAINER] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RailObstacle_R"), TEXT("Model01_Com")));
        break;
    case Client::CZipLine_Obstacle::OBS_DIR::FRONT:
        m_pModelCom[CONTAINER] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_RailObstacle_Front"), TEXT("Model01_Com")));
        break;
    }
    
    if (!m_pShaderCom || !m_pModelCom[MAGNET] || !m_pModelCom[CONTAINER])
        assert(false);

    _matrix RotMat = XMMatrixIdentity();
    _float fOffsetY = { 0 };
    _float RotZ = { 0.f };
    switch (m_eDirection)
    {
    case Client::CZipLine_Obstacle::OBS_DIR::LEFT:
        RotMat = XMMatrixRotationZ(+m_fRotRadian);
        RotZ = +m_fRotRadian;
        fOffsetY = -5.2f;
        break;
    case Client::CZipLine_Obstacle::OBS_DIR::RIGHT:
        RotMat = XMMatrixRotationZ(-m_fRotRadian);
        RotZ = -m_fRotRadian;
        fOffsetY = -5.2f;
        break;
    case Client::CZipLine_Obstacle::OBS_DIR::FRONT:
        fOffsetY = -4.6f;
        break;
    }

    XMStoreFloat4x4(&m_ContainerMatrix, RotMat * XMMatrixTranslation(0.f, fOffsetY, 0.f));

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vExtents = { 3.5f, 2.f, 2.5f };
    obbDesc.vCenter = { 0.f, 2.f, -4.f };
    obbDesc.vRotation = { 0.f, 0.f, -RotZ };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

    assert(m_pColliderCom);
}

HRESULT CZipLine_Obstacle::Bind_ShaderResources()
{
    auto pGI = m_pGameInstance.lock();

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    //	return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CZipLine_Obstacle> CZipLine_Obstacle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CZipLine_Obstacle
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CZipLine_Obstacle(pDevice, pContext)
        {}
    };


    shared_ptr<CZipLine_Obstacle> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CZipLine_Obstacle"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CZipLine_Obstacle::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CZipLine_Obstacle
    {
        MakeSharedEnabler(const CZipLine_Obstacle& rhs) : CZipLine_Obstacle(rhs)
        {}
    };

    shared_ptr<CZipLine_Obstacle> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CZipLine_Obstacle"));
        assert(false);
    }

    return pInstance;
}

void CZipLine_Obstacle::Free()
{
}
