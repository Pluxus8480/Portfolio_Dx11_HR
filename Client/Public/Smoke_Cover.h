#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)

class CSmoke_Cover : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CSmoke_Cover(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSmoke_Cover(const CSmoke_Cover& rhs);
public:
	~CSmoke_Cover();

public:
	virtual void Execute(_bool bExecute = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
#ifdef  _DEBUG
#endif //  _DEBUG


	shared_ptr<CPhysXCollider> m_pPhysxCollider = nullptr;
	_int m_iPrevBeatCnt = { 0 };
	_int m_iHeight = 0;
	const _float4x4* m_pBone;
	_int m_iDelay = 0;
	_int m_iAirTime = 3;
	_int m_iCurrAirTime = 3;

	_float4 m_fBeforePos = {};
	/*
	0: Çª»þ¾Ñ ÁØºñ
	1,4,7: »ó½Â
	2,5,8: Ã¤°ø
	3,6,9: ÇÏ°­
	10: ¼­ÅÂÁö¿Í ¾ÆÀÌµé
	*/

	_bool m_bCollision = { false };
	_bool m_bWait = { true };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CSmoke_Cover> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END