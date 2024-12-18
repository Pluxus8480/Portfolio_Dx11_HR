#include "CUI_Frame.h"

_bool CUI_Frame::Tick(_float fTimeDelta)
{
	if (!__super::Tick(fTimeDelta))
	{
		return false;
	}

	if (m_iFrameStart == m_iFrameEnd)
	{
		m_pOwner->Set_TextureNum(m_iFrameStart);
		return false;
	}

	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= m_fTime)
	{
		m_pOwner->Set_TextureNum(m_iFrameFinish);
		
		return false;
	}

	_int iNextFrame = m_iFrameStart + (int)(m_fElapsedTime / (m_fTime / m_iFrameCount));

	if (m_iCurrentFrame != iNextFrame && iNextFrame <= m_iFrameEnd)
	{
			//	1Second 5Texture -> 0.2second = 1 / 5
		m_iCurrentFrame = iNextFrame;

		m_pOwner->Set_TextureNum(m_iCurrentFrame);
	}

	return true;
}

void CUI_Frame::OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner)
{
	__super::OnPlayAnimationEvent(pOwner);

	if (m_iFrameStart == m_iFrameEnd)
	{
		m_pOwner->Set_TextureNum(m_iFrameStart);
		return;
	}

	m_iFrameCount = m_iFrameEnd - m_iFrameStart + 2; // Finish => +1
}


shared_ptr<CUI_Frame> CUI_Frame::Create(_int iFrameStart, _int iFrameEnd, _float fTime, _int iFrameFinish)
{
	shared_ptr<CUI_Frame> pInstance = make_shared<CUI_Frame>();
	if (pInstance != nullptr)
	{
		pInstance->m_iFrameStart = iFrameStart;
		pInstance->m_iFrameEnd = iFrameEnd;
		pInstance->m_iFrameFinish = iFrameFinish;
		pInstance->m_fTime = fTime;
	}

	return pInstance;
}
