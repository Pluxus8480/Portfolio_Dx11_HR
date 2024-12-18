#include "CCineSubTitleMgr.h"
#include "GameInstance.h"
#include "iostream"
#include "Cine_Manager.h"


IMPLEMENT_SINGLETON(CCineSubTitleMgr)

CCineSubTitleMgr::CCineSubTitleMgr()
{
}

CCineSubTitleMgr::~CCineSubTitleMgr()
{
	Free();
}

void CCineSubTitleMgr::Initialize()
{
	for (size_t i = 0; i < (_uint)CineType::End; ++i)
	{
		m_SubTitleVec[i].resize(100);
	}

	Initialize_CS0106();

	Initialize_CS0230();
	Initialize_CS0235();
	Initialize_CS0240();
	Initialize_CS0245();

	
	Initialize_CS0620();


	Initialize_CS0820();
	Initialize_CS0825();
	Initialize_CS0830();
	Initialize_CS0835();
	Initialize_CS0840();


	Initialize_CS1125();
	Initialize_CS1130();
	Initialize_EV1120();
	Initialize_EV1130();
	Initialize_EV1150();
	Initialize_EV1160();
	Initialize_EV1170();
	Initialize_EV1180();
	Initialize_EV1190();


	Initialize_CS1205();
	Initialize_CS1210();
	Initialize_CS1215();
	Initialize_CS1220();
	Initialize_CS1225();
	Initialize_EV1250();
}

void CCineSubTitleMgr::PriorityTick(_float fTimeDelta)
{
	//if (CCine_Manager::Get_Instance()->IsInGame())
	//{
	//	m_RenderFontTitleVec.clear();
	//}
}

void CCineSubTitleMgr::Tick(_float fTimeDelta)
{
}

void CCineSubTitleMgr::LateTick(_float fTimeDelta)
{
	for (auto& iter : m_RenderFontTitleVec)
		iter.m_fLifeTime -= fTimeDelta;
}

void CCineSubTitleMgr::Tick_Engine(_float fTimeDelta)
{
	PriorityTick(fTimeDelta);
	Tick(fTimeDelta);
	LateTick(fTimeDelta);
}

void CCineSubTitleMgr::Render()
{
	if (m_RenderFontTitleVec.size() >= 2)
	{
		while(m_RenderFontTitleVec.size() >= 2)
		{
			m_RenderFontTitleVec.erase(m_RenderFontTitleVec.begin());
		}
	}
	
	if (m_RenderFontTitleVec.size() != 0)
	{
		if (m_RenderFontTitleVec[0].m_fLifeTime > 0)
		{
			RenderFont(m_RenderFontTitleVec[0]);
		}
	}
}

void CCineSubTitleMgr::RenderStartSetting(CineType eType, _uint vecindex, _float Time)
{
	if ((_uint)CineType::End <= (_uint)eType)
	{
		cout << "SubTitleVec OutofRange" << endl;
		assert(false);
	}

	if (m_SubTitleVec[(_uint)eType].size() <= vecindex)
	{
		cout << "SubTitleVec OutofRange" << endl;
		assert(false);
	}

	m_SubTitleVec[(_uint)eType][vecindex].m_fLifeTime = Time;
	m_RenderFontTitleVec.emplace_back(m_SubTitleVec[(_uint)eType][vecindex]);
}

void CCineSubTitleMgr::Initialize_IntroCallBack()
{
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 5, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 6, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 10, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 4, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 11, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 5, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 12, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 6, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 13, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 7, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0106", 31, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0106, 8, Time);
		}
	);
}

void CCineSubTitleMgr::Initialize_RekkaCallBack()
{
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 1, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 1, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 8, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 2, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 14, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 3, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 15, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 4, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 16, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 5, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 17, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 6, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 21, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 7, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 21, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 8, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 23, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 9, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 23, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 10, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 24, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 11, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 26, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 12, Time);
		}
	);
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0230", 27, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0230, 13, Time);
		}
	);


	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0235", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0235, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0235", 19, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0235, 1, Time);
		}
	);


	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0240", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0240, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0240", 10, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0240, 1, Time);
		}
	);


	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0245", 4, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0245, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0245", 5, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0245, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0245", 6, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0245, 2, Time);
		}
	);
}

