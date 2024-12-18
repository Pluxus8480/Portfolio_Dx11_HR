#include "CJeongGimmicBaseDefault.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"
#include "Math_Manager.h"
#include "GameManager.h"
#include "MapManager.h"


wstring CJeongGimmicBaseDefault::ObjID = TEXT("CJeongGimmicBaseDefault");

CJeongGimmicBaseDefault::CJeongGimmicBaseDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CJeongGimmicBase(pDevice, pContext)
{
}

CJeongGimmicBaseDefault::CJeongGimmicBaseDefault(const CJeongGimmicBaseDefault& rhs)
    : CJeongGimmicBase(rhs)
{
}

CJeongGimmicBaseDefault::~CJeongGimmicBaseDefault()
{
    Free();
}

HRESULT CJeongGimmicBaseDefault::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CJeongGimmicBaseDefault::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    Argument_Setting(Arg);
    InitializeBasicComponent(Arg->ModelName, Arg->ShaderName);
    Initialize_CookPhysX(Arg->ModelName);
    m_strGimmickNameID = Arg->ModelName;

    if (m_strGimmickNameID == L"Prototype_Component_Model_CoreA1")
        m_bDMGConsole = true;
    
    return S_OK;
}

void CJeongGimmicBaseDefault::PriorityTick(_float fTimeDelta)
{
    if (!m_bInit)
    {
        if (m_bDMGConsole)
        {
            _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

            for (_uint i = 0; i < 3; ++i)
                m_fSecPerFrame[i] = fBeat / (_float)m_iMaxFrame[i];
        }

        m_bInit = true;
    }
}

void CJeongGimmicBaseDefault::Tick(_float fTimeDelta)
{
    Use_PhysX();

    if (m_bDMGConsole)
    {
        for (_uint i = 0; i < 3; ++i)
        {
            m_fTime[i] += fTimeDelta;
            if (m_fTime[i] > m_fSecPerFrame[i])
            {
                ++m_iFrameIndex[i];
                m_iFrameIndex[i] %= m_iMaxFrame[i];
            }
        }
    }
}

void CJeongGimmicBaseDefault::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fModelRange))
        return;
    
    pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());

   /* if(GAMEINSTANCE->Get_KeyDown(DIK_ADD))
		m_iSkipMeshNum++;

	if (GAMEINSTANCE->Get_KeyDown(DIK_SUBTRACT))
		m_iSkipMeshNum--;*/

}


void CJeongGimmicBaseDefault::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
     m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Platform_Land", m_pTransformCom->Get_Position());
	 EFFECTMANAGER->Call_Union_Effect("Platform_Cylinder_Effect", m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, false);
}

HRESULT CJeongGimmicBaseDefault::Render()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	//if (m_iSkipMeshNum > iNumMeshes) m_iSkipMeshNum = 0;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
		/*if (m_iSkipMeshNum == i && m_strGimmickNameID == L"Prototype_Component_Model_CoreMainA01" )
			continue;*/

        _bool bEmi = { false };
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
            continue;

        if (SUCCEEDED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
            bEmi = true;

        if (m_bDMGConsole)
        {
            if (i == 11)
            {
                XMINT2 divide = { 4, 4 };
                if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &divide, sizeof(XMINT2))))
                    return E_FAIL;
                if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex[0], sizeof(_uint))))
                    return E_FAIL;

                m_pShaderCom->Begin((_uint)MESH_PASS::Frame);
            }
            else if (i == 12)
            {
                XMINT2 divide = { 1, 8 };
                if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &divide, sizeof(XMINT2))))
                    return E_FAIL;
                if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex[1], sizeof(_uint))))
                    return E_FAIL;

                m_pShaderCom->Begin((_uint)MESH_PASS::Frame);
            }
            else if (i == 39)
            {
                XMINT2 divide = { 2, 2 };
                if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &divide, sizeof(XMINT2))))
                    return E_FAIL;
                if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex[2], sizeof(_uint))))
                    return E_FAIL;

                m_pShaderCom->Begin((_uint)MESH_PASS::Frame);
            }
            else
                m_pShaderCom->Begin((_uint)MESH_PASS::Default);
        }
        else if (i == 2 && m_strGimmickNameID == L"Prototype_Component_Model_CoreMainA01") {
            _float4 vColor = { 5.f, 0.5f, 0.f, 1.f };
            m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
            m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
        }
        else {
            if(bEmi)
                m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
            else
                m_pShaderCom->Begin((_uint)MESH_PASS::Default);
        }
        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

void CJeongGimmicBaseDefault::Render_Reflection()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
    m_pGameInstance.lock()->Bind_RT_SRV(L"RT_Albedo", m_pShaderCom, "g_Texture");
    m_pGameInstance.lock()->Bind_RT_SRV(L"RT_Position", m_pShaderCom, "g_ATOS_Texture");
    _float2 vResolution = _float2{ (_float)g_iWinSizeX, (_float)g_iWinSizeY };
    m_pShaderCom->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2));
    m_pShaderCom->Bind_Float("g_OutlineWidth", 0.1f);
    //m_pShaderCom->Bind_Matrix("g_ViewInv", &m_pGameInstance.lock()->Get_Transform_Float4x4(CPipeLine::PIPELINE_STATE::TS_VIEW_INV));
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pShaderCom->Begin((_uint)MESH_PASS::Reflection);
        m_pModelCom->Render((_uint)i);
    }
}

shared_ptr<CJeongGimmicBaseDefault> CJeongGimmicBaseDefault::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CJeongGimmicBaseDefault
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CJeongGimmicBaseDefault(pDevice, pContext)
        {}
    };

    shared_ptr<CJeongGimmicBaseDefault> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CJeongGimmicBaseDefault"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CJeongGimmicBaseDefault::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CJeongGimmicBaseDefault
    {
        MakeSharedEnabler(const CJeongGimmicBaseDefault& rhs) : CJeongGimmicBaseDefault(rhs)
        {}
    };

    shared_ptr<CJeongGimmicBaseDefault> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CJeongGimmicBaseDefault::Free()
{
}
