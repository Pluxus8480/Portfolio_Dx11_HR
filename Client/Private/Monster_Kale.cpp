#include "Monster_Kale.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Shader.h"
#include "Model.h"
#include "CharacterController.h"

#include "GameManager.h"
#include "Beat_Manager.h"
#include "Effect_Manager.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"


#include "CombatManager.h"
#include "PickUpItem.h"
#include "GimmickBase.h"

#include "PartObject.h"
#include "Kale_Sword.h"
#include "Kale_Funnel.h"
#include "Kale_BArm.h"
#include "Kale_FArm.h"
#include "Kale_SArm.h"

#include "Chai.h"
#include "CUi_KaleSystem.h"
#include "CUi_StunGaugeSystem.h"
#include "Player_Camera.h"
#include "CUi_EnemySpecialAttack.h"
#include "CUi_BeatMarker.h"

#include "808_Playable.h"
#include "Kale_FireWall.h"
#include "Kale_FinaleSequence.h"
#include "AfterImage.h"
#include "Kale_BigSword.h"
#include "MapManager.h"
#include "GimmickManager.h"
#include "CKaleBoss_ObjSystem.h"
#include "CUi_TalkSystem.h"
#include "Cine_Manager.h"
#include "UI_Event_Cut.h"


wstring CMonster_Kale::ObjID = TEXT("CMonster_Kale");

CMonster_Kale::CMonster_Kale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonsterBase{ pDevice, pContext }
{
}

CMonster_Kale::CMonster_Kale(const CMonster_Kale& rhs)
    : CMonsterBase{ rhs }
{
}

CMonster_Kale::~CMonster_Kale()
{
    Free();
}

HRESULT CMonster_Kale::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationRollPitchYaw(0.f, -XM_PIDIV2, 0.f);

    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Monster_Kale"), TEXT("../Bin/Resources/Models/Monsters/Kale/kale_hairCut_Anim.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Monster_Kale_NoCoat"), TEXT("../Bin/Resources/Models/Monsters/Kale/kale_hairCut_Non_Coat.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_Owning_ObjectID.push_back(CKale_Sword::ObjID);
    m_List_Owning_ObjectID.push_back(CKale_Funnel::ObjID);
    m_List_Owning_ObjectID.push_back(CKale_BArm::ObjID);
    m_List_Owning_ObjectID.push_back(CKale_FArm::ObjID);
    m_List_Owning_ObjectID.push_back(CKale_SArm::ObjID);

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKale1_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKale/vsKaleChapter1_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKale2_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKale/vsKaleChapter2_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKale3_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKale/vsKaleChapter3_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKale4_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKale/vsKaleChapter4_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKale5_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKale/vsKaleChapter5_1.ogg")));


    return S_OK;
}

HRESULT CMonster_Kale::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    GAMEINSTANCE->Add_Unique_Sound_Channel("BattleKale");

    //auto pDesc = static_cast<Monster_DESC*>(pArg);
    //_float4x4 matTrans = pDesc->matTransform;
    //m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matTrans));

    //string strDesc = pDesc->strDesc;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    PrepareCallbacks();

    // Init Hp
    m_fCurHp = m_fMaxHp;

    // Physix setting
 /*   if (strDesc == "1")
        CCT_OFF();
    else
        CCT_ON();*/
    CCT_ON();

    // For tool setted effects
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
        {
            //if (strName == "em0100_warningsign")
               // CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matSizeUp_Att, m_fBeatSpeed, isBillBoard);
            //else
                CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fBeatSpeed, isBillBoard);
        }
        else
        {
            //if (strName == "em0100_warningsign")
               // CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matSizeUp_Att, m_fBeatSpeed, isBillBoard);
            //else
                CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fBeatSpeed, isBillBoard);
        }
        }
    );
    m_pModelNoCoatCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
        {
            //if (strName == "em0100_warningsign")
               // CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matSizeUp_Att, m_fBeatSpeed, isBillBoard);
            //else
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fBeatSpeed, isBillBoard);
        }
        else
        {
            //if (strName == "em0100_warningsign")
               // CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matSizeUp_Att, m_fBeatSpeed, isBillBoard);
            //else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fBeatSpeed, isBillBoard);
        }
        }
    );
#pragma region Anim_Initialize
    m_strAnimTag[ATK_45L_60R] = "em7500_atk_000";
    m_AnimFlag[ATK_45L_60R] = { 45.f, 60.f, 120.f };
    m_strAnimTag[ATK_LUNGE_75L] = "em7500_atk_010";
    m_AnimFlag[ATK_LUNGE_75L] = { 75.f, 120.f };
    m_strAnimTag[ATK_LIGHTNING] = "em7500_atk_040";
    m_AnimFlag[ATK_LIGHTNING] = { 135.f, 165.f, 180.f, 240.f };
    m_strAnimTag[ATK_JUMP_75D] = "em7500_atk_030";
    m_AnimFlag[ATK_JUMP_75D] = { 75.f, 90.f, 130.f };
    m_strAnimTag[ATK_SLASH_60L] = "em7500_atk_050";
    m_AnimFlag[ATK_SLASH_60L] = { 60.f, 118.f };
    m_strAnimTag[ATK_75L_90R_105L] = "em7500_atk_055";
    m_AnimFlag[ATK_75L_90R_105L] = { 75.f, 90.f, 105.f, 164.f };
    m_strAnimTag[ATK_DASH_60L_75R_90L_130R_140L_165U] = "em7500_atk_060";
    m_AnimFlag[ATK_DASH_60L_75R_90L_130R_140L_165U] = {60.f, 75.f, 90.f, 130.f, 140.f, 165.f, 225.f };
    m_strAnimTag[ATK_POWERWAVE_120L] = "em7500_atk_070";
    m_AnimFlag[ATK_POWERWAVE_120L] = { 105.f, 120.f, 150.f, 237.f };//3
    m_strAnimTag[ATK_POWERWAVE_45R] = "em7500_atk_075";
    m_AnimFlag[ATK_POWERWAVE_45R] = { 45.f, 105.f, 165.f };//4
    m_strAnimTag[ATK_POWERWAVE_45R_67L] = "em7500_atk_076";
    m_AnimFlag[ATK_POWERWAVE_45R_67L] = { 45.f, 62.5f, 105.f, 165.f };//4
    m_strAnimTag[ATK_POWERWAVE_45R_67L_82R] = "em7500_atk_077";
    m_AnimFlag[ATK_POWERWAVE_45R_67L_82R] = { 45.f, 62.5f, 75.0f, 105.f, 165.f };//4
    m_strAnimTag[ATK_120R_150L_165R] = "em7500_atk_080";
    m_AnimFlag[ATK_120R_150L_165R] = { 120.f, 150.f, 165.f, 220.f };
    m_strAnimTag[ATK_LUNGE_45L_72R] = "em7500_atk_090";
    m_AnimFlag[ATK_LUNGE_45L_72R] = { 45.f, 72.5f, 124.f };
    m_strAnimTag[ATK_ATTACH_ARM] = "em7500_atk-arm_000";
    m_AnimFlag[ATK_ATTACH_ARM] = {};

    m_strAnimTag[ATK_FUNNEL_105] = "em7500_atk-funnel_000";
    m_AnimFlag[ATK_FUNNEL_105] = { 105.f, 180.f };
    m_strAnimTag[ATK_COUNTER_45R] = "em7500_atk-counter_000";
    m_AnimFlag[ATK_COUNTER_45R] = { 45.f, 80.f };

    m_strAnimTag[B_ARM_START] = "em7500_Barm-chance-cp_020";
    m_AnimFlag[B_ARM_START] = { 45.f };
    m_strAnimTag[B_ARM_LOOP] = "em7500_Barm-chance-cp_021";
    m_AnimFlag[B_ARM_LOOP] = { 60.f };
    m_strAnimTag[B_ARM_SHOOT_30_60_90] = "em7500_Barm-chance-cp_022";
    m_AnimFlag[B_ARM_SHOOT_30_60_90] = { 60.f, 90.f };
    m_strAnimTag[B_ARM_GROGGY] = "em7500_Barm-chance-cp_030";
    m_AnimFlag[B_ARM_GROGGY] = { 30.f };
    m_strAnimTag[B_ARM_GROGGY_LOOP] = "em7500_Barm-chance-cp_031";
    m_AnimFlag[B_ARM_GROGGY_LOOP] = { 30.f };
    m_strAnimTag[B_ARM_RECOVER] = "em7500_Barm-chance-cp_032";
    m_AnimFlag[B_ARM_RECOVER] = { 60.f };
    m_strAnimTag[B_ARM_DESTROY] = "em7500_Barm-chance-cp_050";
    m_AnimFlag[B_ARM_DESTROY] = { 60.f };

    m_strAnimTag[F_ARM_START] = "em7500_Farm-chance-cp_020";
    m_AnimFlag[F_ARM_START] = { 65.f };
    m_strAnimTag[F_ARM_LOOP] = "em7500_Farm-chance-cp_021";
    m_AnimFlag[F_ARM_LOOP] = { 60.f };
    m_strAnimTag[F_ARM_SHOOT_30TO75] = "em7500_Farm-chance-cp_022";
    m_AnimFlag[F_ARM_SHOOT_30TO75] = { 30.f, 100.f };
    m_strAnimTag[F_ARM_GROGGY] = "em7500_Farm-chance-cp_030";
    m_AnimFlag[F_ARM_GROGGY] = { 30.f };
    m_strAnimTag[F_ARM_GROGGY_LOOP] = "em7500_Farm-chance-cp_031";
    m_AnimFlag[F_ARM_GROGGY_LOOP] = { 30.f };
    m_strAnimTag[F_ARM_RECOVER] = "em7500_Farm-chance-cp_032";
    m_AnimFlag[F_ARM_RECOVER] = { 60.f };
    m_strAnimTag[F_ARM_DESTROY] = "em7500_Farm-chance-cp_050";
    m_AnimFlag[F_ARM_DESTROY] = { 60.f };

    m_strAnimTag[S_ARM_START_90] = "em7500_Sarm-chance-cp_020";
    m_AnimFlag[S_ARM_START_90] = { 90.f, 120.f };
    m_strAnimTag[S_ARM_LOOP] = "em7500_Sarm-chance-cp_021";
    m_AnimFlag[S_ARM_LOOP] = { 60.f };
    m_strAnimTag[S_ARM_SHOOT] = "em7500_Sarm-chance-cp_022";
    m_AnimFlag[S_ARM_SHOOT] = { 30.f };
    m_strAnimTag[S_ARM_GROGGY] = "em7500_Sarm-chance-cp_030";
    m_AnimFlag[S_ARM_GROGGY] = { 30.f };
    m_strAnimTag[S_ARM_GROGGY_LOOP] = "em7500_Sarm-chance-cp_031";
    m_AnimFlag[S_ARM_GROGGY_LOOP] = { 30.f };
    m_strAnimTag[S_ARM_RECOVER] = "em7500_Sarm-chance-cp_032";
    m_AnimFlag[S_ARM_RECOVER] = { 60.f };
    m_strAnimTag[S_ARM_DESTROY] = "em7500_Sarm-chance-cp_050";
    m_AnimFlag[S_ARM_DESTROY] = { 60.f };

    m_strAnimTag[DMG_AIR_START] = "em7500_dmg-air_000";
    m_AnimFlag[DMG_AIR_START] = { 45.f };
    m_strAnimTag[DMG_AIR_LOOP] = "em7500_dmg-air_001";
    m_AnimFlag[DMG_AIR_LOOP] = { 30.f };
    m_strAnimTag[DMG_AIR_LAND] = "em7500_dmg-air_002";
    m_AnimFlag[DMG_AIR_LAND] = { 15.f };
    m_strAnimTag[DMG_AIR_LIEDOWN] = "em7500_dmg-air_003";
    m_AnimFlag[DMG_AIR_LIEDOWN] = { 60.f };
    m_strAnimTag[DMG_AIR_RECOVER] = "em7500_dmg-air_004";
    m_AnimFlag[DMG_AIR_RECOVER] = { 45.f };
    m_strAnimTag[DMG_AIR_HIT] = "em7500_dmg-air_010";
    m_AnimFlag[DMG_AIR_HIT] = { 15.f };

    m_strAnimTag[DMG_AIR_THROW_START] = "em7500_dmg-air_020";
    m_AnimFlag[DMG_AIR_THROW_START] = { 30.f };
    m_strAnimTag[DMG_AIR_THROW_FALL] = "em7500_dmg-air_021";
    m_AnimFlag[DMG_AIR_THROW_FALL] = { 20.f };
    m_strAnimTag[DMG_AIR_THROW_LAND] = "em7500_dmg-air_022";
    m_AnimFlag[DMG_AIR_THROW_LAND] = { 15.f };

    m_strAnimTag[DMG_GUARD_ON] = "em7500_dmg_000";
    m_AnimFlag[DMG_GUARD_ON] = { 30.f };
    m_strAnimTag[DMG_GUARD_LOOP] = "em7500_dmg_001";
    m_AnimFlag[DMG_GUARD_LOOP] = { 30.f };
    m_strAnimTag[DMG_GUARD_OFF] = "em7500_dmg_002";
    m_AnimFlag[DMG_GUARD_OFF] = { 37.f };

    m_strAnimTag[DMG_KNOCKBACK] = "em7500_dmg_010";
    m_AnimFlag[DMG_KNOCKBACK] = { 84.f };


    m_strAnimTag[DMG_GROGGY_START] = "em7500_dmg_020";
    m_AnimFlag[DMG_GROGGY_START] = { 45.f };
    m_strAnimTag[DMG_GROGGY_LOOP] = "em7500_dmg_021";
    m_AnimFlag[DMG_GROGGY_LOOP] = { 30.f };
    m_strAnimTag[DMG_GROGGY_RECOVER] = "em7500_dmg_022";
    m_AnimFlag[DMG_GROGGY_RECOVER] = { 54.f };
    m_strAnimTag[DMG_GROGGY_HIT] = "em7500_dmg_023";
    m_AnimFlag[DMG_GROGGY_HIT] = { 40.f };
    m_strAnimTag[DMG_GROGGY_HIT_KNOCKBACK] = "em7500_dmg_024";
    m_AnimFlag[DMG_GROGGY_HIT_KNOCKBACK] = { 50.f };

    m_strAnimTag[EVENT_808_RUN] = "em7500_event-kale_000";
    m_AnimFlag[EVENT_808_RUN] = {};
    m_strAnimTag[EVENT_808_SUCCESS] = "em7500_event-kale_005";
    m_AnimFlag[EVENT_808_SUCCESS] = {};
    m_strAnimTag[EVENT_808_FAIL] = "em7500_event-kale_010";
    m_AnimFlag[EVENT_808_FAIL] = {};

    m_strAnimTag[EVENT_FIRST_PHASE_END] = "em7500_event-kale_100";
    m_AnimFlag[EVENT_FIRST_PHASE_END] = {};
    m_strAnimTag[EVENT_FIRST_RP_READY] = "em7500_event-kale_110";
    m_AnimFlag[EVENT_FIRST_RP_READY] = {0.f, 20.f, 45.f, 70.f, 90.f };
    m_strAnimTag[EVENT_FIRST_RP_ATK] = "em7500_event-kale_120";
    m_AnimFlag[EVENT_FIRST_RP_ATK] = {0.f, 20.f, 45.f, 70.f, 90.f };
    m_strAnimTag[EVENT_FIRST_RP_FAIL] = "em7500_event-kale_130";
    m_AnimFlag[EVENT_FIRST_RP_FAIL] = {};
    m_strAnimTag[EVENT_FIRST_RP_SUCCESS] = "em7500_event-kale_140";
    m_AnimFlag[EVENT_FIRST_RP_SUCCESS] = {};

    m_strAnimTag[EVENT_THIRD_PHASE_END] = "em7500_event-kale_200";
    m_AnimFlag[EVENT_THIRD_PHASE_END] = {};
    m_strAnimTag[EVENT_THIRD_RP_READY] = "em7500_event-kale_210";
    m_AnimFlag[EVENT_THIRD_RP_READY] = { 20.f, 35.f, 50.f, 57.5f, 72.5f, 87.5f };
    m_strAnimTag[EVENT_THIRD_RP_ATK] = "em7500_event-kale_220";
    m_AnimFlag[EVENT_THIRD_RP_ATK] = { 20.f, 35.f, 50.f, 57.5f, 72.5f, 87.5f };
    m_strAnimTag[EVENT_THIRD_RP_FAIL] = "em7500_event-kale_230";
    m_AnimFlag[EVENT_THIRD_RP_FAIL] = { };
    m_strAnimTag[EVENT_THIRD_RP_SUCCESS] = "em7500_event-kale_240";
    m_AnimFlag[EVENT_THIRD_RP_SUCCESS] = { };

    m_strAnimTag[IDLE_0] = "em7500_idle_000";
    m_AnimFlag[IDLE_0] = { };
    m_strAnimTag[IDLE_BOARED] = "em7500_idle_001";
    m_AnimFlag[IDLE_BOARED] = { };
    m_strAnimTag[IDLE_TAUNT] = "em7500_idle_002";
    m_AnimFlag[IDLE_TAUNT] = { };

    m_strAnimTag[MVNT_RUN_15] = "em7500_mvnt-run_000";
    m_AnimFlag[MVNT_RUN_15] = { 15.f };
    m_strAnimTag[MVNT_STEP_FORWARD] = "em7500_mvnt-step_000";
    m_AnimFlag[MVNT_STEP_FORWARD] = { 30.f };
    m_strAnimTag[MVNT_STEP_BACKWARD] = "em7500_mvnt-step_001";
    m_AnimFlag[MVNT_STEP_BACKWARD] = { 30.f };
    m_strAnimTag[MVNT_STEP_LEFT] = "em7500_mvnt-step_002";
    m_AnimFlag[MVNT_STEP_LEFT] = { 30.f };
    m_strAnimTag[MVNT_STEP_RIGHT] = "em7500_mvnt-step_003";
    m_AnimFlag[MVNT_STEP_RIGHT] = { 30.f };

    m_strAnimTag[MVNT_WALK_FORWARD] = "em7500_mvnt-walk-st_000";
    m_AnimFlag[MVNT_WALK_FORWARD] = { 60.f };
    m_strAnimTag[MVNT_WALK_BACKWARD] = "em7500_mvnt-walk-st_001";
    m_AnimFlag[MVNT_WALK_BACKWARD] = { 60.f };
    m_strAnimTag[MVNT_WALK_LEFT] = "em7500_mvnt-walk-st_002";
    m_AnimFlag[MVNT_WALK_LEFT] = { 60.f };
    m_strAnimTag[MVNT_WALK_RIGHT] = "em7500_mvnt-walk-st_003";
    m_AnimFlag[MVNT_WALK_RIGHT] = { 60.f };
    m_strAnimTag[MVNT_WALK_LEFT_BACK] = "em7500_mvnt-walk-st_008";
    m_AnimFlag[MVNT_WALK_LEFT_BACK] = { 60.f };
    m_strAnimTag[MVNT_WALK_RIGHT_BACK] = "em7500_mvnt-walk-st_009";
    m_AnimFlag[MVNT_WALK_RIGHT_BACK] = { 60.f };
