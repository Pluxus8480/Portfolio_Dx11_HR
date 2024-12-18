#include "Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include <filesystem>
#include "Channel.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	/*, m_Bones{ rhs.m_Bones }*/
	, m_iNumAnimations{ rhs.m_iNumAnimations }
	/*, m_Animations {rhs.m_Animations }*/
	, m_TransformMatrix{ rhs.m_TransformMatrix }
	, m_eModelType{ rhs.m_eModelType }
	, m_AnimMapping{ rhs.m_AnimMapping }
{
	_uint iAnimIndex = 0;
	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		m_Animations.emplace_back(pPrototypeAnimation->Clone());
		m_AnimMapping.emplace(m_Animations[iAnimIndex]->Get_AnimName(), iAnimIndex);
		++iAnimIndex;
	}


	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());
}

CModel::~CModel()
{
	Free();
}

_uint CModel::Get_BoneIdx(const _char* pBoneName) const
{
	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		if (0 == strcmp(m_Bones[i]->Get_Name(), pBoneName))
		{
			return i;
		}
	}
	return 0;
}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	if (m_iNumMeshes <= iMeshIndex) return 0;

	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

_bool CModel::Check_Material(_uint iMeshIndex, aiTextureType eMaterialType)
{
	if (nullptr == m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType])
		return false;

	return true;
}

void CModel::Get_BoneMatrices(vector<_float4x4>& vecOut, _uint iMeshIndex)
{
	_uint iModelBoneIdx = 0;
	for (auto& idx : m_Meshes[iMeshIndex]->m_Bones)
	{

		_float4x4 Matrix = m_Meshes[iMeshIndex]->m_OffsetMatrices[iModelBoneIdx];
		_matrix Combined = XMLoadFloat4x4(&Matrix);
		XMStoreFloat4x4(&Matrix, Combined * XMLoadFloat4x4(&m_Bones[idx]->m_CombinedTransformationMatrix));
		vecOut.push_back(Matrix);
		++iModelBoneIdx;
	}
}

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	m_pModelData = make_shared<MODEL_DATA>();
	m_pModelData->Make_ModelData(pModelFilePath, eType, TransformMatrix);
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	/* aiProcess_PreTransformVertices : 내 모델은 여러개의 메시(칼, 몸, 가방, 모닝스타, 그림자)로 구분되어있어. */
	/* 이 각각의 메시를 배치되야할 상태대로 다 미리 셋팅하여 로드한다. */
	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (eType == MODEL_TYPE::NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	Init_Model(pModelFilePath, "", 0);

	LoadSoundList(pModelFilePath);

	m_pModelData->Material_Datas.clear();
	m_pModelData->Mesh_Datas.clear();
	m_pModelData->Animation_Datas.clear();

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	/* old : 모든 메시를 순회하면서 각 메시의 렌더함수를 호출한다. */
	/* Curr : 메시의 순회를 클라이언트에서 수행을 하고 지정한 메시에 대해서만 렌더함수를 호출한다.  */
	/* 어떤 머테리얼(Diffuse Ambient)을 어떤 쉐이더에 어떤 변수에 전달할 건지를 결정을 해야하는 작업을 수행해야했다. */
	/* 어떤 머테리얼인지가 메시마다 다르다. CMesh::m_iMaterialIndex */

	//for (size_t i = 0; i < m_iNumMeshes; i++)
	//{
	//	/* 쉐이더에 텍스쳐를 던지느 ㄴ작업을 클라이언트에서 처리하기위해 위의 루프자체를 클라로 옮긴것이다. */
	//	m_Materials[m_Meshes[i]->Get_MaterialIndex()].pTextures[aiTextureType_DIFFUSE]->Bind_ShaderResource();

	//	m_Meshes[i]->Render();
	//}

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Draw(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Draw();

	return S_OK;
}

void CModel::Generate_Mips()
{
	for (auto& pMat : m_Materials)
	{
		for (auto& pTex : pMat.pTextures)
		{
			if(pTex)
			{
				pTex->Generate_Mips();
			}
		}
	}
}


const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationFloat4x4Ptr();
}

const _float4x4* CModel::Get_BoneMatrixPtr(_uint iBoneIdx) const
{
	return m_Bones[iBoneIdx]->Get_CombinedTransformationFloat4x4Ptr();
}

void CModel::Set_BoneMatrixPtr(const _char* pBoneName, const _float4x4& Transform)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return;
	(*iter)->Set_CombinedTransformationFloat4x4Ptr(Transform);
	return;
}

