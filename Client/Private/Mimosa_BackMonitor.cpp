#include "Mimosa_BackMonitor.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Misc_Interface.h"

wstring CMimosa_BackMonitor::ObjID = TEXT("CMimosa_BackMonitor");
CMimosa_BackMonitor::CMimosa_BackMonitor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMimosa_BackMonitor::CMimosa_BackMonitor(const CMimosa_BackMonitor& rhs)
	: CGameObject{ rhs }
{
}

CMimosa_BackMonitor::~CMimosa_BackMonitor()
{
	Free();
}

void CMimosa_BackMonitor::Change_Phase(_uint iPhase)
{
	m_iCurPhase = iPhase;
}

HRESULT CMimosa_BackMonitor::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Mimosa_BackMonitor"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_MonitorA_TJ_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

	return S_OK;
}

HRESULT CMimosa_BackMonitor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
	m_iCurPhase = (_uint)MIMOSA_PHASE::PHASE_1;

	return S_OK;
}

void CMimosa_BackMonitor::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_BackMonitor::Tick(_float fTimeDelta)
{

	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAccMax < m_fTimeAcc)
		m_fTimeAcc = 0.f;
}

void CMimosa_BackMonitor::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	if (!pGI->isIn_Frustum_WorldSpace(XMVectorSet(-52.f, 15.f, 0.f, 1.f), 30.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_LENS, shared_from_this());
}

HRESULT CMimosa_BackMonitor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmissive = { false };

		if (i == 0) {
			m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Texture");
			m_pGameInstance.lock()->Bind_RT_SRV(L"RT_BeforeBack", m_pShaderCom, "g_Emission_Texture");

			m_pShaderCom->Bind_Float("g_fzoomFactor", m_fTimeAcc/5.f);


			bEmissive = true;
		}else{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				bEmissive = false;
			else
				bEmissive = true;
		}

		if (bEmissive) {
			if(i == 0) m_pShaderCom->Begin((_uint)MESH_PASS::EmissionMoving);
			else m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		}
		else {
			m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CMimosa_BackMonitor::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_BackMonitor"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CMimosa_BackMonitor::Bind_ShaderResources()
{
	auto pGI = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

shared_ptr<CMimosa_BackMonitor> CMimosa_BackMonitor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_BackMonitor
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_BackMonitor(pDevice, pContext) {}
	};

	shared_ptr<CMimosa_BackMonitor> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_BackMonitor"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CMimosa_BackMonitor::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_BackMonitor
	{
		MakeSharedEnabler(const CMimosa_BackMonitor& rhs) : CMimosa_BackMonitor(rhs) {}
	};

	shared_ptr<CMimosa_BackMonitor> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_BackMonitor"));
		assert(false);
	}
	return pInstance;
}

void CMimosa_BackMonitor::Free()
{
}
