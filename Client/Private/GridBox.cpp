#include "GridBox.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "PhysxCollider.h"


CGridBox::CGridBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CGridBox::CGridBox(const CGridBox& rhs)
	: CGameObject(rhs)
{
}

CGridBox::~CGridBox()
{
	Free();
}

HRESULT CGridBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGridBox::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(50.f, 10.f, 50.f, 1.f));
	m_pTransformCom->Set_Position(XMVectorSet(0.f, 5.f, 0.f, 1.f));
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CGridBox::PriorityTick(_float fTimeDelta)
{
}

void CGridBox::Tick(_float fTimeDelta)
{
	_matrix Transpose =  m_pTransformCom->Get_WorldMatrix_XMMat();
	Transpose.r[0] = XMVector3Normalize(Transpose.r[0]);
	Transpose.r[1] = XMVector3Normalize(Transpose.r[1]);
	Transpose.r[2] = XMVector3Normalize(Transpose.r[2]);
	Transpose.r[3].m128_f32[1] -= 0.5f;
	PxMat44 mat;

	mat.column0.x = Transpose.r[0].m128_f32[0];
	mat.column0.y = Transpose.r[0].m128_f32[1];
	mat.column0.z = Transpose.r[0].m128_f32[2];
	mat.column0.w = Transpose.r[0].m128_f32[3];
	mat.column1.x = Transpose.r[1].m128_f32[0];
	mat.column1.y = Transpose.r[1].m128_f32[1];
	mat.column1.z = Transpose.r[1].m128_f32[2];
	mat.column1.w = Transpose.r[1].m128_f32[3];
	mat.column2.x = Transpose.r[2].m128_f32[0];
	mat.column2.y = Transpose.r[2].m128_f32[1];
	mat.column2.z = Transpose.r[2].m128_f32[2];
	mat.column2.w = Transpose.r[2].m128_f32[3];
	mat.column3.x = Transpose.r[3].m128_f32[0];
	mat.column3.y = Transpose.r[3].m128_f32[1];
	mat.column3.z = Transpose.r[3].m128_f32[2];
	mat.column3.w = Transpose.r[3].m128_f32[3];

	PxTransform transform{ mat };
	m_pPhysXColliderCom->Get_Static_RigidBody()->setGlobalPose(transform);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance.lock()->Get_CamPosition_Vector());

}

void CGridBox::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CGridBox::Render()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGridBox::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader")));

	/* For.Com_Texture */
	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GridBox"), TEXT("Com_Texture")));

	/* For.Com_VIBuffer */
	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Cube>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer")));


	m_pPhysXColliderCom = static_pointer_cast<CPhysXCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("PhysxCol_Com")));
	assert(m_pPhysXColliderCom);

	PxVec3 BoxDimension{ 25.f,0.5f,25.0f };
	_float3 vScale = m_pTransformCom->Get_Scale();
	BoxDimension.x *= vScale.x;
	BoxDimension.y *= vScale.y;
	BoxDimension.z *= vScale.z;
	PxBoxGeometry Box{ BoxDimension };
	m_pPhysXColliderCom->createStaticShapeActor(Box, "");

	if (!m_pShaderCom || !m_pTextureCom || !m_pVIBufferCom)
		assert(false);

	return S_OK;
}

shared_ptr<CGridBox> CGridBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CGridBox
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGridBox(pDevice, pContext)
		{}
	};


	shared_ptr<CGridBox> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGridBox"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CGridBox::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CGridBox
	{
		MakeSharedEnabler(const CGridBox& rhs) : CGridBox(rhs)
		{}
	};


	shared_ptr<CGridBox> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGridBox"));
		assert(false);
	}

	return pInstance;
}

void CGridBox::Free()
{
}