void CModel::Set_BoneMatrixPtr(_uint iBoneIdx, const _float4x4& Transform)
{
	m_Bones[iBoneIdx]->Set_CombinedTransformationFloat4x4Ptr(Transform);
}

const _float4x4* CModel::Get_BoneBasicMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_TransformationMatrixPtr();
}

const _vector CModel::Get_BonePosition(const _char* pBoneName, _vector Pos) const
{
	/*auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return;*/
	return _vector();
}

void CModel::Set_AnimIndex_Split(_uint iAnimIndex, _bool isLoop, const string& strTag, _bool bInterpole)
{
	if (m_mapisLoop[strTag] && m_mapCurrentAnimIndex[strTag] == iAnimIndex)
		return;

	if (m_iNumAnimations <= iAnimIndex)
		iAnimIndex = 0;

	m_mapPreAnimIndex[strTag] = m_mapCurrentAnimIndex[strTag];
	m_mapPreFramePos[strTag] = m_Animations[m_mapPreAnimIndex[strTag]]->m_fCurrentTrackPosition;

	m_mapCurrentAnimIndex[strTag] = iAnimIndex;
	m_Animations[m_mapCurrentAnimIndex[strTag]]->Reset();

	if (bInterpole)
		m_mapRemainTime[strTag] = 1.f;
	else
		m_mapRemainTime[strTag] = 0.f;
	m_mapisLoop[strTag] = isLoop;
}

void CModel::Play_Animation(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix, _uint _iRootBoneIndex)
{
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		m_Animations[m_iCurrentAnimIndex]->m_iRootBoneIndex = _iRootBoneIndex;

	if (m_fRemainTime > 0.f)
	{
		m_fRemainTime = m_fRemainTime - fTimeDelta * (1.f / fInterPoleTime);
		if (m_fRemainTime > 0.f)
		{
			m_Animations[m_iCurrentAnimIndex]->Set_Aperture(m_fRemainTime);
			m_Animations[m_iCurrentAnimIndex]->Update_Transformation_Aperture(fPlayTimeDelta, m_Animations[m_iPreAnimIndex], m_fPreFramePos, m_Bones);
		}
		else
		{
			m_iPreAnimIndex = -1;
			m_Animations[m_iCurrentAnimIndex]->Set_Aperture(0.f);
			m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fPlayTimeDelta, m_Bones, m_isLoop);
		}
	}
	else
	{
		m_iPreAnimIndex = -1.f;
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fPlayTimeDelta, m_Bones, m_isLoop);
	}

	// For Root Animation
	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = m_Animations[m_iCurrentAnimIndex]->m_iRootBoneIndex;
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Play_Non_InterPole_Animation(_float fTimeDelta, _float4x4& outMatrix, _uint _iRootBoneIndex)
{
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		m_Animations[m_iCurrentAnimIndex]->m_iRootBoneIndex = _iRootBoneIndex;

	m_iPreAnimIndex = -1.f;
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);

	// For Root Animation
	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = m_Animations[m_iCurrentAnimIndex]->m_iRootBoneIndex;
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Set_AnimFrame(_float fFramePos, _bool isWithEffect)
{
	m_iPreAnimIndex = -1.f;
	m_Animations[m_iCurrentAnimIndex]->Update_Transform_Frame(fFramePos, m_Bones, m_isLoop, isWithEffect);

	// For Root Animation
	_float4x4 outMatrix;

	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = 2; //m_Animations[m_iCurrentAnimIndex]->Get_Channel(0)->Get_BoneIndex();
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Set_AnimFrame_With_Effect(_float fFramePos)
{
	Set_AnimFrame(fFramePos, true);
}

void CModel::Ready_Animation(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _uint iRootBoneIndex)
{
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		m_Animations[m_iCurrentAnimIndex]->m_iRootBoneIndex = iRootBoneIndex;

	if (m_fRemainTime > 0.f)
	{
		m_fRemainTime = m_fRemainTime - fTimeDelta * (1.f / fInterPoleTime);
		if (m_fRemainTime > 0.f)
		{
			m_Animations[m_iCurrentAnimIndex]->Set_Aperture(m_fRemainTime);
			m_Animations[m_iCurrentAnimIndex]->Update_Transformation_Aperture(fPlayTimeDelta, m_Animations[m_iPreAnimIndex], m_fPreFramePos, m_Bones);
		}
		else
		{
			m_iPreAnimIndex = -1;
			m_Animations[m_iCurrentAnimIndex]->Set_Aperture(0.f);
			m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fPlayTimeDelta, m_Bones, m_isLoop);
		}
	}
	else
	{
		m_iPreAnimIndex = -1.f;
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fPlayTimeDelta, m_Bones, m_isLoop);
	}
}

