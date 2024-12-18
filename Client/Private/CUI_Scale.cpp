#include "CUI_Scale.h"

_bool CUI_Scale::Tick(_float fTimeDelta)
{
	if (!__super::Tick(fTimeDelta))
	{
		return false;
	}

	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= m_fTime)
	{
		m_pOwner->Set_Scale(m_vTargetScale.x, m_vTargetScale.y, m_vTargetScale.z);

		return false;
	}

	m_pOwner->Add_Scale(m_vScale.x * fTimeDelta / m_fTime, m_vScale.y * fTimeDelta / m_fTime);

	return true;
}

void CUI_Scale::OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner)
{
	__super::OnPlayAnimationEvent(pOwner);

	if (m_pOwner != nullptr)
	{
		auto CurrentScale = m_pOwner->Get_Scale();

		m_vScale = _float3(m_vTargetScale.x - CurrentScale.x, m_vTargetScale.y - CurrentScale.y, m_vTargetScale.z - CurrentScale.z);
	}
}

shared_ptr<CUI_Scale> CUI_Scale::Create(_float3 vTargetScale, float fTime)
{
	shared_ptr<CUI_Scale> pInstance = make_shared<CUI_Scale>();
	if (pInstance != nullptr)
	{
		pInstance->m_vTargetScale = vTargetScale;
		pInstance->m_fTime = fTime;
	}

	return pInstance;
}
