#include "Rekka_Long_Pipe.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"


wstring CRekka_Long_Pipe::ObjID = TEXT("Rekka_Long_Pipe");

CRekka_Long_Pipe::CRekka_Long_Pipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CRekka_Long_Pipe::CRekka_Long_Pipe(const CRekka_Long_Pipe& rhs)
    : CPartObject(rhs)
{
}

CRekka_Long_Pipe::~CRekka_Long_Pipe()
{
    Free();
}

HRESULT CRekka_Long_Pipe::Initialize_Prototype()
{
    _matrix	LocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Rekka_Long_Pipe"), TEXT("../Bin/Resources/Models/Monsters/Rekka/Long_Pipe.fbx"), MODEL_TYPE::NONANIM, preMatrix)
    );

    return S_OK;
}

HRESULT CRekka_Long_Pipe::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    m_pBoneMatrix = pDesc->pBoneMatrix;
    m_iLevelIndex = pDesc->iLevelIndex;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CRekka_Long_Pipe::PriorityTick(_float fTimeDelta)
{
}

void CRekka_Long_Pipe::Tick(_float fTimeDelta)
{
    if (m_isAttached)
    {
        _matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);
        for (size_t i = 0; i < 3; i++)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
        SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);
    }
    else
    {
        _matrix matRotation = XMMatrixRotationZ(XM_PI * 0.5f);
        _matrix matTransform = matRotation * XMMatrixTranslationFromVector(XMLoadFloat4(&m_vDefaultPos));
        XMStoreFloat4x4(&m_WorldMatrix, matTransform);
    }
}

void CRekka_Long_Pipe::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRekka_Long_Pipe::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;

        m_pShaderCom->Begin(0);

        if (FAILED(m_pModelCom->Bind_Buffers(i)))
            return E_FAIL;

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CRekka_Long_Pipe::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Rekka_Long_Pipe"), TEXT("Com_Model")));

    return S_OK;
}

void CRekka_Long_Pipe::AttachOn()
{
    m_isAttached = true;
}

void CRekka_Long_Pipe::AttachOff()
{
    m_isAttached = false;
    
}

shared_ptr<CRekka_Long_Pipe> CRekka_Long_Pipe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRekka_Long_Pipe
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Long_Pipe(pDevice, pContext) {}
    };

    shared_ptr<CRekka_Long_Pipe> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRekka_Long_Pipe"));
        assert(false);
    }
    return pInstance;
}

shared_ptr<CGameObject> CRekka_Long_Pipe::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRekka_Long_Pipe
    {
        MakeSharedEnabler(const CRekka_Long_Pipe& rhs) : CRekka_Long_Pipe(rhs) {}
    };

    shared_ptr<CRekka_Long_Pipe> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRekka_Long_Pipe"));
        assert(false);
    }
    return pInstance;
}

void CRekka_Long_Pipe::Free()
{
}
