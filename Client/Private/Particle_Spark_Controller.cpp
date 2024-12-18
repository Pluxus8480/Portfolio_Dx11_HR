#include "Particle_Spark_Controller.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Effect_Union.h"
#include "Effect_Manager.h"	

wstring CParticle_Spark_Controller::ObjID = TEXT("CParticle_Spark_Controller");

CParticle_Spark_Controller::CParticle_Spark_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CParticle_Spark_Controller::ObjID);
}

CParticle_Spark_Controller::CParticle_Spark_Controller(const CParticle_Spark_Controller& rhs)
	: CEffect_Base(rhs)
{
}

CParticle_Spark_Controller::~CParticle_Spark_Controller()
{
	Free();
}

HRESULT CParticle_Spark_Controller::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Spark_Controller::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));

	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	SetColorPresetRGB(_float4{ 1.f,0.1f,0.1f,1.f }, _float4{ 1.f,0.8f,0.1f,1.f }, _float4{ 0.8f,0.5f,1.0f,1.f });

	EffectInit();
	return S_OK;
}

void CParticle_Spark_Controller::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CParticle_Spark_Controller::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CParticle_Spark_Controller::LateTick(_float fTimeDelta)
{
	PARENTCHECK

		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
			//GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PLAYER, m_pColliderCom);
			//	//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CParticle_Spark_Controller::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	return S_OK;
}

void CParticle_Spark_Controller::EffectStartCallBack()
{
	string strEffectName = "Base_FireCracker_Yellow";
	if (!GetUseGlow()) {
		_float fSize = 1.5f;
		_float fSpeed = 1.f;

		_float4x4 matWorld = INIT_MATRIX;
		matWorld._11 = fSize;
		matWorld._22 = 1.f;
		matWorld._33 = fSize;

		matWorld._41 = 1.f;
		matWorld._42 = 0.f;

		matWorld._43 = -8.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -11.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -14.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -17.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -20.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 8.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 11.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 14.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 17.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 20.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		_matrix		LocalTransformMatrix = XMMatrixIdentity();

		LocalTransformMatrix = XMMatrixScaling(fSize, 1.f, fSize) * XMMatrixRotationX(XMConvertToRadians(180.f));

		XMStoreFloat4x4(&matWorld, LocalTransformMatrix);

		matWorld._41 = -17.f;
		matWorld._42 = 38.f;

		matWorld._43 = -8.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -11.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -14.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -17.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = -20.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 8.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 11.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 14.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 17.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 20.f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);
	}
	else {
		_float fSize = 1.f;
		_float fSpeed = 1.f;

		_float4x4 matWorld = INIT_MATRIX;
		matWorld._11 = fSize;
		matWorld._22 = 1.f;
		matWorld._33 = fSize;

		matWorld._41 = 1186.765f;
		matWorld._42 = 485.386f;
		matWorld._43 = 92.798f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._41 = 1175.942f;
		matWorld._43 = 92.798f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._41 = 1186.869f;
		matWorld._43 = 130.062f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._41 = 1176.149f;
		matWorld._43 = 130.062f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._41 = 1200.96f;
		matWorld._42 = 494.893f;
		matWorld._43 = 120.67f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 119.703f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 118.678f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 117.691f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 105.179f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 104.165f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 103.165f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);

		matWorld._43 = 102.165f;

		EFFECTMANAGER->Call_Fixed_Union_Effect(strEffectName, &matWorld, fSpeed);
	}
}

void CParticle_Spark_Controller::EffectEndCallBack()
{

}

void CParticle_Spark_Controller::EffectSoftEnd()
{

}

void CParticle_Spark_Controller::EffectHardEnd()
{

}

HRESULT CParticle_Spark_Controller::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CParticle_Spark_Controller> CParticle_Spark_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CParticle_Spark_Controller
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CParticle_Spark_Controller(pDevice, pContext)
		{}
	};


	shared_ptr<CParticle_Spark_Controller> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Spark_Controller"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CParticle_Spark_Controller::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CParticle_Spark_Controller
	{
		MakeSharedEnabler(const CParticle_Spark_Controller& rhs) : CParticle_Spark_Controller(rhs) {}
	};

	shared_ptr<CParticle_Spark_Controller> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Spark_Controller"));
		assert(false);
	}

	return pInstance;
}

void CParticle_Spark_Controller::Free()
{
}