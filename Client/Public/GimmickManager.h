#pragma once
#include "Client_Defines.h"

BEGIN(Client)

class CGimmickManager
{
	DECLARE_SINGLETON(CGimmickManager)

private:
	CGimmickManager();
public:
	~CGimmickManager();

public:
	shared_ptr<CGimmickBase> Get_GimmckObj(string strGimmickObjID);
	void Add_GimmickObj(shared_ptr<CGimmickBase> pGimmickObj);

	void Clear_GimmickObj();

	void SetUp_CinematicCallBack();

private:
	map<string, shared_ptr<class CGimmickBase>> m_mapGimmickObjs;

public:
	void Free();
};

END