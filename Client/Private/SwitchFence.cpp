#include "SwitchFence.h"
#include "GameInstance.h"

#include "PartDoor.h"

wstring CSwitchFence::ObjID = TEXT("CSwitchFence");

CSwitchFence::CSwitchFence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CSwitchFence::CSwitchFence(const CSwitchFence& rhs)
    : CGimmickBase{rhs}
{
}

CSwitchFence::~CSwitchFence()
{
    Free();
}

void CSwitchFence::Execute(_bool bExecute)
{
    m_bExecuted = bExecute;
    
    static_pointer_cast<CPartDoor>(m_pPartDoor)->Execute_Door(bExecute);
}

void CSwitchFence::Set_Dead(bool bDead)
{
    m_bIsDead = bDead;

    m_pPartDoor->Set_Dead(bDead);
}

HRESULT CSwitchFence::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SwitchFence"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_SwitchFenceAGlass_WB_01.FBX"), 
            MODEL_TYPE::NONANIM, Identity)
    );

    return S_OK;
}

HRESULT CSwitchFence::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    return S_OK;
}

void CSwitchFence::PriorityTick(_float fTimeDelta)
{
    m_pPartDoor->PriorityTick(fTimeDelta);
}

void CSwitchFence::Tick(_float fTimeDelta)
{
    m_pPartDoor->Tick(fTimeDelta);
}

void CSwitchFence::LateTick(_float fTimeDelta)
{
    m_pPartDoor->LateTick(fTimeDelta);
}

HRESULT CSwitchFence::Render()
{
    return S_OK;
}

HRESULT CSwitchFence::Ready_Components()
{
    return S_OK;
}

HRESULT CSwitchFence::Ready_Parts()
{
    CPartDoor::DOOR_PARTDESC partDesc{};
    partDesc.bOpen = false;
    partDesc.eDir = CPartDoor::DOWN;
    partDesc.fMoveDist = 3.f;
    partDesc.fRotationPerSec = 0.f;
    partDesc.fSpeedPerSec = 10.f;
    partDesc.pParentTransform = m_pTransformCom;
    partDesc.strModelTag = TEXT("Prototype_Component_Model_SwitchFence");
    partDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    partDesc.worldMatrix = Identity;

    //m_pPartDoor = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &partDesc);
    m_pPartDoor = GAMEINSTANCE->Get_Object_Prototype(CPartDoor::ObjID)->Clone(&partDesc);

    assert(m_pPartDoor);

    static_pointer_cast<CPartDoor>(m_pPartDoor)->Init_DoorState(false);

    return S_OK;
}

HRESULT CSwitchFence::Bind_ShaderResources()
{
    return S_OK;
}

shared_ptr<CSwitchFence> CSwitchFence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CSwitchFence
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSwitchFence(pDevice, pContext)
        {}
    };

    shared_ptr<CSwitchFence> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSwitchFence"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CSwitchFence::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CSwitchFence
    {
        MakeSharedEnabler(const CSwitchFence& rhs) : CSwitchFence(rhs)
        {}
    };

    shared_ptr<CSwitchFence> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSwitchFence"));
        assert(false);
    }

    return pInstance;
}

void CSwitchFence::Free()
{
}
