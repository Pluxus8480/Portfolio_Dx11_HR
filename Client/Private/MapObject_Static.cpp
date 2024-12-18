#include "MapObject_Static.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "PhysxCollider.h"
#include "Mesh.h"
#include "OctComp.h"
#include "Beat_Manager.h"
#include "MapManager.h"

wstring CMapObject_Static::ObjID = TEXT("CMapObject_Static");
_uint CMapObject_Static::s_iMapObjID = 0;

CMapObject_Static::CMapObject_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_iMapObjID = s_iMapObjID;
	++s_iMapObjID;
}

CMapObject_Static::CMapObject_Static(const CMapObject_Static& rhs)
	: CGameObject{ rhs }, m_vecShaderPass{ rhs.m_vecShaderPass }
{
	m_iMapObjID = s_iMapObjID;
	++s_iMapObjID;
}

CMapObject_Static::~CMapObject_Static()
{
	Free();
}

HRESULT CMapObject_Static::Set_ModelComponent(const string& strPrototypeTag)
{
	//_tchar szPrototypeTag[MAX_PATH] = {};
	//MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);
	//
	///* For.Com_Model */
	//m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));
	//
	//if (!m_pModelCom)
	//	assert(false);

	return S_OK;
}

HRESULT CMapObject_Static::Initialize_Prototype(_uint iLevelIndex)
{
	auto pMapManager = CMapManager::Get_Instance();

	switch (iLevelIndex)
	{
	case Client::LEVEL_TEST: {
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/General/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Stage01/"));
		break;
	}
	case Client::LEVEL_MAPTOOL:
	{
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/General/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Stage01/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Rekka/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Korsica/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Stage_Kale/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Kale/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/"));
		break;
	}
	case Client::LEVEL_KALEBOSS:
	{
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/General/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Boss_Kale/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Stage01/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Boss_Rekka/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Stage_Kale/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Boss_Korsica/"));
		break;
	}
	case Client::LEVEL_MIMOSA:
	{
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/General/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Stage01/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/"));
		break;
	}
	case Client::LEVEL_LIFT: {
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/General/"));
		break;
	}
	case Client::LEVEL_KORSICA: {
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Boss_Korsica/"));
		break;
	}
	case Client::LEVEL_MODELTEST: {
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Boss_Rekka/"));
		break;
	}
	case Client::LEVEL_CINETOOL: {
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/General/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Stage01/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Rekka/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Korsica/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Stage_Kale/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Kale/"));
		Ready_ModelPrototypes(TEXT("../Bin/Resources/Models/Map/Boss_Mimosa/"));
		break;
	}
	case Client::LEVEL_KALESTAGE: {
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/General/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Stage01/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Stage_Kale/"));
		break;
	}
	case Client::LEVEL_GIMMICTEST: {
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/General/"));
		pMapManager->Read_ModelNames(TEXT("../Bin/Resources/Models/Map/Stage01/"));
		break;
	}
	default:
		break;
	}

	m_vecShaderPass.push_back("Default");
	m_vecShaderPass.push_back("MIRROR_X");
	m_vecShaderPass.push_back("AlphaBlend");
	m_vecShaderPass.push_back("EffectCCW");
	m_vecShaderPass.push_back("Distortion");
	m_vecShaderPass.push_back("Emission");
	m_vecShaderPass.push_back("Belt");
	m_vecShaderPass.push_back("CCW");
	m_vecShaderPass.push_back("CW");
	m_vecShaderPass.push_back("Outline");
	m_vecShaderPass.push_back("SKY");
	m_vecShaderPass.push_back("Water");
	m_vecShaderPass.push_back("Frame");
	m_vecShaderPass.push_back("AlphaEmission");
	m_vecShaderPass.push_back("BeltEmission");
	m_vecShaderPass.push_back("NoneCulling");
	m_vecShaderPass.push_back("DiffuseColor");
	m_vecShaderPass.push_back("Shadow");
	m_vecShaderPass.push_back("Reflection");
	m_vecShaderPass.push_back("ReflectionScreen");
	m_vecShaderPass.push_back("ReflectionCube");
	m_vecShaderPass.push_back("DCBlend");
	m_vecShaderPass.push_back("EmissionMoving");
	m_vecShaderPass.push_back("ShaftLight");
	m_vecShaderPass.push_back("Visualizer");
	m_vecShaderPass.push_back("EmissionFrame");
	m_vecShaderPass.push_back("Execution");

	return S_OK;
}

