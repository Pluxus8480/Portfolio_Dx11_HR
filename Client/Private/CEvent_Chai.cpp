#include "CEvent_Chai.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Shader.h"
#include "Model.h"
#include "Basic_Guitar.h"
#include "iostream"
#include "Effect_Manager.h"
#include "Beat_Manager.h"
#include "CUi_CommandIcon.h"
#include "Cine_Manager.h"

wstring CEvent_Chai::ObjID = L"CEvent_Chai";

CEvent_Chai::CEvent_Chai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CEvent_Chai::CEvent_Chai(const CEvent_Chai& rhs)
	:CGameObject(rhs)
{
}

CEvent_Chai::~CEvent_Chai()
{
}

HRESULT CEvent_Chai::Initialize_Prototype()
{
    m_List_Owning_ObjectID.push_back(CBasic_Guitar::ObjID);

    if (FAILED(__super::Initialize_Prototype()))
        assert(false);

    _matrix		LocalTransformMatrix = XMMatrixIdentity();
    LocalTransformMatrix = XMMatrixScaling(0.0045f, 0.0045f, 0.0045f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
    _float4x4 preMatrix = {};
    XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_EventChai", L"../Bin/Resources/Models/Monsters/EventChai/Chai_Korsica_Event.fbx", MODEL_TYPE::ANIM, preMatrix)
    );

	return S_OK;
}

HRESULT CEvent_Chai::Initialize(void* pArg)
{
    ReadyComponent();
    m_pModelCom->Set_AnimIndex(2, true);
    shared_ptr< CModel> Weapon_Model = static_pointer_cast<CModel>(m_pWeapon->Find_Component(TEXT("Com_Model")));
    Weapon_Model->Set_AnimIndex("wp0000_atk-guitar_241", false);

    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
        }
    );

	return S_OK;
}

void CEvent_Chai::PriorityTick(_float fTimeDelta)
{
    m_bIsInGame = CCine_Manager::Get_Instance()->IsInGame();
    if (!m_bIsInGame)
        return;
    m_pTransformCom->LookAt_Horizontal(XMLoadFloat3(&m_Middle));
	m_pWeapon->PriorityTick(fTimeDelta);
}

void CEvent_Chai::Tick(_float fTimeDelta)
{
    if (!m_bIsInGame)
        return;
    Return_toIdle();
    m_fCoolDown -= fTimeDelta;
    if (m_fReactTimer >= 0.f)
    {
        m_fReactTimer -= fTimeDelta;
    }
    Key_Progress();
    if(m_bFollow)
    {
        _vector vPos = m_pTransformCom->Get_Position();
        _vector vTargetPos = XMLoadFloat4(&m_vTargetPosition);
        _vector vDir = vTargetPos - vPos;
        if (XMVectorGetX(XMVector3Length(vDir)) < 0.05f)
        {
            m_pTransformCom->Set_Position(vTargetPos);
            m_bFollow = false;
        }
        else
        {
            m_pTransformCom->Set_Position(vPos + vDir * fTimeDelta * 8.f);
        }
    }
	m_pWeapon->Tick(fTimeDelta);

    // Call all reserved effects
    m_pModelCom->PlayReservedCallbacks();
}

void CEvent_Chai::LateTick(_float fTimeDelta)
{
    if (!m_bIsInGame)
        return;
    Progress_Anim(fTimeDelta);

    //if (m_bLerp)
    //{
    //    Lerp_Pos(fTimeDelta);
    //}
	m_pWeapon->LateTick(fTimeDelta);
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#ifndef _DEBUG
#ifdef PERFECTION_LOAD
    m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
#endif // !_DEBUG
    Calc_ViewDepth();
}

HRESULT CEvent_Chai::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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
    _float fOutlineWidth = 0.8f;
    if (fViewDepth <= 10.f)
    {
        fOutlineWidth = 0.4f;
    }

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        {
            //if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            //    return E_FAIL;
            //m_pShaderCom->Bind_Float("g_OutlineWidth", 0.01f);
            //m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);


            //m_pModelCom->Render((_uint)i);
            continue;
        }
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
        //    return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //    return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        if (i == 0)        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::NormalMap);
        else               m_pShaderCom->Begin(0);

        m_pModelCom->Render((_uint)i);

        if (i == 0)
            continue;
        
        m_pShaderCom->Bind_Float("g_OutlineWidth", fOutlineWidth);
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CEvent_Chai::Render_Shadow()
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

void CEvent_Chai::TakeDamage(const DamageInfo& damageInfo)
{
}

