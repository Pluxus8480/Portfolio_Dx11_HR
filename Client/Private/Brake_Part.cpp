#include "Brake_Part.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "PlayerManager.h"
#include "CombatManager.h"
#include "Chai.h"
#include "Macaron.h"
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Beat_Manager.h"
#include "CUi_TramGauge.h"
#include "CUi_Mgr.h"
#include "CUi_TramMacaronSign.h"

wstring CBrake_Part::ObjID = TEXT("CBrake_Part");

CBrake_Part::CBrake_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBrake_Part::CBrake_Part(const CBrake_Part& rhs)
	: CGameObject{ rhs }
{
}

CBrake_Part::~CBrake_Part()
{
	Free();
}

HRESULT CBrake_Part::Set_ModelComponent(const string& strPrototypeTag)
{
	_tchar szPrototypeTag[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);

	m_pModelCom->Set_AnimIndex(0, true);

	return S_OK;
}


HRESULT CBrake_Part::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_float4x4 PreTransform;
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Brake_Part"), TEXT("../Bin/Resources/Models/Map/Gimmick/Brake_Part.fbx"), MODEL_TYPE::ANIM, PreTransform)
	);
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Brake_Part2"), TEXT("../Bin/Resources/Models/Map/Gimmick/Brake_Part2.fbx"), MODEL_TYPE::NONANIM, PreTransform)
	);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Brake_Attacked"), TEXT("../Bin/Resources/Sounds/SFX/Lift/Brake_Attacked.ogg")));

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("LiftFriction"), TEXT("../Bin/Resources/Sounds/SFX/Lift/LiftFriction.ogg")));

	m_List_Owning_ObjectID.push_back(CUi_TramMacaronSign::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_TramGauge::ObjID);

	return S_OK;
}

HRESULT CBrake_Part::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ANIMOBJ_DESC* pDesc = nullptr;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	_bool bRight = *((_bool*)pArg);
	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	//_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	//m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	if (pDesc)
		m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);


	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	_uint iBoneIdx = m_pModelCom->Get_BoneIdx("brakeframe_affter00");

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		if (m_pModelCom->Get_Mesh(i)->IsAffected(iBoneIdx))
		{
			m_MeshIdx.push_back(i);
		}
	}
	m_pModelCom->Set_AnimIndex(rand() %2, false);
	_float4x4 Mat;
	m_pModelCom->Play_Animation(0.01f, 0.01f, 1.f, Mat);
	m_bRight = bRight;
	if (m_bRight)
	{
		m_pTransformCom->Set_WorldMatrix(XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
			XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(-XM_PIDIV2, 0.f, 0.f), XMVectorSet(0.f, -5.f, 0.f, 1.f)));
	}
	else
	m_pTransformCom->Set_WorldMatrix(XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(XM_PIDIV2, 0.f, XM_PI), XMVectorSet(0.f, 5.f, 0.f, 1.f)));

	return S_OK;
}

void CBrake_Part::PriorityTick(_float fTimeDelta)
{
}

void CBrake_Part::Tick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
	}
	if(m_bBroken)
	{
		_float4x4 Mat;
		m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 1.f, Mat);
	}


	if(m_bOn)//조건: 마카롱이 이거 때릴 때
	{
		auto pPM = CPlayer_Manager::Get_Instance();
		auto pMacaron = static_pointer_cast<CMacaron>(pPM->Get_Assist(L"Macaron"));
		_int iHitCount = pMacaron->Get_Hit_Count();
		_int prevHp = m_iHp;

		if (m_iHp != 0)
		{
			m_iHp = 7 - iHitCount;
			if (m_iHp < prevHp && m_iHp > 0)
			{
				m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Brake_Attacked");
			}
		}
		if (m_iHp == 0)
		{
			m_MacaronUi->Dead();
			m_TramUi->Dead();

			m_bBroken = true;
		}
	}

	//static _float fDebugX = 0.f;
	//static _float fDebugY = 5.f;
	//static _float fDebugZ = 0.f;
	//if (m_pGameInstance.lock()->Get_KeyPressing(DIK_J))
	//{
	//	fDebugX += fTimeDelta;
	//}
	//if (m_pGameInstance.lock()->Get_KeyPressing(DIK_K))
	//{
	//	fDebugY += fTimeDelta;
	//}
	//if (m_pGameInstance.lock()->Get_KeyPressing(DIK_L))
	//{
	//	fDebugZ += fTimeDelta;
	//}
	//if (m_pGameInstance.lock()->Get_KeyPressing(DIK_U))
	//{
	//	fDebugX -= fTimeDelta;
	//}
	//if (m_pGameInstance.lock()->Get_KeyPressing(DIK_I))
	//{
	//	fDebugY -= fTimeDelta;
	//}
	//if (m_pGameInstance.lock()->Get_KeyPressing(DIK_O))
	//{
	//	fDebugZ -= fTimeDelta;
	//}
	//m_pTransformCom->Set_WorldMatrix(XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
	//	XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(-XM_PIDIV2, 0.f, 0.f), XMVectorSet(fDebugX, fDebugY, fDebugZ, 1.f)));
	if (m_pParent)
	{
		XMStoreFloat4x4(&m_CombMat, m_pTransformCom->Get_WorldMatrix_XMMat() * XMLoadFloat4x4(m_pParent));
	}
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombMat));
}

