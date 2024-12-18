#include "MonsterSpawner.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_OBB.h"

#include "GameManager.h"
#include "CombatManager.h"
#include "MonsterBase.h"

#include <iostream>

wstring CMonsterSpawner::ObjID = TEXT("CMonsterSpawner");

CMonsterSpawner::CMonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{ pDevice, pContext }
{
}

CMonsterSpawner::CMonsterSpawner(const CMonsterSpawner& rhs)
    : CGimmickBase{rhs}
{
}

CMonsterSpawner::~CMonsterSpawner()
{
    Free();
}

void CMonsterSpawner::Execute(_bool bExecute)
{
    CGimmickBase::Done();
}

HRESULT CMonsterSpawner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonsterSpawner::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    return S_OK;
}

void CMonsterSpawner::PriorityTick(_float fTimeDelta)
{
}

void CMonsterSpawner::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
#endif
}

void CMonsterSpawner::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CMonsterSpawner::Render()
{
#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CMonsterSpawner::Edit_Attribute()
{
    _char szMonsterTag[MAX_PATH];
    strcpy(szMonsterTag, m_strMonsterTag.c_str());
    _char szMonsterAttribute[MAX_PATH];
    strcpy(szMonsterAttribute, m_strMonsterAttribute.c_str());

    if (ImGui::InputText("Monster Tag", szMonsterTag, MAX_PATH))
        m_strMonsterTag = szMonsterTag;
    if (ImGui::InputText("Monster Attribute", szMonsterAttribute, MAX_PATH))
        m_strMonsterAttribute = szMonsterAttribute;
}

void CMonsterSpawner::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    size_t size = m_strMonsterTag.size();
    write_typed_data(os, m_strMonsterTag.size());
    os.write(&m_strMonsterTag[0], m_strMonsterTag.size());

    write_typed_data(os, m_strMonsterAttribute.size());
    os.write(&m_strMonsterAttribute[0], m_strMonsterAttribute.size());
}

void CMonsterSpawner::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    size_t size = {};

    read_typed_data(is, size);
    m_strMonsterTag.resize(size);
    is.read(&m_strMonsterTag[0], size);

    read_typed_data(is, size);
    m_strMonsterAttribute.resize(size);
    is.read(&m_strMonsterAttribute[0], size);
}

CMonsterBase::Monster_DESC CMonsterSpawner::Make_MonsterDesc()
{
    _int iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();
    _wstring strMonsterTag = _wstring(m_strMonsterTag.begin(), m_strMonsterTag.end());

    CMonsterBase::Monster_DESC monsterDesc{};
    monsterDesc.strObjID = strMonsterTag;
    monsterDesc.strDesc = m_strMonsterAttribute;
    monsterDesc.matTransform = m_pTransformCom->Get_WorldMatrix();

    return monsterDesc;
}

HRESULT CMonsterSpawner::Ready_Components()
{
    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = { 0.f, 0.f, 0.f };
    obbDesc.vExtents = { 1.f, 1.f, 1.f };
    obbDesc.vRotation = { 0.f, 0.f, 0.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

    assert(m_pColliderCom);

    return S_OK;
}

HRESULT CMonsterSpawner::Bind_ShaderResources()
{
    return S_OK;
}

shared_ptr<CMonsterSpawner> CMonsterSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonsterSpawner
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonsterSpawner(pDevice, pContext)
        {}
    };

    shared_ptr<CMonsterSpawner> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonsterSpawner"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonsterSpawner::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonsterSpawner
    {
        MakeSharedEnabler(const CMonsterSpawner& rhs) : CMonsterSpawner(rhs)
        {}
    };

    shared_ptr<CMonsterSpawner> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonsterSpawner"));
        assert(false);
    }

    return pInstance;
}

void CMonsterSpawner::Free()
{
}
