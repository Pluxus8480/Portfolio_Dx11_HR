#include "LaserPillar.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Beat_Manager.h"
#include "PodLaser.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CLaserPillar::ObjID = TEXT("CLaserPillar");

CLaserPillar::CLaserPillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CLaserPillar::CLaserPillar(const CLaserPillar& rhs)
	: CGimmickBase{ rhs }
{
}

CLaserPillar::~CLaserPillar()
{
	Free();
}

void CLaserPillar::Execute(_bool bExecute)
{
	m_bExecuted = bExecute;
	if(m_bExecuted)
	{
		//pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Loop", m_pTransformCom->Get_Position(), XMVectorZero(), true);
	}
}

HRESULT CLaserPillar::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LaserPillar"), TEXT("../Bin/Resources/Models/Map/Gimmick/LaserPillar.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_Shoot"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_9.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_Loop"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_27.ogg")));


	return S_OK;
}

HRESULT CLaserPillar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pLaser = static_pointer_cast<CPodLaser>(GAMEINSTANCE->Clone_Object(CPodLaser::ObjID, nullptr));

	return S_OK;
}

void CLaserPillar::PriorityTick(_float fTimeDelta)
{
	m_pLaser->PriorityTick(fTimeDelta);
}

void CLaserPillar::Tick(_float fTimeDelta)
{
	auto pGameInstance = m_pGameInstance.lock();
	if (m_bExecuted)
	{
		_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
		}
	}
	else
	{
		//pGameInstance->Stop_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Loop");
		//pGameInstance->Stop_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot");
	}
	m_pLaser->Tick(fTimeDelta);
	_matrix LaserMat = XMMatrixAffineTransformation(XMVectorSet(0.1f, 24.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XM_PIDIV2),
		XMVectorSet(12.f, -2.6f, 0.f, 1.f))
		* m_pTransformCom->Get_WorldMatrix_XMMat();
	m_pLaser->Get_Transform()->Set_WorldMatrix(LaserMat);
	m_pColliderCom->Update(LaserMat);
}

void CLaserPillar::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	if (m_bExecuted)
	{
		m_pLaser->LateTick(fTimeDelta);
		GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_GIMMICK, m_pColliderCom);
	}
}

HRESULT CLaserPillar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}
		if (bNoEmit)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);

		m_pModelCom->Render((_uint)i);
	}
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CLaserPillar::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	DamageInfo Info{};
	Info.eAttackType = EAttackType::Base;
	Info.pObjectOwner = shared_from_this();
	Info.eAttack_Direction = EAttackDirection::DEFAULT;
	Info.fAmountDamage = 10.f;
	pOther->Get_Owner()->TakeDamage(Info);
}

void CLaserPillar::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CLaserPillar::Edit_Attribute()
{
}

HRESULT CLaserPillar::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LaserPillar"), TEXT("Model_Com")));
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vExtents = { 1.f, 1.f, 0.1f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	if (!m_pShaderCom || !m_pModelCom || !m_pColliderCom)
		assert(false);

	return S_OK;
}

HRESULT CLaserPillar::Bind_ShaderResources()
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


shared_ptr<CLaserPillar> CLaserPillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLaserPillar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLaserPillar(pDevice, pContext)
		{}
	};

	shared_ptr<CLaserPillar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLaserPillar"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CLaserPillar::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLaserPillar
	{
		MakeSharedEnabler(const CLaserPillar& rhs) : CLaserPillar(rhs)
		{}
	};

	shared_ptr<CLaserPillar> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLaserPillar"));
		assert(false);
	}

	return pInstance;
}

void CLaserPillar::Free()
{
}
