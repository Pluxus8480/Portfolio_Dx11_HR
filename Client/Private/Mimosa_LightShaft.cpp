#include "Mimosa_LightShaft.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"

wstring CMimosa_LightShaft::ObjID = TEXT("CMimosa_LightShaft");
CMimosa_LightShaft::CMimosa_LightShaft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CMimosa_LightShaft::CMimosa_LightShaft(const CMimosa_LightShaft& rhs)
	: CGameObject{rhs}
{
}

CMimosa_LightShaft::~CMimosa_LightShaft()
{
	Free();
}

HRESULT CMimosa_LightShaft::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_LightShaftA"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_LightShaftA_TJ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_LightShaftB"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_LightShaftB_TJ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_LightShaftD"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_LightShaftD_TJ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_LightShaftE"),
			TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/SM_MktgDBossStage_LightShaftE_TJ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	return S_OK;
}

HRESULT CMimosa_LightShaft::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CMimosa_LightShaft::PriorityTick(_float fTimeDelta)
{
}

void CMimosa_LightShaft::Tick(_float fTimeDelta)
{
}

void CMimosa_LightShaft::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	//if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_LENS, shared_from_this());
}

HRESULT CMimosa_LightShaft::Render()
{
	if (!m_pModelCom) return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	_float4 vEffectBaseColor = { 0.93f,0.85f,0.58f, 0.8f };

	if (m_iModelIndex == 1) vEffectBaseColor = { 0.72f,0.55f,0.97f, 0.8f };
	if (m_iModelIndex == 2) vEffectBaseColor = { 0.76f, 0.56f, 0.98f, 0.8f };
	m_pShaderCom->Bind_RawValue("g_vEffectBaseColor", &vEffectBaseColor, sizeof(_float4));

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		m_pShaderCom->Begin(static_cast<_uint>(MESH_PASS::ShaftLight));

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CMimosa_LightShaft::Edit_Attribute()
{
	ImGui::SliderInt("Model Index", &m_iModelIndex, 0, 3);

	if (ImGui::Button("Create Model"))
	{
		m_pModelCom = nullptr;
		Create_Model();
	}
}

void CMimosa_LightShaft::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	write_typed_data(os, m_iModelIndex);
}

void CMimosa_LightShaft::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	read_typed_data(is, m_iModelIndex);

	Create_Model();
}

HRESULT CMimosa_LightShaft::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_tk_Light_01"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

HRESULT CMimosa_LightShaft::Bind_ShaderResources()
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

void CMimosa_LightShaft::Create_Model()
{
	switch (m_iModelIndex)
	{
	case 0:
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_LightShaftA"), TEXT("Model_Com")));
		break;
	case 1:
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_LightShaftB"), TEXT("Model_Com")));
		break;
	case 2:
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_LightShaftD"), TEXT("Model_Com")));
		break;
	case 3:
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Mimosa_LightShaftE"), TEXT("Model_Com")));
		break;
	}

	assert(m_pModelCom);
}

shared_ptr<CMimosa_LightShaft> CMimosa_LightShaft::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMimosa_LightShaft
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMimosa_LightShaft(pDevice, pContext)
		{}
	};

	shared_ptr<CMimosa_LightShaft> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMimosa_LightShaft"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMimosa_LightShaft::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMimosa_LightShaft
	{
		MakeSharedEnabler(const CMimosa_LightShaft& rhs) : CMimosa_LightShaft(rhs)
		{}
	};

	shared_ptr<CMimosa_LightShaft> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMimosa_LightShaft"));
		assert(false);
	}

	return pInstance;
}

void CMimosa_LightShaft::Free()
{
}
