#include "InvisibleCylinderWall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "Texture.h"
#include "Beat_Manager.h"
#include "VIBuffer_Rect.h"
#include "Cine_Manager.h"
#include "CombatManager.h"

wstring CInvisibleCylinderWall::ObjID = TEXT("CInvisibleCylinderWall");

CInvisibleCylinderWall::CInvisibleCylinderWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CInvisibleCylinderWall::CInvisibleCylinderWall(const CInvisibleCylinderWall& rhs)
	: CGimmickBase{ rhs }
{
}

CInvisibleCylinderWall::~CInvisibleCylinderWall()
{
	Free();
}

void CInvisibleCylinderWall::Execute(_bool bExecute)
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

HRESULT CInvisibleCylinderWall::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(1.f, 1.f, 1.f));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_InvisibleWall_Texture"), TEXT("../Bin/Resources/Textures/InvisibleWall/t_InvisibleWall.png"), 1));
	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_InvisibleWall_ScratchTexture"), TEXT("../Bin/Resources/Textures/InvisibleWall/t_Scratch.png"), 1));
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Cylinder"), TEXT("../Bin/Resources/Models/Effect/InvisibleWall/Cylinder.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);


	return S_OK;
}

HRESULT CInvisibleCylinderWall::Initialize(void* pArg)
{
	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	pDesc->fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInvisibleCylinderWall::PriorityTick(_float fTimeDelta)
{
}

void CInvisibleCylinderWall::Tick(_float fTimeDelta)
{

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
	}
}

void CInvisibleCylinderWall::LateTick(_float fTimeDelta)
{
	if(CCine_Manager::Get_Instance()->IsInGame())
		GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());

}

HRESULT CInvisibleCylinderWall::Render()
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
	_float2 vUVMultX = { vScale.x * XM_PI, 1.f };

	m_pShaderCom->Bind_RawValue("g_vUVMultiplier", &vUVMultX, sizeof(_float2));//이거 각도 따라 달리 해야함
	m_pShaderCom->Bind_Vector4("g_vThreshold", &m_vThreshold);
	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_PASS::InvisibleWall)))
		return E_FAIL;

	_uint iNumMesh = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMesh; ++i)
	{

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

void CInvisibleCylinderWall::Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CInvisibleCylinderWall::Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller)
{
}

void CInvisibleCylinderWall::Edit_Attribute()
{
}

HRESULT CInvisibleCylinderWall::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder"), TEXT("Model_Com")));
		//
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_InvisibleWall_Texture"), TEXT("Com_Texture")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_InvisibleWall_ScratchTexture"), TEXT("Com_Noise_Texture")));
	assert(m_pNoiseTextureCom);
	assert(m_pTextureCom);





	if (!m_pShaderCom )
		assert(false);

	return S_OK;
}

HRESULT CInvisibleCylinderWall::Bind_ShaderResources()
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


shared_ptr<CInvisibleCylinderWall> CInvisibleCylinderWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CInvisibleCylinderWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CInvisibleCylinderWall(pDevice, pContext)
		{}
	};

	shared_ptr<CInvisibleCylinderWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CInvisibleCylinderWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CInvisibleCylinderWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CInvisibleCylinderWall
	{
		MakeSharedEnabler(const CInvisibleCylinderWall& rhs) : CInvisibleCylinderWall(rhs)
		{}
	};

	shared_ptr<CInvisibleCylinderWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CInvisibleCylinderWall"));
		assert(false);
	}

	return pInstance;
}

void CInvisibleCylinderWall::Free()
{
}
