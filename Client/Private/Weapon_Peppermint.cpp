
#include "Weapon_Peppermint.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
wstring CWeapon_Peppermint::ObjID = TEXT("CWeapon_Peppermint");


CWeapon_Peppermint::CWeapon_Peppermint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Peppermint::CWeapon_Peppermint(const CWeapon_Peppermint& rhs)
	: CPartObject(rhs)
{

}

HRESULT CWeapon_Peppermint::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	LocalTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Weapon_Peppermint"), TEXT("../Bin/Resources/Models/Player/Peppermint/Weapon_Peppermint.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CWeapon_Peppermint::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pBoneMatrix = pDesc->pBoneMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	//m_pTransformCom->Rotation(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(-90));
	return S_OK;
}

void CWeapon_Peppermint::PriorityTick(_float fTimeDelta)
{
}

void CWeapon_Peppermint::Tick(_float fTimeDelta)
{
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);

	//m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

}

void CWeapon_Peppermint::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CWeapon_Peppermint::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
		//    return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//    return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
		//	return E_FAIL;

		m_pShaderCom->Begin(0);

		if (FAILED(m_pModelCom->Bind_Buffers(i)))
			return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}



	return S_OK;
}

HRESULT CWeapon_Peppermint::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));


	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Peppermint"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CWeapon_Peppermint::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	/* 부모의 행렬을 완전히 무시한 상황. */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
	//   return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CWeapon_Peppermint> CWeapon_Peppermint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CWeapon_Peppermint
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CWeapon_Peppermint(pDevice, pContext)
		{}
	};


	shared_ptr<CWeapon_Peppermint> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Peppermint"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CWeapon_Peppermint::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CWeapon_Peppermint
	{
		MakeSharedEnabler(const CWeapon_Peppermint& rhs) : CWeapon_Peppermint(rhs)
		{}
	};


	shared_ptr<CWeapon_Peppermint> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Peppermint"));
		assert(false);
	}

	return pInstance;
}


void CWeapon_Peppermint::Free()
{

}

CWeapon_Peppermint::~CWeapon_Peppermint()
{
	Free();
}
