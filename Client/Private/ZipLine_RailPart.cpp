#include "ZipLine_RailPart.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "Cine_Manager.h"


wstring CZipLine_RailPart::ObjID = TEXT("CZipLine_RailPart");
CZipLine_RailPart::CZipLine_RailPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{pDevice, pContext}
{
}

CZipLine_RailPart::CZipLine_RailPart(const CZipLine_RailPart& rhs)
    : CPartObject{rhs}, m_LocalWorld{rhs.m_LocalWorld}
{
}

CZipLine_RailPart::~CZipLine_RailPart()
{
    Free();
}

_bool CZipLine_RailPart::IsIn_RailPart(_fvector vPosition)
{
	_vector vLocalPos = {};
	vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_vector vVector = XMLoadFloat3(&m_vLocalPoint[RIGHT]) - XMLoadFloat3(&m_vLocalPoint[LEFT]);

	//_bool bResult = true;

	//if (abs(XMVectorGetX(vLocalPos) - XMVectorGetX(vVector)) > FLT_MIN)
	//	return false;
	//if (abs(XMVectorGetY(vLocalPos) - XMVectorGetY(vVector)) > FLT_MIN)
	//	return false;
	//if (XMVectorGetZ(vLocalPos) > XMVectorGetZ(vVector))
	//	return false;

	if (XMVectorGetX(XMVector3Length(vLocalPos)) > XMVectorGetX(XMVector3Length(vVector)))
		return false;
	else
		return true;

	return true;
}

_vector CZipLine_RailPart::Adjust_Position(_fvector vPosition)
{
	_vector vLocal = {};
	vLocal = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	_float vDist = XMVectorGetX(XMVector3Length(vLocal));

	_vector vNewPos = {0.f, 0.f, vDist, 1.f};
	vNewPos = XMVector3TransformCoord(vNewPos, XMLoadFloat4x4(&m_WorldMatrix));

	return vNewPos;
}

_vector CZipLine_RailPart::Calc_WorldDirection()
{
	_vector vVector = XMLoadFloat3(&m_vLocalPoint[RIGHT]) - XMLoadFloat3(&m_vLocalPoint[LEFT]);

	_vector vWorldVector = XMVector3TransformNormal(vVector, XMLoadFloat4x4(&m_WorldMatrix));
	vWorldVector = XMVector3Normalize(vWorldVector);

	return vWorldVector;
}

HRESULT CZipLine_RailPart::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_ZipLineRail"), TEXT("../Bin/Resources/Models/Map/Gimmick/SM_ZipLineRailA_WB_03.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

    return S_OK;
}

HRESULT CZipLine_RailPart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Components();

	if (pArg)
	{
		RAILPART_DESC* pDesc = static_cast<RAILPART_DESC*>(pArg);
		m_LocalWorld = pDesc->vLocalMatrix;
		m_bRapidCurve = pDesc->bRapidCurve;
	}

	m_pTransformCom->Set_WorldMatrix(m_LocalWorld);

    return S_OK;
}

void CZipLine_RailPart::PriorityTick(_float fTimeDelta)
{
}

void CZipLine_RailPart::Tick(_float fTimeDelta)
{
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());

	_matrix WorldMat = XMLoadFloat4x4(&m_WorldMatrix);
	XMStoreFloat3(&m_vWorldPoint[LEFT], XMVector3TransformCoord(XMLoadFloat3(&m_vLocalPoint[LEFT]), WorldMat));
	XMStoreFloat3(&m_vWorldPoint[RIGHT], XMVector3TransformCoord(XMLoadFloat3(&m_vLocalPoint[RIGHT]), WorldMat));

#ifdef _DEBUG
	m_pColliderCom[LEFT]->Update(XMLoadFloat4x4(&m_WorldMatrix));
	m_pColliderCom[RIGHT]->Update(XMLoadFloat4x4(&m_WorldMatrix));
#endif
}

void CZipLine_RailPart::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	//if(pGI->isIn_Frustum_WorldSpace())

	if (m_bRender)
		pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CZipLine_RailPart::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Vector4("g_vColor", &m_vYellowColor)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)MESH_PASS::DiffuseColor);
		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom[LEFT])
		m_pColliderCom[LEFT]->Render();
	if (m_pColliderCom[RIGHT])
		m_pColliderCom[RIGHT]->Render();
#endif

    return S_OK;
}

void CZipLine_RailPart::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ZipLineRail"), TEXT("Model_Com")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	_float3 vScale = m_pTransformCom->Get_Scale();

	m_vLocalPoint[LEFT] = { 0.f, 0.f, 0.f };
	m_vLocalPoint[RIGHT] = { 0.f, 0.f, 8.f * vScale.z };

#ifdef _DEBUG
	CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
	sphereDesc.fRadius = 0.5f;
	sphereDesc.vCenter = m_vLocalPoint[LEFT];
	m_pColliderCom[LEFT] = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider01_Com"), &sphereDesc));
	assert(m_pColliderCom[LEFT]);

	sphereDesc.vCenter = m_vLocalPoint[RIGHT];
	m_pColliderCom[RIGHT] = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider02_Com"), &sphereDesc));
	assert(m_pColliderCom[RIGHT]);
#endif
}

HRESULT CZipLine_RailPart::Bind_ShaderResources()
{
	auto pGI = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CZipLine_RailPart> CZipLine_RailPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CZipLine_RailPart
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CZipLine_RailPart(pDevice, pContext)
		{}
	};


	shared_ptr<CZipLine_RailPart> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CZipLine_RailPart"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CZipLine_RailPart::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CZipLine_RailPart
	{
		MakeSharedEnabler(const CZipLine_RailPart& rhs) : CZipLine_RailPart(rhs)
		{}
	};

	shared_ptr<CZipLine_RailPart> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CZipLine_RailPart"));
		assert(false);
	}

	return pInstance;
}

void CZipLine_RailPart::Free()
{
}
