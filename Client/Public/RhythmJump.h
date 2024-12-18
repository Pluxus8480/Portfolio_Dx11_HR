#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CRhythmJump : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CRhythmJump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRhythmJump(const CRhythmJump& rhs);
public:
	~CRhythmJump();

public:
	virtual void Execute(_bool bExecute = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

	virtual void Edit_Attribute() override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pBaseModelCom = { nullptr };
	shared_ptr<CModel> m_pDisplayModelCom = { nullptr };
	shared_ptr<CModel> m_pJointModelCom = { nullptr };
	shared_ptr<CModel> m_pMovingModelCom = { nullptr };
	_float4x4 m_DisplayOffset{1.f,0.f,0.f,0.f,
							  0.f,1.f,0.f,0.f,
							  0.f,0.f,1.f,0.f,
							  -2.2f,2.7f,0.f,1.f};
	
	
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

	_float3 m_vModelCenter = {};
	_float3 m_vModelSize = {};

	_bool m_bCulled = false;
	_int m_iPrevBeatCnt = { 0 };
	_float m_fBeat = {};

	/* Moving Floor */
	_bool m_bDown = { true };
	_bool m_bRest = { false };
	_bool m_bCollision = { false };
	_float4x4 m_MovingFloorMatrix = { Identity };
	_float4x4 m_MovingFloorWorldMat = { Identity };

	/* DMG Display */
	_bool m_bInit = { false };
	_float m_fSecPerFrame = {};
	_float m_fFrameTime = {};
	_int m_iCurFrameIndex = {};
	_int m_iMaxFrame = { 8 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CRhythmJump> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END