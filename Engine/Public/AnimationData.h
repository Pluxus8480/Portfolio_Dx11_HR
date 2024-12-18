#pragma once

#include "Engine_Defines.h"
#include "ChannelData.h"

BEGIN(Engine)
struct ENGINE_DLL ANIMATION_DATA
{
	vector<shared_ptr<CHANNEL_DATA>> Channel_Datas;

	string szName;
	_uint  iNumChannels = 0;
	_float fDuration = 0.f;
	_float fTickPerSecond = 0.f;

	_bool  isRootAnim = false;
	_bool  isForcedNonRootAnim = false;
	vector<_float> Callback_Frames;

	vector<_float> Frame_Effects;
	vector<string> EffectList;
	vector<_float4x4> Effect_Trans_List;
	vector<_bool> Effect_IsFollow_List;
	vector<_bool> Effect_IsBillBoard_List;

	vector<_float> Frame_Sounds;
	vector<wstring> SoundList;
	vector<_float> Sound_Volume_List;
#ifdef _DEBUG
	HRESULT Make_AnimationData(aiAnimation* In_pAiAnimation, _float In_fSpeed = 1.f);
#endif
public:
	void Bake_Binary(const string& strModelFilePath);
	void Load_FromBinary(ifstream& is, const string& strFolderPath, const string& strFileName);
	void Bake_ReverseAnimation(shared_ptr<ANIMATION_DATA>& Out_AnimationData);
};
END