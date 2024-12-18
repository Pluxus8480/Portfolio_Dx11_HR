#include "Rekka_Gauntlet.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Model.h"

#include "Beat_Manager.h"


wstring CRekka_Gauntlet::ObjID = TEXT("Rekka_Gauntlet");

CRekka_Gauntlet::CRekka_Gauntlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CRekka_Gauntlet::CRekka_Gauntlet(const CRekka_Gauntlet& rhs)
	: CPartObject(rhs)
{
}

CRekka_Gauntlet::~CRekka_Gauntlet()
{
	Free();
}

HRESULT CRekka_Gauntlet::Initialize_Prototype()
{
	_matrix	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Model_Rekka_Gauntlet"), TEXT("../Bin/Resources/Models/Monsters/Rekka/Gauntlet.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CRekka_Gauntlet::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	m_pBoneMatrix = pDesc->pBoneMatrix;
	m_iLevelIndex = pDesc->iLevelIndex;

	XMStoreFloat4x4(&m_matScale, XMMatrixIdentity());

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	PrepareCallbacks();

	return S_OK;
}

void CRekka_Gauntlet::PriorityTick(_float fTimeDelta)
{
}

void CRekka_Gauntlet::Tick(_float fTimeDelta)
{
	if (m_isRendering)
	{
		_float4x4 matResult{};
		SelectBeat();
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fBeatSpeed, 0.1f, matResult);

		m_fTimer_Scale -= fTimeDelta;
		if (m_fTimer_Scale > 0.f)
			SizeUp();
		/*
		// 위치 조정
		SetUp_WorldMatrix(XMLoadFloat4x4(&m_matScale) * m_pTransformCom->Get_WorldMatrix_XMMat());
		*/

		_matrix matParentSpine = XMLoadFloat4x4(m_pBoneMatrix) * m_pParentTransform->Get_WorldMatrix_XMMat();
		_matrix matSpine = XMLoadFloat4x4(m_pMat_Spine) * XMLoadFloat4x4(&m_matScale) * m_pTransformCom->Get_WorldMatrix_XMMat() * m_pParentTransform->Get_WorldMatrix_XMMat();
		_vector vDiff = matParentSpine.r[3] - matSpine.r[3];

		// 위치 조정
		SetUp_WorldMatrix(XMLoadFloat4x4(&m_matScale) * m_pTransformCom->Get_WorldMatrix_XMMat());
		_matrix matWorld = XMLoadFloat4x4(&m_WorldMatrix);
		_vector vPos = matWorld.r[3] + vDiff;
		matWorld.r[3] = vPos;
		XMStoreFloat4x4(&m_WorldMatrix, matWorld);
	}
}

void CRekka_Gauntlet::LateTick(_float fTimeDelta)
{
	if (m_isRendering)
	{
		GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
	}
}

HRESULT CRekka_Gauntlet::Render()
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
	_float4 vColor = { 1.f,0.2f, 0.f,1.f };
	m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
	m_pShaderCom->Bind_Vector4("g_vCamPos", &m_pGameInstance.lock()->Get_CamPosition_Float4());
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)ANIMMESH_PASS::ColorOnly);

		if (FAILED(m_pModelCom->Bind_Buffers(i)))
			return E_FAIL;

		m_pModelCom->Draw((_uint)i);


	}

	return S_OK;
}

HRESULT CRekka_Gauntlet::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, TEXT("Model_Rekka_Gauntlet"), TEXT("Com_Model")));

	m_pMat_Spine = m_pModelCom->Get_BoneMatrixPtr("spine_00");

	return S_OK;
}

void CRekka_Gauntlet::PrepareCallbacks()
{
	m_pModelCom->SetAnimCallback("em2002_atk_120", 0, [&]() {
		m_fTimer_Scale = 0.2f;
		m_fCurScale = 1.f;
		m_fTargetScale = 1.3f;
		}
	);
	m_pModelCom->SetAnimCallback("em2002_atk_120", 1, [&]() {
		m_fTimer_Scale = 0.2f;
		m_fCurScale = 1.3f;
		m_fTargetScale = 1.69f;
		}
	);
	m_pModelCom->SetAnimCallback("em2002_atk_120", 2, [&]() {
		m_fTimer_Scale = 0.2f;
		m_fCurScale = 1.69f;
		m_fTargetScale = 2.197f;
		}
	);
}

void CRekka_Gauntlet::SizeUp()
{
	_float fRatio = 1.f - m_fTimer_Scale / 0.2f;
	_float fScale = m_fCurScale + fRatio * (m_fTargetScale - m_fCurScale);
	XMStoreFloat4x4(&m_matScale, XMMatrixScaling(fScale, fScale, fScale));
}

void CRekka_Gauntlet::ReadyForAttack()
{
	m_isRendering = true;
	m_eStep = EStep::Ready;
	m_pModelCom->Set_AnimIndex_NoInterpole("em2002_atk_120", false);
}

void CRekka_Gauntlet::StayForAttack()
{
	m_isRendering = true;
	m_eStep = EStep::Stay;
	m_pModelCom->Set_AnimIndex_NoInterpole("em2002_atk_121", true);
}

void CRekka_Gauntlet::StartAttack()
{
	m_isRendering = true;
	m_eStep = EStep::Attack;
	m_pModelCom->Set_AnimIndex_NoInterpole("em2002_atk_123", false);
}

void CRekka_Gauntlet::FinishAttack()
{
	m_isRendering = false;
	m_eStep = EStep::Default;
}

void CRekka_Gauntlet::SelectBeat()
{
	_float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

	_float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

	m_fBeatSpeed = fPlayTime / fBeat / fTotalBeat;
}

shared_ptr<CRekka_Gauntlet> CRekka_Gauntlet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRekka_Gauntlet
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Gauntlet(pDevice, pContext) {}
	};

	shared_ptr<CRekka_Gauntlet> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_Gauntlet"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CRekka_Gauntlet::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_Gauntlet
	{
		MakeSharedEnabler(const CRekka_Gauntlet& rhs) : CRekka_Gauntlet(rhs) {}
	};

	shared_ptr<CRekka_Gauntlet> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_Gauntlet"));
		assert(false);
	}
	return pInstance;
}

void CRekka_Gauntlet::Free()
{
}
