#pragma once

#pragma region Engine

#include "Texture.h"
#include "Collider.h"

#pragma endregion

#pragma region Test

#include "BackGround.h"

#pragma endregion

#pragma region Camera

#include "Tool_Camera.h"
#include "Player_Camera.h"

#pragma endregion

////////////////////////////////////////////////////////////
// For GameObject Headers
#include "BackGround.h"

// For Beomseok
#include "CUi_Sprite.h"
#include "CUi_Font.h"
#include "CUi_Canvas.h"
#include "CUi_LoadingSystem.h"
#include "CUi_PlayerBar.h"
#include "CUi_TalkSystem.h"
#include "CUi_PlayerHp.h"
#include "CUi_PlayerReverb.h"
#include "CUi_PlayerPartnerGauge.h"
#include "CUi_DashEffect.h"

// For Jeongrae
#include "CUi_Default.h"
#include "CUi_RankSystem.h"
#include "CUi_ComboNum.h"
#include "CUi_ComboWordHits.h"
#include "CUi_ScoreSystem.h"
#include "CUi_BeatMarker.h"
#include "CUi_BeatMarkerSystem.h"
#include "CUi_FinalResult.h"
#include "CUi_FinalResultSystem.h"
#include "CUi_StageResultSystem.h"
#include "CUi_StageCorus.h"
#include "CUi_StageResult_Bonus.h"
#include "CUi_StageResult_FinalScore.h"
#include "CUi_BeatJudge.h"
#include "CUi_StoreSystem.h"
#include "CUi_UpRightBack.h"
#include "CUi_StoreEntry.h"
#include "CUi_StoreAttack.h"
#include "CUi_StoreSkill.h"
#include "CUi_StoreItem.h"
#include "CUi_StoreChip.h"
#include "CUi_StoreBar.h"
#include "CUi_StoreDesc.h"
#include "CUi_Chip.h"
#include "CUi_StoreChipDesc.h"
#include "CUi_StoreBuyBar.h"
#include "CUi_StoreEdge.h"
#include "CUi_EquipDefault.h"
#include "CUi_StoreSkillEquip.h"
#include "CUi_StoreEquipBar.h"
#include "CUi_SkillCutScene.h"
#include "CUi_CommandIcon.h"
#include "CUi_GearSystem.h"
#include "CUi_RekkaSystem.h"
#include "CUi_RekkaHp.h"
#include "CUi_RekkaDeco.h"
#include "CUi_RekkaDecoEffect.h"
#include "CUi_KorsicaSystem.h"
#include "CUi_KorsicaFace.h"
#include "CUi_KorsicaHpBar.h"
#include "CUi_EnemySpecialAttack.h"
#include "CUi_TreasureBox.h"
#include "CUi_TreasureBoxSystem.h"
#include "CUi_FightString.h"
#include "CUi_PartIcon.h"
#include "CUi_JudgeWord.h"
#include "CUi_ComboButton.h"
#include "CUi_SuccessSystem.h"
#include "CUi_BeatMeter.h"
#include "CUi_GeneratorSystem.h"
#include "CMonster_Korsica.h"
#include "CEvent_Chai.h"
#include "CKorsicaCamera.h"
#include "CUi_MapInterAction.h"
#include "CUi_TramTimer.h"
#include "CUi_TramGauge.h"
#include "CUi_TramMacaronSign.h"
#include "CUi_StunGaugeSystem.h"
#include "CUi_KaleHpBar.h"
#include "CUi_KaleSystem.h"
#include "CUi_KaleGroggyGage.h"
#include "CUi_LogoButtonSystem.h"
#include "CUi_SpecialComboAttackSystem.h"
#include "CUi_MimosaSystem.h"
#include "CUi_MimosaBar.h"
#include "CUi_3DDefault.h"
#include "CUi_DefaultBar.h"
#include "CUi_MImosaMusicBattleSystem.h"
#include "CJeongGimmicBaseDefault.h"
#include "CKaleBoss_ObjSystem.h"
#include "CKale_Antenna.h"
#include "UI_Event_Cut.h"
#include "CKaleElevatorGenerator.h"
#include "CContainerBreakPannel.h"
#include "CRobotPunchBreakWall.h"
#include "CRobotPunchStaticWall.h"
#include "CRobotBeamContainer.h"
#include "CScroll2DContainer.h"
#include "CUi_DefaultLife.h"
#include "CUI_MimosaCutin.h"
#include "CTriggerBox.h"
#include "CRailContainer.h"
#include "CBattle3rdAfter.h"
#include "CBattle3rdFloor.h"
#include "CUi_CNMNRazer.h"
#include "CUi_CNMNRazorPart.h"
#include "CUi_DefaultFont.h"
#include "CUi_SkillChangeSystem.h"
#include "CKaleBoss_CineObj.h"
#include "CUi_EnemySpecialAttackPart.h"
#include "CUi_CineAttackSystem.h"
// For Jaeseok
#include "Free_SK_Object.h"
#include "Free_SM_Object.h"
#include "Monster_Saber.h"
#include "Saber_Weapon.h"
#include "Monster_Gunner.h"
#include "Monster_Baboo.h"
#include "Monster_Samurai.h"
#include "Monster_Rekka.h"
#include "Rekka_Pipe_L.h"
#include "Rekka_Pipe_R.h"
#include "Rekka_Long_Pipe.h"

