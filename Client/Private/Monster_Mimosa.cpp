#include "Monster_Mimosa.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Shader.h"
#include "Model.h"
#include "CharacterController.h"

#include "GameManager.h"
#include "Beat_Manager.h"
#include "Effect_Manager.h"
#include "MapManager.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"


#include "CombatManager.h"
#include "PickUpItem.h"
#include "GimmickBase.h"

#include "PartObject.h"


#include "Chai.h"
#include "Player_Camera.h"
#include "CUi_EnemySpecialAttack.h"
#include "CUi_MimosaSystem.h"
#include "CUi_SpecialComboAttackSystem.h"
#include "Weapon_Mimosa.h"
#include "CUi_MImosaMusicBattleSystem.h"
#include "CUi_MimosaCutin.h"
#include "Cine_Manager.h"
#include "Mimosa_Letter_Bomb.h"
#include "BackDancer.h"
#include "Mimosa_SpotLight.h"
#include "Mirror_Ball.h"
#include "Texture.h"
#include "UI_Event_Cut.h"
#include "CUi_TalkSystem.h"


#include "iostream"

wstring CMonster_Mimosa::ObjID = TEXT("CMonster_Mimosa");

CMonster_Mimosa::CMonster_Mimosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonsterBase{ pDevice, pContext }
{
}

CMonster_Mimosa::CMonster_Mimosa(const CMonster_Mimosa& rhs)
    : CMonsterBase{ rhs }
{
}

CMonster_Mimosa::~CMonster_Mimosa()
{
    Free();
}

HRESULT CMonster_Mimosa::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    _matrix		LocalTransformMatrix = XMMatrixIdentity();
    LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Monster_Mimosa"), TEXT("../Bin/Resources/Models/Monsters/Mimosa/mimosa.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Parry_GT"), TEXT("../Bin/Resources/Sounds/SFX/Parry_GT.wav")));
    //m_List_Owning_ObjectID.push_back(CSaber_Weapon::ObjID);

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsMimosa1_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsMimosa/vsMimosaChapter1_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsMimosa1_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsMimosa/vsMimosaChapter1_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsMimosa2_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsMimosa/vsMimosaChapter2_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsMimosa3_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsMimosa/vsMimosaChapter3_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsMimosa3_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsMimosa/vsMimosaChapter3_2.ogg")));

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Wing_Shield_Hit"), TEXT("../Bin/Resources/Models/Monsters/Mimosa/Sound/Shield_Hit.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Wing_Shield_Break"), TEXT("../Bin/Resources/Models/Monsters/Mimosa/Sound/Shield_Break.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Mimosa_SpotLight"), TEXT("../Bin/Resources/Models/Monsters/Mimosa/Sound/SpotLight.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Mimosa_Damaged"), TEXT("../Bin/Resources/Models/Monsters/Mimosa/Sound/Damage.ogg")));
    
    return S_OK;
}

HRESULT CMonster_Mimosa::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    PrepareCallbacks();
    Initialize_Weapon();
    // Init Hp
    m_fCurHp = m_fMaxHp;

    m_pMat_Camera = m_pModelCom->Get_BoneMatrixPtr("prop_02");
    m_pChai_Attach = m_pModelCom->Get_BoneMatrixPtr("prop_01");

    m_pCCT->Enable_Pseudo_Physics();
    m_pCCT->Enable_Gravity();
    m_pCCT->Set_Gravity(40.f);

    // For tool setted effects
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fTime_Offset, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), m_fTime_Offset, isBillBoard);
        }
    );

    CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());

    m_pModelCom->Set_AnimIndex("em5100_idle-air_000",true );
    m_strCurrent_Pattern = "em5100_atk-air_020";
    m_strNext_Pattern = "em5100_atk-air_020";
    m_pCCT->Get_Controller()->getActor()->setName("Monster");
    Initialize_Dance_Event();
    Initialize_Ui();

    // Init cineCallbacks
    auto callbackRemoveUI = [&]() {
        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
    };
    CCine_Manager::Get_Instance()->AddStartCallback("CS0820", callbackRemoveUI);
    CCine_Manager::Get_Instance()->AddStartCallback("CS0825", callbackRemoveUI);
    CCine_Manager::Get_Instance()->AddStartCallback("CS0830", callbackRemoveUI);
    CCine_Manager::Get_Instance()->AddStartCallback("CS0835", callbackRemoveUI);
    CCine_Manager::Get_Instance()->AddStartCallback("CS0840", callbackRemoveUI);

    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0820", 18, 0,
        [&](_float4, _float4, _float3) {
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0820", 22, 0,
        [&](_float4, _float4, _float3) {
            BEATMANAGER->Queue_Sequence(L"Mimosa_Intro");
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0820", 22, 1,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::MimosaIntro;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0820", 22, 2,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0835", 1, 0,
        [&](_float4, _float4, _float3) {
            GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_DanceBattle_Loop");
            BEATMANAGER->Queue_Sequence(L"Mimosa_Verse3_Vo_1");
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0840", 10, 0,
        [&](_float4, _float4, _float3) {
            // TODO: 상현아!!!@!@@! 폭발~~~
            CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this()); Dead();
        }
    );

    GAMEINSTANCE->Add_Unique_Sound_Channel("BattleMimosa");
    // Add sound channel
    GAMEINSTANCE->Add_Unique_Sound_Channel("Mimosa_Hit");
    return S_OK;
}

void CMonster_Mimosa::PriorityTick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
        return;
    else


        XMStoreFloat4(&m_vPlayerPos, PLAYERMANAGER->Get_PlayerPos());
    m_pWeapon_Mic->PriorityTick(fTimeDelta);

    if(GAMEINSTANCE->Get_KeyDown(DIK_0))
		Spawn_Mirror_Ball();

}

