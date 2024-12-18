#include "CMonster_Korsica.h"
#include "GameInstance.h"
#include "Collision_Manager.h"
#include "PlayerManager.h"
#include "Shader.h"
#include "Model.h"
#include "CharacterController.h"
#include "CombatManager.h"
#include "iostream"
#include "Weapon_Korsica.h"
#include "CUi_EnemySpecialAttack.h"
#include "Beat_Manager.h"
#include "CUi_Mgr.h"
#include "CUi_KorsicaSystem.h"
#include "CEvent_Chai.h"
#include "CKorsicaCamera.h"
#include "Effect_Manager.h"
#include "CUi_TalkSystem.h"
#include "MapManager.h"
#include "CUi_StageResultSystem.h"
#include "Cine_Manager.h"
#include "UI_Event_Cut.h"

wstring CMonster_Korsica::ObjID = TEXT("CMonster_Korsica");

CMonster_Korsica::CMonster_Korsica(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonsterBase{ pDevice, pContext }
{
}

CMonster_Korsica::CMonster_Korsica(const CMonster_Korsica& rhs)
    : CMonsterBase{ rhs }
{
}

CMonster_Korsica::~CMonster_Korsica()
{
}

HRESULT CMonster_Korsica::Initialize_Prototype()
{
    _matrix		LocalTransformMatrix = XMMatrixIdentity();
    LocalTransformMatrix = XMMatrixScaling(0.0045f, 0.0045f, 0.0045f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Model_Monster_Korsica"), TEXT("../Bin/Resources/Models/Monsters/Korsica/Korsica.fbx"), MODEL_TYPE::ANIM, preMatrix)
    );
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Parry_GT"), TEXT("../Bin/Resources/Sounds/SFX/Parry_GT.wav")));
    m_List_Owning_ObjectID.push_back(CWeapon_Korsica::ObjID);
    m_List_Owning_ObjectID.push_back(CEvent_Chai::ObjID);
    m_List_Owning_ObjectID.push_back(CKorsicaCamera::ObjID);

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica1_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter1_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica1_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter1_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica1_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter1_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica1_4"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter1_4.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica1_5"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter1_5.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica1_6"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter1_6.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica2_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter2_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica3_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter3_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica3_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter3_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica3_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter3_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica3_4"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter3_4.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica3_5"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter3_5.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica3_6"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter3_6.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica4_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter4_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica4_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter4_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica4_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter4_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica5_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter5_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica6_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter6_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica6_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter6_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica6_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter6_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica6_4"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter6_4.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica7_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter7_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica8_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter8_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica8_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter8_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica8_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter8_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica8_4"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter8_4.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica8_5"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter8_5.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica8_6"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter8_6.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica9_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter9_1.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica9_2"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter9_2.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica9_3"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter9_3.ogg")));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("vsKorsica10_1"), TEXT("../Bin/Resources/Sounds/Ui/Talk/vsKor/vsKorChapter10_1.ogg")));

    return S_OK;
}

HRESULT CMonster_Korsica::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        assert(false);

    Ready_Components();

    m_Radius = 4.35f;
    _float3 Pos = { 0, 3.5f, -m_Radius };
    m_Middle.x = Pos.x;
    m_Middle.y = Pos.y;
    m_Middle.z = 0.f;

    m_pTransformCom->Set_Position(XMLoadFloat3(&Pos));
    PrepareCallbacks();
    Initialize_Weapon();
    Initialize_Camera();
    Initialize_EventChai();

    m_iDeadBone = m_pModelCom->Get_BoneIdx("event_pos");
    m_iCameraBone = m_pModelCom->Get_BoneIdx("prop_02");
    //CCombat_Manager::Get_Instance()->Register_Enemy(shared_from_this());
   
    m_AttackBeatVec.reserve(7);
    m_AttackParryBeatVec.reserve(7);

    Set_Pattern(PatternType::Chat);
    Set_Anim(AnimType::Idle, false);
    CUi_KorsicaSystem::Desc Desc;
    Desc.CorsicaHp = &m_fCurHp;
    CUi_Mgr::Get_Instance()->Add_Clone_Return
    (CUi_KorsicaSystem::ObjID, m_iLevelIndex, L"CUi", CUi_KorsicaSystem::ObjID, &Desc);
    
    Set_ChaiPos_Immediate();

    CBeat_Manager::Get_Instance()->Set_Beat(0.416667f);
    //CBeat_Manager::Get_Instance()->Start_Sequence();

    m_pRealTransform->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());

    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pRealTransform->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pRealTransform->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        }
    );

    GAMEINSTANCE->Add_Unique_Sound_Channel("BattleKorsica");


    // Add callbacks
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0620", 4, 0,
        [&](_float4, _float4, _float3) {
            CUI_Event_Cut::Desc Desc;
            Desc.eCutSceneType = CUI_Event_Cut::CutSceneType::Korsica;
            CUi_Mgr::Get_Instance()->Add_Clone(CUI_Event_Cut::ObjID, LEVEL_STATIC, L"CUi", CUI_Event_Cut::ObjID, &Desc);
        }
    );
    CCine_Manager::Get_Instance()->AddSequenceCallback("CS0620", 4, 1,
        [&](_float4, _float4, _float3) {
            CUi_Mgr::Get_Instance()->Ui_Dead(CUI_Event_Cut::ObjID);
        }
    );

    return S_OK;
}

void CMonster_Korsica::PriorityTick(_float fTimeDelta)
{
    m_bIsInGame = CCine_Manager::Get_Instance()->IsInGame();
    if (!m_bIsInGame)
        return;
    PartPriorityTick(fTimeDelta);
}