HRESULT CMapObject_Static::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	STATICOBJ_DESC* pDesc = static_cast<STATICOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	//_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	//m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
	m_bCollision = pDesc->bCollision;
	//
	//_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
	//_vector q = XMQuaternionRotationMatrix(World);
	//_vector p = World.r[3];
	//PxTransform Transform{};
	//
	//Transform.p.x = p.m128_f32[0];
	//Transform.p.y = p.m128_f32[1];
	//Transform.p.z = p.m128_f32[2];
	//Transform.q.x = q.m128_f32[0];
	//Transform.q.y = q.m128_f32[1];
	//Transform.q.z = q.m128_f32[2];
	//Transform.q.w = q.m128_f32[3];
	//if (!Transform.q.isSane())
	//{
	//	assert(false);
	//}
	//if(m_pPhysXCollider)
	//{
	//	if(m_pPhysXCollider->Get_Static_RigidBody())
	//		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(Transform);
	//}

	const shared_ptr<MODEL_DATA> pModelData = static_pointer_cast<CModel>(GAMEINSTANCE->Get_Component_Prototype(m_iLevelIndex, pDesc->strModelTag))->Get_ModelData();
	_float3 vLocalMeshCenter = pModelData->VertexInfo.vCenter;
	//vLocalMeshCenter.z *= -1.f;
	_vector vMeshCenter = XMVectorSetW(XMLoadFloat3(&vLocalMeshCenter), 1.f);
	XMStoreFloat3(&m_vMeshCenter, XMVector3TransformCoord(vMeshCenter, m_pTransformCom->Get_WorldMatrix_XMMat()));
	_float3 vScale = m_pTransformCom->Get_Scale();
	m_fCullingRange = pModelData->Get_MaxOffsetRange() * (max(vScale.x, max(vScale.y, vScale.z)));
	m_fCullingRange *= 1.5f;

	//m_pOctCom->Calc_Octree(m_vMeshCenter, m_fCullingRange);

	m_iPassIndex = (_uint)MESH_PASS::Default;

	size_t posWater = pDesc->strModelTag.find(L"Water");
	size_t posEff = pDesc->strModelTag.find(L"Effluent");
	if (posWater != pDesc->strModelTag.npos || posEff != pDesc->strModelTag.npos)
		m_iPassIndex = (_uint)MESH_PASS::Water;

	//_vector vDet = XMMatrixDeterminant(XMLoadFloat4x4(&pDesc->worldMatrix));
	//if (XMVectorGetX(vDet) < 0)
	//	m_iPassIndex = (_uint)MESH_PASS::CW;
	//
	//size_t posSheet = pDesc->strModelTag.find(L"SheetA_AG");
	//if (posSheet != pDesc->strModelTag.npos)
	//	m_iPassIndex = (_uint)MESH_PASS::NoneCulling;

	return S_OK;
}

void CMapObject_Static::Push_From_Pool(void* pArg)
{
	//m_pModelCom->Generate_Mips();
	m_vecModel[0]->Generate_Mips();
}

void CMapObject_Static::Edit_Attribute()
{
	Edit_Material();
}

void CMapObject_Static::Save_Data(ofstream& os)
{
	for (_int i = 0; i < m_iMaxMaterial; ++i)
	{
		write_typed_data(os, m_vecColors[i]);
		write_typed_data(os, m_vecPassIndex[i]);
	}
}

void CMapObject_Static::Load_Data(ifstream& is)
{
	//_uint iNumMaterial = m_pModelCom->Get_NumMeshes();
	_uint iNumMaterial = m_vecModel[0]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMaterial; ++i)
	{
		read_typed_data(is, m_vecColors[i]);
		read_typed_data(is, m_vecPassIndex[i]);

		if (m_vecPassIndex[i] == (_uint)MESH_PASS::BeltEmission)
			m_bBeltEmission = true;

		if (m_vecPassIndex[i] == (_uint)MESH_PASS::AlphaBlend)
			m_eRenderGroup = CRenderer::RENDER_BLEND;
	}

	if (m_iPassIndex != (_uint)MESH_PASS::Default)
	{
		for (auto& index : m_vecPassIndex)
			index = m_iPassIndex;
	}
}

