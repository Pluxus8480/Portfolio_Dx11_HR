#include "ComboSwitchButton.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "PhysxCollider.h"
#include "Mesh.h"

#include "Beat_Manager.h"

wstring CComboSwitchButton::ObjID = TEXT("CComboSwitchButton");
CComboSwitchButton::CComboSwitchButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{pDevice, pContext}
{
}

CComboSwitchButton::CComboSwitchButton(const CComboSwitchButton& rhs)
    : CPartObject{rhs}
{
}

CComboSwitchButton::~CComboSwitchButton()
{
    Free();
}

HRESULT CComboSwitchButton::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ComboSwitch_On"), TEXT("../Bin/Resources/Models/Map/Gimmick/ComboSwitch_On.FBX"), MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_BeatHit_Sphere"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_DMG_Sphere_BeatHit_01_0%d.png"), 8)
    );

    return S_OK;
}

HRESULT CComboSwitchButton::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

    return S_OK;
}

void CComboSwitchButton::PriorityTick(_float fTimeDelta)
{
}

void CComboSwitchButton::Tick(_float fTimeDelta)
{
    if (!m_bOn)
        return;

    _uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    if (iCurBeat != m_iPrevBeat)
    {
        m_iPrevBeat = iCurBeat;
        ++m_iCurFrame;
        m_iCurFrame %= 8;
    }

    SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CComboSwitchButton::LateTick(_float fTimeDelta)
{
    if (!m_bOn)
        return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    //GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CComboSwitchButton::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        //    return E_FAIL;
        
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCurFrame)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CComboSwitchButton::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        //m_bOn = false;
    }
}

void CComboSwitchButton::TakeDamage(const DamageInfo& damageInfo)
{
    m_bOn = false;
}

HRESULT CComboSwitchButton::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ComboSwitch_On"), TEXT("Model_Com")));
    m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BeatHit_Sphere"), TEXT("Texture_Com")));

    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

    _float3 vWorldSize{};
    _matrix matWorld = m_pTransformCom->Get_WorldMatrix_XMMat();
    vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
    vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
    vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));

    _matrix scaleMatrix = XMMatrixScaling(vWorldSize.x, vWorldSize.y, vWorldSize.z);
    _float4x4 fScaleMat{};
    XMStoreFloat4x4(&fScaleMat, scaleMatrix);
    PxVec3 pscale = { vWorldSize.x, vWorldSize.y, vWorldSize.z };
    string strTag = string(CComboSwitchButton::ObjID.begin(), CComboSwitchButton::ObjID.end());
    auto pMesh = m_pModelCom->Get_Mesh(0);
    pMesh->Cooking_TriangleMesh(fScaleMat);
    auto pTriangleMesh = pMesh->Get_TriangleMesh();
    if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
        auto pRigid = m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, strTag.c_str());

    if (!m_pShaderCom || !m_pModelCom || !m_pTextureCom || !m_pPhysXCollider)
        assert(false);

    return S_OK;
}

HRESULT CComboSwitchButton::Bind_ShaderResources()
{
    _float4x4 ViewMat;
    auto pGameInstance = m_pGameInstance.lock();
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

shared_ptr<CComboSwitchButton> CComboSwitchButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CComboSwitchButton
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComboSwitchButton(pDevice, pContext)
        {}
    };

    shared_ptr<CComboSwitchButton> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CComboSwitchButton"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CComboSwitchButton::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CComboSwitchButton
    {
        MakeSharedEnabler(const CComboSwitchButton& rhs) : CComboSwitchButton(rhs)
        {}
    };

    shared_ptr<CComboSwitchButton> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CComboSwitchButton"));
        assert(false);
    }

    return pInstance;
}

void CComboSwitchButton::Free()
{
}
