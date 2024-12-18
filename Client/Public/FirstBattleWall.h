#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)

class CFirstBattleWall : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CFirstBattleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFirstBattleWall(const CFirstBattleWall& rhs);
public:
	~CFirstBattleWall();

public:
	virtual void Execute(_bool bExecute = true) override;
	virtual void SetUp_CinematicCallBack() override;

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
	//shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

	_float m_fAnimSpeed = { 0.f };
	_bool m_bInit = { false };

	_uint m_iPrevBeat = { 0 };
	_bool m_bAnimStart = { false };
	_uint m_iRenderGroup = 0;
	_bool m_bCulled = false;
	_bool m_bExhausted = false;

private:
	HRESULT Ready_Components(COMMONOBJ_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CFirstBattleWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END