#include "QACenterDoor.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "PartDoor.h"
#include "DoorTrigger.h"

wstring CQACenterDoor::ObjID = TEXT("CQACenterDoor");

CQACenterDoor::CQACenterDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CQACenterDoor::CQACenterDoor(const CQACenterDoor& rhs)
    : CGimmickBase{rhs}
{
}

CQACenterDoor::~CQACenterDoor()
{
    Free();
}

void CQACenterDoor::Execute_Door()
{
    static_pointer_cast<CPartDoor>(m_vecParts[DOOR_L])->Execute_Door(true);
    static_pointer_cast<CPartDoor>(m_vecParts[DOOR_R])->Execute_Door(true);
    //m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_DoorOpen", m_pTransformCom->Get_Position());
    m_bOpen = !m_bOpen;
}

void CQACenterDoor::Set_Dead(bool bDead)
{
    m_bIsDead = bDead;

    for (auto& pPart : m_vecParts)
        pPart->Set_Dead(bDead);
}

HRESULT CQACenterDoor::Initialize_Prototype()
{
    _float4x4 fMat = {};
    //XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_QACenterDoorFrame"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_ProdDeptQACenterDoorFrameA_MW_01.FBX"), MODEL_TYPE::NONANIM, Identity)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_QACenterDoorR"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_ProdDeptQACenterDoorA_DoorR_MW_01.FBX"), MODEL_TYPE::NONANIM, Identity)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_QACenterDoorL"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_ProdDeptQACenterDoorA_DoorL_MW_01.FBX"), MODEL_TYPE::NONANIM, Identity)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DoorSign_Open"), TEXT("../Bin/Resources/Models/Map/General/t_doorsigna_e_green.dds"))
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DoorSign_Close"), TEXT("../Bin/Resources/Models/Map/General/t_doorsigna_e_red.dds"))
    );
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_DoorOpen", L"../Bin/Resources/Sounds/MapObject/Door_Open.ogg",
        SOUND_CHANNEL::SOUND_EFFECT));
    return S_OK;
}

HRESULT CQACenterDoor::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Parts(pDesc)))
        return E_FAIL;

    //m_bOpen = pDesc->bOpen;
    
    
    return S_OK;
}

void CQACenterDoor::PriorityTick(_float fTimeDelta)
{
    for (auto& pPart : m_vecParts)
        pPart->PriorityTick(fTimeDelta);
}

void CQACenterDoor::Tick(_float fTimeDelta)
{
    //if (GAMEINSTANCE->Get_KeyDown(DIK_L))
    //    Execute_Door();
    //m_bOpen = (static_pointer_cast<CPartDoor>(m_vecParts[DOOR_L])->Get_Open() && static_pointer_cast<CPartDoor>(m_vecParts[DOOR_L])->Get_Open());

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

    for (auto& pPart : m_vecParts)
        pPart->Tick(fTimeDelta);
}

void CQACenterDoor::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLIDER_TYPE::COLLISION_GIMMICK, m_pColliderCom);
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

    for (auto& pPart : m_vecParts)
        pPart->LateTick(fTimeDelta);
}

HRESULT CQACenterDoor::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (i == 0)
        {
            if (FAILED(m_pTextureCom[(m_bOpen) ? OPEN : CLOSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                return E_FAIL;
        }

        m_pShaderCom->Begin(0);
        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CQACenterDoor::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (!m_bReady)
        return;

    if (!m_bDone && pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        Execute_Door();
        m_bDone = true;
        m_bExecuted = false;
    }
}

void CQACenterDoor::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CQACenterDoor::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CQACenterDoor::Edit_Attribute()
{
    ImGui::Checkbox("Open?", &m_bOpen);
    ImGui::Checkbox("Ready?", &m_bReady);
}

void CQACenterDoor::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    write_typed_data(os, m_bOpen);
    write_typed_data(os, m_bReady);
}

void CQACenterDoor::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    read_typed_data(is, m_bOpen);
    read_typed_data(is, m_bReady);

    static_pointer_cast<CPartDoor>(m_vecParts[DOOR_L])->Init_DoorState(m_bOpen);
    static_pointer_cast<CPartDoor>(m_vecParts[DOOR_R])->Init_DoorState(m_bOpen);
}

