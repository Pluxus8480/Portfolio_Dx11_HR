#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CRhythm_RobotArm : public CGameObject
{
public:
	static wstring ObjID;

private:
	CRhythm_RobotArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRhythm_RobotArm(const CRhythm_RobotArm& rhs);
public:
	~CRhythm_RobotArm();

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

	_uint m_iCurIndex = {};
	vector<_uint> m_vecAnimIndex;
	vector<_float> m_vecAnimSpeed;

	_bool m_bInit = { false };

#ifdef _DEBUG
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
#endif
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CRhythm_RobotArm> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END