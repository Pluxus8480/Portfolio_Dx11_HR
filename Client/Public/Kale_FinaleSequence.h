#include "Monster_Kale.h"
#include "Chai.h"

#pragma once
class CKale_FinaleSequence
{
public:
	enum ACTORS
	{
		CHAI,
		KALE,
		PEPPERMINT,
		MACARON,
		KORSICA,
		EIGHT_O_EIGHT,
		ACTOR_END,
	};

public:
	CKale_FinaleSequence(shared_ptr<CMonster_Kale> pKale);
	~CKale_FinaleSequence();
	void Update(_float fTimeDelta);

	void Sequence_Start(shared_ptr<CGameObject> p808);
	void Proceed_Anim();
private:
	weak_ptr<CMonster_Kale> m_pKale;
	weak_ptr<CChai> m_pChai;
	weak_ptr<CModel> m_ArrModels[ACTOR_END];
	_int m_iMaxSequence = 8;
	_int m_iCurrSequence = 0;
	vector<ACTORS> m_vecSequenceEnder;

	vector<ACTORS> m_vecCamOwner;
	vector<_float> m_vecCamOwnerToChai;
	
	string m_strEventTag;//
	_bool m_bStart = false;
};
