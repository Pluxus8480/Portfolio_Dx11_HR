#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMesh : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);

public:
	virtual ~CMesh();

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	PxTriangleMesh* Get_TriangleMesh() {
		return m_pTriangleMesh;
	}

	void Cooking_TriangleMesh();
	void Cooking_TriangleMesh(const _float4x4& Transform);

public:
#ifdef _DEBUG
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eModelType, const aiMesh* pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix);
#endif
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eModelType, shared_ptr<MESH_DATA> pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	void SetUp_BoneMatrices(_float4x4* pMatrices, const vector<shared_ptr<class CBone>>& Bones);

	_bool IsAffected(_uint iBoneIdx)const;

	const _char* Get_Name() { return m_szName; }

	const vector<PxVec3>& Get_Vertices() const { return vertices; }

#pragma region INSTANCE
public:
	void Initialize_InstanceBuffer(_uint iNumMaxInstance);
	void Initialize_InstanceBuffer(_uint iNumMaxInstance, ID3D11Buffer** ppBufferOut);

	HRESULT Render_Instance(ID3D11Buffer* pInstanceBuffer);

	HRESULT Bind_Buffer_Instance(ID3D11Buffer* pInstanceBuffer);
	HRESULT Draw_Instance(ID3D11Buffer* pInstanceBuffer);

#pragma endregion

private:

	_char			m_szName[MAX_PATH] = {};
	_uint			m_iMaterialIndex = {};
	_uint			m_iNumBones = {};

	/* 모델 전체의 뼈들 중, 이 메시에 영향을 주는 뼈의 인덱스를 저장했다. */
	vector<_uint>	m_Bones;
	vector<_float4x4>	m_OffsetMatrices;

	vector<PxVec3> vertices;
	vector<PxU32> indices;

	PxTriangleMesh* m_pTriangleMesh = nullptr;

#pragma region INSTANCE

private:
	_uint m_iNumInstance = 0;
	_uint m_iNumMaxInstance = 0;
	_uint m_iInstanceVertexStride = 0;
	_uint m_iNumIndexPerInstance = 0;
#pragma endregion

private:
#ifdef _DEBUG
	HRESULT Ready_VIBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix TransformMatrix);
	HRESULT Ready_VIBuffer_For_Anim(const aiMesh* pAIMesh, vector<shared_ptr<class CBone>>& Bones);
#endif
	HRESULT Ready_VIBuffer_For_NonAnim(shared_ptr<MESH_DATA> pAIMesh, _fmatrix TransformMatrix);
	HRESULT Ready_VIBuffer_For_Anim(shared_ptr<MESH_DATA> pAIMesh, vector<shared_ptr<class CBone>>& Bones);

public:
#ifdef _DEBUG
	static shared_ptr<CMesh> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eModelType, const aiMesh* pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix);
#endif
	static shared_ptr<CMesh> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eModelType, shared_ptr<MESH_DATA> pAIMesh, vector<shared_ptr<class CBone>>& Bones, _fmatrix TransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();


	friend CModel;
};

END