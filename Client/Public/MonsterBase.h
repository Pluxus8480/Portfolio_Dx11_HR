#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMonsterBase abstract : public CGameObject
{
public:
	struct Monster_DESC {
		wstring strObjID;
		string strDesc;
		_float4x4 matTransform;
	};

protected:
	CMonsterBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBase(const CMonsterBase& rhs);
public:
	virtual ~CMonsterBase();

protected:
	_matrix NormalizeMatrix(const _float4x4* pMatrix);

// Interpret input option(string type)
private:
	void splitString(const string& str, _Out_ vector<string>& vecResult);
	virtual void InterpretString(const string& strOption) {};

public:
	_bool IsBeatAble();
	virtual _bool IsNeedToMove() { return false; };

public:
	virtual void SetBarrierOn();
	virtual void SetBarrierOff();
protected:
	_bool m_isBarrierOn = false;


protected:
	_float m_fCurHp;
public:
	_bool IsDeadState() { return m_fCurHp <= 0.f; };

// For control by external objects
public:
	virtual void SetPos_Hard(_fvector vPos) {};
	virtual void MoveToPos(_fvector vTargetPos) {};
};

END