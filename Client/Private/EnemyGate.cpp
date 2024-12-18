#include "EnemyGate.h"
#include "GameInstance.h"

#include "PartObject.h"
#include "PartDoor.h"
#include "Beat_Manager.h"

wstring CEnemyGate::ObjID = TEXT("CEnemyGate");
CEnemyGate::CEnemyGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CEnemyGate::CEnemyGate(const CEnemyGate& rhs)
    : CGimmickBase{rhs}
{
}

CEnemyGate::~CEnemyGate()
{
    Free();
}

void CEnemyGate::Execute(_bool bExecute)
{
    if (m_bExecuted == bExecute || m_bDone)
        return;

    m_bExecuted = bExecute;
    //m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_DoorOpen", m_pTransformCom->Get_Position());
    //m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

    static_pointer_cast<CPartDoor>(m_vecParts[0])->Execute_Door(true);
    static_pointer_cast<CPartDoor>(m_vecParts[1])->Execute_Door(true);
}

void CEnemyGate::Set_Dead(bool bDead)
{
    m_bIsDead = bDead;

    for (auto& pPart : m_vecParts)
        pPart->Set_Dead(bDead);
}

HRESULT CEnemyGate::Initialize_Prototype()
{
    _float4x4 fMat = {};
    //XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));
     
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_EnemyGateR"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_EnemyGate_WB_R.FBX"), MODEL_TYPE::NONANIM, fMat)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_EnemyGateL"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_EnemyGate_WB_L.FBX"), MODEL_TYPE::NONANIM, fMat)
    );

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_DoorOpen", L"../Bin/Resources/Sounds/MapObject/Door_Open.ogg",
        SOUND_CHANNEL::SOUND_EFFECT));

    return S_OK;
}

HRESULT CEnemyGate::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Parts(pDesc)))
		return E_FAIL;

    return S_OK;
}

void CEnemyGate::PriorityTick(_float fTimeDelta)
{
    for (auto& pPart : m_vecParts)
        pPart->PriorityTick(fTimeDelta);
}

void CEnemyGate::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (GAMEINSTANCE->Get_KeyDown(DIK_L))
        Execute();
#endif
    
    Execute_Door(fTimeDelta);
    
    for (auto& pPart : m_vecParts)
        pPart->Tick(fTimeDelta);
}

void CEnemyGate::LateTick(_float fTimeDelta)
{
	//GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    for (auto& pPart : m_vecParts)
        pPart->LateTick(fTimeDelta);
}

HRESULT CEnemyGate::Render()
{
    return S_OK;
}

void CEnemyGate::Edit_Attribute()
{
    //ImGui::InputInt("Delay Beat", &m_iDelay);
}

void CEnemyGate::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CEnemyGate::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CEnemyGate::Save_Data(ofstream& os)
{
    __super::Save_Data(os);
    Save_Relatives(os);

    //write_typed_data(os, m_iDelay);
}

void CEnemyGate::Load_Data(ifstream& is)
{
    __super::Load_Data(is);
    Load_Relatives(is);

    //read_typed_data(is, m_iDelay);
}

HRESULT CEnemyGate::Ready_Parts(const COMMONOBJ_DESC* pDesc)
{
    CPartDoor::DOOR_PARTDESC doorDesc{};
    doorDesc.fRotationPerSec = { 0.f };
    doorDesc.fSpeedPerSec = { 15.f };
    doorDesc.pParentTransform = m_pTransformCom;
    doorDesc.strModelTag = TEXT("Prototype_Component_Model_EnemyGateL");
    doorDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    doorDesc.eDir = CPartDoor::DOOR_DIR::LEFT;
    doorDesc.fMoveDist = 8.f;
    XMStoreFloat4x4(&doorDesc.worldMatrix, XMMatrixTranslation(0.f, 0.f, 0.f));
    //shared_ptr<CGameObject> pDoorL = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &doorDesc);
    auto pDoorL = GAMEINSTANCE->Get_Object_Prototype(CPartDoor::ObjID)->Clone(&doorDesc);
    m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pDoorL));

    doorDesc.fSpeedPerSec = { 15.f };
    doorDesc.strModelTag = TEXT("Prototype_Component_Model_EnemyGateR");
    doorDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    doorDesc.eDir = CPartDoor::DOOR_DIR::RIGHT;
    doorDesc.fMoveDist = 8.f;
    XMStoreFloat4x4(&doorDesc.worldMatrix, XMMatrixTranslation(0.f, 0.f, 0.f));
    //shared_ptr<CGameObject> pDoorR = GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CPartDoor::ObjID, &doorDesc);
    auto pDoorR = GAMEINSTANCE->Get_Object_Prototype(CPartDoor::ObjID)->Clone(&doorDesc);
    m_vecParts.emplace_back(static_pointer_cast<CPartObject>(pDoorR));

    m_bOpen = false;
    static_pointer_cast<CPartDoor>(pDoorL)->Init_DoorState(m_bOpen);
    static_pointer_cast<CPartDoor>(pDoorR)->Init_DoorState(m_bOpen);

    return S_OK;
}

HRESULT CEnemyGate::Ready_Components()
{
    return S_OK;
}

HRESULT CEnemyGate::Bind_ShaderResources()
{
    return S_OK;
}

void CEnemyGate::Execute_Door(_float fTimeDelta)
{
    shared_ptr<CPartDoor> pDoorL = static_pointer_cast<CPartDoor>(m_vecParts[0]);
    shared_ptr<CPartDoor> pDoorR = static_pointer_cast<CPartDoor>(m_vecParts[1]);

    if (m_bExecuted)
    {
        if (pDoorL->Get_Done() && pDoorL->Get_Open())
        {
            if (pDoorR->Get_Done() && pDoorR->Get_Open())
            {
                m_bOpen = true;
                CGimmickBase::Done();
            }
        }
    }

    if (m_bDone && !m_bAllDone)
    {
        m_fTime += fTimeDelta;

        if (m_fTime > 1.f)
        {
            pDoorL->Execute_Door(true);
            pDoorR->Execute_Door(true);

            //if (pDoorR->Get_Done() && pDoorR->Get_Done())
                m_bAllDone = true;
        }
    }
}

shared_ptr<CEnemyGate> CEnemyGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEnemyGate
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEnemyGate(pDevice, pContext)
		{}
	};

	shared_ptr<CEnemyGate> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEnemyGate"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEnemyGate::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEnemyGate
	{
		MakeSharedEnabler(const CEnemyGate& rhs) : CEnemyGate(rhs)
		{}
	};


	shared_ptr<CEnemyGate> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEnemyGate"));
		assert(false);
	}

	return pInstance;
}

void CEnemyGate::Free()
{
}
