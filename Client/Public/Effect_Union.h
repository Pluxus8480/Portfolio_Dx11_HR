#pragma once

#include "Client_Defines.h"
#include "UnionEffectData.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect_Union : public CGameObject
{
public:
	static wstring ObjID;

public:
	enum EFFECT_UNION_STATE { EFFECT_UNION_SLEEP, EFFECT_UNION_START, EFFECT_UNION_PLAY, EFFECT_UNION_STOP, EFFECT_UNION_END };

	struct EffectUnionDesc
	{
		string			m_strEffectUnionKey = "";
	};

protected:
	CEffect_Union(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Union(const CEffect_Union& rhs);

public:
	virtual ~CEffect_Union();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	void State_Engine(_float fTimeDelta);
	virtual HRESULT Render();

private:
	void UnionMove(_float fTimeDelta);

public:
	void UnionEffectSleep();
	void UnionEffectPlay(_float fTimeDelta);
	void UnionEffectResume(_float fTimeDelta);
	void UnionEffectStop();

	void UnionEffectStart();
	void UnionEffectEnd();

	void UnionEffectStartEvent();
	void UnionEffectEndEvent();

	void PlayEffect(_float fMultiplier, _float4x4* matWorld);
	void PlayEffect(_float fMultiplier, _float4x4 matWorld);
	void PlayEffect(_float fMultiplier, _float4x4 matWorld, _float _fMoveSpeed, _float3 _vMoveDir);

	shared_ptr<CEffect_Union> PlayEffectReturn(_float fMultiplier, _float4x4 matWorld);
	shared_ptr<CEffect_Union> PlayEffectReturn(_float fMultiplier, _float4x4* matWorld);
	shared_ptr<CEffect_Union> PlayEffectReturn(_float fMultiplier, _float4x4 matWorld, _float _fMoveSpeed, _float3 _vMoveDir);

public:
	void UnionEffectCreate();
	void Make_UnionEffects(UNION_EFFECT_DATA _sUnionEffectDat);

	void SetUnionTimeMultiplier(_float _fMultiplier) { m_fMultiplier = _fMultiplier; }
	_float GetUnionTimeMultiplier() { return m_fMultiplier; }

	_float GetUnionTimePer() { return m_fEffectUnionTime / m_fEffectUnionMaxTime; }
	_float GetUnionTimePerReverse() { return 1.f - m_fEffectUnionTime / m_fEffectUnionMaxTime; }

	string GetEffectUnionKey() { return m_strEffectUnionKey; }
	EFFECT_UNION_STATE GetEffectUnionState() { return m_eEffectUnionState; }

	void SetBillboard(_bool _bBillboard) { m_bBillboard = _bBillboard; }
	_bool GetBillboard() { return m_bBillboard; }

	void SetMoveEffect(_bool _bMoveEffect) { m_bMoveEffect = _bMoveEffect; }
	_bool GetMoveEffect() { return m_bMoveEffect; }

	void SetMoveDir(_float3 _vMoveDir) { m_vMoveDir = _vMoveDir; }
	_float3 GetMoveDir() { return m_vMoveDir; }

	void SetMoveSpeed(_float _fMoveSpeed) { m_fMoveSpeed = _fMoveSpeed; }
	_float GetMoveSpeed() { return m_fMoveSpeed; }

	void SetColorAll(const _float4& vColor);
	void SetColorPalette(const _float4& vColor);
	void SetGlowOff();

private:
	vector<shared_ptr<class CEffect_Base>>	m_vecEffects;

private:
	EFFECT_UNION_STATE		m_eEffectUnionState = EFFECT_UNION_SLEEP;

	_float					m_fEffectUnionTime = 0.f;
	_float					m_fEffectUnionMaxTime = 1.f;
	_float					m_fMultiplier = 1.f;

	string					m_strEffectUnionKey = "";

	_float4x4* m_pMatWorldPtr = nullptr;
	_float4x4				m_matFixWorld = INIT_MATRIX;

	_bool					m_bBillboard = false;

	_bool					m_bMoveEffect = false;

	_float3					m_vMoveDir = { 0.f, 0.f, 0.f };
	_float 					m_fMoveSpeed = 0.f;

public:
	static shared_ptr<CEffect_Union> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END