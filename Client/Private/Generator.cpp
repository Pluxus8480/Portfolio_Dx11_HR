#include "Generator.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "PhysxCollider.h"
#include "Texture.h"

#include "QACenterDoor.h"
#include "iostream"
#include "PlayerManager.h"
#include "CUi_GeneratorSystem.h"
#include "MapManager.h"
#include "CUi_MapInterAction.h"


wstring CGenerator::ObjID = TEXT("CGenerator");
CGenerator::CGenerator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CGenerator::CGenerator(const CGenerator& rhs)
    : CGimmickBase{rhs}
{
}

CGenerator::~CGenerator()
{
    Free();
}

HRESULT CGenerator::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Generator"), TEXT("../Bin/Resources/Models/Map/Gimmick/Generator.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Generator_EmiGreen"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_GeneratorSignA_EmiGreen.dds"))
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Generator_EmiRed"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_GeneratorSignA_EmiRed.dds"))
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Generator_EmiYellow"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_GeneratorSignA_EmiYellow.dds"))
    );

    return S_OK;
}

HRESULT CGenerator::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimIndex(3, true);

    if(CMapManager::Get_Instance()->Get_NextLevel() != LEVEL_MAPTOOL)
        Initialize_Sign();
    
    return S_OK;
}

void CGenerator::PriorityTick(_float fTimeDelta)
{
}

void CGenerator::Tick(_float fTimeDelta)
{
    Cal_PlayerDistance();
    Update_AnimState();

    if (m_pGenerator_System.expired() && m_bEvent_Start && m_iCurHit != m_iMaxHit)
    {
        m_iCurHit = 0;
        m_bEvent_Start = false;
        PLAYERMANAGER->Callback_Event("Generator_Finish");
    }
    else if(!m_pGenerator_System.expired() && m_bEvent_Start)
    {
        shared_ptr<CUi_GeneratorSystem> generatorSystem =  static_pointer_cast<CUi_GeneratorSystem>(m_pGenerator_System.lock());
        m_iBefore_Hit = m_iCurHit;
        m_iCurHit = generatorSystem->Get_Success_Count();

        if (m_iBefore_Hit < m_iCurHit)
        {
            m_eAnimState = HIT;
            m_pModelCom->Set_AnimIndex(2, false);
            
            m_iBefore_Hit = m_iCurHit;
        }
        if (generatorSystem->Get_Is_Fail())
        {
            m_iCurHit = 0;
            m_bEvent_Start = false;
            PLAYERMANAGER->Callback_Event("Generator_Finish");
        }
    }

	if (!m_bDone && m_bOn)
	{
		if (m_vecRelatives.size() > 0)
		{
			//m_vecRelatives.back()->Execute();
			static_pointer_cast<CQACenterDoor>(m_vecRelatives.back())->Set_Ready(true);
			CGimmickBase::Done();
		}
	}

    if (m_iCurHit == 0)
        m_iEmiIndex = RED;
    else if (m_iCurHit > 0 && m_iCurHit < m_iMaxHit)
        m_iEmiIndex = YELLOW;
    else
        m_iEmiIndex = GREEN;

    m_pSwitchColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pBodyColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

    _float4x4 fMat = {};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);
}

void CGenerator::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

    if (!m_bDone)
    {
        GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pSwitchColliderCom);
        GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pBodyColliderCom);
    }
}

HRESULT CGenerator::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        _bool bEmi = { false };
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
            bEmi = false;
        
        else
        {
            bEmi = true;
            if (i == 3)
            {
                m_pTextureCom[m_iEmiIndex]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture");
            }
        }
        
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        if (bEmi)
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
        else
            m_pShaderCom->Begin(0);

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pSwitchColliderCom)
        m_pSwitchColliderCom->Render();

    if (m_pBodyColliderCom)
        m_pBodyColliderCom->Render();
#endif

    return S_OK;
}

void CGenerator::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    {
        PLAYERMANAGER->Callback_Event("Generator_Start");
    }
}

void CGenerator::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    {
        if (GAMEINSTANCE->Get_KeyDown(DIK_E))
        {
            if (CUi_Mgr::Get_Instance()->Find_UI(CUi_GeneratorSystem::ObjID) == nullptr)
            {
                m_bEvent_Start = true;
                m_pGenerator_System = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_GeneratorSystem::ObjID, m_iLevelIndex, L"CUi", CUi_GeneratorSystem::ObjID);
                PLAYERMANAGER->Set_Generator_System(m_pGenerator_System);
                m_pSign->Set_Active(false);
            }
            PLAYERMANAGER->Callback_Event("Generator");
        }
    }
}

void CGenerator::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pOther->Get_ColliderTag() == TEXT("Player_Hit") && pCaller == m_pBodyColliderCom)
    {
        PLAYERMANAGER->Callback_Event("Generator_End");
    }
}

void CGenerator::TakeDamage(const DamageInfo& damageInfo)
{
    //if (m_iCurHit < m_iMaxHit)
    //{
    //    m_eAnimState = HIT;
    //    m_pModelCom->Set_AnimIndex(2, false);
    //    ++m_iCurHit;
    //}
}

