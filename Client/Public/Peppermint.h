#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CPeppermint : public CGameObject
{
public:
	static wstring ObjID;

private:
	CPeppermint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPeppermint(const CPeppermint& rhs);
public:
	virtual ~CPeppermint();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Play_Animation(string strAnimName, _bool bLoop);
public:
	void Render_On(_bool bToggle = true)
	{
		m_bRender = bToggle;
	}

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CPeppermint> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

public:
	void Assist_Attack(_vector vPos);
	shared_ptr<CGameObject> Auto_LockOn_Target(list<shared_ptr<CGameObject>> vecTargetList);
	void SP_Attack(_vector vPos);
private:

	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);

	void Turn(_float fTimeDelta);

	void Shoot(_uint iGunIdx);
private:
	vector<string> m_vecAnimList;
	_bool m_bRender = false;
	_bool m_bAttacking = false;

	_float m_fLookDiffDegree = 0.f;
	_float m_fTurnTimer = 0.1f;
	_bool m_bTrunRight = false;
	_bool m_bTurning = false;

	_int m_iBullet = 0;
	_int m_iMaxBullet = 0;

	vector <shared_ptr< CPartObject>> m_pWeapon;

	shared_ptr<CGameObject> m_pLock_On_Target = nullptr;
private:	// For Effect
	_float4x4 m_matCurrentPos = {};
};

END