void CModel::Update_Bones(_float4x4& outMatrix)
{
	// For Root Animation
	_int iRootBoneIndex = -1;
	if (m_Animations[m_iCurrentAnimIndex]->m_isRootAnim)
		iRootBoneIndex = m_Animations[m_iCurrentAnimIndex]->m_iRootBoneIndex;
	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		if (iRootBoneIndex == i)
		{
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix), m_Animations[m_iCurrentAnimIndex]->m_RootAnimPreInfo, outMatrix, m_Animations[m_iCurrentAnimIndex]->m_isLooped);
			m_Animations[m_iCurrentAnimIndex]->m_isLooped = false;
		}
		else
			m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::Set_Basic_BoneMatrix(_int iBoneIdx, _matrix mMatrix)
{
	 m_Bones[iBoneIdx]->Set_TransformationMatrix(mMatrix);
}

void CModel::Init_Model(const char* sModelKey, const string& szTexturePath, _uint iTimeScaleLayer)
{
	//Reset_Model();

	//m_pModelData = GAMEINSTANCE->Get_ModelFromKey(sModelKey);

	if (!m_pModelData)
		assert(false);

	m_szModelKey = sModelKey;

	Init_Model_Internal(m_pModelData, szTexturePath, iTimeScaleLayer);

}

HRESULT CModel::Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	if (m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType] == nullptr)
		return E_FAIL;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

void CModel::Init_Model_Internal(shared_ptr<MODEL_DATA> pModelData, const string& szTexturePath, _uint iTimeScaleLayer)
{
	std::vector<std::thread> threads;

	m_iNumMeshes = pModelData->iNumMeshs;
	m_eModelType = pModelData->eModelType;
	//XMStoreFloat4x4(&m_TransformMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	//XMStoreFloat4x4(&m_TransformMatrix, XMLoadFloat4x4(&pModelData->TransformMatrix));
	//XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

	if (szTexturePath.empty())
	{
		char szDir[MAX_PATH];
		_splitpath_s(m_pModelData->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		threads.emplace_back(&CModel::Create_Materials, this, szDir);
		//Create_Materials(szDir);
	}
	else
	{
		threads.emplace_back(&CModel::Create_Materials, this, szTexturePath.c_str());
		//Create_Materials(szTexturePath.c_str());
	}

	//threads.emplace_back(&CModel::Create_Bone, this, m_pModelData.get()->RootNode, 0);
	Create_Bone(m_pModelData.get()->RootNode, -1);

	/*for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pModelData->Mesh_Datas[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return;

		m_Meshes.emplace_back(pMesh);
	}*/
	threads.emplace_back(&CModel::Mesh_Array, this, pModelData);

	//std::sort(m_pBoneNodes.begin(), m_pBoneNodes.end(), [](weak_ptr<CBoneNode> pSour, weak_ptr<CBoneNode> pDest)
	//	{
	//		return pSour.lock()->Get_Depth() < pDest.lock()->Get_Depth();
	//	});

	for (auto& th : threads) {
		if (th.joinable()) {
			th.join();
		}
	}

	//threads.emplace_back(&CModel::Create_Animations, this);
	Create_Animations();



}

HRESULT CModel::Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex)
{
	/* 쉐이더에 던져야 할 행렬 배열정보. */
	_float4x4	BoneMatrices[768];

	/* 행렬배열은 메시에게 영향을 주는 뼈들의 행렬만 가져야하므로 메시에게 저장좀해줘! */
	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 768);
}

HRESULT CModel::ExportAnimInfo(const string& strFolderPath)
{
	return m_Animations[Get_CurrentAnimIndex()]->ExportAnimInfo(strFolderPath);
}

string CModel::Get_AnimString(_uint AnimIndex) const
{
	if (m_Animations.size() <= AnimIndex)
		assert(false);

	return m_Animations[AnimIndex]->Get_AnimName();
}

HRESULT CModel::SetAnimCallback(const string& strAnimName, _uint iCallbackPos, function<void()> callback)
{
	if (m_AnimMapping.find(strAnimName) == m_AnimMapping.end())
		return E_FAIL;

	_uint iAnimIndex = m_AnimMapping[strAnimName];
	if (m_Animations[iAnimIndex]->m_FrameCallbackList.size() == 0 ||
		m_Animations[iAnimIndex]->m_FrameCallbackList.size() <= iCallbackPos
		)
		return E_FAIL;

	m_Animations[iAnimIndex]->m_CallbackList[iCallbackPos] = callback;
	return S_OK;

}

