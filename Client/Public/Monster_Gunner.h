#pragma once

#include "MonsterBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CCharacterController;
END


BEGIN(Client)

class CMonster_Gunner : public CMonsterBase
{
public:
	static wstring ObjID;

	friend class CGunnerState_Intro;
	friend class CGunnerState_Idle;
	friend class CGunnerState_Move;
	friend class CGunnerState_Attack;
	friend class CGunnerState_Down;
	friend class CGunnerState_Damaged;
	friend class CGunnerState_Dead;

private:
	CMonster_Gunner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Gunner(const CMonster_Gunner& rhs);
public:
	virtual ~CMonster_Gunner();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

public:
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel>	m_pModelCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };

private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };
public:
	void Make_PickUp_Object(_int iNumber, CPickUpItem::PICKUP pickupType);
private:
	HRESULT Ready_Components();
	void	PrepareCallbacks();

public:
	virtual void SetPos_Hard(_fvector vPos) override;
	virtual void MoveToPos(_fvector vTargetPos) override;

public:
	void MonsterDead();

public:
	_bool IsAnimFinished();
	void SetAnim(const string& strAnimName, _bool isLoop = false);
	void SetAnimHard(const string& strAnimName, _bool isLoop = false);

public:
	void GravityOn();
	void GravityOff();

public:
	void AirStateON();
	void AirStateOFF();

public:
	void MoveStraight(_float fSpeed);
	void MoveUp(_float fSpeed);
	void Set_Position(_matrix mMat);

public:
	void CCT_ON();
	void CCT_OFF();

private:
	void SelectBeat();
private:
	_float m_fBeatSpeed = 1.f;
	_bool m_bFinishBlow = false;
public:
	void PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard);
	void PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard);

public:
	virtual _bool IsNeedToMove() override;

private:	// For state handling
	class CGunnerState* m_pCurState = { nullptr };

	_bool m_isAirState = false;

	const _float m_fDistance_Far_C = 10.f;
	const _float m_fDistance_Near_C = 4.f;

private:
	const _float4x4* m_pMat_Spine = { nullptr };

private:
	_float m_fDistance = 0.f;
	_float4 m_vfDirToPlayer;
	_float m_fAngleDegree = 0.f;

private:
	_bool m_isAttackOn = false;

private:
	const _float m_fTimer_Attack_C = 5.f;
	_float m_fTimer_Attack = m_fTimer_Attack_C;

private:
	const _float m_fMaxHp = 70.f;

private:
	_bool m_isCCTWork = false;

private:	// For Effect
	_float4x4 m_matCurrentPos = {};

public:
	static shared_ptr<CMonster_Gunner> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END