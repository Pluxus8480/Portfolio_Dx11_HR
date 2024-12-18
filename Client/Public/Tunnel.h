#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)

class CTunnel : public CGameObject
{
public:
	static wstring ObjID;

	struct ANIMOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_float4x4 worldMatrix = {};
	};

private:
	CTunnel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTunnel(const CTunnel& rhs);
public:
	virtual ~CTunnel();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();




private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

	vector<_float3> m_vecPositions;

	_bool m_bDebug = true;

	_uint m_iPrevBeat = { 0 };
	//±Ê¿Ã 400(/100)
private:
	HRESULT Ready_Components(const ANIMOBJ_DESC* pDesc);

public:
	static shared_ptr<CTunnel> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END