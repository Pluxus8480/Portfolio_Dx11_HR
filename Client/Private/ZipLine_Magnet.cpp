#include "ZipLine_Magnet.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "MapManager.h"
#include "Chai.h"
#include "PlayerManager.h"
#include "Player_Camera.h"
#include "Beat_Manager.h"
#include "CharacterController.h"
#include "GameManager.h"
#include "CUi_Mgr.h"
#include "CUi_MapInterAction.h"

wstring CZipLine_Magnet::ObjID = TEXT("CZipLine_Magnet");
CZipLine_Magnet::CZipLine_Magnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMagnet{pDevice, pContext}
{
}

CZipLine_Magnet::CZipLine_Magnet(const CZipLine_Magnet& rhs)
    : CMagnet{rhs}
{
}

CZipLine_Magnet::~CZipLine_Magnet()
{
    Free();
}

const _float4x4* CZipLine_Magnet::Get_PlayerAttachMatrix()
{
    return m_pModelCom->Get_BoneMatrixPtr("body_attach_00");
}

void CZipLine_Magnet::Set_AnimState(ANIMSTATE eState)
{
    m_eAnimState = eState;

    if (eState == ANIMSTATE::GO_FORWARD)
        m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, true);
    else
        m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, false);
}

void CZipLine_Magnet::Enter_Event()
{
    m_bEnterEvent = true;

    m_eAnimState = ANIMSTATE::EVENT_ENTER;
    m_pModelCom->Set_AnimIndex("bg0540_event-magnetrail_150", false);
}

void CZipLine_Magnet::Exit_Event()
{
    m_bExitEvent = true; 
    m_eAnimState = ANIMSTATE::EVENT_EXIT;
    m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, false);
    PLAYERMANAGER->Callback_Event("Zipline_End");
    m_bPlayer_Attach_End = true;
    _float4x4 fMatAttach = m_pTransformCom->Get_WorldMatrix();

    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());
    _matrix mMat = XMLoadFloat4x4(&fMatAttach);
    //mMat.r[3] -= m_pTransformCom->Get_NormLook() * 1.2f;
    mMat.r[3] -= m_pTransformCom->Get_NormUp() * 3.5f;
    pPlayer->Set_Player_Position(mMat.r[3]);
    shared_ptr<CCharacterController> cpCCT = static_pointer_cast<CCharacterController>(pPlayer->Find_Component(L"Com_CCT"));
    cpCCT->Update(GAMEINSTANCE->Get_TimeDelta(L"Timer_60"));

    pPlayer->Get_Transform()->LookAt_Horizontal(pPlayer->Get_Transform()->Get_Position() + m_pTransformCom->Get_NormLook());
}

HRESULT CZipLine_Magnet::Initialize_Prototype()
{
    _float4x4 fMat = {};
    //XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ZipLineMagnet"), TEXT("../Bin/Resources/Models/Map/Gimmick/ZipLineMagnet.fbx"), MODEL_TYPE::ANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Green.png"))
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_WarningLight_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiWarninglight_E_Green.png"))
    );
    
    return S_OK;
}

HRESULT CZipLine_Magnet::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    pDesc->fSpeedPerSec = 1.f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Ready_Components();

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
    m_eAnimState = ANIMSTATE::IDLE;
    m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, true); // Idle

    _float4x4 mat;
    m_pModelCom->Play_Animation(0.016f, 0.016f, 0.1f, mat);

    CMapManager::Get_Instance()->Assign_Magnet(static_pointer_cast<CMagnet>(shared_from_this()));

    m_bExecuted = true;


    if (CGame_Manager::Get_Instance()->Get_Current_Level() != LEVEL_MAPTOOL &&
        CGame_Manager::Get_Instance()->Get_Current_Level() != LEVEL_GIMMICTEST)
    {
        Initialize_Ui();
    }


    return S_OK;
}

void CZipLine_Magnet::PriorityTick(_float fTimeDelta)
{
}

