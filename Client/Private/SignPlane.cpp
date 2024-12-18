#include "SignPlane.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"

wstring CSignPlane::ObjID = TEXT("CSignPlane");
CSignPlane::CSignPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CSignPlane::CSignPlane(const CSignPlane& rhs)
    : CGameObject{rhs}, m_vecTextureTags{rhs.m_vecTextureTags}, m_iDisplayIndex{rhs.m_iDisplayIndex}
{
}

CSignPlane::~CSignPlane()
{
    Free();
}

void CSignPlane::Set_TextureIndex(_uint iTextureIndex)
{
	m_iTextureIndex = iTextureIndex % m_vecTextureCom.size();
}

HRESULT CSignPlane::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_SignPlane"), TEXT("../Bin/Resources/Models/Map/General/SM_SignsMeshPlane_IZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay01"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayaa.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay02"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayab.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay03"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayac.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay04"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayad.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay05"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayae.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay06"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayaf.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay07"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayba.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay08"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplaybb.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay09"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplayca.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay10"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplaycb.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SignDisplay11"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_secd_signsdisplaycc.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SecurityLogo"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_SecD_SecurityLogo.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_VandelayLogo1"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_vandelaysigna_02.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_VandelayLogo2"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/t_vandelaysignd_01.dds"))
	);
	
	m_vecTextureTags.assign({
		TEXT("Prototype_Component_Texture_SignDisplay01"),
		TEXT("Prototype_Component_Texture_SignDisplay02"),
		TEXT("Prototype_Component_Texture_SignDisplay03"),
		TEXT("Prototype_Component_Texture_SignDisplay04"),
		TEXT("Prototype_Component_Texture_SignDisplay05"),
		TEXT("Prototype_Component_Texture_SignDisplay06"),
		TEXT("Prototype_Component_Texture_SignDisplay07"),
		TEXT("Prototype_Component_Texture_SignDisplay08"),
		TEXT("Prototype_Component_Texture_SignDisplay09"),
		TEXT("Prototype_Component_Texture_SignDisplay10"),
		TEXT("Prototype_Component_Texture_SignDisplay11"),
		TEXT("Prototype_Component_Texture_SecurityLogo"),
		TEXT("Prototype_Component_Texture_VandelayLogo1"),
		TEXT("Prototype_Component_Texture_VandelayLogo2"),
		});

	m_iDisplayIndex = 11;

	return S_OK;
}

HRESULT CSignPlane::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

    return S_OK;
}

void CSignPlane::PriorityTick(_float fTimeDelta)
{
}

void CSignPlane::Tick(_float fTimeDelta)
{
}

void CSignPlane::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CSignPlane::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (m_iTextureIndex < m_iDisplayIndex)
		{
			m_vecTextureCom[m_iTextureIndex]->Bind_ShaderResource(m_pShaderCom, "g_Texture");
			m_vecTextureCom[m_iTextureIndex]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture");

			m_pShaderCom->Begin((_uint)MESH_PASS::AlphaEmission);
		}
		else
		{
			m_vecTextureCom[m_iTextureIndex]->Bind_ShaderResource(m_pShaderCom, "g_Texture");
			m_vecTextureCom[m_iTextureIndex]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture");
			m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}

		m_pModelCom->Render((_uint)i);
	}
	
    return S_OK;
}

void CSignPlane::Edit_Attribute()
{
	ImGui::InputInt("Texture Index", &m_iTextureIndex);

	m_iTextureIndex %= m_vecTextureCom.size();
}

void CSignPlane::Save_Data(ofstream& os)
{
	write_typed_data(os, m_iTextureIndex);
}

void CSignPlane::Load_Data(ifstream& is)
{
	read_typed_data(is, m_iTextureIndex);

	if (CMapManager::Get_Instance()->Get_NextLevel() != LEVEL_MAPTOOL)
	{
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, m_vecTextureTags[m_iTextureIndex], TEXT("Texture_Com"))));
		m_iTextureIndex = 0;

		assert(m_vecTextureCom[0]);
	}
}

HRESULT CSignPlane::Ready_Components(const MISCOBJ_DESC* pDesc)
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_SignPlane"), TEXT("Model_Com")));
	
	if (CMapManager::Get_Instance()->Get_NextLevel() == LEVEL_MAPTOOL)
	{
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay01"), TEXT("Texture01_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay02"), TEXT("Texture02_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay03"), TEXT("Texture03_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay04"), TEXT("Texture04_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay05"), TEXT("Texture05_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay06"), TEXT("Texture06_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay07"), TEXT("Texture07_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay08"), TEXT("Texture08_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay09"), TEXT("Texture09_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay10"), TEXT("Texture10_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SignDisplay11"), TEXT("Texture11_Com"))));
		
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SecurityLogo"), TEXT("Texture12_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VandelayLogo1"), TEXT("Texture13_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VandelayLogo2"), TEXT("Texture14_Com"))));

		for (auto& pTexture : m_vecTextureCom)
			assert(pTexture);
	}

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

    return S_OK;
}

HRESULT CSignPlane::Bind_ShaderResources()
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

shared_ptr<CSignPlane> CSignPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSignPlane
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSignPlane(pDevice, pContext)
		{}
	};


	shared_ptr<CSignPlane> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSignPlane"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CSignPlane::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSignPlane
	{
		MakeSharedEnabler(const CSignPlane& rhs) : CSignPlane(rhs)
		{}
	};

	shared_ptr<CSignPlane> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSignPlane"));
		assert(false);
	}

	return pInstance;
}

void CSignPlane::Free()
{
}