void CCineSubTitleMgr::Initialize_KorsicaCallBack()
{
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 0, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 3, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 4, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 4, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 5, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 2, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 6, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 3, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 7, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 4, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 8, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 5, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 9, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 6, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 10, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 6, 7, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 11, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0620", 9, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0620, 12, Time);
		}
	);
}

void CCineSubTitleMgr::Initialize_MimosaCallBack()
{
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 11, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 12, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 13, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 14, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 15, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 4, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 17, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 5, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 17, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 6, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 19, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 7, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 20, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 8, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0820", 21, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0820, 9, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0825", 1, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0825, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0825", 1, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0825, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0830", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0830, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0830", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0830, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0830", 3, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0830, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0830", 4, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0830, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0835", 1, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0835, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0840", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0840, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS0840", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS0840, 1, Time);
		}
	);
}

void CCineSubTitleMgr::Initialize_KaleStageCallBack()
{
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1130", 0, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1130, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1130", 0, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1130, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1150", 0, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1150, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1150", 1, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1150, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1180", 5, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1180, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1180", 5, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1180, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1180", 5, 2, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1180, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1180", 6, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1180, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1125", 1, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1125, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1125", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1125, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1125", 9, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1125, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1170", 0, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1170, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1170", 0, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1170, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("EV1170", 0, 2, [&](_float Time)
		{
			RenderStartSetting(CineType::EV1170, 2, Time);
		}
	);
}

void CCineSubTitleMgr::Initialize_KaleBossCallBack()
{
	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1205", 4, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1205, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1205", 5, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1205, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1205", 6, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1205, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1205", 7, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1205, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1205", 8, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1205, 4, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1205", 11, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1205, 5, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1205", 16, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1205, 6, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1210", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1210, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1210", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1210, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1210", 4, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1210, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1210", 8, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1210, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1210", 9, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1210, 4, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1215", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1215, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1215", 9, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1215, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1215", 10, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1215, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1215", 11, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1215, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1220", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1220, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1220", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1220, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1220", 10, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1220, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1225", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1225, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1225", 2, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1225, 0, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1225", 3, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1225, 1, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1225", 6, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1225, 2, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1225", 7, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1225, 3, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1225", 7, 1, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1225, 4, Time);
		}
	);

	CCine_Manager::Get_Instance()->AddConversation_Callback
	("CS1225", 16, 0, [&](_float Time)
		{
			RenderStartSetting(CineType::CS1225, 5, Time);
		}
	);
}

void CCineSubTitleMgr::Initialize_CS0106()
{
	CineType eType = CineType::CS0106;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"우왓!");
	Emplace_SubTitle(eType, 1, SpeakerType::SBR, 9999, L"좋아, 꼬맹이.&로봇 팔 LX-DUST75. 자성 폐기물 관리 기능.");
	Emplace_SubTitle(eType, 2, SpeakerType::Chai, 9999, L"잠깐... 자성 폐기물 관리 기능?");
	Emplace_SubTitle(eType, 3, SpeakerType::SBR, 9999, L"미확인 물체 경고. 불량품인 것 같다. 잡아들여!");
	Emplace_SubTitle(eType, 4, SpeakerType::SBR, 9999, L"그건 무기인가?");
	Emplace_SubTitle(eType, 5, SpeakerType::Chai, 9999, L"어.. 아닐걸?");
	Emplace_SubTitle(eType, 6, SpeakerType::SBR, 9999, L"저항하려나 본데.");
	Emplace_SubTitle(eType, 7, SpeakerType::Chai, 9999, L"잠깐, 뭐?");
	Emplace_SubTitle(eType, 8, SpeakerType::SBR, 9999, L"잡아!");
}

