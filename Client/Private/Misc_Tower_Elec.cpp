#include "Misc_Tower_Elec.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Beat_Manager.h"
#include "MapManager.h"
#include "Effect_Manager.h"
#include "Cine_Manager.h"

wstring CMisc_Tower_Elec::ObjID = TEXT("CMisc_Tower_Elec");
CMisc_Tower_Elec::CMisc_Tower_Elec(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMisc_Tower_Elec::CMisc_Tower_Elec(const CMisc_Tower_Elec& rhs)
    : CGameObject{ rhs }
{
}

CMisc_Tower_Elec::~CMisc_Tower_Elec()
{
    Free();
}

HRESULT CMisc_Tower_Elec::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMisc_Tower_Elec::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
    m_iCurPhase = (_uint)KALE_PHASE::Antenna_Phase1;
    return S_OK;
}

void CMisc_Tower_Elec::PriorityTick(_float fTimeDelta)
{
}

void CMisc_Tower_Elec::Tick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
    {
        return;
    }
    _uint iCurrBeat = BEATMANAGER->Get_BeatCount();
    if (m_iPrevBeat != iCurrBeat)
    {
        m_iPrevBeat = iCurrBeat;
        if(m_iCurPhase == (_uint)KALE_PHASE::Antenna_Phase2)
        {
            EFFECTMANAGER->Call_Fixed_Union_Effect("Tower_Elec", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f / BEATMANAGER->Get_Beat());
        }
        else if (m_iCurPhase == (_uint)KALE_PHASE::Antenna_Phase3)
        {
            EFFECTMANAGER->Call_Fixed_Union_Effect("Tower_Elec1", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f / BEATMANAGER->Get_Beat());
        }
    }
}

void CMisc_Tower_Elec::LateTick(_float fTimeDelta)
{
}

HRESULT CMisc_Tower_Elec::Render()
{
    return S_OK;
}

void CMisc_Tower_Elec::Change_Phase(_uint iPhase)
{
    m_iCurPhase = iPhase;
    if (iPhase == (_uint)KALE_PHASE::Antenna_Phase2)
    {
        _vector vDir = XMVector3Normalize(-m_pTransformCom->Get_Position());
        m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() - vDir * 5.f);
    }
    if(iPhase == (_uint)KALE_PHASE::Antenna_Phase3)
    {
        _vector vDir = XMVector3Normalize(-m_pTransformCom->Get_Position());
        m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + vDir * 5.f);
    }
}

shared_ptr<CMisc_Tower_Elec> CMisc_Tower_Elec::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMisc_Tower_Elec
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMisc_Tower_Elec(pDevice, pContext)
        {}
    };

    shared_ptr<CMisc_Tower_Elec> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMisc_Tower_Elec"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMisc_Tower_Elec::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMisc_Tower_Elec
    {
        MakeSharedEnabler(const CMisc_Tower_Elec& rhs) : CMisc_Tower_Elec(rhs)
        {}
    };

    shared_ptr<CMisc_Tower_Elec> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMisc_Tower_Elec"));
        assert(false);
    }

    return pInstance;
}

void CMisc_Tower_Elec::Free()
{
}
