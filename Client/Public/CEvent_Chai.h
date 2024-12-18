#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPartObject;
class CCharacterController;


END

BEGIN(Client)
class CUi_CommandIcon;
class CEvent_Chai : public CGameObject
{
public:
	static wstring ObjID;

public:
	enum class AnimType {
		Parryingone, Parryingtwo, Heat, Idle, Idletomove, Move, Movetoparryready,
		Evade, Evade_Fail, Nonparryheat, Catchrun, MoreRun, Catch, CatchMiss,End
	};


private:
	CEvent_Chai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEvent_Chai(const CEvent_Chai& rhs);


public:
	virtual ~CEvent_Chai();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow();
	virtual void TakeDamage(const DamageInfo& damageInfo) override;


	void Set_DamagedAnim(_bool CanParryHeat);
	void Add_Hp(_float Hp);
	void Set_Player_Position(_vector vPosition);
	void Set_Player_Position_Immediate(_vector vPosition);
	_bool IsCatch() const { return m_bCatch; }

public:
	void Set_Anim(AnimType Type, _bool Loop = false);
	void Set_DelayTime();
	void Set_Middle(_float3 Middle) { m_Middle = Middle; }
	void Set_Angle(_float Angle) { m_fAngle = Angle; }
	void Set_Radius(_float Radius) { m_fRadius = Radius; }
	void Lerp_Pos(_float fTimeDelta);
	void Set_Lerp(_bool bLerp) { m_bLerp = bLerp; }
	CEvent_Chai::AnimType Get_CurAnim();

	void Start_AttackSequence(_bool bShort, _int iAttackReadyStartBeat, const vector<_float>& vecTimer, const vector<_bool>& vecParry);
	_float Get_ReactTimer() const { return m_fReactTimer; }
	void Set_ReactTimer(_float fTimer) { m_fReactTimer = fTimer; }

	shared_ptr<CModel> Get_Model() { return m_pModelCom; }

private:
	void Progress_Anim(_float fTimeDelta);
	void Return_toIdle();
	void Key_Progress();

		

private:
	virtual void Dying() override;
	void ReadyComponent();


private:
	shared_ptr<CPartObject> m_pWeapon;
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel>	m_pModelCom = { nullptr };
	_float3 m_Middle = {};
	_float m_fRadius = {};
	_float m_fAngle = {};
	_bool m_bLerp = { true };
	_bool m_bDamaged = { false };
	_float m_fMaxHp = { 100 };
	_float m_fHp = m_fMaxHp;
	_bool m_bFollow = false;
	_float4 m_vTargetPosition{ 0.f,0.f,0.f,0.f };
	_float m_fCoolDown = 0.f;
	_bool m_bShortAttack = false;
	vector<_float> m_vecBattleTiming;
	vector<_bool> m_vecParry;
	_int m_iAttackReadyStartBeat = 0;
	_bool m_bValid = true;
	_float m_fReactTimer = 0.f;
	_bool m_bCatch = false;
	_bool m_bUI = false;
	shared_ptr<CUi_CommandIcon> m_pCommandIcon;
	_bool m_bIsInGame = false;

public:
	static shared_ptr<CEvent_Chai> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END