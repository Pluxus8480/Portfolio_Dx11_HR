#pragma once
#include "Client_Defines.h"
#include "Effect_Manager.h"
#include "GameObject.h"

BEGIN(Client)

class CGimmickBase abstract : public CGameObject
{
public:
	struct COMMONOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_float4x4 worldMatrix;
	};

protected:
	CGimmickBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGimmickBase(const CGimmickBase& rhs);
public:
	virtual ~CGimmickBase();

public:
	void				Set_GimmickID(string strGimmickID)	{ m_strGimmickID = strGimmickID; }
	string				Get_GimmickID() const				{ return m_strGimmickID; }
	
	const vector<shared_ptr<CGimmickBase>>& Get_Relatives() const { return m_vecRelatives; }

	_bool Get_Done() const { return m_bDone; }
	_bool Get_Executed() const { return m_bExecuted; }

public:
	virtual void Execute(_bool bExecute = true) { m_bExecuted = bExecute; }
	virtual void Done() { m_bExecuted = false; m_bDone = true; }

	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) {}
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) {}
	
	virtual void Edit_Attribute() {}
	virtual void Save_Data(ofstream& os);
	virtual void Load_Data(ifstream& is);

	void Link_Relatives();
	void Set_Other(shared_ptr<CGameObject> pOther);

	void Update_Relatives();

	virtual void SetUp_CinematicCallBack() {}

protected:
	string m_strGimmickID = { 0 };
	vector<shared_ptr<CGimmickBase>> m_vecRelatives;
	vector<string> m_vecGimmickIDs;
	weak_ptr<CGameObject> m_pOther;

	_bool m_bExecuted = { false };
	_bool m_bDone = { false };

protected:
	void Save_Relatives(ofstream& os);
	void Load_Relatives(ifstream& is);

public:
	void Free();
};

END