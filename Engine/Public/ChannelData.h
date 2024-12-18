#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)
struct ENGINE_DLL CHANNEL_DATA
{

    string                              szNodeName;
    _uint                               iNumScalingKeys;
    _uint                               iNumRotationKeys;
    _uint                               iNumPositionKeys;
    _uint                               iNumKeyframes;

    vector<KEYFRAME>                    tKeyFrames;
#ifdef _DEBUG
    HRESULT Make_ChannelData(aiNodeAnim* In_pChannel);
#endif
public:
    void Bake_Binary(ofstream& os);
    void Load_FromBinary(ifstream& is);
    void Bake_ReverseChannel(shared_ptr<CHANNEL_DATA>& Out_ChannelData, const _float In_fDuration);
};
END