#pragma endregion //Anim_Initialize
    m_eCurrState = IDLE_0;
    m_eNextState = ATK_POWERWAVE_120L;
    //m_eNextState = S_ARM_START_90;
    m_pModelCom->Set_AnimIndex(m_strAnimTag[m_eCurrState], true);
    CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());
    m_iGroggyGauge = m_iMaxGroggyGauge;

    auto pUi = static_pointer_cast<CUi_KaleSystem>(CUi_Mgr::Get_Instance()->Find_UI(CUi_KaleSystem::ObjID));
    if (pUi == nullptr)
        assert(false);

    pUi->Set_KaleHp(&m_fCurHp);
    pUi->Set_KaleGroggy(&m_iGroggyGauge);
    pUi->Set_KaleMaxHp(&m_fMaxHp);
    CUi_Mgr::Get_Instance()->Ui_AllActive(false);

    CUi_StunGaugeSystem::Desc Desc;
    Desc.fMaxStunGauge = m_iGroggyGauge;
    Desc.pCurStunGauge = &m_iGroggyGauge;
    Desc.iStar = 3;
    Desc.pTargetTransform = m_pTransformCom;
    Desc.RelativeDistance = { 0.f, 0.f, 0 };
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_StunGaugeSystem::ObjID, m_iLevelIndex, L"CUi", CUi_StunGaugeSystem::ObjID, &Desc);

    m_FinaleSequencer = make_shared<CKale_FinaleSequence>(static_pointer_cast<CMonster_Kale>(shared_from_this()));

    m_pAfterImage = static_pointer_cast<CAfterImage>(m_pGameInstance.lock()->Clone_Object(L"Prototype_GameObject_AfterImage_Anim_500milsec_10", &m_pModelCom));
    m_pAfterImage->Set_Color({ 0.1f,0.4f,0.6f, 0.5f });
    [&]() {
        _float4x4 matResult;
        CCine_Manager::Get_Instance()->GetMatTrans("em7500", matResult);
        auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        m_pTransformCom->Set_Position(vNewPos);
        m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        };


    CCine_Manager::Get_Instance()->AddLastCallback("EV1250", [&]() {
        //CCine_Manager::Get_Instance()->StartCine("CS1225");
        m_bFinalCS = true;
        });
    CCine_Manager::Get_Instance()->AddLastCallback("CS1205", [&]() {
        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        shared_ptr<CKaleBoss_ObjSystem> pInstance = static_pointer_cast
            <CKaleBoss_ObjSystem>(CGimmickManager::Get_Instance()->Get_GimmckObj
            (string(CKaleBoss_ObjSystem::ObjID.begin(), CKaleBoss_ObjSystem::ObjID.end())));
        pInstance->Set_Pos(0.f,-150.f,0.f);
        //_float4x4 matResult;
        //CCine_Manager::Get_Instance()->GetMatTrans("em7500", matResult);
        //auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        //m_pTransformCom->Set_Position(vNewPos);
        //m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        });
    CCine_Manager::Get_Instance()->AddLastCallback("CS1210", [&]() {
        CUi_Mgr::Get_Instance()->Ui_AllActive();
        shared_ptr<CKaleBoss_ObjSystem> pInstance = static_pointer_cast
            <CKaleBoss_ObjSystem>(CGimmickManager::Get_Instance()->Get_GimmckObj
            (string(CKaleBoss_ObjSystem::ObjID.begin(), CKaleBoss_ObjSystem::ObjID.end())));
        pInstance->Set_Pos(0.f, 0.f, 0.f);
        _float4x4 matResult;
        CCine_Manager::Get_Instance()->GetMatTrans("em7500", matResult);
        auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        m_pTransformCom->Set_Position(vNewPos);
        m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        });

    CCine_Manager::Get_Instance()->AddLastCallback("CS1215", [&]() {
        CUi_Mgr::Get_Instance()->Ui_AllActive();
        _float4x4 matResult;
        CCine_Manager::Get_Instance()->GetMatTrans("em7500", matResult);
        auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        m_pTransformCom->Set_Position(vNewPos);
        m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        });
    CCine_Manager::Get_Instance()->AddLastCallback("CS1220", [&]() {
        CUi_Mgr::Get_Instance()->Ui_AllActive();
        _float4x4 matResult;
        CCine_Manager::Get_Instance()->GetMatTrans("em7700", matResult);
        auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        m_pTransformCom->Set_Position(vNewPos);
        m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        });

    CCine_Manager::Get_Instance()->AddStartCallback("CS1205", []() {
        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        });
    CCine_Manager::Get_Instance()->AddStartCallback("CS1210", []() {
        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        });
    CCine_Manager::Get_Instance()->AddStartCallback("CS1215", []() {
        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        });
    CCine_Manager::Get_Instance()->AddStartCallback("CS1220", []() {
        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        });
    CCine_Manager::Get_Instance()->AddStartCallback("CS1225", []() {
        BEATMANAGER->Set_Volume(0.0001f);
        });
    // CutScene
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS1220", 9, 0,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::KalePowerUp;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS1220", 9, 1,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );

    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 3, 1,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::KaleMacaron;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 3, 2,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 5, 1,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::KaleKorsica;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 5, 2,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 7, 1,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::KalePepper;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 7, 2,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 7, 4,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::KaleChai;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("EV1250", 7, 5,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );

    return S_OK;
}

void CMonster_Kale::PriorityTick(_float fTimeDelta)
{
    m_bIsInGame = CCine_Manager::Get_Instance()->IsInGame();
    if (!m_bIsInGame)
        return;
    {
        m_bBArm = !m_pBArm->IsBroken() && m_iPhase == 1;
        m_bFArm = !m_pFArm->IsBroken() && m_iPhase == 1;
        m_bSArm = !m_pSArmL->IsBroken() && m_iPhase == 1;
    }
    if (m_iPhase == 1 && !m_bBArm && !m_bFArm && !m_bSArm)
    {
        DamageInfo Info{};
        Info.fAmountDamage = 100.f;
        TakeDamage(Info);
    }

    m_bBodySlam = false;
    //if (m_isAirState)
    //{
    //    PxControllerState pCS;
    //    m_pCCT->Get_Controller()->getState(pCS);
    //    if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    //    {
    //        
    //    }
    //}

    //_vector vPlayerPos = PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position();
    //_vector vCurPos = m_pTransformCom->Get_Position();
    //_vector vCurLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

    //_vector vDirToPlayer = XMVectorSetY(vPlayerPos - vCurPos, 0.f);
    //m_fDistance = XMVectorGetX(XMVector4Length(vDirToPlayer));
    //vDirToPlayer = XMVector3Normalize(vDirToPlayer);
    //XMStoreFloat4(&m_vfDirToPlayer, vDirToPlayer);

    //_float fCrossY = XMVectorGetY(XMVector3Cross(vCurLook, vDirToPlayer));
    //_float fDot = XMVectorGetX(XMVector3Dot(vCurLook, vDirToPlayer));

    //m_fAngleDegree = XMConvertToDegrees(acosf(fDot));
    //if (fCrossY < 0.f)
    //    m_fAngleDegree *= -1.f;
    m_bBeatChanged = false;
    _uint iCurrBeat = BEATMANAGER->Get_BeatCount();
    if (m_iPrevBeat != iCurrBeat)
    {
        m_iPrevBeat = iCurrBeat;
        Beat_Move();
        m_bBeatChanged = true;
    }
    _vector vPlayerPos = PLAYERMANAGER->Get_PlayerPos();
    XMStoreFloat4(&m_vPlayerPos, vPlayerPos);
    m_fPlayerDist = XMVector3Length(XMVectorSetY(vPlayerPos - m_pTransformCom->Get_Position(), 0.f)).m128_f32[0];

    m_ePrevState = m_eCurrState;
    m_pSword->PriorityTick(fTimeDelta);
    for (_uint i = 0; i < 4; ++i)
    {
        m_ArrFunnel[i]->PriorityTick(fTimeDelta);
    }
    if(m_bBArm)
        m_pBArm->PriorityTick(fTimeDelta);
    if(m_bFArm)
        m_pFArm->PriorityTick(fTimeDelta);
    if(m_bSArm)
    {
        m_pSArmL->PriorityTick(fTimeDelta);
        m_pSArmR->PriorityTick(fTimeDelta);
    }
    m_pAfterImage->PriorityTick(fTimeDelta);
}

