#pragma once
#include "Client_Defines.h"
#include "CMagnet.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CCraneMagnet : public CMagnet
{
public:
	static wstring ObjID;
private:
	CCraneMagnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCraneMagnet(const CCraneMagnet& rhs);
public:
	~CCraneMagnet();

public:
	virtual void Execute(_bool bExecute = true) override;
	void Start_CraneEvent();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr< CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[2] = {nullptr,};
	_uint m_iAnimIndex = {};
	_bool m_bSet_Wire_list = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CCraneMagnet> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END