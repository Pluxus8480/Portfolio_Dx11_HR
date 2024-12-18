#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CCine_Actor;
class CCine_Sequence;
class CCine_Camera;
class CCine_Scenario
{
public:
	CCine_Scenario();
	CCine_Scenario(const string& strScenarioPath);
	~CCine_Scenario();

	////////// For Client //////////
public:
	_bool PlayScenario(_float fTimeDelta);
	void ReadyToPlay();

public:
	_float4x4 GetCurCamMatrix();
	void GetLastActorsTrans(map<string, _float4x4>& mapTrans) {
		mapTrans = m_mapTrans_Last;
	};

private:
	map<string, _float4x4> m_mapTrans_Last;


	////////// For tool //////////
public:
	void SaveScenario(const string& strJsonPath);

public:
	string GetSceneName() const {
		return m_strScenarioName;
	};
	wstring GetSceneName_wstring() const {
		return m_wstrScenarioName;
	};

public:
	_float4x4 GetBeforeTrans() {
		_float4x4 matResult{};
		XMStoreFloat4x4(&matResult, XMMatrixIdentity());

		auto iPreIndexSeq = m_iCurIndexSeq - 1;
		if (iPreIndexSeq < 0)
			return matResult;

		matResult = m_SequenceList[iPreIndexSeq]->GetMatTrans();
		return matResult;
	};

public:	// Only for pass the actors/camera pointer
	void SetActorsAndCamera();

public:
	map<string, shared_ptr<CCine_Actor>> GetActors() const {
		return m_UsableActors;
	};
	void PushActor(const string& strModelPath);

public:
	vector<shared_ptr<CCine_Sequence>> GetSequences() const {
		return m_SequenceList;
	};
	shared_ptr<CCine_Sequence> GetSelectedSeq() {
		return m_pCurSequence;
	};

	void CreateSeq(const string& strSeqName);
	void SelectSeq(_int iIndex);

public:
	_bool IsPlayMode() const { return m_isPlayMode; };
	void ChangeToPlayMode();
	void ChangeToToolMode();

private:
	_bool m_isPlayMode = false;


private:
	string m_strScenarioName = "";
	wstring m_wstrScenarioName = L"";

	_float m_fFrameSpeed = 60.f;

private:
	shared_ptr<CCine_Camera>				m_pCineCam = nullptr;
	map<string, shared_ptr<CCine_Actor>>	m_UsableActors;
	vector<shared_ptr<CCine_Sequence>>		m_SequenceList;

	// Only for model count
	map<string, _uint>						m_ActorCnt;

private:
	_int									m_iCurIndexSeq = -1;
	shared_ptr<CCine_Sequence>				m_pCurSequence = nullptr;
};

END