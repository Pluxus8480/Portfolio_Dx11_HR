#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CKorsica : public CGameObject
{
public:
	static wstring ObjID;

private:
	CKorsica(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKorsica(const CKorsica& rhs);
public:
	virtual ~CKorsica();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void SP_Attack(_vector vPos);
	void Play_Animation(string strAnimName, _bool bLoop);
private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	shared_ptr<CGameObject> Auto_LockOn_Target(list<shared_ptr<CGameObject>> vecTargetList);
public:
	static shared_ptr<CKorsica> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

public:
	void Assist_Attack(_vector vPos);
	void Render_On(_bool bToggle = true)
	{
		m_bRender = bToggle;
	}

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

	 shared_ptr<CPartObject> m_pWeapon_Right;
	 shared_ptr<CPartObject> m_pWeapon_Left;

	 shared_ptr<CGameObject> m_pLock_On_Target = nullptr;
private:	// For Effect
	_float4x4 m_matCurrentPos = {};
};

END