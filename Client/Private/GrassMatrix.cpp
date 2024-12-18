#include "GrassMatrix.h"
#include "GameInstance.h"
#include "Shader.h"

wstring CGrassMatrix::ObjID = TEXT("CGrassMatrix");
CGrassMatrix::CGrassMatrix(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{pDevice, pContext}
{
}

CGrassMatrix::CGrassMatrix(const CGrassMatrix& rhs)
    : CPartObject{rhs}
{
}

CGrassMatrix::~CGrassMatrix()
{
    Free();
}

HRESULT CGrassMatrix::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGrassMatrix::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    GRASS_DESC* pDesc = static_cast<GRASS_DESC*>(pArg);

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
    m_iModelIndex = pDesc->iModelIndex;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGrassMatrix::PriorityTick(_float fTimeDelta)
{
}

void CGrassMatrix::Tick(_float fTimeDelta)
{
}

void CGrassMatrix::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    m_bCulled = !pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f);
    if (m_bCulled) return;

    //m_BillBoardMat = GAMEINSTANCE->Matrix_BillBoard(m_pTransformCom->Get_WorldMatrix());
    //pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

    m_pTransformCom->LookAt_Horizontal(pGI->Get_CamPosition_Vector());
}

HRESULT CGrassMatrix::Render()
{
    
    return S_OK;
}

HRESULT CGrassMatrix::Bind_Matrix()
{
    if (m_bCulled) return S_OK;

    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_BillBoardMat)))
    //    return E_FAIL;
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;

    return S_OK;
}

HRESULT CGrassMatrix::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    assert(m_pShaderCom);

    return S_OK;
}

HRESULT CGrassMatrix::Bind_ShaderResources()
{
    auto pGI = m_pGameInstance.lock();

    //_float4x4 ViewMat;
    //XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    //_float4x4 ProjMat;
    //XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    	return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    //	return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
    //    return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
    //    return E_FAIL;

    return S_OK;
}

shared_ptr<CGrassMatrix> CGrassMatrix::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CGrassMatrix
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGrassMatrix(pDevice, pContext)
        {}
    };


    shared_ptr<CGrassMatrix> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGrassMatrix"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CGrassMatrix::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CGrassMatrix
    {
        MakeSharedEnabler(const CGrassMatrix& rhs) : CGrassMatrix(rhs)
        {}
    };

    shared_ptr<CGrassMatrix> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGrassMatrix"));
        assert(false);
    }

    return pInstance;
}

void CGrassMatrix::Free()
{
}