void CMonster_Kale::Tick(_float fTimeDelta)
{
    if (!m_bIsInGame)
        return;
    if (m_bFinalCS)
    {
        CCine_Manager::Get_Instance()->StartCine("CS1225");
    }

    m_fShakeTime -= fTimeDelta;
    // Play Animation
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
    SelectBeat();
    //if (m_pCurState->IsSlowAnim())
    //    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * 0.2f * m_fBeatSpeed * m_fTimeMultiflier, 0.1f, fmatResult);
    //else
    //if(m_eCurrState >= EVENT_FIRST_PHASE_END && m_eCurrState <= EVENT_FIRST_RP_SUCCESS)
    //    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fBeatSpeed * m_fTimeMultiflier, 0.1f, fmatResult, m_pModelCom->Get_BoneIdx("pelvis"));
    //else
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fBeatSpeed * m_fTimeMultiflier, 0.1f, fmatResult);
    _bool bTooClose = (m_fPlayerDist < 2.f);
    if (m_pModelCom->IsRootAnim() && !m_pModelCom->IsForcedNonRoot())
    {
        _matrix matResult = XMLoadFloat4x4(&fmatResult);
        auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);

        _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
        auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
        _vector vPos = m_pTransformCom->Get_Position();
        _vector vDir = vCurPos - vPos;
        _vector vPlayerDir = XMLoadFloat4(&m_vPlayerPos) - vPos;
        if (m_isAirState)
        {
            vDir.m128_f32[1] = max(vDir.m128_f32[1], 0.f);
            m_pCCT->Add_Displacement((vDir) / fTimeDelta);
        }
        else if ((!bTooClose || XMVector3Dot(XMVector3Normalize(vPlayerDir), XMVector3Normalize(vDir)).m128_f32[0] < cos(XM_PI * 1.333f)) 
            || (m_eCurrState >= MVNT_STEP_BACKWARD && m_eCurrState <= MVNT_STEP_RIGHT))
        {
            m_pCCT->Add_Displacement((vDir) / fTimeDelta);
        }
        else
        {
            m_pCCT->Add_Displacement(XMVectorSet(0.f, vDir.m128_f32[1], 0.f, 0.f) / fTimeDelta);
        }
       
        //m_pTransformCom->Set_Position(vCurPos);
    }
    m_fTrackFrame = m_pModelCom->Get_Current_Track_Position();
    Progress(fTimeDelta);
    if (m_pModelCom->Get_Finished())
        Animation_End();

    // Update Physics
    if (m_isCCTWork)
        m_pCCT->Update(fTimeDelta);

    // Update Colliders
    auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
    _float fCollisionScale = 1.f;
    if (m_eCurrState == S_ARM_LOOP || m_eCurrState == F_ARM_LOOP || m_eCurrState == B_ARM_LOOP)
    {
        fCollisionScale = 2.f;
    }
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]) * fCollisionScale;
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]) * fCollisionScale;
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]) * fCollisionScale;
    m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());

    auto matrixRightHand = XMLoadFloat4x4(m_pMat_RightHand);
    matrixRightHand.r[0] = XMVector3Normalize(matrixRightHand.r[0]);
    matrixRightHand.r[1] = XMVector3Normalize(matrixRightHand.r[1]);
    matrixRightHand.r[2] = XMVector3Normalize(matrixRightHand.r[2]);
    auto matAttackResult = matrixRightHand * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pCollider_Attack->Update(matAttackResult);

    XMStoreFloat4(&m_vCurDir_Attack, matAttackResult.r[3] - XMLoadFloat4(&m_vPrePos_Attack));
    XMStoreFloat4(&m_vPrePos_Attack, matAttackResult.r[3]);

    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    XMStoreFloat4x4(&m_matSizeUp, XMMatrixScaling(2.5f, 2.5f, 2.5f) * XMLoadFloat4x4(&m_matCurrentPos));
    XMStoreFloat4x4(&m_matSizeUp_Att, XMMatrixScaling(2.5f, 2.5f, 2.5f) * matrixRightHand * XMMatrixTranslationFromVector({ 0.f, 0.f, 1.f, 1.f }) * XMLoadFloat4x4(&m_matCurrentPos));
    m_pModelCom->PlayReservedCallbacks();



    // Update PartObjects
    m_pSword->Set_AnimSpeed(m_fBeatSpeed * m_fTimeMultiflier);
    m_pSword->Tick(fTimeDelta);
    for (_uint i = 0; i < 4; ++i)
    {
        m_ArrFunnel[i]->Tick(fTimeDelta);
    }
    if (m_bBArm)
    {
        m_pBArm->Set_AnimSpeed(m_fBeatSpeed * m_fTimeMultiflier);
        m_pBArm->Tick(fTimeDelta);
    }
    if (m_bFArm)
    {
        m_pFArm->Set_AnimSpeed(m_fBeatSpeed * m_fTimeMultiflier);
        m_pFArm->Tick(fTimeDelta);
    }
    if(m_bSArm)
    {
        m_pSArmL->Set_AnimSpeed(m_fBeatSpeed * m_fTimeMultiflier);
        m_pSArmL->Tick(fTimeDelta);
        m_pSArmR->Set_AnimSpeed(m_fBeatSpeed * m_fTimeMultiflier);
        m_pSArmR->Tick(fTimeDelta);
    }
    m_pAfterImage->Tick(fTimeDelta);
}

void CMonster_Kale::LateTick(_float fTimeDelta)
{
    if (!m_bIsInGame)
        return;
    auto pGI = GAMEINSTANCE;
    if (m_isAttackOn)
        pGI->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);
    pGI->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
    Calc_ViewDepth();
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    m_pSword->LateTick(fTimeDelta);
    for (_uint i = 0; i < 4; ++i)
    {
        m_ArrFunnel[i]->LateTick(fTimeDelta);
    }
    if(m_bBArm)
        m_pBArm->LateTick(fTimeDelta);
    if(m_bFArm)
        m_pFArm->LateTick(fTimeDelta);
    if(m_bSArm)
    {
        m_pSArmL->LateTick(fTimeDelta);
        m_pSArmR->LateTick(fTimeDelta);
    }
    m_pAfterImage->LateTick(fTimeDelta);
 
    if (m_eCurrState >= EVENT_THIRD_PHASE_END && m_eCurrState <= EVENT_THIRD_RP_SUCCESS)
    {
        m_pSword->Setup_Matrix();
    }

#ifndef _DEBUG
#ifdef PERFECTION_LOAD
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
}

HRESULT CMonster_Kale::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    _float4x4 matShakingWorld = m_pTransformCom->Get_WorldMatrix();
    if (m_fShakeTime > 0.f)
    {
        _float3 vResult = GAMEINSTANCE->Random_Float3({ -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f });
        _matrix matWorld = XMLoadFloat4x4(&matShakingWorld);
        matWorld.r[3] += XMVectorSetW(XMLoadFloat3(&vResult), 0) * m_fShakeMagnitude;
        XMStoreFloat4x4(&matShakingWorld, matWorld);
    }
    _float fViewDepth = Get_ViewDepth();
    _float fOutlineWidth = 0.8f;
    if (fViewDepth <= 10.f)
    {
        fOutlineWidth = 0.4f;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matShakingWorld)))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
    //   return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;
    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
            continue;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            continue;

        _bool bNoEmit = false;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
        {
            bNoEmit = true;
        }


        if (bNoEmit)
            m_pShaderCom->Begin(0);
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);


        m_pModelCom->Draw((_uint)i);
        if (i == 0)
            continue;
        m_pShaderCom->Bind_Float("g_OutlineWidth", fOutlineWidth);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        m_pModelCom->Draw((_uint)i);

    }

    // For Colliders
#ifdef _DEBUG
    m_pCollider_Hit->Render();
    m_pCollider_Attack->Render();
#endif

    return S_OK;
}

HRESULT CMonster_Kale::Render_Shadow()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_LightView_Matrix());
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_LightProj_Matrix());
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
    //   return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;
    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Shadow);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CMonster_Kale::Start_808Event(shared_ptr<C808_Playable>& p808)
{
    m_eCurrState = EVENT_808_RUN;
    Change_Animation(false);
    _float4x4 Mat;
    m_pModelCom->Play_Animation(0.001f, 0.001f, 0.f, Mat);
    _float4 vPos;
    //XMStoreFloat4(&vPos, (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("prop_01")) * m_pTransformCom->Get_WorldMatrix_XMMat()).r[3] + XMVectorSet(2.f,0.f,0.f,0.f));
    //vPos.w = 1.f;
    XMStoreFloat4(&vPos, m_pTransformCom->Get_Position());// +XMVectorSet(0.0f, 0.f, 0.f, 0.f));
    p808->Set_Position(vPos);
    m_pTransformCom->LookAt_Horizontal(XMLoadFloat4(&vPos) + XMVectorSet(-1.f, 0.f, 0.f, 0.f));
    p808->Get_Transform()->LookAt_Horizontal(XMLoadFloat4(&vPos) + m_pTransformCom->Get_NormLook());
    m_p808 = p808;

    PLAYERMANAGER->Get_Player()->Get_Player_Camera()->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
}

