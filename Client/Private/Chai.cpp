#include "Chai.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "CharacterController.h"
#include "Player_Camera.h"
#include "Basic_Guitar.h"

#include "Cine_Manager.h"
#include "Effect_Manager.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "CombatManager.h"
#include "Beat_Manager.h"
#include "CUi_Mgr.h"
#include "CUi_BeatMarkerSystem.h"
#include "PlayerManager.h"
#include "Peppermint.h"
#include "Macaron.h"
#include "Korsica.h"

#include "Monster_Rekka.h"
#include "Monster_Mimosa.h"

#include "Wire.h"
#include "CUi_CommandIcon.h"
#include "CUi_SuccessSystem.h"
#include "GameManager.h"
#include "808_Controller.h"
#include "CUi_FightString.h"
#include "CUi_PlayerBar.h"
#include "CUi_GeneratorSystem.h"

#include "Mirror_Ball.h"
#include "BuiltIn_Magnet.h"
#include "Chai_Random_Sound.h"
#include "Blaze_FireStarter.h"
#include "PickMeUp_Can.h"
#include "CUi_SpecialComboAttackSystem.h"
#include "CUi_SkillCutScene.h"
#include "CUi_SkillChangeSystem.h"
#include "ODS_Controller.h"


wstring CChai::ObjID = TEXT("CChai");

CChai::CChai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CChai::CChai(const CChai& rhs)
    : CGameObject(rhs)
{
}

CChai::~CChai()
{
    Free();
}

HRESULT CChai::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
    _matrix		LocalTransformMatrix = XMMatrixIdentity();

    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Chai"), TEXT("../Bin/Resources/Models/Player/Chai/Chai.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Chai_Sans"), TEXT("../Bin/Resources/Models/Player/Chai/Chai_Sans.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Just_Beat_0"), TEXT("../Bin/Resources/Sounds/SFX/Player_Sound/Just_Beat/Player_InstFx_17.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Just_Beat_1"), TEXT("../Bin/Resources/Sounds/SFX/Player_Sound/Just_Beat/Player_InstFx_263.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Just_Beat_2"), TEXT("../Bin/Resources/Sounds/SFX/Player_Sound/Just_Beat/Player_InstFx_304.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Player_Dash"), TEXT("../Bin/Resources/Sounds/SFX/Player_Sound/Dash/Player_Dash.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Transfer2D"), TEXT("../Bin/Resources/Sounds/MapObject/Transfer2D.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Parry_GT"), TEXT("../Bin/Resources/Sounds/SFX/Parry_GT.wav")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Player_DamagedVoice1"), TEXT("../Bin/Resources/Models/Player/Chai/Player_DamagedVoice1.ogg")));

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("WireAction"), TEXT("../Bin/Resources/Sounds/MapObject/Magnet.mp3")));
    return S_OK;
}

HRESULT CChai::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
    _float4 vPos = _float4(0.f, 0.f, -0.f, 1.f);
    m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

    vector<shared_ptr<CAnimation>> animList = m_pModelCom->Get_Animation_List();
    for (auto& iter : animList)
    {
        m_vecAnimList.push_back(iter->Get_AnimName());
    }
    m_pModelCom->Set_AnimIndex("ch0000_idle_000", true);
    //m_pSans_ModelCom->Set_AnimIndex("ch0000_damage_040", false);
    _float4x4 xMat = {};
    m_pSans_ModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, xMat);

    //m_pCCT->Disable_Gravity();
    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Set_Gravity(40.f);
    Create_Attack_Tree();
    Create_Air_Attack_Tree();
    PrepareCallbacks();

    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matCurrentPos, m_fTime_Offset, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matCurrentPos, m_fTime_Offset, isBillBoard);
        }
    );

    //m_pTransformCom->Set_Position(XMVectorSet(1185.7f, 496.4f, 110.f, 1.f));//rekka
    m_pTransformCom->Set_Position(XMVectorSet(5.7f, 0.4f, 84.f, 1.f));//main
    //m_pTransformCom->Set_Position(XMVectorSet(0.f, 20.f, 0.f, 1.f));
    m_pCCT->Set_Position(m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->Generate_Mips();

    // Reserve cinematic last callback
    CCine_Manager::Get_Instance()->AddLastCallback("CS0230", 
        [&]() {
            auto vNewPos = XMVectorSet(1176.55884f, 485.6f, 112.57338f, 1.f);
            Set_Player_Position(vNewPos);

            vNewPos += { 1.f, 0.f, 0.f, 0.f };
            m_pTransformCom->LookAt_Horizontal(vNewPos);
        }
    );
    CCine_Manager::Get_Instance()->AddLastCallback("CS0235",
        [&]() {
            auto vNewPos = XMVectorSet(1176.55884f, 485.6f, 112.57338f, 1.f);
            Set_Player_Position(vNewPos);

            vNewPos += { 1.f, 0.f, 0.f, 0.f };
            m_pTransformCom->LookAt_Horizontal(vNewPos);
        }
    );
    CCine_Manager::Get_Instance()->AddLastCallback("EV1130",
        [&]() {
            _float4x4 matResult;
            CCine_Manager::Get_Instance()->GetMatTrans("ch0000", matResult);
            auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
            Set_Player_Position(vNewPos);

            vNewPos -= XMVectorSetW(XMLoadFloat4x4(&matResult).r[2], 0.f);
            m_pTransformCom->LookAt_Horizontal(vNewPos);

            m_eMoveState = CHAI_MOVE_STATE::IDLE;
            m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_idle_000", true);
            //m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        }
    );
    CCine_Manager::Get_Instance()->AddStartCallback("EV1130",
        [&]() {
            m_ZipLineBlur->UnionEffectEnd();
            m_ZipLineBlur = nullptr;
        }
    );
    auto Callback_ch0000_ExcepLookAt = [&]() {
        _float4x4 matResult;
        CCine_Manager::Get_Instance()->GetMatTrans("ch0000", matResult);
        auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        Set_Player_Position(vNewPos);

        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;

        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_idle_000", true);
        };
    auto Callback_ch0000 = [&]() {
        _float4x4 matResult;
        CCine_Manager::Get_Instance()->GetMatTrans("ch0000", matResult);
        auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        Set_Player_Position(vNewPos);

        vNewPos -= XMVectorSetW(XMLoadFloat4x4(&matResult).r[2], 0.f);
        m_pTransformCom->LookAt_Horizontal(vNewPos);

        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;

        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_idle_000", true);
        };
    CCine_Manager::Get_Instance()->AddLastCallback("CS0230", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS0235", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS0240", Callback_ch0000);

    CCine_Manager::Get_Instance()->AddLastCallback("EV1120", Callback_ch0000_ExcepLookAt);
    CCine_Manager::Get_Instance()->AddLastCallback("EV1150", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("EV1160", Callback_ch0000_ExcepLookAt);
    CCine_Manager::Get_Instance()->AddLastCallback("EV1170", Callback_ch0000);

    CCine_Manager::Get_Instance()->AddLastCallback("CS1210", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS1215", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS1220", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS0820", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS0825", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS0830", Callback_ch0000);
    CCine_Manager::Get_Instance()->AddLastCallback("CS0835", Callback_ch0000);

    CCine_Manager::Get_Instance()->AddLastCallback("CS0106", Callback_ch0000_ExcepLookAt);

    CCine_Manager::Get_Instance()->AddStartCallback("CS0106", [&]() {
        m_bFirstTick = true;
        }
    );
    CCine_Manager::Get_Instance()->AddLastCallback("EV1180", [&]() {
        _float4x4 matResult;
        CCine_Manager::Get_Instance()->GetMatTrans("ch0101", matResult);
        auto vNewPos = XMVectorSetW(XMLoadFloat4x4(&matResult).r[3], 1.f);
        Set_Player_Position(vNewPos);

        vNewPos -= XMVectorSetW(XMLoadFloat4x4(&matResult).r[2], 0.f);
        m_pTransformCom->LookAt_Horizontal(vNewPos);

        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        //Adjust_Idle_Timing();
        //m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        }
    );

    // Add sound channel
    GAMEINSTANCE->Add_Unique_Sound_Channel("Chai_Guitar");

    m_pRandSound = GAMEINSTANCE->Add_Clone_Return(GAMEMANAGER->Get_Current_Level(), TEXT("Layer_RandSound"), CChai_Random_Sound::ObjID);

    m_vecSkill.resize(2);
    m_vecSkill[0]= ("Overdrive_Slash");
    m_vecSkill[1]= ("Twin_Assault");
    //m_vecSkill.push_back("Pick_Me_Up");


    PLAYERMANAGER->Set_PlayerEquipSkill(Skill_Change_Equip("Overdrive_Slash"));
    PLAYERMANAGER->Set_PlayerNonEquipSkill(Skill_Change_Equip("Twin_Assault"));
    // PLAYERMANAGER->Set_PlayerEquipSkill(Skill_Change_Equip("Pick_Slide"));
    SkillDesc Desc = Skill_Change_Equip("Overdrive_Slash");
    PLAYERMANAGER->Add_PlayerSkill(Desc.SkillName, Desc);
    Desc = Skill_Change_Equip("Twin_Assault");
    PLAYERMANAGER->Add_PlayerSkill(Desc.SkillName, Desc);
    return S_OK;
}

void CChai::PriorityTick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
        return;
    m_pWire->PriorityTick(fTimeDelta);
}

void CChai::Tick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame() && !m_bFirstTick)
        return;

    if (!PLAYERMANAGER->Get_PlayerEquipSkill().SkillName.empty())
        m_vecSkill[0] = Translate_Skill_Info(PLAYERMANAGER->Get_PlayerEquipSkill());
    if (!PLAYERMANAGER->Get_PlayerNonEquipSkill().SkillName.empty())
        m_vecSkill[1] = Translate_Skill_Info(PLAYERMANAGER->Get_PlayerNonEquipSkill());

    if (m_fRimTimer > 0.f)
    {
        m_fRimTimer -= fTimeDelta;
        m_fRimColor.w -= fTimeDelta * 0.3f;
        if (m_fRimColor.w < 0.f)
            m_fRimColor.w = 0.f;
    }
    else if (m_fRimTimer < 0.f)
    {
        m_bRimLight = false;
        m_fRimTimer = 0;
    }
    if (m_eEvent_State == CHAI_EVENT_STATE::RHYTHMPARRY)
    {
        m_fCoolDown -= fTimeDelta;
        if (m_fReactTimer >= 0.f)
        {
            m_fReactTimer -= fTimeDelta;
        }
        Rhythm_Parry_Mode(fTimeDelta);
    }
    else if (m_eEvent_State == CHAI_EVENT_STATE::ZIPLINE)
        ZiplineMove(fTimeDelta);
    else if (m_eEvent_State == CHAI_EVENT_STATE::JUMPPAD)
        JumpPad_Move(fTimeDelta);
    else if (m_eMoveState != CHAI_MOVE_STATE::DAMAGED && !m_bIsEvent && m_eMoveState != CHAI_MOVE_STATE::EVENT)
    {
        if (m_bWire == false && m_eMoveState != CHAI_MOVE_STATE::EVENT && !Battle_Engage(fTimeDelta) && !Dash(fTimeDelta) && !Attack(fTimeDelta) && m_eMoveState != CHAI_MOVE_STATE::DASH && !m_bOnAir &&
            m_eMoveState != CHAI_MOVE_STATE::JUMP && m_eMoveState != CHAI_MOVE_STATE::DOUBLE_JUMP &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-cp_000") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_200") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_032") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_000") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_010") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_008") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("h0000_atk-jump_082") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("h0000_atk-jump_083")
            )
            Move(fTimeDelta);
        else if (m_bOnAir && (m_eMoveState == CHAI_MOVE_STATE::JUMP || m_eMoveState == CHAI_MOVE_STATE::DOUBLE_JUMP || m_eMoveState == CHAI_MOVE_STATE::JUMP_FALL || m_eMoveState == CHAI_MOVE_STATE::DOUBLE_JUMP_FALL)
            && m_bWire == false)
            AirMove(fTimeDelta);

        if (m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-cp_000") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_200") &&
            m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_032") &&
            m_eEvent_State != CHAI_EVENT_STATE::JUMPPAD && 
            m_bWire == false)
            Jump(fTimeDelta);
    }
    Animation_End();
    Animation(fTimeDelta);

    if(m_eEvent_State==CHAI_EVENT_STATE::MIMOSA_DANCE)
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Change_Animation(m_pModelCom->Get_AnimName());

    Generator_Mode(fTimeDelta);
    Rhythm_Parry_Mode(fTimeDelta);
    if (m_b2D_Mode)
    {
        _vector vPos = {};
        if (m_strFixedXYZ == "X")
        {
            vPos = XMVectorSetX(m_pTransformCom->Get_Position(), m_f2D_FixedPos);
            m_pTransformCom->Set_Position(vPos);
            m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        }
        if (m_strFixedXYZ == "Y")
        {
            vPos = XMVectorSetY(m_pTransformCom->Get_Position(), m_f2D_FixedPos);
            m_pTransformCom->Set_Position(vPos);
            m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        }
        if (m_strFixedXYZ == "Z")
        {
            vPos = XMVectorSetZ(m_pTransformCom->Get_Position(), m_f2D_FixedPos);
            m_pTransformCom->Set_Position(vPos);
            m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        }
    }

    Play_Just_Effect(fTimeDelta);
    if (m_bCCT_On)
        m_pCCT->Update(fTimeDelta);

    Call_Assist(fTimeDelta);

    m_pWire->Tick(fTimeDelta);

    if (GAMEINSTANCE->Get_KeyDown(DIK_F3)) {
        EFFECTMANAGER->Call_Union_Effect("Unlimited_Void", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
    }

    if (GAMEINSTANCE->Get_KeyDown(DIK_F4)) {
        CBlaze_FireStarter::Blaze_FireStarter eFireStarterDesc = {};
		eFireStarterDesc.eFireStarterType = CBlaze_FireStarter::FIRESTARTER_LIST::FIRESTARTER_STRAIGHT;
        eFireStarterDesc.matWorld = m_pTransformCom->Get_WorldMatrix();

		GAMEINSTANCE->Add_Clone(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("CBlaze_FireStarter"), &eFireStarterDesc);
    }

    // Update Colliders
    auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
    //m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pCollider_Hit->Update(matrixSpine);

    auto matrixRightHand = XMLoadFloat4x4(m_pMat_RightHand);
    matrixRightHand.r[0] = XMVector3Normalize(matrixRightHand.r[0]) * m_fWeaponCollider_Extents.x;
    matrixRightHand.r[1] = XMVector3Normalize(matrixRightHand.r[1]) * m_fWeaponCollider_Extents.y;
    matrixRightHand.r[2] = XMVector3Normalize(matrixRightHand.r[2]) * m_fWeaponCollider_Extents.z;
    m_pCollider_Attack->Update(matrixRightHand * m_pTransformCom->Get_WorldMatrix_XMMat());

    for (auto& pPartObj : m_pWeapon)
        pPartObj->Tick(fTimeDelta);
    if (!m_bIsEvent && m_eMoveState != CHAI_MOVE_STATE::EVENT && !m_bInteraction && m_eMoveState != CHAI_MOVE_STATE::DAMAGED)
    {
        if (GAMEINSTANCE->Get_KeyDown(DIK_E))
        {
            m_eNextAttack = ATTACK_TYPE_END;
            m_bAttack_Input = false;
            m_bNextAttack = false;
            m_fAttack_Motion_Offset = 0.f;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
            m_eMoveState = CHAI_MOVE_STATE::PARRY;
            m_pModelCom->Set_AnimIndex("ch0000_action_030", false);
        }

        if (GAMEINSTANCE->Get_KeyDown(DIK_Q) &&
            m_pModelCom->Get_StringAnimIndex("ch0000_action_130") != m_pModelCom->Get_CurrentAnimIndex())
        {
            m_fWire_Target_Distance = 0.f;
            m_fWire_Target_Length = 0.f;
            m_bGimmicWire = false;
            _float fDistance = FLT_MAX;
            m_pWire_Target = nullptr;
            for (auto& iter : CCombat_Manager::Get_Instance()->Get_Enemy_List())
            {
                auto targetTransform = iter->Get_Transform();

                _float length = XMVectorGetX(XMVector3Length(targetTransform->Get_Position() - m_pPlayer_Camera->Get_Transform()->Get_Position()));
                _vector fDir = XMVector3Normalize(targetTransform->Get_Position() - m_pPlayer_Camera->Get_Transform()->Get_Position());
                _vector vDot = XMVector3Dot(fDir, m_pPlayer_Camera->Get_Transform()->Get_NormLook());
                if (fDistance > length && XMVectorGetX(vDot) > 0.f)
                {
                    fDistance = length;
                    m_pWire_Target = iter;
                }
            }
            //shared_ptr<CGameObject> pGimmicTarget = m_pPlayer_Camera->Get_Wire_Target();
            for (auto& iter : m_pPlayer_Camera->Get_Wire_Target())
            {
                if (iter)
                {
                    auto targetTransform = iter->Get_Transform();

                    _float length = XMVectorGetX(XMVector3Length(targetTransform->Get_Position() - m_pPlayer_Camera->Get_Transform()->Get_Position()));
                    _vector fDir = XMVector3Normalize(targetTransform->Get_Position() - m_pPlayer_Camera->Get_Transform()->Get_Position());
                    _vector vDot = XMVector3Dot(fDir, m_pPlayer_Camera->Get_Transform()->Get_NormLook());

                    if(!m_b2D_Mode)
                    {
                        if (length < 40.f && length > 10.f && fDistance > length && XMVectorGetX(vDot) > 0.f)
                        {
                            m_bGimmicWire = true;
                            fDistance = length;
                            m_pWire_Target = iter;
                        }
                    }
                    else
                    {
                        if (m_pComboWire_Target)
                        {
                            m_bGimmicWire = true;
                            fDistance = length;
                            m_pWire_Target = m_pComboWire_Target;
                        }
                        else
                        {
                            if (length < 40.f && length > 10.f && fDistance > length && XMVectorGetX(vDot) > 0.f)
                            {
                                m_bGimmicWire = true;
                                fDistance = length;
                                m_pWire_Target = iter;
                            }
                        }
                    }
                }
            }
            if (m_pWire_Target)
            {
                m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, L"WireAction", 6);
                m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"WireAction");
                m_bWire = true;
                m_eNextAttack = ATTACK_TYPE_END;
                m_bAttack_Input = false;
                m_bNextAttack = false;
                m_fAttack_Motion_Offset = 0.f;
                Initialize_Attack_Tree(m_pTree_Attack);
                Initialize_Attack_Tree(m_pTree_Air_Attack);
                m_vecCombo.clear();
                m_pModelCom->Set_AnimIndex("ch0000_action_130", false);
                m_pCCT->Set_Velocity({ 0,0,0,0 });

                _float4x4 matBone = {};
                XMStoreFloat4x4(&matBone, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("r_hand")) * m_pTransformCom->Get_WorldMatrix_XMMat());
                _float4x4 mat_Wire_Bone = {};
                if (m_bGimmicWire)
                {                
                    shared_ptr<CMagnet> pMagnet = dynamic_pointer_cast<CMagnet>(m_pWire_Target);
                    if (pMagnet == nullptr)
                        assert(false);
                    pMagnet->Player_UseMagnet();

                   _float4x4 wirePos =  m_pWire_Target->Get_Transform()->Get_WorldMatrix();
                   _matrix wireMatPos =  m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat();

                   auto wireTarget = dynamic_pointer_cast<CBuiltIn_Magnet>(m_pWire_Target);
                   if (wireTarget)
                       wireMatPos.r[3] -= m_pWire_Target->Get_Transform()->Get_NormLook() * 0.5f;
                   else
                       wireMatPos.r[3] -= m_pWire_Target->Get_Transform()->Get_NormUp() * 1.5f;
                   XMStoreFloat4x4(&mat_Wire_Bone, wireMatPos);
                   //wirePos.m[3][1] = wirePos.m[3][1] -= 2.f;
                   //mat_Wire_Bone = wirePos;
                   //XMStoreFloat4x4(&mat_Wire_Bone, XMLoadFloat4x4(static_pointer_cast<CModel>(m_pWire_Target->Find_Component(TEXT("Model_Com")))->Get_BoneMatrixPtr("body_attach_00")) * m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat());
                }
                else
                    XMStoreFloat4x4(&mat_Wire_Bone, XMLoadFloat4x4(static_pointer_cast<CModel>(m_pWire_Target->Find_Component(TEXT("Com_Model")))->Get_BoneMatrixPtr("spine_00")) * m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat());


                _vector targetPos = m_pWire_Target->Get_Transform()->Get_Position();

                _vector targetDir = XMVector3Normalize(targetPos - m_pTransformCom->Get_Position());
                _float targetDistance = XMVectorGetX(XMVector3Length(targetPos - m_pTransformCom->Get_Position()));

                _float4 fBonPos = { matBone.m[3][0], matBone.m[3][1], matBone.m[3][2], 1 };
                _float4 fWire_Target_BonPos = { mat_Wire_Bone.m[3][0], mat_Wire_Bone.m[3][1], mat_Wire_Bone.m[3][2], 1 };
                m_fWire_Target_Distance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&fWire_Target_BonPos) - XMLoadFloat4(&fBonPos)));
            }
        }
        if (GAMEINSTANCE->Get_KeyDown(DIK_2))
        {
            if (m_vecSkill.size() < 2)
                return;
            if(m_vecSkill[0].size() < 1 || m_vecSkill[1].size() < 1)
                return;
            SkillDesc EquipSkillDesc = Skill_Change_Equip(m_vecSkill[1]);
            SkillDesc NonEquipSkillDesc = Skill_Change_Equip(m_vecSkill[0]);

            static_pointer_cast<CUi_SkillChangeSystem>(CUi_Mgr::Get_Instance()->Find_UI
            (CUi_SkillChangeSystem::ObjID))->Set_Change(EquipSkillDesc, NonEquipSkillDesc);
            string eqSkill = m_vecSkill[0];
            m_vecSkill[0] = m_vecSkill[1];
            m_vecSkill[1] = eqSkill;

            PLAYERMANAGER->Set_PlayerEquipSkill(Skill_Change_Equip(m_vecSkill[0]));
            PLAYERMANAGER->Set_PlayerNonEquipSkill(Skill_Change_Equip(m_vecSkill[1]));
        }
        if (GAMEINSTANCE->Get_KeyDown(DIK_R))
        {
            SP_Skill(m_vecSkill[0]);
        }
    }
   
    /*  if (GAMEINSTANCE->Get_KeyDown(DIK_R))
      {
          m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
          m_pModelCom->Set_AnimIndex("ch0000_atk-sp-cp_000", false);

          shared_ptr<CPeppermint> pPeppermint = static_pointer_cast<CPeppermint>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Peppermint")));
          pPeppermint->SP_Attack(m_pTransformCom->Get_Position());
          pPeppermint->Get_Transform()->LookAt(pPeppermint->Get_Transform()->Get_Position() + m_pTransformCom->Get_NormLook());
          m_fReverb -= 50.f;
      }*/
    if (GAMEINSTANCE->Get_KeyDown(DIK_T))
    {
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp_200", false);

        shared_ptr<CKorsica> pKorsica = static_pointer_cast<CKorsica>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Korsica")));
        pKorsica->SP_Attack(m_pTransformCom->Get_Position());
        pKorsica->Get_Transform()->LookAt(pKorsica->Get_Transform()->Get_Position() + m_pTransformCom->Get_NormLook());
    }
    if (GAMEINSTANCE->Get_KeyDown(DIK_G))
    {
        m_bIsSans = true;
        m_pModelCom->Set_AnimIndex("ch0000_damage_040", false);
        m_pSans_ModelCom->Set_AnimIndex("ch0000_damage_040", false);
        m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
    }
    if (GAMEINSTANCE->Get_KeyDown(DIK_U))
    {
        DamageInfo dInfo = {};
        dInfo.eAttackType = EAttackType::Fire;
        dInfo.eAttack_Direction = EAttackDirection::DEFAULT;
        dInfo.pObjectOwner = shared_from_this();
        dInfo.fAmountDamage = 0.f;
     
        TakeDamage(dInfo);
    }
    if (GAMEINSTANCE->Get_KeyDown(DIK_I))
    {
        GAMEINSTANCE->Play_Unique_Sound_Channel("Chai_Guitar", SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_0");
    }
    if (GAMEINSTANCE->Get_KeyDown(DIK_F1))
    {
        m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMVector3Normalize(GAMEINSTANCE->Get_CamLook_Vector()) * 10.f);
        _float3 fPos = {};
        m_pCCT->Set_Position(m_pTransformCom->Get_WorldMatrix_XMMat());
    }
    Event_Tick(fTimeDelta);
    Zoom_Tick(fTimeDelta);
    CPlayer_Manager::Get_Instance()->Set_Hp(m_fHp);
    CPlayer_Manager::Get_Instance()->Set_Reverb(m_fReverb);

    if (m_fBefore_Time_Multiflier < 1.f && m_fTimeMultiflier == 1.f)
        EFFECTMANAGER->Change_Union_Effect_Speed(1.f);

    m_fBefore_Time_Multiflier = m_fTimeMultiflier;
    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

    if (m_bFirstTick)
        m_bFirstTick = false;
}

