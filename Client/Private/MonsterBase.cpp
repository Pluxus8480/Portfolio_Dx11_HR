#include "MonsterBase.h"
#include "GameInstance.h"

#include "Beat_Manager.h"


CMonsterBase::CMonsterBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
	: CGameObject{ rhs }
{
}

CMonsterBase::~CMonsterBase()
{
}

_matrix CMonsterBase::NormalizeMatrix(const _float4x4* pMatrix)
{
    _matrix matResult = XMLoadFloat4x4(pMatrix);
    matResult.r[0] = XMVector3Normalize(matResult.r[0]);
    matResult.r[1] = XMVector3Normalize(matResult.r[1]);
    matResult.r[2] = XMVector3Normalize(matResult.r[2]);
    return matResult;
}

_bool CMonsterBase::IsBeatAble()
{
    _float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
    _float fBeatTime = CBeat_Manager::Get_Instance()->Get_Timer();
    return fBeatTime < fBeat * 0.05f || fBeatTime > fBeat * 0.95f;
}

void CMonsterBase::SetBarrierOn()
{
    m_isBarrierOn = true;
}

void CMonsterBase::SetBarrierOff()
{
    m_isBarrierOn = false;
}

void CMonsterBase::splitString(const string& str, vector<string>& vecResult)
{
    stringstream ss(str);
    string token;
    while (getline(ss, token, '|')) {
        vecResult.push_back(move(token));
    }
}