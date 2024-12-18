
#include "Weapon_Mimosa.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
wstring CWeapon_Mimosa::ObjID = TEXT("CWeapon_Mimosa");


CWeapon_Mimosa::CWeapon_Mimosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Mimosa::CWeapon_Mimosa(const CWeapon_Mimosa& rhs)
	: CPartObject(rhs)
{

}

HRESULT CWeapon_Mimosa::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Weapon_Mimosa"), TEXT("../Bin/Resources/Models/Monsters/Mimosa/Mimosa_Mic.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CWeapon_Mimosa::Initialize(void* pArg)
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

	m_pTransformCom->Rotation(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(-90));
	return S_OK;
}

void CWeapon_Mimosa::PriorityTick(_float fTimeDelta)
{
}

void CWeapon_Mimosa::Tick(_float fTimeDelta)
{
	_float4x4 mMat = {};
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fAnimSpeed, 0.1f, mMat);
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);

	//m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));


}

void CWeapon_Mimosa::LateTick(_float fTimeDelta)
{
	if(m_bRender)
		GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CWeapon_Mimosa::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		{

			continue;
		}


		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);

		m_pShaderCom->Bind_Float("g_OutlineWidth", 1.0f);
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

		m_pModelCom->Draw((_uint)i);
	}

	return S_OK;
}

_bool CWeapon_Mimosa::Change_Animation(string strAnimName)
{
	
	m_pModelCom->Set_AnimIndexNonCancle_NoInterpole(strAnimName, false);


	return true;
}

HRESULT CWeapon_Mimosa::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_MIMOSA, TEXT("Prototype_Component_Model_Weapon_Mimosa"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CWeapon_Mimosa::Bind_ShaderResources()
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

shared_ptr<CWeapon_Mimosa> CWeapon_Mimosa::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CWeapon_Mimosa
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CWeapon_Mimosa(pDevice, pContext)
		{}
	};


	shared_ptr<CWeapon_Mimosa> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Mimosa"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CWeapon_Mimosa::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CWeapon_Mimosa
	{
		MakeSharedEnabler(const CWeapon_Mimosa& rhs) : CWeapon_Mimosa(rhs)
		{}
	};


	shared_ptr<CWeapon_Mimosa> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Mimosa"));
		assert(false);
	}

	return pInstance;
}


void CWeapon_Mimosa::Free()
{

}

CWeapon_Mimosa::~CWeapon_Mimosa()
{
	Free();
}
