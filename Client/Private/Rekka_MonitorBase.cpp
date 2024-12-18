#include "Rekka_MonitorBase.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Misc_Interface.h"
#include "MapManager.h"

wstring CRekka_MonitorBase::ObjID = TEXT("CRekka_MonitorBase");
CRekka_MonitorBase::CRekka_MonitorBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CRekka_MonitorBase::CRekka_MonitorBase(const CRekka_MonitorBase& rhs)
    : CGameObject{rhs}
{
}

CRekka_MonitorBase::~CRekka_MonitorBase()
{
    Free();
}

HRESULT CRekka_MonitorBase::Initialize_Prototype()
{
	_float4x4 FMat = {};
	XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(
			TEXT("Prototype_Component_Model_Rekka_MonitorBase"),
			TEXT("../Bin/Resources/Models/Map/Boss_Rekka/SM_BossMain_CenterMonitorBaseA_SH_01.FBX"),
			MODEL_TYPE::NONANIM, FMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiCircle_Blue"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_ComEmiVandelaySignA_E_Blue.png"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiCircle_Purple"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_ComEmiVandelaySignA_E_Purple.png"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiCircle_Yellow"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_ComEmiVandelaySignA_E_Yellow.png"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiCircle_Red"), TEXT("../Bin/Resources/Models/Map/Boss_Rekka/T_ComEmiVandelaySignA_E_Red.png"))
	);

	return S_OK;
}

HRESULT CRekka_MonitorBase::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
	m_iCurPhase = (_uint)REKKA_PHASE::PHASE_1;

	return S_OK;
}

void CRekka_MonitorBase::PriorityTick(_float fTimeDelta)
{
}

void CRekka_MonitorBase::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
	//{
	//	++m_iCurPhase;
	//	m_iCurPhase %= (_uint)REKKA_PHASE::PHASE_END;
	//}
}

void CRekka_MonitorBase::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRekka_MonitorBase::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bEmissive = { false };
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (i == 2)
		{
			if (FAILED(m_pTextureCom[(_uint)m_iCurPhase]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
				return E_FAIL;
			bEmissive = true;
		}
		else
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				bEmissive = false;
			else
				bEmissive = true;
		}
		
		if(bEmissive)
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Default);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CRekka_MonitorBase::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rekka_MonitorBase"), TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	m_pTextureCom[(_uint)REKKA_PHASE::PHASE_1] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiCircle_Blue"), TEXT("Texture0_Com")));
	m_pTextureCom[(_uint)REKKA_PHASE::PHASE_2] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiCircle_Purple"), TEXT("Texture1_Com")));
	m_pTextureCom[(_uint)REKKA_PHASE::PHASE_3] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiCircle_Yellow"), TEXT("Texture2_Com")));
	m_pTextureCom[(_uint)REKKA_PHASE::PHASE_4] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiCircle_Red"), TEXT("Texture3_Com")));

	for (size_t i = 0; i < (_uint)REKKA_PHASE::PHASE_END; ++i)
		assert(m_pTextureCom[i]);

	return S_OK;
}

HRESULT CRekka_MonitorBase::Bind_ShaderResources()
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

shared_ptr<CRekka_MonitorBase> CRekka_MonitorBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRekka_MonitorBase
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRekka_MonitorBase(pDevice, pContext)
		{}
	};

	shared_ptr<CRekka_MonitorBase> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRekka_MonitorBase"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CRekka_MonitorBase::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRekka_MonitorBase
	{
		MakeSharedEnabler(const CRekka_MonitorBase& rhs) : CRekka_MonitorBase(rhs)
		{}
	};

	shared_ptr<CRekka_MonitorBase> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRekka_MonitorBase"));
		assert(false);
	}

	return pInstance;
}

void CRekka_MonitorBase::Free()
{
}
