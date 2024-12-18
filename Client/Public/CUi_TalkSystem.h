#pragma once

#include "Client_Defines.h"
#include "CUi_Canvas.h"

BEGIN(Client)

class CUi_TalkSystem : public CUi_Canvas
{
public:
	static wstring ObjID;

public:
	enum TALK_WINDOW { WINDOW_POINT, WINDOW_LEFT, WINDOW_RIGHT, WINDOW_END };
	enum TALK_CHAR { CHAR_CHAI, CHAR_SPEAKER, CHAR_ROBOT, CHAR_HINTBOT, CHAR_CNMN, CHAR_PEPPER, CHAR_MACARON, CHAR_KORSICA, CHAR_REKKA, CHAR_MONKORSICA, CHAR_MIMOSA, CHAR_KALE, CHAR_CNMN_SECOND, CHAR_END };
	enum TALK_FACE { FACE_NORMAL, FACE_ANGER, FACE_CONFIDENT, FACE_HAPPY, FACE_SURPRISE, FACE_WORRY, FACE_END };
	enum TALK_INFO_NAME
	{
		INFO_NAME_CHARACTER,
		INFO_NAME_TEXT,
		INFO_NAME_NAME,
		INFO_NAME_FACE,
		INFO_NAME_END
	};

public:
	struct TalkInfoDesc
	{
		TALK_CHAR  CharType = { TALK_CHAR::CHAR_END };
		_wstring   Text = { L"" };
		_wstring   Name = { L"" };
		TALK_FACE  FaceType= { TALK_FACE::FACE_END };
		string     SoundChannel = {"" };
		_wstring   SoundFileName = { L"" };
	};

	using Desc = struct CUi_TalkSystemInfo : public CUi_2DInfo
	{
		_wstring wsFileName;
		function<void()> endCallback;
	};

protected:
	CUi_TalkSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TalkSystem(const CUi_TalkSystem& rhs);

public:
	virtual ~CUi_TalkSystem();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Dying() override;
	virtual void    Set_Active(_bool Active) override;

public:
	void Create_TalkWindow();
	void Create_TalkChai();
	void Create_TalkSpeaker();
	void Create_TalkRobot();
	void Create_TalkHintbot();
	void Create_TalkCNMN();
	void Create_TalkPepper();
	void Create_TalkMacaron();
	void Create_TalkKorsica();
	void Create_TalkRekka();
	void Create_TalkMonKorsica();
	void Create_TalkMimosa();
	void Create_TalkKale();
	void Create_TalkCNMNSECOND();

	void LoadTalkFile(_wstring wsFileName);
	void ProceedTalk();
	void ShowTalk();
	void ClearTalk();
	void InitializeTalk();
	void AddCharacterFace(TALK_CHAR eCharacterType, TALK_FACE eFaceType, shared_ptr<class CUi_Sprite> InFaceSprite);
	virtual void ReceiveUIEvent(UI_EVENT InUIEvent) override;

	string WStringToString(const wstring& wstr)
	{
		string str(wstr.begin(), wstr.end());
		return str;
	}

protected:
	_int          m_iAnimTimer = { 0 };
	_int          m_iCurrentIndex = { 0 };
	TALK_CHAR     m_eTalkInfo = { CHAR_END };

protected:
	shared_ptr<class CUi_Font>    m_pBlackBoard;
	shared_ptr<class CUi_Font>    m_pWindowFont;
	shared_ptr<class CUi_Sprite>  m_pWindowSprite[WINDOW_END];

	shared_ptr<class CUi_Sprite>  m_pCharacter[CHAR_END] = { nullptr };
	map<TALK_FACE, vector<shared_ptr<class CUi_Sprite>>> m_pCharacterFace[CHAR_END];

	vector<TalkInfoDesc> m_TalkInfoList;
	TalkInfoDesc         m_PreTalkInfo;

	function<void()>	m_endCallback;

public:
	static shared_ptr<CUi_TalkSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
