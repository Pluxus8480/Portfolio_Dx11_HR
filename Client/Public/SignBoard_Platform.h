#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)

class CSignBoard_Platform : public CGimmickBase
{
public:
	static wstring ObjID;
private:
	enum FALL_SEQUENCE { SHAKE, FALL, REBOUND, SQ_END };

private:
	CSignBoard_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSignBoard_Platform(const CSignBoard_Platform& rhs);
public:
	~CSignBoard_Platform();

public:
	virtual void Set_Dead(bool bDead = true) override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

	shared_ptr<CGameObject> m_pSignBoard = { nullptr };
	_float4x4 m_fBoardMat = {};

	_float3 m_vModelCenter = {};
	
	FALL_SEQUENCE m_eFallSQ = { SHAKE };
	_float m_fSpeedPerSec = {};
	/* Shake */
	_float3 m_vOriginPos = {};
	_float m_fShakeMinY = {};
	_bool m_bLastShake = { false };
	_bool m_bDown = { true };
	/* Fall */
	_float m_fMinPosY = {};
	/* Rebound */
	_float m_fRadPerSec = { XMConvertToRadians(10.f) };
	_float m_fCurRotation = {};

	_float m_fDelayTime = {};

	_bool m_bSoundPlayed = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	
	void Move(_float fTimeDelta);

public:
	static shared_ptr<CSignBoard_Platform> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END