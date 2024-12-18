#include "Tunnel.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "PhysxCollider.h"

#include "Beat_Manager.h"

wstring CTunnel::ObjID = TEXT("CTunnel");

CTunnel::CTunnel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTunnel::CTunnel(const CTunnel& rhs)
	: CGameObject{ rhs }
{
}

CTunnel::~CTunnel()
{
	Free();
}

HRESULT CTunnel::Set_ModelComponent(const string& strPrototypeTag)
{
	_tchar szPrototypeTag[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CTunnel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_float4x4 PreTransform;
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(0.01f,0.01f,0.01f,0.f), 
		XMVectorSet(0.f,0.f,0.f,1.f),XMQuaternionRotationRollPitchYaw(0.f,0.f,0.f),XMVectorSet(0.f,0.f,0.f,1.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Tunnel"), TEXT("../Bin/Resources/Models/Map/Gimmick/Tunnel.fbx"), MODEL_TYPE::NONANIM, PreTransform)
	);

	return S_OK;
}

HRESULT CTunnel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ANIMOBJ_DESC* pDesc = static_cast<ANIMOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	//_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	//m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	if (pDesc)
		m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);


	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();


	m_vecPositions.resize(40);
	for (_int i = 0; i < 40; ++i)
	{
		m_vecPositions[i].x = 0.f;
		m_vecPositions[i].y = 0.f;
		m_vecPositions[i].z = (_float)(i - 20) * 400.f;
	}

	return S_OK;
}

void CTunnel::PriorityTick(_float fTimeDelta)
{
}

void CTunnel::Tick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
	}
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	//{
	//	m_bDebug = !m_bDebug;
	//}

		_uint iSize = static_cast<_uint>(m_vecPositions.size());
		for (_uint i = 0; i < iSize; ++i)
		{
			m_vecPositions[i].z -= 1000.f * fTimeDelta;
			if (m_vecPositions[i].z <= -4000.f)
			{
				m_vecPositions[i].z += (_float)iSize * 400.f;
			}
		}

}

void CTunnel::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_TERRAIN, shared_from_this());
}

HRESULT CTunnel::Render()
{
	if (!m_pModelCom)
		return S_OK;

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));



	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint j = 0; j < m_vecPositions.size(); ++j)
	{
		_matrix World = XMMatrixTranslation(m_vecPositions[j].x, m_vecPositions[j].y, m_vecPositions[j].z) * m_pTransformCom->Get_WorldMatrix_XMMat();
		//World.r[3] += XMLoadFloat3(&m_vecPositions[j]);
		_float4x4 WorldFloat;
		XMStoreFloat4x4(&WorldFloat, World);
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldFloat);
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_bool bNoEmit = false;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			{
				bNoEmit = true;
			}
			if (bNoEmit)
				m_pShaderCom->Begin(0);
			else
				m_pShaderCom->Begin((_uint)MESH_PASS::Emission);

			if (FAILED(m_pModelCom->Bind_Buffers((_uint)(i))))
				return E_FAIL;


				m_pModelCom->Draw((_uint)i);
		
		}
	}
	return S_OK;
}


HRESULT CTunnel::Ready_Components(const ANIMOBJ_DESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tunnel"), TEXT("Com_Model")));



	if (!m_pModelCom)
		assert(false);

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);


	return S_OK;
}

shared_ptr<CTunnel> CTunnel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CTunnel
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTunnel(pDevice, pContext)
		{}
	};


	shared_ptr<CTunnel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTunnel"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CTunnel::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CTunnel
	{
		MakeSharedEnabler(const CTunnel& rhs) : CTunnel(rhs)
		{}
	};


	shared_ptr<CTunnel> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTunnel"));
		assert(false);
	}

	return pInstance;
}

void CTunnel::Free()
{
}
