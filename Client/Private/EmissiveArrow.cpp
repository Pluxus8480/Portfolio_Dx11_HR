#include "EmissiveArrow.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"

wstring CEmissiveArrow::ObjID = TEXT("CEmissiveArrow");
CEmissiveArrow::CEmissiveArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CEmissiveArrow::CEmissiveArrow(const CEmissiveArrow& rhs)
	: CGameObject{rhs}
{
}

CEmissiveArrow::~CEmissiveArrow()
{
	Free();
}

HRESULT CEmissiveArrow::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_CommonArrow"), 
			TEXT("../Bin/Resources/Models/Map/Gimmick/SM_CommonArrowA_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSimple_Yellow"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSimpleA_E_Yellow.png"))
	);
	
	return S_OK;
}

HRESULT CEmissiveArrow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	m_iFirstBeat = m_iPrevBeat;

	return S_OK;
}

void CEmissiveArrow::PriorityTick(_float fTimeDelta)
{
}

void CEmissiveArrow::Tick(_float fTimeDelta)
{
	_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	if (m_iPrevBeat != iCurBeat)
	{
		m_iPrevBeat = iCurBeat;

		if (iCurBeat == m_iFirstBeat + 1)
			m_bEmissive[0] = true;
		else if (iCurBeat == m_iFirstBeat + 2)
			m_bEmissive[1] = true;
		else if (iCurBeat == m_iFirstBeat + 3)
			m_bEmissive[2] = true;
		else if (iCurBeat == m_iFirstBeat + 4)
			m_bEmissive[3] = true;
		else if (iCurBeat == m_iFirstBeat + 6)
		{
			for (size_t i = 0; i < 4; ++i)
				m_bEmissive[i] = false;
			m_iFirstBeat = iCurBeat;
		}
	}
}

void CEmissiveArrow::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	if (false == pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 20.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CEmissiveArrow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		_bool bEmissive = false;
		if (i >= 1 && m_bEmissive[i - 1])
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
				return E_FAIL;

			bEmissive = true;
		}

		if(bEmissive)
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		else
			m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEmissiveArrow::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CommonArrow"), TEXT("Model_Com")));
	m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiSimple_Yellow"), TEXT("Texture_Com")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CEmissiveArrow::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CEmissiveArrow> CEmissiveArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEmissiveArrow
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEmissiveArrow(pDevice, pContext)
		{}
	};

	shared_ptr<CEmissiveArrow> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEmissiveArrow"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CEmissiveArrow::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEmissiveArrow
	{
		MakeSharedEnabler(const CEmissiveArrow& rhs) : CEmissiveArrow(rhs)
		{}
	};

	shared_ptr<CEmissiveArrow> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEmissiveArrow"));
		assert(false);
	}

	return pInstance;
}

void CEmissiveArrow::Free()
{
}
