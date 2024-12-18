#pragma once

#include "Component.h"
#include "Animation.h"


/* 모델러분들이 작성해놓은 정점의 정보와 인덱스의 정보를 로드하여 내가 사용하기좋도록 정리한다. */
/* Vertex -> Polygon -> Mesh -> Model */

BEGIN(Engine)

class ENGINE_DLL CModel : public CComponent
{
public:
	typedef struct tagModelMaterial
	{
		weak_ptr<CTexture> pTextures[AI_TEXTURE_TYPE_MAX];
	}MODEL_MATERIAL;

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);

public:
	virtual ~CModel();

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_NumAnimation() const {
		return (_uint)m_Animations.size();
	}

	_uint Get_NumBones() const {
		return (_uint)m_Bones.size();
	}

	_uint Get_BoneIdx(const _char* pBoneName) const;

	_bool Get_Finished() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Finished();
	}
	_bool Get_Finished(_uint iIdx) {
		return m_Animations[iIdx]->Get_Finished();
	}
	void Set_Finished(_bool isFinished) {
		m_Animations[m_iCurrentAnimIndex]->Set_Finished(isFinished);
	}

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	const _float4x4* Get_BoneMatrixPtr(_uint iBoneIdx) const;
	void Set_BoneMatrixPtr(const _char* pBoneName, const _float4x4& Transform);
	void Set_BoneMatrixPtr(_uint iBoneIdx, const _float4x4& Transform);

	const _float4x4* Get_BoneBasicMatrixPtr(const _char* pBoneName) const;

	const _vector Get_BonePosition(const _char* pBoneName, _vector Pos) const;
	const shared_ptr<MODEL_DATA> Get_ModelData() const {
		return m_pModelData;
	}

	void Set_AnimIndex(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndex(m_AnimMapping[animName], isLoop);
	}
	void Set_AnimIndexNonCancle(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndexNonCancle(m_AnimMapping[animName], isLoop);
	}
	void Set_AnimIndex_NoInterpole(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndex(m_AnimMapping[animName], isLoop, false);
	}
	void Set_AnimIndexNonCancle_NoInterpole(const string& animName, _bool isLoop)
	{
		if (animName.size() == 0)
			return;
		Set_AnimIndexNonCancle(m_AnimMapping[animName], isLoop, false);
	}
	void Set_AnimIndex(_uint iAnimIndex, _bool isLoop, _bool bInterpole = true)
	{
		if (m_isLoop && m_iCurrentAnimIndex == iAnimIndex)
			return;

		if (m_iNumAnimations <= iAnimIndex)
			iAnimIndex = 0;

		m_iPreAnimIndex = m_iCurrentAnimIndex;
		m_fPreFramePos = m_Animations[m_iPreAnimIndex]->m_fCurrentTrackPosition;
		m_Animations[m_iPreAnimIndex]->m_CallbackList.back()();

		m_iCurrentAnimIndex = iAnimIndex;
		m_Animations[m_iCurrentAnimIndex]->Reset();

		if (m_Animations[m_iPreAnimIndex]->m_Channels.size() != m_Animations[m_iCurrentAnimIndex]->m_Channels.size())
			bInterpole = false;

		if (bInterpole)
			m_fRemainTime = 1.f;
		else
			m_fRemainTime = 0.f;
		m_isLoop = isLoop;
	}

	void Set_AnimIndexNonCancle(_uint iAnimIndex, _bool isLoop, _bool bInterpole = true) {
		if (iAnimIndex == m_iCurrentAnimIndex || iAnimIndex == m_iPreAnimIndex)
			return;

		m_iPreAnimIndex = m_iCurrentAnimIndex;
		m_fPreFramePos = m_Animations[m_iPreAnimIndex]->m_fCurrentTrackPosition;
		m_Animations[m_iPreAnimIndex]->m_CallbackList.back()();

		m_iCurrentAnimIndex = iAnimIndex;
		m_Animations[m_iCurrentAnimIndex]->Reset();

		if (m_Animations[m_iPreAnimIndex]->m_Channels.size() != m_Animations[m_iCurrentAnimIndex]->m_Channels.size())
			bInterpole = false;

		if (bInterpole)
			m_fRemainTime = 1.f;
		else
			m_fRemainTime = 0.f;

		m_isLoop = isLoop;
	}

	void Set_AnimSpeed(_float fSpeed) {
		m_Animations[m_iCurrentAnimIndex]->Set_AnimationMultiplier(fSpeed);
	}

	shared_ptr<class CMesh> Get_Mesh(_uint iMeshIndex) {
		if (m_iNumMeshes <= iMeshIndex)
			return nullptr;

		return m_Meshes[iMeshIndex];
	}
	_uint Get_MaterialIndex(_uint iMeshIndex);
	_bool Check_Material(_uint iMeshIndex, aiTextureType eMaterialType);
	_float Get_AnimTickPerSecond() {
		if (m_iCurrentAnimIndex < 0 || m_iCurrentAnimIndex >= m_Animations.size())
			return 1.f;
		return m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
	}
	_float Get_Duration() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Duration();
	}
	_float Get_Duration(_uint iIdx) {
		return m_Animations[iIdx]->Get_Duration();
	}
	_float Get_Duration(const string& animName) {
		auto iAnimIndex = m_AnimMapping[animName];
		return m_Animations[iAnimIndex]->Get_Duration();
	}
	_float Get_Current_Track_Position() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Current_Track_Posiiton();
	}
	_float Get_Current_Anim_Track_Position_Percentage() {
		return Get_Current_Track_Position() / Get_Duration();
	}
	void  Set_CurrentTrackPosition(_float fCurrentTrackPosition) {
		m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fCurrentTrackPosition);
	}
	_float Get_Animation_PlayTime() {
		return Get_Duration() / m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
	}
	_float Get_Animation_PlayTime(_uint iIdx) {
		return Get_Duration(iIdx) / m_Animations[iIdx]->Get_TickPerSecond();
	}
	_float Get_Animation_PlayTime(const string& animName) {
		auto iAnimIndex = m_AnimMapping[animName];
		return Get_Duration(iAnimIndex) / m_Animations[iAnimIndex]->Get_TickPerSecond();
	}

	void Get_BoneMatrices(vector<_float4x4>& vecOut, _uint iMeshIndex);
