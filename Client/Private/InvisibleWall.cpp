#include "InvisibleWall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Texture.h"
#include "Beat_Manager.h"
#include "VIBuffer_Rect.h"
#include "CombatManager.h"
#include "Cine_Manager.h"
wstring CInvisibleWall::ObjID = TEXT("CInvisibleWall");

CInvisibleWall::CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CInvisibleWall::CInvisibleWall(const CInvisibleWall& rhs)
	: CGimmickBase{ rhs }
{
}

CInvisibleWall::~CInvisibleWall()
{
	Free();
}

void CInvisibleWall::Execute(_bool bExecute)
{
	if (bExecute == false)
	{
		m_bEnd = true;
	}
	else
	{
		m_bExecuted = true;
	}
	__super::m_bDone = true;
}

HRESULT CInvisibleWall::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_InvisibleWall_Texture"), TEXT("../Bin/Resources/Textures/InvisibleWall/t_InvisibleWall.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_InvisibleWall_ScratchTexture"), TEXT("../Bin/Resources/Textures/InvisibleWall/t_Scratch.png"), 1));



	return S_OK;
}

HRESULT CInvisibleWall::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	XMStoreFloat4x4(&m_Matrices[0],
		XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 0.1f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
			XMQuaternionIdentity(), XMVectorSet(0.f, 0.5f, 0.5f, 0.f)));

	XMStoreFloat4x4(&m_Matrices[1],
		XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 0.1f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
			XMQuaternionIdentity(), XMVectorSet(0.f, 0.5f, -0.5f, 0.f)));

	XMStoreFloat4x4(&m_Matrices[2],
		XMMatrixAffineTransformation(XMVectorSet(1.0f, 1.f, 0.1f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
			XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, 0.f), XMVectorSet(0.5f, 0.5f, 0.f, 0.f)));

	XMStoreFloat4x4(&m_Matrices[3],
		XMMatrixAffineTransformation(XMVectorSet(1.0f, 1.f, 0.1f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
			XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, 0.f), XMVectorSet(-0.5f, 0.5f, 0.f, 0.f)));



	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInvisibleWall::PriorityTick(_float fTimeDelta)
{
}

void CInvisibleWall::Tick(_float fTimeDelta)
{

	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	for (_uint i = 0; i < 4; ++i)
	{
		XMStoreFloat4x4(&m_WorldMatrices[i], XMLoadFloat4x4(&m_Matrices[i]) * World);
	}

	if (m_bExecuted)
	{

		if (CCombat_Manager::Get_Instance()->Get_Combat_End())
			Execute(false);
		
		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
			auto pGameInstance = m_pGameInstance.lock();
			m_vUVFlow.x = pGameInstance->Random_Float(0.f, 1.f);
			m_vThreshold.x = pGameInstance->Random_Float(0.f, 1.f);
			m_vThreshold.y = pGameInstance->Random_Float(0.f, 1.f);
			m_vThreshold.z = pGameInstance->Random_Float(0.f, 1.f);
			m_vThreshold.w = pGameInstance->Random_Float(0.f, 1.f);
			if (m_bEnd)
			{
				m_bEnd = false;
				m_bExecuted = false;
			}
		}


		for (_uint i = 0; i < 4; ++i)
		{
			_matrix Transpose = XMLoadFloat4x4(&m_Matrices[i]) * World;
			for (_uint j = 0; j < 3; ++j)
			{
				Transpose.r[j] = XMVector3Normalize(Transpose.r[j]);
			}
			_float4x4 mat;
			XMStoreFloat4x4(&mat, Transpose);
			PxMat44 pxWorld;
			pxWorld.column0.x = mat.m[0][0];
			pxWorld.column0.y = mat.m[0][1];
			pxWorld.column0.z = mat.m[0][2];
			pxWorld.column0.w = mat.m[0][3];
			pxWorld.column1.x = mat.m[1][0];
			pxWorld.column1.y = mat.m[1][1];
			pxWorld.column1.z = mat.m[1][2];
			pxWorld.column1.w = mat.m[1][3];
			pxWorld.column2.x = mat.m[2][0];
			pxWorld.column2.y = mat.m[2][1];
			pxWorld.column2.z = mat.m[2][2];
			pxWorld.column2.w = mat.m[2][3];
			pxWorld.column3.x = mat.m[3][0];
			pxWorld.column3.y = mat.m[3][1];
			pxWorld.column3.z = mat.m[3][2];
			pxWorld.column3.w = mat.m[3][3];

			PxTransform transform{ pxWorld };
			m_pPhysXCollider[i]->Get_Static_RigidBody()->setGlobalPose(transform);
		}


	}
	else
	{
		for (_uint i = 0; i < 4; ++i)
		{
			m_pPhysXCollider[i]->Get_Static_RigidBody()->setGlobalPose(PxTransform{PxVec3{-100000.f,-100000.f,-100000.f}});
		}
	}
}