void CCineSubTitleMgr::Initialize_CS0230()
{
	CineType eType = CineType::CS0230;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"조용하네. 수상한 점은 전혀 없어");
	Emplace_SubTitle(eType, 1, SpeakerType::RekkaHost, 9999, L"소개합니다... 레카!");
	Emplace_SubTitle(eType, 2, SpeakerType::Rekka, 9999, L"성공하고 싶나? 생산적이고 싶나?&그렇다면 공격적으로 달려들어라!");
	Emplace_SubTitle(eType, 3, SpeakerType::Rekka, 9999, L"불량품 애송이 녀석, 여기서 도망치려 하다니,&아주 재촉을 해대는군.");
	Emplace_SubTitle(eType, 4, SpeakerType::Chai, 9999, L"뭘 재촉한다는 건데?");
	Emplace_SubTitle(eType, 5, SpeakerType::Rekka, 9999, L"이 무례한 녀석. 안 되겠어. 글러브를 벗어야겠군.");
	Emplace_SubTitle(eType, 6, SpeakerType::Chai, 9999, L"진짜 벗네...");
	Emplace_SubTitle(eType, 7, SpeakerType::Chai, 9999, L"이런 경우엔 어떻게 할 계획이었어, 페퍼민트?");
	Emplace_SubTitle(eType, 8, SpeakerType::Peppermint, 9999, L"글쎄... 싸우기?");
	Emplace_SubTitle(eType, 9, SpeakerType::Chai, 9999, L"이번 보수는 두배로 줘야 할거야.");
	Emplace_SubTitle(eType, 10, SpeakerType::Peppermint, 9999, L"보수라니 뭔 헛소리야.");
	Emplace_SubTitle(eType, 11, SpeakerType::Rekka, 9999, L"어이! 이 쇼의 주연은 나야.");
	Emplace_SubTitle(eType, 12, SpeakerType::Rekka, 9999, L"싸울 준비나 하라고!");
	Emplace_SubTitle(eType, 13, SpeakerType::Rekka, 9999, L"자! 와라!");
}

void CCineSubTitleMgr::Initialize_CS0235()
{
	CineType eType = CineType::CS0235;
	Emplace_SubTitle(eType, 0, SpeakerType::Rekka, 9999, L"불량품치고는 제법이군.");
	Emplace_SubTitle(eType, 1, SpeakerType::Rekka, 9999, L"레카는 충전을 좀 해야겠어!");
}

void CCineSubTitleMgr::Initialize_CS0240()
{
	CineType eType = CineType::CS0240;
	Emplace_SubTitle(eType, 0, SpeakerType::Rekka, 9999, L"회사의 명성에 흠집을 낼 순 없지!");
	Emplace_SubTitle(eType, 1, SpeakerType::Rekka, 9999, L"쓰레기통에 처박아 주마.");
}

void CCineSubTitleMgr::Initialize_CS0245()
{ 
	CineType eType = CineType::CS0245;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"이 싸움은 네 패배야, 빅 보스.");
	Emplace_SubTitle(eType, 1, SpeakerType::Rekka, 9999, L"내가 너 같은 애송이한테 질 거 같아?");
	Emplace_SubTitle(eType, 2, SpeakerType::Chai, 9999, L"응, 그럴 것 같은데.");

}

