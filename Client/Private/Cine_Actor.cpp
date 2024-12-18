#include "Cine_Actor.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"
#include "Bone.h"
#include "Texture.h"

#include "Effect_Manager.h"

#include "sstream"


wstring CCine_Actor::ObjID = TEXT("CCine_Actor");

CCine_Actor::CCine_Actor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CCine_Actor::CCine_Actor(const CCine_Actor& rhs)
    : CGameObject(rhs)
{
}

CCine_Actor::~CCine_Actor()
{
    Free();
}

HRESULT CCine_Actor::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCine_Actor::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC TransformDesc{};
    TransformDesc.fSpeedPerSec = 1.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    auto pDesc = static_cast<DESC*>(pArg);

    m_strModelTag = pDesc->strModelTag;
    m_strModelPath = pDesc->strModelPath;
    m_strActorName = pDesc->strActorName;

    // Loaded prototype of selected model
    _matrix	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));

    wstring wstrModelTag = wstring(m_strModelTag.begin(), m_strModelTag.end());
    if (nullptr == GAMEINSTANCE->Get_Component_Prototype(m_iLevelIndex, wstrModelTag))
    {
        GAMEINSTANCE->Add_Prototype(
            m_iLevelIndex, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, m_strModelPath.c_str(), LocalTransformMatrix)
        );
    }
    if (FAILED(Ready_Components(wstrModelTag)))
        return E_FAIL;

    // About actor's exception
    m_strDesc = pDesc->strDesc;
    SetException(pDesc->strDesc);

    // For effect handling
    m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
        if (isAttach)
            CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_matCurRoot, m_fEffectSpeed, isBillBoard);
        else
            CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_matCurRoot, m_fEffectSpeed, isBillBoard);
        }
    );

    // Only for Kale's arm exception
    m_Before_KaleArm_Red = { 3 };
    m_After_KaleArm_Red = { 5, 6, 7, 8 };
    m_Before_KaleArm_Blue = { 0, 2, 3, 4 };
    m_After_KaleArm_Blue = { 7, 8, 9, 10 };

    return S_OK;
}

void CCine_Actor::PriorityTick(_float fTimeDelta)
{
    m_iRenderGroup = 0;
}

void CCine_Actor::Tick(_float fTimeDelta)
{
    // Effect play speed control
    m_fEffectSpeed = 60.f / m_pModelCom->Get_AnimTickPerSecond();

    m_pModelCom->PlayReservedCallbacks();
}

void CCine_Actor::LateTick(_float fTimeDelta)
{
    if (EState::ACTING == m_eState)
    {
        m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
        if (m_strActorName == "bg1100")
            GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_DISTORTION, shared_from_this());
    }
}

HRESULT CCine_Actor::Render()
{
    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // Exception for Door
        if (m_strActorName == "bg1100")
        {
            RenderIntroDoor(i);
            continue;
        }

        // Noraml Exception
        if (m_isExcepStart)
        {
            if (find(m_ExcepRenderList.begin(), m_ExcepRenderList.end(), i) != m_ExcepRenderList.end())
                continue;
        }
        if (m_strModelTag == "ch0140" && i == 3)
            continue;

        // Exception for kale's Arm
        if (m_strModelTag == "em7510")
        {
            if (m_isKaleArm_Blue_On)
            {
                if (find(m_After_KaleArm_Blue.begin(), m_After_KaleArm_Blue.end(), i) != m_After_KaleArm_Blue.end())
                    continue;
            }
            else
            {
                if (find(m_Before_KaleArm_Blue.begin(), m_Before_KaleArm_Blue.end(), i) != m_Before_KaleArm_Blue.end())
                    continue;
            }
        }
        else if (m_strModelTag == "em7520")
        {
            if (m_isKaleArm_Red_On)
            {
                if (find(m_After_KaleArm_Red.begin(), m_After_KaleArm_Red.end(), i) != m_After_KaleArm_Red.end())
                    continue;
            }
            else
            {
                if (find(m_Before_KaleArm_Red.begin(), m_Before_KaleArm_Red.end(), i) != m_Before_KaleArm_Red.end())
                    continue;
            }
        }

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
            return E_FAIL;

        if (m_isMimosa)
        {
            if (i == 25 || i == 26 || i == 32)
            {
                m_pWingTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture");
                _float4 vColor = { 0.f,1.f,1.f,1.f };
                m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
                m_pShaderCom->Begin((_uint)ANIMMESH_PASS::MimosaWing);

                m_pModelCom->Render((_uint)i);

                continue;
            }
        }

        _bool bEmi = { false };
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
            bEmi = false;
        else
            bEmi = true;

         if (bEmi)
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
        else
            m_pShaderCom->Begin((_uint)ANIMMESH_PASS::DEFFEREDOUTLINE);

        m_pModelCom->Render((_uint)i);
    }

    ++m_iRenderGroup;
    return S_OK;
}