public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);
	virtual HRESULT Draw(_uint iMeshIndex);

	void	Generate_Mips();

public:
	void Init_Model(const char* sModelKey, const string& szTexturePath = "", _uint iTimeScaleLayer = 0);
	void Init_Model_Internal(shared_ptr<MODEL_DATA> pModelData, const string& szTexturePath, _uint iTimeScaleLayer);
	void Reset_Model();

	void Play_Animation(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix, _uint iRootBoneIndex = 2);
	void Play_Non_InterPole_Animation(_float fTimeDelta, _float4x4& outMatrix, _uint iRootBoneIndex = 2);
	void Set_AnimFrame(_float fFramePos, _bool isWithEffect = false);
	void Set_AnimFrame_With_Effect(_float fFramePos);

	void Ready_Animation(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _uint iRootBoneIndex = 2);
	void Update_Bones(_float4x4& outMatrix);
	void Set_Basic_BoneMatrix(_int iBoneIdx, _matrix mMatrix);

	HRESULT Bind_Buffers(_uint iMeshIndex);
	HRESULT Bind_Material(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex);

	_uint Get_CurrentAnimIndex() {
		return m_iCurrentAnimIndex;
	}

public:
	vector<shared_ptr<class CAnimation>>& Get_Animation_List() { return m_Animations; }
	HRESULT ExportAnimInfo(const string& strFolderPath);

	_uint Get_StringAnimIndex(const string& animName) {
		return m_AnimMapping[animName];
	}

	string Get_AnimString(_uint AnimIndex) const;

public:
	vector<shared_ptr<class CBone>>& Get_Bone_List() { return m_Bones; }

public:
	HRESULT SetAnimCallback(const string& strAnimName, _uint iCallbackPos, function<void()> callback);
	HRESULT SetLastAnimCallback(const string& strAnimName, function<void()> callback);

	/** Client Effect 호출 함수 */
	HRESULT SetEffectCallback(function<void(string, _float4x4, _bool, _bool)> callbackEffect);

	void PlayReservedCallbacks();

private:
	/* 디자이너가 제공해준 파일의 정보를 읽어서 aiScene에 저장해주는 역활을 수행한다. */