void CCineSubTitleMgr::Initialize_CS0620()
{
	CineType eType = CineType::CS0620;
	Emplace_SubTitle(eType, 0, SpeakerType::Korsica, 9999, L"뭐야!?");
	Emplace_SubTitle(eType, 1, SpeakerType::Chai, 9999, L"이건 오해야, 코르시카!");
	Emplace_SubTitle(eType, 2, SpeakerType::Chai, 9999, L"그냥 오해라고!");
	Emplace_SubTitle(eType, 3, SpeakerType::Korsica, 9999, L"시끄럽다!");
	Emplace_SubTitle(eType, 4, SpeakerType::Korsica, 9999, L"대체 뭐가 오해란 거지?");
	Emplace_SubTitle(eType, 5, SpeakerType::Chai, 9999, L"어... 난 그냥 대화하러 온 거야.");
	Emplace_SubTitle(eType, 6, SpeakerType::Korsica, 9999, L"내 입장은 보안장치들이 전했을 텐데.");
	Emplace_SubTitle(eType, 7, SpeakerType::Chai, 9999, L"하나같이 허접하던데.");
	Emplace_SubTitle(eType, 8, SpeakerType::CNMN, 9999, L"미스터 차이, 자극하지 마세요!");
	Emplace_SubTitle(eType, 9, SpeakerType::Peppermint, 9999, L"저 멍청한 녀석!");
	Emplace_SubTitle(eType, 10, SpeakerType::Macaron, 9999, L"이런, 큰일이야...");
	Emplace_SubTitle(eType, 11, SpeakerType::Korsica, 9999, L"난 내일에 자부심을 느낀다. 이번엔 절대 못 빠져나가.");
	Emplace_SubTitle(eType, 12, SpeakerType::Chai, 9999, L"어... 흠...");
}

void CCineSubTitleMgr::Initialize_CS0820()
{
	CineType eType = CineType::CS0820;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"넌 내 거다, 미모사!");
	Emplace_SubTitle(eType, 1, SpeakerType::Mimosa, 9999, L"큐 사인 기다려야지...");
	Emplace_SubTitle(eType, 2, SpeakerType::Chai, 9999, L"스펙트라 암호를 내놔!");
	Emplace_SubTitle(eType, 3, SpeakerType::Mimosa, 9999, L"너! 쇼를 망치다니...! 아니지...");
	Emplace_SubTitle(eType, 4, SpeakerType::Mimosa, 9999, L"여러분을 위해 아주 특별한 공연을 준비했습니다! 제 친구가&도와주러 왔네요.");
	Emplace_SubTitle(eType, 5, SpeakerType::Mimosa, 9999, L"난 반전의 여왕이다. 이건 내 쇼야!");
	Emplace_SubTitle(eType, 6, SpeakerType::Mimosa, 9999, L"조수들!");
	Emplace_SubTitle(eType, 7, SpeakerType::Mimosa, 9999, L"그리고 여러분...");
	Emplace_SubTitle(eType, 8, SpeakerType::Mimosa, 9999, L"쇼를 시작하겠습니다!");
	Emplace_SubTitle(eType, 9, SpeakerType::Mimosa, 9999, L"음악 큐!");
}

void CCineSubTitleMgr::Initialize_CS0825()
{
	CineType eType = CineType::CS0825;
	Emplace_SubTitle(eType, 0, SpeakerType::Mimosa, 9999, L"이거 재밌는걸...");
	Emplace_SubTitle(eType, 1, SpeakerType::Mimosa, 9999, L"수준을 좀 끌어 올려 볼까! 조명 꺼.");
}

void CCineSubTitleMgr::Initialize_CS0830()
{
	CineType eType = CineType::CS0830;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"다들 나만 보는데, 미모사.");
	Emplace_SubTitle(eType, 1, SpeakerType::Mimosa, 9999, L"관중들은 내 손안에 있어.");
	Emplace_SubTitle(eType, 2, SpeakerType::Mimosa, 9999, L"모두가 제대로 된 공연을 보고 싶어 하지.");
	Emplace_SubTitle(eType, 3, SpeakerType::Mimosa, 9999, L"누굴 보고 있는지 확인해 볼까.");

}

void CCineSubTitleMgr::Initialize_CS0835()
{
	CineType eType = CineType::CS0835;
	Emplace_SubTitle(eType, 0, SpeakerType::Mimosa, 9999, L"내 스포트라이트를 빼앗고 살아남을 순 없어!");
}

void CCineSubTitleMgr::Initialize_CS0840()
{
	CineType eType = CineType::CS0840;
	Emplace_SubTitle(eType, 0, SpeakerType::Mimosa, 9999, L"안 돼... 절대 질 수 없어...");
	Emplace_SubTitle(eType, 1, SpeakerType::Chai, 9999, L"반전이 없는 쇼도 있는 법이지.");
}

