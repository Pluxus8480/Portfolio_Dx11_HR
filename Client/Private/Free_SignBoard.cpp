#include "Free_SignBoard.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"

#include "MapManager.h"
#include "Beat_Manager.h"
#include "CombatManager.h"

wstring CFree_SignBoard::ObjID = TEXT("CFree_SignBoard");
CFree_SignBoard::CFree_SignBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CFree_SignBoard::CFree_SignBoard(const CFree_SignBoard& rhs)
	: CGameObject{ rhs }
{
}

CFree_SignBoard::~CFree_SignBoard()
{
	Free();
}

HRESULT CFree_SignBoard::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_SpectraSoundWave"), TEXT("../Bin/Resources/Models/Map/Stage_Kale/T_DMG_Monitor_wall_sound_wave_4096_04.dds"))
	);

	return S_OK;
}

HRESULT CFree_SignBoard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CFree_SignBoard::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
		m_fSecPerFrame = fBeat / (_float)m_iMaxFrame;

		m_bInit = false;
	}
}

void CFree_SignBoard::Tick(_float fTimeDelta)
{
	if (m_bBattleBoard)
	{
		auto pCombatManager = CCombat_Manager::Get_Instance();

		if (!m_bSoundWave && pCombatManager->Get_Is_Combat())
		{
			m_bSoundWave = true;
			m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
		}
		else if (m_bSoundWave)
		{
			if (!m_bChanged)
			{
				_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
				if (iCurBeat != m_iPrevBeat)
					m_bChanged = true;
			}
			else
			{
				if (pCombatManager->Get_Combat_End())
				{
					m_bSoundWave = false;
					m_iFrameIndex = 0;
					m_bChanged = false;
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
	}
}

void CFree_SignBoard::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	if (m_pModelCom)
		if (!pGI->isIn_Frustum_WorldSpace(XMLoadFloat4(&m_vModelCenter), m_fCullingRange)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CFree_SignBoard::Render()
{
	if (!m_pModelCom) return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == m_iSignIndex && m_pTextureCom)
		{
			if (!m_bChanged)
			{
				m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture");
				if (m_bEmissive)
				{
					m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture");
					m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
				}
				else
					m_pShaderCom->Begin((_uint)MESH_PASS::Default);
			}
			else if (m_bBattleBoard && m_bChanged)
			{
				m_pSoundWaveTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture");
				XMINT2 vDivide = { 4, 4 };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &vDivide, sizeof(XMINT2))))
					return E_FAIL;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex, sizeof(_int))))
					return E_FAIL;
				m_pShaderCom->Begin((_uint)MESH_PASS::Frame);
			}
		}
		else
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CFree_SignBoard::Edit_Attribute()
{
	if (ImGui::Button("Select Model"))
	{
		m_bModelSelect = true;

		fileDialog = ImGui::FileBrowser(0, "../Bin/Resources/Models/Map/");
		fileDialog.SetTypeFilters({ ".fbx" });
		fileDialog.Open();
	}

	if (m_strModelPath.length())
	{
		string strModelTag;
		_char szFileName[MAX_PATH] = {};

		_splitpath_s(m_strModelPath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		strModelTag = string(szFileName);

		ImGui::SameLine();
		ImGui::Text(strModelTag.c_str());
	}


	if (ImGui::Button("Select Texture"))
	{
		m_bModelSelect = false;

		fileDialog = ImGui::FileBrowser(0, "../Bin/Resources/Models/Map/");
		fileDialog.SetTypeFilters({ ".png", ".dds" });
		fileDialog.Open();
	}


	if (m_strTexturePath.length())
	{
		string strTextureTag;
		_char szFileName[MAX_PATH] = {};

		_splitpath_s(m_strTexturePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		strTextureTag = string(szFileName);

		ImGui::SameLine();
		ImGui::Text(strTextureTag.c_str());
	}

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		_tchar curPath[MAX_PATH] = {};
		GetCurrentDirectory(MAX_PATH, curPath);

		if (m_bModelSelect)
		{
			m_strModelPath = fileDialog.GetSelected().string();	
			m_strModelPath = filesystem::relative(m_strModelPath, curPath).string();
			Create_Model();
		}

		else
		{
			m_strTexturePath = fileDialog.GetSelected().string();
			m_strTexturePath = filesystem::relative(m_strTexturePath, curPath).string();
			Create_Texture();
		}

		fileDialog.ClearSelected();
	}

	if (m_pModelCom)
	{
		_int iNumMesh = m_pModelCom->Get_NumMeshes();
		ImGui::SliderInt("Material Index", &m_iSignIndex, 0, iNumMesh - 1);
	}

	ImGui::Checkbox("Emissive", &m_bEmissive);
	ImGui::SameLine();
	ImGui::Checkbox("Battle Board?", &m_bBattleBoard);
}

void CFree_SignBoard::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	_tchar curPath[MAX_PATH] = {};
	GetCurrentDirectory(MAX_PATH, curPath);
	m_strModelPath = filesystem::relative(m_strModelPath, curPath).string();
	m_strTexturePath = filesystem::relative(m_strTexturePath, curPath).string();

	size_t size = m_strModelPath.size();
	write_typed_data(os, m_strModelPath.size());
	os.write(&m_strModelPath[0], m_strModelPath.size());

	size = m_strTexturePath.size();
	write_typed_data(os, m_strTexturePath.size());
	os.write(&m_strTexturePath[0], m_strTexturePath.size());

	write_typed_data(os, m_iSignIndex);
	write_typed_data(os, m_bEmissive);
	write_typed_data(os, m_bBattleBoard);
}

