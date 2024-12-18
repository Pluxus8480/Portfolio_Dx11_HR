#include "Rekka_LightShaft.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "MapManager.h"

wstring CRekka_LightShaft::ObjID = TEXT("CRekka_LightShaft");
CRekka_LightShaft::CRekka_LightShaft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CRekka_LightShaft::CRekka_LightShaft(const CRekka_LightShaft& rhs)
	: CGameObject{rhs}
{
}

CRekka_LightShaft::~CRekka_LightShaft()
{
	Free();
}

HRESULT CRekka_LightShaft::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Rekka_LightShaftA"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_LightShaftA_SH_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Rekka_LightShaftB"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_LightShaftB_SH_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Rekka_LightShaftC"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_LightShaftC_SH_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	
	return S_OK;
}

HRESULT CRekka_LightShaft::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
	
	return S_OK;
}

void CRekka_LightShaft::PriorityTick(_float fTimeDelta)
{
}

void CRekka_LightShaft::Tick(_float fTimeDelta)
{
}

void CRekka_LightShaft::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	//if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_LENS, shared_from_this());
}

HRESULT CRekka_LightShaft::Render()
{
	if (!m_pModelCom) return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	_float4 vEffectBaseColor = { 0.01f, 0.51f, 0.94f, 0.8f };

	//if (m_iModelIndex == 0)
	//	vEffectBaseColor = m_vPhaseColor[(_uint)m_iCurPhase];
	//if (m_iModelIndex == 1) vEffectBaseColor = { 0.72f,0.55f,0.97f, 0.8f };
	//if (m_iModelIndex == 2) vEffectBaseColor = { 0.76f, 0.56f, 0.98f, 0.8f };
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

void CRekka_LightShaft::Edit_Attribute()
{
	ImGui::SliderInt("Model Index", &m_iModelIndex, 0, 2);

	if (ImGui::Button("Create Model"))
	{
		m_pModelCom = nullptr;
		Create_Model();
	}
}

void CRekka_LightShaft::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	write_typed_data(os, m_iModelIndex);
}

void CRekka_LightShaft::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	read_typed_data(is, m_iModelIndex);

	Create_Model();
}

HRESULT CRekka_LightShaft::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_T_VFX_tk_Light_01"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pTextureCom)
		assert(false);

	return S_OK;
}

HRESULT CRekka_LightShaft::Bind_ShaderResources()
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

void CRekka_LightShaft::Create_Model()
{
	switch (m_iModelIndex)
	{
	case 0:
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MODELTEST, TEXT("Prototype_Component_Model_Rekka_LightShaftA"), TEXT("Model1_Com")));
		break;
	case 1:
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MODELTEST, TEXT("Prototype_Component_Model_Rekka_LightShaftB"), TEXT("Model2_Com")));
		break;
	case 2:
		m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_MODELTEST, TEXT("Prototype_Component_Model_Rekka_LightShaftC"), TEXT("Model3_Com")));
		break;
	}

	assert(m_pModelCom);
}

shared_ptr<CRekka_LightShaft> CRekka_LightShaft::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRekka_LightShaft
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_LightShaft(pDevice, pContext)
		{}
	};

	shared_ptr<CRekka_LightShaft> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_LightShaft"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CRekka_LightShaft::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_LightShaft
	{
		MakeSharedEnabler(const CRekka_LightShaft& rhs) : CRekka_LightShaft(rhs)
		{}
	};

	shared_ptr<CRekka_LightShaft> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_LightShaft"));
		assert(false);
	}

	return pInstance;
}

void CRekka_LightShaft::Free()
{
}
