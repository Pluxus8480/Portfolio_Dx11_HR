#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_StunGaugeSystem : public CUi_2D
{
public:
	static wstring ObjID;

public:
	enum class State { Default, Stun, StuntoDefault, End };
	using Desc = struct CUi_StunGaugeSystemInfo
	{
		shared_ptr<CTransform> pTargetTransform;
		_uint iStar = { 1 };
		_float3 RelativeDistance = {};
		_int* pCurStunGauge = { nullptr };
		_float fMaxStunGauge = { 0 };
	};


protected:
	CUi_StunGaugeSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StunGaugeSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_StunGaugeSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Progress_State(_float fTimeDelta);
	void Set_State(State eState);
	void Initiailize_Body();
	void Initialize_Back();
	void Initialize_Star();
	void Initialize_StarBack();
	void Cal_Percent();
	void Adjust_Pos();
	void Adjust_Scale();
	virtual void Set_Active(_bool Active) override;
	virtual void Dying() override;


private:
	_int* m_pCurStunGauge = { nullptr };
	_float m_fMaxStunGauge = { 0 };
	_float m_fPercent = { 0 };
	shared_ptr<class CUi_3DDefault> m_pStar;
	shared_ptr<class CUi_3DDefault> m_pStarBack;
	shared_ptr<class CUi_3DDefault> m_pBack;
	shared_ptr<CTransform> m_pTargetTransform;
	_float3 m_RelativeDistance = {};
	_float2 m_OriginSize = { 1.5f, 1.5f };
	State m_eState = { State::Default };
	_uint m_iStarBackTextureNum = { 0 };
	_float m_fTest = { 0 };
	

public:
	static shared_ptr<CUi_StunGaugeSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
