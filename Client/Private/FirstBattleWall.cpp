#include "FirstBattleWall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"

#include "Beat_Manager.h"
#include "CombatManager.h"
#include "Cine_Manager.h"
#include "GameManager.h"

wstring CFirstBattleWall::ObjID = TEXT("CFirstBattleWall");
CFirstBattleWall::CFirstBattleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{ pDevice, pContext }
{
}

CFirstBattleWall::CFirstBattleWall(const CFirstBattleWall& rhs)
    : CGimmickBase{ rhs }
{
}

CFirstBattleWall::~CFirstBattleWall()
{
    Free();
}

void CFirstBattleWall::Execute(_bool bExecute)
{
    if (m_bExhausted)
        return;
    m_bExecuted = true;
    m_bDone = bExecute;
    m_bAnimStart = true;
    if(m_bDone)
        m_pModelCom->Set_AnimIndexNonCancle(0, false);
    else
    {
        m_pModelCom->Set_AnimIndexNonCancle(1, false);
        m_bExhausted = true;
    }

    m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
}

void CFirstBattleWall::SetUp_CinematicCallBack()
{
    CCine_Manager::Get_Instance()->AddLastCallback("CS0106",
        [&]() {
            // TODO: 몬스터 0,1,2 를 Clone해주세요
            //  -> 위치를 받아오는 코드
            _float4x4 matResult_0{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0100", matResult_0);

            _float4x4 matResult_1{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0100_1", matResult_1);

            _float4x4 matResult_2{};
            CCine_Manager::Get_Instance()->GetMatTrans("em0100_2", matResult_2);

            vector<CMonsterBase::Monster_DESC> vecMonsterDesc;

            CMonsterBase::Monster_DESC monsterDesc{};
            monsterDesc.strObjID = L"Saber";
            monsterDesc.strDesc = "";
            monsterDesc.matTransform = matResult_0;
            vecMonsterDesc.emplace_back(monsterDesc);

            monsterDesc.matTransform = matResult_1;
            vecMonsterDesc.emplace_back(monsterDesc);

            monsterDesc.matTransform = matResult_2;
            vecMonsterDesc.emplace_back(monsterDesc);

            CCombat_Manager::Get_Instance()->Push_Wave(vecMonsterDesc);
            CCombat_Manager::Get_Instance()->Finish_Input_Wave();

            // TODO: 벽을 Clone해주세요 혹은 렌더링을 이때부터 켜주세요
            //    혹은 FirstWall 속 Latetick을 이걸로 조절해주세요 CCine_Manager::Get_Instance()->IsInGame();
            m_pModelCom->Set_AnimIndex(2, false);
            _float4x4 fMat;
            m_pModelCom->Play_Animation(0.016f, 0.016f, 0.f, fMat);
        }
    );
}

HRESULT CFirstBattleWall::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_FirstBattleWall"), TEXT("../Bin/Resources/Models/Map/Gimmick/First_BattleArea_Wall.fbx"),
            MODEL_TYPE::ANIM, fMat)
    );

    return S_OK;
}

HRESULT CFirstBattleWall::Initialize(void* pArg)
{
    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_pModelCom->Set_AnimIndex(0, false);

    //_float f8Beat = CBeat_Manager::Get_Instance()->Get_Beat() * 8.f;
    //m_fAnimSpeed = m_pModelCom->Get_Animation_PlayTime(0) / f8Beat;

    return S_OK;
}

