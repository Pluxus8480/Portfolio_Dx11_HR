#include "..\Public\Animation.h"
#include "GameInstance.h"

#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration{ rhs.m_fDuration }
	, m_fTickPerSecond{ rhs.m_fTickPerSecond }
	, m_fCurrentTrackPosition{ rhs.m_fCurrentTrackPosition }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_Channels{ rhs.m_Channels }
	, m_isFinished{ rhs.m_isFinished }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
	, m_isRootAnim{ rhs.m_isRootAnim }
	, m_isForcedNonRootAnim{ rhs.m_isForcedNonRootAnim }
	, m_FrameCallbackList{ rhs.m_FrameCallbackList }
	, m_Frame_Effect_List{ rhs.m_Frame_Effect_List }
	, m_EffectList{ rhs.m_EffectList }
	, m_Effect_Trans_List{ rhs.m_Effect_Trans_List }
	, m_Effect_IsFollow_List{ rhs.m_Effect_IsFollow_List }
	, m_Effect_IsBillBoard_List{ rhs.m_Effect_IsBillBoard_List }
	, m_Frame_Sound_List{ rhs.m_Frame_Sound_List }
	, m_Sound_List{ rhs.m_Sound_List }
	, m_Sound_Volume{ rhs.m_Sound_Volume }
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto fFrame : m_FrameCallbackList)
		m_CallbackList.emplace_back([]() {});
	m_CallbackList.emplace_back([]() {});

	m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
	m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
}

CAnimation::~CAnimation()
{
	Free();
}
#ifdef _DEBUG
HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<shared_ptr<CBone>>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;

	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		shared_ptr<CChannel> pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}
#endif
HRESULT CAnimation::Initialize(shared_ptr<ANIMATION_DATA> pAIAnimation, const vector<shared_ptr<class CBone>>& Bones)
{
	strcpy(m_szName, pAIAnimation->szName.c_str());

	m_fDuration = (_float)pAIAnimation->fDuration;

	m_fTickPerSecond = (_float)pAIAnimation->fTickPerSecond;

	m_iNumChannels = pAIAnimation->iNumChannels;

	for (_float fFrame : pAIAnimation->Callback_Frames)
		m_FrameCallbackList.emplace_back(fFrame);

	m_isRootAnim = pAIAnimation->isRootAnim;
	m_isForcedNonRootAnim = pAIAnimation->isForcedNonRootAnim;

	// For effect
	m_Frame_Effect_List = pAIAnimation->Frame_Effects;
	m_EffectList = pAIAnimation->EffectList;
	m_Effect_Trans_List = pAIAnimation->Effect_Trans_List;
	m_Effect_IsFollow_List = pAIAnimation->Effect_IsFollow_List;
	m_Effect_IsBillBoard_List = pAIAnimation->Effect_IsBillBoard_List;

	// For sound
	m_Frame_Sound_List = pAIAnimation->Frame_Sounds;
	m_Sound_List = pAIAnimation->SoundList;
	m_Sound_Volume = pAIAnimation->Sound_Volume_List;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		shared_ptr<CChannel> pChannel = CChannel::Create(pAIAnimation->Channel_Datas[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<CBone>>& Bones, _bool isLoop)
{
	_float fTickTime = m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier;
	for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
	{
		_float fFrame = m_FrameCallbackList[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
			m_CallbackList[i]();
	}
	for (_uint i = 0; i < m_Frame_Sound_List.size(); ++i)
	{
		_float fFrame = m_Frame_Sound_List[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
		{
			GAMEINSTANCE->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, m_Sound_List[i], m_Sound_Volume[i]);
			GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, m_Sound_List[i]);
		}
	}

	m_ReservedEffects.clear();
	for (_uint i = 0; i < m_Frame_Effect_List.size(); ++i)
	{
		_float fFrame = m_Frame_Effect_List[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
			m_ReservedEffects.push_back(i);
	}

	m_fCurrentTrackPosition += fTickTime;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_CallbackList.back()();	// Last Callback Active

		m_isFinished = true;

		if (true == isLoop)
		{
			m_isFinished = false;
			m_fCurrentTrackPosition = 0.f;

			m_isLooped = true;
			m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
			m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
		}

		if (true == m_isFinished)
			return;
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
	}

}

void CAnimation::Update_TransformationMatrix_Split(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop, const vector<_uint>& SplitIdx, _bool& OutFinished, _bool& OutLooped)
{
	if (!m_bMoved)
	{
		m_bMoved = true;
		_float fTickTime = m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier;
		for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
		{
			_float fFrame = m_FrameCallbackList[i];
			if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
				m_CallbackList[i]();
		}

		m_ReservedEffects.clear();
		for (_uint i = 0; i < m_Frame_Effect_List.size(); ++i)
		{
			_float fFrame = m_Frame_Effect_List[i];
			if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
				m_ReservedEffects.push_back(i);
		}
		m_fCurrentTrackPosition += fTickTime;
	}
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_CallbackList.back()();	// Last Callback Active

		m_isFinished = true;

		if (true == isLoop)
		{
			m_isFinished = false;
			m_fCurrentTrackPosition = 0.f;

			m_isLooped = true;
			m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
			m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
			OutLooped = true;
		}
		OutFinished = m_isFinished;
		if (true == m_isFinished)
			return;
	}
	vector<_bool> Check;
	Check.resize(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		for (auto idx : SplitIdx)
		{
			if (m_Channels[i]->Get_BoneIndex() == idx)
			{
				Check[i] = true;
				break;
			}
		}
	}
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if (!Check[i])
			continue;
		m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
	}
}