void CMonster_Kale::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack || (m_bBodySlam && pCaller == m_pCollider_Hit))
    {
        DamageInfo damageInfo{};
        damageInfo.fAmountDamage = 10.f;
        damageInfo.eAttackType = EAttackType::Base;
        damageInfo.pObjectOwner = shared_from_this();
        
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

void CMonster_Kale::TakeDamage(const DamageInfo& damageInfo)
{
    _float4x4 matResult = m_pTransformCom->Get_WorldMatrix();

    _vector vDir = XMVector4Normalize(m_pTransformCom->Get_Position() - XMLoadFloat4(&m_vPlayerPos));

    matResult._41 += XMVectorGetX(vDir) * 0.5f;
    matResult._43 += XMVectorGetZ(vDir) * 0.5f;

    if (m_iPhase == 1 && !m_pSArmL->IsBroken() && Is_Idle())//실드 암 반응 함수 만들기
    {
		
        EFFECTMANAGER->Call_Union_Effect("Chai_Parry", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
        m_pSArmL->React_To_Atk();
        m_pSArmR->React_To_Atk();
        return;
    }

    _float fDamage = damageInfo.fAmountDamage;
    if (m_iGroggyGauge > 0)
        fDamage *= 0.3f;
    
    _float fTargetHp = m_fCurHp - fDamage;

    EFFECTMANAGER->Call_Union_Effect("Big_HitEffect", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f,true);

    //if (m_iPhase == 0 && m_fCurHp >= m_fMaxHp * 0.5f && fTargetHp < m_fMaxHp * 0.5f)
    //{
    //    CBeat_Manager::Get_Instance()->Queue_Sequence(L"Kale_Verse2");
    //}

    auto pPM = PLAYERMANAGER;
    pPM->Add_PlayerRankScore(fDamage / 1.5f);
    pPM->Add_PlayerScore((_int)fDamage * 10.f);
    CCombat_Manager::Get_Instance()->Add_Combo();
    m_fCurHp = fTargetHp;
    if (m_fCurHp <= 0.f || (m_iPhase == 1 && (!m_bBArm && !m_bFArm && !m_bSArm)))
    {
        m_iGroggyGauge = m_iMaxGroggyGauge;
        _int waveNum = CCombat_Manager::Get_Instance()->Get_Wave_Num();
        list <shared_ptr<CGameObject>> enemyList = CCombat_Manager::Get_Instance()->Get_Enemy_List();
        if (enemyList.size() == 1 && waveNum == 0)
        {
            //pPM->Bullet_Time(2.f, 0.2f, shared_from_this());
            //pPM->Bullet_Time(2.f, 0.2f, pPM->Get_Player());
            //static_pointer_cast<CChai>(pPM->Get_Player())->Zoom_In(2.f, 0.5f);
            pPM->Get_Player()->Get_Player_Camera()->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        }
        //++m_iPhase;
        m_fCurHp = m_fMaxHp;
        if (m_iPhase == 0)//TODO 나중에 지우기
        {
            m_pTransformCom->Set_WorldMatrix(Identity);
            m_eCurrState = EVENT_FIRST_PHASE_END;
            CBeat_Manager::Get_Instance()->Queue_Sequence(L"Kale_PreChor2");
            CBeat_Manager::Get_Instance()->Start_Sequence();
            CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        }
        if (m_iPhase == 1)//TODO 나중에 지우기
        {
            m_pModelCom = m_pModelNoCoatCom;
            m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
            m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
            m_ArrMat_Funnel[0] = m_pModelCom->Get_BoneMatrixPtr("prop_03");
            m_ArrMat_Funnel[1] = m_pModelCom->Get_BoneMatrixPtr("prop_04");
            m_ArrMat_Funnel[2] = m_pModelCom->Get_BoneMatrixPtr("prop_05");
            m_ArrMat_Funnel[3] = m_pModelCom->Get_BoneMatrixPtr("prop_06");
            m_pSword->Yellow(true);
            m_pSword->ReAttach(m_pMat_RightHand);
            for (_uint i = 0; i < 4; ++i)
            {
                m_ArrFunnel[i]->ReAttach(m_ArrMat_Funnel[i]);
            }
            CBeat_Manager::Get_Instance()->Queue_Sequence(L"Kale_Bridge");
            pPM->Get_Player()->Get_Player_Camera()->End_Following_Bone();
            ++m_iPhase;
            m_pAfterImage->Set_Color({0.8f,0.4f,0.f,0.5f});
            auto pUi = static_pointer_cast<CUi_KaleSystem>(CUi_Mgr::Get_Instance()->Find_UI(CUi_KaleSystem::ObjID));
            if (pUi)
            {
                pUi->Set_Phase3();
            }
            CMapManager::Get_Instance()->Boss_ChangePhase(2);
            To_Idle();
            CCine_Manager::Get_Instance()->StartCine("CS1220");
        }
        else if (m_iPhase == 2)
        {
            //#Jeong
            //shared_ptr<CKaleBoss_ObjSystem> pInstance = static_pointer_cast
            //    <CKaleBoss_ObjSystem>(CGimmickManager::Get_Instance()->Get_GimmckObj
            //    (string(CKaleBoss_ObjSystem::ObjID.begin(), CKaleBoss_ObjSystem::ObjID.end())));
            //pInstance->Set_Move(false);
            m_eCurrState = EVENT_THIRD_PHASE_END;
            CBeat_Manager::Get_Instance()->Queue_Sequence(L"Kale_Outro");
            CBeat_Manager::Get_Instance()->Start_Sequence();
        }
        //m_eCurrState = DMG_AIR_THROW_START;
        m_pCCT->Enable_Pseudo_Physics();
        m_pCCT->Enable_Gravity();
        //_vector vDir = XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_Position();
        //m_pCCT->Add_Velocity(XMVector3Normalize(vDir) * -20.f);
        LookPlayerPos();
        m_isAttackOn = false;
        Change_Animation(false);
        return;
    }

    m_fShakeTime = 0.3f;
    m_fShakeMagnitude = 0.08f;

    if (m_eCurrState == B_ARM_LOOP || m_eCurrState == S_ARM_LOOP || m_eCurrState == F_ARM_LOOP)
    {
        if (m_eCurrState == B_ARM_LOOP)
        {
            if (damageInfo.eAttackType == EAttackType::Peppermint)
            {
                m_eCurrState = B_ARM_GROGGY;
                Change_Animation(false);
                m_pBArm->BarrierOff();
                return;
            }
        }
        else if (m_eCurrState == F_ARM_LOOP)
        {
            if (damageInfo.eAttackType == EAttackType::Korsica)//TODO: 코르시카로 바꾸기
            {
                m_eCurrState = F_ARM_GROGGY;
                Change_Animation(false);
                m_pFArm->Quite_Flame();
                return;
            }
        }
        else
        {
            if (damageInfo.eAttackType == EAttackType::Macaron)//TODO: 마카롱으로 바꾸기
            {
                m_eCurrState = S_ARM_GROGGY;
                Change_Animation(false);
                return;
            }
        }
    }
    if (m_iGroggyGauge <= 0)
    {
        return;
    }
 
    switch (damageInfo.eAttackType)
    {
    case Engine::EAttackType::Base:
        m_iGroggyGauge -= 1;
        break;
    case Engine::EAttackType::Hard:
        m_iGroggyGauge -= 3;
        break;
    case Engine::EAttackType::Air:
        m_iGroggyGauge -= 2;
        break;
    case Engine::EAttackType::Special:
    {
        Make_PickUp_Object(5, CPickUpItem::PICKUP::BATTERY);
        Make_PickUp_Object(2, CPickUpItem::PICKUP::HP);
        m_iGroggyGauge -= 5;
    }
        break;
    case Engine::EAttackType::SP:
        m_iGroggyGauge -= 5;
        break;
    case Engine::EAttackType::ELECTRIC:
        break;
    case Engine::EAttackType::Parry:
        m_iGroggyGauge -= 3;
        break;
    case Engine::EAttackType::Peppermint:
        break;
    case Engine::EAttackType::Macaron:
        m_iGroggyGauge -= 2;
        break;
    case Engine::EAttackType::Korsica:
        m_iGroggyGauge -= 3;
        break;
    default:
        break;
    }
    ++m_iHitStreak;
    if (m_iGroggyGauge <= 0)
    {
        m_iIdleBeat = 0;
        if (damageInfo.eAttackType == EAttackType::Special)
        {
            //던져지기
            m_eCurrState = DMG_AIR_THROW_START;
            m_pCCT->Enable_Pseudo_Physics();
            m_pCCT->Enable_Gravity();
            _vector vDir = XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_Position();
            m_pCCT->Add_Velocity(XMVector3Normalize(vDir) * -20.f);
            LookPlayerPos();
        }
        else if (damageInfo.eAttackType == EAttackType::Air)
        {
            //뜨기
            m_eCurrState = DMG_AIR_START;
            m_pCCT->Disable_Pseudo_Physics();
            m_pCCT->Disable_Gravity();
            m_isAirState = true;
        }
        else
        {
            //그냥 그로기
            m_eCurrState = DMG_GROGGY_START;
            m_pCCT->Enable_Pseudo_Physics();
            m_pCCT->Enable_Gravity();
        }
        m_isAttackOn = false;
        Change_Animation(false);
        //m_pModelCom->Set_AnimIndex(m_strAnimTag[m_eCurrState], false);
        //m_pSword->Change_Animation(m_strAnimTag[m_eCurrState]);
        m_iGroggyGauge = 0;
        m_iGroggyDuration = 16;
    }
    else if (m_iHitStreak >= 4 && Is_Idle())
    {
        if (m_pGameInstance.lock()->Random_Bool())
        {
            m_eNextState = ATK_COUNTER_45R;
            Melee_Start();
            m_eNextState = (STATE)((_uint)ATK_45L_60R + (_uint)m_pGameInstance.lock()->Random_Int(0, 13));
        }
        else
        {
            m_eCurrState = DMG_GUARD_ON;
            Change_Animation(false);
            //m_pModelCom->Set_AnimIndex(m_strAnimTag[m_eCurrState], false);
            //m_pSword->Change_Animation(m_strAnimTag[m_eCurrState]);
        }
        m_isAttackOn = false;
        m_iHitStreak = 0;
    }

}

HRESULT CMonster_Kale::Ready_PartObjects()
{
    CKale_Sword::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pMat_RightHand;
    WeaponDesc.pParentTransform = m_pTransformCom;
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;
    //WeaponDesc.iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();

    m_pSword = static_pointer_cast<CKale_Sword>(GAMEINSTANCE->Clone_Object(CKale_Sword::ObjID, &WeaponDesc));
    if (nullptr == m_pSword)
        return E_FAIL;


    m_ArrMat_Funnel[0] = m_pModelCom->Get_BoneMatrixPtr("prop_03");
    m_ArrMat_Funnel[1] = m_pModelCom->Get_BoneMatrixPtr("prop_04");
    m_ArrMat_Funnel[2] = m_pModelCom->Get_BoneMatrixPtr("prop_05");
    m_ArrMat_Funnel[3] = m_pModelCom->Get_BoneMatrixPtr("prop_06");

    CKale_Funnel::WEAPON_DESC   FunnelDesc{};
    
    FunnelDesc.pParentTransform = m_pTransformCom;
    FunnelDesc.fSpeedPerSec = 0.f;
    FunnelDesc.fRotationPerSec = 0.f;
    for (_int i = 0; i < 4; ++i)
    {
        FunnelDesc.iNumber = i + 1;
        FunnelDesc.pBoneMatrix = m_ArrMat_Funnel[i];

        m_ArrFunnel[i] = static_pointer_cast<CKale_Funnel>(GAMEINSTANCE->Clone_Object(CKale_Funnel::ObjID, &FunnelDesc));

    }
    m_ArrMat_BackArm[0] = m_pModelCom->Get_BoneMatrixPtr("backarm_attach_down_L");
    CKale_BArm::WEAPON_DESC		BArmDesc{};
    BArmDesc.pBoneMatrix = m_ArrMat_BackArm[0];
    BArmDesc.pParentTransform = m_pTransformCom;
    BArmDesc.fSpeedPerSec = 0.f;
    BArmDesc.fRotationPerSec = 0.f;
    m_pBArm = static_pointer_cast<CKale_BArm>(GAMEINSTANCE->Clone_Object(CKale_BArm::ObjID, &BArmDesc));
    assert(m_pBArm);
    m_ArrMat_BackArm[1] = m_pModelCom->Get_BoneMatrixPtr("backarm_attach_down_R");
    CKale_FArm::WEAPON_DESC		FArmDesc{};
    FArmDesc.pBoneMatrix = m_ArrMat_BackArm[1];
    FArmDesc.pParentTransform = m_pTransformCom;
    FArmDesc.fSpeedPerSec = 0.f;
    FArmDesc.fRotationPerSec = 0.f;
    m_pFArm = static_pointer_cast<CKale_FArm>(GAMEINSTANCE->Clone_Object(CKale_FArm::ObjID, &FArmDesc));
    assert(m_pFArm);
    CKale_SArm::WEAPON_DESC		SArmDesc{};
    m_ArrMat_BackArm[2] = m_pModelCom->Get_BoneMatrixPtr("backarm_attach_up_R");
    SArmDesc.pBoneMatrix = m_ArrMat_BackArm[2];
    SArmDesc.pParentTransform = m_pTransformCom;
    SArmDesc.fSpeedPerSec = 0.f;
    SArmDesc.fRotationPerSec = 0.f;
    SArmDesc.bRight = true;
    m_pSArmR = static_pointer_cast<CKale_SArm>(GAMEINSTANCE->Clone_Object(CKale_SArm::ObjID, &SArmDesc));
    assert(m_pSArmR);
    m_ArrMat_BackArm[3] = m_pModelCom->Get_BoneMatrixPtr("backarm_attach_up_L");
    SArmDesc.pBoneMatrix = m_ArrMat_BackArm[3];
    SArmDesc.pParentTransform = m_pTransformCom;
    SArmDesc.fSpeedPerSec = 0.f;
    SArmDesc.fRotationPerSec = 0.f;
    SArmDesc.bRight = false;
    m_pSArmL = static_pointer_cast<CKale_SArm>(GAMEINSTANCE->Clone_Object(CKale_SArm::ObjID, &SArmDesc));
    assert(m_pSArmL);


    return S_OK;
}

HRESULT CMonster_Kale::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    // Init Model and Matrices
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(CGame_Manager::Get_Instance()->Get_Current_Level(), TEXT("Model_Monster_Kale"), TEXT("Com_Model")));
    m_pModelCoatCom = m_pModelCom;
    m_pModelNoCoatCom = static_pointer_cast<CModel>(__super::Add_Component(CGame_Manager::Get_Instance()->Get_Current_Level(), TEXT("Model_Monster_Kale_NoCoat"), TEXT("Com_ModelNoCoat")));
    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
    m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");

    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 1.0f;
    cctDesc.radius = 0.1f;
    cctDesc.stepOffset = 0.1f;
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));

    {
        CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
        sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
        sphereDesc.fRadius = 1.f;
        m_pCollider_Hit =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
        m_pCollider_Hit->Set_Owner(shared_from_this());
        m_pCollider_Hit->Set_ColliderTag(TEXT("Kale_Hit"));
    }
    {
        CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
        obbDesc.vCenter = _float3{ 0.6f, 0.f, 0.0f };
        obbDesc.vExtents = _float3{ 1.3f, 0.75f, 0.75f };
        obbDesc.vRotation = _float3{ 0.f, 0.f, 0.f };
        m_pCollider_Attack =
            dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));
        m_pCollider_Attack->Set_Owner(shared_from_this());
        m_pCollider_Attack->Set_ColliderTag(TEXT("Kale_Attack"));
    }

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
    return S_OK;
}

void CMonster_Kale::PrepareCallbacks()
{
    // For Attack motions
    //m_pModelCom->SetAnimCallback("em0100_sword-a_000", 0, [&]() { m_isAttackOn = true; });
    //m_pModelCom->SetAnimCallback("em0100_sword-a_000", 1, [&]() { m_isAttackOn = false; });
    //m_pModelCom->SetLastAnimCallback("em0100_sword-a_000", [&]() { m_isAttackOn = false; });
    //m_pModelCom->SetAnimCallback("em0100_sword-a_001", 0, [&]() { m_isAttackOn = true; });
    //m_pModelCom->SetAnimCallback("em0100_sword-a_001", 1, [&]() { m_isAttackOn = false; });
    //m_pModelCom->SetLastAnimCallback("em0100_sword-a_001", [&]() { m_isAttackOn = false; });

    //// Dead
    //m_pModelCom->SetAnimCallback("em0100_dmg-die_100", 0, [&]() { MonsterDead(); });
    //m_pModelCom->SetAnimCallback("em0100_dmg-die_010", 0, [&]() { MonsterDead(); });

    //m_pModelCom->SetAnimCallback("em0100_dmg-mid_000", 0, [&]() { if (IsDeadState()) MonsterDead(); });
    //m_pModelCom->SetAnimCallback("em0100_dmg-mid_010", 0, [&]() { if (IsDeadState()) MonsterDead(); });
    //m_pModelCom->SetAnimCallback("em0100_dmg-mid_020", 0, [&]() { if (IsDeadState()) MonsterDead(); });
    //m_pModelCom->SetAnimCallback("em0100_dmg-mid_600", 0, [&]() { if (IsDeadState()) MonsterDead(); });
}

