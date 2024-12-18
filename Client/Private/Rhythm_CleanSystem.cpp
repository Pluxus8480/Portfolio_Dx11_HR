#include "Rhythm_CleanSystem.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "CombatManager.h"

wstring CRhythm_CleanSystem::ObjID = TEXT("CRhythm_CleanSystem");
CRhythm_CleanSystem::CRhythm_CleanSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CRhythm_CleanSystem::CRhythm_CleanSystem(const CRhythm_CleanSystem& rhs)
    : CGameObject{ rhs }
{
}

CRhythm_CleanSystem::~CRhythm_CleanSystem()
{
    Free();
}

HRESULT CRhythm_CleanSystem::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CleanSystem"), TEXT("../Bin/Resources/Models/Map/AnimObject/SK_bg0260.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CleanSystem_Col"), TEXT("../Bin/Resources/Models/Map/AnimObject/SK_bg0260_Col.fbx"),
            MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Green.png"))
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Red"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Red.png"))
    );

    return S_OK;
}

HRESULT CRhythm_CleanSystem::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vecAnimSpeed.resize(m_pModelCom->Get_NumAnimation());

    return S_OK;
}

void CRhythm_CleanSystem::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

        for (_uint i = 0; i < m_vecAnimSpeed.size(); ++i)
        {
            _float fPlayTime = m_pModelCom->Get_Animation_PlayTime(i);

            m_vecAnimSpeed[i] = fPlayTime / fBeat;
        }

        m_bInit = true;
    }
}

void CRhythm_CleanSystem::Tick(_float fTimeDelta)
{
    

    _float4x4 fMat{};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_vecAnimSpeed[m_iAnimIndex], 0.1f, fMat);
}

void CRhythm_CleanSystem::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f)) return;

    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRhythm_CleanSystem::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        if (i == 3)
        {
            m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture");
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
        }
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CRhythm_CleanSystem::Edit_Attribute()
{
    _uint iNumAnim = m_pModelCom->Get_NumAnimation();
    if(ImGui::SliderInt("Anim Index", &m_iAnimIndex, 0, iNumAnim - 1))
        m_pModelCom->Set_AnimIndex(m_iAnimIndex, true);
}

void CRhythm_CleanSystem::Save_Data(ofstream& os)
{
    write_typed_data(os, m_iAnimIndex);
}

void CRhythm_CleanSystem::Load_Data(ifstream& is)
{
    read_typed_data(is, m_iAnimIndex);
}

HRESULT CRhythm_CleanSystem::Ready_Components()
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Model_CleanSystem"), TEXT("Com_Model")));
    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhysxCollider")));

    auto ColModel = GAMEINSTANCE->Get_Component_Prototype(iLevelIndex, TEXT("Prototype_Component_Model_CleanSystem_Col"));

    auto pMesh = static_pointer_cast<CModel>(ColModel)->Get_Mesh(0);
    _float3 vScale = m_pTransformCom->Get_Scale();
    _float4x4 fScaleMat = {};
    XMStoreFloat4x4(&fScaleMat, XMMatrixScaling(vScale.x * 0.004f, vScale.y * 0.004f, vScale.z * 0.004f));
    pMesh->Cooking_TriangleMesh(fScaleMat);
    auto pTriangleMesh = pMesh->Get_TriangleMesh();
    if (pTriangleMesh && vScale.x && vScale.y && vScale.z)
    {
        string tmp = string(CRhythm_CleanSystem::ObjID.begin(), CRhythm_CleanSystem::ObjID.end());
        m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
    }

    if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
        assert(false);

    m_pTextureCom[0] = static_pointer_cast<CTexture>(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("Com_Texture01")));
    m_pTextureCom[1] = static_pointer_cast<CTexture>(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Texture_EmiSquare_Red"), TEXT("Com_Texture02")));
    
    if (!m_pTextureCom[0] || !m_pTextureCom[2])
        assert(false);

    _uint iNumAnim = m_pModelCom->Get_NumAnimation();
    _uint iRandIndex = rand() % iNumAnim;
    m_pModelCom->Set_AnimIndex(iRandIndex, true);

    SetUp_PhysXCollider();
    
    return S_OK;
}

HRESULT CRhythm_CleanSystem::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

void CRhythm_CleanSystem::SetUp_PhysXCollider()
{
    const _float4x4* pBoneMat = m_pModelCom->Get_BoneMatrixPtr("origin");

    _float4x4 fMat{};
    m_pModelCom->Play_Animation(0.016f, 0.016f, 0.1f, fMat);

    _matrix WorldMat = XMLoadFloat4x4(pBoneMat) * m_pTransformCom->Get_WorldMatrix_XMMat();
    WorldMat.r[0] = XMVector3Normalize(WorldMat.r[0]);
    WorldMat.r[1] = XMVector3Normalize(WorldMat.r[1]);
    WorldMat.r[2] = XMVector3Normalize(WorldMat.r[2]);
    _float4x4 mat;
    XMStoreFloat4x4(&mat, WorldMat);
    PxMat44 pxWorld;
    pxWorld.column0.x = mat.m[0][0];
    pxWorld.column0.y = mat.m[0][1];
    pxWorld.column0.z = mat.m[0][2];
    pxWorld.column0.w = mat.m[0][3];
    pxWorld.column1.x = mat.m[1][0];
    pxWorld.column1.y = mat.m[1][1];
    pxWorld.column1.z = mat.m[1][2];
    pxWorld.column1.w = mat.m[1][3];
    pxWorld.column2.x = mat.m[2][0];
    pxWorld.column2.y = mat.m[2][1];
    pxWorld.column2.z = mat.m[2][2];
    pxWorld.column2.w = mat.m[2][3];
    pxWorld.column3.x = mat.m[3][0];
    pxWorld.column3.y = mat.m[3][1];
    pxWorld.column3.z = mat.m[3][2];
    pxWorld.column3.w = mat.m[3][3];

    PxTransform transform{ pxWorld };
    m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
}

shared_ptr<CRhythm_CleanSystem> CRhythm_CleanSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRhythm_CleanSystem
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRhythm_CleanSystem(pDevice, pContext)
        {}
    };

    shared_ptr<CRhythm_CleanSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRhythm_CleanSystem"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CRhythm_CleanSystem::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRhythm_CleanSystem
    {
        MakeSharedEnabler(const CRhythm_CleanSystem& rhs) : CRhythm_CleanSystem(rhs)
        {}
    };

    shared_ptr<CRhythm_CleanSystem> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRhythm_CleanSystem"));
        assert(false);
    }

    return pInstance;
}

void CRhythm_CleanSystem::Free()
{
}