void CMapObject_Static::Get_MaterialSet(vector<_uint>& PassIndexCopy, vector<_float4>& DiffuseColorCopy)
{
	PassIndexCopy.assign(m_vecPassIndex.begin(), m_vecPassIndex.end());
	DiffuseColorCopy.assign(m_vecColors.begin(), m_vecColors.end());
}

void CMapObject_Static::Set_MaterialSet(const vector<_uint>& PassIndexSour, const vector<_float4>& DiffuseColorSour)
{
	if (m_vecPassIndex.size() != PassIndexSour.size() || m_vecColors.size() != DiffuseColorSour.size()) return;

	m_vecPassIndex.assign(PassIndexSour.begin(), PassIndexSour.end());
	m_vecColors.assign(DiffuseColorSour.begin(), DiffuseColorSour.end());
}

void CMapObject_Static::Edit_Material()
{
	ImGui::SliderInt("Material Index", &m_iMaterialIndex, 0, m_iMaxMaterial - 1);

	if (ImGui::BeginCombo("Shader Pass", m_vecShaderPass[m_vecPassIndex[m_iMaterialIndex]].data(), ImGuiComboFlags_None))
	{
		for (int n = 0; n < m_vecShaderPass.size(); n++)
		{
			if (ImGui::Selectable(m_vecShaderPass[n].data(), n == m_vecPassIndex[m_iMaterialIndex]))
				m_vecPassIndex[m_iMaterialIndex] = n;
		}
		ImGui::EndCombo();
	}

	_float vColor[4] = { m_vecColors[m_iMaterialIndex].x, m_vecColors[m_iMaterialIndex].y,
		m_vecColors[m_iMaterialIndex].z, m_vecColors[m_iMaterialIndex].w };
	if (ImGui::ColorEdit4("##Color", vColor, ImGuiColorEditFlags_Float))
		m_vecColors[m_iMaterialIndex] = _float4(vColor[0], vColor[1], vColor[2], vColor[3]);
}

void CMapObject_Static::PriorityTick(_float fTimeDelta)
{
}

void CMapObject_Static::Tick(_float fTimeDelta)
{
	//Calc_ViewDepth();

	if (m_bBeltEmission)
	{
		m_fUvFlow += fTimeDelta * 0.05f;

		if (m_fUvFlow > 1.f)
			m_fUvFlow = 0.f;
	}
}

void CMapObject_Static::LateTick(_float fTimeDelta)
{
	//if (m_pOctCom->IsCulled())
	//	return;

	auto pGI = m_pGameInstance.lock();
#ifdef PERFECTION_LOAD
	static _bool bShadow = false;
	//if (pGI->Get_KeyDown(DIK_F9))
	//{
	//	bShadow = true;
	//}
	//if (pGI->Get_KeyDown(DIK_F10))
	//{
	//	bShadow = false;
	//}
	if(bShadow && pGI->isIn_Frustum_LightSpace(XMVectorSetW(XMLoadFloat3(&m_vMeshCenter), 1.f), m_fCullingRange))
		pGI->Add_RenderObjects(CRenderer::RENDER_SHADOW, shared_from_this());
#endif
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;
	if (false == pGI->isIn_Frustum_WorldSpace(XMVectorSetW(XMLoadFloat3(&m_vMeshCenter), 1.f), m_fCullingRange)) return;

	_vector CamPos = pGI->Get_CamPosition_Vector();
	_float vCamDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vMeshCenter) - CamPos));

	if (vCamDistance < m_fCullingRange + 50.f)
		m_iLod = 0;
	else if (vCamDistance < m_fCullingRange + 75.f)
		m_iLod = 1;
	else if (vCamDistance < m_fCullingRange + 100.f)
		m_iLod = 2;
	else
		m_iLod = 3;

	if (m_vecModel.size() <= m_iLod)
		m_iLod = static_cast<_uint>(m_vecModel.size()) - 1;

	pGI->Add_RenderObjects(m_eRenderGroup, shared_from_this());

}

