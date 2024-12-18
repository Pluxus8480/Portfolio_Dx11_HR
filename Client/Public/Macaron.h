#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CMacaron : public CGameObject
{
public:
	static wstring ObjID;

private:
	CMacaron(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMacaron(const CMacaron& rhs);
public:
	virtual ~CMacaron();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
public:
	void Render_On(_bool bToggle = true)
	{
		m_bRender = bToggle;
	}

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:
	shared_ptr<CGameObject> Auto_LockOn_Target(list<shared_ptr<CGameObject>> vecTargetList);
	HRESULT Ready_Components();

public:
	static shared_ptr<CMacaron> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

public:
	void Assist_Attack(_vector vPos);
	void Lift_Attack(_vector vPos , _fvector vLookAt);

	_int Get_Hit_Count() const { return m_iHit_Count; }
	_bool Get_Attacking () const { return m_bAttacking; }

	void Play_Animation(string strAnimName, _bool bLoop);
private:

	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);

	void Turn(_float fTimeDelta);
private:
	vector<string> m_vecAnimList;
	_bool m_bRender = false;
	_bool m_bAttacking = false;

	_float m_fLookDiffDegree = 0.f;
	_float m_fTurnTimer = 0.1f;
	_bool m_bTrunRight = false;
	_bool m_bTurning = false;

	_int m_iHit_Count = 0;

	shared_ptr<CGameObject> m_pLock_On_Target = nullptr;
private:	// For Effect
	_float4x4 m_matCurrentPos = {};
};

END