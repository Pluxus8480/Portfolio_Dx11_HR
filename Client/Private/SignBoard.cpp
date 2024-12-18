#include "SignBoard.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "CombatManager.h"

wstring CSignBoard::ObjID = TEXT("CSignBoard");
CSignBoard::CSignBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CSignBoard::CSignBoard(const CSignBoard& rhs)
    : CGameObject{rhs}, m_vecTextureTags{ rhs.m_vecTextureTags }
{
}

CSignBoard::~CSignBoard()
{
    Free();
}

void CSignBoard::Set_TextureIndex(_uint iTextureIndex)
{
	m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, m_vecTextureTags[iTextureIndex], TEXT("Texture0_Com"))));
	m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Soundwave"), TEXT("Texture1_Com"))));
	m_iTextureIndex = 0;

	assert(m_vecTextureCom[0]);
	assert(m_vecTextureCom[1]);
}

HRESULT CSignBoard::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_BigSignFrame"), TEXT("../Bin/Resources/Models/Map/General/SM_SignsFrameA_IZ_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign01"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_h02.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign02"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_h01.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign03"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_d08.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign04"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_d07_horizontal.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign05"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_d06_horizontal.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign06"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_d05_horizontal.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign07"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_d03_horizontal.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign08"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_b03.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign09"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_b01_horizontal.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign10"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_a05.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign11"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_a03_horizontal.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign12"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_a02_horizontal.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_AdSign13"), TEXT("../Bin/Resources/Models/Map/General/t_designsignboard_a01.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Soundwave"), TEXT("../Bin/Resources/Models/Map/General/T_DMG_Monitor_wall_sound_wave_4096.dds"))
	);
	
	m_vecTextureTags.assign({
	TEXT("Prototype_Component_Texture_AdSign01"),
	TEXT("Prototype_Component_Texture_AdSign02"),
	TEXT("Prototype_Component_Texture_AdSign03"),
	TEXT("Prototype_Component_Texture_AdSign04"),
	TEXT("Prototype_Component_Texture_AdSign05"),
	TEXT("Prototype_Component_Texture_AdSign06"),
	TEXT("Prototype_Component_Texture_AdSign07"),
	TEXT("Prototype_Component_Texture_AdSign08"),
	TEXT("Prototype_Component_Texture_AdSign09"),
	TEXT("Prototype_Component_Texture_AdSign10"),
	TEXT("Prototype_Component_Texture_AdSign11"),
	TEXT("Prototype_Component_Texture_AdSign12"),
	TEXT("Prototype_Component_Texture_AdSign13"),
	TEXT("Prototype_Component_Texture_Soundwave"),
	});

	return S_OK;
}

HRESULT CSignBoard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

    return S_OK;
}

void CSignBoard::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
		m_fSecPerFrame = fBeat / (_float)m_iMaxFrame;
	
		m_bInit = false;
	}
}

void CSignBoard::Tick(_float fTimeDelta)
{
	auto pCombatManager = CCombat_Manager::Get_Instance();

	if (!m_bSoundWave && pCombatManager->Get_Is_Combat())
	{
		m_bSoundWave = true;
		m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	}
	else if (m_bSoundWave)
	{
		if (m_iTextureIndex == 0)
		{
			_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
			if (iCurBeat != m_iPrevBeat)
				m_iTextureIndex = 1;
		}
		if (pCombatManager->Get_Combat_End())
		{
			m_bSoundWave = false;
			m_iFrameIndex = 0;
			m_iTextureIndex = 0;
		}
		else
		{
			m_fTime += fTimeDelta;

			if (m_fTime > m_fSecPerFrame)
			{
				m_fTime -= m_fTime;

				++m_iFrameIndex;
				m_iFrameIndex %= m_iMaxFrame;
			}
		}
	}
}

void CSignBoard::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CSignBoard::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_uint iPassIndex = (_uint)MESH_PASS::Default;

		if (i == 0)
		{
			m_vecTextureCom[m_iTextureIndex]->Bind_ShaderResource(m_pShaderCom, "g_Texture");

			if (m_bSoundWave)
			{
				XMINT2 vDivide = { 4, 4 };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &vDivide, sizeof(XMINT2))))
					return E_FAIL;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex, sizeof(_int))))
					return E_FAIL;

				iPassIndex = (_uint)MESH_PASS::Frame;
			}
		}
			
		else
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(iPassIndex);
		m_pModelCom->Render((_uint)i);
	}
	
    return S_OK;
}

void CSignBoard::Edit_Attribute()
{
	ImGui::InputInt("Texture Index", &m_iTextureIndex);

	m_iTextureIndex %= m_vecTextureCom.size();
}

void CSignBoard::Save_Data(ofstream& os)
{
	write_typed_data(os, m_iTextureIndex);
}

void CSignBoard::Load_Data(ifstream& is)
{
	read_typed_data(is, m_iTextureIndex);

	if (CMapManager::Get_Instance()->Get_NextLevel() != LEVEL_MAPTOOL)
	{
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, m_vecTextureTags[m_iTextureIndex], TEXT("Texture0_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Soundwave"), TEXT("Texture1_Com"))));
		m_iTextureIndex = 0;

		assert(m_vecTextureCom[0]);
		assert(m_vecTextureCom[1]);
	}
}

HRESULT CSignBoard::Ready_Components(const MISCOBJ_DESC* pDesc)
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_BigSignFrame"), TEXT("Model_Com")));
	
	if (CMapManager::Get_Instance()->Get_NextLevel() == LEVEL_MAPTOOL)
	{
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign01"), TEXT("Texture01_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign02"), TEXT("Texture02_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign03"), TEXT("Texture03_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign04"), TEXT("Texture04_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign05"), TEXT("Texture05_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign06"), TEXT("Texture06_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign07"), TEXT("Texture07_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign08"), TEXT("Texture08_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign09"), TEXT("Texture09_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign10"), TEXT("Texture10_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign11"), TEXT("Texture11_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign12"), TEXT("Texture12_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AdSign13"), TEXT("Texture13_Com"))));
		m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Soundwave"), TEXT("Texture14_Com"))));

		for (auto& pTexture : m_vecTextureCom)
			assert(pTexture);
	}
	
	if (!m_pShaderCom || !m_pModelCom)
		assert(false);


    return S_OK;
}

HRESULT CSignBoard::Bind_ShaderResources()
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

shared_ptr<CSignBoard> CSignBoard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSignBoard
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSignBoard(pDevice, pContext)
		{}
	};


	shared_ptr<CSignBoard> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSignBoard"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CSignBoard::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSignBoard
	{
		MakeSharedEnabler(const CSignBoard& rhs) : CSignBoard(rhs)
		{}
	};

	shared_ptr<CSignBoard> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSignBoard"));
		assert(false);
	}

	return pInstance;
}

void CSignBoard::Free()
{
}