HRESULT CQACenterDoor::Ready_Parts(const COMMONOBJ_DESC* pDesc)
{
    //CPartDoorFrame::FRAME_PARTDESC frameDesc{};
    //frameDesc.fRotationPerSec = { 0.f };
    //frameDesc.fSpeedPerSec = { 0.f };
    //frameDesc.pParentTransform = m_pTransformCom;
    //frameDesc.strModelTag = TEXT("SM_ProdDeptDoorFrameA_MW_01");
    //frameDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    //shared_ptr<CGameObject> pFrame = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoorFrame::ObjID, &frameDesc);
    //pFrame->Get_Transform()->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pFrame));

    CPartDoor::DOOR_PARTDESC doorDesc{};
    doorDesc.fRotationPerSec = { 0.f };
    doorDesc.fSpeedPerSec = { 10.f };
    doorDesc.pParentTransform = m_pTransformCom;
    doorDesc.strModelTag = TEXT("Prototype_Component_Model_QACenterDoorL");
    doorDesc.vPosition = { -0.f, 0.f, 0.f, 1.f };
    //doorDesc.bOpen = pDesc->bOpen;
    doorDesc.eDir = CPartDoor::DOOR_DIR::LEFT;
    doorDesc.fMoveDist = 8.f;
    XMStoreFloat4x4(&doorDesc.worldMatrix, XMMatrixTranslation(-0.3f, 0.f, 0.f));
    //shared_ptr<CGameObject> pDoorL = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &doorDesc);
    auto pDoorL = GAMEINSTANCE->Get_Object_Prototype(CPartDoor::ObjID)->Clone(&doorDesc);
    //pDoorL->Get_Transform()->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //pDoorL->Get_Transform()->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
    m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pDoorL));
    
    doorDesc.fSpeedPerSec = { 10.f };
    doorDesc.strModelTag = TEXT("Prototype_Component_Model_QACenterDoorR");
    doorDesc.vPosition = { -0.f, 0.f, 0.f, 1.f };
    //doorDesc.bOpen = pDesc->bOpen;
    doorDesc.eDir = CPartDoor::DOOR_DIR::RIGHT;
    doorDesc.fMoveDist = 8.f;
    XMStoreFloat4x4(&doorDesc.worldMatrix, XMMatrixTranslation(-0.3f, 0.f, 0.f));
    //shared_ptr<CGameObject> pDoorR = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &doorDesc);
    auto pDoorR = GAMEINSTANCE->Get_Object_Prototype(CPartDoor::ObjID)->Clone(&doorDesc);
    //pDoorR->Get_Transform()->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //pDoorR->Get_Transform()->Set_Position(XMLoadFloat4(&doorDesc.vPosition));
    //pDoorR->Get_Transform()->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
    m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pDoorR));

    //CDoorTrigger::DOORTRIGGER_DESC triggerDesc{};
    //triggerDesc.pDoorObject = shared_from_this();
    //shared_ptr<CGameObject> pTrigger = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CDoorTrigger::ObjID, &triggerDesc);
    //m_pDoorTrigger = static_pointer_cast<CDoorTrigger>(pTrigger);

    //shared_ptr<CGameObject> pDoorFrame = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CProdDeptDoorFrame::ObjID);
    //m_vecParts.

    return S_OK;
}

HRESULT CQACenterDoor::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_QACenterDoorFrame"), TEXT("Model_Com")));

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vExtents = { 5.f, 3.f, 3.f };
    aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };
    
    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));
    m_pColliderCom->Set_Owner(shared_from_this());

    m_pTextureCom[OPEN] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DoorSign_Open"), TEXT("Open_Texture")));
    m_pTextureCom[CLOSE] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DoorSign_Close"), TEXT("Close_Texture")));

    if (!m_pShaderCom || !m_pModelCom || !m_pColliderCom || !m_pTextureCom[OPEN] || !m_pTextureCom[CLOSE])
        assert(false);

    return S_OK;
}

HRESULT CQACenterDoor::Bind_ShaderResources()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

shared_ptr<CQACenterDoor> CQACenterDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CQACenterDoor
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CQACenterDoor(pDevice, pContext)
        {}
    };

    shared_ptr<CQACenterDoor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CQACenterDoor"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CQACenterDoor::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CQACenterDoor
    {
        MakeSharedEnabler(const CQACenterDoor& rhs) : CQACenterDoor(rhs)
        {}
    };

    shared_ptr<CQACenterDoor> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CQACenterDoor"));
        assert(false);
    }

    return pInstance;
}

void CQACenterDoor::Free()
{
}