void CZipLine_Magnet::Tick(_float fTimeDelta)
{
    //if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_L))
    //    Enter_Event();

    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());
    if(pPlayer && !m_bEnterEvent && !m_bExitEvent)
    {
        _float4x4 fMat = *m_pModelCom->Get_BoneMatrixPtr("body_attach_00");
        _matrix mBoneMat = XMLoadFloat4x4(&fMat) * m_pTransformCom->Get_WorldMatrix_XMMat();
        _vector bonePos = (mBoneMat.r[3]);
        _float distance = XMVectorGetX(XMVector3Length(bonePos - PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position()));
        if (distance < 2.f && !m_bEnterEvent)
        {
            m_bEnterEvent = true;
            m_pUi->Set_Active(false);
            Enter_Event();
            PLAYERMANAGER->Callback_Event("Zipline");
        }
        //else if (distance < 20.f && !m_bEnterEvent)
        //    pPlayer->Get_Player_Camera()->Set_Wire_Target(shared_from_this());
        //else if (pPlayer->Get_Player_Camera()->Get_Wire_Target() == shared_from_this() && (distance >= 10.f || distance < 1.f))
        //    pPlayer->Get_Player_Camera()->Set_Wire_Target(nullptr);
    }
    if (pPlayer && !m_bSet_Wire_list)
    {
        m_bSet_Wire_list = true;
        pPlayer->Get_Player_Camera()->Set_Wire_Target(static_pointer_cast<CMagnet>(shared_from_this()));
    }

    if (m_bEnterEvent && !m_bExitEvent)
    {
        auto gameinstance = m_pGameInstance.lock();
        if (gameinstance->Get_KeyDown(DIK_A))
        {
            m_pModelCom->Set_AnimIndex("bg0540_event-magnetrail_040", false);
        }
        else if (gameinstance->Get_KeyDown(DIK_D))
        {
            m_pModelCom->Set_AnimIndex("bg0540_event-magnetrail_011", false);
        }
        else if (gameinstance->Get_KeyDown(DIK_SPACE))
        {
            m_pModelCom->Set_AnimIndex("bg0540_event-magnetrail_040", false);
        }

        switch (m_eAnimState)
        {
        case Client::CZipLine_Magnet::ANIMSTATE::GO_FORWARD:
			Spark_Control(fTimeDelta);
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::DODGE_LEFT:
            Spark_Control(fTimeDelta);
            if (m_pModelCom->Get_Finished()) {
                m_eAnimState = ANIMSTATE::GO_FORWARD;
                m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, true);
            }
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::DODGE_RIGHT:
            Spark_Control(fTimeDelta);
            if (m_pModelCom->Get_Finished()) {
                m_eAnimState = ANIMSTATE::GO_FORWARD;
                m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, true);
            }
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::DASH:
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::HIT:
            if (m_pModelCom->Get_Finished()) {
                m_eAnimState = ANIMSTATE::GO_FORWARD;
                m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, true);
            }
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::DODGE_TURNOVER:
            if (m_pModelCom->Get_Finished()) {
                m_eAnimState = ANIMSTATE::GO_FORWARD;
                m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, true);
            }
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::EVENT_EXIT:
            if (m_pModelCom->Get_Finished()) {
                m_eAnimState = ANIMSTATE::IDLE;
                m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, false);
                Done();
            }
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::EVENT_ENTER:
            if (m_pModelCom->Get_Finished()) {
                m_eAnimState = ANIMSTATE::GO_FORWARD;
                m_pModelCom->Set_AnimIndex((_uint)m_eAnimState, true);
            }
            break;
        case Client::CZipLine_Magnet::ANIMSTATE::IDLE:
            break;
        }

        //_float4x4 mat;
        //m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, mat);
        Animation_End(fTimeDelta);
        Animation(fTimeDelta);

        if(!m_bPlayer_Attach_End)
        {
            _float4x4 fMatAttach = m_pTransformCom->Get_WorldMatrix();


            _matrix mMat = XMLoadFloat4x4(&fMatAttach);
            mMat.r[3] += m_pTransformCom->Get_NormLook() * 0.4f;
            mMat.r[3] -= m_pTransformCom->Get_NormUp() * 4.f;
            pPlayer->Set_Player_Position(mMat.r[3]);
            shared_ptr<CCharacterController> cpCCT = static_pointer_cast<CCharacterController>(pPlayer->Find_Component(L"Com_CCT"));
            cpCCT->Update(fTimeDelta);

            pPlayer->Get_Transform()->LookAt(pPlayer->Get_Transform()->Get_Position() + m_pTransformCom->Get_NormLook());
        }
        
    }
}