void CMonster_Mimosa::Tick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
        return;
    // Play Animation
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    if (m_bRP_Ready)
        m_fRP_Ready_Timer += fTimeDelta;

    if (m_bFinish && m_fFinish_Timer > 0.f)
        m_fFinish_Timer -= fTimeDelta;
    else if( m_fFinish_Timer < 0.f)
    {
        m_fFinish_Timer = 0.f;
        CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        CCine_Manager::Get_Instance()->StartCine("CS0840");
    }

    if(m_fRP_Ready_Timer > 2.f && m_bRP_Ready)
    {
        m_pTransformCom->Set_Position( _float4(-7.f, -2.f, 2.f, 1.f));
        m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());

        m_pTransformCom->LookAt_Horizontal({ -4.f,-2.f,2.f,1 });
        m_bRP_Ready = false;
        Start_RP();
    }

    if (m_fPhaseChange_Timer > 0.f)
        m_fPhaseChange_Timer -= fTimeDelta;
    else if(m_fPhaseChange_Timer < 0.f)
    {
        m_fPhaseChange_Timer = 0;
        if (m_iPhase == 2)
        {
            BEATMANAGER->Queue_Sequence(L"Mimosa_Verse2_Bridge");
            CCine_Manager::Get_Instance()->StartCine("CS0825");

            m_pModelCom->Set_AnimIndex("em5100_event-dance_100", false);
            m_bInvincivle = false;
            m_fPatternTimer = 0.f;
            m_bPattern_Timer_On = false;
        }
        else if (m_iPhase == 3)
        {
            BEATMANAGER->Queue_Sequence(L"Mimosa_DanceBattle_Intro");
            CCine_Manager::Get_Instance()->StartCine("CS0830");
        }
    }
    if (m_pModelCom->IsRootAnim() && !m_pModelCom->IsForcedNonRoot())
    {
        _matrix matResult = XMLoadFloat4x4(&fmatResult);
        auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);

        _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
        auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
        m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
        //m_pTransformCom->Set_Position(vCurPos);
    }
    m_fTrackFrame = m_pModelCom->Get_Current_Track_Position();

    Update_Lazer(fTimeDelta);

    if(!m_bDance_Event && !m_bRhythm_Parry && m_iPhase != 2)
    {
        Animation_End();
        Animation(fTimeDelta);
    }
    else if (!m_bDance_Event && m_bRhythm_Parry)
    {
        Rhythm_Parry_Animation_End(fTimeDelta);
        Rhythm_Parry_Animation(fTimeDelta);
    }
    else if (m_iPhase == 2)
    {
        _bool bAttack_End = false;
        if (m_fPhase2_Timer > 0.f)
            m_fPhase2_Timer -= fTimeDelta;
        else if (m_fPhase2_Timer < 0.f)
        {
            m_fPhase2_Timer = 0.f;
            Phase2_SpotLight_Pattern(fTimeDelta);
        }
        if(!m_vecBackDancer.empty())
        {
            for (auto& iter : m_vecBackDancer)
                bAttack_End = static_pointer_cast<CBackDancer>(iter)->Get_Attack_End();

            if (bAttack_End)
            {
                m_pSpotLight->Dead();
                m_pSpotLight = nullptr;
                for (auto& iter : m_vecBackDancer)
                    iter->Dead();
                m_vecBackDancer.clear();
                _float fBeat = BEATMANAGER->Get_Beat();
                m_fPhase2_Timer = 2.f * fBeat;
            }
        }
        if (m_fSpotlight_Timer > 0.f)
        {
            m_fSpotlight_Timer -= fTimeDelta;
            if (m_fSpotlight_Timer < 0.f)
            {
                if(m_pSpotLight)
                     static_pointer_cast<CMimosa_SpotLight>(m_pSpotLight)->Change_Color(true);
                m_pModelCom->Set_AnimIndex("em5100_mvnt-walk_000", false);
                m_fSpotlight_Timer = 0.f;
                Animation_End();
                Animation(fTimeDelta);
            }
        }
        else
        {
            Animation_End();
            Animation(fTimeDelta);
        }
    }
    else
    {
        Dance_Event_Animation_End(fTimeDelta);
        Dance_Event_Animation(fTimeDelta);
    }
    // Update Physics
    if (m_isCCTWork)
        m_pCCT->Update(fTimeDelta);

    // Update Colliders
    auto matrixSpine = XMLoadFloat4x4(m_pMat_Spine);
    matrixSpine.r[0] = XMVector3Normalize(matrixSpine.r[0]);
    matrixSpine.r[1] = XMVector3Normalize(matrixSpine.r[1]);
    matrixSpine.r[2] = XMVector3Normalize(matrixSpine.r[2]);
    m_pCollider_Hit->Update(matrixSpine * m_pTransformCom->Get_WorldMatrix_XMMat());

    auto matrixRightHand = XMLoadFloat4x4(m_pMat_RightHand);
    matrixRightHand.r[0] = XMVector3Normalize(matrixRightHand.r[0]);
    matrixRightHand.r[1] = XMVector3Normalize(matrixRightHand.r[1]);
    matrixRightHand.r[2] = XMVector3Normalize(matrixRightHand.r[2]);
    auto matAttackResult = matrixRightHand * m_pTransformCom->Get_WorldMatrix_XMMat();
    m_pCollider_Attack->Update(matAttackResult);

    // Call all reserved effects
    XMStoreFloat4x4(&m_matCurrentPos, m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pModelCom->PlayReservedCallbacks();

    Collision_Down(fTimeDelta);
    LookPlayerPos( fTimeDelta);
    DownState(fTimeDelta);
    if (!m_bDance_Event && !m_bRhythm_Parry)
         Pattern_Change(fTimeDelta);

    if (m_bDance_Ready)
    {
        m_fDanceBattle_Next_Timer += fTimeDelta;
        if (m_fDanceBattle_Next_Timer > ((m_list_Mimosa_Dance_Delay.front() * 2.f) + 2.f) * BEATMANAGER->Get_Beat())
        {
            m_fDanceBattle_Next_Timer = 0;
            m_list_Mimosa_Dance_Delay.pop_front();
            Start_Dance();
        }
    }
    if (GAMEINSTANCE->Get_KeyDown(DIK_L))
    {
        auto pGI = m_pGameInstance.lock();
        _float4x4 EffectMat;
        XMStoreFloat4x4(&EffectMat, m_pTransformCom->Get_WorldMatrix_XMMat());
        EffectMat.m[3][1] += 2.f;
        _float fBeat = BEATMANAGER->Get_Beat();
        _float4 fPos = {};
        CMimosa_Letter_Bomb::MiMOSA_LETTER ls = {};
        XMStoreFloat4(&fPos, m_pTransformCom->Get_NormLook());
        ls.fMimosaLookDir = fPos;
        ls.iLetterModel = m_iLetterIdx;
        m_iLetterIdx++;
        ls.fShootDelay = fBeat * 8.f;
 
        _vector bonePos = m_pTransformCom->Get_WorldMatrix_XMMat().r[3];
        bonePos += m_pTransformCom->Get_NormLook() * 0.5f;
        XMStoreFloat4(&fPos, bonePos);
        fPos.y += 2.f;
        ls.fCenterPos = fPos;
        pGI->Push_Object_From_Pool(CMimosa_Letter_Bomb::ObjID, pGI->Get_Current_LevelID(), L"Effect", &ls, &EffectMat);
    }
    if (!m_bFlyState)
    {
        if (GAMEINSTANCE->Get_KeyDown(DIK_1))
        {
            m_bPlayerLook = false;
            m_pModelCom->Set_AnimIndex("em5100_atk_000", false);
        }
        if (GAMEINSTANCE->Get_KeyDown(DIK_2))
            m_pModelCom->Set_AnimIndex("em5100_atk_010", false);
        if (GAMEINSTANCE->Get_KeyDown(DIK_3))
        {
            m_bPlayerLook = false;
            GravityOff();
            m_pModelCom->Set_AnimIndex("em5100_atk_120", false);
        }
        if (GAMEINSTANCE->Get_KeyDown(DIK_4))
        {
            GravityOff();
            m_pModelCom->Set_AnimIndex("em5100_idle-air_052", false);
        }
        if (!m_bDance_Event && GAMEINSTANCE->Get_KeyDown(DIK_5))
        {
            m_bPlayerLook = false;
            m_bPattern_Timer_On = false;
            m_fPatternTimer = 0.f;

            m_bDance_Event = true;
            Start_Dance();
            GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain");
            BEATMANAGER->Reset();
            GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_DanceBattle_Loop", true);
            CUi_SpecialComboAttackSystem::SPComboAttack spAttack = {};
            spAttack.eSPCombo_Type = CUi_SpecialComboAttackSystem::SPCombo_Type::Mimosa;
            CUi_Mgr::Get_Instance()->Add_Clone(CUi_SpecialComboAttackSystem::ObjID,
                m_iLevelIndex, L"CUi", CUi_SpecialComboAttackSystem::ObjID,&spAttack);
            CUi_Mgr::Get_Instance()->Add_Clone(CUi_MImosaMusicBattleSystem::ObjID,
                m_iLevelIndex, L"CUi", CUi_MImosaMusicBattleSystem::ObjID);
            m_bDance_Ready = true;

            PLAYERMANAGER->Callback_Event("Mimosa_DanceBattle");
        }
        else if (m_bDance_Event && GAMEINSTANCE->Get_KeyDown(DIK_5))
        {
            Start_Dance();
            //m_bDance_Ready = true;
        }
        if (GAMEINSTANCE->Get_KeyDown(DIK_7))
        {
            //m_SpotLight = EFFECTMANAGER->Call_Union_Effect_Return("SpotLight_Enemy", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
            PLAYERMANAGER->Callback_Event("Mimosa_SpotLight");
            Phase2_SpotLight_Pattern(fTimeDelta);
            m_iPhase = 2;
            m_fPatternTimer = 0.f;
            m_bPattern_Timer_On = false;
        }
        if (GAMEINSTANCE->Get_KeyDown(DIK_6))
        {
            GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain");
            BEATMANAGER->Reset();
            GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_RP_Ready", false);
            Wing_On(0.1f);
            GravityOff();
            m_bFlyState = true;
            m_bRhythm_Parry = true;
            m_pModelCom->Set_AnimIndex_NoInterpole("em5100_atk-rp-cp_000", false);
            PLAYERMANAGER->Callback_Event("Mimosa_RP");
            m_fTurnTimer = 1.f;
            m_bTurn = false;
            m_bPlayerLook = false;

            m_pModelCom->Set_AnimFrame(0.01f);

            auto pMainCam = static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main")));
            pMainCam->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pMat_Camera));

            shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());
            
            _matrix matBone = XMLoadFloat4x4(m_pChai_Attach);
            matBone.r[0] = XMVector3Normalize(matBone.r[0]);
            matBone.r[1] = XMVector3Normalize(matBone.r[1]);
            matBone.r[2] = XMVector3Normalize(matBone.r[2]);
            _matrix vMat = matBone * m_pTransformCom->Get_WorldMatrix_XMMat();

            _vector vVec = vMat.r[3];
            vVec = XMVectorSetY(vVec, XMVectorGetY(vVec));
            vMat.r[3] = vVec;
            pPlayer->Set_Player_Position(vVec);

            pPlayer->Get_Transform()->LookAt_Horizontal(pPlayer->Get_Transform()->Get_Position() - m_pTransformCom->Get_NormLook());

        }
    }
    else
    {
        if (GAMEINSTANCE->Get_KeyDown(DIK_4))
            m_pModelCom->Set_AnimIndex("em5100_idle-air_053", false);
        if (GAMEINSTANCE->Get_KeyDown(DIK_1))
            m_pModelCom->Set_AnimIndex("em5100_atk-air_020", false);
        if (GAMEINSTANCE->Get_KeyDown(DIK_2))
            m_pModelCom->Set_AnimIndex("em5100_atk-air_100", false);
        if (GAMEINSTANCE->Get_KeyDown(DIK_3))
        {
            m_bPlayerLook = false;
            m_pModelCom->Set_AnimIndex("em5100_dmg-air_300", false);
        }
    }
    // Update PartObjects
    m_pWeapon_Mic->Tick(fTimeDelta);
}

void CMonster_Mimosa::LateTick(_float fTimeDelta)
{

    m_pWeapon_Mic->LateTick(fTimeDelta);
    Calc_ViewDepth();

    if (!CCine_Manager::Get_Instance()->IsInGame()) 
        return;
    if(m_bRhythm_Parry)
     Judge_Attack(fTimeDelta);

    GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Hit);
    if (m_isAttackOn)
        GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_MONSTER, m_pCollider_Attack);

    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMonster_Mimosa::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    _float fViewDepth = Get_ViewDepth();
    _float fOutlineWidth = 0.8f;
    if (fViewDepth <= 10.f)
    {
        fOutlineWidth = 0.4f;
    }

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

        if (i == 25 || i == 26 || i == 32)
        {
            m_pWingTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture");
            _float4 vColor = { 0.f,1.f,1.f,1.f };
            m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::MimosaWing);

            m_pModelCom->Draw((_uint)i);

            continue;
        }


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

void CMonster_Mimosa::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
    if (pCaller == m_pCollider_Attack)
    {
        if(m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em5100_atk_123"))
        {
            m_pModelCom->Set_AnimIndex_NoInterpole("em5100_event-mimosa_000",false);
            GravityOff();

            m_fTurnTimer = 1.f;
            m_bTurn = false;
            m_bPlayerLook = true;

            auto pMainCam = dynamic_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main")));
            pMainCam->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pMat_Camera));

            m_pModelCom->Set_AnimFrame(0.01f);

            DamageInfo damageInfo{};
            damageInfo.eAttackType = EAttackType::Special;
            damageInfo.pObjectOwner = shared_from_this();
            damageInfo.strAttackName = "Mimosa_Slap";

            //CCT_OFF();
           
            pOther->Get_Owner()->TakeDamage(damageInfo);
        }    
    }
}

