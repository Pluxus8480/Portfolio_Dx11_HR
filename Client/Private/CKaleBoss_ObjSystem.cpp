#include "CKaleBoss_ObjSystem.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"
#include "Math_Manager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "CJeongGimmicBaseDefault.h"
#include "iostream"
#include "PlayerManager.h"
#include "CharacterController.h"

wstring CKaleBoss_ObjSystem::ObjID = TEXT("CKaleBoss_ObjSystem");

CKaleBoss_ObjSystem::CKaleBoss_ObjSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CJeongGimmicBase(pDevice, pContext)
{
}

CKaleBoss_ObjSystem::CKaleBoss_ObjSystem(const CKaleBoss_ObjSystem& rhs)
    : CJeongGimmicBase(rhs)
{
}

CKaleBoss_ObjSystem::~CKaleBoss_ObjSystem()
{
    Free();
}

HRESULT CKaleBoss_ObjSystem::Initialize_Prototype()
{
    Initialize_KaleBossResource();
    wstringtostring(CKaleBoss_ObjSystem::ObjID, m_strGimmickID);
    m_List_Owning_ObjectID.push_back(CJeongGimmicBaseDefault::ObjID);

    return S_OK;
}

HRESULT CKaleBoss_ObjSystem::Initialize(void* pArg)
{
    Initialize_Transform();
    Set_Pos(0, -150, 0);
    Initialize_Obj();

    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

    return S_OK;
}

void CKaleBoss_ObjSystem::PriorityTick(_float fTimeDelta)
{
}

void CKaleBoss_ObjSystem::Tick(_float fTimeDelta)
{
    if (m_bMove)
    {
        Move(fTimeDelta);
    }
}

void CKaleBoss_ObjSystem::LateTick(_float fTimeDelta)
{
    Adjust_ObjPos();

    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
}

HRESULT CKaleBoss_ObjSystem::Render()
{
    for (auto& pObj : m_Objs)
    {
        pObj->Render_Reflection();
    }
    return S_OK;
}

void CKaleBoss_ObjSystem::Set_Move(_bool bUp)
{
    if (bUp)
    {
        cout << "Up" << endl;
    }
    else
    {
        cout << "Down" << endl;
    }
 
    m_bUp = bUp;
    m_bMove = true;
}

void CKaleBoss_ObjSystem::Set_KaleInfo(shared_ptr<CTransform> pKaleTransform, shared_ptr<CCharacterController> pKaleCCt)
{
    if (!pKaleTransform || !pKaleCCt)
        assert(false);

    m_pKaleTransform = pKaleTransform;
    m_pKaleCCt = pKaleCCt;
}

void CKaleBoss_ObjSystem::Set_PlayerInfo(shared_ptr<CTransform> pPlayerTransform, shared_ptr<CCharacterController> pPlayerCCt)
{
    if (!pPlayerTransform || !pPlayerCCt)
        assert(false);

    m_pPlayerTransform = pPlayerTransform;
    m_pPlayerCCt = pPlayerCCt;
}

void CKaleBoss_ObjSystem::Move(_float fTimeDelta)
{
    if (m_bUp)
    {
        _float3 Pos = Get_Pos();
        _float MoveDistance = fTimeDelta * m_fSpeed;
        Pos.y += MoveDistance;

        if (Pos.y >= m_fUpLimitPos)
        {
            Set_Pos(Pos.x, m_fUpLimitPos, Pos.z);
            m_bMove = false;
        }
        else
        {
            Set_Pos(Pos.x, Pos.y, Pos.z);
            MoveTransformPosition(MoveDistance);
        }
    }
    else
    {
        _float3 Pos = Get_Pos();
        _float MoveDistance = -fTimeDelta * m_fSpeed;
        Pos.y += MoveDistance;
        if (Pos.y <= m_fDownLimitPos)
        {
            Set_Pos(Pos.x, m_fDownLimitPos, Pos.z);
            MoveTransformPosition(m_fDownLimitPos);
            m_bMove = false;
        }
        else
        {
            Set_Pos(Pos.x, Pos.y, Pos.z);
            MoveTransformPosition(Pos.y);
        }
    }
}