void CCineSubTitleMgr::Initialize_CS1125()
{
	CineType eType = CineType::CS1125;
	Emplace_SubTitle(eType, 0, SpeakerType::Macaron, 9999, L"시나몬! 그래, 좋았어! 다들 시나몬을 도와줘!");
	Emplace_SubTitle(eType, 1, SpeakerType::Chai, 9999, L"준비됐어?");
	Emplace_SubTitle(eType, 2, SpeakerType::EVERYONE, 9999, L"해 보자고!");
}

void CCineSubTitleMgr::Initialize_CS1130()
{
	CineType eType = CineType::CS1130;
}

void CCineSubTitleMgr::Initialize_EV1120()
{
	CineType eType = CineType::EV1120;
}

void CCineSubTitleMgr::Initialize_EV1130()
{
	CineType eType = CineType::EV1130;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"아직 못 가겠네...");
	Emplace_SubTitle(eType, 1, SpeakerType::CNMN, 9999, L"미스터 차이! 제가 도울 차례로군요!");
}

void CCineSubTitleMgr::Initialize_EV1150()
{
	CineType eType = CineType::EV1150;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"도망칠 곳이 없어...");
	Emplace_SubTitle(eType, 1, SpeakerType::Chai, 9999, L"코르시카! 여기까지 왔는데, 날 실망시키지 말라고.");
}

void CCineSubTitleMgr::Initialize_EV1160()
{
	CineType eType = CineType::EV1160;
}

void CCineSubTitleMgr::Initialize_EV1170()
{
	CineType eType = CineType::EV1170;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"안돼!");
	Emplace_SubTitle(eType, 1, SpeakerType::Kale, 9999, L"그래 봤자 소용없어.");
	Emplace_SubTitle(eType, 2, SpeakerType::Macaron, 9999, L"차이! 뭔가 할 생각이라면 지금이 기회야!");
}

void CCineSubTitleMgr::Initialize_EV1180()
{
	CineType eType = CineType::EV1180;
	Emplace_SubTitle(eType, 0, SpeakerType::Korsica, 9999, L"방금 정말 대단했어!");
	Emplace_SubTitle(eType, 1, SpeakerType::Peppermint, 9999, L"이제 어떻게 하지?");
	Emplace_SubTitle(eType, 2, SpeakerType::Macaron, 9999, L"잠깐, 시나몬은?");
	Emplace_SubTitle(eType, 3, SpeakerType::CNMN, 9999, L"이거라면 쓸 만할지 모르겠군요!");
}

void CCineSubTitleMgr::Initialize_EV1190()
{
	CineType eType = CineType::EV1190;
}

void CCineSubTitleMgr::Initialize_CS1205()
{
	CineType eType = CineType::CS1205;
	Emplace_SubTitle(eType, 0, SpeakerType::Kale, 9999, L"새로운 도구가 생겼군. 넌 좀 더 유용했으면 좋겠어.");
	Emplace_SubTitle(eType, 1, SpeakerType::Chai, 9999, L"반델레이 패밀리의 일원이 되어 자랑스럽습니다.");
	Emplace_SubTitle(eType, 2, SpeakerType::Macaron, 9999, L"차이는 그런 말 안 해!");
	Emplace_SubTitle(eType, 3, SpeakerType::Kale, 9999, L"그래, 그게 핵심이야. 내가 녀석을 조종하고 있...");
	Emplace_SubTitle(eType, 4, SpeakerType::Kale, 9999, L"으으윽! 윽, 뭐야...?");
	Emplace_SubTitle(eType, 5, SpeakerType::Peppermint, 9999, L"그래, 바로 그거야. 잘한다, 808!");
	Emplace_SubTitle(eType, 6, SpeakerType::Kale, 9999, L"장난하는 건가? 내가 무슨... 고양이 따위한테 질 것 같아?");
} 

