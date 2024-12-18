#include "KrscRoomWall.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#include "MapManager.h"

wstring CKrscRoomWall::ObjID = TEXT("CKrscRoomWall");
CKrscRoomWall::CKrscRoomWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CKrscRoomWall::CKrscRoomWall(const CKrscRoomWall& rhs)
    : CGameObject{rhs}
{
}

CKrscRoomWall::~CKrscRoomWall()
{
    Free();
}

HRESULT CKrscRoomWall::Initialize_Prototype()
{
    _float4x4 FMat = {};
    XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_KrscRoomWall"),
            TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_KrscRoomWallA_TS_01.FBX"),
            MODEL_TYPE::NONANIM, FMat)
    );
    
    return S_OK;
}

HRESULT CKrscRoomWall::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    Ready_Components();

    return S_OK;
}

void CKrscRoomWall::PriorityTick(_float fTimeDelta)
{
}

void CKrscRoomWall::Tick(_float fTimeDelta)
{
}

void CKrscRoomWall::LateTick(_float fTimeDelta)
{
    auto pGameInstance = m_pGameInstance.lock();

    pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CKrscRoomWall::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        _bool bEmissive = { false };

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", (_uint)i, aiTextureType_EMISSIVE)))
            bEmissive = false;
        else
            bEmissive = true;

        if(bEmissive)
            m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
        else
            m_pShaderCom->Begin((_uint)MESH_PASS::Default);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CKrscRoomWall::Ready_Components()
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();
    if (iLevelIndex == LEVEL_MAPTOOL)
        iLevelIndex = LEVEL_STATIC;
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Model_KrscRoomWall"), TEXT("Com_Model")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
}

HRESULT CKrscRoomWall::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CKrscRoomWall> CKrscRoomWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CKrscRoomWall
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKrscRoomWall(pDevice, pContext)
        {}
    };

    shared_ptr<CKrscRoomWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKrscRoomWall"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CKrscRoomWall::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CKrscRoomWall
    {
        MakeSharedEnabler(const CKrscRoomWall& rhs) : CKrscRoomWall(rhs)
        {}
    };

    shared_ptr<CKrscRoomWall> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CKrscRoomWall"));
        assert(false);
    }

    return pInstance;
}

void CKrscRoomWall::Free()
{
}
