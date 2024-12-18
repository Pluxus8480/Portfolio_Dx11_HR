#include "CBattle3rdAfter.h"
#include "Cine_Manager.h"
#include "CUi_TalkSystem.h"


wstring CBattle3rdAfter::ObjID = TEXT("CBattle3rdAfter");

CBattle3rdAfter::CBattle3rdAfter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CBattle3rdAfter::CBattle3rdAfter(const CBattle3rdAfter& rhs)
	:CJeongGimmicBase(rhs)
{
}

CBattle3rdAfter::~CBattle3rdAfter()
{
}

HRESULT CBattle3rdAfter::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_Battle3rdAfter",
            L"../Bin/Resources/Models/Map/Gimmick/CBattle3rdAfter.fbx",
            MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("STKale22_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/STKale/STKale22_1.ogg")));

	return S_OK;
}

HRESULT CBattle3rdAfter::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_Battle3rdAfter",
        L"Prototype_Component_Shader_VtxMesh");
    Initialize_CookPhysX(L"Prototype_Component_Model_Battle3rdAfter");

    GAMEINSTANCE->Add_Unique_Sound_Channel("STKale");

    CCine_Manager::Get_Instance()->AddLastCallback
    ("EV1170", [&]()
        {
            m_bRender = true;
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

                    pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_KALESTAGE, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
                }

                auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
                if (PRequiredTalkSystem != nullptr)
                {
                    PRequiredTalkSystem->LoadTalkFile(L"STKale Talk_File_22.csv");

                    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
                }
            }
        }
    );
    
	return S_OK;
}

void CBattle3rdAfter::PriorityTick(_float fTimeDelta)
{
}

void CBattle3rdAfter::Tick(_float fTimeDelta)
{
    if (m_bRender)
    {
        Use_PhysX();
    }
    else
    {
        for (auto& iter : m_vecPxRigid)
        {
            iter->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 100000,-100000.f} });
        }
    }
}

void CBattle3rdAfter::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 50))
        return;

    if (m_bRender)
    {
        pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
    }
}

shared_ptr<CBattle3rdAfter> CBattle3rdAfter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CBattle3rdAfter
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBattle3rdAfter(pDevice, pContext)
        {}
    };

    shared_ptr<CBattle3rdAfter> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CBattle3rdAfter::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CBattle3rdAfter
    {
        MakeSharedEnabler(const CBattle3rdAfter& rhs) : CBattle3rdAfter(rhs)
        {}
    };

    shared_ptr<CBattle3rdAfter> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CBattle3rdAfter::Free()
{

}
