#pragma once

#include "Client_Defines.h"
#include "CUi_Default.h"

BEGIN(Client)

class CUi_DefaultLife : public CUi_Default
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_DefaultLifeInfo : public CUi_2DInfo
	{
		_float fLifeTime = { 0 };
		_float fScaleTime = { 0 };
		_float fAlphaTime = { 0 };
		_float fScalingSpeed = { 0 };
		_float fAlphaSpeed = { 0 };
	};


protected:
	CUi_DefaultLife(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_DefaultLife(const CUi_DefaultLife& rhs);


public:
	virtual ~CUi_DefaultLife();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

private:
	void Initialize_SpecialComboAttackResource();


private:
	void Cal_LifeTime(_float fTimeDelta);
	void Adjust_AlphaTime(_float fTimeDelta);
	void Adjust_ScaleTime(_float fTimeDelta);


private:
	_float m_fLifeTime = { 0 };
	_float m_fScaleTime = { 0 };
	_float m_fAlphaTime = { 0 };
	_float m_fScalingSpeed = { 0 };
	_float m_fAlphaSpeed = { 0 };


public:
	static shared_ptr<CUi_DefaultLife> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
