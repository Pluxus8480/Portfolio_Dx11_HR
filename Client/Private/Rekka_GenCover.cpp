#include "Rekka_GenCover.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#include "MapManager.h"


wstring CRekka_GenCover::ObjID = TEXT("CRekka_GenCover");
CRekka_GenCover::CRekka_GenCover(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CRekka_GenCover::CRekka_GenCover(const CRekka_GenCover& rhs)
    : CGameObject{rhs}
{
}

CRekka_GenCover::~CRekka_GenCover()
{
    Free();
}

HRESULT CRekka_GenCover::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, /*XMMatrixScaling(0.01f, 0.01f, 0.01f) * */XMMatrixRotationY(XMConvertToRadians(-90.f)));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Rekka_GenCover"),
			TEXT("../Bin/Resources/Models/Map/Gimmick/Rekka_GenCover.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Rekka_GenCover_Crushed"),
			TEXT("../Bin/Resources/Models/Map/Gimmick/Rekka_GenCover_Crushed.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

    return S_OK;
}

HRESULT CRekka_GenCover::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
	m_iCurPhase = (_uint)REKKA_PHASE::PHASE_1;

    return S_OK;
}

void CRekka_GenCover::PriorityTick(_float fTimeDelta)
{
}

void CRekka_GenCover::Tick(_float fTimeDelta)
{
}

void CRekka_GenCover::LateTick(_float fTimeDelta)
{
	if (m_iCurPhase != (_uint)REKKA_PHASE::PHASE_1)
		return;

	auto pGI = m_pGameInstance.lock();

	if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 5.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRekka_GenCover::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	auto pModel = (m_bCrushed) ? m_pCrushedModel : m_pModelCom;

	_uint	iNumMeshes = pModel->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		pModel->Render((_uint)i);
	}

    return S_OK;
}

HRESULT CRekka_GenCover::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rekka_GenCover"), TEXT("Com_Model")));
	m_pCrushedModel = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rekka_GenCover_Crushed"), TEXT("Com_CrushedModel")));

	if (!m_pShaderCom || !m_pModelCom || !m_pCrushedModel)
		assert(false);

    return S_OK;
}

HRESULT CRekka_GenCover::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

    return S_OK;
}

shared_ptr<CRekka_GenCover> CRekka_GenCover::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CRekka_GenCover
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_GenCover(pDevice, pContext) {}
	};

	shared_ptr<CRekka_GenCover> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_GenCover"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CRekka_GenCover::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_GenCover
	{
		MakeSharedEnabler(const CRekka_GenCover& rhs) : CRekka_GenCover(rhs) {}
	};

	shared_ptr<CRekka_GenCover> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_GenCover"));
		assert(false);
	}
	return pInstance;
}

void CRekka_GenCover::Free()
{
}