void CKaleBoss_ObjSystem::MoveTransformPosition(_float fYDistance)
{
    if (m_pKaleTransform == nullptr)
        assert(false);
    if (m_pPlayerTransform == nullptr)
        assert(false);

    _float3 KalePos, PlayerPos;
    XMStoreFloat3(&KalePos, m_pKaleTransform->Get_Position());
    XMStoreFloat3(&PlayerPos, m_pPlayerTransform->Get_Position());
   
    KalePos.y = fYDistance;
    PlayerPos.y = fYDistance;
    //PLAYERMANAGER->Get_Player()->Set_Player_Position(XMVectorSetW(XMLoadFloat3(&PlayerPos), 1.f));


    PxVec3T KaleCCtPos = m_pKaleCCt->Get_Controller()->getFootPosition();
    KaleCCtPos.y = fYDistance;
    _float3 AfterKaleCCtPos = { (_float)KaleCCtPos.x, (_float)KaleCCtPos.y, (_float)KaleCCtPos.z };
    m_pKaleCCt->Set_FootPosition(XMMatrixTranslationFromVector(XMLoadFloat3(&AfterKaleCCtPos)));
    m_pKaleTransform->Set_Position(XMLoadFloat3(&KalePos));

    PxVec3T PlayerCCtPos = m_pKaleCCt->Get_Controller()->getFootPosition();
    PlayerCCtPos.y = fYDistance;
    _float3 AfterPlayerCCtPos = { (_float)PlayerCCtPos.x, (_float)PlayerCCtPos.y, (_float)PlayerCCtPos.z };
    m_pPlayerCCt->Set_FootPosition(XMMatrixTranslationFromVector(XMLoadFloat3(&AfterPlayerCCtPos)));
    m_pPlayerTransform->Set_Position(XMLoadFloat3(&PlayerPos));
    PLAYERMANAGER->Get_Player()->Tick_PartObj(0.f);
}

void CKaleBoss_ObjSystem::Initialize_KaleBossResource()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_KaleBossFloor"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SK_bg1591.fbx"),
            MODEL_TYPE::NONANIM, fMat));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CoreA1"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraCoreA_RD_01.fbx"),
            MODEL_TYPE::NONANIM, fMat));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CoreArmA01"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraCoreArmA_RD_01.fbx"),
            MODEL_TYPE::NONANIM, fMat));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CoreMainA01"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraCoreMainA_RD_01.fbx"),
            MODEL_TYPE::NONANIM, fMat));
}

void CKaleBoss_ObjSystem::Initialize_Obj()
{
    m_Objs.reserve(4);


    _float3 Pos = Get_Pos();
    CJeongGimmicBaseDefault::Desc Desc;
    Desc.ModelName = L"Prototype_Component_Model_KaleBossFloor";
    Desc.ShaderName = L"Prototype_Component_Shader_VtxMesh";
    Desc.worldMatrix = Identity;
    Desc.eRenderGroup = CRenderer::RENDERGROUP::RENDER_TERRAIN;
    Desc.Pos = { Pos.x, Pos.y, Pos.z };
    Desc.Scale = { 1, 1, 1 };
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));
 
    Desc.ModelName = L"Prototype_Component_Model_CoreA1";
    _matrix Matrix = XMMatrixIdentity();
    Matrix = XMMatrixMultiply(Matrix, XMMatrixRotationY(XMConvertToRadians(90)));
    XMStoreFloat4x4(&Desc.worldMatrix, Matrix);
    Desc.Pos = { Pos.x - 20, Pos.y, Pos.z };
    Desc.eRenderGroup = CRenderer::RENDERGROUP::RENDER_NONBLEND;
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));

    Desc.ModelName = L"Prototype_Component_Model_CoreArmA01";
    Desc.Pos = { Pos.x - 28, Pos.y, Pos.z + 8 };
    Desc.worldMatrix = Identity;
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));

    Desc.ModelName = L"Prototype_Component_Model_CoreMainA01";
    Desc.Pos = { Pos.x - 28, Pos.y, Pos.z + 8 };
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));
}

void CKaleBoss_ObjSystem::Adjust_ObjPos()
{
    _float3 Pos = Get_Pos();
    m_Objs[0]->Set_Pos(Pos.x, Pos.y, Pos.z);
    m_Objs[1]->Set_Pos(Pos.x - 20, Pos.y, Pos.z);
    m_Objs[2]->Set_Pos(Pos.x - 28, Pos.y, Pos.z + 8);
    m_Objs[3]->Set_Pos(Pos.x - 28, Pos.y, Pos.z + 8);
}

shared_ptr<CKaleBoss_ObjSystem> CKaleBoss_ObjSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CKaleBoss_ObjSystem
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKaleBoss_ObjSystem(pDevice, pContext)
        {}
    };

    shared_ptr<CKaleBoss_ObjSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CKaleBoss_ObjSystem::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CKaleBoss_ObjSystem
    {
        MakeSharedEnabler(const CKaleBoss_ObjSystem& rhs) : CKaleBoss_ObjSystem(rhs)
        {}
    };

    shared_ptr<CKaleBoss_ObjSystem> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CKaleBoss_ObjSystem::Free()
{
}