void CMonster_Korsica::Tick(_float fTimeDelta)
{
    if (!m_bIsInGame)
        return;
	Testfuction(fTimeDelta);
    Progress_Pattern(fTimeDelta);
	Progress_Anim(fTimeDelta);
    Progress_Weapon_Anim(fTimeDelta);
	PartTick(fTimeDelta);
    //m_pTransformCom->LookAt_Horizontal(XMLoadFloat3(&m_Middle));
    //_vector vPos = m_pTransformCom->Get_Position();
    //_vector vTargetPos = XMLoadFloat4(&m_vTargetPosition);
    //_vector vDir = vTargetPos - vPos;
    //if (XMVectorGetX(XMVector3Length(vDir)) < 0.05f)
    //{
    //    m_pTransformCom->Set_Position(vTargetPos);
    //}
    //else
    //{
    //    m_pTransformCom->Set_Position(vPos + vDir * fTimeDelta * 8.f);
    //}

    // Call all reserved effects
    m_pModelCom->PlayReservedCallbacks();
}

void CMonster_Korsica::LateTick(_float fTimeDelta)
{
    if (!m_bIsInGame)
        return;
    Judge_Attack(fTimeDelta);
    Adjust_LookAngle(fTimeDelta);
    AdjustCurAnim_ChaiPos();
    Adjust_Camera(fTimeDelta);
    PartLateTick(fTimeDelta); 
    if(!m_bMissed)
        m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    m_pRealTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_MatOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
}

void CMonster_Korsica::PartPriorityTick(_float fTimeDelta)
{
    m_pWeapon_Right->PriorityTick(fTimeDelta);
    m_pWeapon_Left->PriorityTick(fTimeDelta);
}

void CMonster_Korsica::PartTick(_float fTimeDelta)
{
    m_pWeapon_Right->Tick(fTimeDelta);
    m_pWeapon_Left->Tick(fTimeDelta);
}

void CMonster_Korsica::PartLateTick(_float fTimeDelta)
{
    m_pWeapon_Right->LateTick(fTimeDelta);
    m_pWeapon_Left->LateTick(fTimeDelta);
}

HRESULT CMonster_Korsica::Render()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    //_float4x4 World;
    //XMStoreFloat4x4(&World, XMLoadFloat4x4(&m_MatOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
    //m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World);
    if (FAILED(m_pRealTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
        //    return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //    return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            continue;

        _bool bNormal = true;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        {
            bNormal = false;
        }

        if (i == 0)        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::NormalMap);
        else               m_pShaderCom->Begin(0);

        if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
            continue;

        m_pModelCom->Draw((_uint)i);
        if (i == 0)
            continue;

        m_pShaderCom->Bind_Float("g_OutlineWidth", 0.8f);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        m_pModelCom->Draw((_uint)i);
    }

    return S_OK;
}

HRESULT CMonster_Korsica::Render_Shadow()
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

void CMonster_Korsica::TakeDamage(const DamageInfo& damageInfo)
{

}

void CMonster_Korsica::Set_Anim(AnimType Type, _bool Loop, _bool bInterPole)
{
    m_ePreviousAnim = (AnimType)m_pModelCom->Get_CurrentAnimIndex();
    m_pModelCom->Set_AnimIndex((_uint)Type, Loop, bInterPole);
    if(m_ePreviousAnim != (AnimType)m_pModelCom->Get_CurrentAnimIndex())
        m_pCamera->Start_Lerp();
    switch (Type)
    {
    case AnimType::Idle:
        m_pChai->Set_Anim(CEvent_Chai::AnimType::Idle, true);
        break;
    case AnimType::IdletoMove:
        m_pChai->Set_Anim(CEvent_Chai::AnimType::Idletomove, false);
        break;
    case AnimType::Move:
        m_pChai->Set_Anim(CEvent_Chai::AnimType::Move, true);
        break;
    case AnimType::MovetoAttackReady:
        m_pChai->Set_Anim(CEvent_Chai::AnimType::Movetoparryready, false);
        break;
    case AnimType::AttackReadytoIdle:
        m_pChai->Set_Anim(CEvent_Chai::AnimType::Idle, true);
        break;
    case AnimType::AttackReadytoMove:
        m_pChai->Set_Anim(CEvent_Chai::AnimType::Idletomove, false);
        break;
    case AnimType::AttackEndtoBack:
        break;
    case AnimType::AttackEndtoFly:
        break;
    case AnimType::FourAttack:
        break;
    case AnimType::FiveAttack:
        break;
    case AnimType::SixAttack:
        break;
    case AnimType::NonParryFourAttack:
        break;
    case AnimType::NonParryFiveAttack:
        break;
    case AnimType::NonParryFiveAttackTwo:
        break;
    case AnimType::FourAttackTwo:
        break;
    case AnimType::NonParryFourAttackTwo:
        break;
    case AnimType::NonParrySevenAttack:
        break;
    case AnimType::NonParryTwoAttack:
        break;
    case AnimType::NonParryThreeAttack:
        break;
    case AnimType::Dummy:
        break;
    case AnimType::NonParrySevenAttackTwo:
        break;
    case AnimType::DummyTwo:
        break;
    case AnimType::NonParryFiveAttackThree:
        break;
    case AnimType::DeadFly:
    {
        CMapManager::Get_Instance()->Boss_ChangePhase(1);
        m_pChai->Set_Anim(CEvent_Chai::AnimType::Catchrun, false);
        m_pChai->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
        //auto pChaiModel = m_pChai->Get_Model();
        //m_pCamera->Start_Following_Bone(m_pChai->Get_Transform()->Get_WorldMatrix_Ptr(), const_cast<_float4x4*>(pChaiModel->Get_BoneMatrixPtr("prop_02")));
    }
        break;
    case AnimType::PerfectCatch:
        break;
    case AnimType::GoodCatch:
        break;
    case AnimType::MissCatch:
        break;
    case AnimType::End:
        break;
    case AnimType::EndMark:
        break;
    default:
        break;
    }


}

