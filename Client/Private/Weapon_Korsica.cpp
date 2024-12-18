
#include "Weapon_Korsica.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
wstring CWeapon_Korsica::ObjID = TEXT("CWeapon_Korsica");


CWeapon_Korsica::CWeapon_Korsica(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Korsica::CWeapon_Korsica(const CWeapon_Korsica& rhs)
	: CPartObject(rhs)
{

}

HRESULT CWeapon_Korsica::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Weapon_Korsica"), TEXT("../Bin/Resources/Models/Player/Korsica/Weapon_Korsica.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CWeapon_Korsica::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pBoneMatrix = pDesc->pBoneMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	vector<shared_ptr<CAnimation>> animList = m_pModelCom->Get_Animation_List();
	for (auto& iter : animList)
	{
		m_vecAnimList.push_back(iter->Get_AnimName());
	}

	//m_pTransformCom->Rotation(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(-90));
	return S_OK;
}

void CWeapon_Korsica::PriorityTick(_float fTimeDelta)
{
}

void CWeapon_Korsica::Tick(_float fTimeDelta)
{
	_float4x4 mMat = {};
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fAnimSpeed, 0.1f, mMat);
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);

	//m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
	

}

void CWeapon_Korsica::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CWeapon_Korsica::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		{
			//if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			//    return E_FAIL;
			//m_pShaderCom->Bind_Float("g_OutlineWidth", 0.01f);
			//m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);


			//m_pModelCom->Render((_uint)i);
			continue;
		}
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_SPECULAR)))
		//    return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//    return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//   return E_FAIL;

		m_pModelCom->Render((_uint)i);

		m_pShaderCom->Bind_Float("g_OutlineWidth", 1.0f);
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

		m_pModelCom->Draw((_uint)i);
	}

	return S_OK;
}

_bool CWeapon_Korsica::Change_Animation(string strAnimName)
{
	_bool bFind = false;
	for (auto& iter : m_vecAnimList)
	{
		string animName = iter.substr(iter.find("_"));
		string PlayerAnimName = strAnimName.substr(strAnimName.find("_"));

		if (animName == PlayerAnimName)
		{
			m_pModelCom->Set_AnimIndexNonCancle(iter, false);
			bFind = true;
		}
	}
	if (bFind == false)
		m_pModelCom->Set_AnimIndex("ch0000_event-korsica_000", false);

	return bFind;
}

HRESULT CWeapon_Korsica::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Korsica"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CWeapon_Korsica::Bind_ShaderResources()
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

shared_ptr<CWeapon_Korsica> CWeapon_Korsica::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CWeapon_Korsica
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CWeapon_Korsica(pDevice, pContext)
		{}
	};


	shared_ptr<CWeapon_Korsica> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Korsica"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CWeapon_Korsica::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CWeapon_Korsica
	{
		MakeSharedEnabler(const CWeapon_Korsica& rhs) : CWeapon_Korsica(rhs)
		{}
	};


	shared_ptr<CWeapon_Korsica> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Korsica"));
		assert(false);
	}

	return pInstance;
}


void CWeapon_Korsica::Free()
{

}

CWeapon_Korsica::~CWeapon_Korsica()
{
	Free();
}
