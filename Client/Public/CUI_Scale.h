#pragma once
#include "CUI_Animation.h"

BEGIN(Client)

class CUI_Scale : public CUI_Animation
{
public:
	explicit CUI_Scale() {};
	virtual ~CUI_Scale() {};

public:
	virtual _bool Tick(_float fTimeDelta);
	virtual void OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner);

public:
	static shared_ptr<CUI_Scale> Create(_float3 vTargetScale, float fTime = 1.f);

protected:
	_float3 m_vTargetScale = _float3(0.f, 0.f, 0.f);
	_float3 m_vScale = _float3(0.f, 0.f, 0.f);

	_float m_fTime = 0.f;
	_float m_fElapsedTime = 0.f;
};
END
