#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
class CTransform;
END

BEGIN(Client)

class CCine_Actor;
class CCine_Camera;
class CCine_Sequence
{
	friend class CCine_Scenario;

public:
	struct ActionInfo {
		_float fFrame_Start;
		_float4x4 matInit;
		string strActor;
		string strAnim;
	};

	struct CineCamInfo {
		_int		iInterpType;
		_float		fTimeLine;
		_float		fFov;
		_float4x4	matTrans;
	};

	struct CineCallback {
		_float				fTimeLine;
		_float3				vSize;
		_float4				vLook;
		_float4				vPos;
		function<void(_float4, _float4, _float3)>	funcCallback;
	};

	struct ConvCallback {
		_float				fTimeLine;
		_float				fConvFrame;
		function<void(_float)>	funcCallback;
	};

	struct EffectInfo {
		_float		fTimeLine;
		_float		fVibeScale = 0.f;
		_float		fVibeFrame = 0.f;
	};

public:
	CCine_Sequence(const string& strTitle, map<string, shared_ptr<CCine_Actor>>* pActorList, _int iSeqIndex);
	CCine_Sequence(const json& sequenceInfo, map<string, shared_ptr<CCine_Actor>>* pActorList, _int iSeqIndex);
	~CCine_Sequence();

public:
	const vector<ActionInfo>& GetActionList() const {
		return m_ActionList;
	}
	const vector<CineCamInfo>& GetCamList() const {
		return m_CamInfoList;
	}
	const vector<CineCallback>& GetCallbackList() const {
		return m_CallbackList;
	}
	const vector<EffectInfo>& GetEffectList() const {
		return m_EffectList;
	}
	const vector<ConvCallback>& GetConvList() const {
		return m_ConversationList;
	}
public:
	shared_ptr<CGameObject> GetActorPointer(_int iIndex);
	shared_ptr<CGameObject> GetActorPointer(const string& strActor);

private:
	void SortActionList();
	void SortCamList();
	void SortCallbackList();
	void SortEffectList();
	void SortConvList();

public:
	void PushAction(const ActionInfo& actionInfo);
	void DeleteAction(_int iIndex) {
		if (iIndex < 0 || iIndex >= m_ActionList.size())
			return;
		auto iter = m_ActionList.begin() + iIndex;
		m_ActionList.erase(iter);
	};

	void PushCamInfo(const CineCamInfo& camInfo);
	void DeleteCamInfo(_int iIndex) {
		if (iIndex < 0 || iIndex >= m_CamInfoList.size())
			return;
		auto iter = m_CamInfoList.begin() + iIndex;
		m_CamInfoList.erase(iter);
	};

	void PushEffect(const EffectInfo& effectInfo);
	void DeleteEffect(_int iIndex) {
		if (iIndex < 0 || iIndex >= m_EffectList.size())
			return;
		auto iter = m_EffectList.begin() + iIndex;
		m_EffectList.erase(iter);
	};

	void PushCallback(const CineCallback& callback);
	void DeleteCallback(_int iIndex) {
		if (iIndex < 0 || iIndex >= m_CallbackList.size())
			return;
		auto iter = m_CallbackList.begin() + iIndex;
		m_CallbackList.erase(iter);
	};

	void PushConversation(const ConvCallback& callback);
	void DeleteConversation(_int iIndex) {
		if (iIndex < 0 || iIndex >= m_ConversationList.size())
			return;
		auto iter = m_ConversationList.begin() + iIndex;
		m_ConversationList.erase(iter);
	};

	void EditCallback_Pos(_uint iCallbackIndex, _fvector vPos) {
		if (iCallbackIndex >= m_CallbackList.size() || iCallbackIndex < 0)
			return;
		XMStoreFloat4(&m_CallbackList[iCallbackIndex].vPos, vPos);
		m_CallbackList[iCallbackIndex].vPos.w = 1.f;
	};
	void EditCallback_Look(_uint iCallbackIndex, _fvector vLookPos) {
		if (iCallbackIndex >= m_CallbackList.size() || iCallbackIndex < 0)
			return;
		auto vLook = XMVectorSetW(vLookPos - XMLoadFloat4(&m_CallbackList[iCallbackIndex].vPos), 0.f);
		XMStoreFloat4(&m_CallbackList[iCallbackIndex].vLook, XMVector3Normalize(vLook));
	};
	void EditCallback_Size(_uint iCallbackIndex, _float3 vSize) {
		if (iCallbackIndex >= m_CallbackList.size() || iCallbackIndex < 0)
			return;
		m_CallbackList[iCallbackIndex].vSize = vSize;
	};

	void PushLogicCallback(_uint iCallbackIndex, function<void(_float4, _float4, _float3)> logicCallback) {
		if (iCallbackIndex < 0 || iCallbackIndex >= m_CallbackList.size())
			return;
		m_CallbackList[iCallbackIndex].funcCallback = logicCallback;
	}
	void PushConversationCallback(_uint iCallbackIndex, function<void(_float)> convCallback) {
		if (iCallbackIndex < 0 || iCallbackIndex >= m_ConversationList.size())
			return;
		m_ConversationList[iCallbackIndex].funcCallback = convCallback;
	}
public:	// Play Sequence Logics
	void InitFrame();
	void InitActors();
	void InitCamera();

	_bool PlaySequence(const _float fFrame);
	void SetFrame(const _float fFrame);

private: // For Camera control
	void LinearMove(const CineCamInfo& srcCamInfo, const CineCamInfo& destCamInfo, _float fTime);
	void CurveMove(const vector<CineCamInfo>& curveList, _float fRatio);
	void Vibrate(_float fVibeStrength);

private:
	CineCamInfo BezierPoint(const vector<CineCamInfo>& controlPoints, _float t);
	CineCamInfo GenerateBezierPoint(const vector<CineCamInfo>& controlPoints, _float t);

public:
	const string GetName() const {
		return m_strSequenceName;
	};
	const _float GetProgress() const {
		return m_fCurFrame / m_fFrame_Total;
	};
	const _float GetTotalFrame() const {
		return m_fFrame_Total;
	};
	const _float4x4 GetMatTrans() const {
		return m_pBaseTransform->Get_WorldMatrix();
	};
	shared_ptr<CTransform> GetTransPointer() {
		return m_pBaseTransform;
	};
	shared_ptr<CTransform> GetActionTransPointer(_uint iIndex) {
		return m_InitTrans_List[iIndex];
	};

private:
	_int		m_iMySequenceIndex = -1;
	string		m_strSequenceName;
	_float		m_fFrame_Total = 1.f;

private:	// For play
	_float		m_fCurFrame = 0.f;

private:
	shared_ptr<CCine_Camera>						m_pCineCam = nullptr;
	const map<string, shared_ptr<CCine_Actor>>*		m_pActors;

	vector<ActionInfo>								m_ActionList;
	vector<CineCamInfo>								m_CamInfoList;
	vector<EffectInfo>								m_EffectList;

	vector<CineCallback>							m_CallbackList;
	vector<function<void()>>						m_LogicCallback;

	vector<ConvCallback>							m_ConversationList;

private:
	shared_ptr<CTransform>						m_pBaseTransform = nullptr;
	vector<shared_ptr<CTransform>>				m_InitTrans_List;

};

END