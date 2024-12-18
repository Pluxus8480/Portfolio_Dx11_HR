#include "CUI_CallFunction.h"

_bool CUI_CallFunction::Tick(_float fTimeDelta)
{
	if (!__super::Tick(fTimeDelta))
	{
		return false;
	}

	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= m_fTime)
	{
		if (m_CallFunction != nullptr)
		{
			m_CallFunction();
			m_CallFunction = nullptr;
		}

		return false;
	}

	return true;
}

shared_ptr<CUI_CallFunction> CUI_CallFunction::Create(function<void()> CallFunction, float fTime)
{
	shared_ptr<CUI_CallFunction> pInstance = make_shared<CUI_CallFunction>();
	if (pInstance != nullptr)
	{
		pInstance->m_CallFunction = CallFunction;
		pInstance->m_fTime = fTime;
	}

	return pInstance;
}
