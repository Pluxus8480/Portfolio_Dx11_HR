#include "ComboSwitch.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "PhysxCollider.h"
#include "Mesh.h"

#include "ComboSwitchButton.h"
#include "LargeElevator.h"
#include "GearWall.h"
#include "PlayerManager.h"
#include "Chai.h"
#include "Beat_Manager.h"
#include "CUi_ComboButton.h"
#include "CUi_BeatJudge.h"
#include "PlayerManager.h"
#include "Player_Camera.h"
#include "CUi_JudgeWord.h"
#include "CUi_CommandIcon.h"
#include "GameManager.h"
#include "MapManager.h"


wstring CComboSwitch::ObjID = TEXT("CComboSwitch");
CComboSwitch::CComboSwitch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGimmickBase{pDevice, pContext}
{
}

CComboSwitch::CComboSwitch(const CComboSwitch& rhs)
    : CGimmickBase{rhs}
{
}

CComboSwitch::~CComboSwitch()
{
    Free();
}

void CComboSwitch::Set_Dead(bool bDead)
{
    m_bIsDead = bDead;

    m_pButton->Set_Dead(bDead);
}

HRESULT CComboSwitch::Initialize_Prototype()
{
    _float4x4 fMat = {};
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ComboSwitch_Off"), TEXT("../Bin/Resources/Models/Map/Gimmick/ComboSwitch_Off.FBX"),
            MODEL_TYPE::NONANIM, fMat)
    );
    m_List_Owning_ObjectID.push_back(CComboSwitchButton::ObjID);

    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_FloorSwitch1", L"../Bin/Resources/Sounds/MapObject/Button_Push1.ogg", SOUND_CHANNEL::SOUND_EFFECT));
    m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_FloorSwitch2", L"../Bin/Resources/Sounds/MapObject/Button_Push2.ogg", SOUND_CHANNEL::SOUND_EFFECT));

    return S_OK;
}

HRESULT CComboSwitch::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    if (GAMEMANAGER->Get_Current_Level() != LEVEL_MAPTOOL)
    {
        CUi_ComboButton::Desc Desc;
        vector<_bool> Vec;
        Vec.emplace_back(false);
        Vec.emplace_back(false);
        Vec.emplace_back(false);

        m_vecIs_Left = Vec;
        Desc.LButtonVec = Vec;
        Desc.RelativeDistance = { 0, 170 };
        Desc.OwnerTransform = m_pTransformCom;
        m_pUI = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_ComboButton::ObjID, LEVEL_UITOOL, L"CUi", CUi_ComboButton::ObjID, &Desc);
    }

    return S_OK;
}

void CComboSwitch::PriorityTick(_float fTimeDelta)
{
}

