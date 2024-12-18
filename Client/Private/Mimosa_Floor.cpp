#include "Mimosa_Floor.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Beat_Manager.h"
#include "MapManager.h"
#include "Effect_Manager.h"
#include "Texture.h"

wstring CMimosa_Floor::ObjID = TEXT("CMimosa_Floor");
CMimosa_Floor::CMimosa_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMimosa_Floor::CMimosa_Floor(const CMimosa_Floor& rhs)
    : CGameObject{ rhs }
{
}

CMimosa_Floor::~CMimosa_Floor()
{
    Free();
}

HRESULT CMimosa_Floor::Initialize_Prototype()
{
    //Mimosa_Stage
    _float4x4 FMat = {};
    XMStoreFloat4x4(&FMat, XMMatrixScaling(0.0001f, 0.0001f, 0.0001f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_Stage"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/Mimosa_Stage.fbx"),
            MODEL_TYPE::NONANIM, FMat)
    );

    return S_OK;
}

HRESULT CMimosa_Floor::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Stage"), TEXT("Model_Com")));
    m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sky_Cube_Ref"), TEXT("Com_Texture")));

    
    return S_OK;
}

void CMimosa_Floor::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_Floor::Tick(_float fTimeDelta)
{

}

void CMimosa_Floor::LateTick(_float fTimeDelta)
{
    m_bRefl = false;
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND, shared_from_this());
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
}

HRESULT CMimosa_Floor::Render()
{
    auto pGI = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        assert(false);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        assert(false);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        assert(false);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        assert(false);
    if (!m_bRefl)
    {
        _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                return E_FAIL;
            _float4 vColor = { 0.f,0.8f,1.0f,0.6f };
            m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
            m_pShaderCom->Begin((_uint)MESH_PASS::DCBlend);
            m_pModelCom->Render((_uint)i);
        }
        m_bRefl = true;
    }
    else
    {
        _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
        //pGI->Bind_RT_SRV(L"RT_Albedo", m_pShaderCom, "g_Texture");
        //pGI->Bind_RT_SRV(L"RT_Position", m_pShaderCom, "g_ATOS_Texture");
        //pGI->Bind_RT_SRV(L"RT_Normal", m_pShaderCom, "g_Normal_Texture");
        _float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
        
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_CubeMap")))
        {
            assert(false);
        }
        
        m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
        m_pShaderCom->Bind_Float("g_OutlineWidth", 2.f);
        m_pShaderCom->Bind_Vector4("g_vCampos", &pGI->Get_CamLook_Float4());
        //m_pShaderCom->Bind_Matrix("g_ViewInv", &m_pGameInstance.lock()->Get_Transform_Float4x4(CPipeLine::PIPELINE_STATE::TS_VIEW_INV));
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            m_pShaderCom->Begin((_uint)MESH_PASS::ReflectionCube);
            m_pModelCom->Render((_uint)i);
        }
    }

    return S_OK;
}

void CMimosa_Floor::Change_Phase(_uint iPhase)
{

}

shared_ptr<CMimosa_Floor> CMimosa_Floor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMimosa_Floor
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_Floor(pDevice, pContext)
        {}
    };

    shared_ptr<CMimosa_Floor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMimosa_Floor"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMimosa_Floor::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMimosa_Floor
    {
        MakeSharedEnabler(const CMimosa_Floor& rhs) : CMimosa_Floor(rhs)
        {}
    };

    shared_ptr<CMimosa_Floor> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMimosa_Floor"));
        assert(false);
    }

    return pInstance;
}

void CMimosa_Floor::Free()
{
}