void CFree_SignBoard::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	size_t size = {};

	read_typed_data(is, size);
	m_strModelPath.resize(size);
	is.read(&m_strModelPath[0], size);

	read_typed_data(is, size);
	m_strTexturePath.resize(size);
	is.read(&m_strTexturePath[0], size);

	read_typed_data(is, m_iSignIndex);
	read_typed_data(is, m_bEmissive);
	read_typed_data(is, m_bBattleBoard);

	Create_Model();
	Create_Texture();
}

HRESULT CFree_SignBoard::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));

	assert(m_pShaderCom);

	return S_OK;
}

HRESULT CFree_SignBoard::Bind_ShaderResources()
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

void CFree_SignBoard::Create_Model()
{
	string strModelTag;
	_char szFileName[MAX_PATH] = {};
	_char szExt[MAX_PATH] = {};

	_splitpath_s(m_strModelPath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
	strModelTag = string(szFileName);

	auto pGI = m_pGameInstance.lock();

	_uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();
	iLevelIndex = (iLevelIndex == LEVEL_MAPTOOL) ? LEVEL_STATIC : iLevelIndex;

	wstring wstrModelTag = wstring(strModelTag.begin(), strModelTag.end());
	auto pPrototype = pGI->Get_Component_Prototype(iLevelIndex, wstrModelTag);
	if (pPrototype == nullptr)
	{
		_matrix PreTransform = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		pGI->Add_Prototype(iLevelIndex, wstrModelTag, CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, m_strModelPath.c_str(), XMMatrixIdentity()));
	}

	m_pModelCom = static_pointer_cast<CModel>(Add_Component(iLevelIndex, wstrModelTag, TEXT("Model_Com")));

	assert(m_pModelCom);

	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(pGI->Get_Component_Prototype(iLevelIndex, wstrModelTag))->Get_ModelData();
	_float3 vLocalMeshCenter = pModelData->VertexInfo.vCenter;
	XMStoreFloat4(&m_vModelCenter, XMVector3TransformCoord(XMLoadFloat3(&vLocalMeshCenter), m_pTransformCom->Get_WorldMatrix_XMMat()));
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_fCullingRange = pModelData->Get_MaxOffsetRange() * (max(vScale.x, max(vScale.y, vScale.z)));
	m_fCullingRange *= 1.5f;
}

void CFree_SignBoard::Create_Texture()
{
	string strTextureTag;
	_char szFileName[MAX_PATH] = {};
	_char szExt[MAX_PATH] = {};

	_splitpath_s(m_strTexturePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
	strTextureTag = string(szFileName);

	auto pGI = m_pGameInstance.lock();

	_uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();
	iLevelIndex = (iLevelIndex == LEVEL_MAPTOOL) ? LEVEL_STATIC : iLevelIndex;

	wstring wstrTextureTag = wstring(strTextureTag.begin(), strTextureTag.end());
	wstring wstrTexturePath = wstring(m_strTexturePath.begin(), m_strTexturePath.end());
	auto pPrototype = pGI->Get_Component_Prototype(iLevelIndex, wstrTextureTag);
	if (pPrototype == nullptr)
	{
		string strTemp = string(m_strTexturePath.begin(), m_strTexturePath.end());
		pGI->Add_Prototype(iLevelIndex, wstrTextureTag, CTexture::Create(m_pDevice, m_pContext, wstrTexturePath));
	}

	m_pTextureCom = static_pointer_cast<CTexture>(Add_Component(iLevelIndex, wstrTextureTag, TEXT("Texture_Com")));

	assert(m_pTextureCom);

	if (m_bBattleBoard)
	{
		m_pSoundWaveTexture = static_pointer_cast<CTexture>(Add_Component(iLevelIndex,
			L"Prototype_Component_Texture_SpectraSoundWave", TEXT("SWTexture_Com")));
		assert(m_pSoundWaveTexture);
	}
}

shared_ptr<CFree_SignBoard> CFree_SignBoard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFree_SignBoard
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFree_SignBoard(pDevice, pContext)
		{}
	};


	shared_ptr<CFree_SignBoard> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFree_SignBoard"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CFree_SignBoard::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFree_SignBoard
	{
		MakeSharedEnabler(const CFree_SignBoard& rhs) : CFree_SignBoard(rhs)
		{}
	};

	shared_ptr<CFree_SignBoard> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFree_SignBoard"));
		assert(false);
	}

	return pInstance;
}

void CFree_SignBoard::Free()
{
}