void CEvent_Chai::Set_DamagedAnim(_bool CanParryHeat)
{
    if (CanParryHeat)
    {
        Set_Anim(AnimType::Heat, false);
    }
    else
    {
        Set_Anim(AnimType::Nonparryheat, false);
    }
    m_fHp -= 5.f;
    m_fCoolDown = 0.1f * CBeat_Manager::Get_Instance()->Get_Beat();
}

void CEvent_Chai::Add_Hp(_float Hp)
{
    m_fHp += Hp;
}

void CEvent_Chai::Set_Player_Position(_vector vPosition)
{
    m_bFollow = true;
    XMStoreFloat4(&m_vTargetPosition, vPosition);
    //m_pTransformCom->Set_Position(vPosition);
}

void CEvent_Chai::Set_Player_Position_Immediate(_vector vPosition)
{
    m_bFollow = false;
    XMStoreFloat4(&m_vTargetPosition, vPosition);
    m_pTransformCom->Set_Position(vPosition);
}

void CEvent_Chai::Set_Anim(AnimType Type, _bool Loop)
{
    m_pModelCom->Set_AnimIndex(_uint(Type), Loop);
}

void CEvent_Chai::Set_DelayTime()
{
}

void CEvent_Chai::Progress_Anim(_float fTimeDelta)
{
    _float4x4 fmatResult;
    XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

    //if (m_pModelCom->Get_CurrentAnimIndex() != (_uint)AnimType::Move)
    //{
    //    m_pModelCom->Play_Animation(fTimeDelta * 1.5f, fTimeDelta * 1.5f, 0.1f, fmatResult);
    //}
    //else
    //{
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fmatResult);
    //}
    if (m_pModelCom->IsRootAnim() && !m_pModelCom->IsForcedNonRoot())
    {
        _matrix matResult = XMLoadFloat4x4(&fmatResult);
        auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);

        _float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
        _matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);
        m_pTransformCom->LookAt_Horizontal(m_pTransformCom->Get_Position() + XMVector3Normalize(matAnimRot.r[2]));

        auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);
        m_pTransformCom->Set_Position(vCurPos);
    }
    switch ((AnimType)m_pModelCom->Get_CurrentAnimIndex())
    {
    case AnimType::Catchrun:
        if (!m_bUI)
        {
            m_bUI = true;
            CUi_CommandIcon::Desc Desc;
            Desc.CorrectBeat = 3;
            Desc.fX = 1;
            Desc.fY = 0;
            Desc.eRenderType = CUi::RenderType::Priority;
            m_pCommandIcon = static_pointer_cast<CUi_CommandIcon>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_CommandIcon::ObjID, GAMEINSTANCE->Get_Current_LevelID(), L"CUi", CUi_CommandIcon::ObjID, &Desc));
        }
        else if(m_pCommandIcon)
        {
            if (m_pGameInstance.lock()->Get_MouseDown(MOUSEKEYSTATE::DIMK_LBUTTON))
            {
                auto Result = m_pCommandIcon->Cal_Judge();
                if (Result != CUi_JudgeWord::WordType::Miss)
                {
                    m_bCatch = true;
                }
                m_pCommandIcon = nullptr;
            }
        }
        break;
    default:
        break;
    }
    //_matrix matResult = XMLoadFloat4x4(&fmatResult);

    //_float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
     
    //auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
    //auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);


}

void CEvent_Chai::Return_toIdle()
{
    if (m_pModelCom->Get_Finished())
    {
        switch ((AnimType)m_pModelCom->Get_CurrentAnimIndex())
        {
        case AnimType::Parryingone:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Idle), true);
            break;
        case AnimType::Parryingtwo:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Idle), true);
            break;
        case AnimType::Idle:
            break;
        case AnimType::Idletomove:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Move), true);
            break;
        case AnimType::Move:
            break;
        case AnimType::Movetoparryready:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Idle), true);
            break;
        case AnimType::Evade:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Idle), true);
            break;
        case AnimType::Evade_Fail:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Idle), true);
            break;
        case AnimType::Heat:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Idle), true);
            break;
        case AnimType::Nonparryheat:
            m_pModelCom->Set_AnimIndex(_uint(AnimType::Idle), true);
            break;
        case AnimType::Catchrun:
            if(m_bCatch)
                Set_Anim(AnimType::Catch, false);
            else
                Set_Anim(AnimType::CatchMiss, false);
            break;
        case AnimType::MoreRun:
            if (m_bCatch)
                Set_Anim(AnimType::Catch, false);
            else
                Set_Anim(AnimType::CatchMiss, false);
            break;
        case AnimType::Catch:
            break;
        case AnimType::CatchMiss:
            break;
        default:
            assert(false);
            break;
        }
    }

}

