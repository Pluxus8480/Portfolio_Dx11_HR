#pragma once
#include "CUI_Animation.h"

BEGIN(Client)

class CUI_Move : public CUI_Animation
{
public:
	explicit CUI_Move() {};
	virtual ~CUI_Move() {};

public:
	virtual _bool Tick(_float fTimeDelta);
	virtual void OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner);

public:
	static shared_ptr< CUI_Move> Create(_float3 vTargetPostion, float fTime = 1.f);

protected:
	_float3 m_vTargetPostion = _float3(0.f, 0.f, 0.f);
	_float3 m_vDirection = _float3(0.f, 0.f, 0.f);
	_float m_fTime = 0.f;

	_float m_fElapsedTime = 0.f;
};

END