void CCineSubTitleMgr::Initialize_CS1210()
{
	CineType eType = CineType::CS1210;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"우왓!");
	Emplace_SubTitle(eType, 1, SpeakerType::Kale, 9999, L"젠장!");
	Emplace_SubTitle(eType, 2, SpeakerType::Korsica, 9999, L"해치워, 차이!");
	Emplace_SubTitle(eType, 3, SpeakerType::Chai, 9999, L"돌아왔다고 하지 마.");
	Emplace_SubTitle(eType, 4, SpeakerType::Kale, 9999, L"정말 골치 아픈 녀석이군. 안 되겠어, 밖으로 나가자고.");
}

void CCineSubTitleMgr::Initialize_CS1215()
{
	CineType eType = CineType::CS1215;
	Emplace_SubTitle(eType, 0, SpeakerType::Kale, 9999, L"이 몸의 진정한 위력을 보여 주지.");
	Emplace_SubTitle(eType, 1, SpeakerType::Kale, 9999, L"네 하찮은 암스트롱 테크 따위는 비교도 안 돼.");
	Emplace_SubTitle(eType, 2, SpeakerType::Chai, 9999, L"그건 두고 봐야지.");
	Emplace_SubTitle(eType, 3, SpeakerType::Kale, 9999, L"그래, 두고 보자고.");
}

void CCineSubTitleMgr::Initialize_CS1220()
{
	CineType eType = CineType::CS1220;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"끝났어 케일!");
	Emplace_SubTitle(eType, 1, SpeakerType::Kale, 9999, L"내 편에 섰다면, 넌 뭐라도 될 수 있었을 텐데.");
	Emplace_SubTitle(eType, 2, SpeakerType::Kale, 9999, L"안타깝지만 네 같잖은 모험은 여기서 끝이다.");
}

void CCineSubTitleMgr::Initialize_CS1225()
{
	CineType eType = CineType::CS1225;
	Emplace_SubTitle(eType, 0, SpeakerType::Chai, 9999, L"이제 포기하고 암호를 넘겨.");
	Emplace_SubTitle(eType, 1, SpeakerType::Kale, 9999, L"거절하겠어.");
	Emplace_SubTitle(eType, 2, SpeakerType::Kale, 9999, L"너희는 패배자에 불과해!");
	Emplace_SubTitle(eType, 3, SpeakerType::Chai, 9999, L"그럴지도 모르지. 하지만 함께라면 우린 무적이야.");
	Emplace_SubTitle(eType, 4, SpeakerType::Peppermint, 9999, L"차이!");
	Emplace_SubTitle(eType, 5, SpeakerType::Kale, 9999, L"더 이상은 못 해 먹겠네...");
}

void CCineSubTitleMgr::Initialize_EV1250()
{
	CineType eType = CineType::EV1250;
}

void CCineSubTitleMgr::Emplace_SubTitle(CineType CineType, _uint Index, SpeakerType eType = SpeakerType::End, _float fLifeTime = 9999, wstring SubtitleString = L"")
{
	if (Index > 500)
	{
		cout << "SubtitleIndexoutofrange" << endl;
		assert(false);
	}
	if (m_SubTitleVec[(_uint)CineType][Index].String != L"")
	{
		cout << "OverLap!" << endl;
		cout << "Stage: " << (_uint)CineType << endl;
		cout << "Index: " << Index << endl;
	}

	Desc SubTitle;
	switch (eType)
	{
	case SpeakerType::Chai:
		SubTitle.SpeakerName = L"차이";
		break;
	case SpeakerType::Peppermint:
		SubTitle.SpeakerName = L"페퍼민트";
		break;
	case SpeakerType::Macaron:
		SubTitle.SpeakerName = L"마카론";
		break;
	case SpeakerType::Korsica:
		SubTitle.SpeakerName = L"코르시카";
		break;
	case SpeakerType::SBR:
		SubTitle.SpeakerName = L"SBR-001";
		break;
	case SpeakerType::Rekka:
		SubTitle.SpeakerName = L"레카";
		break;
	case SpeakerType::Mimosa:
		SubTitle.SpeakerName = L"미모사";
		break;
	case SpeakerType::Kale:
		SubTitle.SpeakerName = L"케일";
		break;
	case SpeakerType::RekkaHost:
		SubTitle.SpeakerName = L"바람잡이";
		break;
	case SpeakerType::CNMN:
		SubTitle.SpeakerName = L"시나몬";
		break;
	case SpeakerType::EVERYONE:
		SubTitle.SpeakerName = L"모두";
		break;
	default:
		SubTitle.SpeakerName = L"";
		break;
	}
	SubTitle.String = SubtitleString;
	SubTitle.m_fLifeTime = fLifeTime;
	m_SubTitleVec[(_uint)CineType][Index] = SubTitle;
}