void CMonster_Mimosa::TakeDamage(const DamageInfo& damageInfo)
{
    if (m_bInvincivle)
        return;
    _float fTargetHp = m_fCurHp - damageInfo.fAmountDamage;
    PLAYERMANAGER->Add_PlayerRankScore(damageInfo.fAmountDamage / 1.5f);
    PLAYERMANAGER->Add_PlayerScore((_int)damageInfo.fAmountDamage * 10.f);
    /* Play electric effect
    if (m_fCurHp > 0.f && fTargetHp <= 0.f)
        PlayUnionEffect("", 1.f, false);
    */
    CCombat_Manager::Get_Instance()->Add_Combo();

    if (m_iPhase == 2)
    {
        _float4x4 matResult{};
        _matrix mMat = m_pTransformCom->Get_WorldMatrix_XMMat();
        _float3 vScale = m_pTransformCom->Get_Scale();
        _vector vLook = PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position() - m_pTransformCom->Get_Position();
        //if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
        //    return;
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        _vector vRight = XMVector3Cross(vUp, vLook);
        vLook = XMVector3Cross(vRight, vUp);

        mMat.r[0] = XMVector3Normalize(vRight) * vScale.x;
        mMat.r[1] = XMVector3Normalize(vUp) * vScale.y;
        mMat.r[2] = XMVector3Normalize(vLook) * vScale.z;

        mMat.r[3] = mMat.r[3] + XMVector3Normalize(vLook) * 0.6f;
        XMStoreFloat4x4(&matResult, mMat);
        matResult.m[3][1] = matResult.m[3][1] + 1.f;
        CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect("Mimosa_Barrier_Parry", &matResult, 1.f, false);
        m_fCurHp = fTargetHp;
    }
    else if(m_fBarrier <= 0.f)
    {
        if (!GAMEINSTANCE->Is_Sound_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"Mimosa_Damaged"))
            GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Mimosa_Damaged");
        m_bPlayerLook = false;
        m_fCurHp = fTargetHp;

        _vector playerPos =PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position();
        _vector mimosaPos = m_pTransformCom->Get_Position();
        playerPos =  XMVectorSetY(playerPos,0.f);
        mimosaPos = XMVectorSetY(mimosaPos,0.f);
        _vector vDir = XMVector3Normalize(mimosaPos - playerPos);
        m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + vDir );
        m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
        if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_000") ||
            m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_300"))
            m_pModelCom->Set_AnimIndex("em5100_dmg-down_300",false);
    }
    else
    {
        _float4x4 matResult{};
        _matrix mMat = m_pTransformCom->Get_WorldMatrix_XMMat();
        _float3 vScale = m_pTransformCom->Get_Scale();
        _vector vLook = PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position() - m_pTransformCom->Get_Position();
        //if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
        //    return;
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        _vector vRight = XMVector3Cross(vUp, vLook);
        vLook = XMVector3Cross(vRight, vUp);

        mMat.r[0] = XMVector3Normalize(vRight) * vScale.x;
        mMat.r[1] = XMVector3Normalize(vUp) * vScale.y;
        mMat.r[2] = XMVector3Normalize(vLook) * vScale.z;

        mMat.r[3] = mMat.r[3] + XMVector3Normalize(vLook) * 0.6f;
        XMStoreFloat4x4(&matResult, mMat);
        matResult.m[3][1] = matResult.m[3][1] + 1.f;
        CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect("Mimosa_Barrier_Parry", &matResult, 1.f, false);

        m_fBarrier -= damageInfo.fAmountDamage;
        if(m_fBarrier <= 0.f)
        {
            static_pointer_cast<CWeapon_Mimosa>(m_pWeapon_Mic)->Set_Render(false);
            GAMEINSTANCE->Play_Unique_Sound_Channel("Mimosa_Hit", SOUND_CHANNEL::SOUND_EFFECT, L"Wing_Shield_Break");
        }
        else
            GAMEINSTANCE->Play_Unique_Sound_Channel("Mimosa_Hit", SOUND_CHANNEL::SOUND_EFFECT, L"Wing_Shield_Hit");
        if (!m_bTalkStartBarrier)
        {
            m_bTalkStartBarrier = true;
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
                PRequiredTalkSystem->LoadTalkFile(L"VsMimosa_Talk_File_1.csv");

                CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
            }
        }

        if (m_fBarrier <= 0.f)
        {
            m_fPatternTimer = 0.f;
            m_bPattern_Timer_On = false;
            m_bPlayerLook = false;
            m_pModelCom->Set_AnimIndexNonCancle("em5100_dmg-air_300", false);
            GravityOff();

            if (!m_bTalkEndBarrier)
            {
                m_bTalkEndBarrier = true;
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
                    PRequiredTalkSystem->LoadTalkFile(L"VsMimosa_Talk_File_2.csv");

                    CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
                }
            }
        }
    }

    if (m_fCurHp <= 200.f && m_iPhase == 1)
    {
        m_bInvincivle = true;
        m_fCurHp = 200.f;
        m_iPhase = 2;
        CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::MIMOSA_PHASE::PHASE_2);
        m_fPatternTimer = 0.f;
        m_bPattern_Timer_On = false;
        m_bPlayerLook = false;
        PLAYERMANAGER->Bullet_Time(2.f, 0.2f, shared_from_this());
        PLAYERMANAGER->Bullet_Time(2.f, 0.2f, PLAYERMANAGER->Get_Player());
        static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Zoom_In(2.f, 0.5f);
        m_fPhaseChange_Timer = 2.f;
    }
    else if (m_fCurHp <= 100.f && m_iPhase == 2)
    {
        m_bInvincivle = true;

        m_pModelCom->Set_AnimIndex("em5100_dmg-down_300", false);
        Wing_Off(0.1f);
        if(m_pSpotLight)
            m_pSpotLight->Dead();
        m_pSpotLight = nullptr;
        for (auto& iter : m_vecBackDancer)
            iter->Dead();
        m_vecBackDancer.clear();

        PLAYERMANAGER->Callback_Event("Mimosa_SpotLight_End");

        m_fCurHp = 100.f;
        m_iPhase = 3;
        CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::MIMOSA_PHASE::PHASE_3);
        CMapManager::Get_Instance()->Boss_ChangePattern((_uint)CBoss_InteractObj::MIMOSA_PATTERN::DANCEBATTLE);
        m_fPatternTimer = 0.f;
        m_bPattern_Timer_On = false;
        m_bPlayerLook = false;
        PLAYERMANAGER->Bullet_Time(2.f, 0.2f, shared_from_this());
        PLAYERMANAGER->Bullet_Time(2.f, 0.2f, PLAYERMANAGER->Get_Player());
        static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Zoom_In(2.f, 0.5f);
        m_fPhaseChange_Timer = 2.f;
    }
    else if (m_fCurHp <= 0.f && m_iPhase == 3)
    {
        m_fCurHp = 0.f;
        PLAYERMANAGER->Bullet_Time(2.f, 0.2f, shared_from_this());
        PLAYERMANAGER->Bullet_Time(2.f, 0.2f, PLAYERMANAGER->Get_Player());
        static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Zoom_In(2.f, 0.5f);
        m_bRP_Ready = true;
    }
}

HRESULT CMonster_Mimosa::Ready_PartObjects()
{
    //CSaber_Weapon::WEAPON_DESC		WeaponDesc{};
    //WeaponDesc.pBoneMatrix = m_pMat_RightHand;
    //WeaponDesc.pParentTransform = m_pTransformCom;
    //WeaponDesc.fSpeedPerSec = 0.f;
    //WeaponDesc.fRotationPerSec = 0.f;
    //WeaponDesc.iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();

    //m_pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(CSaber_Weapon::ObjID, &WeaponDesc));
    //if (nullptr == m_pWeapon)
    //    return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Mimosa::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    // Init Model and Matrices
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(CGame_Manager::Get_Instance()->Get_Current_Level(), TEXT("Model_Monster_Mimosa"), TEXT("Com_Model")));
    m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");
    m_pMat_RightHand = m_pModelCom->Get_BoneMatrixPtr("spine_00");

    CCharacterController::CCT_DESC cctDesc;
    cctDesc.pTransform = m_pTransformCom;
    cctDesc.height = 1.0f;
    cctDesc.radius = 1.f;
    cctDesc.stepOffset = 0.2f;
    cctDesc.material = GAMEINSTANCE->Get_Physics()->createMaterial(0.5f, 0.5f, 0.5f);
    cctDesc.userData = this;

    /* For. Com_CCT*/
    m_pCCT = static_pointer_cast<CCharacterController>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"), TEXT("Com_CCT"), &cctDesc));

    
    CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
    sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
    sphereDesc.fRadius = 1.f;
    m_pCollider_Hit =
        dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Hit"), &sphereDesc));
    m_pCollider_Hit->Set_Owner(shared_from_this());
    m_pCollider_Hit->Set_ColliderTag(TEXT("Monster_Hit"));
    
    sphereDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
    sphereDesc.fRadius = 3.f;
    m_pCollider_Attack =
        dynamic_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Attack"), &sphereDesc));
    m_pCollider_Attack->Set_Owner(shared_from_this());
    m_pCollider_Attack->Set_ColliderTag(TEXT("Monster_Attack"));

    m_pWingTexture = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Mimosa_WingTex"), TEXT("Com_Texture")));


    if (!m_pShaderCom || !m_pModelCom || !m_pWingTexture)
        assert(false);
    return S_OK;
}

void CMonster_Mimosa::Initialize_Weapon()
{
    CWeapon_Mimosa::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("prop_00");
    WeaponDesc.pParentTransform = m_pTransformCom;
    /* �߰��� ��Ʈ������Ʈ�� �θ� �������� ȸ���ϰų� �̵��� �� �ִ�. */
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;

    shared_ptr <CPartObject> pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CWeapon_Mimosa"), &WeaponDesc));
    if (nullptr == pWeapon)
        assert(false);

    m_pWeapon_Mic = (pWeapon);
    static_pointer_cast<CWeapon_Mimosa>(m_pWeapon_Mic)->Set_Render(false);
}

