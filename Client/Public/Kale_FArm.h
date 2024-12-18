#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)
class CEffect_Union;

class CKale_FArm : public Engine::CPartObject
{
public:
	static wstring ObjID;
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint* pState;
	};

private:
	CKale_FArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_FArm(const CKale_FArm& rhs);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Collision_On() { m_bCollision = true; }
	void Atk_On() { m_bAtk = true; }

	void Set_AnimSpeed(_float fAnimSpeed) { m_fAnimSpeed = fAnimSpeed; }
	_bool Change_Animation(string strAnimName);
	_bool IsBroken() const { return m_fHp <= 0.f; }

	void Throw_Flame(_float fPlayTime);
	void Hold_Flame(_float fPlayTime);
	void Quite_Flame();

	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CEffect_Union> m_FlameHoldUnion = {};

	vector<string> m_vecAnimList;

	//Flame_Throw_10Sec

private:
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float m_fAnimSpeed = 1.f;

	const _float4x4* m_pColliderBone = nullptr;
	_float4x4 m_MatCollider = Identity;
	_bool m_bAtk = false;
	_bool m_bCollision = false;

	_float m_fHitRed = 0.f;
	_int m_iRenderCount = 0;
	_float m_fHp = 40.f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CKale_FArm> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CKale_FArm();
};


END