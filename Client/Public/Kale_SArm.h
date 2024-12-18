#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CKale_SArm : public Engine::CPartObject
{
public:
	static wstring ObjID;
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_bool bRight;
	};

private:
	CKale_SArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_SArm(const CKale_SArm& rhs);


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
	void Break() { m_fHp = 0.f; }
	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

	void React_To_Atk();

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CCollider> m_pColliderCom = { nullptr };
	//shared_ptr <CCollider> m_pColliderAtkCom = { nullptr };

	vector<string> m_vecAnimList;

private:
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float m_fAnimSpeed = 1.f;
	const _float4x4* m_pColliderBone = nullptr;
	_bool m_bAtk = false;
	_bool m_bCollision = false;

	_int m_iRenderCount = 0;

	_float m_fHp = 40.f;
	_bool m_bRight = false;
	
	_float m_fHitRed = false;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CKale_SArm> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CKale_SArm();
};


END