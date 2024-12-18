/*
0: reward ¿Ã¶ó¿È
1: reward ³»·Á°¨
2: ´ÝÈù idle
3: ¿­¸° idle
*/

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

class CLaserReward : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CLaserReward(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLaserReward(const CLaserReward& rhs);
public:
	~CLaserReward();

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
	shared_ptr<CShader> m_pNonAnimShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CModel> m_pRewardModelCom = { nullptr };

	shared_ptr<CCollider> m_pColliderCom = { nullptr };


	shared_ptr<CPhysXCollider> m_pPhysxCollider = nullptr;
	_int m_iPrevBeatCnt = { 0 };
	const _float4x4* m_pBoneMat = nullptr;
	_bool m_bExhausted = false;
	_bool m_isTalkOver = { true };
	_bool m_bTalk = { false };
	_bool m_bReverb = { false };
	//elevater
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CLaserReward> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END