void CCineSubTitleMgr::RenderFont(Desc FontDesc)
{
	auto pGI = CGameInstance::Get_Instance();
	_float4 SpeakerColor = { 0, 0.3f, 1, 1 };
	_float Scale = { 0.7f };
#ifdef PERFECTION_LOAD
	Scale *= 2;
#endif
	_float Rotation = { 0 };
	_float2 SpeakerPos = { 640 - _float(FontDesc.SpeakerName.size() * 9), 620 };
#ifdef PERFECTION_LOAD
	SpeakerPos.x *= 2;
	SpeakerPos.y *= 2;
#endif
	_float2 SubTitlePos = { 0, 0 };
	_float4 SubTitleColor = { 1, 1, 1, 1 };
	_float4 OutlineColor = { 0, 0, 0, 1 };
	size_t count = 0;
	size_t pos = 0;
	_float2 OutlinePosition = {};
	
	if (FontDesc.String.find(L"&") == string::npos)
	{
		OutlinePosition = { SpeakerPos.x, SpeakerPos.y };
		for (_int i = -1; i <= 1; ++i)
		{
			OutlinePosition.x += i;
			for (_int j = -1; j <= 1; ++j)
			{
				OutlinePosition.y += j;
				pGI->Render_Font(L"Nexon", FontDesc.SpeakerName, OutlinePosition,
					XMLoadFloat4(&OutlineColor), Rotation, Scale);
			}
		}

		pGI->Render_Font(L"Nexon", FontDesc.SpeakerName, SpeakerPos,
			XMLoadFloat4(&SpeakerColor), Rotation, Scale);

		pos = FontDesc.String.find(L".");
		while (pos != wstring::npos)
		{
			count++;
			pos = FontDesc.String.find(L".", pos + 1);
		}
		pos = FontDesc.String.find(L"?");
		while (pos != wstring::npos)
		{
			count++;
			pos = FontDesc.String.find(L"?", pos + 1);
		}

		pos = FontDesc.String.find(L"!");
		while (pos != wstring::npos)
		{
			count++;
			pos = FontDesc.String.find(L"!", pos + 1);
		}

		SubTitlePos = { 640 - _float(FontDesc.String.size() * 10) + count * 5, 660 };
#ifdef PERFECTION_LOAD
		SubTitlePos = { (640 - _float(FontDesc.String.size() * 10) + count * 5) * 2, 660 * 2 };
#endif
		OutlinePosition = { SubTitlePos.x, SubTitlePos.y };
		for (_int i = -1; i <= 1; ++i)
		{
			OutlinePosition.x += i;
			for (_int j = -1; j <= 1; ++j)
			{
				OutlinePosition.y += j;
				pGI->Render_Font(L"Nexon", FontDesc.String, OutlinePosition,
					XMLoadFloat4(&OutlineColor), Rotation, Scale);
			}
		}

		pGI->Render_Font(L"Nexon", FontDesc.String, SubTitlePos,
			XMLoadFloat4(&SubTitleColor), Rotation, Scale);
	}
	else
	{
		SpeakerPos.y = 600;
#ifdef PERFECTION_LOAD
		SpeakerPos.y *= 2;
#endif
		OutlinePosition = { SpeakerPos.x, SpeakerPos.y };
		for (_int i = -1; i <= 1; ++i)
		{
			OutlinePosition.x += i;
			for (_int j = -1; j <= 1; ++j)
			{
				OutlinePosition.y += j;
				pGI->Render_Font(L"Nexon", FontDesc.SpeakerName, OutlinePosition,
					XMLoadFloat4(&OutlineColor), Rotation, Scale);
			}
		}

		pGI->Render_Font(L"Nexon", FontDesc.SpeakerName, SpeakerPos,
			XMLoadFloat4(&SpeakerColor), Rotation, Scale);


		Desc TempFont;
		size_t Find = FontDesc.String.find(L"&");
		TempFont.String = FontDesc.String.substr(0, Find);
		size_t PreviousCount = Find + 1;


		pos = TempFont.String.find(L".");
		while (pos != wstring::npos)
		{
			count++;
			pos = TempFont.String.find(L".", pos + 1);
		}

		pos = TempFont.String.find(L"?");
		while (pos != wstring::npos)
		{
			count++;
			pos = TempFont.String.find(L"?", pos + 1);
		}

		pos = TempFont.String.find(L"!");
		while (pos != wstring::npos)
		{
			count++;
			pos = TempFont.String.find(L"!", pos + 1);
		}

		SubTitlePos = { 640 - _float(TempFont.String.size() * 10) + count * 5, 640 };
#ifdef PERFECTION_LOAD
		SubTitlePos = { (640 - _float(TempFont.String.size() * 10) + count * 5) * 2, 640 * 2 };
#endif
		OutlinePosition = { SubTitlePos.x, SubTitlePos.y };

		for (_int i = -1; i <= 1; ++i)
		{
			OutlinePosition.x += i;
			for (_int j = -1; j <= 1; ++j)
			{
				OutlinePosition.y += j;
				pGI->Render_Font(L"Nexon", TempFont.SpeakerName, OutlinePosition,
					XMLoadFloat4(&OutlineColor), Rotation, Scale);
			}
		}	
		pGI->Render_Font(L"Nexon", TempFont.String, SubTitlePos, XMLoadFloat4(&SubTitleColor), Rotation, Scale);


		count = 0;
		pos = TempFont.String.find(L".");
		while (pos != wstring::npos)
		{
			count++;
			pos = TempFont.String.find(L".", pos + 1);
		}
		pos = TempFont.String.find(L"?");
		while (pos != wstring::npos)
		{
			count++;
			pos = TempFont.String.find(L"?", pos + 1);
		}

		pos = TempFont.String.find(L"!");
		while (pos != wstring::npos)
		{
			count++;
			pos = TempFont.String.find(L"!", pos + 1);
		}


		TempFont.String = FontDesc.String.substr(PreviousCount, FontDesc.String.size() - PreviousCount);
		SubTitlePos = { 640 - _float(TempFont.String.size() * 10) + count * 5, 680 };
#ifdef PERFECTION_LOAD
		SubTitlePos = { (640 - _float(TempFont.String.size() * 10) + count * 5) * 2, 680 * 2};
#endif
		OutlinePosition = { SubTitlePos.x, SubTitlePos.y };

		for (_int i = -1; i <= 1; ++i)
		{
			OutlinePosition.x += i;
			for (_int j = -1; j <= 1; ++j)
			{
				OutlinePosition.y += j;
				pGI->Render_Font(L"Nexon", TempFont.SpeakerName, OutlinePosition,
					XMLoadFloat4(&OutlineColor), Rotation, Scale);
			}
		}
		pGI->Render_Font(L"Nexon", TempFont.String, SubTitlePos, XMLoadFloat4(&SubTitleColor), Rotation, Scale);
	}
}

void CCineSubTitleMgr::Free()
{
	for (_uint i = 0; i < (_uint)CineType::End; ++i)
	{
		m_SubTitleVec[i].clear();
	}
	m_RenderFontTitleVec.clear();
}
