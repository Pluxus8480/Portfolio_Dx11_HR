#include "DMGConsole.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "MapManager.h"
#include "Beat_Manager.h"

wstring CDMGConsole::ObjID = TEXT("CDMGConsole");
CDMGConsole::CDMGConsole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CDMGConsole::CDMGConsole(const CDMGConsole& rhs)
    : CGameObject{rhs}, m_vecConsoleInfo{rhs.m_vecConsoleInfo}, m_vecDMGInfo{rhs.m_vecDMGInfo}
{
}

CDMGConsole::~CDMGConsole()
{
    Free();
}

HRESULT CDMGConsole::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_DMGConsole_0"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_KrscConsoleA_TS_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_DMGConsole_1"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_KrscConsoleB_TS_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_DMGConsole_2"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_KrscConsoleB_TS_02.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_DMGConsole_3"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_SecDConsoleA_TS_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_DMGConsole_4"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_SecDConsoleB_TS_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_DMGConsole_5"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_SecDConsoleC_TS_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_DMGConsole_6"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_SecDConsoleD_TS_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_0"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_A.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_1"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_B.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_2"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_C.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_3"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_D.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_4"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_E.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_5"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_F.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_6"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_G.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_7"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_H.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_8"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_I.dds"))
	//);
	//m_List_ResourceInfo.push_back(
	//	make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_DMGDisplay_9"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/T_DMG_ConsoleDisplay_J.dds"))
	//);

	m_vecDMGInfo.emplace_back(DMGINFO{ 0, 4, 4, 8 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 1, 2, 2, 4 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 2, 2, 2, 4 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 3, 2, 2, 4 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 4, 2, 2, 4 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 5, 2, 2, 4 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 6, 2, 2, 4 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 7, 2, 2, 4 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 8, 1, 8, 8 });
	m_vecDMGInfo.emplace_back(DMGINFO{ 9, 4, 4, 16 });

	CONSOLEINFO tConsoleInfo{};
	tConsoleInfo.iConsoleIndex = CONSOLE1;
	tConsoleInfo.iNumDMG = 1;
	tConsoleInfo.iTextureIndex[0] = 1;
	tConsoleInfo.iMaterialIndex[0] = 2;
	m_vecConsoleInfo.emplace_back(tConsoleInfo);

	tConsoleInfo.iConsoleIndex = CONSOLE2;
	tConsoleInfo.iNumDMG = 2;
	tConsoleInfo.iTextureIndex[0] = 0;
	tConsoleInfo.iTextureIndex[1] = 3;
	tConsoleInfo.iMaterialIndex[0] = 1;
	tConsoleInfo.iMaterialIndex[1] = 4;
	m_vecConsoleInfo.emplace_back(tConsoleInfo);

	tConsoleInfo.iConsoleIndex = CONSOLE3;
	tConsoleInfo.iNumDMG = 2;
	tConsoleInfo.iTextureIndex[0] = 0;
	tConsoleInfo.iTextureIndex[1] = 3;
	tConsoleInfo.iMaterialIndex[0] = 1;
	tConsoleInfo.iMaterialIndex[1] = 4;
	m_vecConsoleInfo.emplace_back(tConsoleInfo);

	tConsoleInfo.iConsoleIndex = CONSOLE4;
	tConsoleInfo.iNumDMG = 1;
	tConsoleInfo.iTextureIndex[0] = 1;
	tConsoleInfo.iMaterialIndex[0] = 1;
	m_vecConsoleInfo.emplace_back(tConsoleInfo);

	tConsoleInfo.iConsoleIndex = CONSOLE5;
	tConsoleInfo.iNumDMG = 2;
	tConsoleInfo.iTextureIndex[0] = 8;
	tConsoleInfo.iTextureIndex[1] = 0;
	tConsoleInfo.iMaterialIndex[0] = 1;
	tConsoleInfo.iMaterialIndex[1] = 2;
	m_vecConsoleInfo.emplace_back(tConsoleInfo);

	tConsoleInfo.iConsoleIndex = CONSOLE6;
	tConsoleInfo.iNumDMG = 1;
	tConsoleInfo.iTextureIndex[0] = 6;
	tConsoleInfo.iMaterialIndex[0] = 2;
	m_vecConsoleInfo.emplace_back(tConsoleInfo);

	tConsoleInfo.iConsoleIndex = CONSOLE7;
	tConsoleInfo.iNumDMG = 1;
	tConsoleInfo.iTextureIndex[0] = 2;
	tConsoleInfo.iMaterialIndex[0] = 2;
	m_vecConsoleInfo.emplace_back(tConsoleInfo);

    return S_OK;
}

HRESULT CDMGConsole::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

    return S_OK;
}

void CDMGConsole::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		_uint iMaxFrame = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[0]].iMaxFrame;

		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

		m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[0]].fSecPerFrame = fBeat / (_float)iMaxFrame;

		iMaxFrame = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[1]].iMaxFrame;
		m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[1]].fSecPerFrame = fBeat / (_float)iMaxFrame;

		m_bInit = true;
	}
}

void CDMGConsole::Tick(_float fTimeDelta)
{
	m_fTime[0] += fTimeDelta;

	if (m_fTime[0] > m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[0]].fSecPerFrame)
	{
		m_fTime[0] -= m_fTime[0];

		++m_iFrameIndex[0];
		m_iFrameIndex[0] %= m_iMaxFrame[0];
	}

	if (m_tCurConsoleInfo.iNumDMG > 1)
	{
		m_fTime[1] += fTimeDelta;

		if (m_fTime[1] > m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[1]].fSecPerFrame)
		{
			m_fTime[1] -= m_fTime[1];

			++m_iFrameIndex[1];
			m_iFrameIndex[1] %= m_iMaxFrame[1];
		}
	}
}

