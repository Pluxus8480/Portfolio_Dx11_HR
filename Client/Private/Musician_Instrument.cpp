#include "Musician_Instrument.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"
#include "Beat_Manager.h"

wstring CMusician_Instrument::ObjID = TEXT("CMusician_Instrument");

CMusician_Instrument::CMusician_Instrument(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CMusician_Instrument::CMusician_Instrument(const CMusician_Instrument& rhs)
	: CPartObject(rhs)
{
}

HRESULT CMusician_Instrument::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_matrix LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationX(XMConvertToRadians(0.f));

	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Musician_Guitar"), TEXT("../Bin/Resources/Models/Monsters/Mimosa_Musician/Musician_Guitar.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Musician_AltSax"), TEXT("../Bin/Resources/Models/Monsters/Mimosa_Musician/Musician_AltSax.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Musician_Drum"), TEXT("../Bin/Resources/Models/Monsters/Mimosa_Musician/Musician_Drum.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Musician_Base"), TEXT("../Bin/Resources/Models/Monsters/Mimosa_Musician/Musician_Base.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Musician_Trombone"), TEXT("../Bin/Resources/Models/Monsters/Mimosa_Musician/Musician_Trombone.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	return S_OK;
}

HRESULT CMusician_Instrument::Initialize(void* pArg)
{
	INSTRUMENT_DESC* pDesc = static_cast<INSTRUMENT_DESC*>(pArg);
	m_pBoneMatrix = pDesc->pBoneMatrix;
	m_iLevelIndex = pDesc->iLevelIndex;
	m_iType = pDesc->iType;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

		// 위치 조정
	

	Play_Instrument();
	//m_pModelCom->Set_AnimIndex("np0134_drum_000", true);
	/*if(m_iType == 6)
		m_pTransformCom->Rotation(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(-90));*/
	return S_OK;
}

void CMusician_Instrument::PriorityTick(_float fTimeDelta)
{
}

void CMusician_Instrument::Tick(_float fTimeDelta)
{
	_matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);
	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);


	_float4x4 fmatResult;
	XMStoreFloat4x4(&fmatResult, XMMatrixIdentity());
	
	Animation(fTimeDelta);
}

void CMusician_Instrument::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CMusician_Instrument::Render()
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

HRESULT CMusician_Instrument::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
	
	
	switch (m_iType)
	{
	case 1:
	{
		m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Musician_Guitar"), TEXT("Com_Model")));
		break;
	}
	case 2:
	{
		m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Musician_Base"), TEXT("Com_Model")));
		break;
	}
	case 4:
	{
		m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Musician_Drum"), TEXT("Com_Model")));
		break;
	}
	case 5:
	{
		m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Musician_Trombone"), TEXT("Com_Model")));
		break;
	}
	case 6:
	{
		m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Musician_AltSax"), TEXT("Com_Model")));
		break;
	}
	}

	return S_OK;
}

HRESULT CMusician_Instrument::Bind_ShaderResources()
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

void CMusician_Instrument::Animation(_float fTimeDelta)
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

void CMusician_Instrument::Play_Instrument()
{
	switch (m_iType)
	{
	case 1:
	{
		m_pModelCom->Set_AnimIndex("np0130_idle_010", true);
		break;
	}
	case 2:
	{
		m_pModelCom->Set_AnimIndex("np0130_idle_020", true);
		break;
	}
	case 4:
	{
		m_pModelCom->Set_AnimIndex("np0130_idle_040", true);
		break;
	}
	case 5:
	{
		m_pModelCom->Set_AnimIndex("np0130_idle_050", true);
		break;
	}
	case 6:
	{
		m_pModelCom->Set_AnimIndex("np0130_idle_060", true);
		break;
	}
	}
}

shared_ptr<CMusician_Instrument> CMusician_Instrument::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CMusician_Instrument
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMusician_Instrument(pDevice, pContext) {}
	};

	shared_ptr<CMusician_Instrument> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMusician_Instrument"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CMusician_Instrument::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMusician_Instrument
	{
		MakeSharedEnabler(const CMusician_Instrument& rhs) : CMusician_Instrument(rhs) {}
	};

	shared_ptr<CMusician_Instrument> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMusician_Instrument"));
		assert(false);
	}
	return pInstance;
}

void CMusician_Instrument::Free()
{
}

CMusician_Instrument::~CMusician_Instrument()
{
	Free();
}
