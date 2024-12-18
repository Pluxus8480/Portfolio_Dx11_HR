#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CCine_Scenario;
class CCine_Manager
{
	DECLARE_SINGLETON(CCine_Manager)

public:
	enum class EState {
		InGame, Scene, LastScene
	};

private:
	CCine_Manager();
public:
	~CCine_Manager();

public:
	void Initialize(const string& strCineFolderPath);
	_bool PlayCinematic(_float fTimeDelta);

public:
	_bool IsInGame() const { return m_curState == EState::InGame; };
	string GetCurSceneName() const;

public:
	void AddSequenceCallback(const string& strScenarioName, _uint iSeqIndex, _uint iCallbackIndex, function<void(_float4, _float4, _float3)> callback);
	void AddConversation_Callback(const string& strScenarioName, _uint iSeqIndex, _uint iCallbackIndex, function<void(_float)> callback);

	void AddStartCallback(const string& strScenarioName, function<void()> callback);
	void AddLastCallback(const string& strScenarioName, function<void()> callback);
	void StartCine(const string& strScenarioName);

public:
	_bool GetMatTrans(const string& strActorTag, _float4x4& matResult) {
		if (m_matTrans.find(strActorTag) == m_matTrans.end())
			return false;
		else
			matResult = m_matTrans[strActorTag];

		return true;
	};
private:
	map<string, _float4x4> m_matTrans;

private:
	EState m_curState = EState::InGame;

private:
	_bool m_triggerStart = false;

private:	// 이거 안되면... 뭐... Scenario 내부에다가 넣어놓으면 될듯
	map<string, list<function<void()>>>			m_List_LastCallback;
	list<function<void()>>						m_curLastCallback;

private:
	map<string, list<function<void()>>>			m_List_StartCallback;

private:
	map<string, shared_ptr<CCine_Scenario>>		m_List_CineScene;
	shared_ptr<CCine_Scenario>					m_pCurCineScene;


};

END