#include "Rekka_Gauntlet.h"
#include "Rekka_Pipe_Door.h"
#include "Rekka_Generator.h"

#include "Tool_Cine_Camera.h"

#include "CineTrigger.h"
#include "Cine_Camera.h"
#include "Cine_Actor.h"

// For Mina
#include "MapObject_Static.h"
#include "MapObject_Anim.h"
#include "GimmickBase.h"
#include "Trigger.h"
#include "PartDoor.h"
#include "PartDoorFrame.h"
#include "ProdDeptDoor.h"
#include "BulbTrigger.h"
#include "MovingWall.h"
#include "SmallElevator.h"
#include "SpawnTrigger_Monster.h"
#include "MonsterSpawner.h"
#include "SwitchButton.h"
#include "CommonSwitch.h"
#include "SwitchFence.h"
#include "Button_Platform.h"
#include "SpawnTrigger_Platform.h"
#include "Catwalk_Bridge.h"
#include "FloorSwitch.h"
#include "FloorSwitchButton.h"
#include "ComboSwitch.h"
#include "ComboSwitchButton.h"
#include "LargeElevator.h"
#include "GearWall.h"
#include "Hanging_Platform.h"
#include "QACenterDoor.h"
#include "FadeElevator.h"
#include "EnemyGate.h"
#include "Generator.h"
#include "Rekka_Logo.h"
#include "EventTrigger.h"
#include "Misc_Interface.h"
#include "FloorSign.h"
#include "Fan_Blade.h"
#include "Fan_Platform.h"
#include "SignBoard_Platform.h"
#include "SignBoard.h"
#include "BattleAreaDoor_PD.h"
#include "GaraElevator.h"
#include "LevelUp_Machine.h"
#include "CullingBox.h"
#include "KrscRoomWall.h"
#include "SignPlane.h"
#include "RhythmWall.h"
#include "DMGConsole.h"
#include "Rekka_GenCover.h"
#include "Rekka_Monitor_S.h"
#include "Rekka_Monitor_L.h"
#include "Rekka_PhaseText.h"
#include "Rekka_MonitorBase.h"
#include "WireMagnet.h"
#include "RekkaRoomDoor.h"
#include "ZipLine_RailPart.h"
#include "ZipLine_Rail.h"
#include "ZipLine_Magnet.h"
#include "ZipLine_Obstacle.h"
#include "MovingFloor.h"
#include "SteelFrame_Platform.h"
#include "Krsc_Generator.h"
#include "SpectraMeshGate.h"
#include "SectionCullingBox.h"
#include "BuiltIn_Magnet.h"
#include "CraneMagnet.h"
#include "EmissiveArrow.h"
#include "TalkTrigger.h"
#include "Mimosa_RingText.h"
#include "Mimosa_BackLEDText.h"
#include "Mimosa_RoofLEDText.h"
#include "Mimosa_Visualizer.h"
#include "Mimosa_BackMonitor.h"
#include "Mimosa_Speaker.h"
#include "Mimosa_LightShaft.h"
#include "Spectra_FanBlade.h"
#include "Spectra_FanMachine.h"
#include "Free_SignBoard.h"
#include "Rekka_LightShaft.h"
#include "Rhythm_RobotArm.h"
#include "Rhythm_CleanSystem.h"
#include "Grass.h"
#include "GrassMatrix.h"