HRESULT CModel::SetLastAnimCallback(const string& strAnimName, function<void()> callback)
{
	if (m_AnimMapping.find(strAnimName) == m_AnimMapping.end())
		return E_FAIL;

	_uint iAnimIndex = m_AnimMapping[strAnimName];
	if (m_Animations[iAnimIndex]->m_FrameCallbackList.size() == 0)
		return E_FAIL;

	m_Animations[iAnimIndex]->m_CallbackList.back() = callback;
	return S_OK;
}

HRESULT CModel::SetEffectCallback(function<void(string, _float4x4, _bool, _bool)> callbackEffect)
{
	for (auto pAnim : m_Animations)
		pAnim-> m_EffectCaller = callbackEffect;

	return S_OK;
}

void CModel::PlayReservedCallbacks()
{
	m_Animations[m_iCurrentAnimIndex]->PlayReservedCallbacks();
}

void CModel::Reset_Model()
{
	m_pModelData.reset();
	m_szModelKey.clear();

	m_Meshes.clear();
	m_Materials.clear();
	m_Bones.clear();
	m_Animations.clear();

	m_iCurrentAnimIndex = 0;
	m_iNumAnimations = 0;
	m_iNumMaterials = 0;
}
#ifdef _DEBUG
HRESULT CModel::Ready_Meshes()
{

	/* 메시의 갯수를 저장하낟. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}
#endif
void CModel::Convert_To_Instancing_Model(_uint iNumInstance)
{
	for (_uint i = 0; i < m_Meshes.size(); ++i)
	{
		if (i == 0)
		{
			m_Meshes[i]->Initialize_InstanceBuffer(iNumInstance, &m_pVBInstance);
		}
		else
		{
			m_Meshes[i]->Initialize_InstanceBuffer(iNumInstance);
		}
	}
}

HRESULT CModel::Render_Instance(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render_Instance(m_pVBInstance);
	return S_OK;
}

void CModel::Play_Animation_Split(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix)
{
	map<_uint, _bool> Looped;
	
	for (auto& Split : m_mapSplitBone)
	{
		const string& Tag = Split.first;
		m_mapIsFinished[Tag] = false;
		auto iter = Looped.find(m_mapCurrentAnimIndex[Tag]);
		if (iter != Looped.end())
		{
			if (!m_mapisLoop[Tag] && iter->second)
			{
				m_mapIsFinished[Tag] = true;
			}
		}
		else
		{
			Looped.emplace(m_mapCurrentAnimIndex[Tag], false);
			m_Animations[m_mapCurrentAnimIndex[Tag]]->Set_Moved(false);
		}
		if (m_mapRemainTime[Tag] > 0.f)
		{
			m_mapRemainTime[Tag] = m_mapRemainTime[Tag] - fTimeDelta * (1.f / fInterPoleTime);
			if (m_mapRemainTime[Tag] > 0.f)
			{
				m_Animations[m_mapCurrentAnimIndex[Tag]]->Set_Aperture(m_mapRemainTime[Tag]);
				m_Animations[m_mapCurrentAnimIndex[Tag]]->Update_Transformation_Aperture_Split(fPlayTimeDelta, m_Animations[m_mapPreAnimIndex[Tag]], m_mapPreFramePos[Tag], m_Bones, Split.second);
			}
			else
			{
				m_mapPreAnimIndex[Tag] = -1;
				m_Animations[m_mapCurrentAnimIndex[Tag]]->Set_Aperture(m_mapRemainTime[Tag]);
				m_Animations[m_mapCurrentAnimIndex[Tag]]->Update_TransformationMatrix_Split(fPlayTimeDelta, m_Bones, m_mapisLoop[Tag], Split.second, m_mapIsFinished[Tag], Looped[m_mapCurrentAnimIndex[Tag]]);
			}
		}
		else
		{
			m_mapPreAnimIndex[Tag] = -1;
			m_Animations[m_mapCurrentAnimIndex[Tag]]->Update_TransformationMatrix_Split(fPlayTimeDelta, m_Bones, m_mapisLoop[Tag], Split.second, m_mapIsFinished[Tag], Looped[m_mapCurrentAnimIndex[Tag]]);
		}
	}


	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}
}

void CModel::SplitBones(_uint iNumSplit, const vector<vector<string>>& vecNames)
{
	assert(iNumSplit > 0);
	_uint iSize = static_cast<_uint>(vecNames.size());
	_bool bAutoSplit = iSize == iNumSplit - 1;
	_bool bManualSplit = iSize == iNumSplit;
	assert(bAutoSplit != bManualSplit);

	vector<_bool> Check;
	Check.resize(m_Bones.size());

	vector<vector<_uint>> BoneChildrenIdx;
	BoneChildrenIdx.resize(m_Bones.size());
	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		for (_uint j = i + 1; j < m_Bones.size(); ++j)
		{
			if (i == m_Bones[j]->Get_Parent())
			{
				BoneChildrenIdx[i].push_back(j);
			}
		}
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		assert(0 == m_mapSplitBone.count(vecNames[i][0]));
		m_mapSplitBone.emplace(vecNames[i][0], vector<_uint>{});
		m_mapisLoop.emplace(vecNames[i][0], false);
		m_mapCurrentAnimIndex.emplace(vecNames[i][0], 0);
		m_mapPreAnimIndex.emplace(vecNames[i][0], 0);
		m_mapPreFramePos.emplace(vecNames[i][0], 0.f);
		m_mapRemainTime.emplace(vecNames[i][0], 0.f);
		m_mapIsFinished.emplace(vecNames[i][0], false);
		for (_uint j = 0; j < vecNames[i].size(); ++j)
		{
			_uint iParentIdx = 0;
			for (auto& Bone : m_Bones)
			{
				if (vecNames[i][j] == Bone->Get_Name())
				{
					break;
				}
				++iParentIdx;
			}
			Check[iParentIdx] = true;
			m_mapSplitBone[vecNames[i][0]].push_back(iParentIdx);
			Push_SplitBones(iParentIdx, BoneChildrenIdx, m_mapSplitBone[vecNames[i][0]], Check);
		}
	}
	if (bAutoSplit)
	{
		m_mapSplitBone.emplace("Rest", vector<_uint>{});
		for (_uint i = 0; i < Check.size(); ++i)
		{
			if (!Check[i])
			{
				m_mapSplitBone["Rest"].push_back(i);
			}
		}
	}

}

void CModel::Push_SplitBones(_uint iParentIdx, const vector<vector<_uint>>& vecBone, vector<_uint>& vecPush, vector<_bool>& Check)
{
	if (iParentIdx >= vecBone.size())
		return;
	for (auto& Child : vecBone[iParentIdx])
	{
		Check[Child] = true;
		vecPush.push_back(Child);
		Push_SplitBones(Child, vecBone, vecPush, Check);
	}
}
#ifdef _DEBUG
HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	//std::vector<std::thread> Matthreads;
	/* 모델에는 여러개의 머테리얼이 존재하낟. */
	/* 각 머테리얼마다 다양한 재질(Diffuse, Ambient,Specular...)을 텍스쳐로 표현한다. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		//Matthreads.emplace_back(&CModel::Material_Array, this, pModelFilePath, (int)i);
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			/* 얻어온 경로에서 신용할 수 있는 건 파일의 이름과 확장자. */
			aiString		strTexturePath;

			/* 내가 지정한 텍스쳐의 경로를 얻어주낟.*/
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
			_char		szFileName[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szPerfectPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			//MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
			MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);
			
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	/*for (auto& th : Matthreads) {
		if (th.joinable()) {
			th.join();
		}
	}*/

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pNode, _int iParentIndex)
{
	shared_ptr<CBone> pBone = CBone::Create(pNode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iNumChildren = pNode->mNumChildren;

	_int		iNumBones = (_int)m_Bones.size();

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Ready_Bones(pNode->mChildren[i], iNumBones - 1);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		shared_ptr<CAnimation> pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
		m_AnimMapping.emplace(pAnimation->Get_AnimName(), i);
	}

	return S_OK;
}
#endif

