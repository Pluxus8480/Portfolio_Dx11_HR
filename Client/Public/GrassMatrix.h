#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CGrassMatrix : public CPartObject
{
public:
	static wstring ObjID;

public:
	struct GRASS_DESC : public CPartObject::PARTOBJ_DESC
	{
		_uint iModelIndex = { 0 };
		_float4x4 worldMatrix;
	};

private:
	CGrassMatrix(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrassMatrix(const CGrassMatrix& rhs);
public:
	~CGrassMatrix();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	HRESULT Bind_Matrix();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };

	_bool m_bCulled = { false };
	_float4x4 m_BillBoardMat = { Identity };

	_uint m_iModelIndex = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CGrassMatrix> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END