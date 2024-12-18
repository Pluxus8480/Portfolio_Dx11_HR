#include "Beat_Manager.h"
#include "GameInstance.h"
#include "Texture.h"

IMPLEMENT_SINGLETON(CBeat_Manager)

CBeat_Manager::CBeat_Manager()
{
}

CBeat_Manager::~CBeat_Manager()
{
	Free();
}

void CBeat_Manager::Tick(_float fTimeDelta)
{
	m_fSuperTimer += fTimeDelta;
	m_fTimer += fTimeDelta;
	for (_uint i = 0; i < 8; ++i)
	{
		m_fNextBeats[i] -= fTimeDelta;
		if (m_fNextBeats[i] <= 0.f)
		{
			m_fNextBeats[i] += m_fBeat * 8.f;
			//if (i % 2)
			//{
			//	m_eNextBeatTypes[i] = BM_DEFAULT;
			//}
			//else
			//{
			//	m_eNextBeatTypes[i] = BM_SMALL;
			//}
		}
	}
	if (m_fTimer >= m_fBeat)
	{
		++m_iBeatCount;
		m_fTimer -= m_fBeat;

		if (m_bSequenceChange)
		{
			Start_Sequence();
		}
		if (m_pCurrSequence)
		{
			if (m_iBeatCount >= (_uint)(m_pCurrSequence->iBeat))
			{
				Proceed();
			}
		}
	}

}

void CBeat_Manager::Initialize()
{
	auto pGI = GAMEINSTANCE;
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"The_Rush", L"../Bin/Resources/Sounds/Music/The_Rush.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Captive_Normals", L"../Bin/Resources/Sounds/Music/Captive_Normals.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Can't_Stop_Us_Now", L"../Bin/Resources/Sounds/Music/Can't_Stop_Us_Now.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Negotiation", L"../Bin/Resources/Sounds/Music/Negotiation.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"InABlink", L"../Bin/Resources/Sounds/Music/InABlink.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"My_Heart_Feels_No_Pain", L"../Bin/Resources/Sounds/Music/My_Heart_Feels_No_Pain.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Surfacing", L"../Bin/Resources/Sounds/Music/Surfacing.ogg");

	/* For Lift Stage */
	pGI->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftPanel1_1", L"../Bin/Resources/Sounds/Ui/Talk/Lift/LiftPanel1_1.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftPanel1_2", L"../Bin/Resources/Sounds/Ui/Talk/Lift/LiftPanel1_2.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftBrake1_1", L"../Bin/Resources/Sounds/Ui/Talk/Lift/LiftBrake1_1.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftBrake1_2", L"../Bin/Resources/Sounds/Ui/Talk/Lift/LiftBrake1_2.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftBrake1_3", L"../Bin/Resources/Sounds/Ui/Talk/Lift/LiftBrake1_3.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftBrake2_1", L"../Bin/Resources/Sounds/Ui/Talk/Lift/LiftBrake2_1.ogg");
	pGI->Add_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftBrake2_2", L"../Bin/Resources/Sounds/Ui/Talk/Lift/LiftBrake2_2.ogg");

	/* For Mimosa Stage */
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_RP_Loop", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/RP_Loop(021).ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_RP_Ready", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/RP_Ready(034).ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_Finish", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Final_Attack(013).ogg");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_DanceBattle_Loop", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Mimosa_Bridge.wav");
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_DanceBattle_CS", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Dance_Battle_Intro_CS.ogg");
	//pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Lift_Battle0", L"../Bin/Resources/Sounds/Music/Lift_Battle0.ogg");
	//pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Lift_Battle1", L"../Bin/Resources/Sounds/Music/Lift_Battle1.ogg");
	//pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Lift_Chill0", L"../Bin/Resources/Sounds/Music/Lift_Chill0.ogg");
	//pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Lift_Chill1", L"../Bin/Resources/Sounds/Music/Lift_Chill1.ogg");
	//pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Lift_Intro", L"../Bin/Resources/Sounds/Music/Lift_Intro.ogg");

