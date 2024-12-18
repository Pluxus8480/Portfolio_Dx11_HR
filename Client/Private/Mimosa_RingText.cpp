#include "Mimosa_RingText.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Misc_Interface.h"

wstring CMimosa_RingText::ObjID = TEXT("CMimosa_RingText");
CMimosa_RingText::CMimosa_RingText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMimosa_RingText::CMimosa_RingText(const CMimosa_RingText& rhs)
	: CGameObject{ rhs }
{
}

CMimosa_RingText::~CMimosa_RingText()
{
	Free();
}

void CMimosa_RingText::Change_Phase(_uint iPhase)
{
	m_iCurPhase = iPhase;
}

HRESULT CMimosa_RingText::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Mimosa_RingTextA"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_RingA_TJ_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Mimosa_RingTextB"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_RingB_TJ_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Mimosa_RingText_01"), 
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/T_MktgDBossStage_RingSignsA_TJ_01.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Mimosa_RingText_02"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/T_MktgDBossStage_RingSignsA_TJ_02.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Mimosa_RingText_03"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/T_MktgDBossStage_RingSignsA_TJ_03.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Mimosa_RingText_04"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/T_MktgDBossStage_RingSignsA_TJ_04.dds"))
	);

	return S_OK;
}

HRESULT CMimosa_RingText::Initialize(void* pArg)
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

void CMimosa_RingText::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_RingText::Tick(_float fTimeDelta)
{
	m_fUvFlow += fTimeDelta * 0.05f;

	if (m_fUvFlow > 1.f)
		m_fUvFlow = 0.f;
}

void CMimosa_RingText::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	//if (!pGI->isIn_Frustum_WorldSpace(XMVectorSet(-52.f, 15.f, 0.f, 1.f), 30.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMimosa_RingText::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	_float2 vFlow{ m_fUvFlow, 0.f };
	m_pShaderCom->Bind_RawValue("g_vUVFlow", &vFlow, sizeof(_float2));

	_uint	iNumMeshes = m_pBackModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmissive = { false };

		if (i == 0)
		{
			m_pTextureCom[m_iCurPhase]->Bind_ShaderResource(m_pShaderCom, "g_Texture");
			m_pTextureCom[m_iCurPhase]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture");
			//m_pBackModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE);
			m_pShaderCom->Begin((_uint)MESH_PASS::BeltEmission);
		}
		else
		{
			if (FAILED(m_pBackModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(m_pBackModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		}

		//if (bEmissive)
		//	m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		//else
		//	m_pShaderCom->Begin((_uint)MESH_PASS::Default);

		m_pBackModelCom->Render((_uint)i);
	}

	iNumMeshes = m_pRoofModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmissive = { false };

		if (i == 0)
		{
			m_pTextureCom[m_iCurPhase]->Bind_ShaderResource(m_pShaderCom, "g_Texture");
			m_pTextureCom[m_iCurPhase]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture");
			//m_pRoofModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE);
			m_pShaderCom->Begin((_uint)MESH_PASS::BeltEmission);
		}
		else
		{
			if (FAILED(m_pRoofModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(m_pRoofModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		}

		//if (bEmissive)
		//	m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		//else
		//	m_pShaderCom->Begin((_uint)MESH_PASS::Default);

		m_pRoofModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CMimosa_RingText::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pBackModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_RingTextA"), TEXT("Com_Model01")));
	m_pRoofModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_RingTextB"), TEXT("Com_Model02")));

	if (!m_pShaderCom || !m_pBackModelCom || !m_pRoofModelCom)
		assert(false);

	m_pTextureCom[0] = static_pointer_cast<CTexture>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Texture_Mimosa_RingText_01"), TEXT("Texture0_Com")));
	m_pTextureCom[1] = static_pointer_cast<CTexture>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Texture_Mimosa_RingText_02"), TEXT("Texture1_Com")));
	m_pTextureCom[2] = static_pointer_cast<CTexture>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Texture_Mimosa_RingText_03"), TEXT("Texture2_Com")));
	m_pTextureCom[3] = static_pointer_cast<CTexture>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Texture_Mimosa_RingText_04"), TEXT("Texture3_Com")));

	return S_OK;
}

HRESULT CMimosa_RingText::Bind_ShaderResources()
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

shared_ptr<CMimosa_RingText> CMimosa_RingText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_RingText
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_RingText(pDevice, pContext) {}
	};

	shared_ptr<CMimosa_RingText> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_RingText"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CMimosa_RingText::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_RingText
	{
		MakeSharedEnabler(const CMimosa_RingText& rhs) : CMimosa_RingText(rhs) {}
	};

	shared_ptr<CMimosa_RingText> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_RingText"));
		assert(false);
	}
	return pInstance;
}

void CMimosa_RingText::Free()
{
}
