#include "SpectraMeshGate.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "PartDoor.h"

wstring CSpectraMeshGate::ObjID = TEXT("CSpectraMeshGate");

CSpectraMeshGate::CSpectraMeshGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CSpectraMeshGate::CSpectraMeshGate(const CSpectraMeshGate& rhs)
    : CGimmickBase{rhs}
{
}

CSpectraMeshGate::~CSpectraMeshGate()
{
    Free();
}

void CSpectraMeshGate::Execute(_bool bExecute)
{
    m_bExecuted = bExecute;

    static_pointer_cast<CPartDoor>(m_pPartDoor)->Execute_Door(bExecute);
}

void CSpectraMeshGate::Set_Dead(bool bDead)
{
    if (m_bIsDead == bDead) return;

    m_bIsDead = bDead;
    m_pPartDoor->Set_Dead(bDead);
}

HRESULT CSpectraMeshGate::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CKaleMap_FirstGimmicDoor"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SpectraConstr_GateB_IZ_01.fbx"),
            MODEL_TYPE::NONANIM, fMat)
    );

    return S_OK;
}

HRESULT CSpectraMeshGate::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    return S_OK;
}

void CSpectraMeshGate::PriorityTick(_float fTimeDelta)
{
    m_pPartDoor->PriorityTick(fTimeDelta);
}

void CSpectraMeshGate::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
        Execute();
#endif

    m_pPartDoor->Tick(fTimeDelta);
}

void CSpectraMeshGate::LateTick(_float fTimeDelta)
{
    m_pPartDoor->LateTick(fTimeDelta);
}

HRESULT CSpectraMeshGate::Render()
{
    return S_OK;
}

HRESULT CSpectraMeshGate::Ready_Parts()
{
    CPartDoor::DOOR_PARTDESC partDesc{};
    partDesc.bOpen = false;
    partDesc.eDir = CPartDoor::DOWN;
    partDesc.fMoveDist = 10.f;
    partDesc.fRotationPerSec = 0.f;
    partDesc.fSpeedPerSec = 15.f;
    partDesc.pParentTransform = m_pTransformCom;
    partDesc.strModelTag = TEXT("Prototype_Component_Model_CKaleMap_FirstGimmicDoor");
    partDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    partDesc.worldMatrix = Identity;

    //m_pPartDoor = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &partDesc);
    m_pPartDoor = GAMEINSTANCE->Get_Object_Prototype(CPartDoor::ObjID)->Clone(&partDesc);

    assert(m_pPartDoor);

    static_pointer_cast<CPartDoor>(m_pPartDoor)->Init_DoorState(false);

    return S_OK;
}

shared_ptr<CSpectraMeshGate> CSpectraMeshGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CSpectraMeshGate
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSpectraMeshGate(pDevice, pContext)
        {}
    };

    shared_ptr<CSpectraMeshGate> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSpectraMeshGate"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CSpectraMeshGate::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CSpectraMeshGate
    {
        MakeSharedEnabler(const CSpectraMeshGate& rhs) : CSpectraMeshGate(rhs)
        {}
    };

    shared_ptr<CSpectraMeshGate> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSpectraMeshGate"));
        assert(false);
    }

    return pInstance;
}

void CSpectraMeshGate::Free()
{
}