void CChai::LateTick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame() && !m_bFirstTick)
        return;

    if(m_eEvent_State != CHAI_EVENT_STATE::ZIPLINE)
        m_pWeapon[0]->LateTick(fTimeDelta);
    if(m_bRender_Can)
        m_pWeapon[1]->LateTick(fTimeDelta);

    if (m_bWire)
        m_pWire->LateTick(fTimeDelta);

    GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_PLAYER, m_pCollider_Hit);
    if (m_isAttackOn)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_PLAYER, m_pCollider_Attack);

    m_iRenderGroup = 0;

    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    if(m_bRimLight)
         m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());

    if (m_bFirstTick)
        m_bFirstTick = false;
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG


}

HRESULT CChai::Render()
{
    if (m_iRenderGroup == 1)
    {
        auto pGameInstance = m_pGameInstance.lock();
        _float4x4 ViewMat;

        XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        _float4x4 ProjMat;
        XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;
        //if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
        //   return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
            return E_FAIL;

        _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
        _float fViewDepth = Get_ViewDepth();
        _float fOutlineWidth = 3.f;
        if (fViewDepth <= 10.f)
        {
            fOutlineWidth = 1.2f;
        }

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
                return E_FAIL;
            //_float4 vColor = { 1.f,.5f,0.f,0.3f };
            _float4 vColor = m_fRimColor;
            m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
            m_pShaderCom->Bind_Float("g_OutlineWidth", fOutlineWidth);
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::MonoColorRim);

            m_pModelCom->Render((_uint)i);

        }
        return S_OK;
    }
    else
        m_iRenderGroup = 1;

    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;

    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
    //   return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    shared_ptr<CModel> pModel = nullptr;
    if (m_bIsSans)
        pModel = m_pSans_ModelCom;
    else
        pModel = m_pModelCom;

    _uint   iNumMeshes = pModel->Get_NumMeshes();
    _float fViewDepth = Get_ViewDepth();
    _float fOutlineWidth = 0.8f;
    if (fViewDepth <= 10.f)
    {
        fOutlineWidth = 0.4f;
    }

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        {
            //if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            //    return E_FAIL;
            //m_pShaderCom->Bind_Float("g_OutlineWidth", 0.01f);
            //m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

            //m_pModelCom->Render((_uint)i);
            continue;
        }
        _bool bNormal = true;
        if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        {
            bNormal = false;
        }

        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;
        if (i == 0)        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::NormalMap);
        else               m_pShaderCom->Begin(0);

        pModel->Render((_uint)i);

        if (i == 0)
            continue;
        m_pShaderCom->Bind_Float("g_OutlineWidth", fOutlineWidth);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        pModel->Draw((_uint)i);
    }

#ifdef _DEBUG

    m_pCollider_Hit->Render();
    // if(m_isAttackOn)
    m_pCollider_Attack->Render();

#endif
    return S_OK;
}

HRESULT CChai::Render_Shadow()
{
    if (m_pGameInstance.lock()->Get_Current_LevelID() == LEVEL_MIMOSA)
        return S_OK;

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

void CChai::Change_Level()
{
    m_fHp = m_fMaxHp;
    m_fReverb = 0.f;

    m_bAttack_Input = false;
    m_bNextAttack = false;
    Initialize_Attack_Tree(m_pTree_Attack);
    Initialize_Attack_Tree(m_pTree_Air_Attack);

    m_bTurning = false;
    m_pModelCom->Set_AnimIndexNonCancle("ch0000_idle_000", true);
}

void CChai::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack)
    {
        DamageInfo damageInfo = {};
        damageInfo.pObjectOwner = shared_from_this();
        if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_000") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 8.f;
            damageInfo.strAttackName = "L";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_001") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 10.f;
            damageInfo.strAttackName = "LL";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_002") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 11.f;
            damageInfo.strAttackName = "LLL";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_003") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::DOWN;
            damageInfo.fAmountDamage = 12.f;
            damageInfo.strAttackName = "LLLL";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_004") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::DOWN;
            damageInfo.fAmountDamage = 12.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_021") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Hard;
            damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
            damageInfo.fAmountDamage = 16.f;
            damageInfo.strAttackName = "RR";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_022") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Special;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 20.f;
            damageInfo.strAttackName = "RRR";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_024") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Hard;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 16.f;
            damageInfo.strAttackName = "R";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_030") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 10.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_031") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 10.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_040") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 16.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_041") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
            damageInfo.fAmountDamage = 3.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_051") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 9.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_053") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Air;
            damageInfo.eAttack_Direction = EAttackDirection::UP;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_081") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 12.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_093") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 10.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_094") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 12.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_095") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Special;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 20.f;
            damageInfo.strAttackName = "LL_LLL";
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_000") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_010") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Air;
            damageInfo.eAttack_Direction = EAttackDirection::UP;
            damageInfo.fAmountDamage = 14.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_120") == m_pModelCom->Get_CurrentAnimIndex())
        {
            if (m_pModelCom->Get_Current_Track_Position() < 22.f)
            {
                damageInfo.eAttackType = EAttackType::Air;
                damageInfo.eAttack_Direction = EAttackDirection::UP;
                damageInfo.fAmountDamage = 13.f;
            }
            else
            {
                damageInfo.eAttackType = EAttackType::Base;
                damageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
                damageInfo.fAmountDamage = 13.f;
            }
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_140") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_007") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::DOWN;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_010") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_011") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_012") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_013") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Special;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 20.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_041") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::LEFT;
            damageInfo.fAmountDamage = 11.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_052") == m_pModelCom->Get_CurrentAnimIndex())
        {
            if (m_pModelCom->Get_Current_Track_Position() < 22.f)
            {
                damageInfo.eAttackType = EAttackType::Base;
                damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
                damageInfo.fAmountDamage = 13.f;
            }
            else
            {
                damageInfo.eAttackType = EAttackType::Base;
                damageInfo.eAttack_Direction = EAttackDirection::DOWN;
                damageInfo.fAmountDamage = 13.f;
            }
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_070") == m_pModelCom->Get_CurrentAnimIndex())
        {
            if (m_pModelCom->Get_Current_Track_Position() < 22.f)
            {
                damageInfo.eAttackType = EAttackType::Base;
                damageInfo.eAttack_Direction = EAttackDirection::LEFT;
                damageInfo.fAmountDamage = 13.f;
            }
            else
            {
                damageInfo.eAttackType = EAttackType::Base;
                damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
                damageInfo.fAmountDamage = 13.f;
            }
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_080") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::RIGHT;
            damageInfo.fAmountDamage = 12.f;
        }
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_081") == m_pModelCom->Get_CurrentAnimIndex())
        {
            damageInfo.eAttackType = EAttackType::Base;
            damageInfo.eAttack_Direction = EAttackDirection::UP;
            damageInfo.fAmountDamage = 15.f;
        }
        pOther->Get_Owner()->TakeDamage(damageInfo);
    }
}

void CChai::TakeDamage(const DamageInfo& damageInfo)
{
    if (m_bIsEvent || m_eEvent_State == CHAI_EVENT_STATE::RHYTHMPARRY)
        return;

    shared_ptr <CTransform> pTargetTransform = damageInfo.pObjectOwner->Get_Transform();

    if (m_eMoveState == CHAI_MOVE_STATE::DAMAGED && m_pModelCom->Get_StringAnimIndex("ch0000_damage_040") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.5f)
        _int a = 10;
    else if (damageInfo.eAttackType == EAttackType::ZiplineHit)
    {
        _float3 vRandomPos = GAMEINSTANCE->Random_Float3({ -0.2f, -0.2f ,-0.2f }, { 0.2f, 0.2f, 0.f });
        _vector vRandomVector = XMVectorSetW(XMLoadFloat3(&vRandomPos), 1.f);
        _float4x4 matResult{};
        XMStoreFloat4x4(&matResult, XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslationFromVector(vRandomVector));
        PlayEffect("Big_HitEffect", matResult, 1.f, true);
        GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Player_DamagedVoice1");

        m_fHp -= damageInfo.fAmountDamage;
    }

    else if (
        m_eMoveState == CHAI_MOVE_STATE::DASH ||
        m_eMoveState == CHAI_MOVE_STATE::EVENT ||
        m_eMoveState == CHAI_MOVE_STATE::DAMAGED
        )
        return;

    if (damageInfo.eAttackType == EAttackType::Fire)
    {
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Is_Render(false);
        m_fHp -= damageInfo.fAmountDamage;
        m_bTurning = false;
        m_pTransformCom->LookAt_Horizontal(pTargetTransform->Get_Position());
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_damage_060", false);
        m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
        m_fIdle_Motion_Offset = 0.f;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
    }
    else if ((m_pModelCom->Get_StringAnimIndex("ch0000_action_030") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_action_031") == m_pModelCom->Get_CurrentAnimIndex()))
    {
        GAMEINSTANCE->Play_Unique_Sound_Channel("Chai_Guitar", SOUND_CHANNEL::SOUND_EFFECT, L"Parry_GT");
        DamageInfo OutdamageInfo = {};
        OutdamageInfo.eAttackType = EAttackType::Parry;
        OutdamageInfo.eAttack_Direction = EAttackDirection::DEFAULT;
        OutdamageInfo.fAmountDamage = 0.f;
        m_bTurning = false;
        _float4x4 fMat = {};
        EFFECTMANAGER->Call_Union_Effect("Chai_Parry", m_pTransformCom->Get_WorldMatrix_Ptr(), m_fTime_Offset);
        m_pPlayer_Camera->Shake(0.2f, 0.2f);
        if (damageInfo.pObjectOwner)
            damageInfo.pObjectOwner->TakeDamage(OutdamageInfo);
    }
    else if (damageInfo.eAttackType == EAttackType::Base)
    {
        m_fHp -= damageInfo.fAmountDamage;
        m_bTurning = false;
        m_pTransformCom->LookAt_Horizontal(pTargetTransform->Get_Position());
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_damage_010", false);
        m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
        m_fIdle_Motion_Offset = 0.f;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
    }
    else if (damageInfo.eAttackType == EAttackType::Hard)
    {
        m_fHp -= damageInfo.fAmountDamage;
        m_bTurning = false;
        m_pTransformCom->LookAt_Horizontal(pTargetTransform->Get_Position());
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_damage_000", false);
        m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
        m_fIdle_Motion_Offset = 0.f;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
    }
    else if (damageInfo.eAttackType == EAttackType::ELECTRIC)
    {
        m_fHp -= damageInfo.fAmountDamage;
        m_bIsSans = true;
        m_pModelCom->Set_AnimIndex("ch0000_damage_040", false);
        m_pSans_ModelCom->Set_AnimIndex("ch0000_damage_040", false);
        m_eMoveState = CHAI_MOVE_STATE::DAMAGED;

        m_pPlayer_Camera->Shake(0.2f, 0.4f);

        _float4x4 vWorldMat = m_pTransformCom->Get_WorldMatrix();

        _vector vPos = XMVector4Normalize(GAMEINSTANCE->Get_CamPosition_Vector() - (m_pTransformCom->Get_Position()));
        vWorldMat._41 -= XMVectorGetX(vPos) * 0.5f;
        vWorldMat._43 -= XMVectorGetZ(vPos) * 0.5f;

        EFFECTMANAGER->Call_Fixed_Union_Effect("Chai_Electric_Shock", &vWorldMat, m_fTime_Offset, true);

    }
    else if (damageInfo.eAttackType == EAttackType::Special)
    {
        if (damageInfo.strAttackName == "Rekka_WWE")
        {
            m_fHp -= damageInfo.fAmountDamage;
            m_bTurning = false;

            // 각도
            m_pTransformCom->Set_Position(pTargetTransform->Get_Position());
            m_pTransformCom->LookAt_Horizontal(pTargetTransform->Get_NormLook() + pTargetTransform->Get_Position());

            m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
            m_pCCT->Disable_Gravity();

            // 위치
            auto pRekka = dynamic_pointer_cast<CMonster_Rekka>(damageInfo.pObjectOwner);
            _matrix matBone = XMLoadFloat4x4(pRekka->m_pMat_Attach_Chai);
            matBone.r[0] = XMVector3Normalize(matBone.r[0]);
            matBone.r[1] = XMVector3Normalize(matBone.r[1]);
            matBone.r[2] = XMVector3Normalize(matBone.r[2]);
            _matrix vMat = matBone * pTargetTransform->Get_WorldMatrix_XMMat();
            m_pCCT->Set_FootPosition(vMat);

            m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_event-rekka_100", false);
            m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
            m_fIdle_Motion_Offset = 0.f;
            m_fAttack_Motion_Offset = 0.f;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
        }
        else if (damageInfo.strAttackName == "Mimosa_Slap")
        {
            m_bTurning = false;

            //// 각도

            // 위치
            auto pMimmosa = dynamic_pointer_cast<CMonster_Mimosa>(damageInfo.pObjectOwner);
            _matrix matBone = XMLoadFloat4x4(pMimmosa->Get_Chai_Attach_Matrix());
            matBone.r[0] = XMVector3Normalize(matBone.r[0]);
            matBone.r[1] = XMVector3Normalize(matBone.r[1]);
            matBone.r[2] = XMVector3Normalize(matBone.r[2]);
            _matrix vMat = matBone * pTargetTransform->Get_WorldMatrix_XMMat();

            _vector vVec = vMat.r[3];
            vVec = XMVectorSetY(vVec, XMVectorGetY(vVec) + 0.1f);
            vMat.r[3] = vVec;
            m_pCCT->Set_FootPosition(vMat);

            m_pTransformCom->Set_Position(vMat.r[3]);
            m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + pTargetTransform->Get_NormLook());

            m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
            m_pCCT->Disable_Gravity();

            m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_event-mimosa_000", false);
            m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
            m_fIdle_Motion_Offset = 0.f;
            m_fAttack_Motion_Offset = 0.f;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
        }
        else if (damageInfo.strAttackName == "Rhythm_Parry")
        {

            m_pRPMonster = damageInfo.pObjectOwner;
            m_bRPMode = true;
            m_bTurning = false;
            m_bIsEvent = true;
            m_eMoveState = CHAI_MOVE_STATE::DAMAGED;
            m_fIdle_Motion_Offset = 0.f;
            m_fAttack_Motion_Offset = 0.f;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
            m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_damage_000", false);
            m_strEvent = "";
        }
    }
}

void CChai::Play_808_Just_Effect()
{
    shared_ptr<C808_Companion> pCompanion = static_pointer_cast<C808_Companion>(static_pointer_cast<C808_Controller>(PLAYERMANAGER->Get_808_Controller())->Get_808_Companion());
    pCompanion->Play_JustMark_Effect(m_i808_Effect);
    m_i808_Effect++;
    if (m_i808_Effect > 3)
        m_i808_Effect = 0;
}

void CChai::Set_Player_Position(_vector vPosition)
{
    m_pTransformCom->Set_Position(vPosition);
    _float3 fPos = {};
    m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
}

HRESULT CChai::Ready_Components()
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chai"), TEXT("Com_Model")));
    m_pChai_ModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chai"), TEXT("Com_Model_Chai")));
    m_pSans_ModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chai_Sans"), TEXT("Com_Model_Sans")));

    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 1.f;
    cctDesc.radius = 0.3f;
    cctDesc.stepOffset = 0.25f;
    cctDesc.position = PxExtendedVec3(0.0f, 0.0f, 0.0f);
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));
    m_pCCT->Get_Controller()->getActor()->setName("Player");

    CBasic_Guitar::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
    WeaponDesc.pEventBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("prop_00");
    WeaponDesc.pLeftHandBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("l_attach_hand_00");
    WeaponDesc.pParentTransform = m_pTransformCom;
    /* 추가된 파트오브젝트가 부모를 기준으로 회전하거나 이동할 수 있다. */
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;

    shared_ptr <CPartObject> pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CBasic_Guitar"), &WeaponDesc));
    if (nullptr == pWeapon)
        return E_FAIL;
    m_pWeapon.push_back(pWeapon);

    CPickMeUp_Can::WEAPON_DESC		canDesc{};
    canDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("l_attach_hand_00");
    canDesc.pParentTransform = m_pTransformCom;
    /* 추가된 파트오브젝트가 부모를 기준으로 회전하거나 이동할 수 있다. */
    canDesc.fSpeedPerSec = 0.f;
    canDesc.fRotationPerSec = 0.f;

    pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CPickMeUp_Can"), &canDesc));
    if (nullptr == pWeapon)
        return E_FAIL;
    m_pWeapon.push_back(pWeapon);

    // m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
    m_pMat_Spine = m_pTransformCom->Get_WorldMatrix_Ptr();
    m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");

    //CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
    //sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
    //sphereDesc.fRadius = 0.6f;
    //m_pCollider_Hit =
    //    dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
    //m_pCollider_Hit->Set_Owner(shared_from_this());
    //m_pCollider_Hit->Set_ColliderTag(TEXT("Player_Hit"));

    CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = _float3{ 0.f, 0.8f, 0.f };
    obbDesc.vExtents = _float3{ 0.3f, 0.8f, 0.3f };
    obbDesc.vRotation = _float3{ 0.f, 0.f, 0.f };
    m_pCollider_Hit =
        dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Hit"), &obbDesc));
    m_pCollider_Hit->Set_Owner(shared_from_this());
    m_pCollider_Hit->Set_ColliderTag(TEXT("Player_Hit"));

    //CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
    obbDesc.vCenter = _float3{ 0.f, 0.f, -0.6f };
    obbDesc.vExtents = _float3{ 0.36f, 0.2f, 1.0f };
    m_fWeaponCollider_Extents = _float3{ 1.f, 1.f, 1.f };
    obbDesc.vRotation = _float3{ 0.f, 0.f, 0.f };
    m_pCollider_Attack =
        dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Attack"), &obbDesc));
    m_pCollider_Attack->Set_Owner(shared_from_this());
    m_pCollider_Attack->Set_ColliderTag(TEXT("Player_Attack"));

    m_pWire = static_pointer_cast<CWire>(GAMEINSTANCE->Clone_Object(L"CWire", nullptr));
    m_pWire->Get_Transform()->Set_Scale(_float3(0.2f, 0.2f, 1));
    m_pWire->SetRootMatrix(m_pWire->Get_Transform()->Get_WorldMatrix());
    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