void CFirstBattleWall::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat() * 8.f;
        _float fPlayTime = m_pModelCom->Get_Animation_PlayTime(0);
        m_fAnimSpeed = fPlayTime / fBeat;

        _float4x4 Mat{};
        m_pModelCom->Play_Animation(0.01f, 0.01f, 0.1f, Mat);

        if (GAMEMANAGER->Get_Current_Level() != LEVEL_MAPTOOL)
        {
            // 여기서는 시네마틱 끝난 후의 동작들이 실행됩니다
        
        }

        m_bInit = true;
    }
    m_iRenderGroup = 0;
}
//9, -0.4, 93.5
void CFirstBattleWall::Tick(_float fTimeDelta)
{
    //if (GAMEINSTANCE->Get_KeyDown(DIK_O))
    //    Execute();
    //if (GAMEINSTANCE->Get_KeyDown(DIK_P))
    //    Execute(false);

    if (CCombat_Manager::Get_Instance()->Get_Combat_End())
        Execute(false);
    auto pGI = m_pGameInstance.lock();
    m_bCulled = !pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 10.f);
    if (m_bExecuted)
    {
        if (!m_bAnimStart)
        {
            _uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
            if (m_iPrevBeat != iCurBeat)
                m_bAnimStart = true;
        }
        if (m_pModelCom->Get_Finished())
        {
            if(m_pModelCom->Get_CurrentAnimIndex() == 0)
                m_bDone = true;
            m_bExecuted = false;
            __super::m_bDone = true;
        }
    }

    //if (m_bDone)
    //{
    //    _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
    //    Transpose = XMMatrixMultiply(XMMatrixTranslation(0.f, 5.f, 0.f), Transpose);
    //    Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
    //    Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
    //    Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
    //    _float4x4 mat;
    //    XMStoreFloat4x4(&mat, Transpose);
    //    PxMat44 pxWorld;
    //    pxWorld.column0.x = mat.m[0][0];
    //    pxWorld.column0.y = mat.m[0][1];
    //    pxWorld.column0.z = mat.m[0][2];
    //    pxWorld.column0.w = mat.m[0][3];
    //    pxWorld.column1.x = mat.m[1][0];
    //    pxWorld.column1.y = mat.m[1][1];
    //    pxWorld.column1.z = mat.m[1][2];
    //    pxWorld.column1.w = mat.m[1][3];
    //    pxWorld.column2.x = mat.m[2][0];
    //    pxWorld.column2.y = mat.m[2][1];
    //    pxWorld.column2.z = mat.m[2][2];
    //    pxWorld.column2.w = mat.m[2][3];
    //    pxWorld.column3.x = mat.m[3][0];
    //    pxWorld.column3.y = mat.m[3][1];
    //    pxWorld.column3.z = mat.m[3][2];
    //    pxWorld.column3.w = mat.m[3][3];

    //    PxTransform transform{ pxWorld };
    //    m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);
    //}

    //else
    //{
    //    m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 300,-100000.f} });
    //}

    if (m_bAnimStart)
    {
        _float4x4 Mat{};
        m_pModelCom->Play_Animation(fTimeDelta, m_fAnimSpeed * fTimeDelta, 0.1f, Mat);
        if (m_pModelCom->Get_Finished())
            m_bAnimStart = false;
    }
}

void CFirstBattleWall::LateTick(_float fTimeDelta)
{
    if (m_bCulled)
        return;
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_DISTORTION, shared_from_this());
}

HRESULT CFirstBattleWall::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);
        _bool bNoEmit = false;
        _bool bGlass = false;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        {
            bGlass = true;
            if (m_iRenderGroup == 0)
                continue;
        }
        string strConstTag = "g_Emission_Texture";
        if (m_iRenderGroup == 1)
        {
            strConstTag = "g_Noise_Texture";
        }
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, strConstTag.c_str(), (_uint)i, aiTextureType_EMISSIVE)))
        {
            bNoEmit = true;
        }
        if (bGlass && m_iRenderGroup == 1)
        {
            m_pShaderCom->Bind_Float("g_fNoiseStrength", 0.01f);
            m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");
            _float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
            m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
            m_pShaderCom->Bind_Float("g_fTime", 0.f);
            _float4 vColor = { 0.4f,0.2f,0.f,0.5f };
            m_pShaderCom->Bind_Vector4("g_vDistortionColor", &vColor);

            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::DistortionBoost);
            m_pModelCom->Render((_uint)i);
            return S_OK;
        }
        else if (bNoEmit)
            m_pShaderCom->Begin(0);
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);

        m_pModelCom->Render((_uint)i);
    }
    ++m_iRenderGroup;
    return S_OK;
}

HRESULT CFirstBattleWall::Ready_Components(COMMONOBJ_DESC* pDesc)
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FirstBattleWall"), TEXT("Model_Com")));

    //m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysXCollider_Com")));

    PxVec3 BoxDimension{ 0.5f, 10.f, 14.f };
    PxBoxGeometry Box{ BoxDimension };
    string ColliderTag = m_strGimmickID;
    //m_pPhysXCollider->createStaticShapeActor(Box, ColliderTag.c_str());

    if (!m_pShaderCom || !m_pModelCom )
        assert(false);

    return S_OK;
}

HRESULT CFirstBattleWall::Bind_ShaderResources()
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

shared_ptr<CFirstBattleWall> CFirstBattleWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CFirstBattleWall
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFirstBattleWall(pDevice, pContext)
        {}
    };

    shared_ptr<CFirstBattleWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFirstBattleWall"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CFirstBattleWall::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CFirstBattleWall
    {
        MakeSharedEnabler(const CFirstBattleWall& rhs) : CFirstBattleWall(rhs)
        {}
    };

    shared_ptr<CFirstBattleWall> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFirstBattleWall"));
        assert(false);
    }

    return pInstance;
}

void CFirstBattleWall::Free()
{
}
