#include "SkySphere.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

wstring CSkySphere::ObjID = TEXT("CSkySphere");


CSkySphere::CSkySphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSkySphere::CSkySphere(const CSkySphere& rhs)
	: CGameObject(rhs)
{
}

CSkySphere::~CSkySphere()
{
	Free();
}

HRESULT CSkySphere::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(10.00f, 10.00f, 10.00f));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_SkySphere"), TEXT("../Bin/Resources/Models/SkySphere/T_SkyBox_RoxanneBlueSkyAndCloud.png"), 1));

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_SkySphere_Kale"), TEXT("../Bin/Resources/Models/SkySphere/T_SkyBox_St12CloudySkyA.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_SkySphere_KaleCloud"), TEXT("../Bin/Resources/Models/SkySphere/T_SkyBox_St12CloudySkyB.png"), 1));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SkySphere"), TEXT("../Bin/Resources/Models/SkySphere/SkySphere.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	
	return S_OK;
}

HRESULT CSkySphere::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CSkySphere::PriorityTick(_float fTimeDelta)
{
	m_eLevel = (LEVEL)m_pGameInstance.lock()->Get_Current_LevelID();
}

void CSkySphere::Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance.lock()->Get_CamPosition_Vector());

}

void CSkySphere::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_PRIORITY, shared_from_this());
}

HRESULT CSkySphere::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if(m_eLevel != LEVEL_KALEBOSS && m_eLevel != LEVEL_MIMOSA)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
				continue;
			m_pShaderCom->Begin((_uint)MESH_PASS::SKY);
			m_pModelCom->Render((_uint)i);

		}
		else
		{
			m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture");
			m_pShaderCom->Begin((_uint)MESH_PASS::SKY);
			m_pModelCom->Render((_uint)i);

			m_pCloudTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture");
			m_pShaderCom->Begin((_uint)MESH_PASS::SKY);
			m_pModelCom->Render((_uint)i);
		}

	}


	return S_OK;
}

HRESULT CSkySphere::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SkySphere"), TEXT("Model_Com")));

	m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_SkySphere_Kale"), TEXT("Texture_Com")));
	m_pCloudTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_SkySphere_KaleCloud"), TEXT("Texture_Com2")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

shared_ptr<CSkySphere> CSkySphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSkySphere
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkySphere(pDevice, pContext)
		{}
	};


	shared_ptr<CSkySphere> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkySphere"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CSkySphere::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSkySphere
	{
		MakeSharedEnabler(const CSkySphere& rhs) : CSkySphere(rhs)
		{}
	};


	shared_ptr<CSkySphere> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkySphere"));
		assert(false);
	}

	return pInstance;
}

void CSkySphere::Free()
{
}
