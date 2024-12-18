#include "CUi_Default.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_Default::ObjID = L"CUi_Default";

CUi_Default::CUi_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_Default::CUi_Default(const CUi_Default& rhs)
	:CUi_2D(rhs)
{
}

CUi_Default::~CUi_Default()
{
}

HRESULT CUi_Default::Initialize_Prototype()
{
	Initialize_BeatmeterResource();
	Initialize_ScoreBackResource();
	Initialize_RankResource();
	Initialize_FinalResultBackResource();
	Initialize_StageResultResource();
	Initialize_GearResource();
	Initialize_BeatMarkerResource();
	Initialize_StoreResource();
	Initialize_CutSceneResource();
	Initialize_RekkaResource();
	Initialize_KorsicaResource();
	Initialize_EnemySpecialAttackResource();
	Initialize_MapInterActionResource();
	Initialize_SuccessResource();
	Initialize_GeneratorResource();
	Initialize_StunGaugeResource();
	Initialize_KaleResource();
	Initialize_SpecialComboAttackResource();
	Initialize_MimosaResource();
	Initialize_MimosaCutinResource();

	return S_OK;
}

HRESULT CUi_Default::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	m_TestName = Arg->TestName;
	if (FAILED(Ready_Components(pArg)))
		assert(false);
	InitializeRenderType(Arg->eRenderType, Arg->iPriority);
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed, Arg->fColor);

	return S_OK;
}

void CUi_Default::PriorityTick(_float fTimeDelta)
{
}

void CUi_Default::Tick(_float fTimeDelta)
{
}

void CUi_Default::LateTick(_float fTimeDelta)
{
	if (m_bActive && m_bOwnActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}


void CUi_Default::Initialize_BeatmeterResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatmeterBack",
			L"../Bin/Resources/Textures/Ui/BeatMeter/BeatmeterBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatmeterSpike",
			L"../Bin/Resources/Textures/Ui/BeatMeter/BeatmeterSpike%d.png", 5));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatmeterSpikeBack",
			L"../Bin/Resources/Textures/Ui/BeatMeter/BeatmeterSpikeBack%d.png", 4));
}

void CUi_Default::Initialize_BeatMarkerResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerBack",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerBackSmall",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerBackSmall.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerCat",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerCat.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarkerCircle",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarkerCircle.png"));
}

void CUi_Default::Initialize_RankResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Rank",
			L"../Bin/Resources/Textures/Ui/Rank/Rank%d.png", 5));
}

void CUi_Default::Initialize_ScoreBackResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ScoreBack",
			L"../Bin/Resources/Textures/Ui/Score/ScoreBack.png"));
}

void CUi_Default::Initialize_FinalResultBackResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultBack",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultString",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultRankString.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultBonus",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultBonus.png"));


	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_FinalResultWord",
			L"../Bin/Resources/Textures/Ui/FinalResult/FinalResultWord.png"));
}

void CUi_Default::Initialize_StageResultResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Fade",
			L"../Bin/Resources/Textures/Ui/StageResult/CUi_Fade.png"));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Effect",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Effect.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Screen",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Screen.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Star",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Star.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StageResult_Title",
			L"../Bin/Resources/Textures/Ui/StageResult/StageResult_Title.png"));
}

void CUi_Default::Initialize_HpResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_HpShadow",
			L"../Bin/Resources/Textures/Ui/Hp/.png"));
}

void CUi_Default::Initialize_GearResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Gear",
			L"../Bin/Resources/Textures/Ui/Gear/CUi_Gear.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_GearAdd",
			L"../Bin/Resources/Textures/Ui/Gear/GearAdd.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_GearAddBg",
			L"../Bin/Resources/Textures/Ui/Gear/Gear_AddBg.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_GearBack",
			L"../Bin/Resources/Textures/Ui/Gear/Gear_Back.png"));
}