void CMonster_Korsica::Set_Pattern(PatternType ePattern, AnimType Type)
{
    m_ePattern = ePattern;
    switch (m_ePattern)
    {
    case PatternType::Chat:
        if (Type == AnimType::End)
            Set_Anim(AnimType::Idle);
        else
            Set_Anim(Type);
        break;
    case PatternType::Select:
        if (Type == AnimType::End)
            Set_Anim(AnimType::Move);
        else
            Set_Anim(Type);
        break;
    case PatternType::Dead:
        if (Type == AnimType::End)
            Set_Anim(AnimType::DeadFly);
        else
            Set_Anim(Type);
        break;
    default:
        if (Type == AnimType::End)
        {
            Set_Anim(AnimType::MovetoAttackReady);
        }
        else
        {
            Set_Anim(Type);
        }
        break;
    }

    if (m_iPattern > 9)
    {
        m_iPattern = 0;
    }
}

void CMonster_Korsica::Set_ChaiPos()
{
    _float3 Look;
    XMStoreFloat3(&Look, m_pTransformCom->Get_NormLook());
    _float3 Pos;
    XMStoreFloat3(&Pos, m_pTransformCom->Get_Position());
    Pos.x += Look.x * m_Radius * 1.98f;
    Pos.y += Look.y * m_Radius * 1.98f;
    Pos.z += Look.z * m_Radius * 1.98f;
    m_pChai->Set_Player_Position(XMLoadFloat3(&Pos));
}

void CMonster_Korsica::Set_ChaiPos_Immediate()
{
    _float3 Look;
    XMStoreFloat3(&Look, m_pTransformCom->Get_NormLook());
    _float3 Pos;
    XMStoreFloat3(&Pos, m_pTransformCom->Get_Position());
    Pos.x += Look.x * m_Radius * 1.98f;
    Pos.y += Look.y * m_Radius * 1.98f;
    Pos.z += Look.z * m_Radius * 1.98f;
    m_pChai->Set_Player_Position_Immediate(XMLoadFloat3(&Pos));
}

void CMonster_Korsica::Set_DelayTime()
{
    _float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
    m_fDelayTime = Beat - Timer;
}

void CMonster_Korsica::Progress_Pattern(_float fTimeDelta)
{
    m_bIdle = false;
    switch (m_ePattern)
    {
    case PatternType::Chat:
        Progress_Chat(fTimeDelta);
        break;
    case PatternType::Select:
        Progress_Select(fTimeDelta);
        break;
    case PatternType::FourAttack:
        Progress_FourAttack(fTimeDelta);
        break;
    case PatternType::FiveAttack:
        Progress_FiveAttack(fTimeDelta);
        break;
    case PatternType::NonParryFourAttack:
        Progress_NonParryFourAttack(fTimeDelta);
        break;
    case PatternType::NonParryFiveAttack:
        Progress_NonParryFiveAttack(fTimeDelta);
        break;
    case PatternType::NonParrySevenAttack:
        Progress_NonParrySevenAttack(fTimeDelta);
        break;
    case PatternType::SixAttack:
        Progress_SixAttack(fTimeDelta);
        break;
    case PatternType::NonParryTwoAttack:
        Progress_NonParryTwoAttack(fTimeDelta);
        break;
    case PatternType::NonParrySevenAttackTwo:
        Progress_NonParrySevenAttackTwo(fTimeDelta);
        break;
    case PatternType::ContinueAttack:
        Progress_ContinueAttack(fTimeDelta);
        break;
    case PatternType::ContinueAttackTwo:
        Progress_ContinueAttackTwo(fTimeDelta);
        break;
    case PatternType::FinalAttack:
        Progress_FinalAttack(fTimeDelta);
        break;
    case PatternType::Dead:
        Progress_Dead(fTimeDelta);
        break;
    case PatternType::Test:
        Progress_Test(fTimeDelta);
        break;
    default:
        assert(false);
        break;
    }
}

void CMonster_Korsica::Progress_Chat(_float fTimeDelta)
{
    map<_int, wstring> chatFiles = {
       {0, L"VsKorsica Talk_File_1.csv"},
       {1, L"VsKorsica Talk_File_2.csv"},
       {2, L"VsKorsica Talk_File_3.csv"},
       {3, L"VsKorsica Talk_File_4.csv"},
       {4, L"VsKorsica Talk_File_5.csv"},
       {5, L"VsKorsica Talk_File_6.csv"},
       {6, L"VsKorsica Talk_File_7.csv"},
       {7, L"VsKorsica Talk_File_8.csv"},
       {8, L"VsKorsica Talk_File_9.csv"}
    };

    if (AnimBeyond(AnimType::Idle) || AnimBeyond(AnimType::AttackReadytoIdle))
    {
        Set_Anim(AnimType::IdletoMove);
    }
    else if (AnimBeyond(AnimType::IdletoMove) || AnimBeyond(AnimType::AttackReadytoMove))
    {
        if (chatFiles.find(m_iTalkPatternCount) != chatFiles.end())
        {
            m_isTalkOver = false;

            auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
            if (pTalkSystem == nullptr)
            {
                CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
                TalkDesc.endCallback = [&]() {
                    m_isTalkOver = true;
                    };
                pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_KORSICA, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
            }

            auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
            if (PRequiredTalkSystem != nullptr)
            {
                PRequiredTalkSystem->LoadTalkFile(chatFiles[m_iTalkPatternCount]);
                
                CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
            }
        }

        m_pCamera->Set_Follow(true);
        Set_Anim(AnimType::Move, true);
    }
    else if (m_pModelCom->Get_CurrentAnimIndex() == _uint(AnimType::Move) && !m_bChat)
    {
        Set_DelayTime();
        Set_Pattern(PatternType::Select);

        m_iTalkPatternCount++;
    }
    m_bIdle = true;
    Set_ChaiPos();
}

