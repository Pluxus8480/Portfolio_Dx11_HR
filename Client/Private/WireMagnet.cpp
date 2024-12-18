#include "WireMagnet.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"

#include "MapManager.h"
#include "GameManager.h"
#include "Beat_Manager.h"
#include "CUi_MapInterAction.h"
#include "CUi_Mgr.h"


#include "Chai.h"
#include "PlayerManager.h"
#include "Player_Camera.h"
#include "Beat_Manager.h"
#include "CharacterController.h"
#include "Cine_Manager.h"

wstring CWireMagnet::ObjID = TEXT("CWireMagnet");
CWireMagnet::CWireMagnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMagnet{pDevice, pContext}
{
}

CWireMagnet::CWireMagnet(const CWireMagnet& rhs)
    : CMagnet{rhs}
{
}

CWireMagnet::~CWireMagnet()
{
    Free();
}

void CWireMagnet::Execute(_bool bExecute)
{
    if (m_bExecuted == bExecute) return;

    m_bExecuted = bExecute;

    if (m_bExecuted)
        m_pModelCom->Set_AnimIndex(1, false);
}

void CWireMagnet::Change_Pattern(_uint iPattern)
{
    if (m_iCurPattern == iPattern) return;

    m_iCurPattern = iPattern;

    if(CGame_Manager::Get_Instance()->Get_Current_Level() == LEVEL_MODELTEST)
    {
        if (m_iCurPattern == (_uint)REKKA_PATTERN::ELECTRIC)
            Execute(true);

        else
            Execute(false);
    }
}

HRESULT CWireMagnet::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_WireMagnet"), TEXT("../Bin/Resources/Models/Map/Gimmick/WireMagnet.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Green.png"))
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Red"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Red.png"))
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_WarningLight_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiWarninglight_E_Green.png"))
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_WarningLight_Red"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiWarninglight_E_Red.png"))
    );

    return S_OK;
}

HRESULT CWireMagnet::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimIndex(5, true);

    auto pMapManager = CMapManager::Get_Instance();
    //if (pMapManager->Get_NextLevel() == LEVEL_MODELTEST)
    pMapManager->Assign_BossObject(shared_from_this());

    pMapManager->Assign_Magnet(static_pointer_cast<CMagnet>(shared_from_this()));

    m_vecAnimSpeeds.resize(m_pModelCom->Get_NumAnimation());

    if (CGame_Manager::Get_Instance()->Get_Current_Level() != LEVEL_MAPTOOL &&
        CGame_Manager::Get_Instance()->Get_Current_Level() != LEVEL_GIMMICTEST)
    {
        Initialize_Ui();
    }

    CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

    return S_OK;
}

void CWireMagnet::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

        for (_uint i = 0; i < m_vecAnimSpeeds.size(); i++)
        {
            _float fPlayTime = m_pModelCom->Get_Animation_PlayTime(i);
            m_vecAnimSpeeds[i] = fPlayTime / fBeat;
        }

        m_bInit = true;
    }
}

void CWireMagnet::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
        Execute();
#endif

    shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());
    if (pPlayer && !m_bSet_Wire_list)
    {
        m_bSet_Wire_list = true;
        pPlayer->Get_Player_Camera()->Set_Wire_Target(static_pointer_cast<CMagnet>(shared_from_this()));
    }

    _float4x4 Mat{};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_vecAnimSpeeds[m_eAnimState], 0.1f, Mat);

    Update_AnimState();
}

void CWireMagnet::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f))
        return;

    if (!CCine_Manager::Get_Instance()->IsInGame())
        return;

    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CWireMagnet::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

        _bool bEmissive = { false };
        if (i == 0)
        {
            bEmissive = true;
            if (FAILED(m_pTextureCom[m_iSquareEmiIndex]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
                return E_FAIL;
        }
        else if (i == 3)
        {
            bEmissive = true;
            if (FAILED(m_pTextureCom[m_iSquareEmiIndex + 2]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
                bEmissive = false;
            else
                bEmissive = true;
        }

        if(bEmissive)
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);

        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CWireMagnet::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
}

void CWireMagnet::Edit_Attribute()
{
    ImGui::Checkbox("Wait?", &m_bWait);
    ImGui::InputInt("InDex", &m_iUiRenderIndex);
}

void CWireMagnet::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    write_typed_data(os, m_bWait);
    write_typed_data(os, m_iUiRenderIndex);
}

void CWireMagnet::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    read_typed_data(is, m_bWait);

    read_typed_data(is, m_iUiRenderIndex);

    if (!m_bWait)
        Execute();
}

void CWireMagnet::Change_Phase(_uint iPhase)
{
    m_iCurPhase = iPhase;
}

HRESULT CWireMagnet::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WireMagnet"), TEXT("Model_Com")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    m_pTextureCom[ON_SQ] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("Texture0_Com")));
    m_pTextureCom[OFF_SQ] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiSquare_Red"), TEXT("Texture1_Com")));
    m_pTextureCom[ON_WARN] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WarningLight_Green"), TEXT("Texture2_Com")));
    m_pTextureCom[OFF_WARN] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WarningLight_Red"), TEXT("Texture3_Com")));

    for (size_t i = 0; i < TEX_END; i++)
        assert(m_pTextureCom[i]);

    return S_OK;
}

HRESULT CWireMagnet::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

void CWireMagnet::Update_AnimState()
{
    switch (m_eAnimState)
    {
    case Client::CWireMagnet::EXCT_BARRIER: {
        if (m_pModelCom->Get_Finished())
        {
            m_eAnimState = IDLE_ON;
            m_pModelCom->Set_AnimIndex(IDLE_ON, true);
            m_iSquareEmiIndex = ON_SQ;
        }
        else
            m_iSquareEmiIndex = OFF_SQ;
        break;
    }
    case Client::CWireMagnet::EXCT_FULL: {
        if (m_pModelCom->Get_Finished())
        {
            m_eAnimState = IDLE_ON;
            m_pModelCom->Set_AnimIndex(IDLE_ON, true);
            m_iSquareEmiIndex = ON_SQ;
        }
        else
            m_iSquareEmiIndex = OFF_SQ;
        break;
    }
    case Client::CWireMagnet::EXCT_SHORT: {
        if (m_pModelCom->Get_Finished())
        {
            m_eAnimState = IDLE_ON;
            m_pModelCom->Set_AnimIndex(IDLE_ON, true);
            m_iSquareEmiIndex = ON_SQ;
        }
        else
            m_iSquareEmiIndex = OFF_SQ;
        break;
    case Client::CWireMagnet::IDLE_BARRIER:
        m_iSquareEmiIndex = OFF_SQ;
        break;
    case Client::CWireMagnet::IDLE_ON:
        m_iSquareEmiIndex = (m_bExecuted) ? ON_SQ : OFF_SQ;        
        break;
    case Client::CWireMagnet::IDLE_OFF:
        m_iSquareEmiIndex = OFF_SQ;
        break;
    }
    }
}

shared_ptr<CWireMagnet> CWireMagnet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CWireMagnet
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CWireMagnet(pDevice, pContext)
        {}
    };

    shared_ptr<CWireMagnet> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWireMagnet"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CWireMagnet::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CWireMagnet
    {
        MakeSharedEnabler(const CWireMagnet& rhs) : CWireMagnet(rhs)
        {}
    };

    shared_ptr<CWireMagnet> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CWireMagnet"));
        assert(false);
    }

    return pInstance;
}

void CWireMagnet::Free()
{
}