void CUi_Default::Initialize_StoreResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBackDown",
			L"../Bin/Resources/Textures/Ui/Store/StoreBackDown.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBackUp",
			L"../Bin/Resources/Textures/Ui/Store/StoreBackUp.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreTitle",
			L"../Bin/Resources/Textures/Ui/Store/StoreTitle.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TitleEquip",
			L"../Bin/Resources/Textures/Ui/Store/TitleEquip.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreOpen",
			L"../Bin/Resources/Textures/Ui/Store/StoreOpen%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SelectAttackStore",
			L"../Bin/Resources/Textures/Ui/Store/SelectAttackStore.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreHasSkill",
			L"../Bin/Resources/Textures/Ui/Store/StoreHasSkill.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StorePauseChip",
			L"../Bin/Resources/Textures/Ui/Store/Chip/PauseChip.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreChipLevel",
			L"../Bin/Resources/Textures/Ui/Store/Chip/ChipLevel%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreBuyBarBack",
			L"../Bin/Resources/Textures/Ui/Store/StoreBuyBarBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreEdgePart",
			L"../Bin/Resources/Textures/Ui/Store/StoreEdgePart%d.png", 2));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EquipChaiBack",
			L"../Bin/Resources/Textures/Ui/Store/SkillEquip/EquipChaiBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EquipArrow",
			L"../Bin/Resources/Textures/Ui/Store/SkillEquip/EquipArrow.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EquipParrel",
			L"../Bin/Resources/Textures/Ui/Store/SkillEquip/EquipParrel.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EquipListBack",
			L"../Bin/Resources/Textures/Ui/Store/SkillEquip/EquipListBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EquipReverbe",
			L"../Bin/Resources/Textures/Ui/Store/SkillEquip/EquipReverbe.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_EquipTwinkle",
			L"../Bin/Resources/Textures/Ui/Store/SkillEquip/EquipTwinkle.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreItem",
			L"../Bin/Resources/Textures/Ui/Store/StoreItem%d.png", 4));
}

void CUi_Default::Initialize_CutSceneResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_CUtScene",
			L"../Bin/Resources/Textures/Ui/SkillCutScene/CUtScene%d.png", 6));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SkillWord",
			L"../Bin/Resources/Textures/Ui/SkillCutScene/Word/SkillWord%d.png", 15));
	 
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_CutSceneBack",
			L"../Bin/Resources/Textures/Ui/SkillCutScene/CutSceneBack.png"));
}

void CUi_Default::Initialize_RekkaResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_RekkaHpBack",
			L"../Bin/Resources/Textures/Ui/Boss/Rekka/RekkaHpBack%d.png", 8));
}

void CUi_Default::Initialize_KorsicaResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ChaiHpBack",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/ChaiHpBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KorsicaHpBack",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/KorsicaHpBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ChaiHpShadow",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/ChaiHpShadow.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KorsicaHpShadow",
			L"../Bin/Resources/Textures/Ui/Boss/Korsica/KorsicaHpShadow.png"));
}

void CUi_Default::Initialize_EnemySpecialAttackResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_Parry",
			L"../Bin/Resources/Textures/Ui/EnemySpecialAttack/Parry%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ParryBoard",
			L"../Bin/Resources/Textures/Ui/EnemySpecialAttack/ParryBoard%d.png", 4));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ParryString",
			L"../Bin/Resources/Textures/Ui/EnemySpecialAttack/ParryString.png"));
}

void CUi_Default::Initialize_MapInterActionResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TreasureBoxArrow",
			L"../Bin/Resources/Textures/Ui/MapInterAction/TreasureBox/TreasureBoxArrow%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ButtonArrow",
			L"../Bin/Resources/Textures/Ui/MapInterAction/TreasureBox/ButtonArrow.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TreasureBoxLine",
			L"../Bin/Resources/Textures/Ui/MapInterAction/TreasureBox/TreasureBoxLine.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ComboButtonBg",
			L"../Bin/Resources/Textures/Ui/ComboButton/ComboButtonBg.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ComboButtonMouse",
			L"../Bin/Resources/Textures/Ui/ComboButton/ComboButtonMouse%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ComboSuccess",
			L"../Bin/Resources/Textures/Ui/ComboButton/ComboSuccess.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ComboButtonCircle",
			L"../Bin/Resources/Textures/Ui/ComboButton/ComboButtonCircle.png"));
}