void CMonster_Mimosa::PrepareCallbacks()
{
    m_pModelCom->SetAnimCallback("em5100_atk_000", 0, [&]() { Wing_On(0.1f); });

    m_pModelCom->SetAnimCallback("em5100_atk_002", 0, [&]() { Wing_Off(0.1f); });

    m_pModelCom->SetAnimCallback("em5100_atk_120", 0, [&]() { Wing_On(0.1f); });
    m_pModelCom->SetAnimCallback("em5100_atk_123",0, [&]() {m_isAttackOn = true; });
    m_pModelCom->SetAnimCallback("em5100_atk_123",1, [&]() {m_isAttackOn = false; });
    m_pModelCom->SetAnimCallback("em5100_atk_123",2, [&]() { Wing_Off(0.1f); });

    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 0, [&]() { Create_Letter_Bomb(); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 1, [&]() { Create_Letter_Bomb(); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 2, [&]() { Create_Letter_Bomb(); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 3, [&]() { Create_Letter_Bomb(); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 4, [&]() { Create_Letter_Bomb(); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 5, [&]() { Create_Letter_Bomb(); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 6, [&]() { Create_Letter_Bomb(); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_100", 7, [&]() { Create_Letter_Bomb(); });

    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 0, [&]() { Lazer_Charge("prop_beam_L_01"); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 1, [&]() { Lazer_Charge("prop_beam_R_01"); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 2, [&]() { Lazer_Charge("prop_beam_L_00"); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 3, [&]() { Lazer_Charge("prop_beam_R_00"); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 4, [&]() { Lazer_Fire(0); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 5, [&]() { Lazer_Fire(1); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 6, [&]() { Lazer_Fire(2); });
    m_pModelCom->SetAnimCallback("em5100_atk-air_020", 7, [&]() { Lazer_Fire(3); });


    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_020", 0, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_020", 1, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_020", 2, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_020", 3, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_020", 4, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_020", 5, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_020", 6, [&]() { RP_Lazer_Fire(); });

    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 0, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 1, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 2, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 3, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 4, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 5, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 6, [&]() { RP_Lazer_Fire(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_040", 7, [&]() { RP_Lazer_Fire(); });

    m_pModelCom->SetAnimCallback("em5100_atk-air_261", 0, [&]() { Spawn_Mirror_Ball(); });

    m_pModelCom->SetAnimCallback("em5100_idle-air_051", 0, [&]() { Wing_Off(0.1f); });
    m_pModelCom->SetAnimCallback("em5100_idle-air_051", 1, [&]() { GravityOn(); });
    m_pModelCom->SetAnimCallback("em5100_idle-air_052", 0, [&]() { Wing_On(0.1f); });
    m_pModelCom->SetAnimCallback("em5100_idle-air_053", 0, [&]() { Wing_Off(0.1f); GravityOn();  });

    m_pModelCom->SetAnimCallback("em5100_dmg-down_011", 0, [&]() { Wing_On(0.1f); });

    m_pModelCom->SetAnimCallback("em5100_mvnt-step_000", 0, [&]() { Wing_On(0.1f); });
    m_pModelCom->SetAnimCallback("em5100_mvnt-step_000", 1, [&]() { Wing_Off(0.1f); });

    m_pModelCom->SetAnimCallback("em5100_event-mimosa_000", 0, [&]() { Wing_Off(0.1f); });


    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_000", 0, [&]() {Create_CutScene(); });
    m_pModelCom->SetAnimCallback("em5100_atk-rp-cp_025", 0, [&]() {Create_CutScene(); });


    CCine_Manager::Get_Instance()->AddStartCallback("CS0820", [&]() {    BEATMANAGER->Queue_Sequence(L"Mimosa_Start");  });
    CCine_Manager::Get_Instance()->AddStartCallback("CS0840", [&]() {    BEATMANAGER->Queue_Sequence(L"Mimosa_OutroTypeA");  });
    CCine_Manager::Get_Instance()->AddLastCallback("CS0820", [&]() {Move_To_CutScene_Pos_1();  });
    CCine_Manager::Get_Instance()->AddLastCallback("CS0825", [&]() {Move_To_CutScene_Pos_2();  });
    CCine_Manager::Get_Instance()->AddLastCallback("CS0830", [&]() {Move_To_CutScene_Pos_3();  });
    CCine_Manager::Get_Instance()->AddLastCallback("CS0835", [&]() {Move_To_CutScene_Pos_4();  });
    CCine_Manager::Get_Instance()->AddLastCallback("CS0840", [&]() { });

}




void CMonster_Mimosa::Animation_End()
{
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_000") && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em5100_atk_001", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_001") && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em5100_atk_002", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk_000") && m_pModelCom->Get_Finished() && m_iPhase == 2)
        m_pModelCom->Set_AnimIndex("em5100_mvnt-walk_021", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk_021") && m_pModelCom->Get_Finished() && m_iPhase == 2)
    {
        m_bPlayerLook = true;
        m_pModelCom->Set_AnimIndex("em5100_atk_000", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_000") && m_pModelCom->Get_Current_Track_Position() > 30.f && m_iPhase == 2)
    {
        for (auto& iter : m_vecBackDancer)
            static_pointer_cast<CBackDancer>(iter)->Attack_Start();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_000") && m_pModelCom->Get_Finished() && m_iPhase == 2)
    {
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_002") && m_pModelCom->Get_Finished())
    {
        m_bTurn = true;
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_010") && m_pModelCom->Get_Finished())
    {
        m_bTurn = true;
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_120") && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em5100_atk_121", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_121") && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em5100_atk_122", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_122") && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em5100_atk_123", false);
    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_052")
        || currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_011"))
        && m_pModelCom->Get_Finished())
    {
        m_bTurn = true;
        m_bFlyState = true;
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_000")
        && m_pModelCom->Get_Finished())
        Pattern_Wait();
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_051")
        && m_pModelCom->Get_Finished())
    {
        m_bFlyState = false;
        m_bTurn = true;
        GravityOn();
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_053")
        && m_pModelCom->Get_Finished() )
    {
        m_bFlyState = false;
        m_bTurn = true;
        GravityOn();
        Pattern_Wait();
    }
    else if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk-air_000") && m_pModelCom->Get_Finished() ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk-air_001") && m_pModelCom->Get_Finished() ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk-air_002") && m_pModelCom->Get_Finished() || 
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk-air_003") && m_pModelCom->Get_Finished())
        Pattern_Wait();
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_123") && m_pModelCom->Get_Finished())
    {
        m_bTurn = true;
        GravityOn();
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_020") && m_pModelCom->Get_Finished())
            Pattern_Wait();
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_260") && m_pModelCom->Get_Current_Track_Position() >= 45.f)
        m_pModelCom->Set_AnimIndex("em5100_atk-air_261", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_261") && m_pModelCom->Get_Finished())
        Pattern_Wait();
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_100") && m_pModelCom->Get_Finished())
    {
        static_pointer_cast<CWeapon_Mimosa>(m_pWeapon_Mic)->Set_Render(false);
        m_pModelCom->Set_AnimIndex("em5100_idle-air_051", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-air_300") && m_pModelCom->Get_Finished())
    {
        GravityOn();
        m_pModelCom->Set_AnimIndex("em5100_dmg-air_050", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_010") && m_pModelCom->Get_Finished())
    {
        GravityOn();
        m_bTurn = true;
        Pattern_Wait();
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_200") && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em5100_dmg-down_000", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_300") && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("em5100_dmg-down_000", false);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_event-mimosa_000") && m_pModelCom->Get_Finished())
    {
        auto pMainCam = dynamic_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main")));
        pMainCam->End_Following_Bone();
        m_bTurn = true;
       // CCT_ON();
        GravityOn();
        Pattern_Wait();
    }
    else if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-step_000") || 
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk_000"))
        && m_pModelCom->Get_Finished())
        Pattern_Wait();
}

void CMonster_Mimosa::Animation(_float fTimeDelta)
{
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * m_fTimeMultiflier;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f * m_fTimeMultiflier;
    _float Three_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.3333f) * m_fTimeMultiflier;
    _float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f * m_fTimeMultiflier;
    _float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f) * m_fTimeMultiflier;
    _float Six_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1666f) * m_fTimeMultiflier;
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f) * m_fTimeMultiflier;
    _float Ten_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.1f) * m_fTimeMultiflier;
    _float Twelve_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.083f) * m_fTimeMultiflier;

    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_000"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_001"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_002"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_010"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Ten_Beat, 0.1f);

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_120"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_121"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_122"))
    {
        m_pTransformCom->LookAt_Horizontal_With_Speed(XMLoadFloat4(&m_vPlayerPos), fTimeDelta * 5.f);
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * One_Beat, 0.1f);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk_123"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-walk_000"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_000"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_051"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_052"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_idle-air_053"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_020"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Twelve_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_260"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_261"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Ten_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-air_100"))
    {
        static_pointer_cast<CWeapon_Mimosa>(m_pWeapon_Mic)->Set_Render(true);
        static_pointer_cast<CWeapon_Mimosa>(m_pWeapon_Mic)->Change_Animation("em5060_atk-air_100");
        static_pointer_cast<CWeapon_Mimosa>(m_pWeapon_Mic)->Set_AnimSpeed(Ten_Beat);
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Ten_Beat, 0.1f);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-air_300"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-air_050"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_000"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_010"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_011"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_200"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_300"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_mvnt-step_000"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_event-mimosa_000"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.071f) * m_fTimeMultiflier, 0.1f);
    else
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * m_fTimeMultiflier, 0.1f);

    Off_Attach_Wing();
    Wing_Controller(fTimeDelta);
    m_pModelCom->Update_Bones(fmatResult);

    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
}

void CMonster_Mimosa::Dance_Event_Animation(_float fTimeDelta)
{
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * m_fTimeMultiflier;
    _float Half_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 2.f * m_fTimeMultiflier;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f* m_fTimeMultiflier;

    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    //if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_event-dance_100"))
    //    m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f);
    //else

    if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_event-dance_025"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Half_Beat, 0.1f);
    else
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * One_Beat * m_fDance_PlaySpeed, 0.1f);

    Off_Attach_Wing();
    Wing_Controller(fTimeDelta);
    m_pModelCom->Update_Bones(fmatResult);

    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
    if (m_CameraBone.Update(fTimeDelta) && m_bIsOwnCamBone)
    {
        m_bIsOwnCamBone = false;
        m_CameraBone.Initialize(m_list_Mimosa_Dance_Delay.front(), 2.6f,PLAYERMANAGER->Get_Player()->Get_Transform());
    }

}

void CMonster_Mimosa::Dance_Event_Animation_End(_float fTimeDelta)
{
    //if (m_listDanceEvent.empty())
    //{
    //    m_bDance_Event = false;
    //    return;
    //}

    if (m_bDance_Rest)
        return;
    list<pair<string,_float>>& dance_list = m_listDanceEvent.front();

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex(dance_list.front().first) && m_pModelCom->Get_Finished())
    {
        dance_list.pop_front();
        if (dance_list.empty())
        {
            m_bDance_Rest = true;
            m_listDanceEvent.pop_front();
        }
        else
        {
            m_fDance_PlaySpeed = dance_list.front().second;
            m_pModelCom->Set_AnimIndex(dance_list.front().first, false);
            for (size_t i = 0; i < m_vecBackDancer.size(); i++)
            {
                static_pointer_cast<CBackDancer> (m_vecBackDancer[i])->Dance(dance_list.front().first, dance_list.front().second);
            }
        }
    }
}

void CMonster_Mimosa::Rhythm_Parry_Animation(_float fTimeDelta)
{
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float One_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * m_fTimeMultiflier;
    _float Half_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 2.f * m_fTimeMultiflier;
    _float Two_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.5f * m_fTimeMultiflier;
    _float Eight_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f) * m_fTimeMultiflier;
    _float Ten_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.125f) * m_fTimeMultiflier;
    _float Five_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * (0.2f) * m_fTimeMultiflier;
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_000"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.0625f * m_fTimeMultiflier, 0.1f);
    else if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_010"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Ten_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_011"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Ten_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_020"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Eight_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_025"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.0625f * m_fTimeMultiflier, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_030"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.0625f * m_fTimeMultiflier, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_040"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Eight_Beat, 0.1f);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_050"))
        m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f);

    Off_Attach_Wing();
    Wing_Controller(fTimeDelta);
    m_pModelCom->Update_Bones(fmatResult);

    //m_pModelCom->Play_Non_InterPole_Animation(fTimeDelta * 60.f, fmatResult);
    _matrix matResult = XMLoadFloat4x4(&fmatResult);

    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
    //m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() +  XMVector3Normalize(matAnimRot.r[2]));

    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

    m_pCCT->Add_Displacement((vCurPos - m_pTransformCom->Get_Position()) / fTimeDelta);
}

void CMonster_Mimosa::Rhythm_Parry_Animation_End(_float fTimeDelta)
{
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

    if(currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_000") && m_pModelCom->Get_Finished())
    {
        GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_RP_Ready");
        BEATMANAGER->Reset();
        GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_RP_Loop", true);
        m_iRP_Success_Count = 0;
        Set_Ui(m_iParry_Success_Count);
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_010", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_010") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 0)
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_020", false);
    }

    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_020") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 1)
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_025", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_020") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 0)
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_030", false);
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_030") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 0)
    {
        m_iRP_Success_Count = 0;
        Set_Ui(m_iParry_Success_Count);
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_010", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_030") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 1)
    {
        m_iRP_Success_Count = 0;
        Set_Ui(m_iParry_Success_Count);
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_011", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_025") && m_pModelCom->Get_Finished()  &&m_iParry_Success_Count == 1)
    {
        m_iRP_Success_Count = 0;
        Set_Ui(m_iParry_Success_Count);
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_011", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_011") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 1)
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_040", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_040") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 2)
    {
        CUi_Mgr::Get_Instance()->Ui_AllActive(false);
        GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_RP_Loop");
        GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_Finish", false);
        PLAYERMANAGER->Callback_Event("Mimosa_RP_End");
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_050", false);
    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_050") && m_pModelCom->Get_Finished() && !m_bFinish)
    {
        m_bFinish = true;
        m_fFinish_Timer = BEATMANAGER->Get_Beat() * 6.f;
        CUI_Event_Cut::Desc Desc;
        Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::MimosaFinal;
        CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);

    }
    else if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_040") && m_pModelCom->Get_Finished() && m_iParry_Success_Count == 1)
        m_pModelCom->Set_AnimIndex("em5100_atk-rp-cp_030", false);
}

