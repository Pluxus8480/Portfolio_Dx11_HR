#include "MimosaWall.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Beat_Manager.h"
#include "Cine_Manager.h"

#include "MapManager.h"

wstring CMimosaWall::ObjID = TEXT("CMimosaWall");
CMimosaWall::CMimosaWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMimosaWall::CMimosaWall(const CMimosaWall& rhs)
    : CGameObject{ rhs }
{
}

CMimosaWall::~CMimosaWall()
{
    Free();
}

HRESULT CMimosaWall::Initialize_Prototype()
{
    _float4x4 FMat = {};
    XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_MimosaWall"),
            TEXT("../Bin/Resources/Models/Map/Gimmick/Mimosa_Wall.fbx"),
            MODEL_TYPE::NONANIM, FMat)
    );

    return S_OK;
}

HRESULT CMimosaWall::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    Ready_Components();

    m_UpperLine.resize(16);
    m_vecUpperTH.resize(16);
    m_RightLine.resize(9);
    m_vecRightTH.resize(9);
    m_LeftLine.resize(9);
    m_vecLeftTH.resize(9);
    _int i = 0;
    for (auto& Mat : m_RightLine)
    {
        XMStoreFloat4x4(&Mat, XMMatrixAffineTransformation(XMVectorSet(1.f,1.f,1.f,0.f), XMVectorSet(0.f,0.f,0.f,1.f), XMQuaternionRotationRollPitchYaw(0.f, -XM_PIDIV2, 0.f),
            XMVectorSet(-28.f, 0.f, -12.f + (i) * 3.3333f, 1.f)));
        ++i;
    }
    i = 0;
    for (auto& Mat : m_LeftLine)
    {
        XMStoreFloat4x4(&Mat, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, 0.f),
            XMVectorSet(28.f, 0.f, -12.f + (i) * 3.3333f, 1.f)));
        ++i;
    }
    i = 0;
    for (auto& Mat : m_UpperLine)
    {
        XMStoreFloat4x4(&Mat, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, -XM_PI, 0.f),
            XMVectorSet(-25.f + i * 3.3333333f, 0.f, -16.f, 1.f)));
        ++i;
    }
    return S_OK;
}

void CMimosaWall::PriorityTick(_float fTimeDelta)
{
}

void CMimosaWall::Tick(_float fTimeDelta)
{
    if (!CCine_Manager::Get_Instance()->IsInGame())
    {
        for (_uint i = 0; i < m_UpperLine.size(); ++i)
        {
            m_UpperLine[i]._42 = 0.f;
        }
        for (_uint i = 0; i < m_RightLine.size(); ++i)
        {
            m_RightLine[i]._42 = 0.f;
        }
        for (_uint i = 0; i < m_LeftLine.size(); ++i)
        {
            m_LeftLine[i]._42 = 0.f;
        }
        return;
    }

    auto pBM = BEATMANAGER;
    _uint iCurrBeat = pBM->Get_BeatCount();
    if (iCurrBeat != m_iPrevBeat)
    {
        auto pGI = m_pGameInstance.lock();
        m_iPrevBeat = iCurrBeat;
        for (auto& fTH : m_vecUpperTH)
        {
            fTH = pGI->Random_Float(0.f, 3.f);
        }
        for (auto& fTH : m_vecRightTH)
        {
            fTH = pGI->Random_Float(0.f, 3.f);
        }
        for (auto& fTH : m_vecLeftTH)
        {
            fTH = pGI->Random_Float(0.f, 3.f);
        }
    }
    _float fBR = pBM->Get_BeatRate();
    if (fBR < 0.25f)
    {
        _float fLerp = fBR * 4.f;
        for (_uint i = 0; i < m_UpperLine.size(); ++i)
        {
            m_UpperLine[i]._42 = m_vecUpperTH[i] * fLerp;
        }
        for (_uint i = 0; i < m_RightLine.size(); ++i)
        {
            m_RightLine[i]._42 = m_vecRightTH[i] * fLerp;
        }
        for (_uint i = 0; i < m_LeftLine.size(); ++i)
        {
            m_LeftLine[i]._42 = m_vecLeftTH[i] * fLerp;
        }
    }
    else if (fBR < 0.75f)
    {
        _float fLerp = 1.f - (fBR - 0.25f) * 2.f;
        for (_uint i = 0; i < m_UpperLine.size(); ++i)
        {
            m_UpperLine[i]._42 = m_vecUpperTH[i] * fLerp;
        }
        for (_uint i = 0; i < m_RightLine.size(); ++i)
        {
            m_RightLine[i]._42 = m_vecRightTH[i] * fLerp;
        }
        for (_uint i = 0; i < m_LeftLine.size(); ++i)
        {
            m_LeftLine[i]._42 = m_vecLeftTH[i] * fLerp;
        }
    }
    else
    {
        for (_uint i = 0; i < m_UpperLine.size(); ++i)
        {
            m_UpperLine[i]._42 = 0.f;
        }
        for (_uint i = 0; i < m_RightLine.size(); ++i)
        {
            m_RightLine[i]._42 = 0.f;
        }
        for (_uint i = 0; i < m_LeftLine.size(); ++i)
        {
            m_LeftLine[i]._42 = 0.f;
        }
    }

}

