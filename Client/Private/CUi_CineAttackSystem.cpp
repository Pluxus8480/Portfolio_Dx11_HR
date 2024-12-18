#include "CUi_CineAttackSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"
#include "CUi_SpecialComboAttackSystem.h"
#include "Cine_Manager.h"
#include "iostream"


wstring CUi_CineAttackSystem::ObjID = L"CUi_CineAttackSystem";

CUi_CineAttackSystem::CUi_CineAttackSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_CineAttackSystem::CUi_CineAttackSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_CineAttackSystem::~CUi_CineAttackSystem()
{
}

HRESULT CUi_CineAttackSystem::Initialize_Prototype()
{
    m_List_Owning_ObjectID.push_back(CUi_SpecialComboAttackSystem::ObjID);


	return S_OK;
}

HRESULT CUi_CineAttackSystem::Initialize(void* pArg)
{
	Initialize_Transform();

    CCine_Manager::Get_Instance()->AddSequenceCallback(
    "EV1250", 3, 0, [&](_float4, _float4, _float3) 
    {
       Macaron_ComboAttack();
    });

    CCine_Manager::Get_Instance()->AddSequenceCallback(
        "EV1250", 5, 0, [&](_float4, _float4, _float3)
        {
            Corsica_ComboAttack();
        });

    CCine_Manager::Get_Instance()->AddSequenceCallback(
        "EV1250", 7, 0, [&](_float4, _float4, _float3)
        {
            PepperMint_ComboAttack();
        });

    CCine_Manager::Get_Instance()->AddSequenceCallback(
        "EV1250", 7, 3, [&](_float4, _float4, _float3)
        {
            Cat808_ComboAttack();
        });

	return S_OK;
}

void CUi_CineAttackSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_CineAttackSystem::Tick(_float fTimeDelta)
{
}

void CUi_CineAttackSystem::LateTick(_float fTimeDelta)
{
}

void CUi_CineAttackSystem::Macaron_ComboAttack()
{
    pair<_float, _int>  RhythmData = {};
    list<pair<_float, _int>> Rhythm_list = {};

    RhythmData.first = 0.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    list <list<pair<_float, _int>>> listDanceEvent_Beats;
    list <_float> list_Dance_Beat;

    listDanceEvent_Beats.push_back(Rhythm_list);
    list_Dance_Beat.push_back(3);
    PLAYERMANAGER->Set_Dance_Rhythm(listDanceEvent_Beats);
    PLAYERMANAGER->Set_Dance_Beats(list_Dance_Beat);

    CUi_SpecialComboAttackSystem::Desc Desc;
    Desc.eSPCombo_Type = CUi_SpecialComboAttackSystem::SPCombo_Type::Kale_Finish;
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_SpecialComboAttackSystem::ObjID, m_iLevelIndex, L"CUi",
        CUi_SpecialComboAttackSystem::ObjID, &Desc);
}

void CUi_CineAttackSystem::Corsica_ComboAttack()
{
    pair<_float, _int>  RhythmData = {};
    list<pair<_float, _int>> Rhythm_list = {};

    RhythmData.first = 0.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    list <list<pair<_float, _int>>> listDanceEvent_Beats;
    list <_float> list_Dance_Beat;

    listDanceEvent_Beats.push_back(Rhythm_list);
    list_Dance_Beat.push_back(3);
    PLAYERMANAGER->Set_Dance_Rhythm(listDanceEvent_Beats);
    PLAYERMANAGER->Set_Dance_Beats(list_Dance_Beat);


    CUi_SpecialComboAttackSystem::Desc Desc;
    Desc.eSPCombo_Type = CUi_SpecialComboAttackSystem::SPCombo_Type::Kale_Finish;
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_SpecialComboAttackSystem::ObjID, m_iLevelIndex, L"CUi",
        CUi_SpecialComboAttackSystem::ObjID, &Desc);
}

void CUi_CineAttackSystem::PepperMint_ComboAttack()
{
    pair<_float, _int>  RhythmData = {};
    list<pair<_float, _int>> Rhythm_list = {};

    RhythmData.first = 0.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    list <list<pair<_float, _int>>> listDanceEvent_Beats;
    list <_float> list_Dance_Beat;

    listDanceEvent_Beats.push_back(Rhythm_list);
    list_Dance_Beat.push_back(3);
    PLAYERMANAGER->Set_Dance_Rhythm(listDanceEvent_Beats);
    PLAYERMANAGER->Set_Dance_Beats(list_Dance_Beat);

    CUi_SpecialComboAttackSystem::Desc Desc;
    Desc.eSPCombo_Type = CUi_SpecialComboAttackSystem::SPCombo_Type::Kale_Finish;
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_SpecialComboAttackSystem::ObjID, m_iLevelIndex, L"CUi",
        CUi_SpecialComboAttackSystem::ObjID, &Desc);
}

void CUi_CineAttackSystem::Cat808_ComboAttack()
{
    pair<_float, _int>  RhythmData = {};
    list<pair<_float, _int>> Rhythm_list = {};

    RhythmData.first = 0.f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    list <list<pair<_float, _int>>> listDanceEvent_Beats;
    list <_float> list_Dance_Beat;

    listDanceEvent_Beats.push_back(Rhythm_list);
    list_Dance_Beat.push_back(3);
    PLAYERMANAGER->Set_Dance_Rhythm(listDanceEvent_Beats);
    PLAYERMANAGER->Set_Dance_Beats(list_Dance_Beat);

    CUi_SpecialComboAttackSystem::Desc Desc;
    Desc.eSPCombo_Type = CUi_SpecialComboAttackSystem::SPCombo_Type::Kale_Finish;
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_SpecialComboAttackSystem::ObjID, m_iLevelIndex, L"CUi",
        CUi_SpecialComboAttackSystem::ObjID, &Desc);
}

shared_ptr<CUi_CineAttackSystem> CUi_CineAttackSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_CineAttackSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_CineAttackSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_CineAttackSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_CineAttackSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_CineAttackSystem
	{
		MakeSharedEnabler(const CUi_CineAttackSystem& rhs) : CUi_CineAttackSystem(rhs) { }
	};

	shared_ptr<CUi_CineAttackSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