void CMonster_Korsica::Progress_Select(_float fTimeDelta)
{
    m_fDelayTime -= fTimeDelta;
    if (m_fDelayTime <= 0)
    {
        switch (m_iPattern)
        {
        case 0:
            Set_Pattern(PatternType::FourAttack);
            break;
        case 1:
            Set_Pattern(PatternType::FiveAttack);
            break;
        case 2:
            Set_Pattern(PatternType::SixAttack);
            break;
        case 3:
            Set_Pattern(PatternType::NonParryFourAttack);
            break;
        case 4:
            Set_Pattern(PatternType::NonParryFiveAttack);
            break;
        case 5:
            Set_Pattern(PatternType::NonParrySevenAttack);
            break;
        case 6:
            Set_Pattern(PatternType::ContinueAttack);
            break;
        case 7:
            Set_Pattern(PatternType::ContinueAttackTwo);
            break;
        case 8:
            Set_Pattern(PatternType::FinalAttack);
            break;
        case 9:
            Set_Pattern(PatternType::Test);
            break;
        default:
            break;
        }
    }
}

void CMonster_Korsica::Progress_FourAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        _uint i = m_pGameInstance.lock()->Random_Int(0, 1);
        switch (i)
        {
        case 0:
            Set_Ui(AnimType::FourAttack);
            Set_Anim(AnimType::FourAttack);
            break;
        case 1:
            Set_Ui(AnimType::FourAttackTwo);
            Set_Anim(AnimType::FourAttackTwo);
            break;
        default:
            break;
        }
    }
    else if (AnimBeyond(AnimType::FourAttack)
       || AnimBeyond(AnimType::FourAttackTwo))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::FourAttack, 0.4f)
        || AnimBeyond(AnimType::FourAttackTwo, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_FiveAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        Set_Ui(AnimType::FiveAttack);
        Set_Anim(AnimType::FiveAttack);
    }
    else if (AnimBeyond(AnimType::FiveAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        m_pCamera->Set_Follow(false);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::FiveAttack, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_NonParryTwoAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        Set_Ui(AnimType::NonParryTwoAttack);
        Set_Anim(AnimType::NonParryTwoAttack);
    }
    else if (AnimBeyond(AnimType::NonParryTwoAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoMove);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::NonParryTwoAttack, 0.35f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_NonParryFourAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        _uint i = m_pGameInstance.lock()->Random_Int(0, 1);
        switch (i)
        {
        case 0:
            Set_Ui(AnimType::NonParryFourAttack);
            Set_Anim(AnimType::NonParryFourAttack);
            break;
        case 1:
            Set_Ui(AnimType::NonParryFourAttackTwo);
            Set_Anim(AnimType::NonParryFourAttackTwo);
            break;
        default:
            assert(false);
            break;
        }

    }
    else if (AnimBeyond(AnimType::NonParryFourAttack)
        || AnimBeyond(AnimType::NonParryFourAttackTwo))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::NonParryFourAttack, 0.4f)
        || AnimBeyond(AnimType::NonParryFourAttackTwo, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_NonParryFiveAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        _uint i = m_pGameInstance.lock()->Random_Int(0, 2);
        switch (i)
        {
        case 0:
            Set_Ui(AnimType::NonParryFiveAttack);
            Set_Anim(AnimType::NonParryFiveAttack);
            break;
        case 1:
            Set_Ui(AnimType::NonParryFiveAttackTwo);
            Set_Anim(AnimType::NonParryFiveAttackTwo);
            break;
        case 2:
            Set_Ui(AnimType::NonParryFiveAttackThree);
            Set_Anim(AnimType::NonParryFiveAttackThree);
            break;
        default:
            break;
        }
    }
    else if (AnimBeyond(AnimType::NonParryFiveAttack)
        || AnimBeyond(AnimType::NonParryFiveAttackTwo)
        || AnimBeyond(AnimType::NonParryFiveAttackThree))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::NonParryFiveAttack, 0.4f)
        || AnimBeyond(AnimType::NonParryFiveAttackTwo, 0.4f)
        || AnimBeyond(AnimType::NonParryFiveAttackThree, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_NonParrySevenAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        Set_Ui(AnimType::NonParrySevenAttack);
        Set_Anim(AnimType::NonParrySevenAttack);
    }
    else if (AnimBeyond(AnimType::NonParrySevenAttack))
    {
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::NonParrySevenAttack, 0.45f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_NonParrySevenAttackTwo(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        Set_Ui(AnimType::NonParrySevenAttackTwo);
        Set_Anim(AnimType::NonParrySevenAttackTwo);
    }
    else if (AnimBeyond(AnimType::NonParrySevenAttackTwo))
    {
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::NonParrySevenAttackTwo, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_SixAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::EndMark)
    {
        Set_Ui(AnimType::SixAttack);
        Set_Anim(AnimType::SixAttack);
    }
    else if (AnimBeyond(AnimType::SixAttack))
    {
        Adjust_KorsicaPos_From_Chai(fTimeDelta);

        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::SixAttack, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_ContinueAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady))
    {
        Set_Ui(AnimType::NonParryTwoAttack);
        Set_Anim(AnimType::NonParryTwoAttack);
    }
    else if (AnimBeyond(AnimType::NonParryTwoAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_ChaiPos();
        Set_Pattern(PatternType::ContinueAttack, AnimType::NonParryThreeAttack);
        Set_Ui(AnimType::NonParryThreeAttack);
    }
    else if (AnimBeyond(AnimType::NonParryThreeAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_ChaiPos();
        Set_Pattern(PatternType::ContinueAttack, AnimType::FourAttack);
        Set_Ui(AnimType::FourAttack);
    }
    else if (AnimBeyond(AnimType::FourAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        m_pCamera->Set_Follow(false);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::NonParryTwoAttack, 0.4f)
        || AnimBeyond(AnimType::NonParryThreeAttack, 0.4f)
        || AnimBeyond(AnimType::FourAttack, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_ContinueAttackTwo(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady))
    {
        Set_Ui(AnimType::NonParrySevenAttackTwo);
        Set_Anim(AnimType::NonParrySevenAttackTwo);
    }
    else if (AnimBeyond(AnimType::NonParrySevenAttackTwo))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_ChaiPos();
        Set_Pattern(PatternType::ContinueAttackTwo, AnimType::NonParryTwoAttack);
        Set_Ui(AnimType::NonParryTwoAttack);
    }
    else if (AnimBeyond(AnimType::NonParryTwoAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_ChaiPos();
        Set_Pattern(PatternType::ContinueAttackTwo, AnimType::FourAttack);
        Set_Ui(AnimType::FourAttack);
    }
    else if (AnimBeyond(AnimType::FourAttack))
    {
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        m_pCamera->Set_Follow(false);
        Set_Pattern(PatternType::Chat, AnimType::AttackReadytoIdle);
        Set_Chat(true);
        ++m_iPattern;
    }

    if (AnimBeyond(AnimType::NonParrySevenAttackTwo, 0.4f)
        || AnimBeyond(AnimType::NonParryTwoAttack, 0.4f)
        || AnimBeyond(AnimType::FourAttack, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_FinalAttack(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady))
    {
        Set_Anim(AnimType::NonParrySevenAttack);
        Set_Ui(AnimType::NonParrySevenAttack);
    }
    else if (AnimBeyond(AnimType::NonParrySevenAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_ChaiPos();
        Set_Pattern(PatternType::FinalAttack, AnimType::NonParryThreeAttack);
        Set_Ui(AnimType::NonParryThreeAttack);
    }
    else if (AnimBeyond(AnimType::NonParryThreeAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_ChaiPos();
        Set_Pattern(PatternType::FinalAttack, AnimType::SixAttack);
        Set_Ui(AnimType::SixAttack);
    }
    else if (AnimBeyond(AnimType::SixAttack))
    {
        //Adjust_KorsicaPos(fTimeDelta);
        Adjust_KorsicaPos_From_Chai(fTimeDelta);
        Set_ChaiPos();
        Set_Pattern(PatternType::FinalAttack, AnimType::NonParryFiveAttackThree);
        Set_Ui(AnimType::NonParryFiveAttackThree);
    }
    else if (AnimBeyond(AnimType::NonParryFiveAttackThree, 0.9f))
    {
        Set_Pattern(PatternType::Dead);
        m_fCurHp = 0;
    }

    if (AnimBeyond(AnimType::NonParrySevenAttack, 0.4f)
        || AnimBeyond(AnimType::NonParryThreeAttack, 0.4f)
        || AnimBeyond(AnimType::SixAttack, 0.4f)
        || AnimBeyond(AnimType::NonParryFiveAttackThree, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Adjust_Camera(_float fTimeDelta)
{
    if (m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::IdletoMove
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.07f)
    {  
        m_pCamera->Set_Follow(true);
    }

    if (m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::AttackReadytoIdle
        && m_pModelCom->Get_Current_Anim_Track_Position_Percentage() > 0.05f)
    {
        m_pCamera->Set_Follow(false);
    }
}

void CMonster_Korsica::Set_Ui(AnimType eType)
{
    m_AttackBeatVec.clear();
    m_AttackParryBeatVec.clear();
    m_iAttackStartBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    _bool bShort = false;

    switch (eType)
    {
    case AnimType::FourAttackTwo:
        m_AttackBeatVec = { 1, 0.5f, 0.5f, 1.f };
        m_AttackParryBeatVec = { true, true, true, true };
        bShort = true;
        break;
    case AnimType::FourAttack:
        m_AttackBeatVec = { 1, 1, 2.5f, 1 };
        m_AttackParryBeatVec = { true, true, true, true };
        break;
    case AnimType::FiveAttack: 
        m_AttackBeatVec = { 1, 2.f, 0.5f, 1.5f, 0.5f };
        m_AttackParryBeatVec = { true, true, true, true, true };
        break;
    case AnimType::SixAttack:
        m_AttackBeatVec = { 1, 0.5f, 0.5f, 2.5f, 1.5f, 1.f };
        m_AttackParryBeatVec = { true, true, true, true, true, true };
        break;
    case AnimType::NonParryFourAttack:
        m_AttackBeatVec = { 1, 2, 2.5f, 1.5f };
        m_AttackParryBeatVec = { true, false, true, true };
        break;
    case AnimType::NonParryFiveAttack:   
        m_AttackBeatVec = { 1, 0.5f, 0.5f, 2.5f, 2.5f };
        m_AttackParryBeatVec = { true, true, true, true, false };
        break;
    case AnimType::NonParryFiveAttackTwo:
        m_AttackBeatVec = { 1, 2.5f, 2, 1, 0.5f };
        m_AttackParryBeatVec = { true, false, true, true, true };
        break;
    case AnimType::NonParryFourAttackTwo:
        m_AttackBeatVec = { 1, 1, 4, 2.5f };
        m_AttackParryBeatVec = { true, true, false, true};
        break;
    case AnimType::NonParrySevenAttack:
        m_AttackBeatVec = { 1, 2, 0.5f, 2.f, 0.5f, 2, 0.5f };
        m_AttackParryBeatVec = { false, true, true, true, true, true, true };
        break;
    case AnimType::NonParryTwoAttack:
        m_AttackBeatVec = { 1, 2 };
        m_AttackParryBeatVec = { true, false };
        bShort = true;
        break;
    case AnimType::NonParryThreeAttack:
        m_AttackBeatVec = { 1, 1, 1 };
        m_AttackParryBeatVec = { false, true, true };
        bShort = true;
        break;
    case AnimType::NonParryFiveAttackThree:
        m_AttackBeatVec = { 1, 2.5f, 2, 2, 0.5f };
        m_AttackParryBeatVec = { true, false, false, true, true };
        break;
    case AnimType::NonParrySevenAttackTwo:
        m_AttackBeatVec = { 1, 2, 1.5f, 1, 1, 1, 1 };
        m_AttackParryBeatVec = { true, false, true, true, true, true, true };
        break;
    default:
        assert(false);
        break;
    }
    Create_AttackUi((_uint)m_AttackBeatVec.size(), m_AttackBeatVec, m_AttackParryBeatVec);

    for (size_t i = 1; i < m_AttackBeatVec.size(); ++i)
    {
        m_AttackBeatVec[i] += m_AttackBeatVec[i - 1];
    }
    m_pChai->Start_AttackSequence(bShort, m_iAttackStartBeat, m_AttackBeatVec, m_AttackParryBeatVec);
}

void CMonster_Korsica::Initialize_Camera()
{
    CCamera::CAMERA_DESC CameraDesc{};
    CameraDesc.vEye = _float4(0.f, 4.f, -10, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    CameraDesc.fFovy = XMConvertToRadians(40.0f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 100.f;
    CameraDesc.fSpeedPerSec = 30.f;
    CameraDesc.strTag = L"CKorsicaCamera";
    CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    auto GameInstance = m_pGameInstance.lock();
    m_pCamera = static_pointer_cast<CKorsicaCamera>
        (GameInstance->Add_Clone_Return(LEVEL_STATIC, L"CKorsicaCamera", CKorsicaCamera::ObjID, &CameraDesc));
    GameInstance->Add_Camera(L"CKorsicaCamera", m_pCamera);
    GameInstance->Set_MainCamera(L"CKorsicaCamera");
    m_pCamera->Start_Following_Bone(m_pRealTransform->Get_WorldMatrix_Ptr(), const_cast<_float4x4*>(m_pModelCom->Get_BoneMatrixPtr(m_pModelCom->Get_BoneIdx("prop_02"))));
}

void CMonster_Korsica::Initialize_EventChai()
{
    m_pChai = static_pointer_cast<CEvent_Chai>(GAMEINSTANCE->Add_Clone_Return
    (m_iLevelIndex, TEXT("Layer_Chai"), CEvent_Chai::ObjID, nullptr));

    _float3 Pos = m_Middle;
    Pos.z += m_Radius;
    m_pChai->Set_Player_Position(XMLoadFloat3(&Pos));
    m_pChai->Set_Anim(CEvent_Chai::AnimType::Idle, true);
    m_pChai->Set_Middle(m_Middle);
    m_pChai->Set_Radius(m_Radius);
}

void CMonster_Korsica::Adjust_LookAngle(_float fTimeDelta)
{
    if (m_pModelCom->Get_CurrentAnimIndex() == _uint(AnimType::Idle)
        || m_pModelCom->Get_CurrentAnimIndex() == _uint(AnimType::IdletoMove)
        || m_pModelCom->Get_CurrentAnimIndex() == _uint(AnimType::Move)
        || m_pModelCom->Get_CurrentAnimIndex() == _uint(AnimType::MovetoAttackReady)
        || m_pModelCom->Get_CurrentAnimIndex() == _uint(AnimType::AttackReadytoIdle))
    {
        _float3 Pos = { 0, 0, 0 };
        XMStoreFloat3(&Pos, m_pTransformCom->Get_Position());
        _float CorsicaAngle = atan2(Pos.z - m_Middle.z, Pos.x - m_Middle.x);
        _float angleDifference = 180.0f;
        _float ChaiAngle = CorsicaAngle + XMConvertToRadians(angleDifference);
        m_pChai->Set_Angle(ChaiAngle);
        m_pTransformCom->LookAt_Horizontal(XMLoadFloat3(&m_Middle));
    }
}

void CMonster_Korsica::AdjustCurAnim_ChaiPos()
{
    switch ((AnimType)m_pModelCom->Get_CurrentAnimIndex())
    {
    case AnimType::Idle:
        Set_ChaiPos();
        break;
    case AnimType::IdletoMove:
        Set_ChaiPos();
        break;
    case AnimType::Move:
        Set_ChaiPos();
        break;
    case AnimType::AttackReadytoIdle:
        Set_ChaiPos();
        break;
    case AnimType::AttackReadytoMove:
        Set_ChaiPos();
        break;
    default:
        break;
    }
}

void CMonster_Korsica::Progress_Anim(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
    if (m_pModelCom->Get_CurrentAnimIndex() != _uint(AnimType::DeadFly)
        && m_pModelCom->Get_CurrentAnimIndex() != _uint(AnimType::PerfectCatch)
        && m_pModelCom->Get_CurrentAnimIndex() != _uint(AnimType::GoodCatch)
        && m_pModelCom->Get_CurrentAnimIndex() != _uint(AnimType::MissCatch))
    {
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    }
    else
    {
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult, m_iDeadBone);
    }
 
    string strAnimTag = m_pModelCom->Get_AnimString(m_pModelCom->Get_CurrentAnimIndex());
    if (strAnimTag == "em4000_event-korsica_220" ||
        strAnimTag == "em4000_event-korsica_610" ||
        strAnimTag == "em4000_event-korsica_621" ||
        strAnimTag == "em4000_event-korsica_630" ||
        strAnimTag == "em4000_event-korsica_631")
    {
        XMStoreFloat4x4(&m_MatOffset, XMMatrixTranslation(0.f, 0.f, 4.5f));
    }
    else
    {
        m_MatOffset = Identity;
    }

    if(m_bIdle)
    {
        _matrix matResult = XMLoadFloat4x4(&fmatResult);
        _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
        _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
        auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
        auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
        m_pTransformCom->Set_Position(vCurPos);
    }
    //else if (strAnimTag == "em4000_event-korsica_650")
    //{
    //    _matrix matResult = XMLoadFloat4x4(&fmatResult);
    //    _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
    //    _matrix matAnimRot = matResult * 2.f * XMLoadFloat4x4(&matTemp);
    //    auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    //    auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
    //    m_pTransformCom->Set_Position(vCurPos);
    //}
}

void CMonster_Korsica::Progress_Dead(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::DeadFly))
    {
        m_pCamera->Set_Follow(true);
        m_bMissed = !m_pChai->IsCatch();
        Set_Anim(AnimType::PerfectCatch);

        _matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
        //World.r[3] -= m_pTransformCom->Get_NormLook() * 2.f;
        m_pChai->Get_Transform()->Set_WorldMatrix(World);
        m_pChai->Set_Player_Position_Immediate(World.r[3]);
       
        m_isTalkOver = false;

        auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
        if (pTalkSystem == nullptr)
        {
            CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
            TalkDesc.endCallback = [&]() {
                m_isTalkOver = true;
                };

            pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_KORSICA, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
        }

        auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
        if (PRequiredTalkSystem != nullptr)
        {
            PRequiredTalkSystem->LoadTalkFile(L"VsKorsica Talk_File_10.csv");

            m_IsStageResultCall = true;
            CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
        }
    }
    else if (AnimBeyond(AnimType::MissCatch))
    {
        _uint i = 0;
    }
}

void CMonster_Korsica::Progress_Test(_float fTimeDelta)
{
    if (AnimBeyond(AnimType::MovetoAttackReady))
    {
        Set_Pattern(PatternType::FinalAttack, AnimType::NonParryFiveAttackThree);
        Set_Ui(AnimType::NonParryFiveAttackThree);
    }
    else if (AnimBeyond(AnimType::NonParryFiveAttackThree, 0.85f))
    {
        _float4 Pos = {};
        XMStoreFloat4(&Pos, m_pTransformCom->Get_Position());
        Set_Pattern(PatternType::Dead);
        m_fCurHp = 0;
    }

    if (AnimBeyond(AnimType::NonParryFiveAttackThree, 0.4f))
    {
        CUi_Mgr::Get_Instance()->Ui_Dead(CUi_EnemySpecialAttack::ObjID);
    }
}

void CMonster_Korsica::Progress_Weapon_Anim(_float fTimeDelta)
{
    if(m_pModelCom->Get_AnimName() == "em4000_event-korsica_110")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_110"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_115");
    }
    else if(m_pModelCom->Get_AnimName() == "em4000_event-korsica_210")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_210"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_215");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_220")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_220"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_225");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_310")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_310"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_315");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_410")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_410"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_415");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_420")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_420"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_425");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_430")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_430"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_435");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_431")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_431"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_436");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_610")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_610"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_615");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_620")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_620"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_625");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_621")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_621"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_626");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_622")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_622"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_627");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_630")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_630"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_635");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_631")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_631"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_636");
    }
    else if (m_pModelCom->Get_AnimName() == "em4000_event-korsica_632")
    {
        if (static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Right)->Change_Animation("wp4000_event-korsica_632"))
            static_pointer_cast<CWeapon_Korsica>(m_pWeapon_Left)->Change_Animation("wp4000_event-korsica_637");
    }
}