void CUi_Default::Initialize_SuccessResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SucessBack",
			L"../Bin/Resources/Textures/Ui/Success/SuccessBackPart.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SuccessBackPartLine",
			L"../Bin/Resources/Textures/Ui/Success/SuccessBackPartLineBlur%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SuccessMiddleGreen",
			L"../Bin/Resources/Textures/Ui/Success/SuccessMiddleGreen1.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SuccessMiddleBack",
			L"../Bin/Resources/Textures/Ui/Success/SuccessMiddleBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SuccessMiddleBackLight",
			L"../Bin/Resources/Textures/Ui/Success/SuccessMiddleBackLight.png"));
	
}

void CUi_Default::Initialize_GeneratorResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_GeneratorBackLight",
			L"../Bin/Resources/Textures/Ui/Generator/GeneratorBackLight.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_GeneratorBackCircle",
			L"../Bin/Resources/Textures/Ui/Generator/GeneratorBackCircle.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_GeneratorSuccess",
			L"../Bin/Resources/Textures/Ui/Generator/GeneratorSuccess%d.png", 3));
}

void CUi_Default::Initialize_StunGaugeResource()
{
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StunGaugeStar",
			L"../Bin/Resources/Textures/Ui/StunGauge/StunGaugeStar%d.png", 6));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StunGaugeBarBack",
			L"../Bin/Resources/Textures/Ui/StunGauge/StunGaugeBarBack.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StunGaugeBarBase",
			L"../Bin/Resources/Textures/Ui/StunGauge/StunGaugeBarBase.png"));
}

void CUi_Default::Initialize_KaleResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KaleHpBack",
			L"../Bin/Resources/Textures/Ui/Boss/Kale/KaleBackReal%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KaleHpShadow",
			L"../Bin/Resources/Textures/Ui/Boss/Kale/KaleHpBarShadowReal%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KaleName",
			L"../Bin/Resources/Textures/Ui/Boss/Kale/KaleName.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KaleStunGaugeBack",
			L"../Bin/Resources/Textures/Ui/Boss/Kale/KaleStunGaugeBg.png"));
}

void CUi_Default::Initialize_SpecialComboAttackResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SpecialComboBG",
			L"../Bin/Resources/Textures/Ui/SpecialComboAttack/SpecialComboBG.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_SpecialComboDot",
			L"../Bin/Resources/Textures/Ui/SpecialComboAttack/SpecialComboDot%d.png", 2));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_KeyBoardInput",
			L"../Bin/Resources/Textures/Ui/SpecialComboAttack/KeyboardInput%d.png", 4));
}

void CUi_Default::Initialize_MimosaResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaHealthBarBack",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaBack%d.png", 6));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaDeco",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/HpDeco%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaName",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaName.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaShadow",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaShadow.png"));
	
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaGroggyGaugeBg",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaGroggyGaugeBg.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaMusicBase",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaMusicBase.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaMusicChai",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaMusicChai%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaMusicMimosa",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaMusicMimosa%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaMusicBack",
			L"../Bin/Resources/Textures/Ui/Boss/Mimosa/MimosaMusicBack%d.png", 2));
}

void CUi_Default::Initialize_MimosaCutinResource()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ChaiCutIn",
			L"../Bin/Resources/Textures/Ui/MimosaCutin/ChaiCutIn.png"));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_MimosaCutIn",
			L"../Bin/Resources/Textures/Ui/MimosaCutin/MimosaCutIn.png"));
}


shared_ptr<CUi_Default> CUi_Default::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_Default
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_Default(pDevice, pContext) { }
	};

	shared_ptr<CUi_Default> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_Default::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_Default
	{
		MakeSharedEnabler(const CUi_Default& rhs) : CUi_Default(rhs) { }
	};

	shared_ptr<CUi_Default> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
