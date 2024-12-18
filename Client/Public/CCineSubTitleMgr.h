#pragma once
#include "Client_Defines.h"

BEGIN(Client)

class CCineSubTitleMgr
{
DECLARE_SINGLETON(CCineSubTitleMgr)

public:
	enum class CineType { CS0106, CS0230, CS0235, CS0240, CS0245, CS0820, CS0825, CS0830, 
		CS0835, CS0840, CS1125, CS1130, EV1120, EV1130, EV1150, EV1160, EV1170, EV1180, 
		EV1190, CS1205, CS1210, CS1215, CS1220, CS1225, EV1250, CS0620, End };
	enum class SpeakerType { Chai, Peppermint, Macaron, Korsica, SBR, Rekka, Mimosa, Kale, RekkaHost, CNMN, EVERYONE, End };
	using Desc = struct CineSubTitleInfo
	{
		wstring SpeakerName = L"";
		wstring String = L"";
		_float m_fLifeTime = { 9999 };
		_bool m_bRender = { false };
	};
private:
	CCineSubTitleMgr();

public:
	~CCineSubTitleMgr();


public:
	void Initialize();
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);
	void Render();


public:
	void RenderStartSetting(CineType eType, _uint vecindex, _float Time);
	void Initialize_IntroCallBack();
	void Initialize_RekkaCallBack();
	void Initialize_KorsicaCallBack();
	void Initialize_MimosaCallBack();
	void Initialize_KaleStageCallBack();
	void Initialize_KaleBossCallBack();

private:
	void Initialize_CS0106();
	
	void Initialize_CS0230();
	void Initialize_CS0235();
	void Initialize_CS0240();
	void Initialize_CS0245();


	void Initialize_CS0620();


	void Initialize_CS0820();
	void Initialize_CS0825();
	void Initialize_CS0830();
	void Initialize_CS0835();
	void Initialize_CS0840();


	void Initialize_CS1125();
	void Initialize_CS1130();
	void Initialize_EV1120();
	void Initialize_EV1130();
	void Initialize_EV1150();
	void Initialize_EV1160();
	void Initialize_EV1170();
	void Initialize_EV1180();
	void Initialize_EV1190();


	void Initialize_CS1205();
	void Initialize_CS1210();
	void Initialize_CS1215();
	void Initialize_CS1220();
	void Initialize_CS1225();
	void Initialize_EV1250();


	void Emplace_SubTitle(CineType CineType, _uint Index, SpeakerType eType, _float fLifeTime, wstring SubtitleString);

private:
	virtual void RenderFont(Desc FontDesc);


private:
	vector<Desc> m_SubTitleVec[(_uint)CineType::End];
	vector<Desc> m_RenderFontTitleVec;

	_uint m_iTestIndex = { 0 };

public:
	void Free();
};

END