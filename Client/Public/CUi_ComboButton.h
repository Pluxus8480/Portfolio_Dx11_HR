#pragma once

#include "Client_Defines.h"
#include "CUi_MapInterAction.h"

BEGIN(Client)

class CUi_ComboButton : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_ComboButtonInfo : public CUi_2DInfo
	{
		shared_ptr<CTransform> OwnerTransform;
		_float2 RelativeDistance = { 0, 0 };
		vector<_bool> LButtonVec = {};
	};

protected:
	CUi_ComboButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_ComboButton(const CUi_ComboButton& rhs);


public:
	virtual ~CUi_ComboButton();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	void Set_Success(_bool Success = true);



private:
	void Initialize_BackGround(_uint iCount);
	void Initialize_Button(vector<_bool> Vec);
	void Initialize_SuccessButton(_uint iCount);
	void Adjust_ComboButtonPos();
	void Adjust_SuccessButton();
	virtual void Set_Active(_bool Active) override;
	virtual void Dying() override;


private:
	shared_ptr<CTransform> m_pTargetTransform;
	_float2 m_RelativeDistance = {};
	shared_ptr<class CUi_Default> m_pBack;
	vector<shared_ptr<class CUi_Default>> m_pButton;
	vector<shared_ptr<class CUi_Default>> m_pSuccessButton;
	vector<_bool> m_SuccessVec;


public:
	static shared_ptr<CUi_ComboButton> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
