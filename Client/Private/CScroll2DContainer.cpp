#include "CScroll2DContainer.h"
#include "iostream"


wstring CScroll2DContainer::ObjID = TEXT("CScroll2DContainer");

CScroll2DContainer::CScroll2DContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CScroll2DContainer::CScroll2DContainer(const CScroll2DContainer& rhs)
	:CJeongGimmicBase(rhs)
{
}

CScroll2DContainer::~CScroll2DContainer()
{
}

HRESULT CScroll2DContainer::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_CScroll2DContainer",
            L"../Bin/Resources/Models/Map/Gimmick/CScroll2DContainer.fbx",
            MODEL_TYPE::ANIM, fMat)
    );

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Scroll2DContainer"), TEXT("../Bin/Resources/Sounds/MapObject/Scroll2DContainer.ogg")));
    
	return S_OK;
}

HRESULT CScroll2DContainer::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    _float3 Pos;
    XMStoreFloat3(&Pos, m_pTransformCom->Get_Position());
    InitializeBasicComponent(L"Prototype_Component_Model_CScroll2DContainer",
        L"Prototype_Component_Shader_VtxAnimMesh");

    _float3 m_vModelSize = { 4.f, 2.4f, 2.f };
    PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
    PxBoxGeometry Box{ BoxDimension };
    auto pRigidBody = m_pPhysXCollider->createStaticShapeActor(Box, "ASDF");
    assert(pRigidBody);
    m_BonePtr = m_pModelCom->Get_BoneMatrixPtr("container");
   
    m_pModelCom->Set_AnimIndex(0, false);
    _float4x4 Dummy;
    m_pModelCom->Play_Animation(0.001f, 0.001f, 0.1f, Dummy);

	return S_OK;
}

void CScroll2DContainer::PriorityTick(_float fTimeDelta)
{
}

void CScroll2DContainer::Tick(_float fTimeDelta)
{
    Follow_BonePhysX(*m_BonePtr);
}

void CScroll2DContainer::LateTick(_float fTimeDelta)
{
    _float4x4 Dummy;
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, Dummy);

    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 150))
        return;
    pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
}

HRESULT CScroll2DContainer::Render()
{
    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(Bind_ShaderResources()))
        assert(false);

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

void CScroll2DContainer::Set_Anim(_uint iAnimIndex)
{
    switch (iAnimIndex)
    {
    case 0:
        m_pModelCom->Set_AnimIndex(iAnimIndex, false);
        break;
    case 1:
        m_pModelCom->Set_AnimIndex(iAnimIndex, false);
        break;
    case 2:
        m_pModelCom->Set_AnimIndex(iAnimIndex, true);
        break;
    default:
        break;
    }
}

void CScroll2DContainer::Execute(_bool bExecute)
{
    m_bExecuted = bExecute;
    m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Scroll2DContainer");
    m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, L"Scroll2DContainer", 4);
    Set_Anim(1);
}


shared_ptr<CScroll2DContainer> CScroll2DContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CScroll2DContainer
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CScroll2DContainer(pDevice, pContext)
        {}
    };

    shared_ptr<CScroll2DContainer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CScroll2DContainer::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CScroll2DContainer
    {
        MakeSharedEnabler(const CScroll2DContainer& rhs) : CScroll2DContainer(rhs)
        {}
    };

    shared_ptr<CScroll2DContainer> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CScroll2DContainer::Free()
{

}