void CModel::Mesh_Array(shared_ptr<MODEL_DATA> pModelData)
{
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pModelData->Mesh_Datas[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return;

		m_Meshes.emplace_back(pMesh);
	}
}

void CModel::Material_Array(const _char* pModelFilePath, int i)
{
	unique_lock<shared_mutex> lock(m_Mtx);

	MATERIAL_DATA* pMaterial = m_pModelData->Material_Datas[i].get();

	MATERIAL_MESH			MeshMaterial = {};

	for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
	{

		_char		szFullPath[MAX_PATH] = {};
		/* 모델파일경로로부터 뜯어온 문자열. */
		_char		szDrive[MAX_PATH] = {};
		_char		szDir[MAX_PATH] = {};

		/* 모델파일(aiScene)로부터 뜯어온 문자열. */
		_char		szFileName[MAX_PATH] = {};
		_char		szExt[MAX_PATH] = {};

		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		_splitpath_s(pMaterial->szTextureName[j].data(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (szFileName[0] == '\0')
			continue;

		strcpy_s(szFullPath, szDrive);
		strcat_s(szFullPath, szDir);
		strcat_s(szFullPath, szFileName);
		strcat_s(szFullPath, szExt);

		_tchar		szPerfectPath[MAX_PATH] = {};

		MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

		//MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
		MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);

		if (nullptr == MeshMaterial.pTextures[j])
			continue;
	}
	m_Materials.emplace_back(MeshMaterial);

}

void CModel::LoadSoundList(const _char* pModelFilePath)
{
	filesystem::path pathSound(pModelFilePath);
	string strSoundPath = pathSound.parent_path().string();
	strSoundPath += "/Sound/";

	if (!filesystem::exists(strSoundPath))
		return;

	for (const auto& entry : filesystem::directory_iterator(strSoundPath)) {
		if (entry.is_regular_file()) {
			wstring fileName = entry.path().filename().wstring();
			fileName.erase(fileName.length() - 4);
			wstring filePath = entry.path().wstring();

			GAMEINSTANCE->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, fileName, filePath);
		}
	}
}

