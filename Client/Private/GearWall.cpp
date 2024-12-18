#include "GearWall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"

#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "MapManager.h"

wstring CGearWall::ObjID = TEXT("CGearWall");
CGearWall::CGearWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{pDevice, pContext}
{
}

CGearWall::CGearWall(const CGearWall& rhs)
	: CGimmickBase{rhs}
{
}

CGearWall::~CGearWall()
{
	Free();
}

void CGearWall::Execute(_bool bExecute)
{
	if (!m_bExecuted)
	{
		m_bExecuted = bExecute;

		m_pModelCom->Set_AnimIndex(0, false);

		m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_Beat();
	}
}

HRESULT CGearWall::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_GearWall"), TEXT("../Bin/Resources/Models/Map/Gimmick/GearWall.fbx"),
			MODEL_TYPE::ANIM, fMat)
	);

	return S_OK;
}

HRESULT CGearWall::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(2, false);

	return S_OK;
}

void CGearWall::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
		_float fPlayTime = m_pModelCom->Get_Animation_PlayTime(0);
		m_fAnimPlayTime = fPlayTime / fBeat * 0.25f;
		_float4x4 fMat{};
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.1f, fMat);

		m_bInit = true;
	}
}

void CGearWall::Tick(_float fTimeDelta)
{
	_float4x4 fMat{};

	if (m_bExecuted)
	{
		if (!m_bJustBeat)
		{
			_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
			if (m_iPrevBeat != iCurBeat)
				m_bJustBeat = true;
		}

		else
			m_pModelCom->Play_Animation(fTimeDelta, m_fAnimPlayTime * fTimeDelta, 0.1f, fMat);

		if (m_pModelCom->Get_Finished())
		{
			m_bDone = true;
			m_bExecuted = false;
		}
	}

}

void CGearWall::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CGearWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ATOS_Texture", (_uint)i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Normal_Texture", (_uint)i, aiTextureType_NORMALS)))
		//	return E_FAIL;
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

		m_pShaderCom->Begin(0);
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CGearWall::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GearWall"), TEXT("Model_Com")));

	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysXCollider_Com")));

	PxVec3 BoxDimension{ 0.2f, 3.f, 4.f };
	PxBoxGeometry Box{ BoxDimension };
	string ColliderTag = m_strGimmickID;
	m_pPhysXCollider->createStaticShapeActor(Box, ColliderTag.c_str());

	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
	Transpose = XMMatrixMultiply(XMMatrixTranslation(0.2f, 2.f, 0.f), Transpose);
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

	PxTransform px = m_pPhysXCollider->Get_Static_RigidBody()->getGlobalPose();
	PxTransform transform{ pxWorld };
	m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

	if (!m_pShaderCom || !m_pModelCom || !m_pPhysXCollider)
		assert(false);

	return S_OK;
}

HRESULT CGearWall::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//    return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CGearWall> CGearWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CGearWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGearWall(pDevice, pContext)
		{}
	};

	shared_ptr<CGearWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGearWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CGearWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CGearWall
	{
		MakeSharedEnabler(const CGearWall& rhs) : CGearWall(rhs)
		{}
	};

	shared_ptr<CGearWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGearWall"));
		assert(false);
	}

	return pInstance;
}

void CGearWall::Free()
{
}
