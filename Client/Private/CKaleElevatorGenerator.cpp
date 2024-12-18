#include "CKaleElevatorGenerator.h"


wstring CKaleElevatorGenerator::ObjID = TEXT("CKaleElevatorGenerator");

CKaleElevatorGenerator::CKaleElevatorGenerator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CKaleElevatorGenerator::CKaleElevatorGenerator(const CKaleElevatorGenerator& rhs)
	:CJeongGimmicBase(rhs)
{
}

CKaleElevatorGenerator::~CKaleElevatorGenerator()
{
}

HRESULT CKaleElevatorGenerator::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_KaleElevatorGenerator",
            L"../Bin/Resources/Models/Map/Gimmick/KaleElevatorGenerator.fbx",
            MODEL_TYPE::ANIM, fMat)
    );
	return S_OK;
}

HRESULT CKaleElevatorGenerator::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_KaleElevatorGenerator",
        L"Prototype_Component_Shader_VtxAnimMesh");
    Initialize_CookPhysX(L"Prototype_Component_Model_KaleElevatorGenerator");
    
    m_pModelCom->Set_AnimIndex(0, false);
    _float4x4 Dummy;
    m_pModelCom->Play_Animation(0.001f, 0.001f, 0.1f, Dummy);

    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

	return S_OK;
}

void CKaleElevatorGenerator::PriorityTick(_float fTimeDelta)
{
}

void CKaleElevatorGenerator::Tick(_float fTimeDelta)
{
    if (m_pGameInstance.lock()->Get_KeyDown(DIK_F7))
    {
        m_bTest = true;
    }
    if (m_bTest)
    {
        _float4x4 Dummy;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, Dummy);
    }
}

void CKaleElevatorGenerator::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fModelRange))
        return;

    pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
}

HRESULT CKaleElevatorGenerator::Render()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);
        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

shared_ptr<CKaleElevatorGenerator> CKaleElevatorGenerator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CKaleElevatorGenerator
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKaleElevatorGenerator(pDevice, pContext)
        {}
    };

    shared_ptr<CKaleElevatorGenerator> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CKaleElevatorGenerator::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CKaleElevatorGenerator
    {
        MakeSharedEnabler(const CKaleElevatorGenerator& rhs) : CKaleElevatorGenerator(rhs)
        {}
    };

    shared_ptr<CKaleElevatorGenerator> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CKaleElevatorGenerator::Free()
{

}
