#include "FloorSwitch.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"

#include "FloorSwitchButton.h"
#include "MapManager.h"

wstring CFloorSwitch::ObjID = TEXT("CFloorSwitch");

CFloorSwitch::CFloorSwitch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{pDevice, pContext}
{
}

CFloorSwitch::CFloorSwitch(const CFloorSwitch& rhs)
	: CGimmickBase{rhs}
{
}

CFloorSwitch::~CFloorSwitch()
{
	Free();
}

void CFloorSwitch::Set_Dead(bool bDead)
{
    m_bIsDead = bDead;

    m_pButton->Set_Dead(bDead);
}

HRESULT CFloorSwitch::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_FloorSwitch_Off"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_FloorSwitchA_Off_IZ_01.FBX"),
            MODEL_TYPE::NONANIM, Identity)
    );

    m_List_Owning_ObjectID.push_back(CFloorSwitchButton::ObjID);

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_FloorSwitch1", L"../Bin/Resources/Sounds/MapObject/Button_Push1.ogg", SOUND_CHANNEL::SOUND_EFFECT));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_FloorSwitch2", L"../Bin/Resources/Sounds/MapObject/Button_Push2.ogg", SOUND_CHANNEL::SOUND_EFFECT));

    return S_OK;
}

HRESULT CFloorSwitch::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

	return S_OK;
}

void CFloorSwitch::PriorityTick(_float fTimeDelta)
{
    m_pButton->PriorityTick(fTimeDelta);
}

void CFloorSwitch::Tick(_float fTimeDelta)
{
    //if (!m_bDone && static_pointer_cast<CFloorSwitchButton>(m_pButton)->Get_SwtichOn() == false)
    //{
    //    static_pointer_cast<CGimmickBase> (m_vecRelatives.back())->Execute();
    //    m_bDone = true;
    //}

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());

    m_pButton->Tick(fTimeDelta);
}

void CFloorSwitch::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

    if(!m_bDone)
        GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

    m_pButton->LateTick(fTimeDelta);
}

HRESULT CFloorSwitch::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        //	return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //	return E_FAIL;
        m_pShaderCom->Begin(0);

        //if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        //	return E_FAIL;

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

	return S_OK;
}

void CFloorSwitch::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CFloorSwitch::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CFloorSwitch::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);
}

void CFloorSwitch::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);
}

void CFloorSwitch::TakeDamage(const DamageInfo& damageInfo)
{
    if (!m_bDone)
    {
        m_bDone = true;
        auto pGI = m_pGameInstance.lock();
        pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_FloorSwitch1", m_pTransformCom->Get_Position());
        pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_FloorSwitch2", m_pTransformCom->Get_Position());

		EFFECTMANAGER->Call_Fixed_Union_Effect("Font_On_Effect", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);

        static_pointer_cast<CGimmickBase> (m_vecRelatives.back())->Execute();
        static_pointer_cast<CFloorSwitchButton>(m_pButton)->Set_SwitchOn(false);
        m_pButton->Set_Dead();
    }
}

HRESULT CFloorSwitch::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FloorSwitch_Off"), TEXT("Model_Com")));
    
    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vExtents = { 0.7f, 0.3f, 0.7f };
    obbDesc.vCenter = { 0.f, 0.3f, 0.f };
    obbDesc.vRotation = { 0.f, 0.f, 0.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

    if (!m_pShaderCom || !m_pModelCom || !m_pColliderCom)
        assert(false);

	return S_OK;
}

HRESULT CFloorSwitch::Bind_ShaderResources()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

HRESULT CFloorSwitch::Ready_Parts()
{
    CFloorSwitchButton::PARTOBJ_DESC partDesc{};
    partDesc.fRotationPerSec = 0.f;
    partDesc.fSpeedPerSec = 0.f;
    partDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    partDesc.pParentTransform = m_pTransformCom;

    //m_pButton = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CFloorSwitchButton::ObjID, &partDesc);
    m_pButton = GAMEINSTANCE->Get_Object_Prototype(CFloorSwitchButton::ObjID)->Clone(&partDesc);

    assert(m_pButton);

    return S_OK;
}

shared_ptr<CFloorSwitch> CFloorSwitch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CFloorSwitch
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFloorSwitch(pDevice, pContext)
        {}
    };

    shared_ptr<CFloorSwitch> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFloorSwitch"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CFloorSwitch::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CFloorSwitch
    {
        MakeSharedEnabler(const CFloorSwitch& rhs) : CFloorSwitch(rhs)
        {}
    };

    shared_ptr<CFloorSwitch> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFloorSwitch"));
        assert(false);
    }

    return pInstance;
}

void CFloorSwitch::Free()
{
}
