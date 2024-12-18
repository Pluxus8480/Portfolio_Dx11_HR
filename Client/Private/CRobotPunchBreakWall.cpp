#include "CRobotPunchBreakWall.h"
#include "Cine_Manager.h"


wstring CRobotPunchBreakWall::ObjID = TEXT("CRobotPunchBreakWall");

CRobotPunchBreakWall::CRobotPunchBreakWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CJeongGimmicBase(pDevice, pContext)
{
}

CRobotPunchBreakWall::CRobotPunchBreakWall(const CRobotPunchBreakWall& rhs)
	:CJeongGimmicBase(rhs)
{
}

CRobotPunchBreakWall::~CRobotPunchBreakWall()
{
}

HRESULT CRobotPunchBreakWall::Initialize_Prototype()
{
    _float4x4 fMat;
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_CRobotPunchBreakWall",
            L"../Bin/Resources/Models/Map/Gimmick/CRobotPunchBreakWall.fbx",
            MODEL_TYPE::ANIM, fMat)
    );

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(L"Prototype_Component_Model_CRobotPunchSurroundWall",
            L"../Bin/Resources/Models/Map/Gimmick/CRobotPunchSurroundWall.fbx",
            MODEL_TYPE::ANIM, fMat)
    );

	return S_OK;
}

HRESULT CRobotPunchBreakWall::Initialize(void* pArg)
{
    Initialize_Transform();
    Desc* Arg = reinterpret_cast<Desc*>(pArg);
    m_pTransformCom->Set_WorldMatrix(Arg->worldMatrix);
    InitializeBasicComponent(L"Prototype_Component_Model_CRobotPunchBreakWall",
        L"Prototype_Component_Shader_VtxAnimMesh");

    m_pSurroundModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC,
        L"Prototype_Component_Model_CRobotPunchSurroundWall", L"AfterSurroundModel_Com"));

    CCine_Manager::Get_Instance()->AddLastCallback
    ("CS1125", [&]() { m_bRender = true; });
    
    m_pModelCom->Set_AnimIndex(0, true);
    m_pSurroundModelCom->Set_AnimIndex(0, true);

	return S_OK;
}

void CRobotPunchBreakWall::PriorityTick(_float fTimeDelta)
{
}

void CRobotPunchBreakWall::Tick(_float fTimeDelta)
{  
}

void CRobotPunchBreakWall::LateTick(_float fTimeDelta)
{
    _float4x4 xMat = {};
    m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, xMat);
    m_pSurroundModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, xMat);

    auto pGI = m_pGameInstance.lock();
    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fModelRange))
        return;

    if (m_bRender)
    {
        pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());
    }
}

HRESULT CRobotPunchBreakWall::Render()
{
    if (FAILED(Bind_ShaderResources()))
        assert(false);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);
		m_pModelCom->Render((_uint)i);
	}

	iNumMeshes = m_pSurroundModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pSurroundModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		m_pSurroundModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);
		m_pSurroundModelCom->Render((_uint)i);
	}

    return S_OK;
}

shared_ptr<CRobotPunchBreakWall> CRobotPunchBreakWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRobotPunchBreakWall
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRobotPunchBreakWall(pDevice, pContext)
        {}
    };

    shared_ptr<CRobotPunchBreakWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CRobotPunchBreakWall::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CRobotPunchBreakWall
    {
        MakeSharedEnabler(const CRobotPunchBreakWall& rhs) : CRobotPunchBreakWall(rhs)
        {}
    };

    shared_ptr<CRobotPunchBreakWall> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        assert(false);
    }

    return pInstance;
}

void CRobotPunchBreakWall::Free()
{

}