void CComboSwitch::Tick(_float fTimeDelta)
{
    //if (!m_bDone && static_pointer_cast<CComboSwitchButton>(m_pButton)->Get_SwtichOn() == false)
    //{
    //    static_pointer_cast<CGimmickBase> (m_vecRelatives.back())->Execute();
    //    m_bDone = true;
    //}

    //if (GAMEINSTANCE->Get_KeyDown(DIK_O))
    //{
    //    static_pointer_cast<CGimmickBase> (m_vecRelatives.back())->Execute();
    //    m_bDone = true;
    //    m_pButton->Set_Dead();
    //}

#ifdef _DEBUG
    if (GAMEINSTANCE->Get_KeyDown(DIK_L))
        m_bExecuted = true;
#endif

    if (GAMEMANAGER->Get_Current_Level() == LEVEL_MAPTOOL)
        return;

    if (m_bTimer_Start)
    {
        m_fTimer += fTimeDelta;
        _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
        if (m_fTimer > fBeat * 2.f)
        {
            m_bExecuted = true;
            m_bTimer_Start = false;
        }
    }
    if (!m_bDone)
    {
        _float distance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - PLAYERMANAGER->Get_Player()->Get_Transform()->Get_Position()));
        if (distance < 5.f && !m_bExecuted)
        {
            m_pUI->Set_Active(true);
            shared_ptr<CChai> pPlayer = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player());
            pPlayer->Get_Player_Camera()->Set_Gimmic_Lock_On_Target(shared_from_this());

            shared_ptr<CUi_ComboButton> pCOmboButton = static_pointer_cast<CUi_ComboButton> (m_pUI);
            shared_ptr<CUi_BeatMarkerSystem> beatMarkersys = static_pointer_cast<CUi_BeatMarkerSystem> (CUi_Mgr::Get_Instance()->Find_UI(TEXT("CUi_BeatMarkerSystem")));

            if (m_iCurrCombo == 0 && m_vecIs_Left[m_iCurrCombo] == false && pPlayer->Get_CurrCombo() == "R" && GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
            {
                pCOmboButton->Set_Success();
                m_iCurrCombo++;
            }
            else if (m_iCurrCombo == 1 && m_vecIs_Left[m_iCurrCombo] == false && pPlayer->Get_CurrCombo() == "R" && GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
            {
                pCOmboButton->Set_Success();
                m_iCurrCombo++;
            }
            else if (m_iCurrCombo == 2 && m_vecIs_Left[m_iCurrCombo] == false && pPlayer->Get_CurrCombo() == "RR" && GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON))
            {
                pCOmboButton->Set_Success();

            }
            else if (pPlayer->Get_CurrCombo() != "R" && pPlayer->Get_CurrCombo() != "RR" && pPlayer->Get_CurrCombo() != "RRR")
            {
                m_iCurrCombo = 0;
                pCOmboButton->Set_Success(false);

            }
            if(pPlayer->Get_Command_Icon())
                m_pCommandICon = pPlayer->Get_Command_Icon();          

            if ((GAMEINSTANCE->Get_MouseDown(DIMK_LBUTTON) || GAMEINSTANCE->Get_MouseDown(DIMK_RBUTTON)) && m_pCommandICon)
            {

                CUi_JudgeWord::WordType eWordType = static_pointer_cast<CUi_CommandIcon>(m_pCommandICon)->Cal_Judge();
                if (m_iCurrCombo == 2 && pPlayer->Get_CurrCombo() == "RRR" && eWordType == CUi_JudgeWord::WordType::Perfect)
                {
                    m_pCommandICon = nullptr;
                    PLAYERMANAGER->Callback_Event("RRR");

                    pCOmboButton->Set_Success();
                    m_bTimer_Start = true;
                }
                else if (m_iCurrCombo == 2 && pPlayer->Get_CurrCombo() == "RRR" && eWordType != CUi_JudgeWord::WordType::Perfect)
                {
                    m_pCommandICon = nullptr;
                    m_iCurrCombo = 0;
                    pCOmboButton->Set_Success(false);
                }
            }
        }
        else
        {
            m_pUI->Set_Active(false);
            static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Player_Camera()->Set_Gimmic_Lock_On_Target(nullptr);
        }
    }
    if (m_bExecuted && !m_bDone)
    {
        //if (!m_bGearExecuted)
        //{
        //    static_pointer_cast<CGearWall>(m_vecRelatives[GEARWALL])->Execute();
        //    m_bGearExecuted = true;
        //}
        //
        //else
        //{
        //    if (static_pointer_cast<CGearWall>(m_vecRelatives[GEARWALL])->Get_Done())
        //    {
        //        m_vecRelatives[ELEVATOR]->Execute();
        //        m_bDone = true;
        //        m_bExecuted = false;
        //    }
        //}

        if (!m_vecRelatives[m_iCurIndex]->Get_Done() && !m_vecRelatives[m_iCurIndex]->Get_Executed())
        {
            if (m_iCurIndex == 0)
            {
                _float4 fPos = {};
                XMStoreFloat4(&fPos, m_vecRelatives[1]->Get_Transform()->Get_Position());
                
               _vector playerPos = static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Transform()->Get_Position();
                _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
                PLAYERMANAGER->Callback_Event("Gear_Wall_End");
                static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Set_Root_Move(false);
                static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Set_Event_Position(fPos);
                static_pointer_cast<CChai>(PLAYERMANAGER->Get_Player())->Get_Transform()->LookAt_Horizontal(m_vecRelatives[0]->Get_Transform()->Get_Position() - m_vecRelatives[0]->Get_Transform()->Get_NormRight() * 100.f);
            }
            m_vecRelatives[m_iCurIndex]->Execute();
        }

        else if(m_vecRelatives[m_iCurIndex]->Get_Done())
        {
            ++m_iCurIndex;

            if (m_iCurIndex == m_iMaxIndex)
            {
                m_bDone = true;
                m_bExecuted = false;
            }
        }

    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CComboSwitch::LateTick(_float fTimeDelta)
{
    //if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

    GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

    if(!m_bDone)
        GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CComboSwitch::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
        //	return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
        //	return E_FAIL;
        m_pShaderCom->Begin(0);

        //if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
        //	return E_FAIL;

        m_pModelCom->Render((_uint)i);
    }

#ifdef _DEBUG
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CComboSwitch::Edit_Attribute()
{
    _char szComboTest[MAX_PATH];
    strcpy(szComboTest, m_strComboTest.c_str());

    if (ImGui::InputText("Combo", szComboTest, MAX_PATH))
        m_strComboTest = szComboTest;
}

void CComboSwitch::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    m_vecRelatives.emplace_back(pRelativeObject);
}

