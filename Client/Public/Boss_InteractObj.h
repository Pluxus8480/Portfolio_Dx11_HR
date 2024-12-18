#pragma once
#include "Client_Defines.h"

BEGIN(Client)

class CBoss_InteractObj abstract
{
public:
	enum class REKKA_PHASE { PHASE_1, PHASE_2, PHASE_3, PHASE_4, PHASE_END };
	enum class REKKA_PATTERN { SOUNDWAVE, GRAB, STOMP, CHARGE, ELECTRIC, PATTERN_END };

	enum class KORSIKA_PHASE { BATTLE, ENDDING, PHASE_END };

	enum class MIMOSA_PHASE {PHASE_1, PHASE_2, PHASE_3, PHASE_4, PHASE_END};
	enum class MIMOSA_PATTERN { IDLE, DANCEBATTLE, PATTERN_END };

	enum class KALE_PHASE { Antenna_Phase1, Antenna_Phase2, Antenna_Phase3, PHASE_END };
	enum class KALEMAP_PHASE { START, CONTAINERCUTSCENE, SCROLLCUTSCENEENTER, 
		ROBOTBEAM, ROBOTPUNCHCHAI, ROBOTPUNCHMACARON, CUTROBOTARM, FIRSTBATTLE, PHASE_END };
	// START = 7:49:00
	// CONTAINERCUTSCENE = 7:52:30
	// SCROLLCUTSCENEENTER = 7:58:30
	// ROBOTPUNCH = 8:00:00
	// 
public:
	virtual void Change_Phase(_uint iPhase) { m_iCurPhase = iPhase; }
	virtual void Change_Pattern(_uint iPattern) { m_iCurPattern = iPattern; }

protected:
	_uint		m_iCurPhase = { 0 };
	_uint		m_iCurPattern = { 0 };

};

END