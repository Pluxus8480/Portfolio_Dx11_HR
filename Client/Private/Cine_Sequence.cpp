#include "Cine_Sequence.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Cine_Actor.h"
#include "Cine_Camera.h"

CCine_Sequence::CCine_Sequence(const string& strTitle, map<string, shared_ptr<CCine_Actor>>* pActorList, _int iSeqIndex)
{
	m_strSequenceName = strTitle;
	m_fFrame_Total = 1.f;
	m_pActors = pActorList;
	m_iMySequenceIndex = iSeqIndex;

	m_pBaseTransform = static_pointer_cast<CTransform>(
		GAMEINSTANCE->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform")));
	m_pBaseTransform->Set_WorldMatrix(XMMatrixIdentity());
}

CCine_Sequence::CCine_Sequence(const json& sequenceInfo, map<string, shared_ptr<CCine_Actor>>* pActorList, _int iSeqIndex)
{
	m_strSequenceName = sequenceInfo["SequenceName"];
	m_fFrame_Total = sequenceInfo["TotalFrame"];
	m_iMySequenceIndex = iSeqIndex;

	_float4x4 matTrans{};
	array<float, 16> fOffsetArray;
	_uint index = 0;
	auto initMat = sequenceInfo["BaseMat"];
	for (auto& fRow : initMat)
		for (auto& fValue : fRow)
			fOffsetArray[index++] = fValue;
	memcpy(&matTrans, &fOffsetArray, sizeof(_float4x4));

	m_pBaseTransform = static_pointer_cast<CTransform>(
		GAMEINSTANCE->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform")));
	m_pBaseTransform->Set_WorldMatrix(matTrans);

	if (sequenceInfo.contains("Actions"))
	{
		auto actionArray = sequenceInfo["Actions"];
		for (auto action : actionArray)
		{
			ActionInfo actionInfo;
			actionInfo.fFrame_Start = action["FrameStart"];
			actionInfo.strActor = action["StrActor"];
			actionInfo.strAnim = action["StrAnim"];

			array<float, 16> fOffsetArray;
			_uint index = 0;
			auto initMat = action["InitMat"];
			for (auto& fRow : initMat)
				for (auto& fValue : fRow)
					fOffsetArray[index++] = fValue;
			memcpy(&actionInfo.matInit, &fOffsetArray, sizeof(_float4x4));

			m_InitTrans_List.push_back(static_pointer_cast<CTransform>(
				GAMEINSTANCE->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"))));
			m_InitTrans_List.back()->Set_WorldMatrix(actionInfo.matInit);

			m_ActionList.emplace_back(actionInfo);
		}
	}

	if (sequenceInfo.contains("CameraInfoes"))
	{
		auto camInfoArray = sequenceInfo["CameraInfoes"];
		for (auto jsonCamInfo : camInfoArray)
		{
			CineCamInfo camInfo{};
			camInfo.fTimeLine = jsonCamInfo["TimeLine"];
			camInfo.fFov = jsonCamInfo["Fov"];
			camInfo.iInterpType = jsonCamInfo["InterpType"];
			auto initMat = jsonCamInfo["TransMat"];
			array<float, 16> fOffsetArray;
			_uint index = 0;
			for (auto& fRow : initMat)
				for (auto& fValue : fRow)
					fOffsetArray[index++] = fValue;
			memcpy(&camInfo.matTrans, &fOffsetArray, sizeof(_float4x4));

			m_CamInfoList.emplace_back(camInfo);
		}
	}

	if (sequenceInfo.contains("Callbacks"))
	{
		auto callbackArray = sequenceInfo["Callbacks"];
		for (auto callback : callbackArray)
		{
			CineCallback callbackInfo;
			callbackInfo.fTimeLine = callback["TimeLine"];
			if (callback.contains("TransPos"))
			{
				array<_float, 4> vPos = callback["TransPos"];
				memcpy(&callbackInfo.vPos, &vPos, sizeof(_float4));
			}
			else
				callbackInfo.vPos = { 0.f, 0.f, 0.f, 1.f };
			if (callback.contains("TransLook"))
			{
				array<_float, 4> vLook = callback["TransLook"];
				memcpy(&callbackInfo.vLook, &vLook, sizeof(_float4));
			}
			else
				callbackInfo.vLook = { 0.f, 0.f, 1.f, 0.f };
			if (callback.contains("TransSize"))
			{
				array<_float, 3> vSize = callback["TransSize"];
				memcpy(&callbackInfo.vSize, &vSize, sizeof(_float3));
			}
			else
				callbackInfo.vSize = { 1.f, 1.f, 1.f };
			callbackInfo.funcCallback = [](_float4, _float4, _float3) {};

			m_CallbackList.emplace_back(callbackInfo);
		}
	}

	if (sequenceInfo.contains("Effects"))
	{
		auto effectArray = sequenceInfo["Effects"];
		for (auto effect : effectArray)
		{
			EffectInfo effectInfo;
			effectInfo.fTimeLine = effect["TimeLine"];
			effectInfo.fVibeScale = effect["VibeScale"];
			effectInfo.fVibeFrame = effect["VibeFrame"];

			m_EffectList.emplace_back(effectInfo);
		}
	}

	if (sequenceInfo.contains("Conversations"))
	{
		auto convArray = sequenceInfo["Conversations"];
		for (auto convInfo : convArray)
		{
			ConvCallback convCallback;
			convCallback.fTimeLine = convInfo["TimeLine"];
			convCallback.fConvFrame = convInfo["ConvFrame"];
			convCallback.funcCallback = [&](_float) {};
			m_ConversationList.emplace_back(convCallback);
		}
	}

	m_pActors = pActorList;
}

CCine_Sequence::~CCine_Sequence()
{
	for (auto pTransform : m_InitTrans_List)
		pTransform = nullptr;
}

shared_ptr<CGameObject> CCine_Sequence::GetActorPointer(_int iIndex)
{
	string strActor = m_ActionList[iIndex].strActor;
	return m_pActors->at(strActor);
}
shared_ptr<CGameObject> CCine_Sequence::GetActorPointer(const string& strActor)
{
	return m_pActors->at(strActor);
}

void CCine_Sequence::SortActionList()
{
	auto iSize = m_ActionList.size();
	vector<size_t> indices(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		indices[i] = i;

	sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
		return m_ActionList[a].fFrame_Start < m_ActionList[b].fFrame_Start;
		});

	vector<ActionInfo> sortedActionList(iSize);
	vector<shared_ptr<CTransform>> sortedTransList(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
	{
		sortedActionList[i] = m_ActionList[indices[i]];
		sortedTransList[i] = m_InitTrans_List[indices[i]];
	}
	m_ActionList = sortedActionList;
	m_InitTrans_List = sortedTransList;
}

void CCine_Sequence::SortCamList()
{
	auto iSize = m_CamInfoList.size();
	vector<size_t> indices(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		indices[i] = i;

	sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
		return m_CamInfoList[a].fTimeLine < m_CamInfoList[b].fTimeLine;
		});

	vector<CineCamInfo> sortedCamList(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		sortedCamList[i] = m_CamInfoList[indices[i]];
	m_CamInfoList = sortedCamList;
}

void CCine_Sequence::SortCallbackList()
{
	auto iSize = m_CallbackList.size();
	vector<size_t> indices(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		indices[i] = i;

	sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
		return m_CallbackList[a].fTimeLine < m_CallbackList[b].fTimeLine;
		});

	vector<CineCallback> sortedCallbackList(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		sortedCallbackList[i] = m_CallbackList[indices[i]];
	m_CallbackList = sortedCallbackList;
}

void CCine_Sequence::SortEffectList()
{
	auto iSize = m_EffectList.size();
	vector<size_t> indices(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		indices[i] = i;

	sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
		return m_EffectList[a].fTimeLine < m_EffectList[b].fTimeLine;
		});

	vector<EffectInfo> sortedEffectList(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		sortedEffectList[i] = m_EffectList[indices[i]];
	m_EffectList = sortedEffectList;
}

void CCine_Sequence::SortConvList()
{
	auto iSize = m_ConversationList.size();
	vector<size_t> indices(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		indices[i] = i;

	sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
		return m_ConversationList[a].fTimeLine < m_ConversationList[b].fTimeLine;
		});

	vector<ConvCallback> sortedConvList(iSize);
	for (size_t i = 0; i < indices.size(); ++i)
		sortedConvList[i] = m_ConversationList[indices[i]];
	m_ConversationList = sortedConvList;
}

void CCine_Sequence::PushAction(const ActionInfo& actionInfo)
{
	m_ActionList.emplace_back(actionInfo);
	m_InitTrans_List.push_back(static_pointer_cast<CTransform>(
		GAMEINSTANCE->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"))));
	m_InitTrans_List.back()->Set_WorldMatrix(actionInfo.matInit);

	SortActionList();

	_float fAnimTime = m_pActors->at(actionInfo.strActor)->GetAnimDuration(actionInfo.strAnim);
	m_fFrame_Total = fAnimTime;
}

void CCine_Sequence::PushCamInfo(const CineCamInfo& camInfo)
{
	m_CamInfoList.emplace_back(camInfo);
	SortCamList();
}

void CCine_Sequence::PushEffect(const EffectInfo& effectInfo)
{
	m_EffectList.emplace_back(effectInfo);
	SortEffectList();
}

void CCine_Sequence::PushCallback(const CineCallback& callback)
{
	m_CallbackList.emplace_back(callback);
	SortCallbackList();
}

void CCine_Sequence::PushConversation(const ConvCallback& callback)
{
	m_ConversationList.emplace_back(callback);
	SortConvList();
}

void CCine_Sequence::InitFrame()
{
	m_fCurFrame = 0.f;
}
void CCine_Sequence::InitActors()
{
	for (auto pActor : *m_pActors)
		pActor.second->SetState(CCine_Actor::EState::NONE);

	for (auto action : m_ActionList)
	{
		auto pActor = m_pActors->at(action.strActor);
		pActor->SwitchAnim(action.strAnim);
		pActor->SetState(CCine_Actor::EState::ACTING);
	}
}
void CCine_Sequence::InitCamera()
{
	if (m_CamInfoList.size() > 0)
		m_pCineCam->SetCamInfo(m_CamInfoList.front());
}

_bool CCine_Sequence::PlaySequence(const _float fFrame)
{
	_float fPreFrame = m_fCurFrame;
	m_fCurFrame += fFrame;

	_uint iSelectIndex = 0;
	for (auto action : m_ActionList)
	{
		_float fActorFrame = m_fCurFrame - action.fFrame_Start;
		if (fActorFrame < 0.f)
			continue;
		_float fDuration = m_pActors->at(action.strActor)->GetAnimDuration(action.strAnim) + action.fFrame_Start;
		if (fDuration <= m_fCurFrame)
			continue;

		_float4x4 matTrans{};
		XMStoreFloat4x4(&matTrans, m_pBaseTransform->Get_WorldMatrix_XMMat() * m_InitTrans_List[iSelectIndex]->Get_WorldMatrix_XMMat());
		m_pActors->at(action.strActor)->SetTransform(matTrans);
		m_pActors->at(action.strActor)->PlayAnimation(fActorFrame, m_iMySequenceIndex);

		++iSelectIndex;
	}

	// Camera move
	if (m_CamInfoList.size() > 1)
	{
		_int iCurCamIndex = -1;	// For Curve

		CineCamInfo srcCamInfo;
		CineCamInfo destCamInfo;	// For Linear
		if (m_fCurFrame >= m_CamInfoList.back().fTimeLine)
		{
			srcCamInfo = m_CamInfoList.back();
			srcCamInfo.iInterpType = 0;
		}
		else
		{
			for (_int i = 0; i < m_CamInfoList.size() - 1; ++i)
			{
				if (m_fCurFrame >= m_CamInfoList[i].fTimeLine && m_fCurFrame < m_CamInfoList[i + 1].fTimeLine)
				{
					srcCamInfo = m_CamInfoList[i];
					if (srcCamInfo.iInterpType == 1)
					{
						destCamInfo = m_CamInfoList[i + 1];
						break;
					}
					else if (srcCamInfo.iInterpType == 2)
					{
						iCurCamIndex = i;
						break;
					}
				}
			}
		}

		switch (srcCamInfo.iInterpType)
		{
			case 1:
				LinearMove(srcCamInfo, destCamInfo, m_fCurFrame);
				break;
			case 2:
			{
				// Generate curve point list
				vector<CineCamInfo> curveList;
				curveList.push_back(m_CamInfoList[iCurCamIndex]);

				_bool tempFlag = true;
				for (_int i = iCurCamIndex - 1; i >= 0; --i)
				{
					if (tempFlag && m_CamInfoList[i].iInterpType == 2)
						curveList.push_back(m_CamInfoList[i]);
					else
						tempFlag = false;
				}
				tempFlag = true;
				for (_int i = iCurCamIndex + 1; i < m_CamInfoList.size(); ++i)
				{
					if (tempFlag && m_CamInfoList[i].iInterpType == 2)
						curveList.push_back(m_CamInfoList[i]);
					else if (tempFlag && m_CamInfoList[i].iInterpType == 3)
					{
						curveList.push_back(m_CamInfoList[i]);
						tempFlag = false;
					}
					else
						tempFlag = false;
				}

				// Sorting CurvList
				sort(curveList.begin(), curveList.end(), [](const CineCamInfo& a, const CineCamInfo& b) {
					return a.fTimeLine < b.fTimeLine;
					});

				_float fStart = curveList.front().fTimeLine;
				_float fEnd = curveList.back().fTimeLine;
				if (fEnd - fStart <= 0.f || fEnd < m_fCurFrame)
					m_pCineCam->SetCamInfo(srcCamInfo);
				else
				{
					_float fRatio = (m_fCurFrame - fStart) / (fEnd - fStart);
					CurveMove(curveList, fRatio);
				}
			}
			break;
			default:
				m_pCineCam->SetCamInfo(srcCamInfo);
				break;
		}
	}
	else
	{
		m_pCineCam->SetCamInfo(m_CamInfoList.front());
	}

	// Callback
	for (_uint i = 0; i < m_CallbackList.size(); ++i)
	{
		auto callback = m_CallbackList[i];
		if (fPreFrame <= callback.fTimeLine && m_fCurFrame >= callback.fTimeLine)
			callback.funcCallback(callback.vPos, callback.vLook, callback.vSize);
	}

	// Effect
	for (_uint i = 0; i < m_EffectList.size(); ++i)
	{
		if (m_EffectList[i].fVibeScale > 0.01f)
		{
			if (m_EffectList[i].fTimeLine <= m_fCurFrame
				&& m_EffectList[i].fTimeLine + m_EffectList[i].fVibeFrame > m_fCurFrame)
				Vibrate(m_EffectList[i].fVibeScale);
		}
	}

	// Conversation
	for (_uint i = 0; i < m_ConversationList.size(); ++i)
	{
		auto callback = m_ConversationList[i];
		if (fPreFrame <= callback.fTimeLine && m_fCurFrame >= callback.fTimeLine)
			callback.funcCallback(callback.fConvFrame / 60.f);
	}

	return m_fCurFrame >= m_fFrame_Total;
}

void CCine_Sequence::SetFrame(const _float fFrame)
{
	_uint iSelectIndex = 0;
	for (auto action : m_ActionList)
	{
		_float fActorFrame = fFrame - action.fFrame_Start;
		if (fActorFrame < 0.f)
			continue;
		_float fDuration = m_pActors->at(action.strActor)->GetAnimDuration(action.strAnim) + action.fFrame_Start;
		if (fDuration <= fFrame)
			continue;

		auto pActor = m_pActors->at(action.strActor);
		pActor->SwitchAnim(action.strAnim);

		_float4x4 matTrans{};
		XMStoreFloat4x4(&matTrans, m_pBaseTransform->Get_WorldMatrix_XMMat() * m_InitTrans_List[iSelectIndex]->Get_WorldMatrix_XMMat());
		pActor->SetTransform(matTrans);
		pActor->SetAnimFrame(fActorFrame);

		++iSelectIndex;
	}
}

void CCine_Sequence::LinearMove(const CineCamInfo& srcCamInfo, const CineCamInfo& destCamInfo, _float fTime)
{
	_float fRatio = (fTime - srcCamInfo.fTimeLine) / (destCamInfo.fTimeLine - srcCamInfo.fTimeLine);
	_float fResultFov = srcCamInfo.fFov + fRatio * (destCamInfo.fFov - srcCamInfo.fFov);

	_matrix matTarget, matCurrent, matResult;
	matCurrent = XMLoadFloat4x4(&srcCamInfo.matTrans);
	matTarget = XMLoadFloat4x4(&destCamInfo.matTrans);

	_vector vCurPos, vCurScale, vCurRot;
	_vector vTargetPos, vTargetScale, vTargetRot;
	XMMatrixDecompose(&vCurScale, &vCurRot, &vCurPos, matCurrent);
	XMMatrixDecompose(&vTargetScale, &vTargetRot, &vTargetPos, matTarget);

	_vector resultRot = XMQuaternionSlerp(vCurRot, vTargetRot, fRatio);
	_vector resultPos = XMVectorSetW(XMVectorLerp(vCurPos, vTargetPos, fRatio), 1.f);
	matResult = XMMatrixAffineTransformation(
		XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), resultRot, resultPos);

	CineCamInfo resultCamInfo{};
	resultCamInfo.fTimeLine = fTime;
	resultCamInfo.fFov = fResultFov;
	XMStoreFloat4x4(&resultCamInfo.matTrans, matResult);
	m_pCineCam->SetCamInfo(resultCamInfo);
}

CCine_Sequence::CineCamInfo CCine_Sequence::BezierPoint(const vector<CineCamInfo>& controlPoints, _float fRatio)
{
	if (controlPoints.size() == 1)
		return controlPoints[0];

	vector<CineCamInfo> newPoints;
	newPoints.reserve(controlPoints.size() - 1);
	for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
		auto resultCineInfo = CineCamInfo{};
		_float fResultFov = controlPoints[i].fFov + fRatio * (controlPoints[i + 1].fFov - controlPoints[i].fFov);

		_matrix matTarget, matCurrent, matResult;
		matCurrent = XMLoadFloat4x4(&controlPoints[i].matTrans);
		matTarget = XMLoadFloat4x4(&controlPoints[i + 1].matTrans);

		_vector vCurPos, vCurScale, vCurRot;
		_vector vTargetPos, vTargetScale, vTargetRot;
		XMMatrixDecompose(&vCurScale, &vCurRot, &vCurPos, matCurrent);
		XMMatrixDecompose(&vTargetScale, &vTargetRot, &vTargetPos, matTarget);

		_vector resultRot = XMQuaternionSlerp(vCurRot, vTargetRot, fRatio);
		_vector resultPos = XMVectorSetW(XMVectorLerp(vCurPos, vTargetPos, fRatio), 1.f);
		matResult = XMMatrixAffineTransformation(
			XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), resultRot, resultPos);

		resultCineInfo.fFov = fResultFov;
		XMStoreFloat4x4(&resultCineInfo.matTrans, matResult);

		newPoints.emplace_back(resultCineInfo);
	}

	return BezierPoint(newPoints, fRatio);
}
CCine_Sequence::CineCamInfo CCine_Sequence::GenerateBezierPoint(const vector<CineCamInfo>& controlPoints, _float t)
{
	t = clamp(t, 0.0f, 1.0f);
	return BezierPoint(controlPoints, t);
}
void CCine_Sequence::CurveMove(const vector<CineCamInfo>& curveList, _float fRatio)
{
	auto cineInfo = GenerateBezierPoint(curveList, fRatio);
	m_pCineCam->SetCamInfo(cineInfo);
}

void CCine_Sequence::Vibrate(_float fVibeStrength)
{
	if (fVibeStrength <= 0.f)
		return;

	_float3 vRandomPos = GAMEINSTANCE->Random_Float3(
		{ -fVibeStrength, -fVibeStrength, -fVibeStrength },
		{ fVibeStrength, fVibeStrength, fVibeStrength }
	);
	auto vCurPos = m_pCineCam->Get_Transform()->Get_Position();
	vCurPos += XMVectorSetW(XMLoadFloat3(&vRandomPos), 0.f);

	m_pCineCam->Get_Transform()->Set_Position(vCurPos);
}