#pragma region PERFECT_DRUG
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_Intro", L"../Bin/Resources/Sounds/Music/Kale_Boss/1.Intro.ogg");//64
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_Verse1", L"../Bin/Resources/Sounds/Music/Kale_Boss/2.Verse1.ogg");//64
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_PreChor1", L"../Bin/Resources/Sounds/Music/Kale_Boss/3.PreChor.ogg");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_Chor1", L"../Bin/Resources/Sounds/Music/Kale_Boss/4.Chor1.ogg");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_PostChor1", L"../Bin/Resources/Sounds/Music/Kale_Boss/5.PostChor1.ogg");//16
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_Verse2", L"../Bin/Resources/Sounds/Music/Kale_Boss/6.Verse2.ogg");//128
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_PreChor2", L"../Bin/Resources/Sounds/Music/Kale_Boss/7.PreChor2.ogg");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_PreChor2_Inst", L"../Bin/Resources/Sounds/Music/Kale_Boss/PreChor2_Inst.wav");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_Chor2", L"../Bin/Resources/Sounds/Music/Kale_Boss/8.Chor2A.wav");//80
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_PostChor2", L"../Bin/Resources/Sounds/Music/Kale_Boss/9.PostChor2.ogg");//64
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_DrumBridge", L"../Bin/Resources/Sounds/Music/Kale_Boss/10.DrumBridge.ogg");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"PD_Outro", L"../Bin/Resources/Sounds/Music/Kale_Boss/11.Outro.ogg");//128

	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_Intro", 64});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_Verse1", 64});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_PreChor1", 32});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_Chor1", 32});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_PostChor1", 16});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_Verse2", 128});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_PreChor2_Inst", 32});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_Chor2", 80});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_PostChor2", 64});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_DrumBridge", 32});
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_Outro", 128});
	//m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"PD_Chor1", 32});

	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Verse1", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/verse1.wav");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Verse1_1", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/verse1_1.wav");//192
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Verse1_Loop", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Verse1_LP_2.wav");// 64
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Verse2_Bridge", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Chorus1(ToVerse2).wav");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Verse2", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Verse2_Lp_2.wav");//64
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_DanceBattle_Intro", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Chorus2(ToDanceBattle).wav");//48
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Chorus3_Intro", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Verse3_Intro.wav");//16
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Verse3_Vo_1", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Verse3_Vo_1.wav");//32
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"MM_Verse3_Lp_1", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Verse2_Lp_1.wav");//192
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"CS0820", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/CS0820.wav");//120
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Intro", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/Intro.wav");//24
	pGI->Add_Sound(SOUND_CHANNEL::MUSIC, L"Mimosa_OutroTypeA", L"../Bin/Resources/Sounds/Music/Mimosa_Stage/OutroTypeA.wav");//72


	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"CS0820", 120 });  // 11
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"Intro", 24 });// 12
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Verse1", 32 });// 13
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Verse1_1", 192 });// 14
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Verse1_Loop", 64 });// 15
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Verse2_Bridge", 32 });// 16
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Verse2", 64 });// 17
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_DanceBattle_Intro", 48 });// 18
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"Mimosa_DanceBattle_Loop", 152 });// 19
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Chorus3_Intro", 16 });// 20
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Verse3_Vo_1", 32 });// 21
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"MM_Verse3_Lp_1", 192 });// 22
	m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"Mimosa_OutroTypeA",72 });// 22
	//m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"Mimosa_RP_Ready", 16 });
	//m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"Mimosa_RP_Loop", 32 });
	//m_vecBGMSequence.push_back(BGM_SEQUENCE{ L"Mimosa_Finish", 8 });

	m_vecBGMSequence[12].pNext = &m_vecBGMSequence[13];
	m_vecBGMSequence[13].pNext = &m_vecBGMSequence[14];
	m_vecBGMSequence[14].pNext = &m_vecBGMSequence[15];
	m_vecBGMSequence[15].pNext = &m_vecBGMSequence[15];
	m_mapBGM.insert({ L"Mimosa_Start", &m_vecBGMSequence[11] });//phase1
	m_mapBGM.insert({ L"Mimosa_Intro", &m_vecBGMSequence[12] });//phase1
	m_mapBGM.insert({ L"Mimosa_Verse1", &m_vecBGMSequence[13] });//phase1

	m_mapBGM.insert({ L"Mimosa_Verse2_Bridge", &m_vecBGMSequence[16] });//phase2 start
	m_mapBGM.insert({ L"Mimosa_Verse2", &m_vecBGMSequence[17] });//phase2
	m_mapBGM.insert({ L"Mimosa_DanceBattle_Intro", &m_vecBGMSequence[18] });//DanceBattle_Intro

	m_vecBGMSequence[19].pNext = &m_vecBGMSequence[20];
	m_vecBGMSequence[20].pNext = &m_vecBGMSequence[21];
	m_vecBGMSequence[21].pNext = &m_vecBGMSequence[22];
	m_vecBGMSequence[22].pNext = &m_vecBGMSequence[22];
	m_mapBGM.insert({ L"Mimosa_DanceBattle_Loop", &m_vecBGMSequence[19] });//DanceBattle
	m_mapBGM.insert({ L"Mimosa_Verse3_Vo_1", &m_vecBGMSequence[20] });//DanceBattle
	m_mapBGM.insert({ L"Mimosa_OutroTypeA", &m_vecBGMSequence[23] });
	//m_mapBGM.insert({ L"Mimosa_RP_Ready", &m_vecBGMSequence[21] });

	//m_vecBGMSequence[22].pNext = &m_vecBGMSequence[22];
	//m_mapBGM.insert({ L"Mimosa_RP_Loop", &m_vecBGMSequence[22] });
	//m_mapBGM.insert({ L"Mimosa_Finish", &m_vecBGMSequence[23] });


	m_vecBGMSequence[0].pNext = &m_vecBGMSequence[1];
	m_vecBGMSequence[1].pNext = &m_vecBGMSequence[1];
	m_mapBGM.insert({L"Kale_Verse1", &m_vecBGMSequence[0] } );//phase0

	m_vecBGMSequence[2].pNext = &m_vecBGMSequence[3];
	m_vecBGMSequence[3].pNext = &m_vecBGMSequence[4];
	m_vecBGMSequence[4].pNext = &m_vecBGMSequence[5];
	m_mapBGM.insert({ L"Kale_Chor1" , &m_vecBGMSequence[2] });//phase1_1

	m_vecBGMSequence[5].pNext = &m_vecBGMSequence[5];
	m_mapBGM.insert({ L"Kale_Verse2", &m_vecBGMSequence[5] });//phase1_1

	m_vecBGMSequence[6].pNext = &m_vecBGMSequence[6];
	m_mapBGM.insert({ L"Kale_PreChor2" , &m_vecBGMSequence[6] });//rp1

	m_vecBGMSequence[7].pNext = &m_vecBGMSequence[7];
	//m_vecBGMSequence[11].pNext = &m_vecBGMSequence[7];
	m_mapBGM.insert({ L"Kale_Chor2", &m_vecBGMSequence[7] });//phase2

	m_vecBGMSequence[8].pNext = &m_vecBGMSequence[9];
	m_vecBGMSequence[9].pNext = &m_vecBGMSequence[9];
	m_mapBGM.insert({ L"Kale_Bridge", &m_vecBGMSequence[8] });//phase3

	m_vecBGMSequence[10].pNext = &m_vecBGMSequence[10];
	m_mapBGM.insert({ L"Kale_Outro" , &m_vecBGMSequence[10] });//outro