void CMonster_Korsica::Judge_Attack(_float fTimeDelta)
{
    _uint CurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
    _float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float BeatPercent = Timer / Beat;
    _int JudgeBeat = 0;
    if(m_pModelCom->Get_CurrentAnimIndex() != (_uint)AnimType::FourAttackTwo
        && m_pModelCom->Get_CurrentAnimIndex() != (_uint)AnimType::NonParryThreeAttack
        && m_pModelCom->Get_CurrentAnimIndex() != (_uint)AnimType::NonParryTwoAttack)
    { 
        JudgeBeat = CurBeat - (m_iAttackStartBeat + 9);
    }
    else
    {
        JudgeBeat = CurBeat - (m_iAttackStartBeat + 4);
    }

    if (m_AttackBeatVec.size() > 0)
    {
        if (m_AttackBeatVec[0] <= JudgeBeat + BeatPercent)
        {
            if (m_AttackParryBeatVec[0])
            {
                //if (m_pChai->Get_CurAnim() != CEvent_Chai::AnimType::Parryingone
                //    && m_pChai->Get_CurAnim() != CEvent_Chai::AnimType::Parryingtwo)
                if(m_pChai->Get_ReactTimer() < 0.001f)
                {
                    m_pChai->Set_DamagedAnim(true);
                }
                else
                {
                    m_pChai->Set_Anim(CEvent_Chai::AnimType::Parryingone);
                    m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Parry_GT");
                    m_fCurHp -= 2.5f;
                }

                m_pCamera->Shake(0.1f, 0.1f);
            }
            else
            {
                //if (m_pChai->Get_CurAnim() != CEvent_Chai::AnimType::Evade)
                if (m_pChai->Get_ReactTimer() < 0.001f)
                {
                    m_pChai->Set_DamagedAnim(false);
                }
                else
                {
                    m_pChai->Set_Anim(CEvent_Chai::AnimType::Evade);
                }

            }
#ifdef _DEBUG
            cout << "Attack" << endl;
#endif
            m_pChai->Set_ReactTimer(0.f);
			m_AttackBeatVec.erase(m_AttackBeatVec.begin());
			m_AttackParryBeatVec.erase(m_AttackParryBeatVec.begin());
		}
	}
}

