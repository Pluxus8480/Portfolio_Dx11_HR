#include "Rekka_Monitor_L.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Misc_Interface.h"

wstring CRekka_Monitor_L::ObjID = TEXT("CRekka_Monitor_L");
CRekka_Monitor_L::CRekka_Monitor_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CRekka_Monitor_L::CRekka_Monitor_L(const CRekka_Monitor_L& rhs)
    : CGameObject{rhs}
{
	memcpy(m_DMGInfos, rhs.m_DMGInfos, sizeof(DMGINFO) * (_uint)REKKA_PATTERN::PATTERN_END);
}

CRekka_Monitor_L::~CRekka_Monitor_L()
{
    Free();
}

void CRekka_Monitor_L::Change_Pattern(_uint iPattern)
{
	m_iCurPattern = min(iPattern, (_uint)REKKA_PATTERN::CHARGE);
	m_iCurFrame = 0;
	m_fTime = 0.f;
}

HRESULT CRekka_Monitor_L::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.004f, 0.004f, 0.004f)/* * XMMatrixRotationY(XMConvertToRadians(-90.f))*/);

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Rekka_MainMonitorB"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_MonitorB_SH_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_RekkaDisplay_SW"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_DMG_Monitor_wall_sound_wave_4096_02.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_RekkaDisplayB_Grab"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_BPM_St02BossDMG_MilestoneDriver_B.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_RekkaDisplayB_Logo"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_BPM_St02BossDMG_Logo_B.dds"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_RekkaDisplayB_Charge"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_BPM_St02BossDMG_MorePower_B.dds"))
	);

	m_DMGInfos[(_uint)REKKA_PATTERN::SOUNDWAVE].vDivide = XMINT2(4, 4);
	m_DMGInfos[(_uint)REKKA_PATTERN::SOUNDWAVE].iMaxFrame = 16;

	m_DMGInfos[(_uint)REKKA_PATTERN::GRAB].vDivide = XMINT2(4, 4);
	m_DMGInfos[(_uint)REKKA_PATTERN::GRAB].iMaxFrame = 16;

	m_DMGInfos[(_uint)REKKA_PATTERN::STOMP].vDivide = XMINT2(1, 1);
	m_DMGInfos[(_uint)REKKA_PATTERN::STOMP].iMaxFrame = 1;

	m_DMGInfos[(_uint)REKKA_PATTERN::CHARGE].vDivide = XMINT2(6, 6);
	m_DMGInfos[(_uint)REKKA_PATTERN::CHARGE].iMaxFrame = 32;

    return S_OK;
}

HRESULT CRekka_Monitor_L::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());

	m_iCurPattern = (_uint)REKKA_PATTERN::SOUNDWAVE;

    return S_OK;
}

void CRekka_Monitor_L::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

		for (size_t i = 0; i <= (_uint)REKKA_PATTERN::CHARGE; ++i)
			m_DMGInfos[i].fSecPerFrame = fBeat / (_float)m_DMGInfos[i].iMaxFrame;

		m_bInit = true;
	}
}

void CRekka_Monitor_L::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
	//{
	//	++m_eState;
	//	m_eState %= ST_END;
	//	Change_State(m_eState);
	//}

	m_fTime += fTimeDelta;

	if (m_fTime > m_DMGInfos[(_uint)m_iCurPattern].fSecPerFrame)
	{
		m_fTime -= m_fTime;

		++m_iCurFrame;
		m_iCurFrame %= m_DMGInfos[(_uint)m_iCurPattern].iMaxFrame;
	}
}

void CRekka_Monitor_L::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRekka_Monitor_L::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 1)
		{
			if (FAILED(m_pTextureCom[(_uint)m_iCurPattern]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
				return E_FAIL;
			if (FAILED(m_pTextureCom[(_uint)m_iCurPattern]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_iDivide", &m_DMGInfos[(_uint)m_iCurPattern].vDivide, sizeof(XMINT2))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iCurFrame, sizeof(_int))))
				return E_FAIL;

			m_pShaderCom->Begin((_uint)MESH_PASS::EmissionFrame);
		}
		else
		{
			_bool bEmissive = { false };

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				bEmissive = false;
			else
				bEmissive = true;

			if(bEmissive)
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		}

		m_pModelCom->Render((_uint)i);
	}

    return S_OK;
}

HRESULT CRekka_Monitor_L::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rekka_MainMonitorB"), TEXT("Com_Model")));
	
	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	m_pTextureCom[(_uint)REKKA_PATTERN::SOUNDWAVE] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, 
																	TEXT("Prototype_Component_Texture_RekkaDisplay_SW"), TEXT("Texture0_Com")));
	m_pTextureCom[(_uint)REKKA_PATTERN::GRAB] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC,
																	TEXT("Prototype_Component_Texture_RekkaDisplayB_Grab"), TEXT("Texture1_Com")));
	m_pTextureCom[(_uint)REKKA_PATTERN::STOMP] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC,
																	TEXT("Prototype_Component_Texture_RekkaDisplayB_Logo"), TEXT("Texture2_Com")));
	m_pTextureCom[(_uint)REKKA_PATTERN::CHARGE] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC,
																	TEXT("Prototype_Component_Texture_RekkaDisplayB_Charge"), TEXT("Texture3_Com")));

	for (size_t i = 0; i <= (_uint)REKKA_PATTERN::CHARGE; ++i)
		assert(m_pTextureCom[i]);

    return S_OK;
}

HRESULT CRekka_Monitor_L::Bind_ShaderResources()
{
	auto pGI = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
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

shared_ptr<CRekka_Monitor_L> CRekka_Monitor_L::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRekka_Monitor_L
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_Monitor_L(pDevice, pContext) {}
	};

	shared_ptr<CRekka_Monitor_L> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_Monitor_L"));
		assert(false);
	}
	return pInstance;
}

shared_ptr<CGameObject> CRekka_Monitor_L::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_Monitor_L
	{
		MakeSharedEnabler(const CRekka_Monitor_L& rhs) : CRekka_Monitor_L(rhs) {}
	};

	shared_ptr<CRekka_Monitor_L> pInstance = make_shared<MakeSharedEnabler>(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_Monitor_L"));
		assert(false);
	}
	return pInstance;
}

void CRekka_Monitor_L::Free()
{
}
