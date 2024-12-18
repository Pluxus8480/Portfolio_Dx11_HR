#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Effect_Union.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CKale_BArm : public Engine::CPartObject
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
	CKale_BArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_BArm(const CKale_BArm& rhs);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Collision_On() { m_bCollision = true; }

	void Set_AnimSpeed(_float fAnimSpeed) { m_fAnimSpeed = fAnimSpeed; }
	_bool Change_Animation(string strAnimName);
	_bool IsBroken() const { return m_fHp <= 0.f; }

	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

	void BarrierControl(_float4x4 matColliderWorld);
	void LaserControl(_float4x4 matColliderWorld);

	void BarrierOn() { m_bBarrierOrder = true; }
	void BarrierOff();
	void LaserOn() { m_bLaserOrder = true; }

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CCollider> m_pColliderCom = { nullptr };

	vector<string> m_vecAnimList;

private:
	_bool m_bBarrierOrder = false;
	_bool m_bLaserOrder = false;
	_int  m_iDir = 1;
private:
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float m_fAnimSpeed = 1.f;
	
	_int m_iRenderCount = 0;

	const _float4x4* m_pColliderBone = nullptr;
	_bool m_bAtk = false;
	_bool m_bCollision = false;

	_float m_fHp = 40.f;
	_float m_fHitRed = 0.f;

	shared_ptr<CEffect_Union> m_BarrierUnion = {};
	shared_ptr<class CTracking_Laser> m_Laser = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CKale_BArm> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CKale_BArm();
};


END