void CMonster_Mimosa::Initialize_Dance_Event()
{
    list<pair<string, _float>> danceMotion_list = {};
    list<pair<_float, _int>> Rhythm_list = {};
    //    000 idle to �� ǻ��
    //    001 �� ǻ�� to idle
    //    002 �� ǻ�� to �� ǻ��
    //    003 �� ǻ�� to �ڼ�
    //    Q
    // 
    //    010 idle to �� ǻ��
    //    011 �� ǻ�� to idle
    //    012 �� ǻ�� to �� ǻ��
    //    013 �� ǻ�� to �ڼ�
    //    E
    //    020 idle to �ڼ�
    //    021 �ڼ� to idle
    //    022 �ڼ� to �� ǻ��
    //    023 �ڼ� to �� ǻ��
    //    024 �ڼ� to ȸ�� ����
    //    025 �ڼ� to �ڼ�(�ݹ���)
    //    026 �ڼ� to �ڼ�(�ѹ���)
    //    SPACE
    // 
    //    030 idle to ȸ�� ����
    //    031 ȸ�� ���� to A����
    //    032 A���� to idle
    //    033 A���� to ȸ�� ����
    //    034 A���� to �ڼ�
    //    LSHIFT
    // first dance 010 011 000 001 010 012 001 ;
    pair<string, _float> pairData = {};
    pair<_float, _int>  RhythmData = {};
    pairData.second = 1.f;

    pairData.first = "em5100_event-dance_010";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_011";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_000";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_001";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_010";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_012";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_001";
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(9.f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();


    RhythmData.first = 0.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    m_list_Mimosa_Dance_Delay.push_back(7.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);
    Rhythm_list.clear();

    // 012 002 012 001 100 000 
    pairData.first = "em5100_event-dance_012";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_002";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_012";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_001";
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_100";
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_000";
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_001";
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(8.f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();


    RhythmData.first = 0.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 3.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    m_list_Mimosa_Dance_Delay.push_back(7.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);
    Rhythm_list.clear();
    //026 026 021 020 025 025 021 020 021 
    pairData.first = "em5100_event-dance_026"; // 1
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_021"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_020"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_026"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025";// 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_021"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_100"; // 0.5f
    pairData.second = 0.5f;
    pairData.first = "em5100_event-dance_020"; // 1.f
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_021"; // 1
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(8.f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();


    RhythmData.first = 0.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    m_list_Mimosa_Dance_Delay.push_back(7.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);
    Rhythm_list.clear();

    // 030 031 033 031 033 031 034 026
    pairData.first = "em5100_event-dance_030"; // 1 
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_031";  // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_033"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_031"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_033"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_031"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_034"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_021"; // 1 
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(6.f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();

    RhythmData.first = 2.f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    m_list_Mimosa_Dance_Delay.push_back(7.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);
    Rhythm_list.clear();

    // 013 025 022 025 026 025 025 026 021

    pairData.first = "em5100_event-dance_013";  // 1 
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_022"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_003"; // 1 
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_026"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025"; // 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025"; // 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_026"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_021"; // 1 
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(8.f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();


    RhythmData.first = 0.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
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

    m_list_Mimosa_Dance_Delay.push_back(7.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);
    Rhythm_list.clear();

    // 012 003 025 025 024 031 034 021
    pairData.first = "em5100_event-dance_012"; // 1 
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_003"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025"; // 0.5
    danceMotion_list.push_back(pairData);

    pairData.first = "em5100_event-dance_025";  // 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_024"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_031"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_034"; // 1 
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_021"; // 1 
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(8.f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();


    RhythmData.first = 0.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    m_list_Mimosa_Dance_Delay.push_back(7.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);
    Rhythm_list.clear();


    //026 025 025 026 025 023 012 003 021
    pairData.first = "em5100_event-dance_026"; // 1
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025"; // 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_026"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025";// 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_023";// 1
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_012";// 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_003";// 0.5
    pairData.second = 2.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_021";// 1
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(7.5f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();

    RhythmData.first = 0.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 2;
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

    RhythmData.first = 0.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    m_list_Mimosa_Dance_Delay.push_back(7.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);
    Rhythm_list.clear();


    // 024 031 032 020 025 025 025 023 012 001
    pairData.first = "em5100_event-dance_020"; // 1
    pairData.second = 1.f;
    pairData.first = "em5100_event-dance_024"; // 1
    pairData.second = 1.f;
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_031"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_032"; // 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_020";// 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025"; // 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_025";// 0.5
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_023";// 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_012";// 1
    danceMotion_list.push_back(pairData);
    pairData.first = "em5100_event-dance_001";// 1
    danceMotion_list.push_back(pairData);

    m_list_Dance_Beat.push_back(8.f);
    m_listDanceEvent.push_back(danceMotion_list);
    danceMotion_list.clear();

    RhythmData.first = 0.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 3;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 2.f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 0.5f;
    RhythmData.second = 2;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 1;
    Rhythm_list.push_back(RhythmData);

    RhythmData.first = 1.f;
    RhythmData.second = 0;
    Rhythm_list.push_back(RhythmData);

    m_list_Mimosa_Dance_Delay.push_back(9.f);
    m_listDanceEvent_Beats.push_back(Rhythm_list);

    PLAYERMANAGER->Set_Dance_Rhythm(m_listDanceEvent_Beats);
    PLAYERMANAGER->Set_Dance_Beats(m_list_Dance_Beat);
    Rhythm_list.clear();

}

void CMonster_Mimosa::Start_Dance()
{
    if (m_listDanceEvent.empty())
    {
        CCine_Manager::Get_Instance()->StartCine("CS0835");
        for (auto& iter: m_vecBackDancer)
              iter->Dead();
        m_vecBackDancer.clear();

    }
    else
    {
        list<pair<string, _float>>& dance_list = m_listDanceEvent.front();

        m_bDance_Rest = false;
        m_pModelCom->Set_AnimIndex(dance_list.front().first, false);
        m_fDance_PlaySpeed = dance_list.front().second;

        for (size_t i = 0; i < m_vecBackDancer.size(); i++)
        {
            static_pointer_cast<CBackDancer> (m_vecBackDancer[i])->Dance(dance_list.front().first, dance_list.front().second);
        }
        m_CameraBone.Initialize((m_list_Dance_Beat.front() - 2.f) * BEATMANAGER->Get_Beat(), 2.6f, m_pTransformCom);
        m_bIsOwnCamBone = true;
    }
}

void CMonster_Mimosa::LookPlayerPos(_float fTimeDelta)
{
    if(m_bPlayerLook && !m_bTurn)
        m_pTransformCom->LookAt_Horizontal(XMLoadFloat4(&m_vPlayerPos));

    else if(m_bTurn)
    {
        m_fTurnTimer -= fTimeDelta * 5.f;
        m_pTransformCom->LookAt_Horizontal_With_Speed(XMLoadFloat4(&m_vPlayerPos), fTimeDelta * 5.f);
        if (m_fTurnTimer < 0.f)
        {
            m_fTurnTimer = 1.f;
            m_bTurn = false;
            m_bPlayerLook = true;
        }
    }
}

_float CMonster_Mimosa::Calculate_Distance_To_Player()
{
   _float resDistance =  XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position()));
    return resDistance;
}

void CMonster_Mimosa::Collision_Down(_float fTimeDelta)
{
    PxControllerState pCS;
    m_pCCT->Get_Controller()->getState(pCS);
    if (pCS.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
        if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-air_300") || 
            currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-air_050"))
        {
            m_bFlyState = false;
            Wing_Off(0.1f);
            m_pModelCom->Set_AnimIndexNonCancle("em5100_dmg-down_200", false);
        }    
    }
}

void CMonster_Mimosa::DownState(_float fTimeDelta)
{
    _float fBeat = BEATMANAGER->Get_Beat();
    _float fBeatRate = BEATMANAGER->Get_BeatRate();
    _uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if (currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_000") ||
         currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_dmg-down_300"))
        m_fDownTimer += fTimeDelta;
    
    if (m_fDownTimer > fBeat * 7.f && fBeatRate > 0.98f)
    {
        //m_pCCT->Resize(1.f, 1.0f);

        m_fBarrier = m_fMaxBarrier;
        m_fDownTimer = 0.f;

        if (!m_bTalkRestartBarrier)
        {
            m_bTalkRestartBarrier = true;
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
                PRequiredTalkSystem->LoadTalkFile(L"VsMimosa_Talk_File_3.csv");

                CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
            }
        }

        if(m_bWakeUp_Fly)
        {
            m_pModelCom->Set_AnimIndexNonCancle("em5100_dmg-down_011", false);
            GravityOff();
            m_bFlyState = true;
        }
        else
            m_pModelCom->Set_AnimIndexNonCancle("em5100_dmg-down_010", false);
    }
}

void CMonster_Mimosa::Pattern_Change(_float fTimeDelta)
{
    _float fBeat = BEATMANAGER->Get_Beat();
    _float fBeatRate = BEATMANAGER->Get_BeatRate();

    if (m_bPattern_Timer_On)
        m_fPatternTimer += fTimeDelta;
    if (m_fPatternTimer > fBeat * 3.f && fBeatRate > 0.96f)
    {
        m_fPatternTimer = 0.f;
        m_bPattern_Timer_On = false;

  
        if (m_iPhase == 1 || m_iPhase == 3)
        {
            if (m_strNext_Pattern != "em5100_atk-air_020" &&
                m_strNext_Pattern != "em5100_atk-air_100" &&
                m_strNext_Pattern != "em5100_atk-air_260")
            {
                m_strCurrent_Pattern = m_strNext_Pattern;
                m_pModelCom->Set_AnimIndex(m_strCurrent_Pattern, false);
            }
            else if ((m_strNext_Pattern == "em5100_atk-air_020" ||
                m_strNext_Pattern == "em5100_atk-air_100" ||
                m_strNext_Pattern == "em5100_atk-air_260") &&
                !m_bFlyState)
            {
                GravityOff();
                m_pModelCom->Set_AnimIndex("em5100_idle-air_052", false);
            }
            else if ((m_strNext_Pattern == "em5100_atk-air_020" ||
                m_strNext_Pattern == "em5100_atk-air_100" ||
                m_strNext_Pattern == "em5100_atk-air_260") &&
                m_bFlyState)
            {
                m_strCurrent_Pattern = m_strNext_Pattern;
                m_pModelCom->Set_AnimIndex(m_strCurrent_Pattern, false);
            }

            if (m_strCurrent_Pattern == "em5100_atk_000")
                m_bPlayerLook = false;
            else if (m_strCurrent_Pattern == "em5100_atk_120")
            {
                m_bPlayerLook = false;
                GravityOff();
            }
        }

        if (m_iPhase == 1)
        {
            if (m_strCurrent_Pattern == "em5100_atk_000")
            {   
                m_bWakeUp_Fly = false;
                m_strNext_Pattern = "em5100_atk_010";
            }
            else if (m_strCurrent_Pattern == "em5100_atk_010")
            {
                m_bWakeUp_Fly = false;
                m_strNext_Pattern = "em5100_atk_120";
            }
            else if (m_strCurrent_Pattern == "em5100_atk_120")
            {
                m_bWakeUp_Fly = true;
                m_strNext_Pattern = "em5100_atk-air_020";
            }
            else if (m_strCurrent_Pattern == "em5100_atk-air_020")
            {
                m_bWakeUp_Fly = true;
                m_strNext_Pattern = "em5100_atk-air_100";
            }
            else if (m_strCurrent_Pattern == "em5100_atk-air_100")
            {
                m_bWakeUp_Fly = false;
                m_strNext_Pattern = "em5100_atk_000";
            }
        }
        else if (m_iPhase == 3)
        {
 
            if (m_strCurrent_Pattern == "em5100_atk-air_260")
            {
                m_bWakeUp_Fly = true;
                m_strNext_Pattern = "em5100_atk-air_020";
            }
            else if (m_strCurrent_Pattern == "em5100_atk-air_020")
            {
                m_bWakeUp_Fly = true;
                m_strNext_Pattern = "em5100_atk-air_100";
            }
            else if (m_strCurrent_Pattern == "em5100_atk-air_100")
            {
                m_bWakeUp_Fly = false;
                m_strNext_Pattern = "em5100_atk_000";
            }
            if (m_strCurrent_Pattern == "em5100_atk_000")
            {
                m_bWakeUp_Fly = false;
                m_strNext_Pattern = "em5100_atk_010";
            }
            else if (m_strCurrent_Pattern == "em5100_atk_010")
            {
                m_bWakeUp_Fly = false;
                m_strNext_Pattern = "em5100_atk_120";
            }
            else if (m_strCurrent_Pattern == "em5100_atk_120")
            {
                m_bWakeUp_Fly = true;
                m_strNext_Pattern = "em5100_atk-air_260";
            }

        }
    }
}

void CMonster_Mimosa::Play_Pattern(_float fTimeDelta)
{
}

void CMonster_Mimosa::Pattern_Wait()
{
    _float distance = Calculate_Distance_To_Player();
    m_bPattern_Timer_On = true;
    if (m_bFlyState)
    {

        if (distance < 6.f)
            m_pModelCom->Set_AnimIndex("em5100_mvnt-walk-air_001", false);
        else if (distance < 11.f)
        {
            _int iRandNum = GAMEINSTANCE->Random_Int(1, 2);
            if (iRandNum == 1)
                m_pModelCom->Set_AnimIndex("em5100_mvnt-walk-air_002", false);
            else
                m_pModelCom->Set_AnimIndex("em5100_mvnt-walk-air_003", false);
        }
        else
            m_pModelCom->Set_AnimIndex("em5100_mvnt-walk-air_000", false);
    }
    else
    {


        if (distance > 10.f)
            m_pModelCom->Set_AnimIndex("em5100_mvnt-step_000", false);
        else
            m_pModelCom->Set_AnimIndex("em5100_mvnt-walk_000", false);
    }
}

void CMonster_Mimosa::Start_RP()
{
    BEATMANAGER->Stop_Sequence();
    GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain");
    BEATMANAGER->Reset();
    GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_RP_Ready", false);
    Wing_On(0.1f);
    GravityOff();
    m_bFlyState = true;
    m_bRhythm_Parry = true;
    m_pModelCom->Set_AnimIndex_NoInterpole("em5100_atk-rp-cp_000", false);
    PLAYERMANAGER->Callback_Event("Mimosa_RP");
    m_fTurnTimer = 1.f;
    m_bTurn = false;
    m_bPlayerLook = false;

    m_pModelCom->Set_AnimFrame(0.01f);

    auto pMainCam = static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main")));
    pMainCam->Start_Following_Bone(m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pMat_Camera));

    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());

    _matrix matBone = XMLoadFloat4x4(m_pChai_Attach);
    matBone.r[0] = XMVector3Normalize(matBone.r[0]);
    matBone.r[1] = XMVector3Normalize(matBone.r[1]);
    matBone.r[2] = XMVector3Normalize(matBone.r[2]);
    _matrix vMat = matBone * m_pTransformCom->Get_WorldMatrix_XMMat();

    _vector vVec = vMat.r[3];
    vVec = XMVectorSetY(vVec, XMVectorGetY(vVec));
    vMat.r[3] = vVec;
    pPlayer->Set_Player_Position(vVec);

    pPlayer->Get_Transform()->LookAt_Horizontal(pPlayer->Get_Transform()->Get_Position() - m_pTransformCom->Get_NormLook());
}

void CMonster_Mimosa::Phase2_SpotLight_Pattern(_float fTimeDelta)
{
    GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Mimosa_Spotlight", false);
    PLAYERMANAGER->Callback_Event("Mimosa_SpotLight");
    m_pModelCom->Set_AnimIndex("em5100_event-dance_100", true);
    m_pModelCom->Set_CurrentTrackPosition(1.f);
    _float4x4 xMat = {};
    m_pModelCom->Ready_Animation(fTimeDelta, fTimeDelta, 0.1f);
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
    //m_pModelCom->Update_Bones(fmatResult);
    m_bPlayerLook = false;
    _float4 fPos = { GAMEINSTANCE->Random_Float(-23.f,-1.f),-2.f,GAMEINSTANCE->Random_Float(-18.f,17.f) ,1 };
    for (size_t i = 0; i < 4; i++)
    {
        CBackDancer::BackDancer_Info bInfo = {};
        bInfo.bSpotLight = true;

        Check_SpawnPos(fPos);
        bInfo.fInitPos = { fPos };

        m_vecSpawnPos.push_back(bInfo.fInitPos);
        bInfo.iMotion = GAMEINSTANCE->Random_Int(0, 2);

        m_vecBackDancer.push_back(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), TEXT("Layer_Monster"), CBackDancer::ObjID, &bInfo));
    }
    fPos = { GAMEINSTANCE->Random_Float(-23.f,-1.f),-2.f,GAMEINSTANCE->Random_Float(-18.f,17.f) ,1 };
    Check_SpawnPos(fPos);

    CTransform::TRANSFORM_DESC			TransformDesc{};
    TransformDesc.fSpeedPerSec = 1.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    TransformDesc.vPosition = fPos;
    m_pSpotLight = GAMEINSTANCE->Add_Clone_Return(LEVEL_MIMOSA, TEXT("Layer_Mimosa_SpotLight"), CMimosa_SpotLight::ObjID, &TransformDesc);


    _vector vDIr = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - XMLoadFloat4(&fPos));
    _vector vPos = XMLoadFloat4(&fPos) - vDIr * 2.6f;

    m_pTransformCom->Set_Position(vPos);
    m_pTransformCom->LookAt_Horizontal(XMLoadFloat4(&fPos));
    m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());

    _float fBeat = BEATMANAGER->Get_Beat();
    m_fSpotlight_Timer = 8.f * fBeat;
}

