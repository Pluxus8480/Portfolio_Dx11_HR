#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXCollider;
class COctComp;
END

BEGIN(Client)

class CMapObject_Static : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

	struct STATICOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_bool bCollision = { true };
		_float fCullingRange = { 0 };
		wstring strModelTag;
		_float4x4 worldMatrix = {};
	};

private:
	CMapObject_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Static(const CMapObject_Static& rhs);
public:
	virtual ~CMapObject_Static();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);
	void Set_CullingRange(_float fCullingRange) { m_fCullingRange = fCullingRange; }
	void Set_Collision(_bool bCollision) { m_bCollision = bCollision; }

	_bool Get_Collision() const { return m_bCollision; }
	
	virtual void Push_From_Pool(void* pArg) override;

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	void Get_MaterialSet(vector<_uint>& PassIndexCopy, vector<_float4>& DiffuseColorCopy);
	void Set_MaterialSet(const vector<_uint>& PassIndexSour, const vector<_float4>& DiffuseColorSour);

public:
	virtual HRESULT Initialize_Prototype(_uint iLevelIndex);
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	//shared_ptr<CModel> m_pModelCom = { nullptr };
	vector<shared_ptr<CModel>> m_vecModel;
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<COctComp> m_pOctCom = { nullptr };

	_bool m_bCollision = { true };

	_float3 m_vMeshCenter = {};
	_float m_fCullingRange = { 0 };

	_uint m_iPassIndex = { 0 };
	_bool m_bMinusScale = { false };

	_bool m_bBeltEmission = { false };
	_float m_fUvFlow = {};
	CRenderer::RENDERGROUP m_eRenderGroup = { CRenderer::RENDER_TERRAIN };

	_uint m_iLod = { 0 };

private:
	_int m_iMaxMaterial = { 0 };
	_int m_iMaterialIndex = { 0 };

	vector<string> m_vecShaderPass;
	vector<_uint> m_vecPassIndex;
	vector<_float4> m_vecColors;

private:
	_uint m_iMapObjID = 0;
	static _uint s_iMapObjID;

	_uint m_iLevelIndex = {LEVEL_STATIC};

private:
	HRESULT Ready_Components(const STATICOBJ_DESC* pDesc);
	HRESULT Ready_ModelPrototypes(const wstring& strLevelDir);

private:
	void Edit_Material();

public:
	static shared_ptr<CMapObject_Static> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

	//friend class CImgui_Manager;
};

END