void CEvent_Chai::Key_Progress()
{
    _int CurBeat = (_int)CBeat_Manager::Get_Instance()->Get_BeatCount();
    _float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
    _float BeatPercent = Timer / Beat;
    _float JudgeBeat = 0.f;
    if (!m_bShortAttack)
    {
        JudgeBeat = CurBeat - (m_iAttackReadyStartBeat + 10);
    }
    else
    {
        JudgeBeat = CurBeat - (m_iAttackReadyStartBeat + 5);
    }

    if (m_vecBattleTiming.size() > 0)
    {
        //while (m_vecBattleTiming.size() && m_vecBattleTiming[0] <= JudgeBeat + BeatPercent)
        //{
        //    m_vecBattleTiming.erase(m_vecBattleTiming.begin());
        //    m_vecParry.erase(m_vecParry.begin());
        //}
        //if (!m_vecBattleTiming.size())
        //    return;

        if (m_bValid)
        {
            if (m_vecParry[0])
            {
                if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
                {
                  /*_uint i = m_pGameInstance.lock()->Random_Int(0, 1);
                    switch (i)
                    {
                    case 0:
                        Set_Anim(AnimType::Parryingone);
                        break;
                    case 1:
                        Set_Anim(AnimType::Parryingtwo);
                        break;
                    default:
                        break;
                    }*/
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
        //else
        //{
        //    if (m_pGameInstance.lock()->Get_KeyDown(DIK_E))
        //    {
        //        m_bValid = false;
        //    }
        //    if (m_pGameInstance.lock()->Get_KeyDown(DIK_LSHIFT))
        //    {
        //        Set_Anim(AnimType::Evade_Fail);
        //        m_bValid = false;
        //    }
        //}
        if (m_vecBattleTiming[0] <= JudgeBeat + BeatPercent)
        {
            m_vecBattleTiming.erase(m_vecBattleTiming.begin());
            m_vecParry.erase(m_vecParry.begin());
            m_bValid = true;
        }
    }
}

void CEvent_Chai::Lerp_Pos(_float fTimeDelta)
{
    if (m_pModelCom->Get_CurrentAnimIndex() == (_uint)AnimType::Idletomove)
    {
        _float PosX = m_Middle.x + m_fRadius * cos(m_fAngle);
        _float PosZ = m_Middle.z + m_fRadius * sin(m_fAngle);
        _float3 Pos = { PosX, 0, PosZ };

        _float3 OriginPos = {};
        XMStoreFloat3(&OriginPos, m_pTransformCom->Get_Position());
        m_pTransformCom->Set_Position(XMLoadFloat3(&Pos));
    }
}


CEvent_Chai::AnimType CEvent_Chai::Get_CurAnim()
{
    return CEvent_Chai::AnimType(m_pModelCom->Get_CurrentAnimIndex());
}

void CEvent_Chai::Start_AttackSequence(_bool bShort, _int iAttackReadyStartBeat, const vector<_float>& vecTimer, const vector<_bool>& vecParry)
{
    m_bShortAttack = bShort;
    m_vecBattleTiming = vecTimer;
    m_vecParry = vecParry;
    m_iAttackReadyStartBeat = iAttackReadyStartBeat;
    m_bValid = true;
}

void CEvent_Chai::Dying()
{
}

void CEvent_Chai::ReadyComponent()
{
    CTransform::TRANSFORM_DESC         TransformDesc{};
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        assert(false);

    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EventChai"), TEXT("Com_Model")));

    CBasic_Guitar::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("r_attach_hand_00");
    WeaponDesc.pParentTransform = m_pTransformCom;
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;

    m_pWeapon = static_pointer_cast<CPartObject>(GAMEINSTANCE->Clone_Object(TEXT("CBasic_Guitar"), &WeaponDesc));
    if (nullptr == m_pWeapon)
        assert(false);
}

shared_ptr<CEvent_Chai> CEvent_Chai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CEvent_Chai
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEvent_Chai(pDevice, pContext)
        {}
    };

    shared_ptr<CEvent_Chai> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CEvent_Chai::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CEvent_Chai
    {
        MakeSharedEnabler(const CEvent_Chai& rhs) : CEvent_Chai(rhs)
        {}
    };

    shared_ptr<CEvent_Chai> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}