// For Jaehwi
#include "BigSlash_Effect.h"
#include "EnemySlash_Effect.h"
#include "Flare_Effect.h"
#include "Impact_Ring_Effect.h"
#include "Impact_Shock_Effect.h"
#include "Impact_Spark_Effect.h"
#include "Beam_Effect.h"
#include "Snap_Impact_Effect.h"
#include "Snap_Font_Effect.h"
#include "Smoke_Effect.h"
#include "ConvexLens_Effect.h"
#include "StarSymbol_Effect.h"
#include "LandImpact_Effect.h"
#include "LandJumpImpact_Effect.h"
#include "Barrier.h"
#include "Zap_Effect.h"
#include "Explosion_Effect.h"
#include "Boom_Font_Effect.h"
#include "Harmonic_Beam.h"
#include "Light_Effect.h"
#include "Swing_Effect.h"
#include "Flat_Wave_Effect.h"
#include "Ring_Effect.h"
#include "Smack_Font_Effect.h"
#include "Dot_Flare_Effect.h"
#include "Particle_Spark.h"
#include "Fire_Effect.h"
#include "Impact_Smack_Shock_Effect.h"
#include "Mesh_Shock_Effect.h"
#include "Shock_Effect_Font_A.h"
#include "Shock_Effect_Font_D.h"
#include "Panel_Warning.h"
#include "Rekka_Spark_Lightning.h"
#include "Rekka_Spark_Impact.h"
#include "Decal_Effect.h"
#include "Decal_Ring_Effect.h"
#include "Schwing_Font.h"
#include "BeamDot_Flare_Effect.h"
#include "Wire.h"
#include "Shash_Font.h"
#include "Wave_Effect.h"
#include "Square_Flare_Effect.h"
#include "PodLaser.h"
#include "WarningDecal_Effect.h"
#include "WarningDecalCircle_Effect.h"
#include "Particle_Anime_Spark.h"
#include "808_Flare_Effect.h"
#include "JustMark_Effect.h"
#include "Laser_Warning_Effect.h"
#include "Lift_Speed_Effect.h"
#include "InvisibleCylinderWall.h"
#include "Monster_Kale.h"
#include "Kale_Sword.h"
#include "Kale_Funnel.h"
#include "Kale_BArm.h"
#include "Kale_FArm.h"
#include "Kale_SArm.h"
#include "808_Playable.h"
#include "Kale_FireWall.h"
#include "Kale_BigSword.h"
#include "GiantRobot.h"
#include "GiantRobot2.h"
#include "GiantRobot3.h"
#include "RhythmJump.h"
#include "AfterImage.h"
#include "PeppermintContainer.h"
#include "MacaronCrane.h"
#include "Mimosa_Floor.h"
#include "MimosaWall.h"
#include "CNMN_Playable.h"

#include "Fire_Effect.h"
#include "Cyclone.h"
#include "Parry_Effect.h"
#include "Procedural_Cyclone_Effect.h"
#include "Effect_Lens.h"
#include "Tower_Elec.h"
#include "Misc_Tower_Elec.h"
#include "Radial_Blur.h"
#include "TrulyInvisibleWall.h"
#include "BeamCylinder.h"
#include "GiantRobot_Arm.h"

#include "Slate_Platform.h"
#include "Tunnel.h"
#include "Lift.h"
#include "Brake_Part.h"
#include "Panel_Lift.h"
#include "Enemy_Lift.h"
#include "Smoke_Cover.h"
#include "ConveyorBelt.h"
#include "LaserCircuit.h"
#include "LaserPod.h"
#include "LaserPillar.h"
#include "LaserBoard.h"
#include "LaserReward.h"
#include "GlassWall.h"
#include "ReverbGaugeRoom.h"
#include "Impact_Shock_Bigger_Effect.h"
#include "WoodenBox.h"
#include "PickUpItem.h"
#include "InvisibleWall.h"
#include "FirstBattleWall.h"
#include "CameraController.h"
#include "Light_Controller.h"

#include "HammerOn_Effect.h"
#include "Smoke_Explode_Effect.h"
#include "Font_Effect_Rekka_Slam.h"
#include "Decal_Special_Effect.h"
#include "SkySphere.h"
#include "Unlimited_Void_Effect.h"
#include "Rekka_Land_Spark.h"
#include "Frustum_Flare_Effect.h"
#include "Sun_Flare_Effect.h"
#include "Rekka_Land_Spark.h"

