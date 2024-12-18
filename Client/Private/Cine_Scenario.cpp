#include "Cine_Scenario.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Cine_Sequence.h"
#include "Cine_Actor.h"
#include "Cine_Camera.h"

CCine_Scenario::CCine_Scenario()
{
	m_strScenarioName = "NewScene";

	auto pCamera = GAMEINSTANCE->Get_Camera(TEXT("Camera_Cine"));
	if (nullptr != pCamera)
		m_pCineCam = dynamic_pointer_cast<CCine_Camera>(pCamera);
	else
	{
		CCine_Camera::DESC	CameraDesc{};
		CameraDesc.vEye = _float4(60.f, 20.f, -55.f, 1.f);
		CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
		CameraDesc.fFovy = XMConvertToRadians(90.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 500.f;
		CameraDesc.fSpeedPerSec = 30.f;
		CameraDesc.strTag = TEXT("Camera_Cine");
		CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		m_pCineCam = dynamic_pointer_cast<CCine_Camera>(
			GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Camera"), CCine_Camera::ObjID, &CameraDesc));
		GAMEINSTANCE->Add_Camera(TEXT("Camera_Cine"), m_pCineCam);
	}
}

CCine_Scenario::CCine_Scenario(const string& strScenarioPath)
{
	auto iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();

	filesystem::path path(strScenarioPath);
	m_strScenarioName = path.stem().string();
	m_wstrScenarioName = wstring(m_strScenarioName.begin(), m_strScenarioName.end());

	ifstream fileScenario(strScenarioPath);
	auto jsonImpoter = json::parse(fileScenario);
	fileScenario.close();

	// Actors
	auto actorArray = jsonImpoter["Actors"];
	for (auto actor : actorArray)
	{
		string strActorName = actor["ActorName"];
		string strModelTag = actor["ModelTag"];
		string strModelPath = actor["ModelPath"];
		string strDesc = actor["Desc"];

		CCine_Actor::DESC desc{};
		desc.strActorName = strActorName;
		desc.strModelTag = strModelTag;
		desc.strModelPath = strModelPath;
		desc.strDesc = strDesc;
		auto pActor = dynamic_pointer_cast<CCine_Actor>(
			GAMEINSTANCE->Add_Clone_Return(iLevelIndex, TEXT("Layer_Actor"), CCine_Actor::ObjID, &desc));

		m_UsableActors.emplace(strActorName, pActor);
	}

	// Camera
	auto pCamera = GAMEINSTANCE->Get_Camera(TEXT("Camera_Cine"));
	if (nullptr != pCamera)
		m_pCineCam = dynamic_pointer_cast<CCine_Camera>(pCamera);
	else
	{
		CCine_Camera::DESC	CameraDesc{};
		CameraDesc.vEye = _float4(60.f, 20.f, -55.f, 1.f);
		CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
		CameraDesc.fFovy = XMConvertToRadians(90.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 500.f;
		CameraDesc.fSpeedPerSec = 30.f;
		CameraDesc.strTag = TEXT("Camera_Cine");
		CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		m_pCineCam = dynamic_pointer_cast<CCine_Camera>(
			GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Camera"), CCine_Camera::ObjID, &CameraDesc));
		GAMEINSTANCE->Add_Camera(TEXT("Camera_Cine"), m_pCineCam);
	}

	// Sequence
	auto sequenceArray = jsonImpoter["Sequences"];
	for (auto sequence : sequenceArray)
	{
		_int iSize = (_int)m_SequenceList.size();
		m_SequenceList.emplace_back(new CCine_Sequence{ sequence, &m_UsableActors, iSize });
	}
}

CCine_Scenario::~CCine_Scenario()
{
	for (auto actor : m_UsableActors)
		actor.second = nullptr;
	m_UsableActors.clear();

	for (auto seq : m_SequenceList)
		seq = nullptr;
	m_SequenceList.clear();
}

_bool CCine_Scenario::PlayScenario(_float fTimeDelta)
{
	if (m_SequenceList.size() == 0 || m_iCurIndexSeq == m_SequenceList.size())
		return true;

	m_mapTrans_Last.clear();
	if (nullptr == m_pCurSequence)
	{
		m_iCurIndexSeq = 0;
		m_pCurSequence = m_SequenceList.front();
		m_pCurSequence->InitFrame();
		m_pCurSequence->InitActors();
		m_pCurSequence->InitCamera();
	}

	if (m_pCurSequence->PlaySequence(fTimeDelta * m_fFrameSpeed))
	{
		++m_iCurIndexSeq;
		if (m_iCurIndexSeq == m_SequenceList.size())
		{
			for (auto pActor : *m_pCurSequence->m_pActors)
			{
				if (CCine_Actor::EState::ACTING == pActor.second->GetState())
					m_mapTrans_Last.emplace(pActor.first, pActor.second->GetRootTrans());
				pActor.second->SetState(CCine_Actor::EState::NONE);
			}

			m_pCurSequence = nullptr;
			m_iCurIndexSeq = -1.f;
			return true;
		}

		m_pCurSequence = m_SequenceList[m_iCurIndexSeq];
		m_pCurSequence->InitFrame();
		m_pCurSequence->InitActors();
		m_pCurSequence->InitCamera();
	}
	return false;
}

void CCine_Scenario::ReadyToPlay()
{
	if (nullptr == m_pCurSequence)
	{
		m_iCurIndexSeq = 0;
		m_pCurSequence = m_SequenceList.front();
	}
	m_pCurSequence->InitFrame();
	m_pCurSequence->InitActors();
	m_pCurSequence->InitCamera();

	GAMEINSTANCE->Set_MainCamera(TEXT("Camera_Cine"));
}

_float4x4 CCine_Scenario::GetCurCamMatrix()
{
	return m_pCineCam->Get_Transform()->Get_WorldMatrix();
}

void CCine_Scenario::SaveScenario(const string& strJsonPath)
{
	filesystem::path path(strJsonPath);
	m_strScenarioName = path.stem().string();

	json exportJson;
	exportJson["Actors"] = json::array();
	for (auto actor : m_UsableActors)
	{
		json actorInfo;
		actorInfo["ActorName"] = actor.second->GetActorName();
		actorInfo["ModelTag"] = actor.second->GetModelTag();
		actorInfo["ModelPath"] = actor.second->GetModelPath();
		actorInfo["Desc"] = actor.second->GetDesc();
		exportJson["Actors"].push_back(actorInfo);
	}

	exportJson["Sequences"] = json::array();
	for (auto sequence : m_SequenceList)
	{
		json seqInfo;
		seqInfo["SequenceName"] = sequence->GetName();
		seqInfo["TotalFrame"] = sequence->GetTotalFrame();
		seqInfo["BaseMat"] = sequence->GetMatTrans().m;

		auto actionArray = json::array();
		_uint iIndex = 0;
		for (auto action : sequence->GetActionList())
		{
			json jsonAction;
			jsonAction["FrameStart"] = action.fFrame_Start;
			jsonAction["StrActor"] = action.strActor;
			jsonAction["StrAnim"] = action.strAnim;
			jsonAction["InitMat"] = sequence->GetActionTransPointer(iIndex)->Get_WorldMatrix().m;

			actionArray.push_back(jsonAction);
			++iIndex;
		}
		seqInfo["Actions"] = actionArray;

		auto camInfoArray = json::array();
		for (auto camInfo : sequence->GetCamList())
		{
			json jsonCamInfo;
			jsonCamInfo["TimeLine"] = camInfo.fTimeLine;
			jsonCamInfo["Fov"] = camInfo.fFov;
			jsonCamInfo["TransMat"] = camInfo.matTrans.m;
			jsonCamInfo["InterpType"] = camInfo.iInterpType;

			camInfoArray.push_back(jsonCamInfo);
		}
		seqInfo["CameraInfoes"] = camInfoArray;

		auto callbackArray = json::array();
		for (auto callbackInfo : sequence->GetCallbackList())
		{
			json jsonCallback;
			jsonCallback["TimeLine"] = callbackInfo.fTimeLine;

			array<_float, 4> array_4;
			memcpy(&array_4, &callbackInfo.vPos, sizeof(_float4));
			jsonCallback["TransPos"] = array_4;
			memcpy(&array_4, &callbackInfo.vLook, sizeof(_float4));
			jsonCallback["TransLook"] = array_4;
			array<_float, 3> array_3;
			memcpy(&array_3, &callbackInfo.vSize, sizeof(_float3));
			jsonCallback["TransSize"] = array_3;

			callbackArray.push_back(jsonCallback);
		}
		seqInfo["Callbacks"] = callbackArray;

		auto effectArray = json::array();
		for (auto effectInfo : sequence->GetEffectList())
		{
			json jsonEffect;
			jsonEffect["TimeLine"] = effectInfo.fTimeLine;
			jsonEffect["VibeScale"] = effectInfo.fVibeScale;
			jsonEffect["VibeFrame"] = effectInfo.fVibeFrame;

			effectArray.push_back(jsonEffect);
		}
		seqInfo["Effects"] = effectArray;

		auto convArray = json::array();
		for (auto convInfo : sequence->GetConvList())
		{
			json jsonConv;
			jsonConv["TimeLine"] = convInfo.fTimeLine;
			jsonConv["ConvFrame"] = convInfo.fConvFrame;
			convArray.push_back(jsonConv);
		}
		seqInfo["Conversations"] = convArray;

		exportJson["Sequences"].push_back(seqInfo);
	}

	ofstream of(strJsonPath);
	of << exportJson;
	if (!of.good())
		throw runtime_error("Error to save file!");
	else
		MSG_BOX(TEXT("Success to save scenario"));

	of.close();
}

void CCine_Scenario::SetActorsAndCamera()
{
	for (auto sequence : m_SequenceList)
	{
		sequence->m_pActors = &m_UsableActors;
		sequence->m_pCineCam = m_pCineCam;
	}
}

void CCine_Scenario::PushActor(const string& strModelPath)
{
	auto iLevelIndex = CGame_Manager::Get_Instance()->Get_Current_Level();

	filesystem::path path(strModelPath);
	string strModelTag = path.stem().string();

	if (m_UsableActors.find(strModelTag) == m_UsableActors.end())
	{
		m_ActorCnt.emplace(strModelTag, 0);

		CCine_Actor::DESC desc{};
		desc.strModelTag = strModelTag;
		desc.strModelPath = strModelPath;
		desc.strActorName = strModelTag;
		desc.strDesc = "";
		auto pActor = dynamic_pointer_cast<CCine_Actor>(
			GAMEINSTANCE->Add_Clone_Return(iLevelIndex, TEXT("Layer_Actor"), CCine_Actor::ObjID, &desc));

		m_UsableActors.emplace(strModelTag, pActor);
	}
	else
	{
		m_ActorCnt[strModelTag] += 1;
		_uint iCnt = m_ActorCnt[strModelTag];

		string strActorName = strModelTag;
		strActorName += "_";
		strActorName += to_string(iCnt);

		CCine_Actor::DESC desc{};
		desc.strModelTag = strModelTag;
		desc.strModelPath = strModelPath;
		desc.strActorName = strActorName;
		desc.strDesc = "";
		auto pActor = dynamic_pointer_cast<CCine_Actor>(
			GAMEINSTANCE->Add_Clone_Return(iLevelIndex, TEXT("Layer_Actor"), CCine_Actor::ObjID, &desc));

		m_UsableActors.emplace(strActorName, pActor);
	}
}

void CCine_Scenario::CreateSeq(const string& strSeqName)
{
	_int iSize = (_int)m_SequenceList.size();
	m_SequenceList.emplace_back(new CCine_Sequence{ strSeqName, &m_UsableActors, iSize });
	m_SequenceList.back()->m_pActors = &m_UsableActors;
	m_SequenceList.back()->m_pCineCam = m_pCineCam;
}

void CCine_Scenario::SelectSeq(_int iIndex)
{
	if (iIndex < 0 || iIndex >= m_SequenceList.size())
	{
		m_iCurIndexSeq = -1;
		m_pCurSequence = nullptr;
	}
	else
	{
		m_iCurIndexSeq = iIndex;
		m_pCurSequence = m_SequenceList[iIndex];
		m_pCurSequence->InitActors();
	}
}

void CCine_Scenario::ChangeToPlayMode()
{
	if (m_SequenceList.size() == 0)
		return;

	m_isPlayMode = true;

	if (nullptr == m_pCurSequence)
	{
		m_iCurIndexSeq = 0;
		m_pCurSequence = m_SequenceList.front();
	}
	m_pCurSequence->InitFrame();
	m_pCurSequence->InitActors();
	m_pCurSequence->InitCamera();

	GAMEINSTANCE->Set_MainCamera(TEXT("Camera_Cine"));
}

void CCine_Scenario::ChangeToToolMode()
{
	m_isPlayMode = false;
	GAMEINSTANCE->Set_MainCamera(TEXT("Camera_Tool_Main"));
}

