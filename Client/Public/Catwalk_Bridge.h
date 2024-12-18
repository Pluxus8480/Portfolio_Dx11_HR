#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)

class CCatwalk_Bridge : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CCatwalk_Bridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCatwalk_Bridge(const CCatwalk_Bridge& rhs);
public:
	~CCatwalk_Bridge();

public:
	virtual void Execute(_bool bExecute = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

	_float m_fAnimSpeed = { 0.f };
	_bool m_bInit = { false };

	_uint m_iPrevBeat = { 0 };
	_bool m_bAnimStart = { false };

private:
	HRESULT Ready_Components(COMMONOBJ_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CCatwalk_Bridge> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END