void CMonster_Mimosa::Check_SpawnPos(_float4& fPos)
{
    for (size_t i = 0; i < m_vecSpawnPos.size(); i++)
    {
        _float distance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vecSpawnPos[i]) - XMLoadFloat4(&fPos)));
        if (distance < 4.f)
        {
            fPos = { GAMEINSTANCE->Random_Float(-23.f,-1.f),-2.f,GAMEINSTANCE->Random_Float(-18.f,17.f) ,1 };
            Check_SpawnPos(fPos);
        }
    }
}

void CMonster_Mimosa::Spawn_Mirror_Ball()
{
    CMirror_Ball::MIRROR_BALL tLaser = {};
    XMStoreFloat4(&tLaser.vOriginPos, m_pTransformCom->Get_Position());
    tLaser.vOriginPos.y += 3.f;

    GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), CMirror_Ball::ObjID, &tLaser);
}

void CMonster_Mimosa::Create_Letter_Bomb()
{
    auto pGI = m_pGameInstance.lock();

    _float4x4 EffectMat;
    XMStoreFloat4x4(&EffectMat, m_pTransformCom->Get_WorldMatrix_XMMat());
    EffectMat.m[3][1] += 1.2f;
    _float fBeat = BEATMANAGER->Get_Beat();
    _float4 fPos = {};
    CMimosa_Letter_Bomb::MiMOSA_LETTER ls = {};
    XMStoreFloat4(&fPos, m_pTransformCom->Get_NormLook());
    ls.fMimosaLookDir = fPos;
    ls.iLetterModel = m_iLetterIdx;

    switch (m_iLetterIdx)
    {
    case 0:
    {
        ls.fShootDelay = 10.f * fBeat;
        break;
    }
    case 1:
    {
        ls.fShootDelay = 11.f * fBeat;
        break;
    }
    case 2:
    {
        ls.fShootDelay = 12.f * fBeat;
        break;
    }
    case 3:
    {
        ls.fShootDelay = 12.f * fBeat;
        break;
    }
    case 4:
    {
        ls.fShootDelay = 14.f * fBeat;
        break;
    }
    case 5:
    {
        ls.fShootDelay = 15.f * fBeat;
        break;
    }
    case 6:
    {
        ls.fShootDelay = 16.f * fBeat;
        break;
    }
    case 7:
    {
        ls.fShootDelay = 15.5f * fBeat;
        break;
    }
    default:
        break;
    }
    m_iLetterIdx++;
    _vector bonePos = m_pTransformCom->Get_WorldMatrix_XMMat().r[3];
    bonePos += m_pTransformCom->Get_NormLook() * 0.5f;
    XMStoreFloat4(&fPos, bonePos);
    fPos.y += 1.2f;
    ls.fCenterPos = fPos;

    pGI->Push_Object_From_Pool(CMimosa_Letter_Bomb::ObjID, pGI->Get_Current_LevelID(), L"Effect", &ls, &EffectMat);

    if (m_iLetterIdx == 8)
        m_iLetterIdx = 0;

     

}

