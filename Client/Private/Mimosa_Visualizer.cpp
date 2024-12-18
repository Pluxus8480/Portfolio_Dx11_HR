#include "Mimosa_Visualizer.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Misc_Interface.h"

wstring CMimosa_Visualizer::ObjID = TEXT("CMimosa_Visualizer");
CMimosa_Visualizer::CMimosa_Visualizer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMimosa_Visualizer::CMimosa_Visualizer(const CMimosa_Visualizer& rhs)
	: CGameObject{ rhs }
{
}

CMimosa_Visualizer::~CMimosa_Visualizer()
{
	Free();
}

void CMimosa_Visualizer::Change_Phase(_uint iPhase)
{
	m_iCurPhase = iPhase;
}

HRESULT CMimosa_Visualizer::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Mimosa_Visualizer"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_VisualizerA_TJ_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

	return S_OK;
}

HRESULT CMimosa_Visualizer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
	m_iCurPhase = (_uint)MIMOSA_PHASE::PHASE_1;

	for (int i = 0; i < m_iMaxX; i++)
		m_iVisualizerXArray.push_back(0);

	for (int i = 0; i < m_iMaxX; i++) {
		m_iVisualizerArray.push_back(vector<_int>(m_iMaxY, 0));
	}

	return S_OK;
}

void CMimosa_Visualizer::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_Visualizer::Tick(_float fTimeDelta)
{
	Visualizer_Update();
}

void CMimosa_Visualizer::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	//if (!pGI->isIn_Frustum_WorldSpace(XMVectorSet(-52.f, 15.f, 0.f, 1.f), 30.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
}

HRESULT CMimosa_Visualizer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmissive = { false };
		if (i == 0)
		{
			//if (FAILED(m_pShaderCom->Bind_RawValue("g_RandomValues", m_iVisualizerXArray.data(), sizeof(_int) * m_iMaxX)))
			//	return E_FAIL;


			if (FAILED(m_pShaderCom->Bind_ScalarIntValue("g_RandomValues", m_iVisualizerXArray.data(), sizeof(_int), m_iMaxX)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fBeatRate", &m_fBeatRate, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			_float4 vColor = { 1.f,1.f, 0.8f, 1.f };
			m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
			m_pShaderCom->Begin((_uint)MESH_PASS::Visualizer);
		}
		else
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
 				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				bEmissive = false;
			else
				bEmissive = true;

			if (bEmissive)
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CMimosa_Visualizer::Visualizer_Update()
{
	Visualizer_Init();

	if (m_fBeatRate > BEATRATE) m_bBeatInit = true;
	m_fBeatRate = BEATRATE;

	if (m_bBeatInit) {
		for (auto& value : m_iVisualizerXArray) {
			value = GAMEINSTANCE->Random_Int(0, 7);
		}
	}


}

void CMimosa_Visualizer::Visualizer_Init()
{
	for (auto& value : m_iVisualizerArray) {
		for (auto& value2 : value) {
			value2 = 0;
		}
	}

	for (auto& value : m_iVisualizerXArray) {
		value = 0;
	}
}

HRESULT CMimosa_Visualizer::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_Visualizer"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CMimosa_Visualizer::Bind_ShaderResources()
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

shared_ptr<CMimosa_Visualizer> CMimosa_Visualizer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_Visualizer
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_Visualizer(pDevice, pContext) {}
	};

	shared_ptr<CMimosa_Visualizer> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_Visualizer"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CMimosa_Visualizer::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_Visualizer
	{
		MakeSharedEnabler(const CMimosa_Visualizer& rhs) : CMimosa_Visualizer(rhs) {}
	};

	shared_ptr<CMimosa_Visualizer> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_Visualizer"));
		assert(false);
	}
	return pInstance;
}

void CMimosa_Visualizer::Free()
{
}