void CComboSwitch::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
    auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
    if (iter != m_vecRelatives.end())
        m_vecRelatives.erase(iter);
}

void CComboSwitch::Save_Data(ofstream& os)
{
    __super::Save_Data(os);

    Save_Relatives(os);

    size_t size = m_strComboTest.size();
    write_typed_data(os, m_strComboTest.size());
    os.write(&m_strComboTest[0], m_strComboTest.size());
}

void CComboSwitch::Load_Data(ifstream& is)
{
    __super::Load_Data(is);

    Load_Relatives(is);

    size_t size = {};

    read_typed_data(is, size);
    m_strComboTest.resize(size);
    is.read(&m_strComboTest[0], size);

    m_iMaxIndex = (_uint)m_vecGimmickIDs.size();
}

void CComboSwitch::TakeDamage(const DamageInfo& damageInfo)
{
    if (!m_bDone && !m_bExecuted)
    {
        //static_pointer_cast<CGimmickBase> (m_vecRelatives.back())->Execute();
        //m_bDone = true;
        //m_pButton->Set_Dead();
        auto pGI = m_pGameInstance.lock();
        pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_FloorSwitch1", m_pTransformCom->Get_Position());
        pGI->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_FloorSwitch2", m_pTransformCom->Get_Position());
        _matrix mMat =  m_pTransformCom->Get_WorldMatrix_XMMat();
        _vector vPos = mMat.r[3];
        vPos += m_pTransformCom->Get_NormRight()*2.f;
        mMat.r[3] = vPos;

        if (damageInfo.strAttackName == "R")
            PLAYERMANAGER->Callback_Event("R");
        if (damageInfo.strAttackName == "RR")
            PLAYERMANAGER->Callback_Event("RR");

        _float4x4 fMat = {};
        XMStoreFloat4x4(&fMat, mMat);
        EFFECTMANAGER->Call_Fixed_Union_Effect("Big_HitEffect",&fMat, 1.f, true);

        shared_ptr<CUi_ComboButton> pCOmboButton = static_pointer_cast<CUi_ComboButton> (m_pUI);
        shared_ptr<CUi_BeatMarkerSystem> beatMarkersys = static_pointer_cast<CUi_BeatMarkerSystem> (CUi_Mgr::Get_Instance()->Find_UI(TEXT("CUi_BeatMarkerSystem")));

       

    }
}

