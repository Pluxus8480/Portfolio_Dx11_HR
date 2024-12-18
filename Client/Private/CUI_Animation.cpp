#include "CUI_Animation.h"

CUI_Animation::CUI_Animation()
{
}

CUI_Animation::~CUI_Animation()
{
	m_pOwner = nullptr;
}

_bool CUI_Animation::Tick(_float fTimeDelta)
{
	if (m_pOwner == nullptr)
	{
		return false;
	}

	return true;
}

void CUI_Animation::OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner)
{
	m_pOwner = dynamic_pointer_cast<CUi_Sprite>(pOwner);
}
