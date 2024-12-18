#include "CraneMagnet.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"

#include "MapManager.h"
#include "GameManager.h"
#include "Beat_Manager.h"

#include "Chai.h"
#include "PlayerManager.h"
#include "Player_Camera.h"
#include "Beat_Manager.h"
#include "CharacterController.h"
#include "CUi_MapInterAction.h"


wstring CCraneMagnet::ObjID = TEXT("CCraneMagnet");
CCraneMagnet::CCraneMagnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMagnet{pDevice, pContext}
{
}

CCraneMagnet::CCraneMagnet(const CCraneMagnet& rhs)
    : CMagnet{rhs}
{
}

CCraneMagnet::~CCraneMagnet()
{
    Free();
}

void CCraneMagnet::Execute(_bool bExecute)
{
    if (m_bExecuted == bExecute) return;

    m_bExecuted = bExecute;
}

void CCraneMagnet::Start_CraneEvent()
{
    //m_iAnimIndex = 10;
    //m_pModelCom->Set_AnimIndex(10, false);
}

HRESULT CCraneMagnet::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ZipLineMagnet"), TEXT("../Bin/Resources/Models/Map/Gimmick/ZipLineMagnet.fbx"), MODEL_TYPE::ANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Green.png"))
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_WarningLight_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiWarninglight_E_Green.png"))
    );
 
    return S_OK;
}

HRESULT CCraneMagnet::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iAnimIndex = 13; // Idle
    m_pModelCom->Set_AnimIndex(m_iAnimIndex, true);

    _float4x4 Mat{};
    m_pModelCom->Play_Animation(0.016f, 0.016f, 0.1f, Mat);

    CMapManager::Get_Instance()->Assign_Magnet(static_pointer_cast<CMagnet>(shared_from_this()));

    Execute();
    Initialize_Ui();

    return S_OK;
}

void CCraneMagnet::PriorityTick(_float fTimeDelta)
{
}

void CCraneMagnet::Tick(_float fTimeDelta)
{
    //if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
    //    Execute();

    _float4x4 Mat{};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, Mat);


    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());
    if (pPlayer && !m_bSet_Wire_list)
    {
        m_bSet_Wire_list = true;
        
        pPlayer->Get_Player_Camera()->Set_Wire_Target(static_pointer_cast<CMagnet>(shared_from_this()));
    }

    if (m_iAnimIndex == 10 && m_pModelCom->Get_Finished())
    {
        m_iAnimIndex = 11;
        m_pModelCom->Set_AnimIndex(m_iAnimIndex, false);
    }
    else if (m_iAnimIndex == 11 && m_pModelCom->Get_Finished())
    {
        m_iAnimIndex = 13;
        m_pModelCom->Set_AnimIndex(m_iAnimIndex, false);
    }
}

void CCraneMagnet::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f))
        return;

    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CCraneMagnet::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        _bool bEmissive = { false };
        if (i == 1)
        {
            bEmissive = true;
            if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
                return E_FAIL;
        }
        else if (i == 2)
        {
            bEmissive = true;
            if (FAILED(m_pTextureCom[1]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
                return E_FAIL;
        }
        else
        {
            if (SUCCEEDED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
                bEmissive = true;
        }

        if(bEmissive)
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CCraneMagnet::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
}

HRESULT CCraneMagnet::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ZipLineMagnet"), TEXT("Model_Com")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    m_pTextureCom[0] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WarningLight_Green"), TEXT("Texture0_Com")));
    m_pTextureCom[1] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("Texture1_Com")));
    
    for (size_t i = 0; i < 2; i++)
        assert(m_pTextureCom[i]);

    return S_OK;
}

HRESULT CCraneMagnet::Bind_ShaderResources()
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

shared_ptr<CCraneMagnet> CCraneMagnet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CCraneMagnet
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCraneMagnet(pDevice, pContext)
        {}
    };

    shared_ptr<CCraneMagnet> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCraneMagnet"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CCraneMagnet::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CCraneMagnet
    {
        MakeSharedEnabler(const CCraneMagnet& rhs) : CCraneMagnet(rhs)
        {}
    };

    shared_ptr<CCraneMagnet> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCraneMagnet"));
        assert(false);
    }

    return pInstance;
}

void CCraneMagnet::Free()
{
}
