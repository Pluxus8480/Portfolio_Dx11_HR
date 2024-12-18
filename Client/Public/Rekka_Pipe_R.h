#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CRekka_Pipe_R : public CPartObject
{
public:
	static wstring ObjID;

public:
	struct DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint iLevelIndex;
	};

private:
	CRekka_Pipe_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Pipe_R(const CRekka_Pipe_R& rhs);
public:
	virtual ~CRekka_Pipe_R();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller) override;

private:
	HRESULT Ready_Components();
private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CCollider> m_pCollider_Attack = { nullptr };

public:
	void AttachOn();
	void PrepareAttack();
	void AttackSpecial();
private:
	enum class EPipeState {
		Default, Attach, PreAttack, Attack, FinalAttack, Disappear
	};
	EPipeState m_curState = EPipeState::Default;

private:
	const _float4 m_vDefaultPos = { 1181.41f, 487.38f, 90.4f, 1.f };
	const _float4x4* m_pBoneMatrix = { nullptr };

private:
	_float4		m_vDirLauch;
	_int		m_iCurBeatCnt;
	_int		m_iTotalBeatCnt;

	_float		m_fLaunchSpeed = 40.f;

private:
	_bool		m_isAttackOn = false;

private:
	_float4x4	m_matEffect;


public:
	static shared_ptr<CRekka_Pipe_R> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END