void CChai::Create_Attack_Tree()
{
    m_pTree_Attack = new AttacK_Tree("Root");

    AttacK_Tree* L = new AttacK_Tree("L");
    AttacK_Tree* R = new AttacK_Tree("R");
    AttacK_Tree* Rest = new AttacK_Tree("Rest");

    m_pTree_Attack->children[ATTACK_TREE_TYPE::L] = L;
    m_pTree_Attack->children[ATTACK_TREE_TYPE::R] = R;
    m_pTree_Attack->children[ATTACK_TREE_TYPE::REST] = Rest;

    L->parent = m_pTree_Attack;
    R->parent = m_pTree_Attack;
    Rest->parent = m_pTree_Attack;

    /* L */
    AttacK_Tree* L_L = new AttacK_Tree("LL");
    AttacK_Tree* L_R = new AttacK_Tree("LR");
    AttacK_Tree* L_Rest = new AttacK_Tree("L_Rest");

    L->children[ATTACK_TREE_TYPE::L] = L_L;
    L->children[ATTACK_TREE_TYPE::R] = (L_R);
    L->children[ATTACK_TREE_TYPE::REST] = (L_Rest);

    L_L->parent = L;
    L_R->parent = L;
    L_Rest->parent = L;
    /* L_Rest_L */
    AttacK_Tree* L_Rest_L = new AttacK_Tree("L_RestL");

    L_Rest->children[ATTACK_TREE_TYPE::L] = (L_Rest_L);
    L_Rest_L->parent = L_Rest;

    /* L_Rest_LL */
    AttacK_Tree* L_Rest_LL = new AttacK_Tree("L_RestLL");
    L_Rest_L->children[ATTACK_TREE_TYPE::L] = (L_Rest_LL);
    L_Rest_LL->parent = L_Rest_L;

    /* L_L */
    AttacK_Tree* L_L_L = new AttacK_Tree("LLL");
    AttacK_Tree* L_L_R = new AttacK_Tree("LLR");
    AttacK_Tree* L_L_Rest = new AttacK_Tree("LL_Rest");

    L_L->children[ATTACK_TREE_TYPE::L] = (L_L_L);
    L_L->children[ATTACK_TREE_TYPE::R] = (L_L_R);
    L_L->children[ATTACK_TREE_TYPE::REST] = (L_L_Rest);

    L_L_L->parent = L_L;
    L_L_R->parent = L_L;
    L_L_Rest->parent = L_L;

    /* LL_RestL */
    AttacK_Tree* L_L_Rest_L = new AttacK_Tree("LL_RestL");
    L_L_Rest->children[ATTACK_TREE_TYPE::L] = (L_L_Rest_L);
    L_L_Rest_L->parent = L_L_Rest;

    /* LL_RestLL */
    AttacK_Tree* L_L_Rest_L_L = new AttacK_Tree("LL_RestLL");
    L_L_Rest_L->children[ATTACK_TREE_TYPE::L] = (L_L_Rest_L_L);
    L_L_Rest_L_L->parent = L_L_Rest_L;

    /* LL_RestLLL */
    AttacK_Tree* L_L_Rest_L_L_L = new AttacK_Tree("LL_RestLLL");
    L_L_Rest_L_L->children[ATTACK_TREE_TYPE::L] = (L_L_Rest_L_L_L);
    L_L_Rest_L_L_L->parent = L_L_Rest_L_L;

    /* L_L_L */
    AttacK_Tree* L_L_L_L = new AttacK_Tree("LLLL");

    L_L_L->children[ATTACK_TREE_TYPE::L] = (L_L_L_L);
    L_L_L_L->parent = L_L_L;

    /* L_R_R */
    AttacK_Tree* L_R_R = new AttacK_Tree("LRR");
    L_R->children[ATTACK_TREE_TYPE::R] = (L_R_R);
    L_R_R->parent = L_R;

    /* L_R_L */
    AttacK_Tree* L_R_L = new AttacK_Tree("LRL");
    L_R->children[ATTACK_TREE_TYPE::L] = (L_R_L);
    L_R_L->parent = L_R;

    /* L_R_L_L */
    AttacK_Tree* L_R_L_L = new AttacK_Tree("LRLL");
    L_R_L->children[ATTACK_TREE_TYPE::L] = (L_R_L_L);
    L_R_L_L->parent = L_R_L;

    /* L_R_L_L_L */
    AttacK_Tree* L_R_L_L_L = new AttacK_Tree("LRLLL");
    L_R_L_L->children[ATTACK_TREE_TYPE::L] = (L_R_L_L_L);
    L_R_L_L_L->parent = L_R_L_L;

    /* R */
    AttacK_Tree* R_L = new AttacK_Tree("RL");
    AttacK_Tree* R_R = new AttacK_Tree("RR");

    R->children[ATTACK_TREE_TYPE::L] = (R_L);
    R->children[ATTACK_TREE_TYPE::R] = (R_R);

    R_L->parent = R;
    R_R->parent = R;

    /* RL */
    AttacK_Tree* R_L_L = new AttacK_Tree("RLL");
    AttacK_Tree* R_L_R = new AttacK_Tree("RLR");

    R_L->children[ATTACK_TREE_TYPE::L] = (R_L_L);
    R_L->children[ATTACK_TREE_TYPE::R] = (R_L_R);

    R_L_L->parent = R_L;
    R_L_R->parent = R_L;

    /* RR */
    AttacK_Tree* R_R_L = new AttacK_Tree("RRL");
    AttacK_Tree* R_R_R = new AttacK_Tree("RRR");

    R_R->children[ATTACK_TREE_TYPE::L] = (R_R_L);
    R_R->children[ATTACK_TREE_TYPE::R] = (R_R_R);

    R_R_L->parent = R_R;
    R_R_R->parent = R_R;

    /* RRL */
    AttacK_Tree* R_R_L_L = new AttacK_Tree("RRLL");

    R_R_L->children[ATTACK_TREE_TYPE::L] = (R_R_L_L);

    R_R_L_L->parent = R_R_L;
}

void CChai::Delete_Attack_Tree(AttacK_Tree* node)
{
    if (!node)
        return;

    // 자식 노드들을 먼저 해제
    for (AttacK_Tree* child : node->children)
    {
        Delete_Attack_Tree(child);
    }

    // 현재 노드를 해제
    delete node;
}

void CChai::Initialize_Attack_Tree(AttacK_Tree* node)
{
    if (!node)
        return;

    for (AttacK_Tree* child : node->children)
    {
        Initialize_Attack_Tree(child);
    }

    node->bActivate = false;
}

void CChai::Create_Air_Attack_Tree()
{
    m_pTree_Air_Attack = new AttacK_Tree("Root");

    AttacK_Tree* L = new AttacK_Tree("L");
    AttacK_Tree* R = new AttacK_Tree("R");

    m_pTree_Air_Attack->children[ATTACK_TREE_TYPE::L] = L;
    m_pTree_Air_Attack->children[ATTACK_TREE_TYPE::R] = R;

    L->parent = m_pTree_Air_Attack;
    R->parent = m_pTree_Air_Attack;

    /* L */
    AttacK_Tree* L_L = new AttacK_Tree("LL");
    AttacK_Tree* L_R = new AttacK_Tree("LR");

    L->children[ATTACK_TREE_TYPE::L] = L_L;
    L->children[ATTACK_TREE_TYPE::R] = (L_R);

    L_L->parent = L;
    L_R->parent = L;

    /* L L */
    AttacK_Tree* L_L_L = new AttacK_Tree("LLL");

    L_L->children[ATTACK_TREE_TYPE::L] = L_L_L;

    L_L_L->parent = L_L;

    /* L L L */
    AttacK_Tree* L_L_L_L = new AttacK_Tree("LLLL");

    L_L_L->children[ATTACK_TREE_TYPE::L] = L_L_L_L;

    L_L_L_L->parent = L_L_L;

    /* L_R */
    AttacK_Tree* L_R_L = new AttacK_Tree("LRL");
    AttacK_Tree* L_R_R = new AttacK_Tree("LRR");

    L_R->children[ATTACK_TREE_TYPE::L] = L_R_L;
    L_R->children[ATTACK_TREE_TYPE::R] = L_R_R;

    L_R_L->parent = L_R;
    L_R_R->parent = L_R;

    /* L_R_L */
    AttacK_Tree* L_R_L_R = new AttacK_Tree("LRLR");

    L_R_L->children[ATTACK_TREE_TYPE::R] = L_R_L_R;

    L_R_L_R->parent = L_R_L;

    /* L_R_R */
    AttacK_Tree* L_R_R_R = new AttacK_Tree("LRRR");

    L_R_R->children[ATTACK_TREE_TYPE::R] = L_R_R_R;

    L_R_R_R->parent = L_R_R;

    /* R_R */
    AttacK_Tree* R_R = new AttacK_Tree("RR");

    R->children[ATTACK_TREE_TYPE::R] = R_R;

    R_R->parent = R;
}
void CChai::PrepareCallbacks()
{
    auto pRandSound = static_pointer_cast<CChai_Random_Sound>(m_pRandSound);
    Prepare_Callbacks("ch0000_atk-guitar_000");
    Prepare_Callbacks("ch0000_atk-guitar_001");
    Prepare_Callbacks("ch0000_atk-guitar_002");
    Prepare_Callbacks("ch0000_atk-guitar_003");

    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_000", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L();});
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_001", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L();});
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_002", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L();});
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_003", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Sound("LLLL"); });

    Prepare_Callbacks("ch0000_atk-guitar_021");
    Prepare_Callbacks("ch0000_atk-guitar_022");

    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_021", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_R(); });
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_022", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Sound("RRR"); });

    Prepare_Callbacks("ch0000_atk-guitar_024");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_024", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_R(); });

    Prepare_Callbacks("ch0000_atk-guitar_030");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_030", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks("ch0000_atk-guitar_032");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_032", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Sound("LRLL"); });

    Prepare_Callbacks("ch0000_atk-guitar_040");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_040", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_R(); });

    Prepare_Callbacks("ch0000_atk-guitar_041");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_041", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks("ch0000_atk-guitar_053");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_053", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks("ch0000_atk-guitar_080");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_080", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });
    Prepare_Callbacks("ch0000_atk-guitar_081");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_081", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks("ch0000_atk-guitar_093");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_093", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks("ch0000_atk-guitar_094");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_093", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks("ch0000_atk-guitar_095");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_095", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Sound("LRLL"); });

    Prepare_Callbacks("ch0000_atk-guitar_110");
    Prepare_Callbacks("ch0000_atk-guitar_130");
    Prepare_Callbacks("ch0000_atk-guitar_140");

    Prepare_Callbacks("ch0000_atk-jump_010");
    m_pModelCom->SetAnimCallback("ch0000_atk-jump_010", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });
    Prepare_Callbacks("ch0000_atk-jump_011");
    m_pModelCom->SetAnimCallback("ch0000_atk-jump_011", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });
    Prepare_Callbacks("ch0000_atk-jump_012");
    m_pModelCom->SetAnimCallback("ch0000_atk-jump_012", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });
    Prepare_Callbacks("ch0000_atk-jump_013");
    m_pModelCom->SetAnimCallback("ch0000_atk-jump_013", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Sound("AerialRave"); });

    Prepare_Callbacks("ch0000_atk-jump_041");

    Prepare_Callbacks("ch0000_atk-dash_000");
    m_pModelCom->SetAnimCallback("ch0000_atk-dash_000", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });
    Prepare_Callbacks("ch0000_atk-dash_010");
    m_pModelCom->SetAnimCallback("ch0000_atk-dash_000", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_R(); });
    Prepare_Callbacks("ch0000_atk-jump_080");

    Prepare_Callbacks_2("ch0000_atk-guitar_031");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_031", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks_2("ch0000_atk-guitar_051");
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_051", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_L(); });

    Prepare_Callbacks_2("ch0000_atk-guitar_120");
    Prepare_Callbacks_2("ch0000_atk-jump_070");
    m_pModelCom->SetAnimCallback("ch0000_atk-dash_000", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_R(); });
    m_pModelCom->SetAnimCallback("ch0000_atk-dash_000",2, [&]() { m_isAttackOn = true; pRandSound->Play_Random_Attack_Sound_R(); });
    Prepare_Callbacks_2("ch0000_atk-jump_052");

    m_pModelCom->SetAnimCallback("ch0000_atk-jump_053", 0, [&]() { pRandSound->Play_Sound("HBeam"); });

    m_pModelCom->SetAnimCallback("ch0000_atk-jump_081", 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("ch0000_atk-jump_081", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetAnimCallback("ch0000_atk-jump_081", 2, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetLastAnimCallback("ch0000_atk-jump_081", [&]() { m_isAttackOn = false; });

    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_060", 0, [&]() { m_isAttackOn = true; pRandSound->Play_Sound("RLR");  });
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_060", 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_060", 2, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_060", 3, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_060", 4, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("ch0000_atk-guitar_060", 5, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback("ch0000_atk-guitar_060", [&]() { m_isAttackOn = false; });

    m_pModelCom->SetAnimCallback("ch0000_atk-sp_060", 0, [&]() {Create_Just_Effect(3); });
    m_pModelCom->SetAnimCallback("ch0000_atk-sp_200", 0, [&]() {Create_Just_Effect(3); });
    m_pModelCom->SetAnimCallback("ch0000_atk-sp-cp_000", 0, [&]() { Create_Just_Effect(3); });

    m_pModelCom->SetAnimCallback("ch0000_event-mimosa_000", 0, [&]() {m_fHp -= 5.f; if (m_fHp <= 0.f) { m_fHp = 0; } });
    m_pModelCom->SetAnimCallback("ch0000_event-mimosa_000", 1, [&]() {m_fHp -= 5.f; if (m_fHp <= 0.f) { m_fHp = 0; } });
    m_pModelCom->SetAnimCallback("ch0000_event-mimosa_000", 2, [&]() {m_fHp -= 5.f; if (m_fHp <= 0.f) { m_fHp = 0; } });
    m_pModelCom->SetAnimCallback("ch0000_event-rekka_100", 0, [&]() {m_fHp -= 5.f; if (m_fHp <= 0.f) { m_fHp = 0; } });
    m_pModelCom->SetAnimCallback("ch0000_event-rekka_100", 1, [&]() {m_fHp -= 30.f; if (m_fHp <= 0.f) { m_fHp = 0; } });
}

void CChai::Prepare_Callbacks(string strName)
{
    m_pModelCom->SetAnimCallback(strName, 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback(strName, 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback(strName, [&]() { m_isAttackOn = false; });
}

void CChai::Prepare_Callbacks_2(string strName)
{
    m_pModelCom->SetAnimCallback(strName, 0, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback(strName, 1, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetAnimCallback(strName, 2, [&]() { m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback(strName, 3, [&]() { m_isAttackOn = false; });
    m_pModelCom->SetLastAnimCallback(strName, [&]() { m_isAttackOn = false; });
}

void CChai::Play_Just_Effect(_float fTimeDelta)
{
    _float4x4 fMat = {};
    if (m_strEffectName == "Combo_LRR_3_2")
    {
        m_fEffectTimer += fTimeDelta;
        auto pRandSound = static_pointer_cast<CChai_Random_Sound>(m_pRandSound);
        if (m_fEffectTimer > CBeat_Manager::Get_Instance()->Get_Beat() && m_iEffectNumber == 0)
        {
            pRandSound->Play_Sound("LRR");
            m_iEffectNumber++;
            XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(45.f)) * XMLoadFloat4x4(&m_fEffectMatrix));
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_LRR_3_2", &fMat, m_fTime_Offset);
            m_fEffectTimer = 0.f;
        }
        else if (m_fEffectTimer > CBeat_Manager::Get_Instance()->Get_Beat() && m_iEffectNumber == 1)
        {
            pRandSound->Play_Sound("LRR");
            m_iEffectNumber++;
            XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(5.f)) * XMLoadFloat4x4(&m_fEffectMatrix));
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_LRR_3_2", &fMat, m_fTime_Offset);
            m_fEffectTimer = 0.f;
        }
        else if (m_fEffectTimer > CBeat_Manager::Get_Instance()->Get_Beat() && m_iEffectNumber == 2)
        {
            pRandSound->Play_Sound("LRR");
            m_iEffectNumber++;
            XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(-30.f)) * XMLoadFloat4x4(&m_fEffectMatrix));
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_LRR_3_2", &fMat, m_fTime_Offset);
            m_fEffectTimer = 0.f;
        }
        else if (m_fEffectTimer > CBeat_Manager::Get_Instance()->Get_Beat() * 0.5f && m_iEffectNumber == 3)
        {
            pRandSound->Play_Sound("LRR");
            XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(-45.f)) * XMLoadFloat4x4(&m_fEffectMatrix));
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_LRR_3_2", &fMat, m_fTime_Offset);
            m_fEffectTimer = 0.f;
            m_iEffectNumber = 0;
            m_strEffectName = "";
            XMStoreFloat4x4(&m_fEffectMatrix, XMMatrixIdentity());
        }
    }
    else if (m_strEffectName == "Combo_RLL_4_1")
    {
        m_fEffectTimer += fTimeDelta;
        if (m_fEffectTimer > CBeat_Manager::Get_Instance()->Get_Beat())
        {
            m_fEffectMatrix = *m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
            XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(0.f)) * XMLoadFloat4x4(&m_fEffectMatrix));
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_RLL_4_1", &fMat, m_fTime_Offset);
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_RLL_4_2", &fMat, m_fTime_Offset, true);
            m_fEffectTimer = 0.f;
            m_iEffectNumber = 0;
            m_strEffectName = "";
            XMStoreFloat4x4(&m_fEffectMatrix, XMMatrixIdentity());
        }
    }
    else if (m_strEffectName == "Combo_LLLL_4_2")
    {
        if (m_fEffectTimer == 0.f)
        {
            m_iEffectNumber++;
            //XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(0.f)) * XMLoadFloat4x4(&m_fEffectMatrix));
            XMStoreFloat4x4(&fMat, XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMLoadFloat4x4(&m_fEffectMatrix));
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_LLLL_4_2", &fMat, m_fTime_Offset);
        }
        m_fEffectTimer += fTimeDelta;
        if (m_fEffectTimer > CBeat_Manager::Get_Instance()->Get_Beat() && m_iEffectNumber == 1)
        {
            _matrix mMat = XMLoadFloat4x4(&m_fEffectMatrix);
            _vector vPos = mMat.r[3];
            vPos += m_pTransformCom->Get_NormLook() * 4.f;
            mMat.r[3] = vPos;
            //XMStoreFloat4x4(&fMat, XMMatrixRotationY(XMConvertToRadians(0.f)) * mMat);
            XMStoreFloat4x4(&fMat, XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMLoadFloat4x4(&m_fEffectMatrix));
            EFFECTMANAGER->Call_Fixed_Union_Effect("Combo_LLLL_4_2", &fMat, m_fTime_Offset);
            m_fEffectTimer = 0.f;
            m_iEffectNumber = 0;
            m_strEffectName = "";
            XMStoreFloat4x4(&m_fEffectMatrix, XMMatrixIdentity());
        }
    }
}

