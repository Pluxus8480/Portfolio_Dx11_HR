#include "CContainerBreakPannel.h"
#include "Cine_Manager.h"


wstring CContainerBreakPannel::ObjID = TEXT("CContainerBreakPannel");

CContainerBreakPannel::CContainerBreakPannel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CContainerBreakPannel::CContainerBreakPannel(const CContainerBreakPannel& rhs)
	:CJeongGimmicBase(rhs)
{
}

CContainerBreakPannel::~CContainerBreakPannel()
{
}

HRESULT CContainerBreakPannel::Initialize_Prototype()
{
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_ContainerBreakPannelBefore",
            L"../Bin/Resources/Models/Map/Gimmick/CContainerBreakPannelBefore.fbx",
            MODEL_TYPE::NONANIM, Identity)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_ContainerBreakPannelAfter",
            L"../Bin/Resources/Models/Map/Gimmick/CContainerBreakPannelAfter.fbx",
            MODEL_TYPE::NONANIM, Identity)
    );

	return S_OK;
}

HRESULT CContainerBreakPannel::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_ContainerBreakPannelBefore",
        L"Prototype_Component_Shader_VtxMesh");
 
    m_pAfterModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, 
        L"Prototype_Component_Model_ContainerBreakPannelAfter", L"AfterModel_Com"));

    CCine_Manager::Get_Instance()->AddLastCallback
    ("EV1130", [&]() { m_bChange = true; });

	return S_OK;
}

void CContainerBreakPannel::PriorityTick(_float fTimeDelta)
{
}

void CContainerBreakPannel::Tick(_float fTimeDelta)
{

}

void CContainerBreakPannel::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 100))
        return;

    pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
}

HRESULT CContainerBreakPannel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

    if (!m_bChange)
    {
        _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                assert(false);

            if (i == 2)
            {
                _float4 vColor = { 0.149f, 0.533f, 0.725f, 1.f };
                m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
                m_pShaderCom->Begin((_uint)MESH_PASS::DiffuseColor);
            }
            else
                m_pShaderCom->Begin(m_iShaderPassIndex);
            m_pModelCom->Render((_uint)i);
        }
    }
    else
    {
        _uint	iNumMeshes = m_pAfterModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pAfterModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                assert(false);

            m_pShaderCom->Begin(m_iShaderPassIndex);
            m_pAfterModelCom->Render((_uint)i);
        }
    }

    return S_OK;
}

shared_ptr<CContainerBreakPannel> CContainerBreakPannel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CContainerBreakPannel
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CContainerBreakPannel(pDevice, pContext)
        {}
    };

    shared_ptr<CContainerBreakPannel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CContainerBreakPannel::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CContainerBreakPannel
    {
        MakeSharedEnabler(const CContainerBreakPannel& rhs) : CContainerBreakPannel(rhs)
        {}
    };

    shared_ptr<CContainerBreakPannel> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CContainerBreakPannel::Free()
{

}


