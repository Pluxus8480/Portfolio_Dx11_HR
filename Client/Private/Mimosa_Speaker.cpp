#include "Mimosa_Speaker.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"

wstring CMimosa_Speaker::ObjID = TEXT("CMimosa_Speaker");
CMimosa_Speaker::CMimosa_Speaker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CMimosa_Speaker::CMimosa_Speaker(const CMimosa_Speaker& rhs)
	: CGameObject{rhs}
{
}

CMimosa_Speaker::~CMimosa_Speaker()
{
	Free();
}

HRESULT CMimosa_Speaker::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_MimosaSpeaker"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_SpeakerA_TJ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	return S_OK;
}

HRESULT CMimosa_Speaker::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	return S_OK;
}

void CMimosa_Speaker::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_Speaker::Tick(_float fTimeDelta)
{
	_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurBeat)
	{
		m_iPrevBeat = iCurBeat;
		m_bLightOn = true;
	}

	if (m_bLightOn)
	{
		m_fLightTime += fTimeDelta;

		if (m_fLightTime > 0.1f)
		{
			m_fLightTime -= m_fLightTime;
			m_bLightOn = false;
		}
	}
}

void CMimosa_Speaker::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	//if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMimosa_Speaker::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (m_bLightOn && i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				return E_FAIL;

			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		}
		else
			m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CMimosa_Speaker::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_MimosaSpeaker"), TEXT("Model_Com")));
	
	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CMimosa_Speaker::Bind_ShaderResources()
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

shared_ptr<CMimosa_Speaker> CMimosa_Speaker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_Speaker
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_Speaker(pDevice, pContext)
		{}
	};

	shared_ptr<CMimosa_Speaker> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_Speaker"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMimosa_Speaker::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_Speaker
	{
		MakeSharedEnabler(const CMimosa_Speaker& rhs) : CMimosa_Speaker(rhs)
		{}
	};

	shared_ptr<CMimosa_Speaker> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_Speaker"));
		assert(false);
	}

	return pInstance;
}

void CMimosa_Speaker::Free()
{
}
