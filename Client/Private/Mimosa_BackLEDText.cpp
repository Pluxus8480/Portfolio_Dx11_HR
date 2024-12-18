#include "Mimosa_BackLEDText.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Misc_Interface.h"

wstring CMimosa_BackLEDText::ObjID = TEXT("CMimosa_BackLEDText");
CMimosa_BackLEDText::CMimosa_BackLEDText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMimosa_BackLEDText::CMimosa_BackLEDText(const CMimosa_BackLEDText& rhs)
	: CGameObject{ rhs }
{
}

CMimosa_BackLEDText::~CMimosa_BackLEDText()
{
	Free();
}

void CMimosa_BackLEDText::Change_Phase(_uint iPhase)
{
	m_iCurPhase = iPhase;
}

void CMimosa_BackLEDText::Change_Pattern(_uint iPattern)
{
	m_iCurPattern = iPattern;
}

HRESULT CMimosa_BackLEDText::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Mimosa_BackLEDText"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_TextA_TJ_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);
	
	return S_OK;
}

HRESULT CMimosa_BackLEDText::Initialize(void* pArg)
{
	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	pDesc->fSpeedPerSec = 3.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
	m_iCurPhase = (_uint)MIMOSA_PHASE::PHASE_1;
	m_iCurPattern = (_uint)MIMOSA_PATTERN::IDLE;

	return S_OK;
}

void CMimosa_BackLEDText::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_BackLEDText::Tick(_float fTimeDelta)
{
	_vector vCurPos = m_pTransformCom->Get_Position();

	if (m_iCurPattern == (_uint)MIMOSA_PATTERN::DANCEBATTLE && !m_bHided)
	{
		if (XMVectorGetY(vCurPos) <= -10.f)
		{
			m_pTransformCom->Set_Position(_float4{ 0.f, -10.f, 0.f, 1.f });
			m_bHided = true;
		}
		else
			m_pTransformCom->Go_Down(fTimeDelta);
	}

	else if (m_bHided && m_iCurPattern == (_uint)MIMOSA_PATTERN::IDLE)
	{
		if (XMVectorGetY(vCurPos) >= 0.f)
		{
			m_pTransformCom->Set_Position(_float4{ 0.f, 0.f, 0.f, 1.f });
			m_bHided = false;
		}
		else
			m_pTransformCom->Go_Up(fTimeDelta);
	}
}

void CMimosa_BackLEDText::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	//if (!pGI->isIn_Frustum_WorldSpace(XMVectorSet(-52.f, 15.f, 0.f, 1.f), 30.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
}

HRESULT CMimosa_BackLEDText::Render()
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
		else if (i == 2)
		{
			_float4 vColor = { 1.f,1.f, 0.8f, 1.f };
			m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
			m_pShaderCom->Begin((_uint)MESH_PASS::DiffuseColor);
		}else
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

HRESULT CMimosa_BackLEDText::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_BackLEDText"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_1] = { 0.69f, 0.39f, 0.68f, 1.f };
	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_2] = { 0.f, 0.f, 0.f, 1.f };
	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_3] = { 1.f, 1.f, 1.f, 1.f };
	m_vDiffuseColor[(_uint)MIMOSA_PHASE::PHASE_4] = { 0.647f, 0.349f, 0.765f, 1.f };

	return S_OK;
}

HRESULT CMimosa_BackLEDText::Bind_ShaderResources()
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

shared_ptr<CMimosa_BackLEDText> CMimosa_BackLEDText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_BackLEDText
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_BackLEDText(pDevice, pContext) {}
	};

	shared_ptr<CMimosa_BackLEDText> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_BackLEDText"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CMimosa_BackLEDText::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_BackLEDText
	{
		MakeSharedEnabler(const CMimosa_BackLEDText& rhs) : CMimosa_BackLEDText(rhs) {}
	};

	shared_ptr<CMimosa_BackLEDText> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_BackLEDText"));
		assert(false);
	}
	return pInstance;
}

void CMimosa_BackLEDText::Free()
{
}