HRESULT CMapObject_Static::Render()
{
	//if (!m_pModelCom)
	//	return S_OK;

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

	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint	iNumMeshes = m_vecModel[m_iLod]->Get_NumMeshes();
	if (m_iPassIndex == (_uint)MESH_PASS::Water)
	{
		m_pShaderCom->Bind_Float("g_fTime", CBeat_Manager::Get_Instance()->Get_SuperTimer());
	}
	for (_uint i = 0; i < iNumMeshes; i++)
	{ 

		_uint iMaterialIndex = m_vecModel[m_iLod]->Get_MaterialIndex(i);

		if (m_vecPassIndex[iMaterialIndex] == (_uint)MESH_PASS::Execution)
			continue;

		_bool bEmissive = { false };
		if (FAILED(m_vecModel[m_iLod]->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_vecModel[m_iLod]->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			bEmissive = false;
		else
		{
			if (m_vecPassIndex[iMaterialIndex] == (_uint)MESH_PASS::Default)
				bEmissive = true;
		}

		if (m_vecPassIndex[iMaterialIndex] == (_uint)MESH_PASS::DiffuseColor)
			m_pShaderCom->Bind_RawValue("g_vColor", &m_vecColors[iMaterialIndex], sizeof(_float4));

		else if (m_vecPassIndex[iMaterialIndex] == (_uint)MESH_PASS::BeltEmission)
		{
			_float2 vFlow{ m_fUvFlow, 0.f };
			m_pShaderCom->Bind_RawValue("g_vUVFlow", &vFlow, sizeof(_float2));
		}

		else if (m_vecPassIndex[iMaterialIndex] == (_uint)MESH_PASS::Frame)
		{
			XMINT2 vDivide = { (_int)(m_vecColors[iMaterialIndex].x * 10), (_int)(m_vecColors[iMaterialIndex].y * 10) };
			_int iFrameIndex = (_int)(m_vecColors[iMaterialIndex].z * 10);
			m_pShaderCom->Bind_RawValue("g_iDivide", &vDivide, sizeof(XMINT2));
			m_pShaderCom->Bind_RawValue("g_iFrameIndex", &iFrameIndex, sizeof(_int));
		}

		if (bEmissive)
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		else
			m_pShaderCom->Begin(m_vecPassIndex[iMaterialIndex]);

		m_vecModel[m_iLod]->Render((_uint)i);
	}

	return S_OK;
}


HRESULT CMapObject_Static::Render_Shadow()
{
	auto pGameInstance = m_pGameInstance.lock();
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_LightView_Matrix());
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_LightProj_Matrix());
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
	//   return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint   iNumMeshes = m_vecModel[0]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin((_uint)MESH_PASS::Shadow);

		m_vecModel[0]->Render((_uint)i);
	}

	return S_OK;
}


HRESULT CMapObject_Static::Ready_Components(const STATICOBJ_DESC* pDesc)
{	
	shared_ptr<CMapManager> pMapManager = CMapManager::Get_Instance();
	_uint iLevelIndex = pMapManager->Get_NextLevel();

	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));

	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	if (pDesc->strModelTag.length() > 0)
	{
		m_iLevelIndex = (iLevelIndex == LEVEL_MAPTOOL || iLevelIndex == LEVEL_CINETOOL || iLevelIndex == LEVEL_UITOOL) ? LEVEL_STATIC : iLevelIndex;

		/* For.Com_Model */
		//m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, pDesc->strModelTag, TEXT("Com_Model")));

		//if (!m_pModelCom)
		//	assert(false);
	
		if (FAILED(pMapManager->Add_SMModelPrototype(m_pDevice, m_pContext, iLevelIndex, pDesc->strModelTag)))
			assert(false);

		m_vecModel.emplace_back(static_pointer_cast<CModel>(__super::Add_Component(m_iLevelIndex, pDesc->strModelTag, TEXT("Com_Model"))));

		for (size_t i = 1; i <= 3; ++i)
		{
			wstring strModelTag = pDesc->strModelTag + L"_Lod" + to_wstring(i);
			wstring strCompTag = L"Com_Model" + to_wstring(i);

			if (FAILED(pMapManager->Add_SMModelPrototype(m_pDevice, m_pContext, iLevelIndex, strModelTag)))
				break;

			auto pModel = Add_Component(m_iLevelIndex, strModelTag, strCompTag);
			if (pModel)
				m_vecModel.emplace_back(static_pointer_cast<CModel>(pModel));
			else
				break;
		}

		for (auto& pModel : m_vecModel)
			assert(pModel);

		//m_iMaxMaterial = m_pModelCom->Get_NumMeshes();
		m_iMaxMaterial = m_vecModel[0]->Get_NumMeshes();
		m_vecColors.resize(m_iMaxMaterial, _float4(1.f, 1.f, 1.f, 1.f));
		m_vecPassIndex.resize(m_iMaxMaterial, (_uint)MESH_PASS::Default);
	}

	if (true == pDesc->bCollision)
	{
		_uint    iNumMeshes = m_vecModel[0]->Get_NumMeshes();
		string strTag = string(CMapObject_Static::ObjID.begin(), CMapObject_Static::ObjID.end());
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			string tmp = strTag + to_string(m_iMapObjID) + string("_") + to_string(i);
			auto pMesh = m_vecModel[0]->Get_Mesh((_uint)i);
			_matrix mat = XMLoadFloat4x4(&pDesc->worldMatrix);
			_vector vScale, vRot, vPos;
			XMMatrixDecompose(&vScale, &vRot, &vPos, mat);
			_float3 scale;
			XMStoreFloat3(&scale, vScale);
			PxVec3 pscale;
			pscale.x = scale.x;
			pscale.y = scale.y;
			pscale.z = scale.z;
			pMesh->Cooking_TriangleMesh(pDesc->worldMatrix);
			auto pTriangleMesh = pMesh->Get_TriangleMesh();
			if (pTriangleMesh && pscale.x && pscale.y && pscale.z)
				m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, tmp.c_str());
		}

		if (!m_pPhysXCollider)
			assert(false);
	}

	//m_pOctCom = static_pointer_cast<COctComp>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Octree"), TEXT("Com_Octree")));

	if (!m_pShaderCom /*|| !m_pOctCom*/)
		assert(false);

	if (iLevelIndex == (_uint)LEVEL_MAPTOOL)
		m_bBeltEmission = true;

	return S_OK;
}

