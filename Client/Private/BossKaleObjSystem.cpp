#include "BossKaleObjSystem.h"
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

wstring BossKaleObjSystem::ObjID = TEXT("BossKaleObjSystem");

BossKaleObjSystem::BossKaleObjSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CJeongGimmicBase(pDevice, pContext)
{
}

BossKaleObjSystem::BossKaleObjSystem(const BossKaleObjSystem& rhs)
    : CJeongGimmicBase(rhs)
{
}

BossKaleObjSystem::~BossKaleObjSystem()
{
    Free();
}

HRESULT BossKaleObjSystem::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_BossKaleObjSystem"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraConstr_GateB_IZ_01.fbx"),
            MODEL_TYPE::NONANIM, Identity)
    );
    
    return S_OK;
}

HRESULT BossKaleObjSystem::Initialize(void* pArg)
{
    Initialize_Transform();


    return S_OK;
}

void BossKaleObjSystem::PriorityTick(_float fTimeDelta)
{
}

void BossKaleObjSystem::Tick(_float fTimeDelta)
{
}

void BossKaleObjSystem::LateTick(_float fTimeDelta)
{
}

void BossKaleObjSystem::Initialize_Obj()
{
}

void BossKaleObjSystem::Adjust_ObjPos()
{
}

shared_ptr<BossKaleObjSystem> BossKaleObjSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public BossKaleObjSystem
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : BossKaleObjSystem(pDevice, pContext)
        {}
    };

    shared_ptr<BossKaleObjSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : BossKaleObjSystem"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> BossKaleObjSystem::Clone(void* pArg)
{
    struct MakeSharedEnabler :public BossKaleObjSystem
    {
        MakeSharedEnabler(const BossKaleObjSystem& rhs) : BossKaleObjSystem(rhs)
        {}
    };

    shared_ptr<BossKaleObjSystem> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void BossKaleObjSystem::Free()
{
}
