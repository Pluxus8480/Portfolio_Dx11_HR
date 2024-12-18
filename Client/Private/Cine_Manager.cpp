#include "Cine_Manager.h"
#include "GameInstance.h"
#include "Cine_Scenario.h"
#include "Cine_Sequence.h"

#include "Player_Camera.h"


IMPLEMENT_SINGLETON(CCine_Manager)

CCine_Manager::CCine_Manager()
{
}
CCine_Manager::~CCine_Manager()
{
}

void CCine_Manager::Initialize(const string& strCineFolderPath)
{
	m_curState = EState::InGame;

	filesystem::path pathCineRekka(strCineFolderPath);
	for (const auto& entry : filesystem::directory_iterator(pathCineRekka))
	{
		if (entry.is_regular_file()) {
			string strFileName = entry.path().filename().string();
			string jsonExtension = ".json";
			if (strFileName.find(jsonExtension) != string::npos)
			{
				string filePath = entry.path().string();

				auto pCineScene = new CCine_Scenario{ filePath };
				pCineScene->SetActorsAndCamera();
				string strSceneName = pCineScene->GetSceneName();
				m_List_CineScene.emplace(strSceneName, pCineScene);

				m_List_LastCallback.emplace(strSceneName, list<function<void()>>{});
			}
		}
	}
}

_bool CCine_Manager::PlayCinematic(_float fTimeDelta)
{
	if (EState::Scene == m_curState)
	{
		if (m_triggerStart)
		{
			m_triggerStart = false;

			// Active start callback
			string strScenName = m_pCurCineScene->GetSceneName();
			for (auto startCallback : m_List_StartCallback[strScenName])
				startCallback();

			m_List_StartCallback[strScenName].clear();
			m_List_StartCallback.erase(strScenName);

			// Start synced sound
			GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, m_pCurCineScene->GetSceneName_wstring());

			return false;
		}

		if (m_pCurCineScene->PlayScenario(fTimeDelta))
		{
			m_pCurCineScene->GetLastActorsTrans(m_matTrans);

			// Call reserved callbacks
			for (auto callback : m_curLastCallback)
				callback();

			if (m_pCurCineScene->GetSceneName() != "CS0620")
			{
				// PlayerCamera Matrix
				auto pPlayerCamera = static_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main")));
				pPlayerCamera->SetTransform(m_pCurCineScene->GetCurCamMatrix());
				GAMEINSTANCE->Set_MainCamera(TEXT("Camera_Player_Main"));
			}
			else
			{
				GAMEINSTANCE->Set_MainCamera(TEXT("CKorsicaCamera"));
			}

			// Remove Scenario
			m_pCurCineScene = nullptr;

			m_curState = EState::InGame;
			return true;
		}
	}

	return false;
}

string CCine_Manager::GetCurSceneName() const
{
	if (m_pCurCineScene == nullptr)
		return "";
	return m_pCurCineScene->GetSceneName();
}

void CCine_Manager::AddSequenceCallback(const string& strScenarioName, _uint iSeqIndex, _uint iCallbackIndex, function<void(_float4, _float4, _float3)> callback)
{
	if (m_List_CineScene.find(strScenarioName) != m_List_CineScene.end())
	{
		auto pScenario = m_List_CineScene[strScenarioName];
		auto pSequence = pScenario->GetSequences()[iSeqIndex];
		pSequence->PushLogicCallback(iCallbackIndex, callback);
	}
	else if(m_pCurCineScene->GetSceneName() == strScenarioName)
	{
		auto pSequence = m_pCurCineScene->GetSequences()[iSeqIndex];
		pSequence->PushLogicCallback(iCallbackIndex, callback);
	}
}

void CCine_Manager::AddConversation_Callback(const string& strScenarioName, _uint iSeqIndex, _uint iCallbackIndex, function<void(_float)> callback)
{
	auto pScenario = m_List_CineScene[strScenarioName];
	auto pSequence = pScenario->GetSequences()[iSeqIndex];
	pSequence->PushConversationCallback(iCallbackIndex, callback);
}

void CCine_Manager::AddStartCallback(const string& strScenarioName, function<void()> callback)
{
	m_List_StartCallback[strScenarioName].emplace_back(callback);
}

void CCine_Manager::AddLastCallback(const string& strScenarioName, function<void()> callback)
{
	m_List_LastCallback[strScenarioName].emplace_back(callback);
}

void CCine_Manager::StartCine(const string& strScenarioName)
{
	m_triggerStart = true;
	m_curState = EState::Scene;

	m_pCurCineScene = m_List_CineScene[strScenarioName];

	// Save last callback
	m_curLastCallback = m_List_LastCallback[strScenarioName];

	m_List_CineScene[strScenarioName] = nullptr;
	m_List_CineScene.erase(strScenarioName);

	m_List_LastCallback[strScenarioName].clear();
	m_List_LastCallback.erase(strScenarioName);

	m_pCurCineScene->ReadyToPlay();

	// Trans ¿˙¿Â
	m_matTrans.clear();
}