#pragma endregion

	m_pGameInstance = GAMEINSTANCE;
	//

}

void CBeat_Manager::Kill_Instance()
{
	m_pGameInstance = nullptr;
	m_pInstance->Free();
	m_pInstance->Destroy_Instance();
}

void CBeat_Manager::Stop_Sequence()
{
	m_bSequenceStoped = true;
	if (m_pCurrSequence)
	{
		m_pGameInstance->Stop_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);
	}
}

void CBeat_Manager::Start_Sequence()
{
	m_bSequenceChange = false;
	if (m_pCurrSequence)
	{
		m_pGameInstance->Stop_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);
	}
	m_pCurrSequence = m_mapBGM[m_strNextSequenceTag];
	m_pGameInstance->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag, m_fVolume);
	m_pGameInstance->Play_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);

	m_iBeatCount = 0;
	for (_uint i = 0; i < 8; ++i)
	{
		m_fNextBeats[i] = (_float)i * m_fBeat;
	}
	m_fTimer = 0.f;
}

void CBeat_Manager::Proceed()
{
	if (m_bSequenceStoped)
	{
		return;
	}
	m_pCurrSequence = m_pCurrSequence->pNext;
	if (m_pCurrSequence)
	{
		m_pGameInstance->Set_Sound_Volume(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag, m_fVolume);
		m_pGameInstance->Play_Sound(SOUND_CHANNEL::MUSIC, m_pCurrSequence->strTag);

	}
	m_iBeatCount = 0;
	for (_uint i = 0; i < 8; ++i)
	{
		m_fNextBeats[i] = (_float)i * m_fBeat;
	}
	m_fTimer = 0.f;
}

_bool CBeat_Manager::Beat_Move(_float fRate)
{
	if (m_iBeatCount == m_iBeatMoved)
		return false;
	_float fDiff = min(m_fBeat - m_fTimer, m_fTimer);
	
	if (abs(fDiff) > m_fBeat * fRate)
	{
		return false;
	}
	
	if (m_fTimer > m_fBeat * 0.5f)
	{
		m_iBeatMoved = m_iBeatCount + 1;
	}
	else
	{
		m_iBeatMoved = m_iBeatCount;
	}

	return true;
}

//void CBeat_Manager::Set_N_BeatType(_uint iN, BM_TYPE eType)
//{
//	_float fLeast = 10.f;
//	_uint iIdx = 0;
//	for (_uint i = 0; i < 8; ++i)
//	{
//		if (fLeast > m_fNextBeats[i])
//		{
//			fLeast = m_fNextBeats[i];
//			iIdx = i;
//		}
//	}
//
//	_uint iNext = iIdx + iN;
//	iNext %= 8;
//	m_eNextBeatTypes[iNext] = eType;
//
//}



void CBeat_Manager::Free()
{
}