void CZipLine_Magnet::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    //if(pGI->isIn_Frustum_WorldSpace())
    if(m_bRender)
        pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CZipLine_Magnet::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        //    return E_FAIL;
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        _bool bEmissive = { false };
        if (i == 1)
        {
            if (FAILED(m_pEmissionTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
                return E_FAIL;
            if (FAILED(m_pEmissionTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
                return E_FAIL;
            bEmissive = true;
        }
        else if (i == 2)
        {
            if (FAILED(m_pEmissionTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
                return E_FAIL;
            if (FAILED(m_pEmissionTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
                return E_FAIL;
            bEmissive = true;
        }
        else
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
                return E_FAIL;
            if (SUCCEEDED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
                bEmissive = true;
        }
        
        if(bEmissive)
            m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
        else
            m_pShaderCom->Begin((_uint)MESH_PASS::Default);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CZipLine_Magnet::Animation(_float fTimeDelta)
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
    /* Magnetrail */
    if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_150"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Five_Beat, 0.1f, fmatResult);

    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_020"))
        m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Two_Beat, 0.1f, fmatResult);

    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_000") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_040"))
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Eight_Beat, 0.1f, fmatResult);

    else if (m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_010") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_011"))
            m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Six_Beat, 0.1f, fmatResult);

}

void CZipLine_Magnet::Animation_End(_float fTimeDelta)
{
    if ((m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_150") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_010")||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_011") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_020") ||
        m_pModelCom->Get_CurrentAnimIndex() == m_pModelCom->Get_StringAnimIndex("bg0540_event-magnetrail_040"))
        && m_pModelCom->Get_Finished())
        m_pModelCom->Set_AnimIndex("bg0540_event-magnetrail_000", true);
}

void CZipLine_Magnet::Spark_Control(_float fTimeDelta)
{
	m_fSpark_Delay += fTimeDelta;
	if (m_fSpark_Delay >= m_fSpark_DelayMax)
	{
        m_fSpark_Delay -= m_fSpark_DelayMax;
		Zipline_Spark(fTimeDelta);
	}
}

void CZipLine_Magnet::Zipline_Spark(_float fTimeDelta)
{
	EFFECTMANAGER->Call_Union_Effect("Magnet_Spark", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f);
}

void CZipLine_Magnet::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ZipLineMagnet"), TEXT("Model_Com")));

    m_pEmissionTexture[0] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WarningLight_Green"), TEXT("Texture0_Com")));
    m_pEmissionTexture[1] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("Texture1_Com")));

    if (!m_pShaderCom || !m_pModelCom || !m_pEmissionTexture[0] || !m_pEmissionTexture[1])
        assert(false);
}

HRESULT CZipLine_Magnet::Bind_ShaderResources()
{
    auto pGI = m_pGameInstance.lock();

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    	return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CZipLine_Magnet> CZipLine_Magnet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CZipLine_Magnet
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CZipLine_Magnet(pDevice, pContext)
        {}
    };


    shared_ptr<CZipLine_Magnet> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CZipLine_Magnet"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CZipLine_Magnet::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CZipLine_Magnet
    {
        MakeSharedEnabler(const CZipLine_Magnet& rhs) : CZipLine_Magnet(rhs)
        {}
    };

    shared_ptr<CZipLine_Magnet> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CZipLine_Magnet"));
        assert(false);
    }

    return pInstance;
}

void CZipLine_Magnet::Free()
{
}