HRESULT CCine_Actor::Ready_Components(const wstring& strModelTag)
{
    /* For.Com_Shader */
    m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    /* For.Com_Model */
    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, strModelTag, TEXT("Com_Model")));

    if (!m_pShaderCom || !m_pModelCom)
        assert(false);

    return S_OK;
}

void CCine_Actor::SetPosition(_fvector vInitPos)
{
    m_pTransformCom->Set_Position(vInitPos);
}
void CCine_Actor::SetTransform(const _float4x4& _matTrans)
{
    m_matCurrent = _matTrans;
    m_pTransformCom->Set_WorldMatrix(_matTrans);
    _matrix Bone = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(2));
    Bone = XMMatrixTranslationFromVector(Bone.r[3]);
    XMStoreFloat4x4(&m_matCurRoot, Bone * m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CCine_Actor::GetAnimStrList(vector<string>& listAnim) const
{
    vector<shared_ptr< CAnimation>> vecAnimList = m_pModelCom->Get_Animation_List();
    for (size_t i = 0; i < vecAnimList.size(); i++)
    {
        listAnim.push_back(vecAnimList[i]->Get_AnimName());
    }
}

void CCine_Actor::SwitchAnim(const string& strAnim)
{
    m_pModelCom->Set_AnimIndex_NoInterpole(strAnim, false);
}

_bool CCine_Actor::PlayAnimation(_float fFrame, _int iSeqIndex)
{
    _float4x4 matResult{};
    m_pModelCom->Set_AnimFrame_With_Effect(fFrame);

    if (m_isMimosa)
    {
        if (m_strAnim_Excep == m_pModelCom->Get_AnimString(m_pModelCom->Get_CurrentAnimIndex()))
        {
            if (m_fFrame_ExcepStart <= fFrame)
                m_isMimosa_ExcepEnd = true;
        }

        if (!m_isMimosa_ExcepEnd)
        {
            _float4x4 matResult{};

            _uint iFakeWingRNum = m_pModelCom->Get_BoneIdx("catch_attach_R_000");
            _uint iFakeWingLNum = m_pModelCom->Get_BoneIdx("catch_attach_L_000");

            XMStoreFloat4x4(&matResult, XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("catch_attach_R_000")));
            m_pModelCom->Set_Basic_BoneMatrix(iFakeWingRNum, XMLoadFloat4x4(&matResult));
            XMStoreFloat4x4(&matResult, XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("catch_attach_L_000")));
            m_pModelCom->Set_Basic_BoneMatrix(iFakeWingLNum, XMLoadFloat4x4(&matResult));

            _uint iWingRNum = m_pModelCom->Get_BoneIdx("attach_R_000");
            _uint iWingLNum = m_pModelCom->Get_BoneIdx("attach_L_000");

            XMStoreFloat4x4(&matResult, XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("attach_R_000")));
            m_pModelCom->Set_Basic_BoneMatrix(iWingRNum, XMLoadFloat4x4(&matResult));
            XMStoreFloat4x4(&matResult, XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMLoadFloat4x4(m_pModelCom->Get_BoneBasicMatrixPtr("attach_L_000")));
            m_pModelCom->Set_Basic_BoneMatrix(iWingLNum, XMLoadFloat4x4(&matResult));

            m_pModelCom->Update_Bones(matResult);
        }
    }
    else if (!m_strAnim_Excep.empty())
    {
        if (m_strAnim_Excep == m_pModelCom->Get_AnimString(m_pModelCom->Get_CurrentAnimIndex()))
        {
            if (m_fFrame_ExcepStart <= fFrame)
                m_isExcepStart = true;
        }
    }

    if ("CS1215_00_008_em7510_00" == m_pModelCom->Get_AnimString(m_pModelCom->Get_CurrentAnimIndex()) && fFrame >= 12.f)
        m_isKaleArm_Blue_On = true;
    else if ("CS1215_00_007_em7520_00" == m_pModelCom->Get_AnimString(m_pModelCom->Get_CurrentAnimIndex()) && fFrame >= 19.f)
        m_isKaleArm_Red_On = true;

    if (m_pModelCom->Get_Current_Anim_Track_Position_Percentage() >= 1.f)
        return true;
    else
        return false;
}