void CAnimation::Update_Transformation_Aperture(_float fTimeDelta, shared_ptr<CAnimation> pTargetAnim, _float fFramePos, const vector<shared_ptr<class CBone>>& Bones)
{
	// Active setting callbacks
	_float fTickTime = m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier;
	for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
	{
		_float fFrame = m_FrameCallbackList[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
			m_CallbackList[i]();
	}
	for (_uint i = 0; i < m_Frame_Sound_List.size(); ++i)
	{
		_float fFrame = m_Frame_Sound_List[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
		{
			GAMEINSTANCE->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, m_Sound_List[i], m_Sound_Volume[i]);
			GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, m_Sound_List[i]);
		}
	}

	m_ReservedEffects.clear();
	for (_uint i = 0; i < m_Frame_Effect_List.size(); ++i)
	{
		_float fFrame = m_Frame_Effect_List[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
			m_ReservedEffects.push_back(i);
	}

	m_fCurrentTrackPosition += fTickTime;
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if (m_iRootBoneIndex == m_Channels[i]->Get_BoneIndex())
		{
			m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
		}
		else
		{
			auto TargetFrame = pTargetAnim->m_Channels[i]->Get_Frame(fFramePos);
			m_Channels[i]->Update_Linear_Aperture(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i], TargetFrame, m_fAperture);
		}
	}
}

void CAnimation::Update_Transformation_Aperture_Split(_float fTimeDelta, shared_ptr<CAnimation> pTargetAnim, _float fFramePos, const vector<shared_ptr<class CBone>>& Bones, const vector<_uint>& SplitIdx)
{
	_float fTickTime = m_fTickPerSecond * fTimeDelta * m_fAnimationMultiplier;
	for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
	{
		_float fFrame = m_FrameCallbackList[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
			m_CallbackList[i]();
	}

	m_ReservedEffects.clear();
	for (_uint i = 0; i < m_Frame_Effect_List.size(); ++i)
	{
		_float fFrame = m_Frame_Effect_List[i];
		if (m_fCurrentTrackPosition <= fFrame && m_fCurrentTrackPosition + fTickTime > fFrame)
			m_ReservedEffects.push_back(i);
	}

	m_fCurrentTrackPosition += fTickTime;
	vector<_bool> Check;
	Check.resize(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		for (auto idx : SplitIdx)
		{
			if (m_Channels[i]->Get_BoneIndex() == idx)
			{
				Check[i] = true;
				break;
			}
		}
	}
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if (!Check[i])
			continue;
		if (m_iRootBoneIndex == m_Channels[i]->Get_BoneIndex())
		{
			m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
		}
		else
		{
			auto TargetFrame = pTargetAnim->m_Channels[i]->Get_Frame(fFramePos);
			m_Channels[i]->Update_Linear_Aperture(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i], TargetFrame, m_fAperture);
		}
	}
}

void CAnimation::Update_Transform_Frame(_float fFrame, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop, _bool isWithEffect)
{
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update(0.f, Bones, &m_CurrentKeyFrameIndices[i]);
	}

	if (isWithEffect)
	{
		for (_uint i = 0; i < m_FrameCallbackList.size(); ++i)
		{
			_float fCallbackFrame = m_FrameCallbackList[i];
			if (m_fCurrentTrackPosition <= fCallbackFrame && fFrame > fCallbackFrame)
				m_CallbackList[i]();
		}
		for (_uint i = 0; i < m_Frame_Sound_List.size(); ++i)
		{
			_float fCallbackFrame = m_Frame_Sound_List[i];
			if (m_fCurrentTrackPosition <= fCallbackFrame && fFrame > fCallbackFrame)
			{
				GAMEINSTANCE->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, m_Sound_List[i], m_Sound_Volume[i]);
				GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, m_Sound_List[i]);
			}
		}

		m_ReservedEffects.clear();
		for (_uint i = 0; i < m_Frame_Effect_List.size(); ++i)
		{
			_float fEffectFrame = m_Frame_Effect_List[i];
			if (m_fCurrentTrackPosition <= fEffectFrame && fFrame > fEffectFrame)
				m_ReservedEffects.push_back(i);
		}
	}

	m_fCurrentTrackPosition = fFrame;
	if (m_fCurrentTrackPosition >= m_fDuration)
		return;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
	}
}

