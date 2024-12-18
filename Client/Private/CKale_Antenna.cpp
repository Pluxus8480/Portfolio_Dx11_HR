#include "CKale_Antenna.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"
#include "Math_Manager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "iostream"
#include "Cine_Manager.h"


wstring CKale_Antenna::ObjID = TEXT("CKale_Antenna");

CKale_Antenna::CKale_Antenna(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CJeongGimmicBase(pDevice, pContext)
{
}

CKale_Antenna::CKale_Antenna(const CKale_Antenna& rhs)
    : CJeongGimmicBase(rhs)
{
}

CKale_Antenna::~CKale_Antenna()
{
    Free();
}

HRESULT CKale_Antenna::Initialize_Prototype()
{
    _float4x4 fBlenderMat = {};
    XMStoreFloat4x4(&fBlenderMat, XMMatrixScaling(0.004f, 0.004f, 0.004f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_KaleAntenna"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/Kale_Antenna.fbx"),
            MODEL_TYPE::ANIM, fBlenderMat));
    
    return S_OK;
}

HRESULT CKale_Antenna::Initialize(void* pArg)
{
    Initialize_Transform();
    Ready_Component();
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
    m_pModelCom->Set_AnimIndex(0, true);
    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

    return S_OK;
}

void CKale_Antenna::PriorityTick(_float fTimeDelta)
{
}

void CKale_Antenna::Tick(_float fTimeDelta)
{  
    _float4x4 matOut = Identity;
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 3.f, matOut);
}

void CKale_Antenna::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    if (!CCine_Manager::Get_Instance()->IsInGame())
        return;

    if (pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fModelRange + 100))
    {
        pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
    }
}


void CKale_Antenna::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
     m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Platform_Land", m_pTransformCom->Get_Position());
	 EFFECTMANAGER->Call_Union_Effect("Platform_Cylinder_Effect", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
}

HRESULT CKale_Antenna::Render()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        _bool bNoEmit = false;
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            assert(false);

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
        {
            bNoEmit = true;
        }
        if (bNoEmit)
            m_pShaderCom->Begin(m_iShaderPassIndex);
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CKale_Antenna::Change_Phase(_uint iPhase)
{
    m_iCurPhase = iPhase;

    switch ((KALE_PHASE)m_iCurPhase)
    {
    case KALE_PHASE::Antenna_Phase1:
        m_pModelCom->Set_AnimIndex(0, false);
        break;
    case KALE_PHASE::Antenna_Phase2:
        m_pModelCom->Set_AnimIndex(1, false);
        break;
    case KALE_PHASE::Antenna_Phase3:
        m_pModelCom->Set_AnimIndex(2, false);
        break;
    default:
        break;
    }
}

void CKale_Antenna::Ready_Component()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_KaleAntenna"), TEXT("Model_Com")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysxCol_Com")));
    assert(m_pPhysXCollider);
    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
}

shared_ptr<CKale_Antenna> CKale_Antenna::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CKale_Antenna
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKale_Antenna(pDevice, pContext)
        {}
    };

    shared_ptr<CKale_Antenna> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKale_Antenna"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CKale_Antenna::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CKale_Antenna
    {
        MakeSharedEnabler(const CKale_Antenna& rhs) : CKale_Antenna(rhs)
        {}
    };

    shared_ptr<CKale_Antenna> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CKale_Antenna::Free()
{
}
