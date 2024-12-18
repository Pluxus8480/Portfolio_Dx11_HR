#include "LaserPod.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "PhysxCollider.h"
#include "PodLaser.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"

wstring CLaserPod::ObjID = TEXT("CLaserPod");

CLaserPod::CLaserPod(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLaserPod::CLaserPod(const CLaserPod& rhs)
	: CGameObject{ rhs }
{
}

CLaserPod::~CLaserPod()
{
	Free();
}

HRESULT CLaserPod::Set_ModelComponent(const string& strPrototypeTag)
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

HRESULT CLaserPod::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_float4x4 PreTransform;
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(0.00003f, 0.00003f, 0.00003f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, -XM_PIDIV2), XMVectorSet(0.f, 0.f, 0.f, 1.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LaserPod"), TEXT("../Bin/Resources/Models/Map/Gimmick/LaserPod.fbx"), MODEL_TYPE::NONANIM, PreTransform)
	);
	//m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_Shoot"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_9.ogg")));
	//m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_Loop"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_27.ogg")));



	return S_OK;
}

HRESULT CLaserPod::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	//_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	//m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	m_pLaser = static_pointer_cast<CPodLaser>(GAMEINSTANCE->Clone_Object(CPodLaser::ObjID, nullptr));

	return S_OK;
}

void CLaserPod::PriorityTick(_float fTimeDelta)
{
	m_pLaser->PriorityTick(fTimeDelta);
}

void CLaserPod::Tick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
	}
	m_pLaser->Tick(fTimeDelta);
	if(m_bShoot)
	{
		if (m_pParent)
		{
			_matrix mat = m_pTransformCom->Get_WorldMatrix_XMMat() * XMLoadFloat4x4(m_pParent);
			m_pLaser->Get_Transform()->Set_Position(mat.r[3]);
			m_pColliderCom->Update(mat);
		}
	}

}

void CLaserPod::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	if (m_bShoot)
	{
		m_pLaser->LateTick(fTimeDelta);
		GAMEINSTANCE->Add_Collider(COLLIDER_GROUP::COLLISION_GIMMICK, m_pColliderCom);
	}
}

HRESULT CLaserPod::Render()
{
	if (!m_pModelCom)
		return S_OK;

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	if (m_pParent)
	{
		XMStoreFloat4x4(&Temp, m_pTransformCom->Get_WorldMatrix_XMMat() * XMLoadFloat4x4(m_pParent));
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp);
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	//m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)0, aiTextureType_EMISSIVE);

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
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render(i);
	}
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG



	return S_OK;
}

void CLaserPod::Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
	DamageInfo Info{};
	Info.eAttackType = EAttackType::Base;
	Info.pObjectOwner = shared_from_this();
	Info.eAttack_Direction = EAttackDirection::DEFAULT;
	Info.fAmountDamage = 10.f;
	pOther->Get_Owner()->TakeDamage(Info);
}

void CLaserPod::Collision(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller)
{
}



HRESULT CLaserPod::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LaserPod"), TEXT("Com_Model")));
	CBounding_AABB::BOUNDING_AABB_DESC obbDesc{};
	obbDesc.vCenter = { 0.f, -12.f, 0.f };
	obbDesc.vExtents = { 0.1f, 24.f, 0.1f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &obbDesc));

	if (!m_pModelCom)
		assert(false);

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);


	return S_OK;
}

shared_ptr<CLaserPod> CLaserPod::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLaserPod
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLaserPod(pDevice, pContext)
		{}
	};


	shared_ptr<CLaserPod> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLaserPod"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CLaserPod::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLaserPod
	{
		MakeSharedEnabler(const CLaserPod& rhs) : CLaserPod(rhs)
		{}
	};


	shared_ptr<CLaserPod> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLaserPod"));
		assert(false);
	}

	return pInstance;
}

void CLaserPod::Free()
{
}