HRESULT CMapObject_Static::Ready_ModelPrototypes(const wstring& strLevelDir)
{
	_float4x4 FMat = {};
	//XMStoreFloat4x4(&FMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	XMStoreFloat4x4(&FMat, XMMatrixIdentity());

	wstring strFilePath = strLevelDir + L"*.*";

	WIN32_FIND_DATA	fd;
	HANDLE	hFind = FindFirstFile(strFilePath.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL bContinue = true;

		do
		{
			if (!lstrcmp(L".", fd.cFileName) || !lstrcmp(L"..", fd.cFileName))
				continue;

			DWORD dwFileAttribute = fd.dwFileAttributes;

			if (dwFileAttribute == FILE_ATTRIBUTE_DIRECTORY)
			{
				wstring strChildPath = strLevelDir + fd.cFileName + L"/";

				Ready_ModelPrototypes(strChildPath);
			}
			else if (dwFileAttribute == FILE_ATTRIBUTE_ARCHIVE)
			{
				wstring strFileName = fd.cFileName;

				_tchar szFullPath[MAX_PATH] = {};

				_tchar szFileName[MAX_PATH] = {};
				_tchar szExt[MAX_PATH] = {};

				_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

				if (lstrcmp(L".FBX", szExt) && lstrcmp(L".fbx", szExt))
					continue;

				wcscpy_s(szFullPath, strLevelDir.c_str());
				wcscat_s(szFullPath, strFileName.c_str());

				size_t index = strLevelDir.find_last_of(L'/');
				wstring strSectionName = strLevelDir.substr(0, index);
				index = strSectionName.find_last_of(L'/');
				strSectionName = strSectionName.substr(index + 1, strSectionName.length());

				wstring strPrototypeTag = strSectionName;

				m_List_ResourceInfo.push_back(
					make_shared<RES_MODEL>(szFileName, wstring(szFullPath), MODEL_TYPE::NONANIM, FMat)
				);
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	return S_OK;
}

shared_ptr<CMapObject_Static> CMapObject_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex)
{
	struct MakeSharedEnabler :public CMapObject_Static
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMapObject_Static(pDevice, pContext)
		{}
	};

	shared_ptr<CMapObject_Static> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevelIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CMapObject_Static"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CMapObject_Static::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CMapObject_Static
	{
		MakeSharedEnabler(const CMapObject_Static& rhs) : CMapObject_Static(rhs)
		{}
	};

	shared_ptr<CMapObject_Static> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMapObject_Static"));
		assert(false);
	}

	return pInstance;
}

void CMapObject_Static::Free()
{
}

