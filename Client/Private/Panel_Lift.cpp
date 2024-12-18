#include "Panel_Lift.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Beat_Manager.h"
#include "Panel_Warning.h"

#include "Bounding_AABB.h"

wstring CPanel_Lift::ObjID = TEXT("CPanel_Lift");

CPanel_Lift::CPanel_Lift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPanel_Lift::CPanel_Lift(const CPanel_Lift& rhs)
	: CGameObject{ rhs }
{
}

CPanel_Lift::~CPanel_Lift()
{
	Free();
}

HRESULT CPanel_Lift::Set_ModelComponent(const string& strPrototypeTag)
{
	_tchar szPrototypeTag[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CPanel_Lift::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_float4x4 PreTransform;
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(0.003333f, 0.003333f, 0.003333f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, -XM_PIDIV2, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Panel_Lift"), TEXT("../Bin/Resources/Models/Map/Gimmick/Panel_Lift.fbx"), MODEL_TYPE::NONANIM, PreTransform)
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Lift_Panel"), TEXT("../Bin/Resources/Sounds/SFX/Lift/Lift_Panel.ogg")));
	Set_PoolSize(10);

	return S_OK;
}

HRESULT CPanel_Lift::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	//m_pTransformCom->Set_Position(XMVectorSet(0.f, 28.f, 0.f, 1.f));

	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	return S_OK;
}

void CPanel_Lift::PriorityTick(_float fTimeDelta)
{
}

void CPanel_Lift::Tick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
		++m_iSoundCount;
		if(m_iSoundCount == 4)
			m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Lift_Panel");
	}
	m_fPositionZ -= 500.f * fTimeDelta;
	m_pTransformCom->Set_Position(XMVectorSetZ(m_pTransformCom->Get_Position(), m_fPositionZ));

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
	if (m_fPositionZ < -4000.f)
	{
		Dead();
	}
}

void CPanel_Lift::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	
	pGI->Add_Collider(COLLIDER_GROUP::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CPanel_Lift::Render()
{
	if (!m_pModelCom)
		return S_OK;

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

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, (aiTextureType)aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}
		if (bNoEmit)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);

		if (FAILED(m_pModelCom->Bind_Buffers((_uint)(i))))
			return E_FAIL;
		m_pModelCom->Draw((_uint)i);

	}
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CPanel_Lift::Push_From_Pool(void* pArg)
{
	m_fPositionZ = CBeat_Manager::Get_Instance()->Get_Beat() * 2000.f;
	_int iRand = m_pGameInstance.lock()->Random_Int(0, 4);

	m_pTransformCom->Set_Position(XMVectorSet((_float)(iRand - 2) * 3.f, 28.f, m_fPositionZ, 1.f));
	_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	World.r[3] = XMVectorSetZ(World.r[3], 0.f);
	_float4x4 Waring;
	XMStoreFloat4x4(&Waring, XMMatrixAffineTransformation(XMVectorSet(10.f, 20.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(XM_PIDIV2, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)) * World);
	m_pGameInstance.lock()->Push_Object_From_Pool(CPanel_Warning::ObjID, m_pGameInstance.lock()->Get_Current_LevelID(), L"Effect", nullptr, &Waring);
	m_iSoundCount = 0;

}

void CPanel_Lift::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	DamageInfo Info{};
	Info.eAttackType = EAttackType::Base;
	Info.pObjectOwner = shared_from_this();
	Info.eAttack_Direction = EAttackDirection::DEFAULT;
	Info.fAmountDamage = 10.f;
	pOther->Get_Owner()->TakeDamage(Info);
}


HRESULT CPanel_Lift::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Panel_Lift"), TEXT("Com_Model")));

	CBounding_AABB::BOUNDING_AABB_DESC desc{};
	desc.vCenter = { 0.f,2.f,0.f };
	desc.vExtents = { 2.f,4.f,5.f };
	m_pColliderCom = static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &desc));
	assert(m_pColliderCom);
	if (!m_pModelCom)
		assert(false);

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);


	return S_OK;
}

shared_ptr<CPanel_Lift> CPanel_Lift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CPanel_Lift
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPanel_Lift(pDevice, pContext)
		{}
	};


	shared_ptr<CPanel_Lift> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPanel_Lift"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CPanel_Lift::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CPanel_Lift
	{
		MakeSharedEnabler(const CPanel_Lift& rhs) : CPanel_Lift(rhs)
		{}
	};


	shared_ptr<CPanel_Lift> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPanel_Lift"));
		assert(false);
	}

	return pInstance;
}

void CPanel_Lift::Free()
{
}
