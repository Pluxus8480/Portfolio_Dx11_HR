#pragma once

#include "MonsterBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CCharacterController;
END


BEGIN(Client)

class CRekka_Pipe_R;
class CRekka_Pipe_L;
class CRekka_Long_Pipe;
class CRekka_Gauntlet;
class CRekka_Pipe_Door;
class CUi_RekkaSystem;
class CMonster_Rekka : public CMonsterBase
{
public:
	static wstring ObjID;

	friend class CRekkaState_Intro;

	friend class CRekkaState_Idle;
	friend class CRekkaState_Move;
	friend class CRekkaState_Rotate;
	friend class CRekkaState_Attack;
	friend class CRekkaState_Groggy;

	friend class CRekkaState_Elect;

	friend class CRekkaState_Gimmick;

	friend class CRekkaState_Pipe;
	friend class CRekkaState_PipeAtt_Long;
	friend class CRekkaState_PipeAtt_Short;

	friend class CRekkaState_Event;

	friend class CRekkaState_Pase_2;
	friend class CRekkaState_Pase_3;
	friend class CRekkaState_Pase_4;
	friend class CRekkaState_Pase_Dead;


private:
	CMonster_Rekka(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Rekka(const CMonster_Rekka& rhs);
public:
	virtual ~CMonster_Rekka();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

public:
	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr<CCharacterController>	m_pCCT = { nullptr };

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };
	_float	m_fAttackScale = 1.f;
	enum class EAttackBone { RHand, LHand, Body, Foot };
	EAttackBone m_curAttackBone = EAttackBone::Foot;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_GimmickObjects();

	void	PrepareCallbacks();

public:
	_bool IsAnimFinished();
	void SetAnim(const string& strAnimName, _bool isLoop = false);
	void SetAnimHard(const string& strAnimName, _bool isLoop = false);

private:
	_bool GravityCheck();

	void CCT_ON();
	void CCT_Off();

public:
	void GravityOff();
	void GravityOn();

public:
	void AnimationOff();
	void AnimationOn(_float fFrameBegin);
private:
	_bool m_isAnimPlay = true; 

private:
	void SelectBeat();
private:
	_float m_fBeatSpeed = 1.f;

private:
	void MoveStraight(_float fSpeed);
	void MoveRight(_float fSpeed);
	void MoveUp(_float fSpeed);

	void SetPosition_Hard(_vector vTargetPos);

public:
	void PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard);
	void PlayUnionEffect(string strEffect, _float fmultiplier, _bool isBillBoard);

private:
	const _float4x4* m_pMat_Spine = { nullptr };
	const _float4x4* m_pMat_RightHand = { nullptr };
	const _float4x4* m_pMat_LeftHand = { nullptr };
	const _float4x4* m_pMat_Attach_R = { nullptr };
	const _float4x4* m_pMat_Attach_L = { nullptr };
	const _float4x4* m_pMat_ForeArm_R = { nullptr };
	const _float4x4* m_pMat_ForeArm_L = { nullptr };
	const _float4x4* m_pMat_Gauntlet = { nullptr };

	const _float4x4* m_pMat_Camera = { nullptr };

public:
	const _float4x4* m_pMat_Attach_Chai = { nullptr };

private:
	class CRekkaState* m_pCurState = { nullptr };

	_bool m_isAttackOn = false;
	_bool m_isGrapOn = false;

	_bool m_isCCT_Disable = false;

private:
	_float m_fDistance = 0.f;
	_float4 m_vfDirToPlayer;
	_float m_fAngleDegree = 0.f;

private:
	const _float m_fTimer_Attack_C = 4.f;
	_float m_fTimer_Attack = m_fTimer_Attack_C;

private:
	_float m_fTimerGroggy = 0.f;
private:
	_float m_fTimerElect = 0.f;

public:
	_bool Get_Buff() const { return m_isElectric; };
private:
	_bool m_isElectric = false;

public:
	_float Get_MaxHp() const { return m_fMaxHp_C; };
	_float Get_Hp() const { return m_fCurHp; };
private:
	const _float m_fMaxHp_C = 400.f;

private:
	_uint m_iCurPase = 1;

private:
	_uint m_iAttack_Seq = 0;
	_float m_fAttack_Damage = 10.f;

private: // For pipe actions
	shared_ptr<CRekka_Pipe_L> m_pPipeL = nullptr;
	shared_ptr<CRekka_Pipe_R> m_pPipeR = nullptr;
	shared_ptr<CRekka_Long_Pipe> m_pPipe_Long = nullptr;

private:
	shared_ptr<CRekka_Pipe_Door> m_pDoorL = nullptr;
	shared_ptr<CRekka_Pipe_Door> m_pDoorR = nullptr;

private:
	shared_ptr<CRekka_Gauntlet> m_pGauntlet = nullptr;


private:	// Don't touch it
	_float4x4 m_matCurrentPos;
	_float4x4 m_matRekka_Rasengan;
	_float4x4 m_matRekka_Groggy_R;
	_float4x4 m_matRekka_Groggy_L;

public:
	static shared_ptr<CMonster_Rekka> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END