#include "CUI_Sequence.h"

_bool CUI_Sequence::Tick(_float fTimeDelta)
{
	if (!__super::Tick(fTimeDelta))
	{
		return false;
	}

	if (!m_AnimationList.front()->Tick(fTimeDelta))
	{
		m_AnimationList.pop_front();
	}

	if (m_AnimationList.size() <= 0)
	{
		return false;
	}

	return true;
}

void CUI_Sequence::OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner)
{
	__super::OnPlayAnimationEvent(pOwner);

	for (auto& iter : m_AnimationList)
	{
		iter->OnPlayAnimationEvent(pOwner);
	}
}

shared_ptr<CUI_Sequence> CUI_Sequence::Create(shared_ptr<CUI_Animation> pInAnimations, ...)
{
	shared_ptr<CUI_Sequence> pInstance = make_shared<CUI_Sequence>();
	if (pInstance != nullptr)
	{
		va_list argptr;
		shared_ptr<CUI_Animation> pAnimation;

		va_start(argptr, pInAnimations);		
		pAnimation = pInAnimations;
		while(pAnimation != nullptr)
		{
			pInstance->m_AnimationList.emplace_back(pAnimation);

			pAnimation = va_arg(argptr, shared_ptr<CUI_Animation>);
			if (dynamic_pointer_cast<CUI_SequenceEnd>(pAnimation) != nullptr)
			{
				break;
			}
		}

		va_end(argptr);
	}

	return pInstance;
}

shared_ptr<CUI_SequenceEnd> CUI_SequenceEnd::Create()
{
	return make_shared<CUI_SequenceEnd>();
}
