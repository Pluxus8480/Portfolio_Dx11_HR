#include "Free_SM_Object.h"
#include "GameInstance.h"

#include "Model.h"
#include "Shader.h"


wstring CFree_SM_Object::ObjID = TEXT("CFree_SM_Object");

CFree_SM_Object::CFree_SM_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CFree_SM_Object::CFree_SM_Object(const CFree_SM_Object& rhs)
    : CGameObject(rhs)
{
}

CFree_SM_Object::~CFree_SM_Object()
{
    Free();
}

HRESULT CFree_SM_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFree_SM_Object::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC TransformDesc{};
    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    _matrix LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
    //_matrix LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationRollPitchYaw(0.f, 0.f, XM_PI); //Ä³ÀÏ ÆÈ
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);

    auto pDesc = static_cast<DESC*>(pArg);
    string strPath = pDesc->strModelPath;
    m_strModelTag = wstring(strPath.begin(), strPath.end());
    if (nullptr == GAMEINSTANCE->Get_Component_Prototype(m_iLevelIndex, m_strModelTag))
    {
        GAMEINSTANCE->Add_Prototype(
            m_iLevelIndex, m_strModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, strPath.c_str(), LocalTransformMatrix)
        );
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimIndex("wp0000_atk-guitar_241", false);
    //m_pModelCom->Set_AnimIndex("em7520_Farm-chance-cp_022", true);

    return S_OK;
}

void CFree_SM_Object::PriorityTick(_float fTimeDelta)
{
}

void CFree_SM_Object::Tick(_float fTimeDelta)
{
    _float4x4 mMat = {};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, mMat);
}

void CFree_SM_Object::LateTick(_float fTimeDelta)
{
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CFree_SM_Object::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    _float4x4 matWorld = m_pTransformCom->Get_WorldMatrix();
    if (m_pBoneMatrix != nullptr)
    {
        _matrix	BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);
        for (size_t i = 0; i < 3; i++)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

        XMStoreFloat4x4(&matWorld, m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CFree_SM_Object::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, m_strModelTag, TEXT("Com_Model")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

void CFree_SM_Object::AttachBone(const _float4x4* pBoneMatrix)
{
    m_pBoneMatrix = pBoneMatrix;
}

shared_ptr<CFree_SM_Object> CFree_SM_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CFree_SM_Object
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFree_SM_Object(pDevice, pContext)
        {}
    };
    shared_ptr<CFree_SM_Object> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFree_SM_Object"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CFree_SM_Object::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CFree_SM_Object
    {
        MakeSharedEnabler(const CFree_SM_Object& rhs) : CFree_SM_Object(rhs)
        {}
    };
    shared_ptr<CFree_SM_Object> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFree_SM_Object"));
        assert(false);
    }

    return pInstance;
}

void CFree_SM_Object::Free()
{
}