void CInvisibleWall::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_bExecuted || m_pGameInstance.lock()->Get_Current_LevelID() == LEVEL_MAPTOOL)
		GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
#else
	if (!CCine_Manager::Get_Instance()->IsInGame()) return;

	if (m_bExecuted)
		GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
#endif
}

HRESULT CInvisibleWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_float fTimerPerBeat = CBeat_Manager::Get_Instance()->Get_Timer() / CBeat_Manager::Get_Instance()->Get_Beat();
	m_pShaderCom->Bind_Float("g_fDissolveThreshold", pow(cos(fTimerPerBeat * XM_PI), 2.f));
	m_pShaderCom->Bind_Float("g_fEffectAlpha", 0.8f);

	m_pShaderCom->Bind_RawValue("g_vUVFlow", &m_vUVFlow, sizeof(_float2));
	m_pShaderCom->Bind_Vector4("g_vEffectBaseColorR", &m_vR);
	m_pShaderCom->Bind_Vector4("g_vEffectBaseColorG", &m_vG);
	_float3 vScale = m_pTransformCom->Get_Scale();
	_float2 vUVMultX = { vScale.x, 1.f };
	_float2 vUVMultZ = { vScale.z, 1.f };
	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUVMultX, sizeof(_float2));//이거 각도 따라 달리 해야함
	m_pShaderCom->Bind_Vector4("g_vThreshold", &m_vThreshold);
	for (_uint i = 0; i < 4; ++i)
	{
		if (i == 2)
			m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUVMultZ, sizeof(_float2));
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[i]);

		if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::InvisibleWall)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	

	return S_OK;
}

void CInvisibleWall::Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CInvisibleWall::Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CInvisibleWall::Edit_Attribute()
{
}

HRESULT CInvisibleWall::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_InvisibleWall_Texture"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_InvisibleWall_ScratchTexture"), TEXT("Com_Noise_Texture")));
	assert(m_pNoiseTextureCom);
	assert(m_pTextureCom);
	

	_float3 vScale = m_pTransformCom->Get_Scale();
	for(_uint i = 0; i < 4;  ++i)
	{
		m_pPhysXCollider[i] = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), 
			wstring(TEXT("Com_PhsyXCollider")) + to_wstring(i)));
		string strTag = m_strGimmickID;
		m_vModelSize = { 1.f * vScale.x, 20.f * vScale.y, 0.2f };
		m_vModelCenter = { m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
		PxVec3 BoxDimension{ m_vModelSize.x, m_vModelSize.y, m_vModelSize.z };
		PxBoxGeometry Box{ BoxDimension };
		auto pRigidBody = m_pPhysXCollider[i]->createStaticShapeActor(Box, strTag.c_str());
	}



	if (!m_pShaderCom || !m_pPhysXCollider)
		assert(false);

	return S_OK;
}

HRESULT CInvisibleWall::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	return S_OK;
}

void CInvisibleWall::Move(_float fTimeDelta)
{
	_float fRadPerSec = XMConvertToRadians(360.f);

	//if (XMConvertToRadians(90.f) < m_fCurRotation)
	//{
	//	m_pTransformCom->Rotation_Override(m_pTransformCom->Get_NormLook() * (XMConvertToRadians(90.f) - m_fCurRotation));
	//	m_bDone = true;
	//}

	//else if (fabs(XMConvertToRadians(90.f) - m_fCurRotation) <= XMConvertToRadians(5.f))
	//{
	//	m_pTransformCom->Rotation_Override(m_pTransformCom->Get_NormLook() * (XMConvertToRadians(90.f) - m_fCurRotation));
	//	m_bDone = true;
	//}

	//else if (fabs(XMConvertToRadians(90.f) - m_fCurRotation) > XMConvertToRadians(5.f))
	//{
	//	m_fCurRotation += fRadPerSec * fTimeDelta;
	//	m_pTransformCom->Turn(m_pTransformCom->Get_NormLook(), fRadPerSec, fTimeDelta);
	//}
}

shared_ptr<CInvisibleWall> CInvisibleWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CInvisibleWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CInvisibleWall(pDevice, pContext)
		{}
	};

	shared_ptr<CInvisibleWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CInvisibleWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CInvisibleWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CInvisibleWall
	{
		MakeSharedEnabler(const CInvisibleWall& rhs) : CInvisibleWall(rhs)
		{}
	};

	shared_ptr<CInvisibleWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CInvisibleWall"));
		assert(false);
	}

	return pInstance;
}

void CInvisibleWall::Free()
{
}