void CMonster_Kale::Progress(_float fTimeDelta)
{
    switch (m_eCurrState)
    {
    case Client::CMonster_Kale::ATK_45L_60R:
        Look_At_Atk_Timing();
        break;
    case Client::CMonster_Kale::ATK_LUNGE_75L:
        Look_At_Atk_Timing();
        if (m_fTrackFrame >= 57.5f && m_fTrackFrame <= 62.5f)
        {
            m_bBodySlam = true;
        }

        if (m_fTrackFrame >= 75.f && !m_bCallBack)
        {
            m_bCallBack = true;
            if (m_bFArm)
            {
                //Change_Animation(false);
                m_pFArm->Change_Animation("em7520_atk-arm_000");
            }
        }
        break;
    case Client::CMonster_Kale::ATK_LIGHTNING:
        Lightning(fTimeDelta);
        break;
    case Client::CMonster_Kale::ATK_JUMP_75D:
        Look_At_Atk_Timing();
        break;
    case Client::CMonster_Kale::ATK_SLASH_60L:
        Look_At_Atk_Timing();
        break;
    case Client::CMonster_Kale::ATK_75L_90R_105L:
        Look_At_Atk_Timing();
        break;
    case Client::CMonster_Kale::ATK_DASH_60L_75R_90L_130R_140L_165U:
        Look_At_Atk_Timing();
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_120L:
    {
        if (m_fTrackFrame >= 105.f && m_iFireWallCount == 0)
        {
            ++m_iFireWallCount;
            FireWall(3, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV4 * 0.5f, 0.f), XMVectorSet(1.5f, 0.f, 1.5f, 0.f)));
        }
        else if (m_fTrackFrame >= 120.f && m_iFireWallCount == 1)
        {
            ++m_iFireWallCount;
            FireWall(3, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f, -XM_PIDIV4 * 0.5f, 0.f), XMVectorSet(-1.5f, 0.f, 1.5f, 0.f)));
        }
    }
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R://{ 45.f, 105.f, 165.f };//4
    {
        if (m_fTrackFrame >= 45.f && m_iFireWallCount == 0)
        {
            ++m_iFireWallCount;
            FireWall(4, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f,0.f, 0.f), XMVectorSet(0.0f, 0.f, 1.5f, 0.f)));
        }
    }
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R_67L://{ 45.f, 62.5f, 105.f, 165.f };//4
    {
        if (m_fTrackFrame >= 45.f && m_iFireWallCount == 0)
        {
            ++m_iFireWallCount;
            FireWall(4, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f, -XM_PIDIV4 * 0.5f, 0.f), XMVectorSet(-1.5f, 0.f, 1.5f, 0.f)));
        }
        else if (m_fTrackFrame >= 62.5f && m_iFireWallCount == 1)
        {
            ++m_iFireWallCount;
            FireWall(4, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV4 * 0.5f, 0.f), XMVectorSet(1.5f, 0.f, 1.5f, 0.f)));
        }
    }
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R_67L_82R:// { 45.f, 62.5f, 75.0f, 105.f, 165.f };//4*/
    {
        if (m_fTrackFrame >= 45.f && m_iFireWallCount == 0)
        {
            ++m_iFireWallCount;
            FireWall(4, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f, -XM_PIDIV4 * 0.5f, 0.f), XMVectorSet(-1.5f, 0.f, 1.5f, 0.f)));
        }
        else if (m_fTrackFrame >= 62.5f && m_iFireWallCount == 1)
        {
            ++m_iFireWallCount;
            FireWall(4, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV4 * 0.5f, 0.f), XMVectorSet(1.5f, 0.f, 1.5f, 0.f)));
        }
        else if (m_fTrackFrame >= 75.0f && m_iFireWallCount == 2)
        {
            ++m_iFireWallCount;
            FireWall(4, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
                XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f), XMVectorSet(0.0f, 0.f, 1.5f, 0.f)));
        }
    }
        break;
    case Client::CMonster_Kale::ATK_120R_150L_165R:
        Look_At_Atk_Timing();
        break;
    case Client::CMonster_Kale::ATK_LUNGE_45L_72R:
        Look_At_Atk_Timing();
        break;
    case ATK_ATTACH_ARM:
    {
        //m_pFArm->Atk_On();
    }
    break;
    case Client::CMonster_Kale::ATK_FUNNEL_105:
    {
        Funnel_Control();
        Look_At_Atk_Timing(false);
    }
        break;
    case Client::CMonster_Kale::ATK_COUNTER_45R:
        Look_At_Atk_Timing();
        break;
    case Client::CMonster_Kale::B_ARM_START:
    {
        Generate_Talk(1);
    }
        break;
    case Client::CMonster_Kale::B_ARM_LOOP:
        m_pTransformCom->LookAt_Horizontal_With_Speed(XMLoadFloat4(&m_vPlayerPos), fTimeDelta);
        break;
    case Client::CMonster_Kale::B_ARM_SHOOT_30_60_90:
    {
        if (m_iFireWallCount < (_uint)m_AnimFlag[m_eCurrState].size()  && m_fTrackFrame >= m_AnimFlag[m_eCurrState][m_iFireWallCount])
        {
            ++m_iFireWallCount;
            m_pBArm->LaserOn();
        }

    }
        break;
    case Client::CMonster_Kale::B_ARM_GROGGY:
        m_pBArm->Collision_On();
        break;
    case Client::CMonster_Kale::B_ARM_GROGGY_LOOP:
    {
        if (m_bBeatChanged)
        {
            --m_iGroggyDuration;
            if (m_iGroggyDuration <= 0)
            {
                m_eCurrState = B_ARM_RECOVER;
                Change_Animation(false);
            }
        }

        m_pBArm->Collision_On();
        if (m_pBArm->IsBroken())
        {
            EFFECTMANAGER->Call_Fixed_Union_Effect("Enemy_Boom", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
            m_eCurrState = B_ARM_DESTROY;
            Change_Animation(false);
        }
    }
        break;
    case Client::CMonster_Kale::B_ARM_RECOVER:
        break;
    case Client::CMonster_Kale::B_ARM_DESTROY:
        break;
    case Client::CMonster_Kale::F_ARM_START:
        Generate_Talk(3);
        break;
    case Client::CMonster_Kale::F_ARM_LOOP:
        m_pTransformCom->LookAt_Horizontal_With_Speed(XMLoadFloat4(&m_vPlayerPos), fTimeDelta);
        break;
    case Client::CMonster_Kale::F_ARM_SHOOT_30TO75:
        break;
    case Client::CMonster_Kale::F_ARM_GROGGY:
        m_pFArm->Collision_On();
        break;
    case Client::CMonster_Kale::F_ARM_GROGGY_LOOP:
        if (m_bBeatChanged)
        {
            --m_iGroggyDuration;
            if (m_iGroggyDuration <= 0)
            {
                m_eCurrState = F_ARM_RECOVER;
                Change_Animation(false);
            }
        }

        m_pFArm->Collision_On();
        if (m_pFArm->IsBroken())
        {
            EFFECTMANAGER->Call_Fixed_Union_Effect("Enemy_Boom", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
            m_eCurrState = F_ARM_DESTROY;
            Change_Animation(false);
        }
        break;
    case Client::CMonster_Kale::F_ARM_RECOVER:
        break;
    case Client::CMonster_Kale::F_ARM_DESTROY:
        break;
    case Client::CMonster_Kale::S_ARM_START_90:
    {
        if (m_fTrackFrame >= 87.5f && m_fTrackFrame <= 92.5f)
        {
            m_pSArmL->Atk_On();
            m_pSArmR->Atk_On();
        }
    }
        break;
    case Client::CMonster_Kale::S_ARM_LOOP:
        Generate_Talk(2);
        break;
    case Client::CMonster_Kale::S_ARM_SHOOT:
        break;
    case Client::CMonster_Kale::S_ARM_GROGGY:
        m_pSArmL->Collision_On();
        m_pSArmR->Collision_On();
        break;
    case Client::CMonster_Kale::S_ARM_GROGGY_LOOP:
        if (m_bBeatChanged)
        {
            --m_iGroggyDuration;
            if (m_iGroggyDuration <= 0)
            {
                m_eCurrState = S_ARM_RECOVER;
                Change_Animation(false);
            }
        }
        m_pSArmL->Collision_On();
        m_pSArmR->Collision_On();
        if (m_pSArmL->IsBroken() || m_pSArmR->IsBroken())
        {
            EFFECTMANAGER->Call_Fixed_Union_Effect("Enemy_Boom", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, true);
            m_pSArmL->Break();
            m_pSArmR->Break();
            m_eCurrState = S_ARM_DESTROY;
            Change_Animation(false);
        }
        break;
    case Client::CMonster_Kale::S_ARM_RECOVER:
        break;
    case Client::CMonster_Kale::S_ARM_DESTROY:
        break;
    case Client::CMonster_Kale::DMG_AIR_START:
        break;
    case Client::CMonster_Kale::DMG_AIR_LOOP:
        Air_Loop(fTimeDelta);
        break;
    case Client::CMonster_Kale::DMG_AIR_LAND:
        break;
    case Client::CMonster_Kale::DMG_AIR_LIEDOWN:
        Groggy_Recover();
        break;
    case Client::CMonster_Kale::DMG_AIR_RECOVER:
        break;
    case Client::CMonster_Kale::DMG_AIR_HIT:
        Air_Loop(fTimeDelta);
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_START:
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_FALL:
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_LAND:
        break;
    case Client::CMonster_Kale::DMG_GUARD_ON:
    {
        Idle(fTimeDelta, false);
    }
        break;
    case Client::CMonster_Kale::DMG_GUARD_LOOP:
    {
        Idle(fTimeDelta, false);
    }
        break;
    case Client::CMonster_Kale::DMG_GUARD_OFF:
    {
        Idle(fTimeDelta, false);
    }
        break;
    case Client::CMonster_Kale::DMG_KNOCKBACK:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_START:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_LOOP:
        Groggy_Recover();
        break;
    case Client::CMonster_Kale::DMG_GROGGY_RECOVER:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_HIT:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_HIT_KNOCKBACK:
        break;
    case Client::CMonster_Kale::EVENT_808_RUN:
    {
        m_pSword->Off_Render();
        if (m_fTrackFrame >= 195.f)
        {
            //바꾸기
            m_iPhase = 0;
            To_Idle();
            PLAYERMANAGER->Get_Player()->Get_Player_Camera()->End_Following_Bone();
            
            CBeat_Manager::Get_Instance()->Queue_Sequence(L"Kale_Chor1");
            CMapManager::Get_Instance()->Boss_ChangePhase(1);
            CCine_Manager::Get_Instance()->StartCine("CS1210");
        }
    }
        break;
    case Client::CMonster_Kale::EVENT_808_SUCCESS:
        break;
    case Client::CMonster_Kale::EVENT_808_FAIL:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_PHASE_END:
    {
        if (m_fTrackFrame >= 135.f)
        {
            Generate_Talk(0);
        }
        shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();
        pChai->Set_Player_Position(m_pTransformCom->Get_Position());
        pChai->Get_Transform()->Set_WorldMatrix(XMMatrixRotationY(XM_PI) * m_pTransformCom->Get_WorldMatrix_XMMat());

    }
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_READY:
    {
        Judge_RP();
    }
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_ATK:
    {
        Judge_RP();
        if (m_fTrackFrame >= 92.5f) // && 성공시
        {
            CBeat_Manager::Get_Instance()->Queue_Sequence(L"Kale_Chor2");
            m_eCurrState = EVENT_FIRST_RP_SUCCESS;
            Change_Animation(false);

        }
    }
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_FAIL:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_SUCCESS:
        break;
    case Client::CMonster_Kale::EVENT_THIRD_PHASE_END:
    {
        if (m_fTrackFrame >= 135.f)
        {
            Generate_Talk(4);
        }
        m_pCCT->Disable_Pseudo_Physics();
        m_pCCT->Disable_Gravity();
        m_pTransformCom->Set_Position(XMVectorSet(0.f, 0.f, 0.f, 1.f));
        m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();
        pChai->Set_Player_Position(m_pTransformCom->Get_Position());
        pChai->Get_Transform()->Set_WorldMatrix(XMMatrixRotationY(XM_PI) * m_pTransformCom->Get_WorldMatrix_XMMat());

    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_READY:
    {
        Judge_RP();
    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_ATK:
    {
        Judge_RP();
        if (m_fTrackFrame >= 83.f) // && 성공시
        {
            //CUi_Mgr::Get_Instance()->Ui_AllActive();
            PLAYERMANAGER->Get_Player()->Get_Player_Camera()->End_Following_Bone();
            m_eCurrState = EVENT_FINALE;
            CCine_Manager::Get_Instance()->StartCine("EV1250");
        }
    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_FAIL:
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_SUCCESS:
        break;
    case EVENT_FINALE:
    {

    }
    break;
    case Client::CMonster_Kale::IDLE_0:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::IDLE_BOARED:
        break;
    case Client::CMonster_Kale::IDLE_TAUNT:
        break;
    case Client::CMonster_Kale::MVNT_RUN_15:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_FORWARD:
    {
        Step(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_BACKWARD:
    {
        Step(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_LEFT:
    {
        Step(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_RIGHT:
    {
        Step(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_WALK_FORWARD:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_WALK_BACKWARD:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_WALK_LEFT:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_WALK_RIGHT:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_WALK_LEFT_BACK:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::MVNT_WALK_RIGHT_BACK:
    {
        Idle(fTimeDelta);
    }
        break;
    case Client::CMonster_Kale::STATE_END:
        break;
    default:
        break;
    }
}

void CMonster_Kale::Animation_End()
{
    switch (m_eCurrState)
    {
    case Client::CMonster_Kale::ATK_45L_60R:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_LUNGE_75L:
    {
        if (m_iPhase == 1 && m_bFArm)
        {
            m_eCurrState = ATK_ATTACH_ARM;
            Change_Animation(false);
        }
        else
        {
            To_Idle();
        }
    }
        break;
    case Client::CMonster_Kale::ATK_LIGHTNING:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_JUMP_75D:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_SLASH_60L:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_75L_90R_105L:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_DASH_60L_75R_90L_130R_140L_165U:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_120L:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R_67L:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R_67L_82R:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_120R_150L_165R:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_LUNGE_45L_72R:
    {
        To_Idle();
    }
        break;
    case ATK_ATTACH_ARM:
    {
        To_Idle();
    }
    break;
    case Client::CMonster_Kale::ATK_FUNNEL_105:
        To_Idle();
        break;
    case Client::CMonster_Kale::ATK_COUNTER_45R:
        To_Idle();
        break;
    case Client::CMonster_Kale::B_ARM_START:
    {
        m_eCurrState = B_ARM_LOOP;
        Change_Animation(false);
        m_pBArm->BarrierOn();
    }
        break;
    case Client::CMonster_Kale::B_ARM_LOOP:
    {
        m_eCurrState = B_ARM_SHOOT_30_60_90;
        Change_Animation(false);

    }
        break;
    case Client::CMonster_Kale::B_ARM_SHOOT_30_60_90:
        To_Idle();
        break;
    case Client::CMonster_Kale::B_ARM_GROGGY:
    {
        m_iGroggyDuration = 8;
        m_eCurrState = B_ARM_GROGGY_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::B_ARM_GROGGY_LOOP:
    {
        m_eCurrState = B_ARM_RECOVER;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::B_ARM_RECOVER:
        To_Idle();
        break;
    case Client::CMonster_Kale::B_ARM_DESTROY:
        m_bBArm = false;
        To_Idle();
        break;
    case Client::CMonster_Kale::F_ARM_START:
    {
        m_eCurrState = F_ARM_LOOP;
        Change_Animation(false);
        _float fTrackFrame = m_pModelCom->Get_Duration();
        m_pFArm->Hold_Flame(fTrackFrame / 15.f * BEATMANAGER->Get_Beat());
    }
        break;
    case Client::CMonster_Kale::F_ARM_LOOP:
    {
        m_eCurrState = F_ARM_SHOOT_30TO75;
        Change_Animation(false);
        _float fTrackFrame =  m_pModelCom->Get_Duration();
        m_pFArm->Throw_Flame(fTrackFrame / 15.f * BEATMANAGER->Get_Beat());
    }
        break;
    case Client::CMonster_Kale::F_ARM_SHOOT_30TO75:
        To_Idle();
        break;
    case Client::CMonster_Kale::F_ARM_GROGGY:
    {
        m_iGroggyDuration = 8;
        m_eCurrState = F_ARM_GROGGY_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::F_ARM_GROGGY_LOOP:
    {
        m_eCurrState = F_ARM_RECOVER;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::F_ARM_RECOVER:
        To_Idle();
        break;
    case Client::CMonster_Kale::F_ARM_DESTROY:
        m_bFArm = false;
        To_Idle();
        break;
    case Client::CMonster_Kale::S_ARM_START_90:
    {
        m_eCurrState = S_ARM_LOOP;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::S_ARM_LOOP:
    {
        m_eCurrState = S_ARM_SHOOT;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::S_ARM_SHOOT:
    {
        To_Idle();
    }
        break;
    case Client::CMonster_Kale::S_ARM_GROGGY:
    {
        m_iGroggyDuration = 8;
        m_eCurrState = S_ARM_GROGGY_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::S_ARM_GROGGY_LOOP:
    {
        m_eCurrState = S_ARM_RECOVER;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::S_ARM_RECOVER:
        To_Idle();
        break;
    case Client::CMonster_Kale::S_ARM_DESTROY:
        m_bSArm = false;
        To_Idle();
        break;
    case Client::CMonster_Kale::DMG_AIR_START:
    {
        m_eCurrState = DMG_AIR_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::DMG_AIR_LOOP:
    {
        m_eCurrState = DMG_AIR_LAND;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::DMG_AIR_LAND:
    {
        m_eCurrState = DMG_AIR_LIEDOWN;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::DMG_AIR_LIEDOWN:
    {
        m_eCurrState = DMG_AIR_RECOVER;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::DMG_AIR_RECOVER:
        To_Idle();
        break;
    case Client::CMonster_Kale::DMG_AIR_HIT:
    {
        m_eCurrState = DMG_AIR_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_START:
    {
        m_eCurrState = DMG_AIR_THROW_FALL;
        Change_Animation(false);
        m_pCCT->Set_Velocity(g_XMZero);
    }
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_FALL:
    {
        m_eCurrState = DMG_AIR_THROW_LAND;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_LAND:
    {
        m_eCurrState = DMG_AIR_LIEDOWN;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::DMG_GUARD_ON:
    {
        m_eCurrState = DMG_GUARD_LOOP;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::DMG_GUARD_LOOP:
    {
        m_eCurrState = DMG_GUARD_OFF;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::DMG_GUARD_OFF:
    {
        m_eCurrState = IDLE_0;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::DMG_KNOCKBACK:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_START:
    {
        m_eCurrState = DMG_GROGGY_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::DMG_GROGGY_LOOP:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_RECOVER:
    {
        To_Idle();
    }
        break;
    case Client::CMonster_Kale::DMG_GROGGY_HIT:
    {
        m_eCurrState = DMG_GROGGY_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::DMG_GROGGY_HIT_KNOCKBACK:
    {
        m_eCurrState = DMG_GROGGY_LOOP;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::EVENT_808_RUN:
        break;
    case Client::CMonster_Kale::EVENT_808_SUCCESS:
        break;
    case Client::CMonster_Kale::EVENT_808_FAIL:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_PHASE_END:
    {
        Setting_RP();
        m_eCurrState = EVENT_FIRST_RP_READY;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_READY:
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
        m_eCurrState = EVENT_FIRST_RP_ATK;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_ATK:
    {
        m_eCurrState = EVENT_FIRST_RP_FAIL;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_FAIL:
    {
        Setting_RP();
        m_eCurrState = EVENT_FIRST_RP_READY;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_SUCCESS:
    {
        PLAYERMANAGER->Get_Player()->CallBack_Event("Kale_RP_End");
        //CUi_Mgr::Get_Instance()->Ui_AllActive(true);
        PLAYERMANAGER->Get_Player()->Get_Player_Camera()->End_Following_Bone();
        m_iPhase = 1;
        To_Idle();
        CCine_Manager::Get_Instance()->StartCine("CS1215");
    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_PHASE_END:
    {
        shared_ptr<CKaleBoss_ObjSystem> pInstance = static_pointer_cast
            <CKaleBoss_ObjSystem>(CGimmickManager::Get_Instance()->Get_GimmckObj
            (string(CKaleBoss_ObjSystem::ObjID.begin(), CKaleBoss_ObjSystem::ObjID.end())));
        pInstance->Set_Move(false);
        Setting_RP();
        m_eCurrState = EVENT_THIRD_RP_READY;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_READY:
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
        m_eCurrState = EVENT_THIRD_RP_ATK;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_ATK:
    {
        m_eCurrState = EVENT_THIRD_RP_FAIL;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_FAIL:
    {
        Setting_RP();
        m_eCurrState = EVENT_THIRD_RP_READY;
        Change_Animation(false);
    }
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_SUCCESS:
    {
        PLAYERMANAGER->Get_Player()->CallBack_Event("Kale_RP_End");
        CUi_Mgr::Get_Instance()->Ui_AllActive(true);
        PLAYERMANAGER->Get_Player()->Get_Player_Camera()->End_Following_Bone();
        //m_iPhase = 1;
        //To_Idle();
       
        m_FinaleSequencer->Sequence_Start(m_p808);
        m_eCurrState = EVENT_FINALE;
    }
        break;
    case Client::CMonster_Kale::IDLE_0:
        break;
    case Client::CMonster_Kale::IDLE_BOARED:
        break;
    case Client::CMonster_Kale::IDLE_TAUNT:
        break;
    case Client::CMonster_Kale::MVNT_RUN_15:
    {
        m_eCurrState = IDLE_0;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_FORWARD:
    {
        m_eCurrState = IDLE_0;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_BACKWARD:
    {
        m_eCurrState = IDLE_0;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_LEFT:
    {
        m_eCurrState = IDLE_0;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::MVNT_STEP_RIGHT:
    {
        m_eCurrState = IDLE_0;
        Change_Animation(true);
    }
        break;
    case Client::CMonster_Kale::MVNT_WALK_FORWARD:
        break;
    case Client::CMonster_Kale::MVNT_WALK_BACKWARD:
        break;
    case Client::CMonster_Kale::MVNT_WALK_LEFT:
        break;
    case Client::CMonster_Kale::MVNT_WALK_RIGHT:
        break;
    case Client::CMonster_Kale::MVNT_WALK_LEFT_BACK:
        break;
    case Client::CMonster_Kale::MVNT_WALK_RIGHT_BACK:
        break;
    case Client::CMonster_Kale::STATE_END:
        break;
    default:
        break;
    }
}

void CMonster_Kale::To_Idle()
{
    m_iFireWallCount = 0;
    m_bCallBack = false;
    m_isAirState = false;
    m_eCurrState = IDLE_0;
    Change_Animation(true);
    m_iIdleBeat = 0;
    m_iHitStreak = 0;
    _int iLackBeat = (_int)m_iPrevBeat % 8;
    if (iLackBeat < 3)
        m_iIdleDelay = 8 - iLackBeat;
    else
        m_iIdleDelay = 16 - iLackBeat;
    m_iIdleDelay -= 1;
    if (m_iPhase == 0 || m_iPhase == 2)
    {
        if (m_eNextState >= ATK_POWERWAVE_120L && m_eNextState <= ATK_POWERWAVE_45R_67L_82R)
            m_eNextState = ATK_POWERWAVE_45R_67L_82R;
        if (m_iPhase == 1 && m_eNextState == ATK_SLASH_60L)
        {
            m_eNextState = ATK_75L_90R_105L;
        }
        m_eNextState = (STATE)((_uint)m_eNextState + 1);
        //m_eNextState = ATK_FUNNEL_105;
        //if (m_iPhase == 2 && m_eNextState == ATK_SLASH_60L)
        //{
        //    m_eNextState = ATK_75L_90R_105L;
        //}
        if (m_eNextState == ATK_POWERWAVE_120L)
        {
            m_eNextState = (STATE)((_uint)m_eNextState + m_pGameInstance.lock()->Random_Int(0, 3));
        }
        if (m_eNextState > ATK_FUNNEL_105)
        {
            m_eNextState = ATK_45L_60R;
        }
        if (m_eNextState != ATK_FUNNEL_105)
        {
            m_iIdleFunnelShoot = m_pGameInstance.lock()->Random_Int(1, m_iIdleDelay - 4);
        }
    }
    if (m_iPhase == 1)
    {
        if ((m_eNextState == B_ARM_START || m_eNextState == F_ARM_START || m_eNextState == S_ARM_START_90) && m_bFArm && !m_pGameInstance.lock()->Random_Int(0, 1))
        {
            m_eNextState = ATK_LUNGE_75L;
        }
        else
        {
            if (m_eNextState == S_ARM_START_90)
            {
                if (!m_pBArm->IsBroken())
                    m_eNextState = B_ARM_START;
                else if (!m_pFArm->IsBroken())
                    m_eNextState = F_ARM_START;
                else
                    m_eNextState = S_ARM_START_90;
            }
            else if (m_eNextState == B_ARM_START)
            {
                if (!m_pFArm->IsBroken())
                    m_eNextState = F_ARM_START;
                else if (!m_pSArmL->IsBroken())
                    m_eNextState = S_ARM_START_90;
                else
                    m_eNextState = B_ARM_START;
            }
            else
            {
                if (!m_pSArmL->IsBroken())
                    m_eNextState = S_ARM_START_90;
                else if (!m_pBArm->IsBroken())
                    m_eNextState = B_ARM_START;
                else
                    m_eNextState = F_ARM_START;
            }

            

        }
    }
    if (m_iPhase == -1)
    {
        if (m_eNextState > ATK_POWERWAVE_45R_67L_82R)
        {
            m_eNextState = ATK_POWERWAVE_120L;
        }
    }
    
    switch (m_eNextState)
    {
    case Client::CMonster_Kale::ATK_45L_60R:
    {
        m_fMinRange = 2.f;
        m_fMaxRange = 4.f;
    }
        break;
    case Client::CMonster_Kale::ATK_LUNGE_75L:
    {
        m_fMinRange = 4.f;
        m_fMaxRange = 12.f;
    }
        break;
    case Client::CMonster_Kale::ATK_LIGHTNING:
    {
        m_fMinRange = 4.f;
        m_fMaxRange = 24.f;
    }
        break;
    case Client::CMonster_Kale::ATK_JUMP_75D:
    {
        m_fMinRange = 4.f;
        m_fMaxRange = 6.f;
    }
        break;
    case Client::CMonster_Kale::ATK_SLASH_60L:
    {
        m_fMinRange = 3.f;
        m_fMaxRange = 5.f;
    }
        break;
    case Client::CMonster_Kale::ATK_75L_90R_105L:
    {
        m_fMinRange = 3.f;
        m_fMaxRange = 5.f;
    }
        break;
    case Client::CMonster_Kale::ATK_DASH_60L_75R_90L_130R_140L_165U:
    {
        m_fMinRange = 4.f;
        m_fMaxRange = 8.f;
    }
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_120L:
    {
        m_fMinRange = 12.f;
        m_fMaxRange = 24.f;
    }
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R:
    {
        m_fMinRange = 12.f;
        m_fMaxRange = 24.f;
    }
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R_67L:
    {
        m_fMinRange = 12.f;
        m_fMaxRange = 24.f;
    }
        break;
    case Client::CMonster_Kale::ATK_POWERWAVE_45R_67L_82R:
    {
        m_fMinRange = 12.f;
        m_fMaxRange = 24.f;
    }
        break;
    case Client::CMonster_Kale::ATK_120R_150L_165R:
    {
        m_fMinRange = 2.f;
        m_fMaxRange = 4.f;
    }
        break;
    case Client::CMonster_Kale::ATK_LUNGE_45L_72R:
    {
        m_fMinRange = 4.f;
        m_fMaxRange = 8.f;
    }
        break;		
    case Client::CMonster_Kale::ATK_FUNNEL_105:
    {
        m_fMinRange = 16.f;
        m_fMaxRange = 24.f;
        m_iFunnelPattern = (_uint)m_pGameInstance.lock()->Random_Int(0, 2);
        auto pGI = m_pGameInstance.lock();
        for (_uint i = 0; i < 3; ++i)
        {
            _uint iSrc = m_ArrFunnelSequence[i];
            _uint iDestIdx = i + (_uint)pGI->Random_Int(0, 3 - i);
            _uint iDest = m_ArrFunnelSequence[iDestIdx];
            m_ArrFunnelSequence[i] = iDest;
            m_ArrFunnelSequence[iDestIdx] = iSrc;
        }
        m_iCurrFunnelToActivate = 0;
    }
        break;
    case Client::CMonster_Kale::ATK_COUNTER_45R:
        break;
    case Client::CMonster_Kale::B_ARM_START:
    {
        m_fMinRange = 8.f;
        m_fMaxRange = 16.f;
    }
        break;
    case Client::CMonster_Kale::B_ARM_LOOP:
        break;
    case Client::CMonster_Kale::B_ARM_SHOOT_30_60_90:
        break;
    case Client::CMonster_Kale::B_ARM_GROGGY:
        break;
    case Client::CMonster_Kale::B_ARM_GROGGY_LOOP:
        break;
    case Client::CMonster_Kale::B_ARM_RECOVER:
        break;
    case Client::CMonster_Kale::B_ARM_DESTROY:
        break;
    case Client::CMonster_Kale::F_ARM_START:
    {
        m_fMinRange = 6.f;
        m_fMaxRange = 12.f;
    }
        break;
    case Client::CMonster_Kale::F_ARM_LOOP:
        break;
    case Client::CMonster_Kale::F_ARM_SHOOT_30TO75:
        break;
    case Client::CMonster_Kale::F_ARM_GROGGY:
        break;
    case Client::CMonster_Kale::F_ARM_GROGGY_LOOP:
        break;
    case Client::CMonster_Kale::F_ARM_RECOVER:
        break;
    case Client::CMonster_Kale::F_ARM_DESTROY:
        break;
    case Client::CMonster_Kale::S_ARM_START_90:
    {
        m_fMinRange = 4.f;
        m_fMaxRange = 8.f;
    }
        break;
    case Client::CMonster_Kale::S_ARM_LOOP:
        break;
    case Client::CMonster_Kale::S_ARM_SHOOT:
        break;
    case Client::CMonster_Kale::S_ARM_GROGGY:
        break;
    case Client::CMonster_Kale::S_ARM_GROGGY_LOOP:
        break;
    case Client::CMonster_Kale::S_ARM_RECOVER:
        break;
    case Client::CMonster_Kale::S_ARM_DESTROY:
        break;
    case Client::CMonster_Kale::DMG_AIR_START:
        break;
    case Client::CMonster_Kale::DMG_AIR_LOOP:
        break;
    case Client::CMonster_Kale::DMG_AIR_LAND:
        break;
    case Client::CMonster_Kale::DMG_AIR_LIEDOWN:
        break;
    case Client::CMonster_Kale::DMG_AIR_RECOVER:
        break;
    case Client::CMonster_Kale::DMG_AIR_HIT:
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_START:
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_FALL:
        break;
    case Client::CMonster_Kale::DMG_AIR_THROW_LAND:
        break;
    case Client::CMonster_Kale::DMG_GUARD_ON:
        break;
    case Client::CMonster_Kale::DMG_GUARD_LOOP:
        break;
    case Client::CMonster_Kale::DMG_GUARD_OFF:
        break;
    case Client::CMonster_Kale::DMG_KNOCKBACK:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_START:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_LOOP:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_RECOVER:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_HIT:
        break;
    case Client::CMonster_Kale::DMG_GROGGY_HIT_KNOCKBACK:
        break;
    case Client::CMonster_Kale::EVENT_808_RUN:
        break;
    case Client::CMonster_Kale::EVENT_808_SUCCESS:
        break;
    case Client::CMonster_Kale::EVENT_808_FAIL:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_PHASE_END:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_READY:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_ATK:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_FAIL:
        break;
    case Client::CMonster_Kale::EVENT_FIRST_RP_SUCCESS:
        break;
    case Client::CMonster_Kale::EVENT_THIRD_PHASE_END:
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_READY:
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_ATK:
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_FAIL:
        break;
    case Client::CMonster_Kale::EVENT_THIRD_RP_SUCCESS:
        break;
    case Client::CMonster_Kale::IDLE_0:
        break;
    case Client::CMonster_Kale::IDLE_BOARED:
        break;
    case Client::CMonster_Kale::IDLE_TAUNT:
        break;
    case Client::CMonster_Kale::MVNT_RUN_15:
        break;
    case Client::CMonster_Kale::MVNT_STEP_FORWARD:
        break;
    case Client::CMonster_Kale::MVNT_STEP_BACKWARD:
        break;
    case Client::CMonster_Kale::MVNT_STEP_LEFT:
        break;
    case Client::CMonster_Kale::MVNT_STEP_RIGHT:
        break;
    case Client::CMonster_Kale::MVNT_WALK_FORWARD:
        break;
    case Client::CMonster_Kale::MVNT_WALK_BACKWARD:
        break;
    case Client::CMonster_Kale::MVNT_WALK_LEFT:
        break;
    case Client::CMonster_Kale::MVNT_WALK_RIGHT:
        break;
    case Client::CMonster_Kale::MVNT_WALK_LEFT_BACK:
        break;
    case Client::CMonster_Kale::MVNT_WALK_RIGHT_BACK:
        break;
    case Client::CMonster_Kale::STATE_END:
        break;
    default:
        break;
    }

    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Enable_Gravity();
    m_pSword->Change_Animation(m_strAnimTag[m_eCurrState]);
    if (m_iPhase == 1)
    {
        m_pBArm->Change_Animation(m_strAnimTag[m_eCurrState]);
    }
}

void CMonster_Kale::Beat_Move()
{
    if (m_iHitStreak > 0 && !(m_iPrevBeat % 4))
    {
        --m_iHitStreak;
        if(m_iGroggyGauge != 0 && m_iGroggyGauge < m_iMaxGroggyGauge)
            ++m_iGroggyGauge;
    }
    ++m_iIdleBeat;
    --m_iGroggyDuration;
    if (m_eCurrState == IDLE_0)
    {
       
    }
}

void CMonster_Kale::Idle(_float fTimeDelta, _bool bMovementControl)
{
    m_isAttackOn = false;
    m_pTransformCom->LookAt_Horizontal_With_Speed(XMLoadFloat4(&m_vPlayerPos), fTimeDelta * 2.5f);
    if(bMovementControl)
        Movement_Control(fTimeDelta);
    if (m_iIdleBeat == m_iIdleFunnelShoot && m_bBeatChanged)
    {
        m_ArrFunnel[m_pGameInstance.lock()->Random_Int(0, 3)]->Shoot(4);
    }
    if (m_iIdleBeat >= m_iIdleDelay)
    {
        Melee_Start();
    }
}

void CMonster_Kale::Melee_Start()
{
    m_pCCT->Disable_Gravity();
    m_pCCT->Disable_Pseudo_Physics();
    m_eCurrState = m_eNextState;
    //m_isAttackOn = true;
    Change_Animation(false);
}

void CMonster_Kale::Look_At_Atk_Timing(_bool bAtk)
{
    m_isAttackOn = false;
    for (_int i = 0; i < (_int)m_AnimFlag[m_eCurrState].size() - 1; ++i)
    {
        if (m_fTrackFrame < m_AnimFlag[m_eCurrState][0])
        {
            LookPlayerPos();
        }
        if ((m_fTrackFrame >= m_AnimFlag[m_eCurrState][i] - 2.f && m_fTrackFrame <= m_AnimFlag[m_eCurrState][i] + 2.f))
        {
            LookPlayerPos();
            if(bAtk)
                m_isAttackOn = true;
        }
    }
}

void CMonster_Kale::LookPlayerPos()
{
    m_pTransformCom->LookAt_Horizontal(XMLoadFloat4(&m_vPlayerPos));
}

void CMonster_Kale::Movement_Control(_float fTimeDelta)
{
    if (m_iPhase == -1 || (m_eCurrState >= MVNT_STEP_FORWARD && m_eCurrState <= MVNT_STEP_RIGHT) || m_eCurrState == MVNT_RUN_15)
        return;

    if (m_fPlayerDist < m_fMinRange * .5f)
    {
        if(m_iPhase == 2)
        {
            if (m_eCurrState != MVNT_STEP_BACKWARD && m_eCurrState != MVNT_STEP_RIGHT && m_eCurrState != MVNT_STEP_LEFT)
            {

                _int iRand = m_pGameInstance.lock()->Random_Int(0, 2);
                if (iRand == 0)
                {
                    m_eCurrState = MVNT_STEP_BACKWARD;
                }
                else if (iRand == 1)
                {
                    m_eCurrState = MVNT_STEP_RIGHT;
                }
                else
                {
                    m_eCurrState = MVNT_STEP_LEFT;
                }
                m_pModelCom->Set_AnimIndex(m_strAnimTag[m_eCurrState], false);
            }
        }
        else
        {
            m_eCurrState = MVNT_WALK_BACKWARD;
            m_pModelCom->Set_AnimIndexNonCancle(m_strAnimTag[m_eCurrState], true);
        }
    }
    else if (m_fPlayerDist < m_fMinRange)
    {
        m_eCurrState = MVNT_WALK_BACKWARD;
        m_pModelCom->Set_AnimIndexNonCancle(m_strAnimTag[m_eCurrState], true);
    }
    else if (m_fPlayerDist < m_fMaxRange)
    {
        if (m_eCurrState != MVNT_WALK_RIGHT_BACK && m_eCurrState != MVNT_WALK_LEFT_BACK)
        {
            _int iRand = m_pGameInstance.lock()->Random_Int(0, 1);
            if(iRand)
                m_eCurrState = MVNT_WALK_RIGHT_BACK;
            else
                m_eCurrState = MVNT_WALK_LEFT_BACK;
        }
        m_pModelCom->Set_AnimIndexNonCancle(m_strAnimTag[m_eCurrState], true);
    }
    else if(m_fPlayerDist < m_fMaxRange * 1.25f)
    {
        m_eCurrState = MVNT_WALK_FORWARD;
        m_pModelCom->Set_AnimIndexNonCancle(m_strAnimTag[m_eCurrState], true);
    }
    else
    {
        LookPlayerPos();
        m_eCurrState = MVNT_STEP_FORWARD;
        m_pModelCom->Set_AnimIndex(m_strAnimTag[m_eCurrState], false);
    }
}

void CMonster_Kale::Groggy_Recover()
{
    if (m_iGroggyDuration <= 0)
    {
        if (m_eCurrState == DMG_AIR_LIEDOWN)
        {
            m_eCurrState = DMG_AIR_RECOVER;
            Change_Animation(false);
        }
        else if (m_eCurrState == DMG_GROGGY_LOOP)
        {
            m_eCurrState = DMG_GROGGY_RECOVER;
            Change_Animation(false);
        }
        m_iGroggyGauge = m_iMaxGroggyGauge;
    }
}

void CMonster_Kale::Air_Loop(_float fTimeDelta)
{
    if (m_bBeatChanged)
        --m_iAirborneDutation;
    if (m_iAirborneDutation <= 0)
    {
        m_pCCT->Enable_Pseudo_Physics();
        m_pCCT->Enable_Gravity();
        m_isAirState = false;
    }
    PxControllerState pCS;
    m_pCCT->Get_Controller()->getState(pCS);
    if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        m_eCurrState = DMG_AIR_LAND;
        Change_Animation(false);
    }
}

_bool CMonster_Kale::Is_Idle() const
{
    return (m_eCurrState >= IDLE_0 && m_eCurrState <=IDLE_TAUNT) || 
            (m_eCurrState >= MVNT_RUN_15 && m_eCurrState <= MVNT_WALK_RIGHT_BACK);
}

void CMonster_Kale::Funnel_Control()
{
    if (m_iCurrFunnelToActivate >= 4)
        return;
    if (m_arrFunnelPattern[m_iFunnelPattern][m_iCurrFunnelToActivate] <= m_fTrackFrame - 30.f)
    {
        m_ArrFunnel[m_ArrFunnelSequence[m_iCurrFunnelToActivate]]->Shoot(4);
        ++m_iCurrFunnelToActivate;
    }
}

void CMonster_Kale::Create_AttackUi(_uint iParryCount, vector<_float>& fBeatGap, vector<_bool>& bParry, _float2 fDistance, _bool SecondPattern) const
{
    CUi_EnemySpecialAttack::Desc Desc;
    Desc.iParryCount = iParryCount;
    Desc.bParry = bParry;
    Desc.fBeatGap = fBeatGap;
    Desc.fDistance = fDistance;
    Desc.TargetTransform = m_pTransformCom;
    if(!SecondPattern)
    {
        Desc.RealAttackGapBeat = 9;
    }
    else
    {
        Desc.RealAttackGapBeat = 10;
    }
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_EnemySpecialAttack::ObjID,
        m_iLevelIndex, L"CUi", CUi_EnemySpecialAttack::ObjID, &Desc);
}

void CMonster_Kale::Setting_RP()
{
    m_AttackBeatVec.clear();
    m_AttackParryBeatVec.clear();
    m_iAttackReadyStartBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

    _bool Second = { false };
    if (m_iPhase == 0)
    {
        m_AttackBeatVec = { 1.f, 1.5f, 1.5f, 1.5f ,1.5f };
        m_AttackParryBeatVec = { true, true, false, true, true };
    }
    else if (m_iPhase == 2)
    {
        m_AttackBeatVec = { 2.f, 1.f, 1.f, 0.5f ,1.f, 1.f };
        m_AttackParryBeatVec = { true, false, true, true, false, true };
        Second = true;
    }

    CUi_Mgr::Get_Instance()->Ui_AllActive(false);

    Create_AttackUi((_uint)m_AttackBeatVec.size(), m_AttackBeatVec, m_AttackParryBeatVec, {200.f, 200.f}, Second);

    for (size_t i = 1; i < m_AttackBeatVec.size(); ++i)
    {
        m_AttackBeatVec[i] += m_AttackBeatVec[i - 1];
    }
    shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();
    pChai->Set_Player_Position(m_pTransformCom->Get_Position());
    pChai->Get_Transform()->Set_WorldMatrix(XMMatrixRotationY(XM_PI) * m_pTransformCom->Get_WorldMatrix_XMMat());
    pChai->CallBack_Event("Kale_RP");
    pChai->Start_AttackSequence(8.f, m_iAttackReadyStartBeat, m_AttackBeatVec, m_AttackParryBeatVec);
}

void CMonster_Kale::Judge_RP()
{
    _int CurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    _float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float BeatPercent = Timer / Beat;
    _float JudgeBeat = 0;

    shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();

    if ((m_eCurrState >= EVENT_FIRST_PHASE_END && m_eCurrState <= EVENT_FIRST_RP_SUCCESS) ||
        (m_eCurrState >= EVENT_THIRD_PHASE_END && m_eCurrState <= EVENT_THIRD_RP_SUCCESS))
    {
        JudgeBeat = CurBeat - (m_iAttackReadyStartBeat + 8.f);
    }

    if (m_AttackBeatVec.size() > 0)
    {
        if (m_AttackBeatVec[0] <= JudgeBeat + BeatPercent - 0.15f)
        {
            if (m_AttackParryBeatVec[0])
            {
                if (pChai->Get_ReactTimer() < 0.001f)
                {
                    pChai->Set_DamagedAnim();
                }
                else
                {
                    pChai->Set_ParryAnim();
                    m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Parry_GT");
                }

                pChai->Get_Player_Camera()->Shake(0.1f, 0.1f);
            }
            else
            {
                for (auto& pFunnel : m_ArrFunnel)
                {
                    pFunnel->Shoot(0);
                }
                if (pChai->Get_ReactTimer() < 0.001f)
                    pChai->Set_DamagedAnim();
                else
                    pChai->Set_EvadeAnim();

            }

            pChai->Set_ReactTimer(0.f);
            m_AttackBeatVec.erase(m_AttackBeatVec.begin());
            m_AttackParryBeatVec.erase(m_AttackParryBeatVec.begin());
        }
    }
}

void CMonster_Kale::FireWall(_int iDelay, _fmatrix Mat)
{
    auto pGI = m_pGameInstance.lock();
    _float4x4 EffectMat;
    XMStoreFloat4x4(&EffectMat, Mat * m_pTransformCom->Get_WorldMatrix_XMMat());
    pGI->Push_Object_From_Pool(CKale_FireWall::ObjID, pGI->Get_Current_LevelID(), L"Effect", &iDelay, &EffectMat);
}
void CMonster_Kale::Lightning(_float fTimeDelta)
{
    if (m_iFireWallCount > 2)
        return;
    if (m_iFireWallCount < (_uint)m_AnimFlag[m_eCurrState].size() &&  m_fTrackFrame >= m_AnimFlag[m_eCurrState][m_iFireWallCount] - 45.f)
    {
        ++m_iFireWallCount;
        BigSword();
    }
}
void CMonster_Kale::BigSword()
{
    auto pGI = m_pGameInstance.lock();
    auto pSword = pGI->Push_Object_From_Pool(CKale_BigSword::ObjID, pGI->Get_Current_LevelID(), L"Effect");  
	static_pointer_cast<CKale_BigSword>(pSword)->Set_Extend(m_iPhase == 2 ? true : false);
}
//CKale_BigSword::ObjID

void CMonster_Kale::Change_Animation(_bool bLoop)
{
    m_pModelCom->Set_AnimIndex(m_strAnimTag[m_eCurrState], bLoop);
    m_pSword->Change_Animation(m_strAnimTag[m_eCurrState]);
    if(m_iPhase == 1)
    {
        m_pBArm->Change_Animation(m_strAnimTag[m_eCurrState]);
        m_pFArm->Change_Animation(m_strAnimTag[m_eCurrState]);
        m_pSArmR->Change_Animation(m_strAnimTag[m_eCurrState]);
        m_pSArmL->Change_Animation(m_strAnimTag[m_eCurrState]);
    }
}

void CMonster_Kale::Step(_float fTimeDelta)
{
    m_fAfterImageCooldown += fTimeDelta;
    if(m_fAfterImageCooldown >= 0.05f)
    {
        m_pAfterImage->SetUp_Matrices(m_pTransformCom->Get_WorldMatrix());
        m_fAfterImageCooldown -= 0.05f;
    }
}

void CMonster_Kale::Generate_Talk(_int iNum)
{
    if (m_bTalk[iNum])
        return;
    m_bTalk[iNum] = true;
    auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
    if (pTalkSystem == nullptr)
    {
        CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
        TalkDesc.endCallback = [&]() {
            };

        pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_KALEBOSS, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
    }

    auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
    if (PRequiredTalkSystem != nullptr)
    {
        wstring Tag = L"VsKale_Talk_File_";
        Tag += to_wstring(iNum + 1);
        Tag += wstring(L".csv");
        PRequiredTalkSystem->LoadTalkFile(Tag);

        CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
    }
}

void CMonster_Kale::SetPos_Hard(_fvector vPos)
{
    m_pTransformCom->Set_Position(vPos);
}

void CMonster_Kale::MoveToPos(_fvector vTargetPos)
{
}

void CMonster_Kale::MonsterDead()
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMMatrixIdentity());
    //PlayEffect("Enemy_Boom", matResult, 1.f, true);

    Set_Dead();
    Make_PickUp_Object(5, CPickUpItem::PICKUP::GEAR);
    CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
}

_bool CMonster_Kale::IsAnimFinished()
{
    return m_pModelCom->Get_Finished();
}

void CMonster_Kale::SetAnim(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndexNonCancle(strAnimName, isLoop);
    m_pSword->Change_Animation(strAnimName);
    m_pBArm->Change_Animation(strAnimName);
    m_pFArm->Change_Animation(strAnimName);
    m_pSArmR->Change_Animation(strAnimName);
    m_pSArmL->Change_Animation(strAnimName);
}

void CMonster_Kale::SetAnimHard(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndex_NoInterpole(strAnimName, isLoop);
    m_pSword->Change_Animation(strAnimName);
    m_pBArm->Change_Animation(strAnimName);
    m_pFArm->Change_Animation(strAnimName);
    m_pSArmR->Change_Animation(strAnimName);
    m_pSArmL->Change_Animation(strAnimName);
}

void CMonster_Kale::GravityOn()
{
    m_pCCT->Enable_Gravity();
}
void CMonster_Kale::GravityOff()
{
    m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    m_pCCT->Disable_Gravity();
}

void CMonster_Kale::AirStateON()
{
    m_isAirState = true;
    GravityOff();
}
void CMonster_Kale::AirStateOFF()
{
    m_isAirState = false;
    GravityOn();
}

void CMonster_Kale::MoveStraight(_float fSpeed)
{
    _vector vCurLook = m_pTransformCom->Get_NormLook();
    m_pCCT->Add_Displacement(vCurLook * fSpeed);
}

void CMonster_Kale::MoveUp(_float fSpeed)
{
    _vector vUpVector = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    m_pCCT->Add_Displacement(vUpVector * fSpeed);
}

void CMonster_Kale::Set_Position(_matrix mMat)
{
    m_pCCT->Set_Position(mMat);
}

void CMonster_Kale::CCT_ON()
{
    m_isCCTWork = true;
    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Enable_Gravity();
    m_pCCT->Set_Gravity(40.f);
}

void CMonster_Kale::CCT_OFF()
{
    m_isCCTWork = false;
    m_pCCT->Disable_Pseudo_Physics();
    m_pCCT->Disable_Gravity();
}

void CMonster_Kale::SelectBeat()
{
    _float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;
    if(m_eCurrState == EVENT_FINALE)
        fTotalBeat = m_pModelCom->Get_Duration() / 20.f;

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

    m_fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
    if (m_eCurrState == S_ARM_LOOP || m_eCurrState == B_ARM_LOOP || m_eCurrState == F_ARM_LOOP)
    {
        m_fBeatSpeed *= 0.5f;
    }
}

void CMonster_Kale::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(&m_matCurrentPos));
    CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier * m_fBeatSpeed, isBillBoard);
}
void CMonster_Kale::PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard)
{
    CEffect_Manager::Get_Instance()->Call_Union_Effect(strEffect, &m_matCurrentPos, fmultiplier * m_fBeatSpeed, isBillBoard);
}

void CMonster_Kale::Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType)
{
    CGimmickBase::COMMONOBJ_DESC desc{};
    desc.fRotationPerSec = 0.f;
    desc.fSpeedPerSec = 0.f;

    for (size_t i = 0; i < iNumber; i++)
    {
        _float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
        _vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
        _float4x4 matResults{};
        XMStoreFloat4x4(&matResults, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));

        _float4x4 matResult{};
        XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matResults) * XMLoadFloat4x4(&m_matCurrentPos));

        XMStoreFloat4(&desc.vPosition, XMLoadFloat4x4(&matResult).r[3]);
        XMStoreFloat4x4(&desc.worldMatrix, XMLoadFloat4x4(&matResult));

        auto pPickup = static_pointer_cast<CPickUpItem>(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), L"PickUp", CPickUpItem::ObjID, &desc));
        CPickUpItem::PICKUP_DESC PUDESC;
        PUDESC.eSize = (CPickUpItem::SIZE)(rand() % 2);
        PUDESC.eType = pickupType;
        pPickup->Model_Change(PUDESC);
        pPickup->Execute();
    }
}

shared_ptr<CMonster_Kale> CMonster_Kale::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_Kale
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Kale(pDevice, pContext) {}
    };

    shared_ptr<CMonster_Kale> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Kale"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_Kale::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_Kale
    {
        MakeSharedEnabler(const CMonster_Kale& rhs) : CMonster_Kale(rhs) {}
    };

    shared_ptr<CMonster_Kale> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Kale"));
        assert(false);
    }

    return pInstance;
}

void CMonster_Kale::Free()
{
}