void CChai::Create_Just_Effect(_int iBeat)
{
    CUi_CommandIcon::Desc Desc;
    Desc.CorrectBeat = iBeat;
    Desc.fX = 1;
    Desc.fY = 0;
    Desc.eRenderType = CUi::RenderType::Priority;
    m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi", CUi_CommandIcon::ObjID, &Desc);
}

void CChai::Tick_PartObj(_float fTimeDelta)
{
    for (auto& pPartObj : m_pWeapon)
        pPartObj->Tick(fTimeDelta);
}

PxExtendedVec3 CChai::Get_Foot_Position()
{
    return m_pCCT->Get_FootPosition();
}

void CChai::Catch_Item(string strItemName, _int iSize)
{
    if (strItemName == "Gear")
    {
        _int iGear = 0;
        switch (iSize)
        {
        case 0:
            iGear = 100;
            break;
        case 1:
            iGear = 500;
            break;
        case 2:
            iGear = 1000;
            break;
        default:
            break;
        }
        CPlayer_Manager::Get_Instance()->Add_PlayerGear(iGear);
    }
    else if (strItemName == "Battery")
    {
        m_bRimLight = true;
        m_fRimTimer = 1.f;
        m_fRimColor = { 1.f,0.5f,0.f,0.3f };

        switch (iSize)
        {
        case 0:
            m_fReverb += 5.f;
            break;
        case 1:
            m_fReverb += 15.f;
            break;
        case 2:
            m_fReverb += 30.f;
            break;
        default:
            break;
        }
        if (m_fReverb > m_fMaxReverb)
            m_fReverb = m_fMaxReverb;
        CPlayer_Manager::Get_Instance()->Set_Reverb(m_fReverb);
    }
    else if (strItemName == "Hp")
    {
        m_bRimLight = true;
        m_fRimTimer = 1.f;
        m_fRimColor = { 0.f,1.f,0.f,0.3f };
        switch (iSize)
        {
        case 0:
            m_fHp += 10.f;
            break;
        case 1:
            m_fHp += 15.f;
            break;
        case 2:
            m_fHp += 30.f;
            break;
        default:
            break;
        }
        if (m_fHp > m_fMaxHp)
            m_fHp = m_fMaxHp;
    }
}

void CChai::Event_Tick(_float fTimeDelta)
{
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * m_fTimeMultiflier;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f * m_fTimeMultiflier;
    _float Three_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.3333f) * m_fTimeMultiflier;
    _float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f * m_fTimeMultiflier;
    _float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f) * m_fTimeMultiflier;
    _float Six_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f) * m_fTimeMultiflier;
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f) * m_fTimeMultiflier;

    if (m_strEvent == "R")
    {
        m_pPlayer_Camera->Shake(0.1f, 0.1f);
        m_strEvent = "";
    }
    if (m_strEvent == "RR")
    {
        m_pPlayer_Camera->Shake(0.1f, 0.1f);
        m_strEvent = "";
    }

    if (m_strEvent == "RRR")
    {
        EFFECTMANAGER->Change_Union_Effect_Speed("Combo_RRR_3_1", Six_Beat);
        m_fEvent_Timer += fTimeDelta;
        if (m_fEvent_Timer > CBeat_Manager::Get_Instance()->Get_Beat() && !(m_fTimeMultiflier < 1.f))
        {
            m_fEvent_Timer = 0.f;
            m_strEvent = "";
            m_pPlayer_Camera->Shake(0.2f, 0.2f);
            m_fTimeMultiflier = 1.f;
            m_fTime_Offset = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f) * m_fTimeMultiflier;
            EFFECTMANAGER->Change_Union_Effect_Speed("Combo_RRR_3_1", m_fTime_Offset);
        }
    }
    else if (m_strEvent == "LL_LLL")
    {
        EFFECTMANAGER->Change_Union_Effect_Speed("Combo_LL_LLL_5_1", Five_Beat);
        m_fEvent_Timer += fTimeDelta;
        if (m_fEvent_Timer > CBeat_Manager::Get_Instance()->Get_Beat() && !(m_fTimeMultiflier < 1.f))
        {
            m_fEvent_Timer = 0.f;
            m_strEvent = "";
            m_pPlayer_Camera->Shake(0.2f, 0.2f);
            m_fTimeMultiflier = 1.f;
            m_fTime_Offset = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f) * m_fTimeMultiflier;
            EFFECTMANAGER->Change_Union_Effect_Speed("Combo_LL_LLL_5_1", m_fTime_Offset);
        }
    }

    else if (m_strEvent == "LargeElevator")
    {
        m_bIsEvent = true;
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_00")));
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0, 0));
        m_pCCT->Disable_Gravity();
        m_pModelCom->Set_AnimIndexNonCancle("EV8110_01_000_ch0000_00", false);
    }
    else if (m_strEvent == "LargeElevator_End")
    {
        m_bRootMove_On = true;
        m_pCCT->Enable_Gravity();
        m_pCCT->Set_Velocity({ 0, 0, 0, 0 });
        m_fEvent_Timer = 0.f;
        m_strEvent = "";
        m_fEventPosition = {};
    }
    else if (m_strEvent == "Gear_Wall_End")
    {
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_bIsEvent = true;
        m_fEvent_Timer += fTimeDelta;
        if (m_fEvent_Timer < fBeat)
        {
            _vector vDir = XMVector3Normalize(XMLoadFloat4(&m_fEventPosition) - m_pTransformCom->Get_Position());
            m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + vDir * 10.f * fTimeDelta);
            m_pCCT->Set_Position(m_pTransformCom->Get_WorldMatrix_XMMat());
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_001", false);
        }
    }
    else if (m_strEvent == "Battle_Transition")
    {
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_pModelCom->Set_AnimIndexNonCancle("EV2000_00_000_ch0000_00", false);
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Change_Event_Animation("EV2000_00_000_wp0011_00");
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(1.f);
    }
    else if (m_strEvent == "2D_Map_Start")
    {
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-st01_020", false);

        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Transfer2D");
        m_pPlayer_Camera->Shake(m_pModelCom->Get_Animation_PlayTime(), 0.1f);
        m_strEvent = "";
    }
    else if (m_strEvent == "Hanging_Platform")
    {
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-st01_", false);
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_strEvent = "";
    }
    else if (m_strEvent == "Fade_Elevator")
    {
        GAMEMANAGER->Dissolve(2.f);
        m_pPlayer_Camera->End_2DScroll();
        m_b2D_Mode = false;
        Set_Player_Position(XMVectorSet(84.1956f, 53.9834f, 647.496f, 1));
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_strEvent = "";
    }
    else if (m_strEvent == "SignBoard_Platform")
    {
        m_fEvent_Timer += fTimeDelta;
        if (m_fEvent_Timer < 1.f)
            m_f2D_FixedPos -= fTimeDelta * 2.f;
        else if (m_fEvent_Timer > 1.f)
        {
            m_fEvent_Timer = 0.f;
            m_strEvent = "";
        }
        /*  m_f2D_FixedPos = -315.686f;*/
    }

    else if (m_strEvent == "Fade_Elevator_End")
    {
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        m_strEvent = "";
    }
    else if (m_strEvent == "Shopping_Zone")
    {
        m_bInteraction = true;
        m_strEvent = "";
    }
    else if (m_strEvent == "Shopping_Start")
    {
        m_pPlayer_Camera->Camera_Fixed(m_pTransformCom, m_pTransformCom->Get_NormLook(), m_pPlayer_Camera->Get_Original_DistanceTarget());
        static_pointer_cast<C808_Controller>(PLAYERMANAGER->Get_808_Controller())->Active_808_Shop_Animation();
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_strEvent = "";
    }
    else if (m_strEvent == "Shopping_End")
    {
        static_pointer_cast<C808_Controller>(PLAYERMANAGER->Get_808_Controller())->Active_808_Shop_Animation_End();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        m_strEvent = "";
    }
    else if (m_strEvent == "Lift_Macaron")
    {
        m_bLift_Macaron = true;
        m_strEvent = "";
    }
    else if (m_strEvent == "Generator_Start")
    {
        m_bInteraction = true;
        m_strEvent = "";
    }
    else if (m_strEvent == "Generator")
    {
        m_eEvent_State = CHAI_EVENT_STATE::GENERATOR;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_000", false);
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_strEvent = "";
    }
    else if (m_strEvent == "Generator_Finish")
    {
        if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_001") == m_pModelCom->Get_CurrentAnimIndex())
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_002", false);
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_101") == m_pModelCom->Get_CurrentAnimIndex())
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_102", false);
        else if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_201") == m_pModelCom->Get_CurrentAnimIndex())
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_202", false);
        //else
        //{
        //    m_eMoveState = CHAI_MOVE_STATE::IDLE;
        //    Adjust_Idle_Timing();
        //    m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        //}
        //m_pPlayer_Camera->End_Following_Bone();
        m_eEvent_State = CHAI_EVENT_STATE::EVENT_NONE;
        m_strEvent = "";
    }
    else if (m_strEvent == "Generator_End")
    {
        m_bInteraction = false;
        m_strEvent = "";
    }
    else if (m_strEvent == "Lift_Macaron")
        m_bLift_Macaron = true;
    else if (m_strEvent == "Lift_Macaron_End")
        m_bLift_Macaron = false;

    else if (m_strEvent == "Mimosa_RP")
    {
        _vector vLookDir = XMVector3Normalize(XMVectorSet(-10.f, -2.f, 2.f, 1.f) - m_pTransformCom->Get_Position());
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + vLookDir);

        m_bTurning = false;
        m_bIsEvent = true;
        m_eEvent_State = CHAI_EVENT_STATE::RHYTHMPARRY;
        m_bCCT_On = false;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-rp-cp_048", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "Mimosa_RP_End")
    {
        m_bTurning = false;
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
        m_bCCT_On = false;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-rp-cp_MFinish_1", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "Kale_RP")
    {
        m_bTurning = false;
        m_bIsEvent = true;
        m_eEvent_State = CHAI_EVENT_STATE::RHYTHMPARRY;
        m_bCCT_On = false;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_idle_200", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "Kale_RP_End")
    {
        m_bIsEvent = false;
        m_eEvent_State = CHAI_EVENT_STATE::EVENT_NONE;
        m_bCCT_On = true;
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        m_strEvent = "";
    }
    else if (m_strEvent == "Zipline")
    {
        m_ZipLineBlur = CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect_Return("RadialBlur_1000Sec", &m_matCurrentPos, 1, false);
        m_eEvent_State = CHAI_EVENT_STATE::ZIPLINE;
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
        m_pCCT->Disable_Gravity();
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-magnetrail_150", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "Zipline_End")
    {
        if(m_ZipLineBlur)
        {
            m_ZipLineBlur->UnionEffectEnd();
            m_ZipLineBlur = nullptr;
        }
        CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect_Return("RadialBlur_1Sec_Out", &m_matCurrentPos, 1, false);
        m_bWire = false;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-magnetrail_060", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "Mimosa_SpotLight")
    {
        if(!m_SpotLight)
             m_SpotLight = EFFECTMANAGER->Call_Union_Effect_Return("SpotLight_Chai", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
        m_strEvent = "";
    }
    else if (m_strEvent == "Mimosa_SpotLight_End")
    {
        if(m_SpotLight)
             m_SpotLight->UnionEffectEnd();
        m_SpotLight = nullptr;
        m_strEvent = "";
    }
    else if (m_strEvent == "Mimosa_DanceBattle")
    {
        Set_Player_Position({-4.f,-2.f,2.f,1});
        _vector vLookDir = XMVector3Normalize(XMVectorSet(-22.f, -2.f, 2.f, 1.f) - m_pTransformCom->Get_Position());
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + vLookDir);

        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-mimosa_050", true);
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Change_Guitar_State(CBasic_Guitar::GUITAR_STATE::MIMOSA_DANCE);
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Is_Electric_Guitar(true);
        m_eEvent_State = CHAI_EVENT_STATE::MIMOSA_DANCE;
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_strEvent = "";
    }
    else if (m_strEvent == "Mimosa_DanceBattle_End")
    {
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Change_Guitar_State(CBasic_Guitar::GUITAR_STATE::NORMAL);
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Is_Electric_Guitar(false);
        m_bIsEvent = false;
        m_eEvent_State = CHAI_EVENT_STATE::EVENT_NONE;
        m_bCCT_On = true;
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        m_strEvent = "";
    }
    else if (m_strEvent == "PeppermintContainer")
    {
        m_bIsEvent = true;
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() - m_pTransformCom->Get_NormLook());
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-st11_010", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "GiantRobot_1_End")
        m_strEvent = "";
    else if (m_strEvent == "GiantRobot_1")
    {
        if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_140"))
            return;
        m_bWire = false;
        m_pCCT->Set_Velocity({ 0,0,0,0 });
        m_bIsEvent = true;
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() - m_pTransformCom->Get_NormLook());
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-st11_020", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "GiantRobot_2")
    {
        m_bWire = false;
        m_pCCT->Set_Velocity({ 0,0,0,0 });
        m_bIsEvent = true;
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + m_pTransformCom->Get_NormLook());
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-st11_050", false);
        m_strEvent = "";
    }
    else if (m_strEvent == "RhythmJump")
    {
       _float fBeat = BEATMANAGER->Get_Beat();
       _float fTimer =  BEATMANAGER->Get_Timer();

       if ((fTimer < fBeat * 0.2f || fTimer > fBeat * 0.8f) && GAMEINSTANCE->Get_KeyDown(DIK_SPACE))
       {
           CUi_CommandIcon::Desc Desc;
           Desc.CorrectBeat = 3;
           Desc.fX = 1;
           Desc.fY = 0;
           Desc.eRenderType = CUi::RenderType::Priority;
           m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi", CUi_CommandIcon::ObjID, &Desc);
           m_pCCT->Set_Velocity(XMVectorSet(0.f, 40.f, 0, 0));
           m_eMoveState = CHAI_MOVE_STATE::JUMP;
           m_eEvent_State = CHAI_EVENT_STATE::JUMPPAD;
           m_bIsEvent = true;
           m_strEvent = "";
       }

    }
    else if (m_strEvent == "RhythmJump_End")
        m_strEvent = "";
    else if (m_strEvent == "Spectra_2D_1")
    {
        m_f2D_FixedPos = 130.f;
        m_strFixedXYZ = "Z";
        m_strEvent = "";
    }
    else if (m_strEvent == "Spectra_2D_0")
    {
        m_f2D_FixedPos = 30.75f;
        m_strFixedXYZ = "X";
        m_strEvent = "";
    }
    else if (m_strEvent == "Rhythm_Parry_End")
    {
      
        m_bTurning = false;
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
        m_bCCT_On = false;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-rp-cp_002", false);
        m_strEvent = "";
    }
}

void CChai::Zoom_Tick(_float fTimeDelta)
{
    if (m_bZoomIn)
    {
        m_fZoomIn_Timer -= fTimeDelta;
        if (m_fZoomIn_Timer < fTimeDelta)
        {
            m_pPlayer_Camera->Set_DistanceTarget(m_pPlayer_Camera->Get_Original_DistanceTarget());
            m_fZoomIn_Timer = 0.f;
            m_bZoomIn = false;
        }
    }
}

void CChai::Zoom_In(_float fDuration, _float fLinearZoom)
{
    m_bZoomIn = true;
    m_fZoomIn_Timer = fDuration;
    m_pPlayer_Camera->Set_DistanceTarget(m_pPlayer_Camera->Get_Original_DistanceTarget() * fLinearZoom);
}

shared_ptr<CChai> CChai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CChai
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CChai(pDevice, pContext)
        {}
    };

    shared_ptr<CChai> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CChai"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CChai::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CChai
    {
        MakeSharedEnabler(const CChai& rhs) : CChai(rhs)
        {}
    };

    shared_ptr<CChai> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CChai"));
        assert(false);
    }

    return pInstance;
}

void CChai::Free()
{
    delete m_pTree_Attack;
    delete m_pTree_Air_Attack;
}

