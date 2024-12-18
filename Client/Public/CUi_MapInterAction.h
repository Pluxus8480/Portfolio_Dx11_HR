#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "Transform.h"


BEGIN(Client)

class CUi_MapInterAction : public CUi_2D
{
public:
	static wstring ObjID;
	enum class WorkTexture { Aware, B_Key, Wire, Left, Right, E_Key, Macaron, End };

public:
	using Desc = struct CUi_MapInterActionInfo
	{
		shared_ptr<CTransform> OwnerTransform;
		_float3 FixPosition = { 0, 0, 0 };
		_float2 RelativeDistance = { 0, 0 };
		_float fAwareDistance = { 0 };
		_float fWorkDistance = { 0 };
		_float2 Size = { 70, 70 };
		_float fInactiveDistance = { 0 };
		WorkTexture iWorkTextureIndex = { WorkTexture::End };
	};


protected:
	CUi_MapInterAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_MapInterAction(const CUi_MapInterAction& rhs);


public:
	virtual ~CUi_MapInterAction();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	void Phase_Active(_bool PhaseActive);

private:
	void Check_Target();
	void Initialize_Arg(Desc* Arg);
	void Initialize_Component();
	//void Maybe_PlayerUse(_bool PlayerUseActive = false);



protected:
	shared_ptr<CTransform> m_pTargetTransform;
	_float2 m_RelativeDistance = { 0, 0 };
	_float m_fAwareDistance = { 0 };
	_float m_fWorkDistance = { 0 };
	_float m_fInactiveDistance = { 0 };
	_float3 m_FixPos = {};
	WorkTexture m_iWorkTextureIndex = { WorkTexture::End };
	_bool m_bOwnActive = { true };
	//_bool m_bPlayerUseActive = { true };
	_bool m_bPhaseActive = { true };


public:
	static shared_ptr<CUi_MapInterAction> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