HRESULT CComboSwitch::Ready_Components()
{
    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ComboSwitch_Off"), TEXT("Model_Com")));
    
    CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
    sphereDesc.fRadius = 1.3f;
    sphereDesc.vCenter = { 1.f, 1.f, 0.f };

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Com"), &sphereDesc));

    m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));
    _float3 vWorldSize{};
    _matrix matWorld = m_pTransformCom->Get_WorldMatrix_XMMat();
    vWorldSize.x = XMVectorGetX(XMVector3Length(matWorld.r[0]));
    vWorldSize.y = XMVectorGetX(XMVector3Length(matWorld.r[1]));
    vWorldSize.z = XMVectorGetX(XMVector3Length(matWorld.r[2]));

    _matrix scaleMatrix = XMMatrixScaling(vWorldSize.x, vWorldSize.y, vWorldSize.z);
    _float4x4 fScaleMat{};
    XMStoreFloat4x4(&fScaleMat, scaleMatrix);
    PxVec3 pscale = { vWorldSize.x, vWorldSize.y, vWorldSize.z };
    string strTag = string(CComboSwitch::ObjID.begin(), CComboSwitch::ObjID.end());
    auto pMesh = m_pModelCom->Get_Mesh(0);
    pMesh->Cooking_TriangleMesh(fScaleMat);
    auto pTriangleMesh = pMesh->Get_TriangleMesh();
    if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
        auto pRigid = m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, strTag.c_str());
    
    if (!m_pShaderCom || !m_pModelCom || !m_pColliderCom || !m_pPhysXCollider)
        assert(false);

    _matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
    //Transpose = XMMatrixMultiply(XMMatrixTranslation(m_vModelCenter.x, m_vModelCenter.y, m_vModelCenter.z), Transpose);
    Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
    Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
    Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
    _float4x4 mat;
    XMStoreFloat4x4(&mat, Transpose);
    PxMat44 pxWorld;
    pxWorld.column0.x = mat.m[0][0];
    pxWorld.column0.y = mat.m[0][1];
    pxWorld.column0.z = mat.m[0][2];
    pxWorld.column0.w = mat.m[0][3];
    pxWorld.column1.x = mat.m[1][0];
    pxWorld.column1.y = mat.m[1][1];
    pxWorld.column1.z = mat.m[1][2];
    pxWorld.column1.w = mat.m[1][3];
    pxWorld.column2.x = mat.m[2][0];
    pxWorld.column2.y = mat.m[2][1];
    pxWorld.column2.z = mat.m[2][2];
    pxWorld.column2.w = mat.m[2][3];
    pxWorld.column3.x = mat.m[3][0];
    pxWorld.column3.y = mat.m[3][1];
    pxWorld.column3.z = mat.m[3][2];
    pxWorld.column3.w = mat.m[3][3];

    PxTransform transform{ pxWorld };
    m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

    return S_OK;
}

HRESULT CComboSwitch::Bind_ShaderResources()
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

HRESULT CComboSwitch::Ready_Parts()
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

    CComboSwitchButton::PARTOBJ_DESC partDesc{};
    partDesc.fRotationPerSec = 0.f;
    partDesc.fSpeedPerSec = 0.f;
    partDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    partDesc.pParentTransform = m_pTransformCom;

    m_pButton = GAMEINSTANCE->Add_Clone_Return(iLevelIndex, TEXT("Layer_Map"), CComboSwitchButton::ObjID, &partDesc);

    assert(m_pButton);
    
    return S_OK;
}

shared_ptr<CComboSwitch> CComboSwitch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CComboSwitch
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComboSwitch(pDevice, pContext)
        {}
    };

    shared_ptr<CComboSwitch> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CComboSwitch"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CComboSwitch::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CComboSwitch
    {
        MakeSharedEnabler(const CComboSwitch& rhs) : CComboSwitch(rhs)
        {}
    };

    shared_ptr<CComboSwitch> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CComboSwitch"));
        assert(false);
    }

    return pInstance;
}

void CComboSwitch::Free()
{
}