void CChai::Animation_End()
{
    _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_080") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_fIdle_Motion_Offset = 0.f;
        m_fAttack_Motion_Offset = 0.f;
        m_eMoveState = CHAI_MOVE_STATE::RUN;
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_mvnt-run_000", true);
    }
    if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_023") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_bCommandIcon = false;
        _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
        m_fAttack_Motion_Offset = fTimer;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_024", false);
    }
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_050") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_060") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-cp_000") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_200") == m_pModelCom->Get_CurrentAnimIndex()
        )
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.95f)
        m_pPlayer_Camera->End_Following_Bone();

    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_050") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_060") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-cp_000") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_200") == m_pModelCom->Get_CurrentAnimIndex()
        )
        && m_pModelCom->Get_Finished())
    {
        if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_060") == m_pModelCom->Get_CurrentAnimIndex())
            m_bRender_Can = false;

        m_bCommandIcon = false;
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For Level 11*/
    if (
        m_pModelCom->Get_StringAnimIndex("ch0000_event-st11_050") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_bIsEvent = false;
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-st11_010") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-st11_020") == m_pModelCom->Get_CurrentAnimIndex() )
        && m_pModelCom->Get_Finished())
    {
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() - m_pTransformCom->Get_NormLook());
        m_bIsEvent = false;
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For Mimosa_Dance*/
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_010") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_020") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_030") == m_pModelCom->Get_CurrentAnimIndex() || 
        m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_040") == m_pModelCom->Get_CurrentAnimIndex() || 
        m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_060") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_event-mimosa_050", true);
    /* For Mimosa_RP*/
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_048") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_049") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-kale_111") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_idle_200", true);
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_002") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Current_Track_Position() > 59.f)
    {
        m_bCCT_On = true;
        m_pPlayer_Camera->Change_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_pModelCom->Set_AnimIndex("ch0000_atk-rp-cp_005", false);
    }

    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_MFinish_1") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        if(m_bRPMode)
        {
            m_bCCT_On =  true;
            m_pModelCom->Set_AnimIndex("ch0000_atk-rp-cp_005", false);
        }
        else
        {
            m_bCCT_On = true;
            m_pModelCom->Set_AnimIndex("ch0000_atk-rp-cp_050", false);
        }
    }
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_005") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_pPlayer_Camera->End_Following_Bone();
        m_eEvent_State = CHAI_EVENT_STATE::EVENT_NONE;
        m_bRPMode = false;
        m_bIsEvent = false;
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For Generator*/
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_000") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_event-generator_001", true);

    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_100") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_event-generator_101", true);

    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_200") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_event-generator_201", true);

    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_002") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_102") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_202") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_010") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_020") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_pPlayer_Camera->End_Following_Bone();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For MagneTrail */
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_150") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_010") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_011") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_040") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_event-magnetrail_000", true);
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_060") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_eEvent_State = CHAI_EVENT_STATE::EVENT_NONE;
        m_pCCT->Enable_Gravity();
        m_eMoveState = CHAI_MOVE_STATE::JUMP_FALL;
        m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_001", true);
    }
    /* For Battle Transition*/
    if ((m_pModelCom->Get_StringAnimIndex("EV2000_00_000_ch0000_00") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_strEvent = "";
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();

        CCombat_Manager::Get_Instance()->Set_Trasnition_End(true);
    }
    /* For 2D Event End */
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-st01_") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_strEvent = "";
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-rhythm_000") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp-rhythm_001", false);
    else if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-rhythm_001") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp-rhythm_002", false);
    else if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-rhythm_002") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Current_Track_Position() > 15.f && !m_bODS)
    {
        m_bODS = true;
        auto pGI = m_pGameInstance.lock();
        _int iBeat = 5;
        _float4x4 EffectMat;
        XMStoreFloat4x4(&EffectMat, XMMatrixTranslation(0.f,1.5f,2.f) * m_pTransformCom->Get_WorldMatrix_XMMat());
        pGI->Push_Object_From_Pool(CODS_Controller::ObjID, pGI->Get_Current_LevelID(), L"Effect", &iBeat, &EffectMat);
    }
    else if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-rhythm_002") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Current_Track_Position() > 100.f)
    {
        m_pPlayer_Camera->End_Following_Bone();
        if(m_pModelCom->Get_Current_Track_Position() > 105.f)
        {
            CUi_Mgr::Get_Instance()->Ui_AllActive(true);
            m_bAttack_Input = false;
            m_bNextAttack = false;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_eMoveState = CHAI_MOVE_STATE::IDLE;
            Adjust_Idle_Timing();
            m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
            m_bODS = false;
        }
    }


    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_000") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_strEvent = "";
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() - m_pTransformCom->Get_NormLook());
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For Rekka WWE */
    if (m_pModelCom->Get_StringAnimIndex("ch0000_event-rekka_100") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_strEvent = "";
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For 2D_Squence_Intro */
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_event-st01_020") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        GAMEMANAGER->Dissolve(1.f);
        m_f2D_FixedPos = -313.686f;
        m_strFixedXYZ = "X";
        Set_Player_Position(XMVectorSet(-313.891f, 87.360f, -56.430f, 1));
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For Big_Jump_Intro */
    if ((m_pModelCom->Get_StringAnimIndex("EV8110_01_000_ch0000_00") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_bIsEvent = false;
        m_pPlayer_Camera->End_Following_Bone();
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* For CombatIntro */
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_action_051") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_action_054") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        static_pointer_cast<CUi_PlayerBar>(CUi_Mgr::Get_Instance()->Find_UI(CUi_PlayerBar::ObjID))->Fight_Mode();
        GAMEINSTANCE->Add_Clone(GAMEINSTANCE->Get_Current_LevelID(), L"CUi", CUi_FightString::ObjID);
        CCombat_Manager::Get_Instance()->Set_Combat_Engage(false);
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* FOR First JUMP*/
    if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_000") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::JUMP_FALL;
        m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_001", true);
    }
    /* FOR SECOND JUMP*/
    if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_010") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::DOUBLE_JUMP_FALL;
        m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_011", true);
    }
    if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_020") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::JUMP_FALL;
        m_pModelCom->Set_AnimIndex("ch0000_mvnt-escape_021", true);
    }
    if (m_eEvent_State == CHAI_EVENT_STATE::RHYTHMPARRY && m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_030")
        && m_pModelCom->Get_Finished())
    {
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    if (m_pModelCom->Get_StringAnimIndex("ch0000_action_104") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_pCCT->Enable_Gravity();
        m_eMoveState = CHAI_MOVE_STATE::DOUBLE_JUMP_FALL;
        m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_011", true);
    }
    else if (m_pModelCom->Get_StringAnimIndex("ch0000_action_140") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.5f )
    {
        if (m_pModelCom->Get_Finished())
        {

            m_pTransformCom->Set_State(CTransform::STATE_UP, (XMVectorSet(0, 1, 0, 0)));
            m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Cross( m_pTransformCom->Get_NormRight(), m_pTransformCom->Get_NormUp()));
            m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(m_pTransformCom->Get_NormUp(),m_pTransformCom->Get_NormLook()));

            m_eMoveState = CHAI_MOVE_STATE::IDLE;
            Adjust_Idle_Timing();
            m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        }
        else
            m_pCCT->Enable_Gravity();
    }
    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_130")
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.67f)
    {
        if (m_bGimmicWire == true)
        {
            m_bTurning = false;
            auto wireTarget =  dynamic_pointer_cast<CBuiltIn_Magnet>(m_pWire_Target);
            if (wireTarget)
            {
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() - m_pWire_Target->Get_Transform()->Get_NormLook());
                _matrix wireMatPos = m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat();
                wireMatPos.r[3] -= m_pWire_Target->Get_Transform()->Get_NormLook() * 0.5f;
                Set_Player_Position(wireMatPos.r[3]);
                m_pCCT->Disable_Gravity();
                if (wireTarget->Is_ComboMagnet())
                {
                    m_pModelCom->Set_AnimIndex("ch0000_action_104", false);
                    m_pComboWire_Target = wireTarget->Get_NextMagnet();
                }
                else
                    m_pModelCom->Set_AnimIndex("ch0000_action_140", false);
            }
            else
            {
                m_pCCT->Set_Velocity({ 0,0,0,0 });
                m_pModelCom->Set_AnimIndex("ch0000_action_104", false);
            }
            m_bWire = false;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
        }
        else
        {
            m_bWire = false;
            m_pCCT->Set_Velocity({ 0,0,0,0 });
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
            if (m_bOnAir)
            {
                m_eMoveState = CHAI_MOVE_STATE::DOUBLE_JUMP_FALL;
                m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_011", true);
            }
            else
            {
                m_eMoveState = CHAI_MOVE_STATE::IDLE;
                Adjust_Idle_Timing();
                m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
            }
        }
    }
    else if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_053") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_002") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_095") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_140") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_041") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_082") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_082") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_000") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_010") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_action_030") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_action_031") == m_pModelCom->Get_CurrentAnimIndex()
        )
        && m_pModelCom->Get_Finished())
    {
        m_bWire = false;
        m_fWire_Target_Distance = 0.f;
        m_fWire_Target_Length = 0.f;
        m_pWire_Target = nullptr;
        m_pPlayer_Camera->Set_DistanceTarget(m_pPlayer_Camera->Get_Original_DistanceTarget());
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_060") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.909f)
    {
        m_bCommandIcon = false;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    else if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_000") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_001") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_002") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_003") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_003") == m_pModelCom->Get_CurrentAnimIndex() ||

        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_000") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_001") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_002") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_003") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_030") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_031") == m_pModelCom->Get_CurrentAnimIndex() ||

        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_040") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_080") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_081") == m_pModelCom->Get_CurrentAnimIndex() ||

        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_094") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_093") == m_pModelCom->Get_CurrentAnimIndex() ||

        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_051") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_031") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_032") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_030") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_022") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_021") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_024") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.85f)
    {
        m_bCommandIcon = false;
        m_iDashCount = 0;
        m_eNextAttack = ATTACK_TYPE_END;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    if (
        (m_pModelCom->Get_StringAnimIndex("ch0000_damage_060") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_damage_061", false);
    else if (
        (m_pModelCom->Get_StringAnimIndex("ch0000_damage_061") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_damage_062", false);
    if (
        (m_pModelCom->Get_StringAnimIndex("ch0000_damage_040") == m_pModelCom->Get_CurrentAnimIndex() ||
            m_pModelCom->Get_StringAnimIndex("ch0000_damage_062") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Is_Render(true);
        m_bCommandIcon = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    if (
        m_pSans_ModelCom->Get_StringAnimIndex("ch0000_damage_040") == m_pSans_ModelCom->Get_CurrentAnimIndex()
        && m_pSans_ModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.5f)
    {
        m_bIsSans = false;
    }
    if (
        (m_pModelCom->Get_StringAnimIndex("ch0000_damage_000") == m_pModelCom->Get_CurrentAnimIndex() ||
            m_pModelCom->Get_StringAnimIndex("ch0000_damage_010") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        if(m_bRPMode)
        {
            m_bCCT_On = false;
            m_eEvent_State = CHAI_EVENT_STATE::RHYTHMPARRY;
            m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
            m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_idle_200", true);
        }
        else
        {
            m_bCommandIcon = false;
            m_bNextAttack = false;
            m_fAttack_Motion_Offset = 0.f;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
            m_eMoveState = CHAI_MOVE_STATE::IDLE;
            Adjust_Idle_Timing();
            m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        }
    }

    /* Air Motion End */
    if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_053") == m_pModelCom->Get_CurrentAnimIndex()
        || m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_120") == m_pModelCom->Get_CurrentAnimIndex()
        )
        && m_pModelCom->Get_Finished())
    {
        m_bCommandIcon = false;
        m_bUpperAttack = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::JUMP_FALL;
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-jump_001", true);
    }
    else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_081") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_bCommandIcon = false;
        m_bUpperAttack = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_082", true);
    }
    else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_006") == m_pModelCom->Get_CurrentAnimIndex()
        && m_pModelCom->Get_Finished())
    {
        m_isAttackOn = true;
        _float3 fVel = { 0.f,-30.f,0.f };
        m_pCCT->Set_Velocity(fVel);
        m_bCommandIcon = false;
        m_bUpperAttack = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_007", true);
    }
    else if ((m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_008") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_083") == m_pModelCom->Get_CurrentAnimIndex())
        && m_pModelCom->Get_Finished())
    {
        m_bCommandIcon = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
    /* Air Motion End */
    else if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_053") != m_pModelCom->Get_CurrentAnimIndex() &&
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_006") != m_pModelCom->Get_CurrentAnimIndex() &&
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_007") != m_pModelCom->Get_CurrentAnimIndex() &&
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_008") != m_pModelCom->Get_CurrentAnimIndex() &&
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_081") != m_pModelCom->Get_CurrentAnimIndex() &&
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_082") != m_pModelCom->Get_CurrentAnimIndex() &&
        m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_083") != m_pModelCom->Get_CurrentAnimIndex() &&
        (m_eMoveState == CHAI_MOVE_STATE::AIR_ATTACK)
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.85f)
    {
        m_bCommandIcon = false;
        m_bL_Rest_UpperAttack = false;
        m_bUpperAttack = false;
        m_bNextAttack = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
        Adjust_Idle_Timing();
        m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
    }
}

void CChai::Animation(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    /* 반박자에 끝나는 모션 ex) 단타 공격모션 */
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * m_fTimeMultiflier;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f * m_fTimeMultiflier;
    _float Three_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.3333f) * m_fTimeMultiflier;
    _float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f * m_fTimeMultiflier;
    _float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f) * m_fTimeMultiflier;
    _float Six_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f) * m_fTimeMultiflier;
    _float Seven_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.142f) * m_fTimeMultiflier;
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f) * m_fTimeMultiflier;
    _float Ten_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1f) * m_fTimeMultiflier;

    _uint index = m_pModelCom->Get_CurrentAnimIndex();

    _float rhythmOffset = false;
    _float animationOffset = 1.f;
    /* 가장 가까운 박자에 맞춰서 빨리 재생 */

    _float leftBeatTime = fBeat - m_fAttack_Motion_Offset;
    _float leftIdleBeatTime = fBeat - m_fIdle_Motion_Offset;
    if (m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.5f)
    {
        animationOffset += (leftBeatTime / fBeat) * 0.5f;
        rhythmOffset = true;
    }

    if (m_pSans_ModelCom->Get_CurrentAnimIndex() == m_pSans_ModelCom->Get_StringAnimIndex("ch0000_damage_040") ||
        m_pSans_ModelCom->Get_CurrentAnimIndex() == m_pSans_ModelCom->Get_StringAnimIndex("ch0000_damage_060") ||
        m_pSans_ModelCom->Get_CurrentAnimIndex() == m_pSans_ModelCom->Get_StringAnimIndex("ch0000_damage_061"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_pSans_ModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Two_Beat;
    }
    else if (m_pSans_ModelCom->Get_CurrentAnimIndex() == m_pSans_ModelCom->Get_StringAnimIndex("ch0000_damage_062"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(One_Beat);
        m_pSans_ModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f, fmatResult);
        m_fTime_Offset = One_Beat;
    }
    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_idle_000"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Four_Beat);
        if (!rhythmOffset)
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
        else
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Four_Beat;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_damage_040"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Two_Beat;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_damage_000"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Four_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Four_Beat;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_080"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f, fmatResult);
    /* EVENT Level 11 */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-st11_050"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Seven_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Seven_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Seven_Beat;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-st11_020"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.05f) * m_fTimeMultiflier);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * (m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.05f) * m_fTimeMultiflier), 0.1f, fmatResult);
        m_fTime_Offset = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.05f) * m_fTimeMultiflier;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-st11_010"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Five_Beat;
    }
    /* Magnetrail */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_150"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Five_Beat;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_020") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_060"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Two_Beat;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_040"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Eight_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Eight_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Eight_Beat;
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_010") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_011"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Six_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Six_Beat;
    }
    /* Battle_Transition */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV2000_00_000_ch0000_00"))
    {
        _float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

        _float fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(fBeatSpeed);
        m_fTime_Offset = fBeatSpeed;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fBeatSpeed, 0.1f, fmatResult);
    }
    /* Event_Animation */
    /* Kale Finish Event */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1250_80_000_ch0000_00") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1250_80_001_ch0000_00") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1250_80_003_ch0000_00") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1250_80_004_ch0000_00") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1250_80_005_ch0000_00") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1250_80_006_ch0000_00") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV1250_80_007_ch0000_00")
        )
    {
        _float fTotalBeat = m_pModelCom->Get_Duration() / 20.f;

        _float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

        _float fBeatSpeed = fPlayTime / fBeat / fTotalBeat;

        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(fBeatSpeed);
        m_fTime_Offset = fBeatSpeed;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fBeatSpeed, 0.1f, fmatResult);
    }
    /* Kale Event Rp */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-kale_111"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(One_Beat);
        m_fTime_Offset = One_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f, fmatResult);
    }
    /* Mimosa Dance */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_010") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_020") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_030") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_040") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_060"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_fTime_Offset = Two_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_050"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Four_Beat);
        m_fTime_Offset = Four_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    }
    /* Mimosa_RP*/

    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_048") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_049"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.0625f);
        m_fTime_Offset = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.0625f;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.0625f * m_fTimeMultiflier, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_MFinish_1"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Three_Beat);
        m_fTime_Offset = Three_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Three_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_002"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Six_Beat);
        m_fTime_Offset = Three_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_005"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_fTime_Offset = Three_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-rp-cp_050"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_fTime_Offset = Two_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_idle_200"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_fTime_Offset = Two_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
    }
    /* Generator */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_001") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_002") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_101") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_201"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_fTime_Offset = Two_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_010") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_020"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Four_Beat);
        m_fTime_Offset = Four_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_100") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_102") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_200") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_202"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(One_Beat);
        m_fTime_Offset = One_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f, fmatResult);
    }
    /* Stge1 2D Fall*/
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-st01_"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(1.f);
        m_fTime_Offset = 1.f;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_000"))
    {
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
        m_pCCT->Disable_Gravity();

        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.071f) * m_fTimeMultiflier);
        m_fTime_Offset = fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.071f) * m_fTimeMultiflier;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.071f) * m_fTimeMultiflier, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-rekka_100"))
    {
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
        m_pCCT->Disable_Gravity();

        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(1.f);
        m_fTime_Offset = 1.f;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-st01_020"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(1.f);
        m_fTime_Offset = 1.f;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("EV8110_01_000_ch0000_00"))
    {
        m_pPlayer_Camera->PriorityTick(fTimeDelta);
        GAMEINSTANCE->Update_Pipeline();
        m_fTime_Offset = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.0666f) * m_fTimeMultiflier;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.0666f) * m_fTimeMultiflier, 0.1f, fmatResult);
    }
    else if ( 
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-rhythm_000"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(One_Beat);
        m_fTime_Offset = One_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f, fmatResult);
    }
    else if (
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-rhythm_001"))
    {
        for (auto& iter : CCombat_Manager::Get_Instance()->Get_Enemy_List())
        {
            iter->Set_Time_Multiflier(0.2f);
        }
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Ten_Beat);
        m_fTime_Offset = Ten_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Ten_Beat, 0.1f, fmatResult);
    }
    else if (
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-rhythm_002"))
    {
            for (auto& iter : CCombat_Manager::Get_Instance()->Get_Enemy_List())
            {
                iter->Set_Time_Multiflier(1.f);
            }
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Seven_Beat);
        m_fTime_Offset = Seven_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Seven_Beat, 0.1f, fmatResult);
    }
    /* Combat Intro */
    else if (
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_051") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_054")
        )
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);
        m_fTime_Offset = Five_Beat;
    }
    /* Attack Animation*/
    else if ( // 강공격 1타 준비모션 전용
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_023"))
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(One_Beat);
            m_fTime_Offset = One_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * One_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    else if (// 강공격 1타 공격 전용
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_024")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Five_Beat);
            m_fTime_Offset = Five_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    else if ( // 5박 공격

        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_051")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Five_Beat);
            m_fTime_Offset = Five_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    else if ( // LR
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_040")
        )
    {
        _float fOffsetTime = fBeat - CBeat_Manager::Get_Instance()->Get_Timer();

        if (m_pModelCom->Get_Duration() < 20.f)

        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed((1.f + fOffsetTime / fBeat) * m_fTimeMultiflier);
            m_fTime_Offset = 1.f + fOffsetTime / fBeat * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * (1.f + fOffsetTime / fBeat)) * m_fTimeMultiflier, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Six_Beat);
            m_fTime_Offset = Six_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat), 0.1f, fmatResult);
        }
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_104"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_fTime_Offset = Two_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Two_Beat) * animationOffset, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_140"))
    {
        m_pCCT->Set_Velocity({ 0,0,0,0 });
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_fTime_Offset = Two_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Two_Beat) * animationOffset, 0.1f, fmatResult);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_130"))
    {
        _float4x4 matBone = {};
        XMStoreFloat4x4(&matBone, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("r_hand")) * m_pTransformCom->Get_WorldMatrix_XMMat());

        _float4x4 mat_Wire_Bone = {};
        _matrix wireMatPos = m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat();
        if (!m_bGimmicWire)
            XMStoreFloat4x4(&mat_Wire_Bone, XMLoadFloat4x4(static_pointer_cast<CModel>(m_pWire_Target->Find_Component(TEXT("Com_Model")))->Get_BoneMatrixPtr("spine_00")) * m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat());
        else
        {
            wireMatPos = m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat();
            auto wireTarget = dynamic_pointer_cast<CBuiltIn_Magnet>(m_pWire_Target);
            if (wireTarget)
                wireMatPos.r[3] -= m_pWire_Target->Get_Transform()->Get_NormLook() * 0.5f;
            else
                wireMatPos.r[3] -= m_pWire_Target->Get_Transform()->Get_NormUp() * 1.5f;
            XMStoreFloat4x4(&mat_Wire_Bone, wireMatPos);
            //_float4x4 wirePos = m_pWire_Target->Get_Transform()->Get_WorldMatrix();
            //wirePos.m[3][1] = wirePos.m[3][1] -= 2.f;
            //mat_Wire_Bone = wirePos;
            //XMStoreFloat4x4(&mat_Wire_Bone, XMLoadFloat4x4(static_pointer_cast<CModel>(m_pWire_Target->Find_Component(TEXT("Model_Com")))->Get_BoneMatrixPtr("body_attach_00")) * m_pWire_Target->Get_Transform()->Get_WorldMatrix_XMMat());
        }
        _vector targetPos = {};
        if (!m_bGimmicWire)
            targetPos = m_pWire_Target->Get_Transform()->Get_Position();
        else
            targetPos = wireMatPos.r[3];
        m_pTransformCom->LookAt_Horizontal_With_Speed(targetPos, fTimeDelta * 10.f);

        _vector targetDir = XMVector3Normalize(targetPos - m_pTransformCom->Get_Position());

        _float targetDistance = XMVectorGetX(XMVector3Length(targetPos - m_pTransformCom->Get_Position()));

        _float4 fBonPos = { matBone.m[3][0], matBone.m[3][1], matBone.m[3][2], 1 };
        _float4 fWireBonPos = { mat_Wire_Bone.m[3][0], mat_Wire_Bone.m[3][1], mat_Wire_Bone.m[3][2], 1 };

        _vector targetWireDir = XMVector3Normalize(XMLoadFloat4(&fWireBonPos) - XMLoadFloat4(&fBonPos));

        if (m_pModelCom->Get_Current_Anim_Track_Position_Percentage() >= 0.66f)
        {
            m_bWire = false;
            m_pCCT->Set_Velocity({ 0,0,0,0 });
        }
        else if (m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.33f && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.66f)
        {
            m_pWire->Set_Start_Pos(fBonPos);
            m_pCCT->Set_Velocity(targetDir * m_fWire_Target_Distance * (1.f / fBeat));
        }

        else if (m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.33f)
        {
            m_pWire->Set_Start_Pos(fBonPos);
            _float4 endPos = {};
            m_fWire_Target_Length += m_fWire_Target_Distance * (fTimeDelta / fBeat);
            XMStoreFloat4(&endPos, XMLoadFloat4(&fBonPos) + targetWireDir * m_fWire_Target_Length);
            m_pWire->Set_End_Pos(endPos);
        }

        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Three_Beat);
        m_fTime_Offset = Three_Beat;

        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Three_Beat), 0.1f, fmatResult);
    }
    else if ( // 3박 공격
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_053") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_120") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_093")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Three_Beat);
            m_fTime_Offset = Three_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Three_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    else if ( // 2박 공격

        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_001") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_002") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_003") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_020") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_006") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_030") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_031")

        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Two_Beat);
            m_fTime_Offset = Two_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Two_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    else if ( // 4박 공격
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_030") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_140") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_080") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_081") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_001") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_000") ||

        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_002")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Four_Beat);
            m_fTime_Offset = Four_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Four_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    else if ( // 6박 공격
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_010")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Six_Beat);
            m_fTime_Offset = Six_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    else if ( // 6박 공격 마무리
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_004")
        )
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Six_Beat);
        m_fTime_Offset = Six_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat), 0.1f, fmatResult);
    }
    else if ( // 4박 공격(우클릭)
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_021") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_082") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_083")
        )
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Four_Beat);
        m_fTime_Offset = Four_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Four_Beat), 0.1f, fmatResult);
    }
    else if (// 5박 공격(우클릭)
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_141")
        )
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_fTime_Offset = Five_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat), 0.1f, fmatResult);
    }
    else if (
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_060")
        )
    {
            if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
            {
                m_bRimLight = true;
                m_fRimTimer = 1.f;
                m_fRimColor = { 0.f,1.f,0.f,0.3f };
                CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
                if (eWordType == CUi_JudgeWord::WordType::Perfect || eWordType == CUi_JudgeWord::WordType::Good)
                    m_fHp += 40.f;
                if (m_fHp > m_fMaxHp)
                    m_fHp = m_fMaxHp;
                m_pCommandIcon = nullptr;
            }
            if(m_pModelCom->Get_Current_Track_Position() > 80.f && m_pCommandIcon)
            {
                m_bRimLight = true;
                m_fRimTimer = 1.f;
                m_fRimColor = { 0.f,1.f,0.f,0.3f };
                m_fHp += 20.f;
                if (m_fHp > m_fMaxHp)
                    m_fHp = m_fMaxHp;
                m_pCommandIcon->Dead();
                m_pCommandIcon = nullptr;
            }
           
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Seven_Beat);
            m_fTime_Offset = Seven_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta* Seven_Beat, 0.1f, fmatResult);
    }
    else if (
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp-cp_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_200")
        )
    {
         if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
         {
             CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
             if (eWordType == CUi_JudgeWord::WordType::Perfect || eWordType == CUi_JudgeWord::WordType::Good)
                 _int a = 10;
             m_pCommandIcon = nullptr;
         }
        _float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;
        _float fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(fBeatSpeed);
        m_fTime_Offset = fBeatSpeed;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fBeatSpeed, 0.1f, fmatResult);
    }
    /*FOR JUMP*/
    else if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_000") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_001") == m_pModelCom->Get_CurrentAnimIndex() ||
        m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_002") == m_pModelCom->Get_CurrentAnimIndex())
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    /* Air Attack  Rhytthm  2박자*/
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_082"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_010") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_011") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_012") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_081")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Two_Beat);
            m_fTime_Offset = Two_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Two_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    /* Air Attack  Rhythm  4박자*/
    else  if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_013") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_041") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_052")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Four_Beat);
            m_fTime_Offset = Four_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Four_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    /* Air Attack  Rhythm  6박자*/
    else  if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_053") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_070") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_071")
        )
    {
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Six_Beat);
            m_fTime_Offset = Six_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    // SP Attacks
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-sp_050"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Seven_Beat);
        m_fTime_Offset = Seven_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Seven_Beat, 0.1f, fmatResult);

        if (m_pModelCom->Get_Current_Track_Position() > (m_pModelCom->Get_Duration() / 7.f * 2.f) &&
            m_pModelCom->Get_Current_Track_Position() < (m_pModelCom->Get_Duration() / 7.f * 2.1f) &&
            !m_bCommandIcon)
        {
            m_bCommandIcon = true;
            CUi_CommandIcon::Desc Desc;
            Desc.CorrectBeat = 2;
            Desc.fX = 1;
            Desc.fY = 0;
            Desc.eRenderType = CUi::RenderType::Priority;
            m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, LEVEL_TEST, L"CUi", CUi_CommandIcon::ObjID, &Desc);
        }
        if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
        {
            CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
            if (eWordType == CUi_JudgeWord::WordType::Perfect)
                EFFECTMANAGER->Call_Union_Effect("Chai_Special_Slide_2", m_pTransformCom->Get_WorldMatrix_Ptr(), m_fTime_Offset);

            else if (eWordType == CUi_JudgeWord::WordType::Good)
                EFFECTMANAGER->Call_Union_Effect("Chai_Special_Slide_2", m_pTransformCom->Get_WorldMatrix_Ptr(), m_fTime_Offset);

            else
                EFFECTMANAGER->Call_Union_Effect("Chai_Special_Slide_2_F", m_pTransformCom->Get_WorldMatrix_Ptr(), m_fTime_Offset);

            m_pCommandIcon = nullptr;
        }
    }
    // Final Attacks

    /*LLLL */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_003"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_fTime_Offset = Five_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat), 0.1f, fmatResult);

        if (m_pModelCom->Get_Current_Track_Position() > 0 &&
            m_pModelCom->Get_Current_Track_Position() < (m_pModelCom->Get_Duration() / 5.f) &&
            !m_bCommandIcon)
        {
            m_bCommandIcon = true;
            CUi_CommandIcon::Desc Desc;
            Desc.CorrectBeat = 2;
            Desc.fX = 1;
            Desc.fY = 0;
            Desc.eRenderType = CUi::RenderType::Priority;
            m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi", CUi_CommandIcon::ObjID, &Desc);
        }
        if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
        {
            CUi_SuccessSystem::Desc successDesc;
            successDesc.CenterPos = { 0,0 };
            CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
            if (eWordType == CUi_JudgeWord::WordType::Perfect)
            {
                CUi_Mgr::Get_Instance()->Add_Clone(CUi_SuccessSystem::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi_Success_System", CUi_SuccessSystem::ObjID, &successDesc);
                m_fEffectMatrix = m_pTransformCom->Get_WorldMatrix();
                m_strEffectName = "Combo_LLLL_4_2";
            }

            else if (eWordType == CUi_JudgeWord::WordType::Good)
            {
                CUi_Mgr::Get_Instance()->Add_Clone(CUi_SuccessSystem::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi_Success_System", CUi_SuccessSystem::ObjID, &successDesc);
                m_fEffectMatrix = m_pTransformCom->Get_WorldMatrix();
                m_strEffectName = "Combo_LLLL_4_2";
            }
            m_pCommandIcon = nullptr;
        }
    }
    /*RLL */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_031"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Six_Beat);
        m_fTime_Offset = Six_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat), 0.1f, fmatResult);

        if (m_pModelCom->Get_Current_Track_Position() > (m_pModelCom->Get_Duration() / 6.f) &&
            m_pModelCom->Get_Current_Track_Position() < (m_pModelCom->Get_Duration() / 6.f * 1.1f) &&
            !m_bCommandIcon)
        {
            m_bCommandIcon = true;
            CUi_CommandIcon::Desc Desc;
            Desc.CorrectBeat = 2;
            Desc.fX = 1;
            Desc.fY = 0;
            Desc.eRenderType = CUi::RenderType::Priority;
            m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, LEVEL_TEST, L"CUi", CUi_CommandIcon::ObjID, &Desc);
        }
        if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
        {
            CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
            if (eWordType == CUi_JudgeWord::WordType::Perfect)
            {
                m_bAttack_Input = false;
                m_bNextAttack = false;
                m_fAttack_Motion_Offset = 0.f;
                Initialize_Attack_Tree(m_pTree_Attack);
                Initialize_Attack_Tree(m_pTree_Air_Attack);
                m_vecCombo.clear();
                m_pModelCom->Set_AnimIndex("ch0000_atk-guitar_032", false);
                m_fEffectMatrix = *m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
                m_strEffectName = "Combo_RLL_4_1";
            }

            else if (eWordType == CUi_JudgeWord::WordType::Good)
            {
                m_bAttack_Input = false;
                m_bNextAttack = false;
                m_fAttack_Motion_Offset = 0.f;
                Initialize_Attack_Tree(m_pTree_Attack);
                Initialize_Attack_Tree(m_pTree_Air_Attack);
                m_vecCombo.clear();
                m_pModelCom->Set_AnimIndex("ch0000_atk-guitar_032", false);
                m_fEffectMatrix = *m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
                m_strEffectName = "Combo_RLL_4_1";
            }
            m_pCommandIcon = nullptr;
        }
    }
    /*RLL Just*/
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_032"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_fTime_Offset = Five_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat), 0.1f, fmatResult);
    }
    /* LRR */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_041"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Six_Beat);
        m_fTime_Offset = Six_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat), 0.1f, fmatResult);

        if (m_pModelCom->Get_Current_Track_Position() > m_pModelCom->Get_Duration() * 0.4f &&
            m_pModelCom->Get_Current_Track_Position() < m_pModelCom->Get_Duration() * 0.45f &&
            !m_bCommandIcon)
        {
            m_bCommandIcon = true;
            CUi_CommandIcon::Desc Desc;
            Desc.CorrectBeat = 2;
            Desc.fX = 1;
            Desc.fY = 0;
            Desc.eRenderType = CUi::RenderType::Priority;
            m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, LEVEL_TEST, L"CUi", CUi_CommandIcon::ObjID, &Desc);
        }
        if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
        {
            CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
            if (eWordType == CUi_JudgeWord::WordType::Perfect)
            {
                m_fEffectMatrix = m_pTransformCom->Get_WorldMatrix();
                m_strEffectName = "Combo_LRR_3_2";
            }

            else if (eWordType == CUi_JudgeWord::WordType::Good)
            {
                m_fEffectMatrix = m_pTransformCom->Get_WorldMatrix();
                m_strEffectName = "Combo_LRR_3_2";
            }

            m_pCommandIcon = nullptr;
        }
    } /* LL_LL */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_094"))
    {
        m_bZoomIn = true;
        m_fZoomIn_Timer = fBeat;
        m_pPlayer_Camera->Set_DistanceTarget(m_pPlayer_Camera->Get_Original_DistanceTarget() * 0.7f);
        if (!rhythmOffset)
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(Three_Beat);
            m_fTime_Offset = Three_Beat;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Three_Beat) * animationOffset, 0.1f, fmatResult);
        }
        else
        {
            shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
            Weapon_Model->Set_AnimSpeed(animationOffset * m_fTimeMultiflier);
            m_fTime_Offset = animationOffset * m_fTimeMultiflier;
            m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta)*animationOffset * m_fTimeMultiflier, 0.1f, fmatResult);
        }
    }
    /* LL_LLL */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_095"))
    {
        if (m_pModelCom->Get_Current_Track_Position() < (m_pModelCom->Get_Duration() / 5.f * 2.f))
        {
            m_bZoomIn = true;
            m_fZoomIn_Timer = fBeat;
            m_pPlayer_Camera->Set_DistanceTarget(m_pPlayer_Camera->Get_Original_DistanceTarget() * 0.7f);
        }

        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Five_Beat);
        m_fTime_Offset = Five_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Five_Beat), 0.1f, fmatResult);

        if (m_pModelCom->Get_Current_Track_Position() > 0 &&
            m_pModelCom->Get_Current_Track_Position() < (m_pModelCom->Get_Duration() / 5.f) &&
            !m_bCommandIcon)
        {
            m_bCommandIcon = true;
            CUi_CommandIcon::Desc Desc;
            Desc.CorrectBeat = 2;
            Desc.fX = 1;
            Desc.fY = 0;
            Desc.eRenderType = CUi::RenderType::Priority;
            m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, LEVEL_TEST, L"CUi", CUi_CommandIcon::ObjID, &Desc);
        }
        if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
        {
            CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
            if (eWordType == CUi_JudgeWord::WordType::Perfect)
            {
            }

            else if (eWordType == CUi_JudgeWord::WordType::Good)
            {
            }
            m_pCommandIcon = nullptr;
        }
    }
    /* RRR */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_022"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Six_Beat);
        m_fTime_Offset = Six_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Six_Beat), 0.1f, fmatResult);

        if (m_pModelCom->Get_Current_Track_Position() > 0 &&
            m_pModelCom->Get_Current_Track_Position() < (m_pModelCom->Get_Duration() / 6.f) &&
            !m_bCommandIcon)
        {
            m_bCommandIcon = true;
            CUi_CommandIcon::Desc Desc;
            Desc.CorrectBeat = 2;
            Desc.fX = 1;
            Desc.fY = 0;
            Desc.eRenderType = CUi::RenderType::Priority;
            m_pCommandIcon = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, LEVEL_TEST, L"CUi", CUi_CommandIcon::ObjID, &Desc);
        }
        if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandIcon)
        {
            CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
            if (eWordType == CUi_JudgeWord::WordType::Perfect)
            {
            }

            else if (eWordType == CUi_JudgeWord::WordType::Good)
            {
            }
            m_pCommandIcon = nullptr;
        }
    }
    /* RLR */
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_060"))
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Eight_Beat);
        m_fTime_Offset = Eight_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, (fTimeDelta * Eight_Beat), 0.1f, fmatResult);
    }
    else
    {
        shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon[0]->Find_Component(TEXT("Com_Model")));
        Weapon_Model->Set_AnimSpeed(Two_Beat);
        m_fTime_Offset = Two_Beat;
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);
    }

    /* Attack Anim Gravity Controller */
    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_007") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_082") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_021"))
    {
        m_pCCT->Enable_Gravity();
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_140"))
        _int a = 10;
    else if(m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_130"))
        m_pCCT->Disable_Gravity();
    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_053") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_120") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_081") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_082") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_060") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_020") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_010") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-rekka_100") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-mimosa_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_action_104") ||
        m_eEvent_State == CHAI_EVENT_STATE::ZIPLINE ||
        m_eMoveState == CHAI_MOVE_STATE::AIR_ATTACK)
    {
        m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0, 0));
        m_pCCT->Disable_Gravity();
    }
    else
        m_pCCT->Enable_Gravity();

    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    if (!m_bTurning && m_bRootMove_On)
        m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
}

