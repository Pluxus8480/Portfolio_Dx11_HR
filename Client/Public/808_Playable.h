#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CCharacterController;
class CCollider;
END


BEGIN(Client)
class C808_Flare_Effect;
class CFlare_Effect;
class CJustMark_Effect;
class CCamera_Player;
class CUi_CommandIcon;

class C808_Playable : public CGameObject
{
public:
	static wstring ObjID;
	enum STATE
	{
		IDLE,
		RUN,
		JUMP,
		AIR_JUMP,
		FALL,
		LAND,
		ATK,
		ATK_FAIL,
		ATK_SUCCESS,
		STATE_END
	};

private:
	C808_Playable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C808_Playable(const C808_Playable& rhs);
public:
	virtual ~C808_Playable();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;
	void Set_Position(const _float4& vPosition);

	void Take_Control();
	void End_Control();

	void Render_On(_bool bToggle = true) { m_bRender = bToggle; }

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCharacterController> m_pCCT = { nullptr };
	shared_ptr<CCamera_Player> m_pPlayer_Camera = nullptr;
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Event = { nullptr };
	const _float4x4* m_pBone_Spine = nullptr;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<C808_Playable> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

private:
	//void To_Idle();
	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);

private:
	_bool m_bIsInGame = true;
	vector<string> m_vecAnimList;
	_bool m_bRender = true;
	_bool m_bInput = false;
	_bool m_bTakeControl = true;

	_float m_fDistance = 0.f;
	_float m_fSpeed = 3.f;
	_bool	m_bOnAir = false;
	_int	m_iJumpCount = 2;
	
	_bool	m_bEvent = false;

	_uint m_iPrevBeat = 0;
	_bool m_bUI = false;
	shared_ptr< CUi_CommandIcon> m_pCommandIcon;

private:	// For Effect
	_float4x4 m_matCurrentPos = {};
};

END