void CMonster_Mimosa::Lazer_Charge(string strBoneName)
{
    _float4x4 matResult{};
    matResult = *m_pModelCom->Get_BoneMatrixPtr(strBoneName.c_str());
    _matrix mMat = XMLoadFloat4x4(&matResult);
    mMat.r[0] =  XMVector4Normalize(mMat.r[0]);
    mMat.r[1] =  XMVector4Normalize(mMat.r[1]);
    mMat.r[2] =  XMVector4Normalize(mMat.r[2]);
    XMStoreFloat4x4(&matResult, mMat);
    //PlayEffect("Mimosa_Laser_Ready", matResult, 1.f, true);
    _float4x4 matResults{};
    XMStoreFloat4x4(&matResults, XMLoadFloat4x4(&matResult) * m_pTransformCom->Get_WorldMatrix_XMMat());
    m_listLazer.push_back(CEffect_Manager::Get_Instance()->Call_Union_Effect_Return("Mimosa_Laser_Ready", &matResults, 1.f, true));

}

void CMonster_Mimosa::Lazer_Fire(_int iIndex)
{
    m_listLazer[iIndex]->UnionEffectEnd();
    m_listLazer[iIndex] = nullptr;
    _float4x4 matResult{};

    switch (iIndex)
    {
    case 0:
    {
        matResult = *m_pModelCom->Get_BoneMatrixPtr("prop_beam_L_01");
        break;
    }
    case 1:
    {
        matResult = *m_pModelCom->Get_BoneMatrixPtr("prop_beam_R_01");
        break;
    }
    case 2:
    {
        matResult = *m_pModelCom->Get_BoneMatrixPtr("prop_beam_L_00");
        break;
    }
    case 3:
    {
        matResult = *m_pModelCom->Get_BoneMatrixPtr("prop_beam_R_00");
        break;
    }
    }

    _matrix mMat = XMLoadFloat4x4(&matResult);
    mMat.r[0] =  XMVector4Normalize(mMat.r[0]);
    mMat.r[1] =  XMVector4Normalize(mMat.r[1]);
    mMat.r[2] =  XMVector4Normalize(mMat.r[2]);
    XMStoreFloat4x4(&matResult, mMat);
    PlayEffect("Mimosa_Laser_Shoot", matResult, 1.f, false);
    PlayEffect("Mimosa_Laser_Fire_1", matResult, 1.f, true);

    
    shared_ptr<CEffect_Union> pLaser =  PlayEffect_Return("Mimosa_Laser_Fire_2", matResult, 1.f, false);
    pLaser->Get_Transform()->LookAt(XMLoadFloat4(& m_vPlayerPos));
    if (iIndex == 3)
        m_listLazer.clear();
}

void CMonster_Mimosa::RP_Lazer_Fire()
{
    _float4x4 matResult{};
    matResult = *m_pModelCom->Get_BoneMatrixPtr("spine_01");

    _matrix mMat = XMLoadFloat4x4(&matResult);
    mMat.r[0] = XMVector4Normalize(mMat.r[0]);
    mMat.r[1] = XMVector4Normalize(mMat.r[1]);
    mMat.r[2] = XMVector4Normalize(mMat.r[2]);

    _vector lazerDir = XMVector3Normalize(PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position() - m_pTransformCom->Get_Position());
    mMat.r[3] += lazerDir * 0.5f;
    XMStoreFloat4x4(&matResult, mMat);
    PlayEffect("Mimosa_Laser_Shoot", matResult, 1.f, false);
    PlayEffect("Mimosa_Laser_Fire_1", matResult, 1.f, true);


    shared_ptr<CEffect_Union> pLaser = PlayEffect_Return("Mimosa_Laser_Fire_2", matResult, 1.f, false);
    pLaser->Get_Transform()->LookAt(XMLoadFloat4(&m_vPlayerPos));
}

_float4x4 CMonster_Mimosa::Lazer_Matrix(string strBoneName)
{
    _float4x4 matResult{};
    matResult = *m_pModelCom->Get_BoneMatrixPtr(strBoneName.c_str());
    _matrix mMat = XMLoadFloat4x4(&matResult);
    mMat.r[0] = XMVector4Normalize(mMat.r[0]);
    mMat.r[1] = XMVector4Normalize(mMat.r[1]);
    mMat.r[2] = XMVector4Normalize(mMat.r[2]);
    XMStoreFloat4x4(&matResult, mMat);
    //PlayEffect("Mimosa_Laser_Ready", matResult, 1.f, true);
    _float4x4 matResults{};
    XMStoreFloat4x4(&matResults, XMLoadFloat4x4(&matResult) * m_pTransformCom->Get_WorldMatrix_XMMat());
    return matResults;
}

void CMonster_Mimosa::Update_Lazer(_float fTimeDelta)
{
    if(!m_listLazer.empty())
    {
        for (size_t i = 0; i < m_listLazer.size(); i++)
        {
            if (m_listLazer[i] == nullptr)
                continue;
            switch (i)
            {
            case 0:
            {
                m_listLazer[i]->Get_Transform()->Set_WorldMatrix(Lazer_Matrix("prop_beam_L_01"));
                break;
            }
            case 1:
            {
                m_listLazer[i]->Get_Transform()->Set_WorldMatrix(Lazer_Matrix("prop_beam_R_01"));
                break;
            }
            case 2:
            {
                m_listLazer[i]->Get_Transform()->Set_WorldMatrix(Lazer_Matrix("prop_beam_L_00"));
                break;
            }
            case 3:
            {
                m_listLazer[i]->Get_Transform()->Set_WorldMatrix(Lazer_Matrix("prop_beam_R_00"));
                break;
            }
            }
        }
    }
}

void CMonster_Mimosa::Create_AttackUi(_uint iParryCount, vector<_float>& fBeatGap, vector<_bool>& bParry, _float2 fDistance)
{
    CUi_EnemySpecialAttack::Desc Desc;
    Desc.iParryCount = iParryCount;
    Desc.bParry = bParry;
    Desc.fBeatGap = fBeatGap;
    Desc.fDistance = fDistance;
    Desc.TargetTransform = m_pTransformCom;
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_EnemySpecialAttack::ObjID,
        m_iLevelIndex, L"CUi", CUi_EnemySpecialAttack::ObjID, &Desc);
}

void CMonster_Mimosa::Set_Ui(_int iSuccessCount)
{
    m_AttackBeatVec.clear();
    m_AttackParryBeatVec.clear();
    m_iAttackReadyStartBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

    if(iSuccessCount == 0)
    {
        m_AttackBeatVec = { 0, 1.5f, 0.5f, 1.5f ,0.5f, 1.5f, 0.5f };
        m_AttackParryBeatVec = { true, true, true, true, true, true,true };
    }
    else if (iSuccessCount == 1)
    {
        m_AttackBeatVec = { 0, 0.5f, 1.5f, 0.5f ,1.5f, 1.f, 0.5f, 0.5f };
        m_AttackParryBeatVec = { true, true, true, true, true, true,true,true };
    }
    Create_AttackUi((_uint)m_AttackBeatVec.size(), m_AttackBeatVec, m_AttackParryBeatVec);

    for (size_t i = 1; i < m_AttackBeatVec.size(); ++i)
    {
        m_AttackBeatVec[i] += m_AttackBeatVec[i - 1];
    }
    shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();
    if (iSuccessCount == 0)
      pChai->Start_AttackSequence(8.f , m_iAttackReadyStartBeat, m_AttackBeatVec, m_AttackParryBeatVec);
    else if (iSuccessCount == 1)
      pChai->Start_AttackSequence(8.f, m_iAttackReadyStartBeat, m_AttackBeatVec, m_AttackParryBeatVec);
}

void CMonster_Mimosa::Create_CutScene()
{
    CUi_Mgr::Get_Instance()->Add_Clone(CUI_MimosaCutin::ObjID, LEVEL_MIMOSA, L"CUi", CUI_MimosaCutin::ObjID);
}

void CMonster_Mimosa::Move_To_CutScene_Pos_1()
{
    CCombat_Manager::Get_Instance()->Set_Combat_True();
    m_bFlyState = true;
    GravityOff();
    _float4x4 mMat = {};
    CCine_Manager::Get_Instance()->GetMatTrans("em5100", mMat);

    Wing_On(0.01f);
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&mMat));
    CUi_Mgr::Get_Instance()->Ui_AllActive(true);
}

void CMonster_Mimosa::Move_To_CutScene_Pos_2()
{
    m_bFlyState = false;
    GravityOn();
    _float4x4 mMat = {};
    CCine_Manager::Get_Instance()->GetMatTrans("em5100", mMat);

    Wing_Off(0.01f);
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&mMat));
    CUi_Mgr::Get_Instance()->Ui_AllActive(true);

    PLAYERMANAGER->Callback_Event("Mimosa_SpotLight");
    Phase2_SpotLight_Pattern(1.f);
    BEATMANAGER->Queue_Sequence(L"Mimosa_Verse1");
}

void CMonster_Mimosa::Move_To_CutScene_Pos_3()
{
    m_bFlyState = false;
    GravityOn();
    //_float4x4 mMat = {};
    //CCine_Manager::Get_Instance()->GetMatTrans("em5100", mMat);

    Wing_Off(0.01f);
   // m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&mMat));
    m_pTransformCom->Set_Position({ -15.f,-2.f,2.f});
    m_pCCT->Set_FootPosition(m_pTransformCom->Get_WorldMatrix_XMMat());
    m_pTransformCom->LookAt_Horizontal({ -4.f,-2.f,2.f,1 });

    m_bInvincivle = false;
    m_bPlayerLook = false;
    m_bPattern_Timer_On = false;
    m_fPatternTimer = 0.f;

    m_bDance_Event = true;
    BEATMANAGER->Stop_Sequence();
    GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain");
    BEATMANAGER->Reset();
    GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_DanceBattle_Loop", true);
    //BEATMANAGER->Queue_Sequence(L"Mimosa_DanceBattle_Loop");

    Start_Dance();
    CUi_SpecialComboAttackSystem::SPComboAttack spAttack = {};
    spAttack.eSPCombo_Type = CUi_SpecialComboAttackSystem::SPCombo_Type::Mimosa;
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_SpecialComboAttackSystem::ObjID,
        m_iLevelIndex, L"CUi", CUi_SpecialComboAttackSystem::ObjID, &spAttack);
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_MImosaMusicBattleSystem::ObjID,
        m_iLevelIndex, L"CUi", CUi_MImosaMusicBattleSystem::ObjID);
    m_bDance_Ready = true;

    PLAYERMANAGER->Callback_Event("Mimosa_DanceBattle");

    m_vecBackDancer.clear();
    CBackDancer::BackDancer_Info bInfo = {};

    for (size_t i = 0; i < 6; i++)
    {
        _float4x4 mmMat = {};
        //string emName = {};
        //if (i == 0)
        //    emName = "em0100";
        //else
        //    emName = "em0100_" + to_string(i);
        //CCine_Manager::Get_Instance()->GetMatTrans("em5100", mMat);

        mmMat = m_pTransformCom->Get_WorldMatrix();

        switch (i)

        {
        case 0:
        {
            mmMat.m[3][0] -= 3.f;
            mmMat.m[3][2] -= 3.f;
            break;
        }
        case 1:
        {
            mmMat.m[3][0] -= 2.f;
            mmMat.m[3][2] -= 2.f;
            break;
        }
        case 2:
        {
            mmMat.m[3][0] -= 1.f;
            mmMat.m[3][2] -= 1.f;
            break;
        }
        case 3:
        {
            mmMat.m[3][0] -= 1.f;
            mmMat.m[3][2] += 1.f;
            break;
        }
        case 4:
        {
            mmMat.m[3][0] -= 2.f;
            mmMat.m[3][2] += 2.f;
            break;
        }
        case 5:
        {
            mmMat.m[3][0] -= 3.f;
            mmMat.m[3][2] += 3.f;
            break;
        }
        }
        _float4 fPos = { mmMat.m[3][0], mmMat.m[3][1], mmMat.m[3][2],1 };
        bInfo.bSpotLight = false;
        bInfo.fInitPos = { fPos };
        bInfo.listDanceEvent = m_listDanceEvent;
        bInfo.list_Mimosa_Dance_Delay = m_list_Mimosa_Dance_Delay;
        m_vecBackDancer.push_back(GAMEINSTANCE->Add_Clone_Return(GAMEINSTANCE->Get_Current_LevelID(), TEXT("Layer_Monster"), CBackDancer::ObjID, &bInfo));

    }
   

}

