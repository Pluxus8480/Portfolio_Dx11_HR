#include "CUI_Move.h"

_bool CUI_Move::Tick(_float fTimeDelta)
{
	if (!__super::Tick(fTimeDelta))
	{
		return false;
	}

	m_fElapsedTime += fTimeDelta;
	if(m_fElapsedTime >= m_fTime)
	{
		m_pOwner->Set_Pos(m_vTargetPostion.x, m_vTargetPostion.y, m_vTargetPostion.z);

		return false;
	}

	m_pOwner->Add_Pos(m_vDirection.x * fTimeDelta / m_fTime, m_vDirection.y * fTimeDelta / m_fTime);

	return true;
}

void CUI_Move::OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner)
{
	__super::OnPlayAnimationEvent(pOwner);

	if (m_pOwner != nullptr)
	{
		auto CurrentPos = m_pOwner->Get_Pos();

		m_vDirection = _float3(m_vTargetPostion.x - CurrentPos.x, m_vTargetPostion.y - CurrentPos.y, m_vTargetPostion.z - CurrentPos.z);
	}
}

shared_ptr<CUI_Move> CUI_Move::Create(_float3 vTargetPostion, float fTime)
{
	shared_ptr<CUI_Move> pInstance = make_shared<CUI_Move>();
	if (pInstance != nullptr)
	{
		pInstance->m_vTargetPostion = vTargetPostion;
		pInstance->m_fTime = fTime;
	}

	return pInstance;
}