void CGenerator::Edit_Attribute()
{
    _int iMaxHit = { m_iMaxHit };
    if (ImGui::InputInt("Hit Count", &iMaxHit))
        m_iMaxHit = iMaxHit;
}

void CGenerator::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CGenerator::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CGenerator::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);

    write_typed_data(os, m_iMaxHit);
}

void CGenerator::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);

    read_typed_data(is, m_iMaxHit);
}

HRESULT CGenerator::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Generator"), TEXT("Model_Com")));

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = { 0.5f, 2.f, 1.5f };
    obbDesc.vExtents = { 1.f, 1.f, 1.f };
    obbDesc.vRotation = { 0.f, 0.f, 0.f };
    m_pSwitchColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("SwitchCollider_Com"), &obbDesc));

    obbDesc.vExtents = { 3.f, 2.f, 2.f };
    obbDesc.vCenter = { -1.f, 2.f, 3.f };
    m_pBodyColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("BodyCollider_Com"), &obbDesc));

    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysXCollider_Com")));

    _float3 vModelCenter = { -1.f, 2.f, 0.f };
    _float3 vModelSize = { 3.f, 2.f, 2.f };

    _float3 vScale = m_pTransformCom->Get_Scale();
    vModelSize = { vModelSize.x * vScale.x, vModelSize.y * vScale.y , vModelSize.z * vScale.z };
    vModelCenter = { vModelCenter.x * vScale.x, vModelCenter.y * vScale.y , vModelCenter.z * vScale.z };
    
    PxVec3 BoxDimension{ vModelSize.x, vModelSize.y, vModelSize.z };
    PxBoxGeometry Box{ BoxDimension };
    string ColliderTag = m_strGimmickID;
    m_pPhysXCollider->createStaticShapeActor(Box, ColliderTag.c_str());

    _matrix WorldMat = m_pTransformCom->Get_WorldMatrix_XMMat();
    WorldMat = XMMatrixMultiply(XMMatrixTranslation(vModelCenter.x, vModelCenter.y, vModelCenter.z), WorldMat);
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

    if (!m_pShaderCom || !m_pModelCom || !m_pSwitchColliderCom || !m_pPhysXCollider)
        assert(false);

    PxTransform transform{ pxWorld };
    m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

    m_pTextureCom[RED] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Generator_EmiRed"),
        TEXT("RedTexture_Com")));
    m_pTextureCom[GREEN] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Generator_EmiGreen"),
        TEXT("GreenTexture_Com")));
    m_pTextureCom[YELLOW] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Generator_EmiYellow"),
        TEXT("YellowTexture_Com")));

    if (!m_pTextureCom[RED] || !m_pTextureCom[GREEN] || !m_pTextureCom[YELLOW])
        assert(false);

    return S_OK;
}

HRESULT CGenerator::Ready_Parts()
{
    return S_OK;
}

HRESULT CGenerator::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

void CGenerator::Update_AnimState()
{
    switch (m_eAnimState)
    {
    case Client::CGenerator::IDLE_OUT:
        break;

    case Client::CGenerator::IDLE_ON:
        break;

    case Client::CGenerator::HIT:
        if (m_pModelCom->Get_Finished())
        {
            if (m_iCurHit == m_iMaxHit)
            {
                m_bOn = true;
                PLAYERMANAGER->Callback_Event("Generator_Finish");
                m_eAnimState = IDLE_ON;
                m_pModelCom->Set_AnimIndex(4, true);
            }
            else
            {
                m_eAnimState = IDLE_OUT;
                m_pModelCom->Set_AnimIndex(3, true);
            }
        }
        break;
    }
}

void CGenerator::Cal_PlayerDistance()
{
    //_float vecLength = PLAYERMANAGER->Get_DistancePlayer(m_pTransformCom);
    //if (vecLength < m_fWorkDistance)
    //{
    //    if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
    //    {

    //    }
    //}
}

void CGenerator::Initialize_Sign()
{
    CUi_MapInterAction::Desc Desc;
    Desc.fAwareDistance = m_fWorkDistance * 2;
    Desc.fWorkDistance = m_fWorkDistance;
    Desc.OwnerTransform = m_pTransformCom;
    Desc.RelativeDistance = { 0, 100 };
    Desc.iWorkTextureIndex = CUi_MapInterAction::WorkTexture::E_Key;
    m_pSign = static_pointer_cast<CUi_MapInterAction>(CUi_Mgr::Get_Instance()->
        Add_Clone_Return(CUi_MapInterAction::ObjID, m_iLevelIndex, L"CUi", CUi_MapInterAction::ObjID, &Desc));
}

shared_ptr<CGenerator> CGenerator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CGenerator
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGenerator(pDevice, pContext)
        {}
    };

    shared_ptr<CGenerator> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGenerator"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CGenerator::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CGenerator
    {
        MakeSharedEnabler(const CGenerator& rhs) : CGenerator(rhs)
        {}
    };

    shared_ptr<CGenerator> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGenerator"));
        assert(false);
    }

    return pInstance;
}

void CGenerator::Free()
{
}