void CChai::Move(_float fTimeDelta)
{
#pragma region Run
    /* RUN */
    _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
    auto pGI = GAMEINSTANCE;
    _vector camLookVec = XMVectorSetY(pGI->Get_CamLook_Vector(), 0);

    _vector camRightvec = {};
    shared_ptr <CTransform> camTrasnform = m_pPlayer_Camera->Get_Transform();
    camRightvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_RIGHT), 0);

    m_bWire = false;
    m_fWire_Target_Distance = 0.f;
    m_fWire_Target_Length = 0.f;

    if (!m_b2D_Mode)
    {
        if ((pGI->Get_KeyPressing(DIK_W) || pGI->Get_KeyPressing(DIK_S) || pGI->Get_KeyPressing(DIK_A) || pGI->Get_KeyPressing(DIK_D))
            && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000"))
        {
            m_bNextAttack = false;
            m_iDashCount = 0;
            m_eMoveState = CHAI_MOVE_STATE::RUN_START;
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-run_080", false);
            m_vecCombo.clear();
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            Initialize_Attack_Tree(m_pTree_Attack);
        }
        else if ((pGI->Get_KeyPressing(DIK_W) || pGI->Get_KeyPressing(DIK_S) || pGI->Get_KeyPressing(DIK_A) || pGI->Get_KeyPressing(DIK_D))
            && m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000"))
            m_pModelCom->Set_AnimIndexNonCancle_NoInterpole("ch0000_mvnt-run_000", true);
    }
    else
    {
        if ((pGI->Get_KeyPressing(DIK_A) || pGI->Get_KeyPressing(DIK_D))
            && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000"))
        {
            m_bNextAttack = false;
            m_iDashCount = 0;
            m_eMoveState = CHAI_MOVE_STATE::RUN_START;
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-run_080", false);
            m_vecCombo.clear();
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            Initialize_Attack_Tree(m_pTree_Attack);
        }
        else if ((pGI->Get_KeyPressing(DIK_W) || pGI->Get_KeyPressing(DIK_S) || pGI->Get_KeyPressing(DIK_A) || pGI->Get_KeyPressing(DIK_D))
            && m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000"))
            m_pModelCom->Set_AnimIndexNonCancle_NoInterpole("ch0000_mvnt-run_000", true);
    }

    /* 중립 입력 */

    if (pGI->Get_KeyNone(DIK_W) && pGI->Get_KeyNone(DIK_S) && pGI->Get_KeyNone(DIK_A) && pGI->Get_KeyNone(DIK_D)
        && (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_080") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-run_000")))
    {
        if (m_eMoveState != CHAI_MOVE_STATE::IDLE)
            Adjust_Idle_Timing();
        m_eMoveState = CHAI_MOVE_STATE::IDLE;
    }

    if (m_eMoveState == CHAI_MOVE_STATE::RUN && !m_bTurning)
        m_pCCT->Move_Forward(m_pTransformCom->Get_Speed());

#pragma endregion 

    Turn(fTimeDelta);
}

_bool CChai::Dash(_float fTimeDelta)
{
    _bool Dashing = false;
    auto pGI = GAMEINSTANCE;
    if (pGI->Get_KeyDown(DIK_LSHIFT))
    {
        m_bTurning = false;
        m_bNextAttack = false;
        m_isAttackOn = false;
        m_fAttack_Motion_Offset = 0.f;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);
        m_vecCombo.clear();
        Dashing = true;
        m_eMoveState = CHAI_MOVE_STATE::DASH;

        shared_ptr<CUi_BeatMarkerSystem> beatMarkersys = static_pointer_cast<CUi_BeatMarkerSystem> (CUi_Mgr::Get_Instance()->Find_UI(TEXT("CUi_BeatMarkerSystem")));
        if (m_iDashCount == 0)
        {
            Play_808_Just_Effect();
            _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
            _vector camLookVec = XMVectorSetY(GAMEINSTANCE->Get_CamLook_Vector(), 0);

            _vector camRightvec = {};
            shared_ptr <CTransform> camTrasnform = m_pPlayer_Camera->Get_Transform();
            camRightvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_RIGHT), 0);

            if (beatMarkersys && beatMarkersys->Cal_Judge() == "Perfect")
            {
                pGI->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Player_Dash");
            }

            m_iDashCount++;
            if (m_fDashDir.x == 0 && m_fDashDir.y == 0 && m_fDashDir.z == 0)
                XMStoreFloat3(&m_fDashDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

            if (pGI->Get_KeyPressing(DIK_W) && pGI->Get_KeyPressing(DIK_A))
            {
                if (m_bOnAir)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(camLookVec + -camRightvec));
            }

            else if (pGI->Get_KeyPressing(DIK_W) && pGI->Get_KeyPressing(DIK_D))
            {
                if (m_bOnAir)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(camLookVec + camRightvec));
            }

            else if (pGI->Get_KeyPressing(DIK_S) && pGI->Get_KeyPressing(DIK_A))
            {
                if (m_bOnAir)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(-camLookVec + -camRightvec));
            }

            else if (pGI->Get_KeyPressing(DIK_S) && pGI->Get_KeyPressing(DIK_D))
            {
                if (m_bOnAir)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(-camLookVec + camRightvec));
            }
            else if (pGI->Get_KeyPressing(DIK_W) && m_bOnAir)
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
            else if (pGI->Get_KeyPressing(DIK_W))
            {
                _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(camLookVec));
                _float LookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

                if (LookDiffDegree > -45.f && LookDiffDegree < 45.f)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                else if (XMVectorGetX(LookDiffrence) < 1.01f && XMVectorGetY(LookDiffrence) < 1.01f &&  XMVectorGetZ(LookDiffrence) < 1.01f)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                else if (LookDiffDegree > 45.f && LookDiffDegree < 135.f)
                {
                    _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(camLookVec));
                    _float turnFloat = XMVectorGetY(LookCross);
                    if (turnFloat < 0)
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_002", false);
                    else
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_003", false);
                }
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_001", false);
            }
            else if (pGI->Get_KeyPressing(DIK_S) && m_bOnAir)
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
            else if (pGI->Get_KeyPressing(DIK_S))
            {
                _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
                _float LookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

                if (LookDiffDegree > -45.f && LookDiffDegree < 45.f)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                else if (LookDiffDegree > 45.f && LookDiffDegree < 135.f)
                {
                    _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
                    _float turnFloat = XMVectorGetY(LookCross);
                    if (turnFloat < 0)
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_002", false);
                    else
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_003", false);
                }
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_001", false);
            }
            else if (pGI->Get_KeyPressing(DIK_A) && m_bOnAir)
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
            else if (pGI->Get_KeyPressing(DIK_A))
            {
                _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(-camRightvec));
                _float LookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

                if (LookDiffDegree > -45.f && LookDiffDegree < 45.f)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                else if (LookDiffDegree > 45.f && LookDiffDegree < 135.f)
                {
                    _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(-camRightvec));
                    _float turnFloat = XMVectorGetY(LookCross);
                    if (turnFloat < 0)
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_002", false);
                    else
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_003", false);
                }
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_001", false);
            }
            else if (pGI->Get_KeyPressing(DIK_D) && m_bOnAir)
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
            else if (pGI->Get_KeyPressing(DIK_D))
            {
                _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(camRightvec));
                _float LookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

                if (LookDiffDegree > -45.f && LookDiffDegree < 45.f)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
                else if (LookDiffDegree > 45.f && LookDiffDegree < 135.f)
                {
                    _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(camRightvec));
                    _float turnFloat = XMVectorGetY(LookCross);
                    if (turnFloat < 0)
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_002", false);
                    else
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_003", false);
                }
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_001", false);
            }

            else
            {
                if (m_bOnAir)
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_020", false);
                else
                    m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_000", false);
            }
        }
        else if (m_iDashCount >= 1 && beatMarkersys->Cal_Judge() == "Perfect" && !m_bOnAir)
        {
            Play_808_Just_Effect();
            pGI->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Player_Dash");

            m_iDashCount++;
            _vector camRightvec = {};
            _vector camLookvec = {};
            shared_ptr <CTransform> camTrasnform = m_pPlayer_Camera->Get_Transform();
            camRightvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_RIGHT), 0);
            camLookvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_LOOK), 0);

            if (pGI->Get_KeyPressing(DIK_W) && pGI->Get_KeyPressing(DIK_A))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(camLookvec + -camRightvec));

            else  if (pGI->Get_KeyPressing(DIK_W) && pGI->Get_KeyPressing(DIK_D))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(camLookvec + camRightvec));

            else if (pGI->Get_KeyPressing(DIK_S) && pGI->Get_KeyPressing(DIK_A))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(-camLookvec + -camRightvec));

            else if (pGI->Get_KeyPressing(DIK_S) && pGI->Get_KeyPressing(DIK_D))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(-camLookvec + camRightvec));
            else if (pGI->Get_KeyPressing(DIK_W))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(camLookvec));

            else if (pGI->Get_KeyPressing(DIK_S))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(-camLookvec));

            else if (pGI->Get_KeyPressing(DIK_A))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(-camRightvec));

            else if (pGI->Get_KeyPressing(DIK_D))
                m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(camRightvec));

            if (m_iDashCount == 2)
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_030", false);
            else if (m_iDashCount == 3)
            {
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-escape_031", false);
                m_iDashCount = 0;
            }
        }
    }

    return Dashing;
}

