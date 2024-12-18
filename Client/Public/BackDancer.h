#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Union.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPartObject;
END


BEGIN(Client)

class CBackDancer : public CGameObject
{
public:
	static wstring ObjID;

public:
	struct BackDancer_Info
	{
		_float4 fInitPos = {};
		_float4x4 fInitMat = {};
		_bool	bSpotLight = false;
		_int	iMotion = 0;
		list<list<pair<string, _float>>> listDanceEvent;
		list <_float> list_Mimosa_Dance_Delay;
	};
private:
	CBackDancer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackDancer(const CBackDancer& rhs);
public:
	virtual ~CBackDancer();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void  Attack_Start();
	void  Start_Dance();
	_bool  Get_Attack_End() { return m_bAttack_End; }
	

	void	Dance_Event_Animation(_float fTimeDelta);
	void	Dance_Event_Animation_End(_float fTimeDelta);
	void	Dance(string strDance, _float danceSpeed);
public:
	void Render_On(_bool bToggle = true)
	{
		m_bRender = bToggle;
	}

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CBackDancer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual void Dying()override;
private:

	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);

	void Turn(_float fTimeDelta);
	
private:
	list<list<pair<string, _float>>> m_listDanceEvent;
	_bool m_bDance_Rest = false;
	_bool m_bDance_Ready = false;
	_bool m_bDanceMode = false;
	_float m_fDance_PlaySpeed = 1.f;
	_float m_fDanceBattle_Next_Timer = 0.f;
	list <_float> m_list_Mimosa_Dance_Delay;
private:
	vector<string> m_vecAnimList;
	_bool m_bRender = true;
	_bool m_bAttacking = false;

	_float m_fLookDiffDegree = 0.f;
	_float m_fTurnTimer = 0.1f;
	_bool m_bTrunRight = false;
	_bool m_bTurning = false;

	_int m_iBullet = 0;
	_int m_iMaxBullet = 0;

	vector <shared_ptr< CPartObject>> m_pWeapon;

	shared_ptr<CGameObject> m_pLock_On_Target = nullptr;


	_float m_fSpotlight_Timer = 0.f;
	_bool m_bAttack_End = false;
private:	// For Effect
	_float4x4 m_matCurrentPos = {};

	shared_ptr<CEffect_Union> m_SpotLight = {};
};

END