#include "Font_Effect_Gyaaang.h"
#include "Font_Effect_Bam_B.h"
#include "Font_Effect_Bam_A.h"
#include "Font_Effect_Bam_M.h"
#include "Font_Effect_Zing.h"
#include "Font_Effect_Shwaa.h"
#include "Font_Effect_Phaa.h"
#include "Font_Effect_Shoom.h"
#include "Font_Effect_HNGH.h"
#include "Font_Effect_On.h"
#include "Font_Effect_Charged.h"
#include "Font_Effect_Doon.h"
#include "Font_Effect_Wvuong.h"

#include "Bullet_Effect.h"

#include "Balloon_Effect.h"

#include "Dash_Effect.h"
#include "Glow_Slash_Effect.h"
#include "Slam_Effect.h"
#include "Rekka_Dash_Effect.h"

#include "Special_Effect_PS_Spark.h"
#include "Font_Effect_RaEx_Ex.h"
#include "Font_Effect_RaEx_R.h"
#include "Font_Effect_RaEx_A.h"

#include "Effect_FireWall.h"

#include "PepperMint_Effect_MF_1.h"
#include "PepperMint_Effect_MF_2.h"

#include "Damage_Effect.h"
#include "Damage_SP.h"
#include "Damage_Special.h"
#include "Damage_PepperMint.h"

#include "Hit_Base.h"
#include "Hit_Special.h"
#include "Hit_Hard.h"
#include "Hit_Electro.h"

#include "FireWall_Effect.h"
#include "Flat_Fire_Effect.h"
#include "Particle_Fire.h"
#include "Effect_Sun_Flare_Extend.h"
#include "Effect_Kale_Judgement.h"
#include "Effect_Kale_Barrier.h"
#include "Tracking_Laser.h"
#include "Particle_Fire_Loop.h"
#include "Effect_ODSlash.h"
#include "Decal_SpotLight.h"
#include "Background_Effect.h"
#include "ExpandingCylinder.h"
#include "FireSwingEffect.h"
#include "BallLighting.h"
#include "TriggerBox_Level.h"
#include "BG_Overrider.h"
#include "TabletScreen.h"
#include "Sound_Explode_Effect.h"
#include "Spectra_Mark.h"

#pragma region Mimosa

#include "Font_Mimosa_DoremiCode.h"
#include "Effect_Mimosa_Barrier.h"
#include "Effect_Mimosa_Warning.h"
#include "Effect_Line.h"

#include "Mirror_Ball.h"
#include "Random_Laser.h"
#pragma endregion

// For Minhyok
#include "TestEffect.h"
#include "Texture_Effect.h"
#include "Particle_Effect.h"
#include "Effect_Union.h"
#include "ODS_Controller.h"

// For Sanghyun
#include "Vergil.h"
#include "Chai.h"
#include "Peppermint.h"
#include "Macaron.h"
#include "Korsica.h"
#include "808_Companion.h"
#include "808_Cat.h"
#include "808_Controller.h"
#include "GridBox.h"

#include "Basic_Guitar.h"
#include "Weapon_Peppermint.h"
#include "Weapon_Korsica.h"
#include "Monster_Mimosa.h"
#include "Particle_Mimosa_Panel.h"

#include "Effect_Filter_Focus_Trigger.h"
#include "Weapon_Mimosa.h"
#include "Mimosa_Letter_Bomb.h"
#include "BackDancer.h"
#include "Mimosa_SpotLight.h"
#include "Monster_SwordMachine.h"
#include "SwordMachine_Sword.h"
#include "Monster_Blaze.h"
#include "Chai_Random_Sound.h"

#include "Decal_Flare_Effect.h"

#include "Effect_Audience_Upper_Plane.h"
#include "Effect_Audience_Lower_Plane.h"

#include "Effect_LightStick_Upper_Plane.h"
#include "Effect_LightStick_Lower_Plane.h"
#include "Effect_LightStick_Upper.h"
#include "Effect_LightStick_Lower.h"

#include "Particle_FireCracker_Normal.h"
#include "Musician_Robot.h"
#include "Musician_Instrument.h"

#include "Effect_Directional_Light_Controler.h"
#include "Drakken_Laser.h"
#include "Font_Effect_Container_Donk.h"
#include "Font_Effect_Container_Spark.h"

#include "Blaze_Fire.h"
#include "Blaze_FireStarter.h"
#include "Particle_Spark_Controller.h"
#include "Effect_ShakeFilter.h"
#include "PickMeUp_Can.h"
#include "Camera_Void_Effect.h"
#include "Effect_JudgementCut.h"