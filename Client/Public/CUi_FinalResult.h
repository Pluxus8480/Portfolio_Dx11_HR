#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_FinalResult : public CUi_2D
{
public:
	static wstring ObjID;


public:
	enum class FontType { Corus, Score, JustTiming, ClearTime, End };
	using Desc = struct CUi_FinalResultInfo : public CUi_2DInfo
	{
		FontType eFontType = { FontType::End };
	};

protected:
	CUi_FinalResult(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_FinalResult(const CUi_FinalResult& rhs);


public:
	virtual ~CUi_FinalResult();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void Set_SecondFontString(_uint Score);
	_uint Get_BonusScore() const { return m_iBonusScore; }


private:
	void Appear(_float fTimeDelta);
	void Disappear(_float fTimeDelta);
	void Cal_CorusRank();
	_uint Get_RankNum(string Rank);

private:
	void Initialize_Font();
	void Initialize_Texture();
	void Initialize_BackGround();
	void Initialize_Bonus();
	void Adjust_FontPos();
	void Adjust_BackPos();
	void Create_Rank();
	void Create_RankString();
	void Create_FinalResult();
	void Set_RankTexture(string Rank);
	virtual void Dying() override;


private:
	shared_ptr<class CUi_Default> m_pBack = {};
	shared_ptr<class CUi_Default> m_pBonus = {};
	FontDescInfo m_SecondFontDesc = {};
	FontDescInfo m_BonusFontDesc = {};
	FontDescInfo m_BonusStringFontDesc = {};
	_float m_fScore = { 0 };
	_int m_iBonusScore = { 0 };
	_float m_fAppearTime = { 0 };
	_float m_fDisappearTime = { 0 };
	_float m_fAcc = { 0 };
	_float m_fScaleAcc = { 0 };
	_float m_fSpeedAcc = { 0 };
	_bool m_bBackAdjust = { false };
	FontType m_eFontType = { FontType::End };
	shared_ptr<class CUi_Default> m_pRank;
	shared_ptr<class CUi_Default> m_pRankString;
	shared_ptr<class CUi_Default> m_pFinalResult;
	_float2 m_RankSize = { 60, 60 };
	_bool m_bSound = { false };

public:
	static shared_ptr<CUi_FinalResult> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
