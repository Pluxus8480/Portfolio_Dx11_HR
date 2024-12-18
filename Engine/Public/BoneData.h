#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

struct ENGINE_DLL BONE_DATA
{
    string szName;
    _float4x4 OffsetMatrix;

public:
#ifdef _DEBUG
    HRESULT Make_BoneData(aiBone* In_pAIBone);
#endif
    void    Bake_Binary(ofstream& os);
    void    Load_FromBinary(ifstream& is);
};

END