void CChai::Turn(_float fTimeDelta)
{
    _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
    auto pGI = GAMEINSTANCE;
    _vector camLookVec = XMVectorSetY(pGI->Get_CamLook_Vector(), 0);

    _vector camRightvec = {};
    shared_ptr <CTransform> camTrasnform = m_pPlayer_Camera->Get_Transform();
    camRightvec = XMVectorSetY(camTrasnform->Get_State(CTransform::STATE_RIGHT), 0);
    /* 오른쪽 위 대각*/
    if (pGI->Get_KeyPressing(DIK_W) && pGI->Get_KeyPressing(DIK_D) && !m_b2D_Mode)
    {
        _vector diagonalVec = XMVector3Normalize(camLookVec + camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
    }
    else if (pGI->Get_KeyPressing(DIK_W) && pGI->Get_KeyPressing(DIK_A) && !m_b2D_Mode)
    {
        _vector diagonalVec = XMVector3Normalize(camLookVec + -camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
        /*    m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + diagonalVec * m_pTransformCom->Get_Speed() * fTimeDelta);*/
    }
    else if (pGI->Get_KeyPressing(DIK_S) && pGI->Get_KeyPressing(DIK_D) && !m_b2D_Mode)
    {
        _vector diagonalVec = XMVector3Normalize(-camLookVec + camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
    }
    else if (pGI->Get_KeyPressing(DIK_S) && pGI->Get_KeyPressing(DIK_A) && !m_b2D_Mode)
    {
        _vector diagonalVec = XMVector3Normalize(-camLookVec + -camRightvec);
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(diagonalVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
    }
    else if (pGI->Get_KeyPressing(DIK_W) && !m_b2D_Mode)
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(camLookVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(camLookVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(camLookVec * m_pTransformCom->Get_Speed());
    }
    else if (pGI->Get_KeyPressing(DIK_D))
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(camRightvec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(camRightvec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(camRightvec * m_pTransformCom->Get_Speed());
    }
    else if (pGI->Get_KeyPressing(DIK_A))
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(-camRightvec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(-camRightvec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(-camRightvec * m_pTransformCom->Get_Speed());
    }
    else if (pGI->Get_KeyPressing(DIK_S) && !m_b2D_Mode)
    {
        _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
        m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

        _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(-camLookVec));
        _float turnFloat = XMVectorGetY(LookCross);
        if (turnFloat < 0)
            m_bTrunRight = false;
        else
            m_bTrunRight = true;

        if (m_fLookDiffDegree > 1.f)
        {
            m_fTurnTimer = 0.1f;
            m_bTurning = true;
        }
        else
        {
            m_fTurnTimer = 0.f;
            m_bTurning = false;
        }
        if (m_bTurning && !m_bOnAir)
            m_pCCT->Add_Displacement(-camLookVec * m_pTransformCom->Get_Speed());
    }
    if (m_fLookDiffDegree != 0.f && m_fTurnTimer > 0.f && m_bTurning == true)
    {
        m_fTurnTimer -= fTimeDelta;
        _vector turnDir = {};
        if (m_bTrunRight)
            turnDir = { 0.f, 1.f, 0.f, 0.f };
        else
            turnDir = { 0.f, -1.f, 0.f, 0.f };
        m_pTransformCom->Turn(turnDir, XMConvertToRadians(m_fLookDiffDegree), fTimeDelta * 10.f);
    }
    else if (m_fTurnTimer <= 0.f)
    {
        m_bTurning = false;
        m_fTurnTimer = 0.1f;
        m_fLookDiffDegree = 0.f;
    }
}

void CChai::Jump(_float fTimeDelta)
{
    _bool JumpStart = false;
    if (GAMEINSTANCE->Get_KeyDown(DIK_SPACE))
    {
        if (m_eMoveState != CHAI_MOVE_STATE::JUMP && !m_bOnAir)
        {
            m_vecCombo.clear();
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            Initialize_Attack_Tree(m_pTree_Attack);
            JumpStart = true;
            m_bOnAir = true;
            m_pCCT->Set_Velocity(XMVectorSet(0.f, 18.f, 0, 0));
            m_eMoveState = CHAI_MOVE_STATE::JUMP;
            m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_000", false);
        }
        else if (m_eMoveState == CHAI_MOVE_STATE::JUMP && m_bOnAir)
        {
            m_bOnAir = true;
            m_pCCT->Set_Velocity(XMVectorSet(0.f, 18.f, 0, 0));
            m_eMoveState = CHAI_MOVE_STATE::DOUBLE_JUMP;
            m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_010", false);
        }
    }

    PxControllerState pCS;
    m_pCCT->Get_Controller()->getState(pCS);
    if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_007"))
        {
            m_isAttackOn = false;
            m_pModelCom->Set_AnimIndex("ch0000_atk-jump_008", false);
            m_bNextAttack = false;
            m_vecCombo.clear();
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
        }
        else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_021"))
        {
            m_eNextAttack = ATTACK_TYPE_END;
            m_bAttack_Input = false;
            m_bNextAttack = false;
            m_fAttack_Motion_Offset = 0.f;
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
            m_vecCombo.clear();
            m_eMoveState = CHAI_MOVE_STATE::IDLE;
            Adjust_Idle_Timing();
            m_fIdle_Motion_Offset = CBeat_Manager::Get_Instance()->Get_Timer();
        }

        else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-jump_082"))
        {
            m_pModelCom->Set_AnimIndex("ch0000_atk-jump_083", false);
            m_bNextAttack = false;
            m_vecCombo.clear();
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
        }
        else if ((m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_000") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_010") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_001") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-jump_011") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_event-magnetrail_060") ||
            m_eMoveState == CHAI_MOVE_STATE::AIR_ATTACK)
            && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_120")
            && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_081")
            && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_082")
            && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_053")
            && !JumpStart
            )
        {
         
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-jump_002", false);
            EFFECTMANAGER->Call_Fixed_Union_Effect("LandingSmoke", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
            m_bNextAttack = false;
            m_vecCombo.clear();
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
        }
        else if (m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_120"))
            m_bUpperAttack = false;
        else if (m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_53"))
            m_bL_Rest_UpperAttack = false;

        if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("ch0000_atk-dash_010"))
            m_bOnAir = true;
        else
            m_bOnAir = false;
    }
    else
    {
        PxVec3 pos = { (_float)m_pCCT->Get_Controller()->getFootPosition().x,(_float)m_pCCT->Get_Controller()->getFootPosition().y,(_float)m_pCCT->Get_Controller()->getFootPosition().z };
        PxRaycastBuffer hit;
        PxVec3 origin = pos;
        PxVec3 direction = PxVec3(0, -1, 0); // 아래 방향으로 레이캐스팅
        PxReal distance = 0.5f; // 계단의 높이보다 살짝 큰 값
        direction.normalize();
        bool status = GAMEINSTANCE->Get_Scene()->raycast(origin, direction, distance, hit);
        if (status)
        {
        }
        else
        {
            if (m_eMoveState != CHAI_MOVE_STATE::ATTACK && m_eMoveState != CHAI_MOVE_STATE::JUMP && m_eMoveState != CHAI_MOVE_STATE::DOUBLE_JUMP
                && m_eMoveState != CHAI_MOVE_STATE::JUMP_FALL && m_eMoveState != CHAI_MOVE_STATE::DOUBLE_JUMP_FALL
                && m_eMoveState != CHAI_MOVE_STATE::AIR_ATTACK
                && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_action_130")
                && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_action_104")
                && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_action_140")
                && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_020")
                && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_021")
                && m_bOnAir)
            {
                m_eMoveState = CHAI_MOVE_STATE::JUMP_FALL;
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-jump_001", true);
            }
            else if (m_eMoveState != CHAI_MOVE_STATE::ATTACK)
                m_bOnAir = true;
        }
    }
}

void CChai::AirMove(_float fTimeDelta)
{
    if ((GAMEINSTANCE->Get_KeyPressing(DIK_W) || GAMEINSTANCE->Get_KeyPressing(DIK_S) || GAMEINSTANCE->Get_KeyPressing(DIK_A) || GAMEINSTANCE->Get_KeyPressing(DIK_D)))
    {
        m_pCCT->Move_Forward(m_pTransformCom->Get_Speed());
    }

    Turn(fTimeDelta);
}

_bool CChai::Attack(_float fTimeDelta)
{
    _bool bAttacking = false;
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    m_fAttackTimer += fTimeDelta;

    if (m_bNextAttack && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.25f && m_eNextAttack != ATTACK_TREE_TYPE::ATTACK_TYPE_END)
    {
        if (m_eNextAttack == ATTACK_TREE_TYPE::R && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.25f
            && m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_024") == m_pModelCom->Get_CurrentAnimIndex())
            return true;
        else if (m_eNextAttack == ATTACK_TREE_TYPE::R && m_vecCombo.size() == 1 && m_vecCombo.back() == ATTACK_TREE_TYPE::L)
        {
        }
        else if (m_bOnAir && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.5f)
            return true;
        else if (m_eNextAttack == ATTACK_TREE_TYPE::R && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() < 0.5f
            && m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_024") != m_pModelCom->Get_CurrentAnimIndex())
            return true;
        _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
        m_fAttack_Motion_Offset = fTimer;
        m_bTurning = false;
        m_bAttack_Input = false;

        if (m_bOnAir)
            Traverse_AttacK_Tree(m_pTree_Air_Attack, m_eNextAttack, m_vecCombo);
        else
            Traverse_AttacK_Tree(m_pTree_Attack, m_eNextAttack, m_vecCombo);

        m_eNextAttack = ATTACK_TREE_TYPE::ATTACK_TYPE_END;
    }
    else
    {
        if (!m_bAttack_Input && !m_bNextAttack)
        {
            shared_ptr<CUi_BeatMarkerSystem> beatMarkersys = static_pointer_cast<CUi_BeatMarkerSystem> (CUi_Mgr::Get_Instance()->Find_UI(TEXT("CUi_BeatMarkerSystem")));
            if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
            {
                m_iDashCount = 0;
                if (beatMarkersys && beatMarkersys->Cal_Judge() == "Perfect")
                {
                    Play_808_Just_Effect();
                    _int iSound = GAMEINSTANCE->Random_Int(0, 2);
                    if (iSound == 0)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_0");
                    else if (iSound == 1)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_1");
                    else if (iSound == 2)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_2");
                }
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bAttack_Input = true;
                m_bTurning = false;

                if (m_bOnAir)
                    Traverse_AttacK_Tree(m_pTree_Air_Attack, ATTACK_TREE_TYPE::L, m_vecCombo);
                else
                    Traverse_AttacK_Tree(m_pTree_Attack, ATTACK_TREE_TYPE::L, m_vecCombo);
            }

            else if (GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
            {
                m_iDashCount = 0;
                if (beatMarkersys && beatMarkersys->Cal_Judge() == "Perfect")
                {
                    Play_808_Just_Effect();
                    _int iSound = GAMEINSTANCE->Random_Int(0, 2);
                    if (iSound == 0)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_0");
                    else if (iSound == 1)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_1");
                    else if (iSound == 2)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_2");
                }
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bAttack_Input = true;
                m_bTurning = false;
                if (m_bOnAir)
                    Traverse_AttacK_Tree(m_pTree_Air_Attack, ATTACK_TREE_TYPE::R, m_vecCombo);
                else
                    Traverse_AttacK_Tree(m_pTree_Attack, ATTACK_TREE_TYPE::R, m_vecCombo);
            }
        }
        else
        {
            if (m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_023") == m_pModelCom->Get_CurrentAnimIndex())
                return true;
            shared_ptr<CUi_BeatMarkerSystem> beatMarkersys = static_pointer_cast<CUi_BeatMarkerSystem> (CUi_Mgr::Get_Instance()->Find_UI(TEXT("CUi_BeatMarkerSystem")));
            if (GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON))
            {
                m_iDashCount = 0;
                if (beatMarkersys && beatMarkersys->Cal_Judge() == "Perfect" && m_eNextAttack != ATTACK_TREE_TYPE::L)
                {
                    Play_808_Just_Effect();
                    _int iSound = GAMEINSTANCE->Random_Int(0, 2);
                    if (iSound == 0)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_0");
                    else if (iSound == 1)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_1");
                    else if (iSound == 2)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_2");
                }
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bNextAttack = true;
                m_bTurning = false;
                m_eNextAttack = ATTACK_TREE_TYPE::L;
            }

            else if (GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
            {
                m_iDashCount = 0;
                if (beatMarkersys && beatMarkersys->Cal_Judge() == "Perfect" && m_eNextAttack != ATTACK_TREE_TYPE::R)
                {
                    Play_808_Just_Effect();
                    _int iSound = GAMEINSTANCE->Random_Int(0, 2);
                    if (iSound == 0)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_0");
                    else if (iSound == 1)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_1");
                    else if (iSound == 2)
                        m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Just_Beat_2");
                }
                _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
                m_fIdle_Motion_Offset = 0.f;
                m_fAttack_Motion_Offset = fTimer;
                m_bNextAttack = true;
                m_bTurning = false;
                m_eNextAttack = ATTACK_TREE_TYPE::R;
            }
            else if (((m_vecCombo.size() == 1 && m_vecCombo.back() == ATTACK_TREE_TYPE::L) || (m_vecCombo.size() == 2 && m_vecCombo.back() == ATTACK_TREE_TYPE::L))
                && !Find_R()
                && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.45f
                && m_bOnAir == false)
            {
                m_bNextAttack = true;
                m_bTurning = false;
                m_eNextAttack = ATTACK_TREE_TYPE::REST;
            }
        }
    }
    //if (m_fAttackTimer > fBeat * 2.f)
    //{
    //    m_fAttackTimer = 0.f;
    //    m_bAttack_Input = false;
    //}
    string combo = {};
    for (auto& iter : m_vecCombo)
    {
        if (iter == L)
            combo += "L";
        else if (iter == R)
            combo += "R";
        else if (iter == REST)
            combo += "_Rest";
    }
    m_strCombo = combo;
    if (!combo.empty() && m_bOnAir == false)
    {
        m_fWeaponCollider_Extents = _float3{ 1.f, 1.f, 1.f };
        m_eMoveState = CHAI_MOVE_STATE::ATTACK;
        Attack_Turn(fTimeDelta);
        bAttacking = true;
        if (combo == "L")
        {
            if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_000") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_001") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_002") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_003") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_030") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_031") == m_pModelCom->Get_CurrentAnimIndex()
                )
            {
                m_eNextAttack = ATTACK_TYPE_END;
                m_bAttack_Input = false;
                m_bNextAttack = false;
                m_fAttack_Motion_Offset = 0.f;
                Initialize_Attack_Tree(m_pTree_Attack);
                Initialize_Attack_Tree(m_pTree_Air_Attack);
                m_vecCombo.clear();
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-dash_000", false);
            }
            else
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_000", false);
        }

        else if (combo == "LL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_001", false);

        else if (combo == "LLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_002", false);

        else if (combo == "LLLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_003", false);

        else if (combo == "L_RestL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_051", false);

        else if (combo == "L_RestLL")
        {
            m_bL_Rest_UpperAttack = true;
            m_bOnAir = true;
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_053", false);
        }

        else if (combo == "LL_RestL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_093", false);

        else if (combo == "LL_RestLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_094", false);

        else if (combo == "LLR")
        {
            m_bOnAir = true;
            m_bUpperAttack = true;
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_120", false);
        }

        else if (combo == "LR")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_040", false);

        else if (combo == "LRR")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_041", false);

        else if (combo == "LRL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_080", false);

        else if (combo == "LRLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_081", false);

        else if (combo == "LRLLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_082", false);
        else if (combo == "LL_RestLLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_095", false);

        else if (combo == "R" && m_pModelCom->Get_CurrentAnimIndex() != m_pModelCom->Get_StringAnimIndex("ch0000_atk-guitar_024"))
        {
            if (m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_000") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_001") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_002") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_003") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_030") == m_pModelCom->Get_CurrentAnimIndex() ||
                m_pModelCom->Get_StringAnimIndex("ch0000_mvnt-escape_031") == m_pModelCom->Get_CurrentAnimIndex()
                )
            {
                m_eNextAttack = ATTACK_TYPE_END;
                m_bAttack_Input = false;
                m_bNextAttack = false;
                m_fAttack_Motion_Offset = 0.f;
                Initialize_Attack_Tree(m_pTree_Attack);
                Initialize_Attack_Tree(m_pTree_Air_Attack);
                m_vecCombo.clear();
                m_bOnAir = true;
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-dash_010", false);
            }
            else
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_023", false);
        }

        else if (combo == "RR")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_021", false);

        else if (combo == "RRR")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_022", false);

        else if (combo == "RL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_030", false);

        else if (combo == "RLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_031", false);

        else if (combo == "RLR")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_060", false);

        else if (combo == "RRL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_140", false);

        else if (combo == "RRLL")
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-guitar_141", false);
    }
    else  if (!combo.empty() && m_bOnAir)
    {
        m_eMoveState = CHAI_MOVE_STATE::AIR_ATTACK;
        m_fWeaponCollider_Extents = _float3{ 1.7f, 4.f, 1.f };
        if (m_bL_Rest_UpperAttack)
        {
            if (combo == "L_RestLLL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_010", false);

            else if (combo == "L_RestLLLL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_011", false);

            else if (combo == "L_RestLLLLL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_012", false);

            else if (combo == "L_RestLLLLLL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_013", false);

            else if (combo == "L_RestLLLR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_041", false);

            else if (combo == "L_RestLLLRL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_080", false);

            /* 하강 착지 다 해야함 이건 시작모션 */
            else if (combo == "L_RestLLLRLR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_081", false);

            else if (combo == "L_RestLLLRR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_052", false);

            else if (combo == "L_RestLLLRRR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_053", false);
        }
        else
        {
            if (combo == "L")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_010", false);

            else if (combo == "LL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_011", false);

            else if (combo == "LLL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_012", false);

            else if (combo == "LLLL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_013", false);

            else if (combo == "LR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_041", false);

            else if (combo == "LRL")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_080", false);

            /* 하강 착지 다 해야함 이건 시작모션 */
            else if (combo == "LRLR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_081", false);

            else if (combo == "LRR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_052", false);

            else if (combo == "LRRR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_053", false);

            /* 찍기 공격 */
            if (combo == "R")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_006", false);
        }

        if (m_bUpperAttack)
        {
            if (combo == "LLRR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_070", false);

            else if (combo == "LLRRR")
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_atk-jump_071", false);
        }
    }

    if (m_eMoveState != CHAI_MOVE_STATE::EVENT)
        static_pointer_cast<CBasic_Guitar>(m_pWeapon[0])->Change_Animation(m_pModelCom->Get_AnimName());

    if (m_fLookDiffDegree != 0.f && m_fTurnTimer > 0.f && m_bAttacKTurning == true)
    {
        m_fTurnTimer -= fTimeDelta;
        _vector turnDir = {};
        if (m_bTrunRight)
            turnDir = { 0.f, 1.f, 0.f, 0.f };
        else
            turnDir = { 0.f, -1.f, 0.f, 0.f };
        m_pTransformCom->Turn(turnDir, XMConvertToRadians(m_fLookDiffDegree), fTimeDelta * 10.f);
    }
    else if (m_fTurnTimer <= 0.f)
    {
        m_bAttacKTurning = false;
        m_fTurnTimer = 0.1f;
        m_fLookDiffDegree = 0.f;
    }
    return bAttacking;
}

void CChai::Attack_Turn(_float fTimeDelta)
{
    _vector prevPlayerLookVec = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0));
    shared_ptr <CGameObject> pTarget = m_pPlayer_Camera->Get_Lock_On_Target();
    shared_ptr <CGameObject> pGimmic_Target = m_pPlayer_Camera->Get_Gimmic_Lock_On_Target();
    if (!pTarget && !pGimmic_Target)
        return;
    if (!pTarget && pGimmic_Target)
        pTarget = pGimmic_Target;
    shared_ptr <CTransform > pTargetTransform = pTarget->Get_Transform();

    _float fDistance = XMVectorGetX(XMVector3Length(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position()));

    if (fDistance > 4.f)
        return;
    _vector tagetVector = XMVectorSetY(XMVector3Normalize(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position()), 0);

    _vector LookDiffrence = XMVector3Dot(prevPlayerLookVec, XMVector3Normalize(tagetVector));
    m_fLookDiffDegree = XMConvertToDegrees(acos(XMVectorGetX(LookDiffrence)));

    _vector LookCross = XMVector3Cross(prevPlayerLookVec, XMVector3Normalize(tagetVector));
    _float turnFloat = XMVectorGetY(LookCross);
    if (turnFloat < 0)
        m_bTrunRight = false;
    else
        m_bTrunRight = true;

    if (m_fLookDiffDegree > 1.f)
    {
        m_fTurnTimer = 0.1f;
        m_bAttacKTurning = true;
    }
    else
    {
        m_fTurnTimer = 0.f;
        m_bAttacKTurning = false;
    }
    //if (m_bAttacKTurning)
    //    m_pCCT->Add_Displacement(diagonalVec * m_pTransformCom->Get_Speed());
}

_bool CChai::Battle_Engage(_float fTimeDelta)
{
    _bool bEngaged = false;
    bEngaged = CCombat_Manager::Get_Instance()->Is_Combat_Engage();
    if (bEngaged)
    {
        if (m_pModelCom->Get_StringAnimIndex("ch0000_action_054") != m_pModelCom->Get_CurrentAnimIndex() &&
            m_pModelCom->Get_StringAnimIndex("ch0000_action_051") != m_pModelCom->Get_CurrentAnimIndex())
        {
            //if (rand() % 2 == 0)
            //    m_pModelCom->Set_AnimIndexNonCancle("ch0000_action_054", false);
            //else
                m_pModelCom->Set_AnimIndexNonCancle("ch0000_action_051", false);
        }
    }
    return bEngaged;
}

void CChai::Generator_Mode(_float fTimeDelta)
{
    if (m_eEvent_State == CHAI_EVENT_STATE::GENERATOR)
    {
        weak_ptr<CGameObject> pGeneratorSystem = PLAYERMANAGER->Get_Generator_System().lock();
        if (!pGeneratorSystem.expired())
        {
            weak_ptr< CUi_GeneratorSystem> wptrGsys = static_pointer_cast<CUi_GeneratorSystem>(pGeneratorSystem.lock());
            if ((m_pGameInstance.lock()->Get_MouseDown(MOUSEKEYSTATE::DIMK_RBUTTON)
                || m_pGameInstance.lock()->Get_MouseDown(MOUSEKEYSTATE::DIMK_LBUTTON)))
            {
                if (wptrGsys.lock()->Generator_Judge())
                {
                    if (wptrGsys.lock()->Get_Success_Count() == 4)
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_010", false);
                    else
                    {
                        if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_101") == m_pModelCom->Get_CurrentAnimIndex())
                            m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_200", false);
                        else if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_201") == m_pModelCom->Get_CurrentAnimIndex())
                            m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_100", false);
                        else
                            m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_100", false);
                    }
                }
                else
                {
                    if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_101") == m_pModelCom->Get_CurrentAnimIndex())
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_102", false);
                    else if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_201") == m_pModelCom->Get_CurrentAnimIndex())
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_202", false);
                    else if (m_pModelCom->Get_StringAnimIndex("ch0000_event-generator_001") == m_pModelCom->Get_CurrentAnimIndex())
                        m_pModelCom->Set_AnimIndexNonCancle("ch0000_event-generator_002", false);
                }
            }
        }
    }
}

void CChai::SP_Skill(string strSkillName)
{
    if (strSkillName == "Pick_Me_Up" && m_fReverb >= 50.f)
    {
        m_fReverb -= 50.f;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);

        m_bTurning = false;
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp_060", false);
        m_bRender_Can = true;


    	CUi_SkillCutScene::Desc Desc;
    	Desc.CutScenetype = CUi_SkillCutScene::CutSceneType::Chai;
    	Desc.Wordtype = CUi_SkillCutScene::WordType::PickMeUp;
    	CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillCutScene::ObjID, GAMEMANAGER->Get_Current_Level(), L"CUi", CUi_SkillCutScene::ObjID, &Desc);

    }
    else if (strSkillName == "Overdrive_Slash" && m_fReverb >= 100.f)
    {
        m_fReverb -= 100.f;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);

        m_bTurning = false;
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp-rhythm_000", false);

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

        RhythmData.first = 0.5f;
        RhythmData.second = 2;
        Rhythm_list.push_back(RhythmData);

        RhythmData.first = 0.5f;
        RhythmData.second = 2;
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

        CUi_SkillCutScene::Desc scDesc;
        scDesc.CutScenetype = CUi_SkillCutScene::CutSceneType::Chai;
        scDesc.Wordtype = CUi_SkillCutScene::WordType::OverDriveSlash;
        CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillCutScene::ObjID, GAMEMANAGER->Get_Current_Level(), L"CUi", CUi_SkillCutScene::ObjID, &scDesc);
        
    }
    else if (strSkillName == "Pick_Slide" && m_fReverb >= 50.f)
    {
        m_fReverb -= 50.f;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);

        m_bTurning = false;
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp_050", false);


        CUi_SkillCutScene::Desc Desc;
        Desc.CutScenetype = CUi_SkillCutScene::CutSceneType::Chai;
        Desc.Wordtype = CUi_SkillCutScene::WordType::PickSlide;
        CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillCutScene::ObjID, GAMEMANAGER->Get_Current_Level(), L"CUi", CUi_SkillCutScene::ObjID, &Desc);
    }
    else if (strSkillName == "Double_Riff" && m_fReverb >= 50.f)
    {
        m_fReverb -= 50.f;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);

        m_bTurning = false;
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp-cp_000", false);

        shared_ptr<CPeppermint> pPeppermint = static_pointer_cast<CPeppermint>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Peppermint")));
        pPeppermint->SP_Attack(m_pTransformCom->Get_Position());
        pPeppermint->Get_Transform()->LookAt(pPeppermint->Get_Transform()->Get_Position() + m_pTransformCom->Get_NormLook());


        CUi_SkillCutScene::Desc Desc;
        Desc.CutScenetype = CUi_SkillCutScene::CutSceneType::ChaiPepper;
        Desc.Wordtype = CUi_SkillCutScene::WordType::DoubleLeaf;
        CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillCutScene::ObjID, GAMEMANAGER->Get_Current_Level(), L"CUi", CUi_SkillCutScene::ObjID, &Desc);
    }
    else if (strSkillName == "Twin_Assault" && m_fReverb >= 75.f)
    {
        m_fReverb -= 75.f;
        m_bAttack_Input = false;
        m_bNextAttack = false;
        Initialize_Attack_Tree(m_pTree_Attack);
        Initialize_Attack_Tree(m_pTree_Air_Attack);

        m_bTurning = false;
        m_eMoveState = CHAI_MOVE_STATE::EVENT;
        m_pPlayer_Camera->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pModelCom->Get_BoneMatrixPtr("prop_02")));
        m_pModelCom->Set_AnimIndex("ch0000_atk-sp_200", false);

        shared_ptr<CKorsica> pKorsica = static_pointer_cast<CKorsica>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Korsica")));
        pKorsica->SP_Attack(m_pTransformCom->Get_Position());
        pKorsica->Get_Transform()->LookAt(pKorsica->Get_Transform()->Get_Position() + m_pTransformCom->Get_NormLook());

        CUi_SkillCutScene::Desc Desc;
        Desc.CutScenetype = CUi_SkillCutScene::CutSceneType::ChaiKorsica;
        Desc.Wordtype = CUi_SkillCutScene::WordType::TwinAssualt;
        CUi_Mgr::Get_Instance()->Add_Clone(CUi_SkillCutScene::ObjID, GAMEMANAGER->Get_Current_Level(), L"CUi", CUi_SkillCutScene::ObjID, &Desc);
    }

}

