
#include "Kale_Sword.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
wstring CKale_Sword::ObjID = TEXT("CKale_Sword");


CKale_Sword::CKale_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CKale_Sword::CKale_Sword(const CKale_Sword& rhs)
	: CPartObject(rhs)
{

}

HRESULT CKale_Sword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Kale_Sword"), TEXT("../Bin/Resources/Models/Monsters/Kale/kale_sword.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CKale_Sword::Initialize(void* pArg)
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
	m_pModelCom->Set_AnimIndex(0, false);
	_float4x4 Mat;
	m_pModelCom->Play_Animation(0.01f, 0.01f, 0.f, Mat);
	m_bAnimStop = true;
	//m_pTransformCom->Rotation(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(-90));
	return S_OK;
}

void CKale_Sword::PriorityTick(_float fTimeDelta)
{
	m_bRender = true;
}

void CKale_Sword::Tick(_float fTimeDelta)
{
	if(!m_bAnimStop)
	{
		_float4x4 mMat = {};
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fAnimSpeed, 0.1f, mMat);
	}
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	Setup_Matrix();

	//m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));


}

void CKale_Sword::LateTick(_float fTimeDelta)
{
	if (!m_bRender)
		return;
	m_iRenderCount = 0;
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	//GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
}

HRESULT CKale_Sword::Render()
{
	++m_iRenderCount;
	if (m_iRenderCount == 1)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;
		_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_bool bEmit = true;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			{
				continue;
			}
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			{
				bEmit = false;
			}
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
				return E_FAIL;
			if (bEmit)
			{
				if(!m_bYellow)
					m_pShaderCom->Begin((_uint)ANIMMESH_PASS::EmissionCCW);
				else
				{
					_float4 vColor{ 1.f,0.5f, 0.f,1.f };
					m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
					m_pShaderCom->Begin((_uint)ANIMMESH_PASS::EmissionColor);
				}
			}
			else
			{
				m_pShaderCom->Begin(0);
			}

			//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			//   return E_FAIL;

			m_pModelCom->Render((_uint)i);
			//if(!bEmit)
			//{
			//	m_pShaderCom->Bind_Float("g_OutlineWidth", 1.0f);
			//	m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

			//	m_pModelCom->Draw((_uint)i);
			//}
		}
	}
	//else
	//{
	//	if (FAILED(Bind_ShaderResources()))
	//		return E_FAIL;
	//	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	//	for (size_t i = 0; i < iNumMeshes; i++)
	//	{
	//		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
	//		{
	//			continue;
	//		}

	//		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
	//			return E_FAIL;

	//		m_pShaderCom->Begin(0);

	//		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
	//		//   return E_FAIL;

	//		m_pModelCom->Render((_uint)i);
	//	}
	//}


	return S_OK;
}

void CKale_Sword::Setup_Matrix()
{
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);

}

_bool CKale_Sword::Change_Animation(string strAnimName)
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
	{
		m_pModelCom->Set_AnimIndex(0, false);
		_float4x4 Mat;
		m_pModelCom->Play_Animation(0.01f, 0.01f, 0.f, Mat);
		m_bAnimStop = true;
	}
	else
	{
		m_bAnimStop = false;
	}
	return bFind;
}

HRESULT CKale_Sword::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_KALEBOSS, TEXT("Prototype_Component_Model_Kale_Sword"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CKale_Sword::Bind_ShaderResources()
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

shared_ptr<CKale_Sword> CKale_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CKale_Sword
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKale_Sword(pDevice, pContext)
		{}
	};


	shared_ptr<CKale_Sword> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKale_Sword"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CKale_Sword::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CKale_Sword
	{
		MakeSharedEnabler(const CKale_Sword& rhs) : CKale_Sword(rhs)
		{}
	};


	shared_ptr<CKale_Sword> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKale_Sword"));
		assert(false);
	}

	return pInstance;
}


void CKale_Sword::Free()
{

}

CKale_Sword::~CKale_Sword()
{
	Free();
}