void CAnimation::Reset()
{
	m_isFinished = false;
	m_fCurrentTrackPosition = 0;

	m_ReservedEffects.clear();	// Reset reserved effect callbacks

	for (size_t i = 0; i < m_iNumChannels; i++)
		m_CurrentKeyFrameIndices[i] = 0;

	m_RootAnimPreInfo.vPreRot = { 0.f,0.f,0.f,0.f };
	m_RootAnimPreInfo.vPreTrans = { 0.f,0.f,0.f,1.f };
}

HRESULT CAnimation::ExportAnimInfo(const string& strFolderPath)
{
	string strFilePath = strFolderPath;
	strFilePath.append(m_szName);
	strFilePath.append(".dat");

	ofstream binFile(strFilePath, ios::out | ios::binary);
	if (!binFile.is_open())
	{
		binFile.close();
		return E_FAIL;
	}

	// Rootanimation
	write_typed_data(binFile, m_isRootAnim);

	// Callback List
	_uint iCallbackSize = static_cast<_uint>(m_FrameCallbackList.size());
	write_typed_data(binFile, iCallbackSize);
	for (_float fFrameCallback : m_FrameCallbackList)
		write_typed_data(binFile, fFrameCallback);
	write_typed_data(binFile, m_isForcedNonRootAnim);

	iCallbackSize = static_cast<_uint>(m_Frame_Effect_List.size());
	write_typed_data(binFile, iCallbackSize);
	for (_uint i = 0; i < iCallbackSize; ++i)
	{
		write_typed_data(binFile, m_Frame_Effect_List[i]);
		write_typed_data(binFile, m_EffectList[i].size());
		binFile.write(&m_EffectList[i][0], m_EffectList[i].size());
		write_typed_data(binFile, m_Effect_Trans_List[i]);

		_bool isFollow = m_Effect_IsFollow_List[i];
		binFile.write(reinterpret_cast<const char*>(&isFollow), sizeof(_bool));

		_bool isBillBoard = m_Effect_IsBillBoard_List[i];
		binFile.write(reinterpret_cast<const char*>(&isBillBoard), sizeof(_bool));
	}

	binFile.close();

	return S_OK;
}

HRESULT CAnimation::ExportSoundInfo(const string& strFolderPath)
{
	string strFilePath = strFolderPath;
	strFilePath.append(m_szName);
	strFilePath.append(".snf");

	ofstream binFile(strFilePath, ios::out | ios::binary);
	if (!binFile.is_open())
	{
		binFile.close();
		return E_FAIL;
	}

	_uint iCallbackSize = static_cast<_uint>(m_Frame_Sound_List.size());
	write_typed_data(binFile, iCallbackSize);
	for (_uint i = 0; i < iCallbackSize; ++i)
	{
		write_typed_data(binFile, m_Frame_Sound_List[i]);
		write_typed_data(binFile, m_Sound_List[i].size());
		binFile.write(reinterpret_cast<const char*>(m_Sound_List[i].c_str()), m_Sound_List[i].size() * sizeof(_tchar));
		write_typed_data(binFile, m_Sound_Volume[i]);
	}

	return S_OK;
}

void CAnimation::PlayReservedCallbacks()
{
	for (_uint iIndex : m_ReservedEffects)
		m_EffectCaller(m_EffectList[iIndex], m_Effect_Trans_List[iIndex], m_Effect_IsFollow_List[iIndex], m_Effect_IsBillBoard_List[iIndex]);
}

void CAnimation::PushEffectCallback(_float fFrame, const string& strEffect, _float4x4 matTrans)
{
	m_Frame_Effect_List.emplace_back(fFrame);
	m_EffectList.emplace_back(strEffect);
	m_Effect_Trans_List.emplace_back(matTrans);
	m_Effect_IsFollow_List.emplace_back(true);
	m_Effect_IsBillBoard_List.emplace_back(false);

	SortEffectCallbacks();
}