#ifdef _DEBUG
	Assimp::Importer			m_Importer = {};
	/* 모델에 대한 모든 정보를 다 들고 있는 객체다. */
	const aiScene* m_pAIScene = { nullptr };
#endif

protected:
	shared_ptr<MODEL_DATA>			m_pModelData;
	string							m_szModelKey;

private:
	MODEL_TYPE						m_eModelType = { MODEL_TYPE::TYPE_END };
	_float4x4						m_TransformMatrix = {};

private:
	_uint							m_iNumMeshes = {};
	vector<shared_ptr<class CMesh>>	m_Meshes;

private:
	_uint							m_iNumMaterials = {};
	vector<MATERIAL_MESH>			m_Materials;

private:
	/* 이 모델 전체의 뼈들을 모아놓은 벡터 컨테이너. */
	vector<shared_ptr<class CBone>>			m_Bones;

public:
	_bool IsRootAnim() const {
		return m_Animations[m_iCurrentAnimIndex]->m_isRootAnim;
	}
	_bool IsForcedNonRoot() const {
		return m_Animations[m_iCurrentAnimIndex]->m_isForcedNonRootAnim;
	}
	string Get_AnimName() const {
		return m_Animations[m_iCurrentAnimIndex]->Get_AnimName();
	}
private:
	_bool										m_isLoop = { false };
	_uint										m_iCurrentAnimIndex = {};

	_int										m_iPreAnimIndex = -1;
	_float										m_fPreFramePos = 0.f;
	_float										m_fRemainTime = 0.f;

	_uint										m_iNumAnimations = { 0 };
	vector<shared_ptr<class CAnimation>>		m_Animations;
	map<string, _uint>							m_AnimMapping;

#pragma region INSTANCE
public:
	void Convert_To_Instancing_Model(_uint iNumInstance);
	HRESULT Render_Instance(_uint iMeshIndex);
	ID3D11Buffer* Get_Instance_Buffer() { return m_pVBInstance; }
private:
	ID3D11Buffer* m_pVBInstance = nullptr;
	_uint m_iNumInstance = 0;
	_uint m_iNumMaxInstance = 0;
#pragma endregion

#pragma region SPLIT_ANIM
public:
	void Play_Animation_Split(_float fTimeDelta, _float fPlayTimeDelta, _float fInterPoleTime, _float4x4& outMatrix);
	void SplitBones(_uint iNumSplit, const vector<vector<string>>& vecNames);
	void Set_AnimIndex_Split(_uint iAnimIndex, _bool isLoop, const string& strTag, _bool bInterpole = true);
	_uint Get_CurrentAnimIndex_Split(const string& strTag) {
		return m_mapCurrentAnimIndex[strTag];
	}
	_bool Get_Finished_Split(const string& strTag) { return m_mapIsFinished[strTag]; }
private:
	void Push_SplitBones(_uint iParentIdx, const vector<vector<_uint>>& vecBone, vector<_uint>& vecPush, vector<_bool>& Check);

private:
	map<string, _bool>		m_mapisLoop;
	map<string, _uint>		m_mapCurrentAnimIndex;

	map<string, _int>		m_mapPreAnimIndex;
	map<string, _float>		m_mapPreFramePos;
	map<string, _float>		m_mapRemainTime;

	map<string, vector<_uint>> m_mapSplitBone;
	map<string, _bool> m_mapIsFinished;
#pragma endregion
private:
#ifdef _DEBUG
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pNode, _int iParentIndex);
	HRESULT Ready_Animations();
#endif

	void Mesh_Array(shared_ptr<MODEL_DATA> pModelData);
	void Material_Array(const _char* pModelFilePath, int i);

	//void Create_ORM_Material(MODEL_MATERIAL& Out_Material, const _uint In_iMaterialIndex, const _char* pModelFilePath);
	HRESULT Create_Materials(const _char* pModelFilePath);
	HRESULT Create_Bone(shared_ptr<NODE_DATA> pNode, _int iParentIndex);
	HRESULT Create_Animations();

	shared_mutex m_Mtx;
	shared_mutex m_MaterialMtx;

#pragma region Sound_Depended
private:
	void LoadSoundList(const _char* pModelFilePath);
#pragma endregion

public:
	static shared_ptr<CModel> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END