void CMonster_Korsica::Testfuction(_float fTimeDelta)
{
    if (m_isTalkOver && m_pGameInstance.lock()->Get_KeyDown(DIK_U))
    {
        Set_Chat(false);
        Set_Anim(AnimType::Move, true);
    }

    if (m_pGameInstance.lock()->Get_KeyDown(DIK_Z) && m_IsStageResultCall)
    {
        CUi_StageResultSystem::Desc Desc;
        Desc.iNextLevel = LEVEL_MIMOSA;
        CUi_Mgr::Get_Instance()->Add_Clone(CUi_StageResultSystem::ObjID, m_iLevelIndex,
            L"CUi", CUi_StageResultSystem::ObjID, &Desc);
    }
}

_bool CMonster_Korsica::AnimBeyond(AnimType eType, _float Percent)
{
    if (m_pModelCom->Get_CurrentAnimIndex() == (_uint)eType)
    {
        if (Percent == 1)
        {
            if (m_pModelCom->Get_Finished())
                return true;
        }
        else
        {
            if (m_pModelCom->Get_Duration() * Percent <= m_pModelCom->Get_Current_Track_Position())
            {
                return true;
            }
        }
    }
    return false;
}

void CMonster_Korsica::Set_CameraInterPole(AnimType eType)
{
    //저장
    _uint CurType = m_pModelCom->Get_CurrentAnimIndex();
    _float CurTrackPos = m_pModelCom->Get_Current_Track_Position();
    _float4x4 CurPos = m_pTransformCom->Get_WorldMatrix();


    //세팅 
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
    m_pModelCom->Set_AnimIndex(_uint(eType), false);
    m_pModelCom->Play_Animation(0.01f, 0.01f, 0.1f, fmatResult);

    //넘기기
    m_pCamera->Set_NextBoneFollow(true, *(m_pModelCom->Get_BoneMatrixPtr(m_iCameraBone)));

    //복구
    m_pModelCom->Set_AnimIndex(CurType, false);
    m_pModelCom->Set_CurrentTrackPosition(CurTrackPos);
    m_pTransformCom->Set_WorldMatrix(CurPos);
}