void CAnimation::PopEffectCallback(_int iIndex)
{
	if (iIndex < 0 || iIndex >= m_Frame_Effect_List.size())
		return;

	m_Frame_Effect_List.erase(m_Frame_Effect_List.begin() + iIndex);
	m_EffectList.erase(m_EffectList.begin() + iIndex);
	m_Effect_Trans_List.erase(m_Effect_Trans_List.begin() + iIndex);
	m_Effect_IsFollow_List.erase(m_Effect_IsFollow_List.begin() + iIndex);
	m_Effect_IsBillBoard_List.erase(m_Effect_IsBillBoard_List.begin() + iIndex);
}

void CAnimation::SortEffectCallbacks()
{
	vector<size_t> indices(m_Frame_Effect_List.size());
	for (size_t i = 0; i < indices.size(); ++i) {
		indices[i] = i;
	}
	sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
		return m_Frame_Effect_List[a] < m_Frame_Effect_List[b];
		});

	vector<_float> sortedFrameList(m_Frame_Effect_List.size());
	vector<string> sortedNameList(m_EffectList.size());
	vector<_float4x4> sortedTransList(m_Effect_Trans_List.size());
	vector<_bool> sortedIsFollowList(m_Effect_IsFollow_List.size());
	vector<_bool> sortedIsBillBoard(m_Effect_IsBillBoard_List.size());
	for (size_t i = 0; i < indices.size(); ++i) {
		sortedFrameList[i] = m_Frame_Effect_List[indices[i]];
		sortedNameList[i] = m_EffectList[indices[i]];
		sortedTransList[i] = m_Effect_Trans_List[indices[i]];
		sortedIsFollowList[i] = m_Effect_IsFollow_List[indices[i]];
		sortedIsBillBoard[i] = m_Effect_IsBillBoard_List[indices[i]];
	}

	m_Frame_Effect_List = sortedFrameList;
	m_EffectList = sortedNameList;
	m_Effect_Trans_List = sortedTransList;
	m_Effect_IsFollow_List = sortedIsFollowList;
	m_Effect_IsBillBoard_List = sortedIsBillBoard;
}

void CAnimation::PushSoundCallback(_float fFrame, const string& strSound, _float fVolume)
{
	m_Frame_Sound_List.emplace_back(fFrame);
	m_Sound_List.emplace_back(wstring(strSound.begin(), strSound.end()));
	m_Sound_Volume.emplace_back(fVolume);

	SortSoundCallbacks();
}

void CAnimation::PopSoundCallback(_int iIndex)
{
	if (iIndex < 0 || iIndex >= m_Frame_Sound_List.size())
		return;

	m_Frame_Sound_List.erase(m_Frame_Sound_List.begin() + iIndex);
	m_Sound_List.erase(m_Sound_List.begin() + iIndex);
	m_Sound_Volume.erase(m_Sound_Volume.begin() + iIndex);
}

void CAnimation::SortSoundCallbacks()
{
	auto iSize = m_Frame_Sound_List.size();
	vector<size_t> indices(iSize);
	for (size_t i = 0; i < indices.size(); ++i) {
		indices[i] = i;
	}
	sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
		return m_Frame_Sound_List[a] < m_Frame_Sound_List[b];
		});

	vector<_float> sortedFrameList(iSize);
	vector<wstring> sortedNameList(iSize);
	vector<_float> sortedVolumeList(iSize);
	for (size_t i = 0; i < indices.size(); ++i) {
		sortedFrameList[i] = m_Frame_Sound_List[indices[i]];
		sortedNameList[i] = m_Sound_List[indices[i]];
		sortedVolumeList[i] = m_Sound_Volume[indices[i]];
	}
	m_Frame_Sound_List = sortedFrameList;
	m_Sound_List = sortedNameList;
	m_Sound_Volume = sortedVolumeList;
}
#ifdef _DEBUG
shared_ptr<CAnimation> CAnimation::Create(const aiAnimation* pAIAnimation, const vector<shared_ptr<class CBone>>& Bones)
{
	MAKE_SHARED_ENABLER(CAnimation);
	shared_ptr<CAnimation> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		assert(false);
	}

	return pInstance;
}
#endif
shared_ptr<CAnimation> CAnimation::Create(shared_ptr<ANIMATION_DATA> pAnimation, const vector<shared_ptr<class CBone>>& Bones)
{
	MAKE_SHARED_ENABLER(CAnimation);
	shared_ptr<CAnimation> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CAnimation> CAnimation::Clone()
{
	struct MakeSharedEnabler : public CAnimation
	{
		MakeSharedEnabler(const CAnimation& rhs) : CAnimation(rhs) { }
	};

	shared_ptr<CAnimation> pInstance = make_shared<MakeSharedEnabler>(*this);

	return pInstance;
}

void CAnimation::Free()
{

}
