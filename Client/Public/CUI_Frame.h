#pragma once
#include "CUI_Animation.h"

BEGIN(Client)
class CUI_Frame : public CUI_Animation
{
public:
	explicit CUI_Frame() {};
	virtual ~CUI_Frame() {};

	virtual _bool Tick(_float fTimeDelta);
	virtual void OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner);

	static shared_ptr<CUI_Frame> Create(_int iFrameStart, _int iFrameEnd, _float fTime = 1.f, _int iFrameFinish = 0);

protected:
	_int m_iFrameStart = 0;
	_int m_iFrameEnd = 0;
	_int m_iFrameFinish = 0;
	_int m_iCurrentFrame = 0;
	_int m_iFrameCount = 0;

	_float m_fTime = 0.f;
	_float m_fElapsedTime = 0.f;
};
END
