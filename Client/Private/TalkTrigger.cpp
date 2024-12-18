#include "TalkTrigger.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "CharacterController.h"
#include "PlayerManager.h"
#include "CUi_TalkSystem.h"
wstring CTalkTrigger::ObjID = TEXT("CTalkTrigger");
CTalkTrigger::CTalkTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CTalkTrigger::CTalkTrigger(const CTalkTrigger& rhs)
    : CGimmickBase{rhs}
{
}

CTalkTrigger::~CTalkTrigger()
{
    Free();
}

HRESULT CTalkTrigger::Initialize_Prototype()
{
    //Stage1
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Transfer2DTalk"), TEXT("../Bin/Resources/Sounds/Ui/Talk/Stage1/ST1Transfer2D.ogg")));

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("ST1LaserSpeaker1_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/Stage1/ST1LaserSpeaker1_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("ST1LaserSpeaker1_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/Stage1/ST1LaserSpeaker1_2.ogg")));

    //StageKale
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale1_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale1_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale1_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale1_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale1_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale1_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale2_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale2_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale3_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale3_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale3_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale3_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale4_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale4_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale4_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale4_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale5_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale5_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale6_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale6_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale6_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale6_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale6_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale6_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale7_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale7_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale7_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale7_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale8_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale8_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale9_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale9_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale10_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale10_1.ogg")));

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale21_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale21_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale24_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale24_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale25_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale25_1.ogg")));

    return S_OK;
}

HRESULT CTalkTrigger::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    GAMEINSTANCE->Add_Unique_Sound_Channel("ST1_2D");
    GAMEINSTANCE->Add_Unique_Sound_Channel("ST1_Laser_1");
    GAMEINSTANCE->Add_Unique_Sound_Channel("STKale");

    return S_OK;
}

void CTalkTrigger::PriorityTick(_float fTimeDelta)
{
}

void CTalkTrigger::Tick(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CTalkTrigger::LateTick(_float fTimeDelta)
{
    GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);

#ifdef _DEBUG
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CTalkTrigger::Render()
{
#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif
    return S_OK;
}

void CTalkTrigger::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (m_iCollisionTiming != 0) return;

    if (!m_bTalk && pOther->Get_ColliderTag() == TEXT("Player_Hit"))
    {
        m_bTalk = true;
        m_isTalkOver = false;

        auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
        if (pTalkSystem == nullptr)
        {
            CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
            TalkDesc.endCallback = [&]() {
                m_isTalkOver = true;
                };

            pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_TEST, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
        }

        auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
        if (PRequiredTalkSystem != nullptr)
        {
            wstring strTalkTag = wstring(m_strTalkTag.begin(), m_strTalkTag.end());
            PRequiredTalkSystem->LoadTalkFile(strTalkTag);

            CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
        }
    }
}

void CTalkTrigger::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (m_iCollisionTiming != 1) return;
}

void CTalkTrigger::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (m_iCollisionTiming != 2) return;

    if (!m_bTalk)
    {
        m_bTalk = true;
        m_isTalkOver = false;

        auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
        if (pTalkSystem == nullptr)
        {
            CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
            TalkDesc.endCallback = [&]() {
                m_isTalkOver = true;
                };

            pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_TEST, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
        }

        auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
        if (PRequiredTalkSystem != nullptr)
        {
            wstring strTalkTag = wstring(m_strTalkTag.begin(), m_strTalkTag.end());
            PRequiredTalkSystem->LoadTalkFile(strTalkTag);

            CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
        }
    }
}

void CTalkTrigger::Edit_Attribute()
{
    _char szEventTag[MAX_PATH];
    strcpy(szEventTag, m_strTalkTag.c_str());

    if (ImGui::InputText("Event Tag", szEventTag, MAX_PATH))
        m_strTalkTag = szEventTag;

    ImGui::RadioButton("Enter", &m_iCollisionTiming, 0); ImGui::SameLine();
    ImGui::RadioButton("ing", &m_iCollisionTiming, 1); ImGui::SameLine();
    ImGui::RadioButton("Exit", &m_iCollisionTiming, 2);
}

void CTalkTrigger::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    size_t size = m_strTalkTag.size();
    write_typed_data(os, m_strTalkTag.size());
    os.write(&m_strTalkTag[0], m_strTalkTag.size());

    write_typed_data(os, m_iCollisionTiming);
}

void CTalkTrigger::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    size_t size = {};

    read_typed_data(is, size);
    m_strTalkTag.resize(size);
    is.read(&m_strTalkTag[0], size);

    read_typed_data(is, m_iCollisionTiming);
}

HRESULT CTalkTrigger::Ready_Components()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
    aabbDesc.vCenter = { 0.f, 0.f, 0.f };
    aabbDesc.vExtents = { 1.f, 1.f, 1.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

    assert(m_pColliderCom);

    return S_OK;
}

shared_ptr<CTalkTrigger> CTalkTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CTalkTrigger
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTalkTrigger(pDevice, pContext)
        {}
    };

    shared_ptr<CTalkTrigger> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTalkTrigger"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CTalkTrigger::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CTalkTrigger
    {
        MakeSharedEnabler(const CTalkTrigger& rhs) : CTalkTrigger(rhs)
        {}
    };

    shared_ptr<CTalkTrigger> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTalkTrigger"));
        assert(false);
    }

    return pInstance;
}

void CTalkTrigger::Free()
{
}
