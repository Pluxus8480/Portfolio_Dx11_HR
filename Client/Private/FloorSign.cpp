#include "FloorSign.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "MapObject_Static.h"

#include "MapManager.h"

wstring CFloorSign::ObjID = TEXT("CFloorSign");
CFloorSign::CFloorSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CFloorSign::CFloorSign(const CFloorSign& rhs)
	: CGameObject{ rhs }
{
}

CFloorSign::~CFloorSign()
{
	Free();
}

HRESULT CFloorSign::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SignPlane"), TEXT("../Bin/Resources/Models/Map/General/SM_SignsMeshPlane_IZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_FloorSign_1F"), TEXT("../Bin/Resources/Models/Map/General/t_proddeptsignsak_01.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_FloorSign_2F"), TEXT("../Bin/Resources/Models/Map/General/t_proddeptsignsal_01.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_FloorSign_3F"), TEXT("../Bin/Resources/Models/Map/General/t_proddeptsignsam_01.dds"))
	);

	return S_OK;
}

HRESULT CFloorSign::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CFloorSign::PriorityTick(_float fTimeDelta)
{
}

void CFloorSign::Tick(_float fTimeDelta)
{
}

void CFloorSign::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;
	
	auto pGI = m_pGameInstance.lock();
	if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CFloorSign::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//m_pTextureCom[m_iNumFloor]->Bind_ShaderResource(m_pShaderCom, "g_Texture");
	//m_pShaderCom->Begin(26);
	//m_pVIBufferCom->Render();

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		m_pTextureCom[m_iNumFloor]->Bind_ShaderResource(m_pShaderCom, "g_Texture");
		m_pShaderCom->Begin(0);
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CFloorSign::Edit_Attribute()
{
	ImGui::InputInt("Floor Num", &m_iNumFloor);
}

void CFloorSign::Save_Data(ofstream& os)
{
	write_typed_data(os, m_iNumFloor);
}

void CFloorSign::Load_Data(ifstream& is)
{
	read_typed_data(is, m_iNumFloor);
}

HRESULT CFloorSign::Ready_Components()
{
	//m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Shader_Com")));
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	//m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("VIBuffer_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SignPlane"), TEXT("Model_Com")));
	m_pTextureCom[FIRST] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FloorSign_1F"), TEXT("1FTexture_Com")));
	m_pTextureCom[SECOND] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FloorSign_2F"), TEXT("2FTexture_Com")));
	m_pTextureCom[THIRD] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FloorSign_3F"), TEXT("3FTexture_Com")));

	if (!m_pShaderCom || /*!m_pVIBufferCom || */!m_pTextureCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CFloorSign::Bind_ShaderResources()
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

shared_ptr<CFloorSign> CFloorSign::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFloorSign
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFloorSign(pDevice, pContext)
		{}
	};

	shared_ptr<CFloorSign> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFloorSign"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CFloorSign::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFloorSign
	{
		MakeSharedEnabler(const CFloorSign& rhs) : CFloorSign(rhs)
		{}
	};

	shared_ptr<CFloorSign> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFloorSign"));
		assert(false);
	}

	return pInstance;
}

void CFloorSign::Free()
{
}
