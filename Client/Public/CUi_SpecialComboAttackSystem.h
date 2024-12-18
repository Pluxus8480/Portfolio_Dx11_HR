#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"


BEGIN(Client)
// 0 == a
// 1 == d
// 2 == w
// 3 == s
class CUi_SpecialComboAttackSystem : public CUi_2D
{
public:
	static wstring ObjID;


public:
	enum class SPCombo_Type { SPAttack, Mimosa, Kale_Finish, End };
public:
	enum class State { Start, Process, End };

public:
	using Desc = struct SPComboAttack
	{
		SPCombo_Type eSPCombo_Type = SPCombo_Type::End;
	};

protected:
	CUi_SpecialComboAttackSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_SpecialComboAttackSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_SpecialComboAttackSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


public:
	_float Get_ChaiTurnBeat() const { return m_fChaiTurnBeat; }
	_float Get_SuccessPercent() const { return m_fPreviousSuccessPercent; }


private:
	void Progress_State(_float fTimeDelta);
	void Progress_Start(_float fTimeDelta);
	void Progress_Process(_float fTimeDelta);
	void Set_State(State eState);
	void Set_Process();
	void Set_Start();


private:
	void Initialize_Back();
	void Initialize_ComboDot();
	void Create_Rhythm();
	_bool Cal_Judge(_uint iPressKey);
	virtual void Dying() override;
	void KeyPress(_float fTimeDelta);
	void Dance_Battle_KeyPress(_float fTimeDelta);
	void Set_WholeCount(_uint Count) { m_iWholeCount = Count; }
	void Initialize_Count(_uint Count);
	void Set_RhythmList();
	void Set_KeyboardInputStartPos();
	void Create_KeyboardInput();
	void Set_JudgeBeat();
	void Create_Judge(_bool bSuccess, _float2 Pos);


private:
	shared_ptr<class CUi_Default> m_pBack;
	vector<shared_ptr<class CUi_Default>> m_pComboDotVec;
	vector<shared_ptr<class CUi_Default>> m_InputKeyVec;
	list<pair<_float, _int>> m_RhythmList;
	list<list<pair<_float, _int>>> m_RhythmCollectionList;
	list<_float> m_FirstBeatList;
	vector<_float> m_JudgeBeatVec;
	State m_eState = { State::Start };
	const _float m_fJudgePointX = { -300 };
	const _float m_fDotInterval = { 100 };
	_float2 m_DotStartPos = { 0, -230 };
	_float m_fRealBeatJudgeInterval = { 0 };
	_float m_fStartBeat = { 0 };
	_uint m_iSuccessCount = { 0 };
	_uint m_iWholeCount = { 0 };
	_float m_fPreviousSuccessPercent = { 0 };
	_float m_fChaiTurnBeat = { 0 };
	_float m_fNextStartTime = { 0 };
	SPCombo_Type m_eSPCombo_Type = { SPCombo_Type::End };


public:
	static shared_ptr<CUi_SpecialComboAttackSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
