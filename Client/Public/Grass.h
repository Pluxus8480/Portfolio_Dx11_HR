#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CGrass : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrass(const CGrass& rhs);
public:
	~CGrass();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom[2] = { nullptr, };

	vector<shared_ptr<class CGrassMatrix>> m_vecMatrixObj[2];
	_float m_fCullingRange = {};

	/* Tool */
	_int m_iModelIndex = { 0 };
	_float m_fDistance = {};

#ifdef _DEBUG
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
#endif

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CGrass> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END