void CMimosaWall::LateTick(_float fTimeDelta)
{
    auto pGameInstance = m_pGameInstance.lock();

    pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMimosaWall::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();


    for (size_t i = 0; i < iNumMeshes; i++)
    {
        _bool bEmissive = { false };

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", (_uint)i, aiTextureType_EMISSIVE)))
            bEmissive = false;
        else
            bEmissive = true;

        m_pModelCom->Bind_Buffers((_uint)i);
        _float4x4 World;
        for(auto& Mat : m_UpperLine)
        {
            XMStoreFloat4x4(&World, XMLoadFloat4x4(&Mat) * m_pTransformCom->Get_WorldMatrix_XMMat());
            m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World);
            if (bEmissive)
                m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
            else
                m_pShaderCom->Begin((_uint)MESH_PASS::Default);
            m_pModelCom->Draw((_uint)i);
        }
        for (auto& Mat : m_RightLine)
        {
            XMStoreFloat4x4(&World, XMLoadFloat4x4(&Mat) * m_pTransformCom->Get_WorldMatrix_XMMat());
            m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World);
            if (bEmissive)
                m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
            else
                m_pShaderCom->Begin((_uint)MESH_PASS::Default);
            m_pModelCom->Draw((_uint)i);
        }
        for (auto& Mat : m_LeftLine)
        {
            XMStoreFloat4x4(&World, XMLoadFloat4x4(&Mat) * m_pTransformCom->Get_WorldMatrix_XMMat());
            m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World);
            if (bEmissive)
                m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
            else
                m_pShaderCom->Begin((_uint)MESH_PASS::Default);
            m_pModelCom->Draw((_uint)i);
        }
    }

    return S_OK;
}

void CMimosaWall::Ready_Components()
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();
    if (iLevelIndex == LEVEL_MAPTOOL)
        iLevelIndex = LEVEL_STATIC;
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Model_MimosaWall"), TEXT("Com_Model")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);
}

HRESULT CMimosaWall::Bind_ShaderResources()
{
    auto pGameInstance = m_pGameInstance.lock();

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    //    return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CMimosaWall> CMimosaWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CMimosaWall
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosaWall(pDevice, pContext)
        {}
    };

    shared_ptr<CMimosaWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMimosaWall"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CMimosaWall::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CMimosaWall
    {
        MakeSharedEnabler(const CMimosaWall& rhs) : CMimosaWall(rhs)
        {}
    };

    shared_ptr<CMimosaWall> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMimosaWall"));
        assert(false);
    }

    return pInstance;
}

void CMimosaWall::Free()
{
}