void CCine_Actor::SetAnimFrame(_float fFrame)
{
    m_pModelCom->Set_AnimFrame(fFrame);
}

void CCine_Actor::GetAnimInfo(shared_ptr<CAnimation>& ppAnim, _uint iAnimIndex)
{
    auto pAnim = m_pModelCom->Get_Animation_List()[iAnimIndex];
    ppAnim = pAnim;
}

HRESULT CCine_Actor::ExportAnimInfo(const string& strFolderPath)
{
    return m_pModelCom->ExportAnimInfo(strFolderPath);
}

void CCine_Actor::GetBoneList(vector<string>& listBone) const
{
    vector<shared_ptr<CBone>> vecBoneList = m_pModelCom->Get_Bone_List();
    for (size_t i = 0; i < vecBoneList.size(); ++i)
        listBone.push_back(vecBoneList[i]->Get_Name());
}

const _float4x4* CCine_Actor::GetBoneMatrix(const string& strBoneName)
{
    return m_pModelCom->Get_BoneMatrixPtr(strBoneName.c_str());
}

_float CCine_Actor::GetAnimDuration(const string& strAnimName)
{
    return m_pModelCom->Get_Duration(strAnimName);
}

void CCine_Actor::SetException(const string& strDesc)
{
    if (strDesc.empty())
        return;

    vector<string> resultList;
    stringstream ss(strDesc);

    string strToken;
    while (getline(ss, strToken, '|'))
        resultList.push_back(strToken);

    for (_uint i = 0; i < resultList.size(); ++i)
    {
        if (i == 0 && !resultList[i].empty())
            m_fSize = stof(resultList[i]);
        if (i == 1 && !resultList[i].empty())
        {
            stringstream ss(resultList[i]);
            string strToken;
            while (getline(ss, strToken, ','))
            {
                if (!strToken.empty())
                    m_ExcepRenderList.push_back(stoi(strToken));
            }
        }
        if (i == 2 && !resultList[i].empty())
        {
            m_isMimosa = true;
            m_pWingTexture = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Mimosa_WingTex"), TEXT("Com_Texture")));

            if (stoi(resultList[i]) == 2)
                m_isMimosa_ExcepEnd = true;
        }
        if (i == 3 && !resultList[i].empty())
        {
            m_isExcepStart = false;
            m_strAnim_Excep = resultList[i];
        }
        if (i == 4 && !resultList[i].empty())
        {
            m_fFrame_ExcepStart = stof(resultList[i]);
        }
    }
}

shared_ptr<CCine_Actor> CCine_Actor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CCine_Actor
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCine_Actor(pDevice, pContext) {}
    };

    shared_ptr<CCine_Actor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCine_Actor"));
        assert(false);
    }
    return pInstance;
}
shared_ptr<CGameObject> CCine_Actor::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CCine_Actor
    {
        MakeSharedEnabler(const CCine_Actor& rhs) : CCine_Actor(rhs) {}
    };

    shared_ptr<CCine_Actor> pInstance = make_shared<MakeSharedEnabler>(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCine_Actor"));
        assert(false);
    }
    return pInstance;
}

void CCine_Actor::Free()
{
}

HRESULT CCine_Actor::RenderIntroDoor(size_t iMeshIndex)
{
    m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)iMeshIndex);
    _bool bNoEmit = false;
    _bool bGlass = false;
    if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)iMeshIndex, aiTextureType_DIFFUSE)))
    {
        bGlass = true;
        if (m_iRenderGroup == 0)
            return S_OK;
    }
    string strConstTag = "g_Emission_Texture";
    if (m_iRenderGroup == 1)
    {
        strConstTag = "g_Noise_Texture";
    }
    if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, strConstTag.c_str(), (_uint)iMeshIndex, aiTextureType_EMISSIVE)))
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
        m_pModelCom->Render((_uint)iMeshIndex);
        return S_OK;
    }
    else if (bNoEmit)
        m_pShaderCom->Begin(0);
    else
        m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);

    m_pModelCom->Render((_uint)iMeshIndex);
    return S_OK;
}