void CDMGConsole::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CDMGConsole::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_vecModelCom[m_iModelIndex]->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bDMG = { false };
		if (i == m_tCurConsoleInfo.iMaterialIndex[0])
		{
			bDMG = true;
			DMGINFO curDMG = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[0]];
			
			XMINT2 divide = { curDMG.iX, curDMG.iY };
			if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &divide, sizeof(XMINT2))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex[0], sizeof(_int))))
				return E_FAIL;
		}
		else if (i == m_tCurConsoleInfo.iMaterialIndex[1])
		{
			if (m_tCurConsoleInfo.iNumDMG == 2)
			{
				bDMG = true;
				DMGINFO curDMG = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[1]];
				
				XMINT2 divide = { curDMG.iX, curDMG.iY };
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &divide, sizeof(XMINT2))))
					return E_FAIL;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex[1], sizeof(_int))))
					return E_FAIL;
			}
		}
		
		if (FAILED(m_vecModelCom[m_iModelIndex]->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (bDMG)
		{
			if (FAILED(m_vecModelCom[m_iModelIndex]->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			m_pShaderCom->Begin((_uint)MESH_PASS::Frame);
		}
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Default);

		m_vecModelCom[m_iModelIndex]->Render((_uint)i);
	}

	return S_OK;
}

void CDMGConsole::Edit_Attribute()
{
	if (ImGui::InputInt("Console Index", &m_iModelIndex))
	{
		m_iModelIndex %= m_vecModelCom.size();
	
		m_tCurConsoleInfo = m_vecConsoleInfo[m_iModelIndex];
		m_iMaxFrame[0] = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[0]].iMaxFrame;
		m_iMaxFrame[1] = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[1]].iMaxFrame;

		m_iFrameIndex[0] = 0;
		m_iFrameIndex[1] = 0;
	}
}

void CDMGConsole::Save_Data(ofstream& os)
{
	write_typed_data(os, m_iModelIndex);
}

void CDMGConsole::Load_Data(ifstream& is)
{
	read_typed_data(is, m_iModelIndex);

	m_tCurConsoleInfo = m_vecConsoleInfo[m_iModelIndex];
	m_iMaxFrame[0] = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[0]].iMaxFrame;
	m_iMaxFrame[1] = m_vecDMGInfo[m_tCurConsoleInfo.iTextureIndex[1]].iMaxFrame;

	if (CMapManager::Get_Instance()->Get_NextLevel() != LEVEL_MAPTOOL)
	{
		wstring strModelPrototype = TEXT("Prototype_Component_Model_DMGConsole_");
		strModelPrototype += to_wstring(m_iModelIndex);
		m_vecModelCom[0] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, strModelPrototype, TEXT("Model_Com")));

		m_iModelIndex = 0;
	}
}

HRESULT CDMGConsole::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));

	assert(m_pShaderCom);

	if (CMapManager::Get_Instance()->Get_NextLevel() == LEVEL_MAPTOOL)
	{
		m_vecModelCom.resize(CONSOLE_END);
		m_vecModelCom[CONSOLE1] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DMGConsole_0"), TEXT("Model01_Com")));
		m_vecModelCom[CONSOLE2] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DMGConsole_1"), TEXT("Model02_Com")));
		m_vecModelCom[CONSOLE3] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DMGConsole_2"), TEXT("Model03_Com")));
		m_vecModelCom[CONSOLE4] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DMGConsole_3"), TEXT("Model04_Com")));
		m_vecModelCom[CONSOLE5] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DMGConsole_4"), TEXT("Model05_Com")));
		m_vecModelCom[CONSOLE6] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DMGConsole_5"), TEXT("Model06_Com")));
		m_vecModelCom[CONSOLE7] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DMGConsole_6"), TEXT("Model07_Com")));

		for (auto& pModel : m_vecModelCom)
			assert(pModel);

		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_0"), TEXT("Texture1_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_1"), TEXT("Texture2_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_2"), TEXT("Texture3_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_3"), TEXT("Texture4_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_4"), TEXT("Texture5_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_5"), TEXT("Texture6_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_6"), TEXT("Texture7_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_7"), TEXT("Texture8_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_8"), TEXT("Texture9_Com"))));
		//m_vecTextureCom.emplace_back(static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DMGDisplay_9"), TEXT("Texture10_Com"))));
		//
		//for (auto& pTexture : m_vecTextureCom)
		//	assert(pTexture);

		m_tCurConsoleInfo = m_vecConsoleInfo[CONSOLE1];
	}
	
    return S_OK;
}

HRESULT CDMGConsole::Bind_ShaderResources()
{
	auto pGI = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

    return S_OK;
}

shared_ptr<CDMGConsole> CDMGConsole::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CDMGConsole
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDMGConsole(pDevice, pContext)
		{}
	};


	shared_ptr<CDMGConsole> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDMGConsole"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CDMGConsole::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CDMGConsole
	{
		MakeSharedEnabler(const CDMGConsole& rhs) : CDMGConsole(rhs)
		{}
	};

	shared_ptr<CDMGConsole> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDMGConsole"));
		assert(false);
	}

	return pInstance;
}

void CDMGConsole::Free()
{
}
