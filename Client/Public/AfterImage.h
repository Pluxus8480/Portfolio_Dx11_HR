#pragma once
#include "Client_Defines.h"
#include "Model.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CAfterImage : public CGameObject
{
public:
	class AFTERIMAGE_DESC
	{
	public:
		_uint iRenderPass = 0;
		_bool bAnimated = false;
		_uint iMaxImage = 0;
		_float fLifeTime = 0.f;
		_float4 vColor;
	};
protected:
	CAfterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAfterImage(const CAfterImage& rhs);
public:
	virtual ~CAfterImage();

public:
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void SetUp_Matrices(const _float4x4& Transform);

	virtual void Push_From_Pool(void* pArg) override;
	virtual void Dying() override;
	virtual HRESULT Initialize_Prototype(const AFTERIMAGE_DESC& desc);
	virtual HRESULT Initialize(void* Arg);

	void Set_Color(const _float4& vColor) { m_tDesc.vColor = vColor; }
	void Set_LifeTime(_float fLT) { m_tDesc.fLifeTime = fLT; }
	_bool m_bActive = false;
protected:
	AFTERIMAGE_DESC m_tDesc;
	_uint m_iRenderPass = 0;
	shared_ptr<CModel>* m_pModelCom = nullptr;
	shared_ptr<CShader> m_pShaderCom = nullptr;
	//CTexture* m_pTextureCom = nullptr;
	_uint m_iNumImages = 0;
	vector<_float4x4> m_Transforms;
	vector<vector<vector<_float4x4>>> m_BoneMatrices;//[NumMaxImage][NumMesh]
	vector<_float> m_fLifeTime;

public:
	static shared_ptr<CAfterImage> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const AFTERIMAGE_DESC& desc);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END