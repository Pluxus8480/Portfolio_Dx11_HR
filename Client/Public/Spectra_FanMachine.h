#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CSpectra_FanMachine : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CSpectra_FanMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpectra_FanMachine(const CSpectra_FanMachine& rhs);
public:
	~CSpectra_FanMachine();

public:
	virtual void Set_Dead(bool bDead = true);

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
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
#ifdef _DEBUG
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
#endif

	shared_ptr<CGameObject> m_pBlade[2] = {nullptr, };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSpectra_FanMachine> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END