void CMonster_Korsica::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Model_Monster_Korsica"), TEXT("Com_Model")));
    m_pRealTransform = static_pointer_cast<CTransform>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_RealTransform"))); 
    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
}

void CMonster_Korsica::PrepareCallbacks()
{
    vector<shared_ptr<CAnimation>> Vec = m_pModelCom->Get_Animation_List();
}

void CMonster_Korsica::Initialize_Weapon()
{
    CWeapon_Korsica::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
    WeaponDesc.pParentTransform = m_pRealTransform;
    /* 추가된 파트오브젝트가 부모를 기준으로 회전하거나 이동할 수 있다. */
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;

    shared_ptr <CPartObject> pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CWeapon_Korsica"), &WeaponDesc));
    if (nullptr == pWeapon)
        assert(false);

    m_pWeapon_Right = (pWeapon);

    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("l_attach_hand_00");

    pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CWeapon_Korsica"), &WeaponDesc));
    if (nullptr == pWeapon)
        assert(false);

    m_pWeapon_Left = (pWeapon);

    shared_ptr<CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon_Left->Find_Component(TEXT("Com_Model")));
    Weapon_Model->Set_AnimIndex("ch0000_event-korsica_000", false);
    _float4x4 Mat;
    Weapon_Model->Play_Animation(10.f, 10.f, 0.f, Mat);
    Weapon_Model = static_pointer_cast<CModel>(m_pWeapon_Right->Find_Component(TEXT("Com_Model")));
    Weapon_Model->Set_AnimIndex("ch0000_event-korsica_000", false);
    Weapon_Model->Play_Animation(10.f, 10.f, 0.f, Mat);

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
}

