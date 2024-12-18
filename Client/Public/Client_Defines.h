#pragma once

#include "Engine_Defines.h"
#include <process.h>

namespace Client
{
#ifdef _DEBUG
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
#else
#ifdef PERFECTION_LOAD
	const unsigned int		g_iWinSizeX = 1280 * 2;
	const unsigned int		g_iWinSizeY = 720 * 2;
#else
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
#endif
#endif
	struct BGM_SEQUENCE
	{
		std::wstring strTag;
		_int	 iBeat = 0;
		BGM_SEQUENCE* pNext = nullptr;
	};

	enum LEVEL {
		LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TEST,
		LEVEL_EFFECTTOOL, LEVEL_ANIMTOOL, LEVEL_MAPTOOL, LEVEL_UITOOL, LEVEL_CINETOOL,
		LEVEL_LIFT, LEVEL_KORSICA, LEVEL_MODELTEST, LEVEL_KALEBOSS, LEVEL_MIMOSA, LEVEL_GIMMICTEST, LEVEL_KALESTAGE, LEVEL_END
	};
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

enum class MESH_PASS
{
	Default,
	MIRROR_X,
	AlphaBlend,
	EffectCCW,
	Distortion,
	Emission,
	Belt,
	CCW,
	CW,
	Outline,
	SKY,
	Water,
	Frame,
	AlphaEmission,
	BeltEmission,
	NoneCulling,
	DiffuseColor,
	Shadow,
	Reflection,
	ReflectionScreen,
	ReflectionCube,
	DCBlend,
	EmissionMoving,
	ShaftLight,
	Visualizer,
	EmissionFrame,
	Execution,
	Grass,
};

enum class EFFECT_PASS
{
	Default,
	AlphaBlend,
	EffectCCW,
	UVFlow,
	Barrier,
	Fire,
	Distortion,
	DistortRim,
	MonoColorMesh,
	InvisibleWall,
	ColorReverse,
	DistortMonoColor,
	DrakkenLaser,
	MonoColorMeshAccurate,
	ColorReverseEx,
	JudgementCut,
};
enum class CUBE_PASS
{
	Default,
	SKY_BOX_1,
	GRID_BOX_2,
	SSD,
	SSD_MONOCOLOR,
	SSD_WARNING,
	SSD_WARNING_CIRCLE,
	SSD_WARNING_CIRCLE_CLIP,
	SSD_SPOTLIGHT,
};
enum class INSTANCE_PASS
{
	Default,
	HpBar,
	Blend,
	RoundProgress,
	ShakeVertical,
	BlendPoint,
	FromRight,
	FromLeft,
	DecalToDiffuse,
	ParticlePtInstance,
	ParticleSparkPtInstance,
	ParticleFirePtInstance,
	ParticlePtInstanceMask,
};

enum class EFFECT_TEX_PASS
{
	AlphaBlend,
	WeightBlend,
	WeightBlendMonoColor,
	NonBlendMonoColor,
	UVFlow,
	Flare,
	BlendOriginColor,
	ConvexLens,
	Explosion,
	TextureColor,
	Fire,
	Warning,
	MaskFlare,
	BlendMonoColor,
	InvisibleWall,
	BlendOriginColorDissolve,
	FIREWALL,
	FlareAlpha,
	BACKEFFECT,
	LENS_WHITEOUT,
	TOWER_ELEC,
	BG_OVERRIDE,
	BG_OVERRIDE_TEXT,
};

enum class ANIMMESH_PASS
{
	Default,
	Emission,
	Dissolve,
	MonoColor,
	RimEmission,
	Rim,
	ColorOnly,
	Disintegration,
	Outline,
	Distortion,
	Outlined,
	OutlineColor,
	Shadow,
	NormalMap,
	DistortionBoost,
	OutlineCCW,
	EmissionColor,
	EmissionFrame,
	EmissionCCW,
	MimosaWing,
	DEFFEREDOUTLINE,
	MonoColorRim,
};

enum UI_EVENT
{
	UI_EVENT_CHANGE_PARTNER,
	UI_EVENT_CHANGE_HP,
	UI_EVENT_CHANGE_MAX_HP,
	UI_EVENT_CHANGE_REVERB,
	UI_EVENT_CHANGE_MAX_REVERB,
	UI_EVENT_FIGHT_MODE,
	UI_EVENT_ENABLE_REVERB,
	UI_EVENT_PROCEED_TALK,
	UI_EVEN_END
};

#define CHAI_COLLIDER_HIT     TEXT("Player_Hit")
#define CHAI_COLLIDER_ATTACK  TEXT("Player_Attack")

#define PARENTCHECK if (m_pParentUnion.lock() && m_pParentUnion.lock()->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_SLEEP) return;

#define INIT_MATRIX _float4x4(1.f, 0.f, 0.f, 0.f, \
							 0.f, 1.f, 0.f, 0.f, \
							 0.f, 0.f, 1.f, 0.f, \
							 0.f, 0.f, 0.f, 1.f)

#ifdef _DEBUG

#define CURSOR_CONTROL POINT ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 }; \
						if(GetFocus() != NULL)\
						{ \
							ClientToScreen(g_hWnd, &ptWindow); \
							SetCursorPos(ptWindow.x, ptWindow.y); \
							ShowCursor(false);\
						}
#else

#define CURSOR_CONTROL POINT ptWindow = { g_iWinSizeX >> 2, g_iWinSizeY >> 2 }; \
						if(GetFocus() != NULL)\
						{ \
							ClientToScreen(g_hWnd, &ptWindow); \
							SetCursorPos(ptWindow.x, ptWindow.y); \
							ShowCursor(false);\
						}
#endif

#define BEATRATE BEATMANAGER->Get_BeatRate()

using namespace Client;