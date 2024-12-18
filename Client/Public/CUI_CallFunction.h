#pragma once
#include "CUI_Animation.h"

BEGIN(Client)

class CUI_CallFunction : public CUI_Animation
{
public:
	explicit CUI_CallFunction() {};
	virtual ~CUI_CallFunction() {};

public:
	virtual _bool Tick(_float fTimeDelta);

public:
	static shared_ptr<CUI_CallFunction> Create(function<void()> CallFunction, float fTime = 1.f);

protected:
	std::function<void()> m_CallFunction;

	_float m_fTime = 0.f;
	_float m_fElapsedTime = 0.f;
};

END
