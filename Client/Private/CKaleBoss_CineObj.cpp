#include "CKaleBoss_CineObj.h"
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


wstring CKaleBoss_CineObj::ObjID = TEXT("CKaleBoss_CineObj");

CKaleBoss_CineObj::CKaleBoss_CineObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CJeongGimmicBase(pDevice, pContext)
{
}

CKaleBoss_CineObj::CKaleBoss_CineObj(const CKaleBoss_CineObj& rhs)
    : CJeongGimmicBase(rhs)
{
}

CKaleBoss_CineObj::~CKaleBoss_CineObj()
{
    Free();
}

HRESULT CKaleBoss_CineObj::Initialize_Prototype()
{
    Initialize_KaleBossResource();
    wstringtostring(CKaleBoss_CineObj::ObjID, m_strGimmickID);
    m_List_Owning_ObjectID.push_back(CJeongGimmicBaseDefault::ObjID);

    return S_OK;
}

HRESULT CKaleBoss_CineObj::Initialize(void* pArg)
{
    Initialize_Transform();
    if (pArg == nullptr)
    {
        Set_Pos(0, 0, 0);
    }
    else
    {
        Desc* Arg = reinterpret_cast<Desc*>(pArg);
        m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    }
    Initialize_Obj();

    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

    return S_OK;
}

void CKaleBoss_CineObj::PriorityTick(_float fTimeDelta)
{
}

void CKaleBoss_CineObj::Tick(_float fTimeDelta)
{
}

void CKaleBoss_CineObj::LateTick(_float fTimeDelta)
{
    Adjust_ObjPos();
}

void CKaleBoss_CineObj::Initialize_KaleBossResource()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_KaleBossFloorCine"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SK_bg1591.fbx"),
            MODEL_TYPE::NONANIM, fMat));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CoreA1Cine"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraCoreA_RD_01.fbx"),
            MODEL_TYPE::NONANIM, fMat));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CoreArmA01Cine"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraCoreArmA_RD_01.fbx"),
            MODEL_TYPE::NONANIM, fMat));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CoreMainA01Cine"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraCoreMainA_RD_01.fbx"),
            MODEL_TYPE::NONANIM, fMat));
}

void CKaleBoss_CineObj::Initialize_Obj()
{
    m_Objs.reserve(4);

    _float3 Pos = Get_Pos();
    CJeongGimmicBaseDefault::Desc Desc;
    Desc.ModelName = L"Prototype_Component_Model_KaleBossFloorCine";
    Desc.ShaderName = L"Prototype_Component_Shader_VtxMesh";
    Desc.worldMatrix = Identity;
    Desc.eRenderGroup = CRenderer::RENDERGROUP::RENDER_TERRAIN;
    Desc.Pos = { Pos.x, Pos.y, Pos.z };
    Desc.Scale = { 1, 1, 1 };
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));
 
    Desc.ModelName = L"Prototype_Component_Model_CoreA1Cine";
    _matrix Matrix = XMMatrixIdentity();
    Matrix = XMMatrixMultiply(Matrix, XMMatrixRotationY(XMConvertToRadians(90)));
    XMStoreFloat4x4(&Desc.worldMatrix, Matrix);
    Desc.Pos = { Pos.x - 20, Pos.y, Pos.z };
    Desc.eRenderGroup = CRenderer::RENDERGROUP::RENDER_NONBLEND;
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));

    Desc.ModelName = L"Prototype_Component_Model_CoreArmA01Cine";
    Desc.Pos = { Pos.x - 28, Pos.y, Pos.z + 8 };
    Desc.worldMatrix = Identity;
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));

    Desc.ModelName = L"Prototype_Component_Model_CoreMainA01Cine";
    Desc.Pos = { Pos.x - 28, Pos.y, Pos.z + 8 };
    m_Objs.emplace_back(static_pointer_cast<CJeongGimmicBaseDefault>(m_pGameInstance.lock()
        ->Add_Clone_Return(m_iLevelIndex, L"Obj", CJeongGimmicBaseDefault::ObjID, &Desc)));
}

void CKaleBoss_CineObj::Adjust_ObjPos()
{
    _float3 Pos = Get_Pos();
    m_Objs[0]->Set_Pos(Pos.x, Pos.y, Pos.z);
    m_Objs[1]->Set_Pos(Pos.x - 20, Pos.y, Pos.z);
    m_Objs[2]->Set_Pos(Pos.x - 28, Pos.y, Pos.z + 8);
    m_Objs[3]->Set_Pos(Pos.x - 28, Pos.y, Pos.z + 8);
}

shared_ptr<CKaleBoss_CineObj> CKaleBoss_CineObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CKaleBoss_CineObj
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKaleBoss_CineObj(pDevice, pContext)
        {}
    };

    shared_ptr<CKaleBoss_CineObj> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CKaleBoss_CineObj::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CKaleBoss_CineObj
    {
        MakeSharedEnabler(const CKaleBoss_CineObj& rhs) : CKaleBoss_CineObj(rhs)
        {}
    };

    shared_ptr<CKaleBoss_CineObj> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CKaleBoss_CineObj::Free()
{
}