CChai::SkillDesc CChai::Skill_Change_Equip(string strEquipSkill)
{
    SkillDesc EquipSkillDesc = {};
    if(strEquipSkill == "Pick_Me_Up")
    {
        EquipSkillDesc.ConsumeReverb = 1;
        EquipSkillDesc.SkillName = L"Pickmeup!";
        EquipSkillDesc.SkillType = SkillType::Recover;
    }
    else if (strEquipSkill == "Overdrive_Slash")
    {
        EquipSkillDesc.ConsumeReverb = 4;
        EquipSkillDesc.SkillName = L"Overdrive!";
        EquipSkillDesc.SkillType = SkillType::Attack;
    }
    else if (strEquipSkill == "Pick_Slide")
    {
        EquipSkillDesc.ConsumeReverb = 1;
        EquipSkillDesc.SkillName = L"PickSlide!";
        EquipSkillDesc.SkillType = SkillType::Attack;
    }
    else if (strEquipSkill == "Double_Riff")
    {
        EquipSkillDesc.ConsumeReverb = 1;
        EquipSkillDesc.SkillName = L"DoubleRiff!";
        EquipSkillDesc.SkillType = SkillType::Attack;
    }
    else if (strEquipSkill == "Twin_Assault")
    {
        EquipSkillDesc.ConsumeReverb = 3;
        EquipSkillDesc.SkillName = L"TwinAssault!";
        EquipSkillDesc.SkillType = SkillType::Attack;
    }

    return EquipSkillDesc;
}

string CChai::Translate_Skill_Info(SkillDesc skillDesc)
{
    string strSkillName = {};
    if (skillDesc.SkillName == L"Pickmeup!")
        strSkillName = "Pick_Me_Up";

    else if (skillDesc.SkillName == L"Overdrive!")
        strSkillName = "Overdrive_Slash";

    else if (skillDesc.SkillName == L"PickSlide!")
        strSkillName = "Pick_Slide";

    else if (skillDesc.SkillName == L"DoubleRiff!")
        strSkillName = "Double_Riff";

    else if (skillDesc.SkillName == L"TwinAssault!")
        strSkillName = "Twin_Assault";

    return strSkillName;
}

void CChai::Call_Assist(_float fTimedelta)
{
    PxVec3 pos = { (_float)m_pCCT->Get_Controller()->getFootPosition().x,(_float)m_pCCT->Get_Controller()->getFootPosition().y + 0.1f,(_float)m_pCCT->Get_Controller()->getFootPosition().z };
    PxRaycastBuffer hit;
    PxVec3 origin = pos;
    PxVec3 direction = PxVec3(0, -1, 0); // 아래 방향으로 레이캐스팅
    PxReal distance = 30.f; // 계단의 높이보다 살짝 큰 값
    direction.normalize();
    bool status = GAMEINSTANCE->Get_Scene()->raycast(origin, direction, distance, hit);

    CPlayer_Manager::Assist_Struct assistData = PLAYERMANAGER->Get_Assist_Struct((_int)PLAYERMANAGER->Get_Assist());

    if (assistData.bIs_CoolTime == true)
        return;
    if (status)
    {
        PxVec3 hitPos = hit.block.position;
        if (GAMEINSTANCE->Get_KeyDown(DIK_V))
        {
            if ((_int)PLAYERMANAGER->Get_Assist() == 0)
            {
                shared_ptr<CPeppermint> pPeppermint = static_pointer_cast<CPeppermint>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Peppermint")));
                pPeppermint->Assist_Attack(XMVectorSet(hitPos.x, hitPos.y, hitPos.z, 1));
            }
            else if ((_int)PLAYERMANAGER->Get_Assist() == 1)
            {
                if (m_bLift_Macaron)
                {
                    if (m_fEventPosition.x < 0.f)
                    {
                        // left
                        _vector eventPos = XMLoadFloat4(&m_fEventPosition);
                        shared_ptr<CMacaron> pMacaron = static_pointer_cast<CMacaron>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Macaron")));
                        pMacaron->Lift_Attack(eventPos, eventPos + XMVectorSet(-1, 0, 0, 0) * 2.f);
                    }
                    else
                    {
                        // Right
                        _vector eventPos = XMLoadFloat4(&m_fEventPosition);
                        shared_ptr<CMacaron> pMacaron = static_pointer_cast<CMacaron>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Macaron")));
                        pMacaron->Lift_Attack(eventPos, eventPos + XMVectorSet(1, 0, 0, 0) * 2.f);
                    }
                }
                else
                {
                    shared_ptr<CMacaron> pMacaron = static_pointer_cast<CMacaron>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Macaron")));
                    pMacaron->Assist_Attack(XMVectorSet(hitPos.x, hitPos.y, hitPos.z, 1));
                }
            }
            else if ((_int)PLAYERMANAGER->Get_Assist() == 2)
            {
                shared_ptr<CKorsica> pKorsica = static_pointer_cast<CKorsica>(CPlayer_Manager::Get_Instance()->Get_Assist(TEXT("Korsica")));
                pKorsica->Assist_Attack(XMVectorSet(hitPos.x, hitPos.y, hitPos.z, 1));
            }
        }
    }
}

void CChai::Adjust_Idle_Timing()
{
    _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float currentTimeRatio = fTimer / fBeat;
    if (m_eEvent_State != CHAI_EVENT_STATE::RHYTHMPARRY)
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_idle_000", true);
    else
        m_pModelCom->Set_AnimIndexNonCancle("ch0000_idle_200", true);
    m_pModelCom->Set_CurrentTrackPosition(m_pModelCom->Get_Duration() * currentTimeRatio * 0.25f);
    _float curTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();
}

void CChai::Adjust_Idle_Timing_Hard()
{
    _float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float currentTimeRatio = fTimer / fBeat;
    if (m_eEvent_State != CHAI_EVENT_STATE::RHYTHMPARRY)
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_idle_000", true);
    else
        m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_idle_200", true);
    m_pModelCom->Set_CurrentTrackPosition(m_pModelCom->Get_Duration() * currentTimeRatio * 0.25f);
    _float curTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();
}

_bool CChai::Find_R()
{
    for (auto& iter : m_vecCombo)
    {
        if (iter == ATTACK_TREE_TYPE::R)
            return true;
    }
    return false;
}

void CChai::Traverse_AttacK_Tree(AttacK_Tree* node, ATTACK_TREE_TYPE eAttackType, vector<ATTACK_TREE_TYPE> vecCombo)
{
    if (!node)
        return;

    _bool temp = true;
    for (AttacK_Tree* child : node->children)
    {
        if (child && child->bActivate == true)
        {
            Traverse_AttacK_Tree(child, eAttackType, vecCombo);
            temp = false;
            return;
        }
    }

    if (temp)
    {
        if (node->children[eAttackType])
        {
            node->children[eAttackType]->bActivate = true;
            m_vecCombo.push_back(eAttackType);
        }
    }
}

void CChai::Rhythm_Parry_Mode(_float fTimeDelta)
{
    if (m_eEvent_State != CHAI_EVENT_STATE::RHYTHMPARRY)
        return;

    _int CurBeat = (_int)CBeat_Manager::Get_Instance()->Get_BeatCount();
    _float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float BeatPercent = Timer / Beat;
    _float JudgeBeat = 0.f;

    JudgeBeat = CurBeat - (m_iAttackReadyStartBeat);

    if (m_vecBattleTiming.size() > 0)
    {
        if (m_bValid)
        {
            if (m_vecParry[0])
            {
                if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
                {
                    m_fCoolDown = CBeat_Manager::Get_Instance()->Get_Beat() * .25f;
                    m_bValid = false;
                    m_fReactTimer = CBeat_Manager::Get_Instance()->Get_Beat();
                }
            }
            else
            {
                if (m_pGameInstance.lock()->Get_KeyDown(DIK_LSHIFT))
                {
                    //Set_Anim(AnimType::Evade);
                    m_fCoolDown = CBeat_Manager::Get_Instance()->Get_Beat() * .25f;
                    m_bValid = false;
                    m_fReactTimer = CBeat_Manager::Get_Instance()->Get_Beat();
                }
            }
        }
        if (m_vecBattleTiming[0] <= JudgeBeat + BeatPercent)
        {
            m_vecBattleTiming.erase(m_vecBattleTiming.begin());
            m_vecParry.erase(m_vecParry.begin());
            m_bValid = true;
        }
    }
}
void CChai::ZiplineMove(_float fTimeDelta)
{
    auto gameinstance = m_pGameInstance.lock();
    if (gameinstance->Get_KeyDown(DIK_A))
        m_pModelCom->Set_AnimIndex("ch0000_event-magnetrail_040", false);
    else if (gameinstance->Get_KeyDown(DIK_D))
        m_pModelCom->Set_AnimIndex("ch0000_event-magnetrail_011", false);
    else if (gameinstance->Get_KeyDown(DIK_SPACE))
        m_pModelCom->Set_AnimIndex("ch0000_event-magnetrail_040", false);
}
void CChai::BuiltIn_Magnet_Move()
{
}
void CChai::JumpPad_Move(_float fTimeDelta)
{
    if (GAMEINSTANCE->Get_KeyDown(DIK_SPACE) && m_pCommandIcon)
    {
        CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon> (m_pCommandIcon)->Cal_Judge();
        if (eWordType == CUi_JudgeWord::WordType::Perfect)
        {
            m_pCCT->Set_Velocity(XMVectorSet(0.f, 50.f, 0, 0));
            m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_010", false);
        }

        else if (eWordType == CUi_JudgeWord::WordType::Good)
        {
            m_pCCT->Set_Velocity(XMVectorSet(0.f, 50.f, 0, 0));
            m_pModelCom->Set_AnimIndex("ch0000_mvnt-jump_010", false);
        }
        m_pCommandIcon = nullptr;
    }
    PxControllerState pCS;
    m_pCCT->Get_Controller()->getState(pCS);
    if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        if (m_eEvent_State == CHAI_EVENT_STATE::JUMPPAD)
        {
            m_eEvent_State = CHAI_EVENT_STATE::EVENT_NONE;
            m_bIsEvent = false;
            m_pModelCom->Set_AnimIndexNonCancle("ch0000_mvnt-jump_002", false);
            EFFECTMANAGER->Call_Fixed_Union_Effect("LandingSmoke", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
            m_bNextAttack = false;
            m_vecCombo.clear();
            Initialize_Attack_Tree(m_pTree_Attack);
            Initialize_Attack_Tree(m_pTree_Air_Attack);
        }
    }
}
void CChai::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(&m_matCurrentPos));
    CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier * m_fTimeMultiflier, isBillBoard);
}
void CChai::Start_AttackSequence(_float iBeat, _int iAttackReadyStartBeat, const vector<_float>& vecTimer, const vector<_bool>& vecParry)
{
    m_vecBattleTiming = vecTimer;
    m_vecParry = vecParry;
    m_iAttackReadyStartBeat = iAttackReadyStartBeat + iBeat;
    m_bValid = true;
}

void CChai::Set_DamagedAnim()
{
    m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_damage_010", false);
    m_fCoolDown = 0.1f * CBeat_Manager::Get_Instance()->Get_Beat();
}

void CChai::Set_ParryAnim()
{
    m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_action_030", false);
    EFFECTMANAGER->Call_Union_Effect("Chai_Parry", m_pTransformCom->Get_WorldMatrix_Ptr(), m_fTime_Offset);
}

void CChai::Set_EvadeAnim()
{
    m_pModelCom->Set_AnimIndex_NoInterpole("ch0000_event-kale_111", false);
}

void CChai::Set_DanceBattle_Anim(_int iIndex)
{
    switch (iIndex)
    {
    case 0:
    {
        m_pModelCom->Set_AnimIndex("ch0000_event-mimosa_010", false);
        break;
    }
    case 1:
    {
        m_pModelCom->Set_AnimIndex("ch0000_event-mimosa_020", false);
        break;
    }
    case 2:
    {
        m_pModelCom->Set_AnimIndex("ch0000_event-mimosa_030", false);
        break;
    }
    case 3:
    {
        m_pModelCom->Set_AnimIndex("ch0000_event-mimosa_040", false);
        break;
    }
    case 4:
    {
        m_pModelCom->Set_AnimIndex("ch0000_event-mimosa_060", false);
        break;
    }
    }
}