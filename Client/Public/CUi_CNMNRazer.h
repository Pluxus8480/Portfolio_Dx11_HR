#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

// 0 == a
// 1 == d
// 2 == w
// 3 == s

class CUi_CNMNRazer : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_CNMNRazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_CNMNRazer(const CUi_2D& rhs);


public:
	virtual ~CUi_CNMNRazer();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	void Set_Special(_bool Special = true) { m_bSpecialJudge = Special; }
	//void Fail();
	//void Success();

private:
	void KeyPress(_uint Keypress, _float fTimeDelta);
	void Create_Icon(_float2 Pos, _uint CorrectBeat, _uint KeyNum);
	void Create_SpecialIcon(_float2 Pos, _uint KeyInput, _float BeatDuration);
	void Check_Fail();
	void Initialize_FirstPhaseCallback();
	void Initialize_SecondPhaseCallback();
	void Initialize_ThirdPhaseCallback();
	void Initialize_FourthPhaseCallback();
	void Initialize_FifthPhaseCallback();

private:
	vector<shared_ptr<class CUi_CommandIcon>> m_pIconVec;
	vector<shared_ptr<class CUi_Default>> m_pDefaultVec;
	shared_ptr<class CUi_CNMNRazorPart> m_pPart;
	_uint m_iPhase = { 0 };
	_float m_fTime = { 0 };
	_int m_iCreateCount = { 0 };
	_float m_fPercent = { 0 };
	_bool m_bSpecialJudge = { false };
	_bool m_bFail = { false };
	_float m_fTimeInterval = { 0 };

public:
	static shared_ptr<CUi_CNMNRazer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
