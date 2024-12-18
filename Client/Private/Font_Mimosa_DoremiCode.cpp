#include "Font_Mimosa_DoremiCode.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Effect_Union.h"

wstring CFont_Mimosa_DoremiCode::ObjID = TEXT("CFont_Mimosa_DoremiCode");

CFont_Mimosa_DoremiCode::CFont_Mimosa_DoremiCode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CFont_Mimosa_DoremiCode::ObjID);
}

CFont_Mimosa_DoremiCode::CFont_Mimosa_DoremiCode(const CFont_Mimosa_DoremiCode& rhs)
	: CEffect_Base(rhs)
{
}

CFont_Mimosa_DoremiCode::~CFont_Mimosa_DoremiCode()
{
	Free();
}

HRESULT CFont_Mimosa_DoremiCode::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	LocalTransformMatrix = XMMatrixScaling(0.01f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_Do"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Font/Mimosa_Do.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_Re"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Font/Mimosa_Re.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_Mi"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Font/Mimosa_Mi.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_Fa"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Font/Mimosa_Fa.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_So"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Font/Mimosa_So.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_La"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Font/Mimosa_La.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Mimosa_Ti"), TEXT("../Bin/Resources/Models/Effect/Mimosa/Font/Mimosa_Ti.fbx"), MODEL_TYPE::NONANIM, preMatrix)
	);



	return S_OK;
}

HRESULT CFont_Mimosa_DoremiCode::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	m_pTransformCom->Set_Scale(_float3{ 48.f,48.f,48.f });
	m_vEffectColorPresets.push_back(_float4{ 1.f, 0.5f, 0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.8f, 0.9f, 0.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.f,0.f,0.f,1.f });
	m_fEffectUvFlowXSpeed = 1.f;
	m_fEffectUvFlowYSpeed = 1.f;
	m_bUseEffectUvFlowX = true;
	m_bUseEffectUvFlowY = true;
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CFont_Mimosa_DoremiCode::PriorityTick(_float fTimeDelta)
{
	PARENTCHECK
}

void CFont_Mimosa_DoremiCode::Tick(_float fTimeDelta)
{
	PARENTCHECK
		EffectTick(fTimeDelta);
}

void CFont_Mimosa_DoremiCode::LateTick(_float fTimeDelta)
{
	PARENTCHECK
		if (GetEffectIsView()) {
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
			//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
		}
}

HRESULT CFont_Mimosa_DoremiCode::Render()
{
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = max(m_fEffectTimePercent * 0.5f + 0.5f, 0.85f);
	XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale, fScale, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());

	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(&Temp), XMLoadFloat4x4(m_pParentWorld)));

		ParentMultipleWorld = GAMEINSTANCE->Matrix_BillBoard(ParentMultipleWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
			return E_FAIL;
	}
	else {
		Temp = GAMEINSTANCE->Matrix_BillBoard(Temp);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp)))
			return E_FAIL;
	}

	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", abs((m_fEffectTimePercent - 0.5f) * 2.f));

	_int iMeshNum = (_int)m_fEffectAlpha;

	if (iMeshNum >= m_vecModelCom.size())
		iMeshNum = 0;

	_uint iNumMeshes = m_vecModelCom[iMeshNum]->Get_NumMeshes();
	//m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0);
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_vecModelCom[iMeshNum]->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			continue;*/

		m_pShaderCom->Begin((_uint)MESH_PASS::Default);

		
		m_vecModelCom[iMeshNum]->Render((_uint)i);
	}

	return S_OK;
}

void CFont_Mimosa_DoremiCode::EffectStartCallBack()
{
}

void CFont_Mimosa_DoremiCode::EffectEndCallBack()
{
}

void CFont_Mimosa_DoremiCode::EffectSoftEnd()
{
}

void CFont_Mimosa_DoremiCode::EffectHardEnd()
{
}

HRESULT CFont_Mimosa_DoremiCode::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	/* For.Com_Model */
	//m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Do"), TEXT("Com_Model")));

	m_vecModelCom.push_back(static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Do"), TEXT("Com_Model"))));
	m_vecModelCom.push_back(static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Re"), TEXT("Com_Model1"))));
	m_vecModelCom.push_back(static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Mi"), TEXT("Com_Model2"))));
	m_vecModelCom.push_back(static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Fa"), TEXT("Com_Model3"))));
	m_vecModelCom.push_back(static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_So"), TEXT("Com_Model4"))));
	m_vecModelCom.push_back(static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_La"), TEXT("Com_Model5"))));
	m_vecModelCom.push_back(static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mimosa_Ti"), TEXT("Com_Model6"))));
	
	//m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Slash_01_Noise"), TEXT("Com_Texture")));
	//m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Fire_Scratch"), TEXT("Com_Texture")));

	if (!m_pShaderCom)
		assert(false);

	return S_OK;
}

shared_ptr<CFont_Mimosa_DoremiCode> CFont_Mimosa_DoremiCode::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFont_Mimosa_DoremiCode
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFont_Mimosa_DoremiCode(pDevice, pContext)
		{}
	};

	shared_ptr<CFont_Mimosa_DoremiCode> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFont_Mimosa_DoremiCode"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CFont_Mimosa_DoremiCode::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFont_Mimosa_DoremiCode
	{
		MakeSharedEnabler(const CFont_Mimosa_DoremiCode& rhs) : CFont_Mimosa_DoremiCode(rhs) {}
	};

	shared_ptr<CFont_Mimosa_DoremiCode> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFont_Mimosa_DoremiCode"));
		assert(false);
	}

	return pInstance;
}

void CFont_Mimosa_DoremiCode::Free()
{
}