
#include "Basic_Guitar.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
wstring CBasic_Guitar::ObjID = TEXT("CBasic_Guitar");


CBasic_Guitar::CBasic_Guitar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBasic_Guitar::CBasic_Guitar(const CBasic_Guitar& rhs)
	: CPartObject( rhs )
{

}

HRESULT CBasic_Guitar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Junk_Guitar"), TEXT("../Bin/Resources/Models/Player/Chai/Guitar/Junk_Guitar.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ElecGuitar"), TEXT("../Bin/Resources/Models/Player/Chai/Guitar/ElectircGuitar.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	return S_OK;
}

HRESULT CBasic_Guitar::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pBoneMatrix = pDesc->pBoneMatrix;
	m_pEvent_BoneMatrix = pDesc->pEventBoneMatrix;
	m_pLeftHand_BoneMatrix = pDesc->pLeftHandBoneMatrix;

	_float4x4 xMat = {};

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	vector<shared_ptr<CAnimation>> animList = m_pModelCom->Get_Animation_List();
	for (auto& iter : animList)
	{
		m_vecAnimList.push_back(iter->Get_AnimName());
	}
	m_pElecGuitar_ModelCom->Play_Animation(0.0001f, 0.0001f, 0.1f, xMat);
	return S_OK;
}

void CBasic_Guitar::PriorityTick(_float fTimeDelta)
{
}

void CBasic_Guitar::Tick(_float fTimeDelta)
{
	_matrix		BoneMatrix = {};
	if(m_eGuitar_State ==GUITAR_STATE::NORMAL)
		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);
	else if(m_eGuitar_State == GUITAR_STATE::EVENT)
		BoneMatrix = XMLoadFloat4x4(m_pEvent_BoneMatrix);
	else if (m_eGuitar_State == GUITAR_STATE::LEFTHAND || m_eGuitar_State == GUITAR_STATE::MIMOSA_DANCE)
		BoneMatrix = XMLoadFloat4x4(m_pLeftHand_BoneMatrix);


	shared_ptr<CModel> pModel = {};

	if (!m_bElectric_Guitar)
		pModel = m_pModelCom;
	else
		pModel = m_pElecGuitar_ModelCom;

	Animation_End();
	_float4x4 mMat = {};
	pModel->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, mMat);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);

	//m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));


}

void CBasic_Guitar::LateTick(_float fTimeDelta)
{
	if(m_bRender)
		GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CBasic_Guitar::Render()
{
	shared_ptr<CModel> pModel = {};

	if (!m_bElectric_Guitar)
		pModel = m_pModelCom;
	else
		pModel = m_pElecGuitar_ModelCom;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = pModel->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		
			continue;

		if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		pModel->Render((_uint)i);

		m_pShaderCom->Bind_Float("g_OutlineWidth", 0.7f);
		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

		pModel->Render((_uint)i);
	}

	return S_OK;
}

void CBasic_Guitar::Change_Animation(string strAnimName)
{
	shared_ptr<CModel> pModel = {};

	if (!m_bElectric_Guitar)
		pModel = m_pModelCom;
	else
		pModel = m_pElecGuitar_ModelCom;

	_bool bFind = false;
	for (auto& iter :m_vecAnimList)
	{
		string animName = iter.substr(iter.find("_"));
		string PlayerAnimName = strAnimName.substr(strAnimName.find("_"));

		if (animName == PlayerAnimName)
		{
			pModel->Set_AnimIndexNonCancle(iter,false);
			bFind = true;
		}
	}
	if(bFind == false)
		pModel->Set_AnimIndex("wp0000_atk-guitar_241", false);
}

void CBasic_Guitar::Change_Event_Animation(string strAnimName)
{
	m_eGuitar_State = GUITAR_STATE::EVENT;
	m_pModelCom->Set_AnimIndexNonCancle(strAnimName, false);
}

void CBasic_Guitar::Animation_End()
{
	shared_ptr<CModel> pModel = {};

	if (!m_bElectric_Guitar)
		pModel = m_pModelCom;
	else
		pModel = m_pElecGuitar_ModelCom;


	if (pModel->Get_StringAnimIndex("EV2000_00_000_wp0011_00") == pModel->Get_CurrentAnimIndex()
		&& pModel->Get_Finished())
	{
		m_eGuitar_State = GUITAR_STATE::NORMAL;
		pModel->Set_AnimIndexNonCancle_NoInterpole("wp0000_atk-guitar_241", false);
	}
}

HRESULT CBasic_Guitar::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Junk_Guitar"), TEXT("Com_Model")));

	/* For.Com_Model */
	m_pElecGuitar_ModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ElecGuitar"), TEXT("Com_ElecGuitar_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CBasic_Guitar::Bind_ShaderResources()
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

shared_ptr<CBasic_Guitar> CBasic_Guitar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBasic_Guitar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBasic_Guitar(pDevice, pContext)
		{}
	};


	shared_ptr<CBasic_Guitar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBasic_Guitar"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CBasic_Guitar::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBasic_Guitar
	{
		MakeSharedEnabler(const CBasic_Guitar& rhs) : CBasic_Guitar(rhs)
		{}
	};


	shared_ptr<CBasic_Guitar> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBasic_Guitar"));
		assert(false);
	}

	return pInstance;
}


void CBasic_Guitar::Free()
{

}

CBasic_Guitar::~CBasic_Guitar()
{
	Free();
}
