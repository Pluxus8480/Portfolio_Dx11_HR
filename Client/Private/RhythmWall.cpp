#include "RhythmWall.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "Cine_Manager.h"

wstring CRhythmWall::ObjID = TEXT("CRhythmWall");
CRhythmWall::CRhythmWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CRhythmWall::CRhythmWall(const CRhythmWall& rhs)
    : CGameObject{ rhs }
{
}

CRhythmWall::~CRhythmWall()
{
    Free();
}

HRESULT CRhythmWall::Initialize_Prototype()
{
	_float4x4 fMat = {};
	//XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_UpDownBlock_1"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_KrscRoomFloorALine_TS_01.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_UpDownBlock_2"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_KrscRoomFloorALine_TS_02.FBX"), MODEL_TYPE::NONANIM, fMat)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_UpDownBlock_3"), TEXT("../Bin/Resources/Models/Map/Boss_Korsica/SM_KrscRoomFloorALine_TS_03.FBX"), MODEL_TYPE::NONANIM, fMat)
	);

    return S_OK;
}

HRESULT CRhythmWall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	CMapManager::Get_Instance()->Assign_BossObject(shared_from_this());
    return S_OK;
}

void CRhythmWall::PriorityTick(_float fTimeDelta)
{
}

void CRhythmWall::Tick(_float fTimeDelta)
{
	_float fBeatRate = CBeat_Manager::Get_Instance()->Get_BeatRate();
	for (auto& Block : m_vecPartBlocks)
	{
		if (fBeatRate <= 0.2f)
		{
			Block.vLocalMatrix._42 = Block.fMaxHeight * fBeatRate * 5.f + Block.fMinHeight * (1.f - fBeatRate * 5.f);
		}
		else
		{
			Block.vLocalMatrix._42 = Block.fMinHeight * (fBeatRate - .2f) * 1.25f + Block.fMaxHeight * (1.f - (fBeatRate - .2f) * 1.25f);
		}
		
		XMStoreFloat4x4(&Block.vWorldMatrix, XMLoadFloat4x4(&Block.vLocalMatrix) * m_pTransformCom->Get_WorldMatrix_XMMat());
	}
}

void CRhythmWall::LateTick(_float fTimeDelta)
{
	if (m_iCurPhase == (_uint)KORSIKA_PHASE::ENDDING || !CCine_Manager::Get_Instance()->IsInGame())
		return;
	auto pGI = m_pGameInstance.lock();

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CRhythmWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (auto& Block : m_vecPartBlocks)
	{
		_uint	iNumMeshes = m_pModelCom[Block.iModelIndex]->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Block.vWorldMatrix)))
				return E_FAIL;
			if (FAILED(m_pModelCom[Block.iModelIndex]->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pShaderCom->Begin(0);
			m_pModelCom[Block.iModelIndex]->Render((_uint)i);
		}
	}

    return S_OK;
}

HRESULT CRhythmWall::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom[BLOCK1] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_UpDownBlock_1"), TEXT("Model01_Com")));
	m_pModelCom[BLOCK2] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_UpDownBlock_2"), TEXT("Model02_Com")));
	m_pModelCom[BLOCK3] = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_UpDownBlock_3"), TEXT("Model03_Com")));

	assert(m_pShaderCom);

	for (size_t i = 0; i < BLOCK_END; ++i)
		assert(m_pModelCom[i]);
	
	vector<_int> ModelGroup = { BLOCK1, BLOCK2, BLOCK2, BLOCK3 };
	vector<_float> Angle = { XMConvertToRadians(3.f), XMConvertToRadians(4.5f), XMConvertToRadians(6.f) };

	_float fAngle = {};
	_float fPrevAngle = {};

	auto pGI = m_pGameInstance.lock();

	for (size_t i = 0; i < 10; ++i)
	{
		_int iCount = 0;
		while (iCount < 5)
		{
			_int iDest = pGI->Random_Int(0, 3);
			_int iSour = pGI->Random_Int(0, 3);

			_int iTemp = ModelGroup[iDest];
			ModelGroup[iDest] = ModelGroup[iSour];
			ModelGroup[iSour] = iTemp;

			++iCount;
		}
		
		for (size_t j = 0; j < ModelGroup.size(); ++j)
		{
			BLOCKINFO tBlockInfo{};

			tBlockInfo.iModelIndex = ModelGroup[j];
			_float fCurAngle = fAngle + fPrevAngle + Angle[ModelGroup[j]];
			_matrix localMat = XMMatrixScaling(0.95f, 1.f, 1.f) * XMMatrixRotationY(fCurAngle);
			XMStoreFloat4x4(&tBlockInfo.vLocalMatrix, localMat);
			//XMStoreFloat4x4(&tBlockInfo.vWorldMatrix, XMMatrixScaling(0.9f, 1.f, 1.f));
			tBlockInfo.vWorldMatrix = Identity;
			tBlockInfo.fMinHeight = 0.f;
			tBlockInfo.fMaxHeight = pGI->Random_Float(0.3f, 1.5f);
			m_vecPartBlocks.emplace_back(tBlockInfo);

			fAngle = fCurAngle;
			fPrevAngle = Angle[ModelGroup[j]];
		}
	}

    return S_OK;
}

HRESULT CRhythmWall::Bind_ShaderResources()
{
	auto pGI = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
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

shared_ptr<CRhythmWall> CRhythmWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CRhythmWall
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CRhythmWall(pDevice, pContext)
		{}
	};


	shared_ptr<CRhythmWall> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRhythmWall"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CRhythmWall::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CRhythmWall
	{
		MakeSharedEnabler(const CRhythmWall& rhs) : CRhythmWall(rhs)
		{}
	};

	shared_ptr<CRhythmWall> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRhythmWall"));
		assert(false);
	}

	return pInstance;
}

void CRhythmWall::Free()
{
}
