#include "Kale_FinaleSequence.h"
#include "PlayerManager.h"
#include "Model.h"
#include "Player_Camera.h"
#include "Korsica.h"
#include "Peppermint.h"
#include "Macaron.h"
#include "GameInstance.h"
#include "808_Playable.h"

CKale_FinaleSequence::CKale_FinaleSequence(shared_ptr<CMonster_Kale> pKale)
{
	m_pKale = pKale; 
	
}

CKale_FinaleSequence::~CKale_FinaleSequence()
{
}

void CKale_FinaleSequence::Update(_float fTimeDelta)
{
	if(!(m_iCurrSequence < m_iMaxSequence) || !m_bStart)
		return;
	if (m_vecCamOwnerToChai[m_iCurrSequence] != 0.f)
	{
		_float fTP = m_ArrModels[m_vecSequenceEnder[m_iCurrSequence]].lock()->Get_Current_Track_Position();
		if (m_vecCamOwnerToChai[m_iCurrSequence] <= fTP && m_vecCamOwnerToChai[m_iCurrSequence] + 2.5f >= fTP)
		{
			m_pChai.lock()->Get_Player_Camera()->Start_Following_Bone(m_ArrModels[CHAI].lock()->Get_Owner()->Get_Transform()->Get_WorldMatrix_Ptr(),
				const_cast<_float4x4*>(m_ArrModels[CHAI].lock()->Get_BoneMatrixPtr("prop_02")));
		}
	}

	if (m_ArrModels[m_vecSequenceEnder[m_iCurrSequence]].lock()->Get_Finished())
	{
		Proceed_Anim();
	}
}

void CKale_FinaleSequence::Sequence_Start(shared_ptr<CGameObject> p808)
{
	m_bStart = true;
	auto pPM = PLAYERMANAGER;
	auto pKale = m_pKale.lock();
	m_pChai = pPM->Get_Player();
	_matrix OriginMat =	m_pChai.lock()->Get_Transform()->Get_WorldMatrix_XMMat();
	m_ArrModels[CHAI] = static_pointer_cast<CModel>(m_pChai.lock()->Find_Component(L"Com_Model"));
	m_ArrModels[PEPPERMINT] = static_pointer_cast<CModel>(pPM->Get_Assist(L"Peppermint")->Find_Component(L"Com_Model"));
	m_ArrModels[MACARON] = static_pointer_cast<CModel>(pPM->Get_Assist(L"Macaron")->Find_Component(L"Com_Model"));
	m_ArrModels[KORSICA] = static_pointer_cast<CModel>(pPM->Get_Assist(L"Korsica")->Find_Component(L"Com_Model"));
	m_ArrModels[KALE] = pKale->Get_Model();
	m_ArrModels[EIGHT_O_EIGHT] = static_pointer_cast<CModel>(p808->Find_Component(L"Com_Model"));
	static_pointer_cast<C808_Playable>(p808)->Render_On();

	m_iCurrSequence = -1;
	m_iMaxSequence = 8;
	m_strEventTag = "EV1250_80_";
	static_pointer_cast<CPeppermint>(m_ArrModels[PEPPERMINT].lock()->Get_Owner())->Render_On();
	m_ArrModels[PEPPERMINT].lock()->Get_Owner()->Get_Transform()->Set_WorldMatrix(OriginMat);
	static_pointer_cast<CMacaron>(m_ArrModels[MACARON].lock()->Get_Owner())->Render_On();
	m_ArrModels[MACARON].lock()->Get_Owner()->Get_Transform()->Set_WorldMatrix(OriginMat);
	static_pointer_cast<CKorsica>(m_ArrModels[KORSICA].lock()->Get_Owner())->Render_On();
	m_ArrModels[KORSICA].lock()->Get_Owner()->Get_Transform()->Set_WorldMatrix(OriginMat);

	m_pKale.lock()->Get_Transform()->Set_WorldMatrix(OriginMat);

	m_vecSequenceEnder.resize(m_iMaxSequence);
	m_vecSequenceEnder[0] = CHAI;
	m_vecSequenceEnder[1] = CHAI;
	m_vecSequenceEnder[2] = MACARON;
	m_vecSequenceEnder[3] = CHAI;
	m_vecSequenceEnder[4] = CHAI;
	m_vecSequenceEnder[5] = CHAI;
	m_vecSequenceEnder[6] = CHAI;
	m_vecSequenceEnder[7] = CHAI;

	m_vecCamOwner.resize(m_iMaxSequence); m_vecCamOwnerToChai.resize(m_iMaxSequence);
	m_vecCamOwner[0] = CHAI;
	m_vecCamOwner[1] = CHAI;
	m_vecCamOwner[2] = MACARON;
	m_vecCamOwner[3] = MACARON; m_vecCamOwnerToChai[3] = 200.f;
	m_vecCamOwner[4] = CHAI;
	m_vecCamOwner[5] = KORSICA; m_vecCamOwnerToChai[5] = 210.f;
	m_vecCamOwner[6] = CHAI;
	m_vecCamOwner[7] = PEPPERMINT; m_vecCamOwnerToChai[7] = 210.f;
	
	Proceed_Anim();
}

void CKale_FinaleSequence::Proceed_Anim()
{
	++m_iCurrSequence;
	if (m_iCurrSequence >= m_iMaxSequence)
		return;
	string strNextAnimTag = m_strEventTag + string("00") + to_string(m_iCurrSequence);
	for (auto& pModels : m_ArrModels)
	{
		if (pModels.expired())
			continue;
		auto& vecTags = pModels.lock()->Get_Animation_List();
		for (auto& Tag : vecTags)
		{
			if (string::npos != Tag->Get_AnimName().find(strNextAnimTag))
			{
				pModels.lock()->Set_AnimIndex(Tag->Get_AnimName(), false);
			}
		}
	}
	string BoneTag;
	if (m_vecCamOwner[m_iCurrSequence] == CHAI)
	{
		BoneTag = "prop_02";
	}
	else
	{
		BoneTag = "prop_02";
	}
	m_pChai.lock()->Get_Player_Camera()->Start_Following_Bone(m_ArrModels[m_vecCamOwner[m_iCurrSequence]].lock()->Get_Owner()->Get_Transform()->Get_WorldMatrix_Ptr(),
		const_cast<_float4x4*>(m_ArrModels[m_vecCamOwner[m_iCurrSequence]].lock()->Get_BoneMatrixPtr(BoneTag.c_str())));
}