void CBrake_Part::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	if(m_bOn)
		pGI->Add_Collider(COLLIDER_GROUP::COLLISION_GIMMICK, m_pColliderCom);
	 

}

HRESULT CBrake_Part::Render()
{
	if (!m_pModelCom)
		return S_OK;
	auto pGameInstance = m_pGameInstance.lock();
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	if (m_pParent)
	{
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombMat);
		m_pNonAnimShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombMat);
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pNonAnimShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	{
		for (size_t i = 0; i < m_MeshIdx.size(); i++)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", m_MeshIdx[i])))
				return E_FAIL;
			_bool bNoEmit = false;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", m_MeshIdx[i], aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", m_MeshIdx[i], aiTextureType_EMISSIVE)))
			{
				bNoEmit = true;
			}
			if (bNoEmit)
				m_pShaderCom->Begin(0);
			else
				m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
			//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			//	return E_FAIL;

			m_pModelCom->Render(m_MeshIdx[i]);
		}
	}
	{
		_uint	iNumMeshes = m_pNonAnimModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{

			_bool bNoEmit = false;
			if (FAILED(m_pNonAnimModelCom->Bind_Material(m_pNonAnimShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pNonAnimModelCom->Bind_Material(m_pNonAnimShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			{
				bNoEmit = true;
			}
			if (bNoEmit)
				m_pNonAnimShaderCom->Begin(0);
			else
				m_pNonAnimShaderCom->Begin((_uint)MESH_PASS::Emission);
			//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			//	return E_FAIL;

			m_pNonAnimModelCom->Render((_uint)i);
		}
	}
#ifdef  _DEBUG
	m_pColliderCom->Render();
#endif //  _DEBUG



	return S_OK;
}

void CBrake_Part::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		auto pPM = PLAYERMANAGER;
		pPM->Callback_Event("Lift_Macaron");
		if(m_bRight)
		{
			pPM->Get_Player()->Set_Event_Position(_float4{ m_CombMat._41 - 1.f , m_CombMat._42 + 1.f, m_CombMat._43, 1.f });
		}
		else
		{
			pPM->Get_Player()->Set_Event_Position(_float4{ m_CombMat._41 + 1.f , m_CombMat._42 + 1.f, m_CombMat._43, 1.f });
		}
	}
}

void CBrake_Part::Collision_Exit(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		auto pPM = PLAYERMANAGER;
		pPM->Callback_Event("Lift_Macaron_End");
	}
}

void CBrake_Part::Brake_On()
{
	m_bOn = true;
	m_MacaronUi->Set_Active(true);
	m_TramUi->Set_Active(true);
}

void CBrake_Part::Create_Ui(BRAKE Pos, const _float4x4* BonePtr)
{
	CUi_TramGauge::Desc Desc;
	Desc.PartPos = Pos;
	Desc.pBonePtr = BonePtr;
	Desc.pHp = &m_iHp;
	m_TramUi = static_pointer_cast<CUi_TramGauge>(CUi_Mgr::Get_Instance()->Add_Clone_Return
	(CUi_TramGauge::ObjID, m_iLevelIndex, L"CUi", CUi_TramGauge::ObjID, &Desc));

	CUi_TramMacaronSign::Desc MacaronDesc;
	MacaronDesc.PartPos =  Pos;
	MacaronDesc.pBonePtr = BonePtr;
	MacaronDesc.AwareDistance = 20;
	m_MacaronUi = static_pointer_cast<CUi_TramMacaronSign>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_TramMacaronSign::ObjID, m_iLevelIndex, L"CUi", CUi_TramMacaronSign::ObjID, &MacaronDesc));

	m_MacaronUi->Set_Active(false);
	m_TramUi->Set_Active(false);
}

HRESULT CBrake_Part::Ready_Components(const ANIMOBJ_DESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
	m_pNonAnimShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader2")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Brake_Part"), TEXT("Com_Model")));
	m_pNonAnimModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Brake_Part2"), TEXT("Com_Model2")));


	CBounding_AABB::BOUNDING_AABB_DESC desc{};
	desc.vCenter = { 0.f,2.f,0.f };
	desc.vExtents = { 4.f,4.f,4.f };
	m_pColliderCom = static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &desc));
	assert(m_pColliderCom);

	assert(m_pNonAnimModelCom);
	assert(m_pNonAnimShaderCom);

	if (!m_pModelCom)
		assert(false);

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);


	return S_OK;
}

shared_ptr<CBrake_Part> CBrake_Part::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CBrake_Part
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CBrake_Part(pDevice, pContext)
		{}
	};


	shared_ptr<CBrake_Part> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBrake_Part"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CBrake_Part::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CBrake_Part
	{
		MakeSharedEnabler(const CBrake_Part& rhs) : CBrake_Part(rhs)
		{}
	};


	shared_ptr<CBrake_Part> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBrake_Part"));
		assert(false);
	}

	return pInstance;
}

void CBrake_Part::Free()
{
}
