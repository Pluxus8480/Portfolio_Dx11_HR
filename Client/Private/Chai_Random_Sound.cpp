#include "Chai_Random_Sound.h"
#include "GameInstance.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "Chai.h"

wstring CChai_Random_Sound::ObjID = TEXT("CChai_Random_Sound");
CChai_Random_Sound::CChai_Random_Sound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CChai_Random_Sound::CChai_Random_Sound(const CChai_Random_Sound& rhs)
	: CGameObject(rhs)
{
}

CChai_Random_Sound::~CChai_Random_Sound()
{
}

HRESULT CChai_Random_Sound::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_0"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_0.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_1"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_1.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_2"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_2.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_3"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_3.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_4"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_4.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_5"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_5.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_6"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_6.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_9"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_9.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_10"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_10.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_11"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_11.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("L_Attack_12"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/L_Attack_12.ogg")));

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("LLLL"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/LLLL.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("RRR"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/RRR.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("HBeam"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/HBeam.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("AerialRave"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/AerialRave.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("LRLL"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/LRLL.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("RLR"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/RLR.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("LRR"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/LRR.ogg")));


	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("R_Attack_0"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/R_Attack_0.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("R_Attack_1"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/R_Attack_1.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("R_Attack_2"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/R_Attack_2.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("R_Attack_3"), TEXT("../Bin/Resources/Models/Player/Chai/Sound/R_Attack_3.ogg")));


	return S_OK;
}

HRESULT CChai_Random_Sound::Initialize(void* pArg)
{
	return S_OK;
}

void CChai_Random_Sound::PriorityTick(_float fTimeDelta)
{
}

void CChai_Random_Sound::Tick(_float fTimeDelta)
{
}

void CChai_Random_Sound::LateTick(_float fTimeDelta)
{
}

HRESULT CChai_Random_Sound::Render()
{
	return S_OK;
}

void CChai_Random_Sound::Play_Sound(string strSoundName)
{
	if(strSoundName == "LLLL")
		GAMEINSTANCE->Play_Sound( SOUND_CHANNEL::SOUND_EFFECT, L"LLLL");
	else if (strSoundName == "RRR")
		GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"RRR");
	else if (strSoundName == "HBeam")
		GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"HBeam");
	else if (strSoundName == "AerialRave")
		GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"AerialRave");
	else if (strSoundName == "LRLL")
		GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LRLL");
	else if (strSoundName == "RLR")
		GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"RLR");
	else if (strSoundName == "LRR")
		GAMEINSTANCE->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LRR");
}

void CChai_Random_Sound::Play_Random_Attack_Sound_L()
{
	_int iRand = GAMEINSTANCE->Random_Int(0, 4);
	string strSound = "L_Attack_" + to_string(iRand);
	wstring wstrSound;
	wstrSound.assign(strSound.begin(), strSound.end());

	GAMEINSTANCE->Play_Unique_Sound_Channel("Chai_Guitar", SOUND_CHANNEL::SOUND_EFFECT, wstrSound);
}

void CChai_Random_Sound::Play_Random_Attack_Sound_R()
{
	_int iRand = GAMEINSTANCE->Random_Int(0, 2);
	string strSound = "R_Attack_" + to_string(iRand);
	wstring wstrSound;
	wstrSound.assign(strSound.begin(), strSound.end());

	GAMEINSTANCE->Play_Unique_Sound_Channel("Chai_Guitar", SOUND_CHANNEL::SOUND_EFFECT, wstrSound);
}

HRESULT CChai_Random_Sound::Ready_Components()
{
	return S_OK;
}

shared_ptr<CChai_Random_Sound> CChai_Random_Sound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CChai_Random_Sound
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CChai_Random_Sound(pDevice, pContext)
		{}
	};


	shared_ptr<CChai_Random_Sound> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CChai_Random_Sound"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CChai_Random_Sound::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CChai_Random_Sound
	{
		MakeSharedEnabler(const CChai_Random_Sound& rhs) : CChai_Random_Sound(rhs)
		{}
	};


	shared_ptr<CChai_Random_Sound> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CChai_Random_Sound"));
		assert(false);
	}

	return pInstance;
}

void CChai_Random_Sound::Free()
{
}