void CMonster_Korsica::Create_AttackUi(_uint iParryCount, vector<_float>& fBeatGap, vector<_bool>& bParry, _float2 fDistance)
{
    _float Percent = CBeat_Manager::Get_Instance()->Get_BeatRate();
    CUi_EnemySpecialAttack::Desc Desc;
    Desc.iParryCount = iParryCount;
    Desc.bParry = bParry;
    Desc.fBeatGap = fBeatGap;
    Desc.fDistance = fDistance;
    Desc.TargetTransform = m_pTransformCom;
    if (m_pModelCom->Get_CurrentAnimIndex() != (_uint)AnimType::FourAttackTwo
        && m_pModelCom->Get_CurrentAnimIndex() != (_uint)AnimType::NonParryThreeAttack
        && m_pModelCom->Get_CurrentAnimIndex() != (_uint)AnimType::NonParryTwoAttack)
    {
        Desc.RealAttackGapBeat = 9 + Percent;
    }
    else
    {
        Desc.RealAttackGapBeat = 4 + Percent;
    }
    CUi_Mgr::Get_Instance()->Add_Clone(CUi_EnemySpecialAttack::ObjID, 
        m_iLevelIndex, L"CUi", CUi_EnemySpecialAttack::ObjID, &Desc);
}

void CMonster_Korsica::Adjust_KorsicaPos(_float fTimeDelta)
{
    XMVECTOR center = XMVectorSet(m_Middle.x, m_Middle.y, m_Middle.z, 0.0f);
    XMVECTOR characterPos = m_pTransformCom->Get_Position(); 
    XMVECTOR direction = XMVectorSubtract(characterPos, center);
    _float distance = XMVectorGetX(XMVector3Length(direction));
    if (abs(distance - m_Radius) > 0.2f)
    {
        XMVECTOR newDirection = XMVector3Normalize(direction) * m_Radius;
        XMVECTOR newPosition = XMVectorAdd(center, newDirection);
        //XMStoreFloat4(&m_vTargetPosition, newPosition);
        m_pTransformCom->Set_Position(newPosition);
        m_pTransformCom->LookAt_Horizontal(XMLoadFloat3(&m_Middle));
        Set_ChaiPos();
    }

}

void CMonster_Korsica::Adjust_KorsicaPos_From_Chai(_float fTimeDelta)
{
    _vector center = XMVectorSet(m_Middle.x, m_Middle.y, m_Middle.z, 0.0f);
    _vector vChaiPos = m_pChai->Get_Transform()->Get_Position();
    _vector vNewPos = center + XMVector3Normalize(center - vChaiPos) * m_Radius;
    m_pTransformCom->Set_Position(vNewPos);
    m_pTransformCom->LookAt_Horizontal(XMLoadFloat3(&m_Middle));
    Set_ChaiPos();
}

shared_ptr<CMonster_Korsica> CMonster_Korsica::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMonster_Korsica
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster_Korsica(pDevice, pContext) {}
    };

    shared_ptr<CMonster_Korsica> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Korsica"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMonster_Korsica::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMonster_Korsica
    {
        MakeSharedEnabler(const CMonster_Korsica& rhs) : CMonster_Korsica(rhs) {}
    };

    shared_ptr<CMonster_Korsica> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Korsica"));
        assert(false);
    }

    return pInstance;
}

void CMonster_Korsica::Free()
{
}