shared_ptr<CModel> CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	struct MakeSharedEnabler : public CModel
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CModel(pDevice, pContext) { }
	};

	shared_ptr<CModel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CComponent> CModel::Clone(void* pArg)
{

	struct MakeSharedEnabler : public CModel
	{
		MakeSharedEnabler(const CModel& rhs) : CModel(rhs) { }
	};

	shared_ptr<CModel> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModel"));
		assert(false);
	}

	return pInstance;
}

void CModel::Free()
{

}

HRESULT CModel::Create_Materials(const _char* pModelFilePath)
{

	vector<thread> Matthreads;

	m_iNumMaterials = m_pModelData->iNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MATERIAL_DATA* pMaterial = m_pModelData->Material_Datas[i].get();

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{

			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
			_char		szFileName[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};
			_char		strTga[] = ".tga";

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(pMaterial->szTextureName[j].data(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			if (szFileName[0] == '\0')
				continue;
			if (strcmp(szExt, strTga) == false) {
				strcpy_s(szExt, ".dds");
			}


			_tchar		szPerfectPath[MAX_PATH] = {};

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			// ALBM -> ATOS
			if (j == 2)	
			{
				string strTarget = szFileName;
				string toReplace = "ALBM";
				string replacement = "ATOS_NoesisAO";

				size_t pos = strTarget.find(toReplace);
				if (pos != std::string::npos)
					strTarget.replace(pos, toReplace.length(), replacement);
				strcat_s(szFullPath, strTarget.c_str());
				strcat_s(szFullPath, szExt);

				MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);
				_bool isExist = filesystem::exists(szPerfectPath);
				if (!isExist)
				{
					ZeroMemory(szPerfectPath, sizeof(szPerfectPath));
					_char szTempPath[MAX_PATH] = "../Bin/Resources/Textures/NullWhite.png";
					MultiByteToWideChar(CP_ACP, 0, szTempPath, (_int)strlen(szTempPath), szPerfectPath, MAX_PATH);
					//continue;
				}
			}
			else
			{
				strcat_s(szFullPath, szFileName);
				strcat_s(szFullPath, szExt);

				MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);
			}

			MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	for (auto& th : Matthreads) {
		if (th.joinable()) {
			th.join();
		}
	}

	return S_OK;
}

HRESULT CModel::Bind_Buffers(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_Buffer();
}


HRESULT CModel::Create_Animations()
{
	m_iNumAnimations = m_pModelData->iNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		shared_ptr<CAnimation> pAnimation = CAnimation::Create(m_pModelData->Animation_Datas[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
		m_AnimMapping.emplace(pAnimation->Get_AnimName(), i);
	}

	return S_OK;
}

HRESULT CModel::Create_Bone(shared_ptr<NODE_DATA> pNode, _int iParentIndex)
{
	shared_ptr<CBone> pBone = CBone::Create(pNode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iNumChildren = pNode->iNumChildren;

	_int		iNumBones = (_int)m_Bones.size();

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Create_Bone(pNode->pChildren[i], iNumBones - 1);
	}

	return S_OK;


}