void CMonster_Mimosa::Move_To_CutScene_Pos_4()
{
    GAMEINSTANCE->Stop_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_DanceBattle_Loop");

    m_pModelCom->Set_AnimIndex("em5100_idle-air_000", false);

    m_strCurrent_Pattern = "em5100_atk-air_260";
    m_strNext_Pattern = "em5100_atk-air_260";

    m_bTurn = true;
    m_bFlyState = true;
    GravityOff();
    _float4x4 mMat = {};
    CCine_Manager::Get_Instance()->GetMatTrans("em5100", mMat);


    Wing_On(0.01f);
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&mMat));
    m_pCCT->Set_Position(m_pTransformCom->Get_WorldMatrix_XMMat());
    CUi_Mgr::Get_Instance()->Ui_AllActive(true);


    m_bDance_Event = false;
    m_bDance_Ready = false;
    PLAYERMANAGER->Callback_Event("Mimosa_DanceBattle_End");
    CMapManager::Get_Instance()->Boss_ChangePhase((_uint)CBoss_InteractObj::MIMOSA_PHASE::PHASE_4);
    CMapManager::Get_Instance()->Boss_ChangePattern((_uint)CBoss_InteractObj::MIMOSA_PATTERN::IDLE);

}

void CMonster_Mimosa::Wing_On(_float fOnTime)
{
    m_bWingOff = false;
    m_fWingChange_Speed = 1.f / fOnTime;
}

void CMonster_Mimosa::Wing_Off(_float fOffTime)
{
    m_bWingOff = true;
    m_fWingChange_Speed = 1.f / fOffTime;
}

void CMonster_Mimosa::Wing_Controller(_float fTimeDelta)
{
    if (m_bWingOff)
    {
        m_fWingSize -= m_fWingChange_Speed * fTimeDelta;

        if (m_fWingSize < 0.001f)
            m_fWingSize = 0.001f;

        _uint iWingRNum = m_pModelCom->Get_BoneIdx("attach_R_000");
        _uint iWingLNum = m_pModelCom->Get_BoneIdx("attach_L_000");

        _float4x4 matResult{};

        XMStoreFloat4x4(&matResult, XMMatrixScaling(m_fWingSize, m_fWingSize, m_fWingSize) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("attach_R_000")));
        m_pModelCom->Set_Basic_BoneMatrix(iWingRNum, XMLoadFloat4x4(&matResult));
        XMStoreFloat4x4(&matResult, XMMatrixScaling(m_fWingSize, m_fWingSize, m_fWingSize) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("attach_L_000")));
        m_pModelCom->Set_Basic_BoneMatrix(iWingLNum, XMLoadFloat4x4(&matResult));
    }
    else if (!m_bWingOff && m_fWingSize < 1.f)
    {
        m_fWingSize += m_fWingChange_Speed * fTimeDelta;

        if (m_fWingSize > 1.f)
            m_fWingSize = 1.f;

        _uint iWingRNum = m_pModelCom->Get_BoneIdx("attach_R_000");
        _uint iWingLNum = m_pModelCom->Get_BoneIdx("attach_L_000");

        _float4x4 matResult{};

        if(m_bRightWing_On)
        {
            XMStoreFloat4x4(&matResult, XMMatrixScaling(m_fWingSize, m_fWingSize, m_fWingSize) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("attach_R_000")));
            m_pModelCom->Set_Basic_BoneMatrix(iWingRNum, XMLoadFloat4x4(&matResult));
        }
        if(m_bLeftWing_On)
        {
            XMStoreFloat4x4(&matResult, XMMatrixScaling(m_fWingSize, m_fWingSize, m_fWingSize) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("attach_L_000")));
            m_pModelCom->Set_Basic_BoneMatrix(iWingLNum, XMLoadFloat4x4(&matResult));
        }
    }
}

void CMonster_Mimosa::Off_Attach_Wing()
{
    _uint iFakeWingRNum = m_pModelCom->Get_BoneIdx("catch_attach_R_000");
    _uint iFakeWingLNum = m_pModelCom->Get_BoneIdx("catch_attach_L_000");

    _float4x4 matResult{};

    XMStoreFloat4x4(&matResult, XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("catch_attach_R_000")));
    m_pModelCom->Set_Basic_BoneMatrix(iFakeWingRNum, XMLoadFloat4x4(&matResult));
    XMStoreFloat4x4(&matResult, XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("catch_attach_L_000")));
    m_pModelCom->Set_Basic_BoneMatrix(iFakeWingLNum, XMLoadFloat4x4(&matResult));
}

void CMonster_Mimosa::Judge_Attack(_float fTimeDelta)
{
    _int CurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    _float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float BeatPercent = Timer / Beat;
    _float JudgeBeat = 0;

    shared_ptr<CChai> pChai = PLAYERMANAGER->Get_Player();
    _int currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
    if ((currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_020") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_010") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_011") ||
        currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_040")))
    {
        JudgeBeat = CurBeat - (m_iAttackReadyStartBeat + 8.f);
    }
    //else if(m_iParry_Success_Count == 1 && currAnimIndex == m_pModelCom->Get_StringAnimIndex("em5100_atk-rp-cp_040"))
    //{
    //    JudgeBeat = CurBeat - (m_iAttackReadyStartBeat + 8.f);
    //}

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
                    m_iRP_Success_Count++;
                    if (m_iRP_Success_Count > 4)
                    {
                        m_iParry_Success_Count++;
                        m_iRP_Success_Count = 0;
                    }
                }

                pChai->Get_Player_Camera()->Shake(0.1f, 0.1f);
            }
            else
            {
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

void CMonster_Mimosa::SetPos_Hard(_fvector vPos)
{
    m_pTransformCom->Set_Position(vPos);
}

void CMonster_Mimosa::MoveToPos(_fvector vTargetPos)
{
}

void CMonster_Mimosa::MonsterDead()
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMMatrixIdentity());
    //PlayEffect("Enemy_Boom", matResult, 1.f, true);

    //Dead();
    //Make_PickUp_Object(5, CPickUpItem::PICKUP::GEAR);
    //CCombat_Manager::Get_Instance()->Erase_Enemy(shared_from_this());
}

_bool CMonster_Mimosa::IsAnimFinished()
{
    return m_pModelCom->Get_Finished();
}

void CMonster_Mimosa::SetAnim(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndexNonCancle(strAnimName, isLoop);
}

void CMonster_Mimosa::SetAnimHard(const string& strAnimName, _bool isLoop)
{
    m_pModelCom->Set_AnimIndex_NoInterpole(strAnimName, isLoop);
}

void CMonster_Mimosa::GravityOn()
{
    m_pCCT->Enable_Gravity();
}
void CMonster_Mimosa::GravityOff()
{
    m_pCCT->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    m_pCCT->Disable_Gravity();
}

void CMonster_Mimosa::AirStateON()
{
    m_isAirState = true;
    GravityOff();
}
void CMonster_Mimosa::AirStateOFF()
{
    m_isAirState = false;
    GravityOn();
}

void CMonster_Mimosa::MoveStraight(_float fSpeed)
{
    _vector vCurLook = m_pTransformCom->Get_NormLook();
    m_pCCT->Add_Displacement(vCurLook * fSpeed);
}

void CMonster_Mimosa::MoveUp(_float fSpeed)
{
    _vector vUpVector = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    m_pCCT->Add_Displacement(vUpVector * fSpeed);
}

void CMonster_Mimosa::Set_Position(_matrix mMat)
{
    m_pCCT->Set_Position(mMat);
}

void CMonster_Mimosa::CCT_ON()
{
    m_isCCTWork = true;
    m_pCCT->Disable(false);
}

void CMonster_Mimosa::CCT_OFF()
{
    m_isCCTWork = false;
    m_pCCT->Disable(true);
}

void CMonster_Mimosa::PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * m_pTransformCom->Get_WorldMatrix_XMMat());
    CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strEffect, &matResult, fmultiplier , isBillBoard);
}
shared_ptr<CEffect_Union> CMonster_Mimosa::PlayEffect_Return(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard)
{
    _float4x4 matResult{};
    XMStoreFloat4x4(&matResult, XMLoadFloat4x4(&matTrans) * m_pTransformCom->Get_WorldMatrix_XMMat());
    return CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect_Return(strEffect, &matResult, fmultiplier, isBillBoard);
}
void CMonster_Mimosa::PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard)
{
    CEffect_Manager::Get_Instance()->Call_Union_Effect(strEffect, &m_matCurrentPos, fmultiplier , isBillBoard);
}

void CMonster_Mimosa::Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType)
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

void CMonster_Mimosa::Initialize_Ui()
{
    auto pUI = static_pointer_cast<CUi_MimosaSystem>(CUi_Mgr::Get_Instance()->Find_UI(CUi_MimosaSystem::ObjID));
    if (pUI == nullptr)
        assert(false);
    pUI->Set_Hp(&m_fCurHp);
    pUI->Set_Groggy(&m_fBarrier);
}

shared_ptr<CMonster_Mimosa> CMonster_Mimosa::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_Mimosa
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Mimosa(pDevice, pContext) {}
    };

    shared_ptr<CMonster_Mimosa> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Mimosa"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_Mimosa::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_Mimosa
    {
        MakeSharedEnabler(const CMonster_Mimosa& rhs) : CMonster_Mimosa(rhs) {}
    };

    shared_ptr<CMonster_Mimosa> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Mimosa"));
        assert(false);
    }

    return pInstance;
}

void CMonster_Mimosa::Free()
{
}
