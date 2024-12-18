#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

// 0 == a
// 1 == d
// 2 == w
// 3 == s

class CUi_CNMNRazorPart : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_CNMNRazorPartInfo
	{
		_float2 Pos = { 0, 0 };
		_float BeatDuration = { 2 };
		_uint KeyInput = { 0 };
	};


protected:
	CUi_CNMNRazorPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_CNMNRazorPart(const CUi_2D& rhs);


public:
	virtual ~CUi_CNMNRazorPart();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	void Set_StartJudge() { m_bJudgeStart = true; }

private:
	virtual void Dying() override;
	virtual void Set_Active(_bool Active) override;
	void Initialize_Icon(_uint KeyNum, _float2 StartPos);
	void Initialize_Bar(_float2 StartPos);
	_bool Cal_CommandIcon(_uint PressKeyNum);
	void Fail();
	void Create_AfterIcon();
	void KeyPress(_float fTimeDelta);

	void Progress_KeyDown();
	void Progress_KeyPressing(_float fTimeDelta);
	void Progress_KeyUp();

private:
	vector<shared_ptr<class CUi_Default>> m_pInputIconVec;
	shared_ptr<class CUi_CommandIcon> m_pCommandIcon;
	vector<shared_ptr<class CUi_DefaultBar>> m_pBarVec;
	_float m_fBeatDuration = { 0 };
	_float m_fTimeDuration = { 0 };
	_float m_fWholeTime = { 0 };
	_uint m_iKeyInput = { 0 };
	_bool m_bJudgeStart = { false };


public:
	static shared_ptr<CUi_CNMNRazorPart> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
