#pragma once
#include "CUI_Animation.h"

BEGIN(Client)

class CUI_Sequence : public CUI_Animation
{
public:
	explicit CUI_Sequence() {};
	virtual ~CUI_Sequence() {};

public:
	virtual _bool Tick(_float fTimeDelta);
	virtual void OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner);

public:
	static shared_ptr<CUI_Sequence> Create(shared_ptr<CUI_Animation> pInAnimations, ...);

protected:
	std::list<shared_ptr<CUI_Animation>> m_AnimationList;
	
};

class CUI_SequenceEnd : public CUI_Animation
{
public:
	explicit CUI_SequenceEnd() {};
	virtual ~CUI_SequenceEnd() {};

	static shared_ptr<CUI_SequenceEnd> Create();
};

END
