#include "SwitchButton.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"

wstring CSwitchButton::ObjID = TEXT("CSwitchButton");

CSwitchButton::CSwitchButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{pDevice, pContext}
{
}

CSwitchButton::CSwitchButton(const CSwitchButton& rhs)
    : CPartObject{rhs}
{
}

CSwitchButton::~CSwitchButton()
{
    Free();
}

HRESULT CSwitchButton::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CommonSwitch_On"), TEXT("../Bin/Resources/Models/Map/Gimmick/CommonSwitch_ON.FBX"), MODEL_TYPE::NONANIM, Identity)
    );

    return S_OK;
}

HRESULT CSwitchButton::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CSwitchButton::PriorityTick(_float fTimeDelta)
{
}

void CSwitchButton::Tick(_float fTimeDelta)
{
    if (!m_bOn)
        return;

    //if (GAMEINSTANCE->Get_KeyDown(DIK_O))
    //    m_bOn = false;

    SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());

    //m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CSwitchButton::LateTick(_float fTimeDelta)
{
    if (!m_bOn)
        return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    //GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CSwitchButton::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        //	return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //	return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render((_uint)i);
    }

//#ifdef _DEBUG
//    if (m_pColliderCom)
//        m_pColliderCom->Render();
//#endif

    return S_OK;
}

//void CSwitchButton::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
//{
//    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
//    {
//        m_bOn = false;
//    }
//}
//
//void CSwitchButton::TakeDamage(const DamageInfo& damageInfo)
//{
//    m_bOn = false;
//}

HRESULT CSwitchButton::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CommonSwitch_On"), TEXT("Model_Com")));

    //CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
    //sphereDesc.fRadius = 0.5f;
    //sphereDesc.vCenter = { 0.f, 0.5f, 0.f };
    //
    //m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Com"), &sphereDesc));

    return S_OK;
}

HRESULT CSwitchButton::Bind_ShaderResources()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    //    return E_FAIL;
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

shared_ptr<CSwitchButton> CSwitchButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CSwitchButton
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSwitchButton(pDevice, pContext)
        {}
    };

    shared_ptr<CSwitchButton> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSwitchButton"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CSwitchButton::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CSwitchButton
    {
        MakeSharedEnabler(const CSwitchButton& rhs) : CSwitchButton(rhs)
        {}
    };

    shared_ptr<CSwitchButton> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSwitchButton"));
        assert(false);
    }

    return pInstance;
}

void CSwitchButton::Free()
{
}
