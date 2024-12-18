#include "Spectra_FanBlade.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

wstring CSpectra_FanBlade::ObjID = TEXT("CSpectra_FanBlade");
CSpectra_FanBlade::CSpectra_FanBlade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{pDevice, pContext}
{
}

CSpectra_FanBlade::CSpectra_FanBlade(const CSpectra_FanBlade& rhs)
	: CPartObject{rhs}
{
}

CSpectra_FanBlade::~CSpectra_FanBlade()
{
	Free();
}

HRESULT CSpectra_FanBlade::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Spectra_FanBlade"), 
			TEXT("../Bin/Resources/Models/Map/Stage_Kale/SM_ACUnitFanBlade_IZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	return S_OK;
}

HRESULT CSpectra_FanBlade::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);
	pDesc->fSpeedPerSec = 0.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CSpectra_FanBlade::PriorityTick(_float fTimeDelta)
{
}

void CSpectra_FanBlade::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_NormLook(), fTimeDelta);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CSpectra_FanBlade::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CSpectra_FanBlade::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CSpectra_FanBlade::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Spectra_FanBlade"), TEXT("Model_Com")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CSpectra_FanBlade::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CSpectra_FanBlade> CSpectra_FanBlade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSpectra_FanBlade
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSpectra_FanBlade(pDevice, pContext)
		{}
	};

	shared_ptr<CSpectra_FanBlade> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpectra_FanBlade"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CSpectra_FanBlade::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSpectra_FanBlade
	{
		MakeSharedEnabler(const CSpectra_FanBlade& rhs) : CSpectra_FanBlade(rhs)
		{}
	};


	shared_ptr<CSpectra_FanBlade> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpectra_FanBlade"));
		assert(false);
	}

	return pInstance;
}

void CSpectra_FanBlade::Free()
{
}
