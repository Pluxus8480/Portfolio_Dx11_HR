#include "Mimosa_RoofLEDText.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Misc_Interface.h"

wstring CMimosa_RoofLEDText::ObjID = TEXT("CMimosa_RoofLEDText");
CMimosa_RoofLEDText::CMimosa_RoofLEDText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMimosa_RoofLEDText::CMimosa_RoofLEDText(const CMimosa_RoofLEDText& rhs)
	: CGameObject{ rhs }
{
}

CMimosa_RoofLEDText::~CMimosa_RoofLEDText()
{
	Free();
}

void CMimosa_RoofLEDText::Change_Phase(_uint iPhase)
{
	m_iCurPhase = iPhase;
}

HRESULT CMimosa_RoofLEDText::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Mimosa_RoofLEDText"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_TextB_TJ_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

	return S_OK;
}

HRESULT CMimosa_RoofLEDText::Initialize(void* pArg)
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

void CMimosa_RoofLEDText::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_RoofLEDText::Tick(_float fTimeDelta)
{
}

void CMimosa_RoofLEDText::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	//if (!pGI->isIn_Frustum_WorldSpace(XMVectorSet(-52.f, 15.f, 0.f, 1.f), 30.f)) return;
	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
}

HRESULT CMimosa_RoofLEDText::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (i == 1)
		{
			m_pShaderCom->Bind_Vector4("g_vColor", &m_vDiffuseColor[m_iCurPhase]);
			m_pShaderCom->Begin((_uint)MESH_PASS::DiffuseColor);
		}
		else if (i == 0)
		{
			_float4 vColor = { 0.5f,1.f, 1.f, 1.f };
			m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
			m_pShaderCom->Begin((_uint)MESH_PASS::DiffuseColor);
		}
		else
		{
			if (SUCCEEDED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CMimosa_RoofLEDText::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_RoofLEDText"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_1] = { 0.78f, 0.24f, 0.99f, 1.f };
	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_2] = { 0.f, 0.f, 0.f, 1.f };
	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_3] = { 0.314f, 0.694f, 0.275f, 1.f };
	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_4] = { 0.741f, 0.424f, 0.765f, 1.f };

	return S_OK;
}

HRESULT CMimosa_RoofLEDText::Bind_ShaderResources()
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

shared_ptr<CMimosa_RoofLEDText> CMimosa_RoofLEDText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_RoofLEDText
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_RoofLEDText(pDevice, pContext) {}
	};

	shared_ptr<CMimosa_RoofLEDText> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_RoofLEDText"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CMimosa_RoofLEDText::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_RoofLEDText
	{
		MakeSharedEnabler(const CMimosa_RoofLEDText& rhs) : CMimosa_RoofLEDText(rhs) {}
	};

	shared_ptr<CMimosa_RoofLEDText> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_RoofLEDText"));
		assert(false);
	}
	return pInstance;
}

void CMimosa_RoofLEDText::Free()
{
}
