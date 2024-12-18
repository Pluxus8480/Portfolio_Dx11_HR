#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CModel;
END

BEGIN(Client)

class CMovingFloor : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CMovingFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMovingFloor(const CMovingFloor& rhs);
public:
	~CMovingFloor();

public:
	virtual void Execute(_bool bExecute = true);

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
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

	_float3 m_vModelCenter = {};
	_float3 m_vModelSize = {};

	_float m_fCurRotation = { 0.f };

	_bool m_bUp = { false };

#ifdef _DEBUG
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
#endif
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CMovingFloor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END