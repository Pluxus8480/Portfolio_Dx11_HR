#include "SwordMachine_Sword.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"
#include "Beat_Manager.h"

wstring CSwordMachine_Sword::ObjID = TEXT("CSwordMachine_Sword");

CSwordMachine_Sword::CSwordMachine_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CSwordMachine_Sword::CSwordMachine_Sword(const CSwordMachine_Sword& rhs)
	: CPartObject(rhs)
{
}

HRESULT CSwordMachine_Sword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_matrix LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(90.f));

	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_SwordMachine_Sword"), TEXT("../Bin/Resources/Models/Monsters/SwordMachine/SwordMachine_Sword.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	return S_OK;
}

HRESULT CSwordMachine_Sword::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
	m_pBoneMatrix = pDesc->pBoneMatrix;
	m_iLevelIndex = pDesc->iLevelIndex;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 위치 조정

	return S_OK;
}

void CSwordMachine_Sword::PriorityTick(_float fTimeDelta)
{
}

void CSwordMachine_Sword::Tick(_float fTimeDelta)
{
	_matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);
	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);


	_float4x4 fmatResult;
	XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

	Animation(fTimeDelta);
}

void CSwordMachine_Sword::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CSwordMachine_Sword::Render()
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

HRESULT CSwordMachine_Sword::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

    m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Model_SwordMachine_Sword"), TEXT("Com_Model")));

	return S_OK;
}

HRESULT CSwordMachine_Sword::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

void CSwordMachine_Sword::Animation(_float fTimeDelta)
{
	_float4x4 fmatResult;
	XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());

	_float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

	_float Four_Beat = m_pModelCom->Get_Animation_PlayTime() / fBeat * 0.25f;

	_uint index = m_pModelCom->Get_CurrentAnimIndex();
	_float fTP = m_pModelCom->Get_Current_Anim_Track_Position_Percentage();

	_uint currAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * Four_Beat, 0.1f, fmatResult);

	_matrix matResult = XMLoadFloat4x4(&fmatResult);

	_float4x4 matTemp = m_pTransformCom->Get_WorldMatrix();
	_matrix matAnimRot = matResult * XMLoadFloat4x4(&matTemp);


	auto matTranslation = XMMatrixTranslationFromVector(matResult.r[3]);
	auto vCurPos = XMVectorSetW((matTranslation * XMLoadFloat4x4(&matTemp)).r[3], 1.f);

	m_pTransformCom->Set_Position(vCurPos);
}


shared_ptr<CSwordMachine_Sword> CSwordMachine_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSwordMachine_Sword
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSwordMachine_Sword(pDevice, pContext) {}
	};

	shared_ptr<CSwordMachine_Sword> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSwordMachine_Sword"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CSwordMachine_Sword::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSwordMachine_Sword
	{
		MakeSharedEnabler(const CSwordMachine_Sword& rhs) : CSwordMachine_Sword(rhs) {}
	};

	shared_ptr<CSwordMachine_Sword> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSwordMachine_Sword"));
		assert(false);
	}
	return pInstance;
}

void CSwordMachine_Sword::Free()
{
}

CSwordMachine_Sword::~CSwordMachine_Sword()
{
	Free();
}
