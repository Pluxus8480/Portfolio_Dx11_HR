#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "GameObject.h"

#include "PlayerManager.h"
#include "GameManager.h"
#include "Imgui_Manager.h"
#include "CUi_Mgr.h"

#include "Component_Manager.h"
#include "Load_Include.h"

#include "ResourceWizard.h"
#include "Beat_Manager.h"

#include "CombatManager.h"
#include "Effect_Manager.h"

#include "GimmickManager.h"
#include "MapManager.h"

#include "BackGround.h"
#include "CCineSubTitleMgr.h"

#ifdef PERFECTION_LOAD

#else
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
}

CMainApp::~CMainApp()
{
	Free();
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.hInstance = g_hInst;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iLoadingLevelIdx = LEVEL_LOADING;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	m_pImguiManager = CImgui_Manager::Get_Instance();
	m_pImguiManager->Initialize(g_hWnd, m_pDevice, m_pContext);

	m_pPlayerManager = CPlayer_Manager::Get_Instance();
	m_pPlayerManager->Initialize(m_pDevice, m_pContext);

	m_pGameManager = CGame_Manager::Get_Instance();
	m_pGameManager->Initialize(m_pDevice, m_pContext);

	m_pCUiMgr = CUi_Mgr::Get_Instance();
	m_pCUiMgr->Initialize(m_pDevice, m_pContext);

	m_pBeat_Manager = CBeat_Manager::Get_Instance();
	m_pBeat_Manager->Initialize();

	m_pCombat_Manager = CCombat_Manager::Get_Instance();
	m_pCombat_Manager->Initialize(m_pDevice, m_pContext);

	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	m_pEffect_Manager->Initialize(m_pDevice, m_pContext);

	auto pGimmickManager = CGimmickManager::Get_Instance();
	if (!pGimmickManager)
		return E_FAIL;

	m_pMapManager = CMapManager::Get_Instance();
	if (!m_pMapManager)
		return E_FAIL;

	m_pCineSubTitleMgr = CCineSubTitleMgr::Get_Instance();
	if (!m_pCineSubTitleMgr)
		return E_FAIL;
	m_pCineSubTitleMgr->Initialize();

	if (FAILED(Ready_Prototoype_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_Prototoype_Objects()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	auto pAnimShader = m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh");
	auto pMeshShader = m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxMesh");

	m_pGameManager->Bind_DefaultNormal(static_pointer_cast<CShader>(pAnimShader), "g_Normal_Texture");
	m_pGameManager->Bind_DefaultNormal(static_pointer_cast<CShader>(pMeshShader), "g_Normal_Texture");
	
	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pBeat_Manager->Tick(fTimeDelta);
	m_pPlayerManager->Tick_Engine(fTimeDelta);
	m_pGameManager->Tick_Engine(fTimeDelta);
	m_pCombat_Manager->Tick_Engine(fTimeDelta);

	m_pGameInstance->Tick_Engine(fTimeDelta);
	m_pCUiMgr->Tick_Engine(fTimeDelta);

	m_pImguiManager->Tick_Engine(fTimeDelta);
	m_pEffect_Manager->Tick_Engine(fTimeDelta);

	m_pCineSubTitleMgr->Tick_Engine(fTimeDelta);

	m_pMapManager->Update();
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();
	m_pCineSubTitleMgr->Render();

	m_pImguiManager->Imgui_Render();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	CResourceWizard::LoadResources(CUi_LoadingSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototoype_Component_ForStatic()
{
	/*Static Collideres*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/*Static Shader*/

	///* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UiGauge"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UiGauge.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	///* For.Prototype_Component_Shader_VtxRectInstance */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectNorm"),
	//	CVIBuffer_RectNorm::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"),
		CCharacterController::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"),
		CPhysXCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Octree"),
		COctComp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Ready_Prototoype_Objects()
{
#pragma region JAEHWI
	if (FAILED(m_pGameInstance->Add_Prototype(CBackGround::ObjID, CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSlate_Platform::ObjID, CSlate_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTunnel::ObjID, CTunnel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLift::ObjID, CLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBrake_Part::ObjID, CBrake_Part::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPanel_Lift::ObjID, CPanel_Lift::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSmoke_Cover::ObjID, CSmoke_Cover::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CConveyorBelt::ObjID, CConveyorBelt::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLaserCircuit::ObjID, CLaserCircuit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLaserPod::ObjID, CLaserPod::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLaserPillar::ObjID, CLaserPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLaserBoard::ObjID, CLaserBoard::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLaserReward::ObjID, CLaserReward::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGlassWall::ObjID, CGlassWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CReverbGaugeRoom::ObjID, CReverbGaugeRoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWoodenBox::ObjID, CWoodenBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPickUpItem::ObjID, CPickUpItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSkySphere::ObjID, CSkySphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CInvisibleWall::ObjID, CInvisibleWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFirstBattleWall::ObjID, CFirstBattleWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCameraController::ObjID, CCameraController::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEnemy_Lift::ObjID, CEnemy_Lift::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Fire_Loop::ObjID, CParticle_Fire_Loop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(CInvisibleCylinderWall::ObjID, CInvisibleCylinderWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Kale::ObjID, CMonster_Kale::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_Sword::ObjID, CKale_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_Funnel::ObjID, CKale_Funnel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_BArm::ObjID, CKale_BArm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_FArm::ObjID, CKale_FArm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_SArm::ObjID, CKale_SArm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(C808_Playable::ObjID, C808_Playable::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_3DDefault::ObjID, CUi_3DDefault::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_FireWall::ObjID, CKale_FireWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGiantRobot::ObjID, CGiantRobot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRhythmJump::ObjID, CRhythmJump::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_BigSword::ObjID, CKale_BigSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPeppermintContainer::ObjID, CPeppermintContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMacaronCrane::ObjID, CMacaronCrane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLight_Controller::ObjID, CLight_Controller::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMisc_Tower_Elec::ObjID, CMisc_Tower_Elec::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_Floor::ObjID, CMimosa_Floor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosaWall::ObjID, CMimosaWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTrulyInvisibleWall::ObjID, CTrulyInvisibleWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDrakken_Laser::ObjID, CDrakken_Laser::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGiantRobot2::ObjID, CGiantRobot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGiantRobot3::ObjID, CGiantRobot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCNMN_Playable::ObjID, CCNMN_Playable::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBeamCylinder::ObjID, CBeamCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGiantRobot_Arm::ObjID, CGiantRobot_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTriggerBox_Level::ObjID, CTriggerBox_Level::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CODS_Controller::ObjID, CODS_Controller::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
#pragma region AFTERIMAGE
	CAfterImage::AFTERIMAGE_DESC aiDesc;
	aiDesc.bAnimated = true;
	aiDesc.fLifeTime = 0.5f;
	aiDesc.iMaxImage = 10;
	aiDesc.iRenderPass = (_uint)ANIMMESH_PASS::MonoColor;
	aiDesc.vColor = { 0.2f,1.0f,0.8f, 0.5f };
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AfterImage_Anim_500milsec_10"),
		CAfterImage::Create(m_pDevice, m_pContext, aiDesc))))
		throw;

#pragma endregion //AFTERIMAGE
	
#pragma endregion //JAEHWI

#pragma region BEOMSEOK
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_Sprite::ObjID, CUi_Sprite::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_Font::ObjID, CUi_Font::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_LoadingSystem::ObjID, CUi_LoadingSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerBar::ObjID, CUi_PlayerBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TalkSystem::ObjID, CUi_TalkSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerHp::ObjID, CUi_PlayerHp::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerReverb::ObjID, CUi_PlayerReverb::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PlayerPartnerGauge::ObjID, CUi_PlayerPartnerGauge::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_DashEffect::ObjID, CUi_DashEffect::Create(m_pDevice, m_pContext))))
		assert(false);
#pragma endregion //BEOMSEOK

#pragma region JEONGRAE
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_Default::ObjID, CUi_Default::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_RankSystem::ObjID, CUi_RankSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_ComboNum::ObjID, CUi_ComboNum::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_ComboWordHits::ObjID, CUi_ComboWordHits::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_ScoreSystem::ObjID, CUi_ScoreSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_BeatMarker::ObjID, CUi_BeatMarker::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_BeatMarkerSystem::ObjID, CUi_BeatMarkerSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_BeatJudge::ObjID, CUi_BeatJudge::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_FinalResult::ObjID, CUi_FinalResult::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_FinalResultSystem::ObjID, CUi_FinalResultSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageResultSystem::ObjID, CUi_StageResultSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageCorus::ObjID, CUi_StageCorus::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageResult_Bonus::ObjID, CUi_StageResult_Bonus::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StageResult_FinalScore::ObjID, CUi_StageResult_FinalScore::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreSystem::ObjID, CUi_StoreSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_UpRightBack::ObjID, CUi_UpRightBack::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreEntry::ObjID, CUi_StoreEntry::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreAttack::ObjID, CUi_StoreAttack::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreSkill::ObjID, CUi_StoreSkill::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreItem::ObjID, CUi_StoreItem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreChip::ObjID, CUi_StoreChip::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreBar::ObjID, CUi_StoreBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreDesc::ObjID, CUi_StoreDesc::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_Chip::ObjID, CUi_Chip::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreChipDesc::ObjID, CUi_StoreChipDesc::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreBuyBar::ObjID, CUi_StoreBuyBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreEdge::ObjID, CUi_StoreEdge::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreSkillEquip::ObjID, CUi_StoreSkillEquip::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_EquipDefault::ObjID, CUi_EquipDefault::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StoreEquipBar::ObjID, CUi_StoreEquipBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_SkillCutScene::ObjID, CUi_SkillCutScene::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_CommandIcon::ObjID, CUi_CommandIcon::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_GearSystem::ObjID, CUi_GearSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_RekkaSystem::ObjID, CUi_RekkaSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_RekkaHp::ObjID, CUi_RekkaHp::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_RekkaDeco::ObjID, CUi_RekkaDeco::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_RekkaDecoEffect::ObjID, CUi_RekkaDecoEffect::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_KorsicaFace::ObjID, CUi_KorsicaFace::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_KorsicaSystem::ObjID, CUi_KorsicaSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_KorsicaHpBar::ObjID, CUi_KorsicaHpBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_EnemySpecialAttack::ObjID, CUi_EnemySpecialAttack::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TreasureBoxSystem::ObjID, CUi_TreasureBoxSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TreasureBox::ObjID, CUi_TreasureBox::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_FightString::ObjID, CUi_FightString::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_PartIcon::ObjID, CUi_PartIcon::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_JudgeWord::ObjID, CUi_JudgeWord::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_SuccessSystem::ObjID, CUi_SuccessSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CEvent_Chai::ObjID, CEvent_Chai::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Korsica::ObjID, CMonster_Korsica::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CKorsicaCamera::ObjID, CKorsicaCamera::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_ComboButton::ObjID, CUi_ComboButton::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_BeatMeter::ObjID, CUi_BeatMeter::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_GeneratorSystem::ObjID, CUi_GeneratorSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_MapInterAction::ObjID, CUi_MapInterAction::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TramTimer::ObjID, CUi_TramTimer::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TramGauge::ObjID, CUi_TramGauge::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_TramMacaronSign::ObjID, CUi_TramMacaronSign::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_StunGaugeSystem::ObjID, CUi_StunGaugeSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_KaleHpBar::ObjID, CUi_KaleHpBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_KaleSystem::ObjID, CUi_KaleSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_KaleGroggyGage::ObjID, CUi_KaleGroggyGage::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_LogoButtonSystem::ObjID, CUi_LogoButtonSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_SpecialComboAttackSystem::ObjID, CUi_SpecialComboAttackSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_MimosaSystem::ObjID, CUi_MimosaSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_MimosaBar::ObjID, CUi_MimosaBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_DefaultBar::ObjID, CUi_DefaultBar::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_MImosaMusicBattleSystem::ObjID, CUi_MImosaMusicBattleSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CJeongGimmicBaseDefault::ObjID, CJeongGimmicBaseDefault::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CKaleBoss_ObjSystem::ObjID, CKaleBoss_ObjSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CKale_Antenna::ObjID, CKale_Antenna::Create(m_pDevice, m_pContext))))
		assert(false);	
	if (FAILED(m_pGameInstance->Add_Prototype(CUI_Event_Cut::ObjID, CUI_Event_Cut::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CKaleElevatorGenerator::ObjID, CKaleElevatorGenerator::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CContainerBreakPannel::ObjID, CContainerBreakPannel::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CRobotPunchBreakWall::ObjID, CRobotPunchBreakWall::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CRobotPunchStaticWall::ObjID, CRobotPunchStaticWall::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CRobotBeamContainer::ObjID, CRobotBeamContainer::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CScroll2DContainer::ObjID, CScroll2DContainer::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_DefaultLife::ObjID, CUi_DefaultLife::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUI_MimosaCutin::ObjID, CUI_MimosaCutin::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CTriggerBox::ObjID, CTriggerBox::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CRailContainer::ObjID, CRailContainer::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CBattle3rdAfter::ObjID, CBattle3rdAfter::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CBattle3rdFloor::ObjID, CBattle3rdFloor::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_CNMNRazer::ObjID, CUi_CNMNRazer::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_CNMNRazorPart::ObjID, CUi_CNMNRazorPart::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_DefaultFont::ObjID, CUi_DefaultFont::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_SkillChangeSystem::ObjID, CUi_SkillChangeSystem::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CKaleBoss_CineObj::ObjID, CKaleBoss_CineObj::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_EnemySpecialAttackPart::ObjID, CUi_EnemySpecialAttackPart::Create(m_pDevice, m_pContext))))
		assert(false);
	if (FAILED(m_pGameInstance->Add_Prototype(CUi_CineAttackSystem::ObjID, CUi_CineAttackSystem::Create(m_pDevice, m_pContext))))
		assert(false);
#pragma endregion JEONGRAE


#pragma region MINA
	//if (FAILED(m_pGameInstance->Add_Prototype(CMapObject_Static::ObjID, CMapObject_Static::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMapObject_Anim::ObjID, CMapObject_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPartDoor::ObjID, CPartDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPartDoorFrame::ObjID, CPartDoorFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CProdDeptDoor::ObjID, CProdDeptDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTrigger::ObjID, CTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBulbTrigger::ObjID, CBulbTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMovingWall::ObjID, CMovingWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CElevator::ObjID, CElevator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSpawnTrigger_Monster::ObjID, CSpawnTrigger_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMonsterSpawner::ObjID, CMonsterSpawner::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCommonSwitch::ObjID, CCommonSwitch::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSwitchButton::ObjID, CSwitchButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSwitchFence::ObjID, CSwitchFence::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CButton_Platform::ObjID, CButton_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSpawnTrigger_Platform::ObjID, CSpawnTrigger_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCatwalk_Bridge::ObjID, CCatwalk_Bridge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFloorSwitch::ObjID, CFloorSwitch::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFloorSwitchButton::ObjID, CFloorSwitchButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CComboSwitch::ObjID, CComboSwitch::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CComboSwitchButton::ObjID, CComboSwitchButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLargeElevator::ObjID, CLargeElevator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGearWall::ObjID, CGearWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHanging_Platform::ObjID, CHanging_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CQACenterDoor::ObjID, CQACenterDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFadeElevator::ObjID, CFadeElevator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEnemyGate::ObjID, CEnemyGate::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGenerator::ObjID, CGenerator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Logo::ObjID, CRekka_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEventTrigger::ObjID, CEventTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFloorSign::ObjID, CFloorSign::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFan_Blade::ObjID, CFan_Blade::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFan_Platform::ObjID, CFan_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSignBoard_Platform::ObjID, CSignBoard_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSignBoard::ObjID, CSignBoard::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBattleAreaDoor_PD::ObjID, CBattleAreaDoor_PD::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGaraElevator::ObjID, CGaraElevator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLevelUp_Machine::ObjID, CLevelUp_Machine::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCullingBox::ObjID, CCullingBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKrscRoomWall::ObjID, CKrscRoomWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSignPlane::ObjID, CSignPlane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRhythmWall::ObjID, CRhythmWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDMGConsole::ObjID, CDMGConsole::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_GenCover::ObjID, CRekka_GenCover::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Monitor_S::ObjID, CRekka_Monitor_S::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Monitor_L::ObjID, CRekka_Monitor_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_PhaseText::ObjID, CRekka_PhaseText::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_MonitorBase::ObjID, CRekka_MonitorBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWireMagnet::ObjID, CWireMagnet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekkaRoomDoor::ObjID, CRekkaRoomDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CZipLine_RailPart::ObjID, CZipLine_RailPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CZipLine_Rail::ObjID, CZipLine_Rail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CZipLine_Magnet::ObjID, CZipLine_Magnet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CZipLine_Obstacle::ObjID, CZipLine_Obstacle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMovingFloor::ObjID, CMovingFloor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSteelFrame_Platform::ObjID, CSteelFrame_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKrsc_Generator::ObjID, CKrsc_Generator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSpectraMeshGate::ObjID, CSpectraMeshGate::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSectionCullingBox::ObjID, CSectionCullingBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBuiltIn_Magnet::ObjID, CBuiltIn_Magnet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCraneMagnet::ObjID, CCraneMagnet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEmissiveArrow::ObjID, CEmissiveArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTalkTrigger::ObjID, CTalkTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_BackMonitor::ObjID, CMimosa_BackMonitor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_RingText::ObjID, CMimosa_RingText::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_BackLEDText::ObjID, CMimosa_BackLEDText::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_RoofLEDText::ObjID, CMimosa_RoofLEDText::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_Visualizer::ObjID, CMimosa_Visualizer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_Speaker::ObjID, CMimosa_Speaker::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_LightShaft::ObjID, CMimosa_LightShaft::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSpectra_FanBlade::ObjID, CSpectra_FanBlade::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSpectra_FanMachine::ObjID, CSpectra_FanMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFree_SignBoard::ObjID, CFree_SignBoard::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_LightShaft::ObjID, CRekka_LightShaft::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRhythm_RobotArm::ObjID, CRhythm_RobotArm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRhythm_CleanSystem::ObjID, CRhythm_CleanSystem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGrass::ObjID, CGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGrassMatrix::ObjID, CGrassMatrix::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion MINA

#pragma region JAESEOK
	if (FAILED(m_pGameInstance->Add_Prototype(CFree_SK_Object::ObjID, CFree_SK_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFree_SM_Object::ObjID, CFree_SM_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Saber::ObjID, CMonster_Saber::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSaber_Weapon::ObjID, CSaber_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Gunner::ObjID, CMonster_Gunner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Baboo::ObjID, CMonster_Baboo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Samurai::ObjID, CMonster_Samurai::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Rekka::ObjID, CMonster_Rekka::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Pipe_L::ObjID, CRekka_Pipe_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Pipe_R::ObjID, CRekka_Pipe_R::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Long_Pipe::ObjID, CRekka_Long_Pipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Gauntlet::ObjID, CRekka_Gauntlet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Pipe_Door::ObjID, CRekka_Pipe_Door::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Generator::ObjID, CRekka_Generator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CTool_Cine_Camera::ObjID, CTool_Cine_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CCineTrigger::ObjID, CCineTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCine_Actor::ObjID, CCine_Actor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCine_Camera::ObjID, CCine_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// For Mimosa...
	GAMEINSTANCE->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Mimosa_WingTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Mimosa/T_VFX_tc_tk_em5000_TechniqueWave_01.png"), 1));

#pragma endregion JAESEOK

#pragma region SANGHYUN
	if (FAILED(m_pGameInstance->Add_Prototype(CChai::ObjID, CChai::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBasic_Guitar::ObjID, CBasic_Guitar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPeppermint::ObjID, CPeppermint::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMacaron::ObjID, CMacaron::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CKorsica::ObjID, CKorsica::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWeapon_Peppermint::ObjID, CWeapon_Peppermint::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWeapon_Korsica::ObjID, CWeapon_Korsica::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(C808_Companion::ObjID, C808_Companion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(C808_Cat::ObjID, C808_Cat::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(C808_Controller::ObjID, C808_Controller::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Mimosa::ObjID, CMonster_Mimosa::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWeapon_Mimosa::ObjID, CWeapon_Mimosa::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_Letter_Bomb::ObjID, CMimosa_Letter_Bomb::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBackDancer::ObjID, CBackDancer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMimosa_SpotLight::ObjID, CMimosa_SpotLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMusician_Robot::ObjID, CMusician_Robot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMusician_Instrument::ObjID, CMusician_Instrument::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_SwordMachine::ObjID, CMonster_SwordMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSwordMachine_Sword::ObjID, CSwordMachine_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMonster_Blaze::ObjID, CMonster_Blaze::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CChai_Random_Sound::ObjID, CChai_Random_Sound::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPickMeUp_Can::ObjID, CPickMeUp_Can::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion SANGHYUN

#pragma region MINHYUK

#pragma endregion MINHYUK

#pragma region Effect

	if (FAILED(m_pGameInstance->Add_Prototype(CBigSlash_Effect::ObjID, CBigSlash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CEnemySlash_Effect::ObjID, CEnemySlash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CFlare_Effect::ObjID, CFlare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Ring_Effect::ObjID, CImpact_Ring_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Shock_Effect::ObjID, CImpact_Shock_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Smack_Shock_Effect::ObjID, CImpact_Smack_Shock_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Spark_Effect::ObjID, CImpact_Spark_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CTestEffect::ObjID, CTestEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CTexture_Effect::ObjID, CTexture_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CBeam_Effect::ObjID, CBeam_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CSnap_Impact_Effect::ObjID, CSnap_Impact_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSnap_Font_Effect::ObjID, CSnap_Font_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSmoke_Effect::ObjID, CSmoke_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CConvexLens_Effect::ObjID, CConvexLens_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CStarSymbol_Effect::ObjID, CStarSymbol_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Container_Donk::ObjID, CFont_Effect_Container_Donk::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CZap_Effect::ObjID, CZap_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CExplosion_Effect::ObjID, CExplosion_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBoom_Font_Effect::ObjID, CBoom_Font_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHarmonic_Beam::ObjID, CHarmonic_Beam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLight_Effect::ObjID, CLight_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSwing_Effect::ObjID, CSwing_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFlat_Wave_Effect::ObjID, CFlat_Wave_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRing_Effect::ObjID, CRing_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSmack_Font_Effect::ObjID, CSmack_Font_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDot_Flare_Effect::ObjID, CDot_Flare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBeamDot_Flare_Effect::ObjID, CBeamDot_Flare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMesh_Shock_Effect::ObjID, CMesh_Shock_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CShock_Effect_Font_A::ObjID, CShock_Effect_Font_A::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CShock_Effect_Font_D::ObjID, CShock_Effect_Font_D::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPanel_Warning::ObjID, CPanel_Warning::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Spark_Lightning::ObjID, CRekka_Spark_Lightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Spark_Impact::ObjID, CRekka_Spark_Impact::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDecal_Effect::ObjID, CDecal_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDecal_Ring_Effect::ObjID, CDecal_Ring_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSchwing_Font::ObjID, CSchwing_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWire::ObjID, CWire::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPodLaser::ObjID, CPodLaser::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSpecial_Effect_PS_Spark::ObjID, CSpecial_Effect_PS_Spark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLaser_Warning_Effect::ObjID, CLaser_Warning_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLift_Speed_Effect::ObjID, CLift_Speed_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CUnlimited_Void_Effect::ObjID, CUnlimited_Void_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CShash_Font::ObjID, CShash_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Bam_A::ObjID, CFont_Effect_Bam_A::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Bam_B::ObjID, CFont_Effect_Bam_B::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Bam_M::ObjID, CFont_Effect_Bam_M::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Zing::ObjID, CFont_Effect_Zing::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Shwaa::ObjID, CFont_Effect_Shwaa::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Phaa::ObjID, CFont_Effect_Phaa::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Shoom::ObjID, CFont_Effect_Shoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_HNGH::ObjID, CFont_Effect_HNGH::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBalloon_Effect::ObjID, CBalloon_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(CWave_Effect::ObjID, CWave_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSquare_Flare_Effect::ObjID, CSquare_Flare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLandImpact_Effect::ObjID, CLandImpact_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CLandJumpImpact_Effect::ObjID, CLandJumpImpact_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBarrier::ObjID, CBarrier::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFire_Effect::ObjID, CFire_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCyclone::ObjID, CCyclone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParry_Effect::ObjID, CParry_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CProcedural_Cyclone_Effect::ObjID, CProcedural_Cyclone_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDash_Effect::ObjID, CDash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CImpact_Shock_Bigger_Effect::ObjID, CImpact_Shock_Bigger_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHammerOn_Effect::ObjID, CHammerOn_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CGlow_Slash_Effect::ObjID, CGlow_Slash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Effect::ObjID, CParticle_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSlam_Effect::ObjID, CSlam_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDamage_Effect::ObjID, CDamage_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDamage_Special::ObjID, CDamage_Special::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDamage_SP::ObjID, CDamage_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDamage_PepperMint::ObjID, CDamage_PepperMint::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHit_Base::ObjID, CHit_Base::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHit_Hard::ObjID, CHit_Hard::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHit_Special::ObjID, CHit_Special::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CHit_Electro::ObjID, CHit_Electro::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFlat_Fire_Effect::ObjID, CFlat_Fire_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Fire::ObjID, CParticle_Fire::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRandom_Laser::ObjID, CRandom_Laser::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CMirror_Ball::ObjID, CMirror_Ball::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSmoke_Explode_Effect::ObjID, CSmoke_Explode_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWarningDecal_Effect::ObjID, CWarningDecal_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Rekka_Slam::ObjID, CFont_Effect_Rekka_Slam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CWarningDecalCircle_Effect::ObjID, CWarningDecalCircle_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Dash_Effect::ObjID, CRekka_Dash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(C808_Flare_Effect::ObjID, C808_Flare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CJustMark_Effect::ObjID, CJustMark_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRekka_Land_Spark::ObjID, CRekka_Land_Spark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFrustum_Flare_Effect::ObjID, CFrustum_Flare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSun_Flare_Effect::ObjID, CSun_Flare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Mimosa_Barrier::ObjID, CEffect_Mimosa_Barrier::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Doon::ObjID, CFont_Effect_Doon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Wvuong::ObjID, CFont_Effect_Wvuong::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBullet_Effect::ObjID, CBullet_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_ODSlash::ObjID, CEffect_ODSlash::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDecal_SpotLight::ObjID, CDecal_SpotLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBackground_Effect::ObjID, CBackground_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CExpandingCylinder::ObjID, CExpandingCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Lens::ObjID, CEffect_Lens::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTower_Elec::ObjID, CTower_Elec::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CRadial_Blur::ObjID, CRadial_Blur::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFireSwingEffect::ObjID, CFireSwingEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBallLighting::ObjID, CBallLighting::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_ShakeFilter::ObjID, CEffect_ShakeFilter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBG_Overrider::ObjID, CBG_Overrider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTabletScreen::ObjID, CTabletScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSound_Explode_Effect::ObjID, CSound_Explode_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CSpectra_Mark::ObjID, CSpectra_Mark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Spark::ObjID, CParticle_Spark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Anime_Spark::ObjID, CParticle_Anime_Spark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDecal_Special_Effect::ObjID, CDecal_Special_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_RaEx_Ex::ObjID, CFont_Effect_RaEx_Ex::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_RaEx_R::ObjID, CFont_Effect_RaEx_R::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_RaEx_A::ObjID, CFont_Effect_RaEx_A::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Gyaaang::ObjID, CFont_Effect_Gyaaang::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPepperMint_Effect_MF_1::ObjID, CPepperMint_Effect_MF_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CPepperMint_Effect_MF_2::ObjID, CPepperMint_Effect_MF_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_On::ObjID, CFont_Effect_On::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Charged::ObjID, CFont_Effect_Charged::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_FireWall::ObjID, CEffect_FireWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFireWall_Effect::ObjID, CFireWall_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Mimosa_DoremiCode::ObjID, CFont_Mimosa_DoremiCode::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Filter_Focus_Trigger::ObjID, CEffect_Filter_Focus_Trigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Mimosa_Panel::ObjID, CParticle_Mimosa_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Mimosa_Warning::ObjID, CEffect_Mimosa_Warning::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Line::ObjID, CEffect_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Sun_Flare_Extend::ObjID, CEffect_Sun_Flare_Extend::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Kale_Judgement::ObjID, CEffect_Kale_Judgement::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Kale_Barrier::ObjID, CEffect_Kale_Barrier::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CTracking_Laser::ObjID, CTracking_Laser::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CDecal_Flare_Effect::ObjID, CDecal_Flare_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Audience_Upper_Plane::ObjID, CEffect_Audience_Upper_Plane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Audience_Lower_Plane::ObjID, CEffect_Audience_Lower_Plane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_LightStick_Upper_Plane::ObjID, CEffect_LightStick_Upper_Plane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_LightStick_Lower_Plane::ObjID, CEffect_LightStick_Lower_Plane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_LightStick_Upper::ObjID, CEffect_LightStick_Upper::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_LightStick_Lower::ObjID, CEffect_LightStick_Lower::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_FireCracker_Normal::ObjID, CParticle_FireCracker_Normal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Directional_Light_Controler::ObjID, CEffect_Directional_Light_Controler::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CFont_Effect_Container_Spark::ObjID, CFont_Effect_Container_Spark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBlaze_Fire::ObjID, CBlaze_Fire::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CBlaze_FireStarter::ObjID, CBlaze_FireStarter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CParticle_Spark_Controller::ObjID, CParticle_Spark_Controller::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CCamera_Void_Effect::ObjID, CCamera_Void_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Judgement_Cut::ObjID, CEffect_Judgement_Cut::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(CEffect_Union::ObjID, CEffect_Union::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (IMGUIMANAGER != nullptr) IMGUIMANAGER->Init_Effect_Prototype();

	CResourceWizard::LoadResources(CSkySphere::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
#pragma endregion Effect

	CResourceWizard::LoadResources(CTexture_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	//CResourceWizard::LoadResources(CTestEffect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBigSlash_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEnemySlash_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFlare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CImpact_Ring_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CImpact_Shock_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CImpact_Spark_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBeam_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSnap_Impact_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSnap_Font_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSmoke_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CConvexLens_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CStarSymbol_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CZap_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CExplosion_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBoom_Font_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CHarmonic_Beam::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLight_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSwing_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFlat_Wave_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRing_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSmack_Font_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDot_Flare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBeamDot_Flare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CMesh_Shock_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPanel_Warning::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Spark_Lightning::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Spark_Impact::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDecal_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDecal_Ring_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWire::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPodLaser::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CImpact_Shock_Bigger_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLaser_Warning_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLift_Speed_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Zing::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Shwaa::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Phaa::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Shoom::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_HNGH::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFlat_Fire_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CParticle_Fire::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CParticle_Fire_Loop::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CBalloon_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFrustum_Flare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSun_Flare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CLandImpact_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CLandJumpImpact_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBarrier::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFire_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CParticle_Spark::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CCyclone::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CParry_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CProcedural_Cyclone_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CParticle_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CShock_Effect_Font_A::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CShock_Effect_Font_D::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSchwing_Font::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CShash_Font::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CWave_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSquare_Flare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Bam_A::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Bam_B::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Bam_M::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDash_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CHammerOn_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CGlow_Slash_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSlam_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Rekka_Slam::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDecal_Special_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CRekka_Dash_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpecial_Effect_PS_Spark::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_RaEx_Ex::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_RaEx_A::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_RaEx_R::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(C808_Flare_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CJustMark_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Gyaaang::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CPepperMint_Effect_MF_1::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_On::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Charged::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_FireWall::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFireWall_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Mimosa_DoremiCode::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_SuccessSystem::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CParticle_Mimosa_Panel::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_Mimosa_Barrier::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_Mimosa_Warning::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_Sun_Flare_Extend::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_Kale_Barrier::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Doon::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Wvuong::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBullet_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_ODSlash::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDecal_SpotLight::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBackground_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CExpandingCylinder::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_Audience_Upper_Plane::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_Audience_Lower_Plane::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_Judgement_Cut::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CEffect_LightStick_Upper_Plane::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_LightStick_Lower_Plane::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_LightStick_Upper::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CEffect_LightStick_Lower::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	CResourceWizard::LoadResources(CEffect_Lens::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CTower_Elec::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CDrakken_Laser::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Container_Donk::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFont_Effect_Container_Spark::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBeamCylinder::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CFireSwingEffect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CUi_DashEffect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBallLighting::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CBG_Overrider::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CTabletScreen::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSound_Explode_Effect::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);
	CResourceWizard::LoadResources(CSpectra_Mark::ObjID, LEVEL_STATIC, m_pGameInstance, m_pDevice, m_pContext);

	auto pEM = EFFECTMANAGER;

	pEM->Add_Union_Effect("Combo_Air_L_1_1", 2);
	pEM->Add_Union_Effect("Combo_Air_LL_2_1", 2);
	pEM->Add_Union_Effect("Combo_Air_LLL_3_1", 2);
	pEM->Add_Union_Effect("Combo_Air_LLLL_4", 2);

	pEM->Add_Union_Effect("Combo_LLLL_1_1", 2);
	pEM->Add_Union_Effect("Combo_LLLL_2_1", 2);
	pEM->Add_Union_Effect("Combo_LLLL_3_1", 2);
	pEM->Add_Union_Effect("Combo_LLLL_4_1", 2);
	pEM->Add_Union_Effect("Combo_LLLL_4_2", 4);

	pEM->Add_Union_Effect("Combo_L_LL_2_1", 2);
	pEM->Add_Union_Effect("Combo_L_LL_3_1", 2);

	pEM->Add_Union_Effect("Combo_LL_LLL_3_1", 2);
	pEM->Add_Union_Effect("Combo_LL_LLL_4_1", 2);
	pEM->Add_Union_Effect("Combo_LL_LLL_5_1", 2);

	pEM->Add_Union_Effect("Combo_RRR_1_1", 2);
	pEM->Add_Union_Effect("Combo_RRR_2_1", 2);
	pEM->Add_Union_Effect("Combo_RRR_3_1", 2);

	pEM->Add_Union_Effect("Combo_RLL_2_1", 2);
	pEM->Add_Union_Effect("Combo_RLL_4_1", 2);
	pEM->Add_Union_Effect("Combo_RLL_4_2", 2);

	pEM->Add_Union_Effect("Combo_LRR_2_1", 2);
	pEM->Add_Union_Effect("Combo_LRR_3_1", 2);
	pEM->Add_Union_Effect("Combo_LRR_3_2", 4);

	pEM->Add_Union_Effect("stomp_box_land", 2);//60f


	pEM->Add_Union_Effect("Combo_Air_LR", 2);
	pEM->Add_Union_Effect("Combo_Air_LRR", 2);
	pEM->Add_Union_Effect("Harmonic_Beam", 2);//60f

	pEM->Add_Union_Effect("Snap", 5);//60f


	pEM->Add_Union_Effect("Dash_Flash", 2);
	pEM->Add_Union_Effect("Dash_Left", 2);
	pEM->Add_Union_Effect("Dash_Right", 2);
	pEM->Add_Union_Effect("Dash_Front", 2);
	pEM->Add_Union_Effect("Dash_Front_2", 2);
	pEM->Add_Union_Effect("Dash_Front_3", 2);
	pEM->Add_Union_Effect("Dash_Air_Front", 2);
	pEM->Add_Union_Effect("Dash_Back", 2);

	pEM->Add_Union_Effect("Effect_HammerOn", 2);

	pEM->Add_Union_Effect("HitEffect", 10);
	pEM->Add_Union_Effect("Big_HitEffect", 20);
	pEM->Add_Union_Effect("PepperMint_HitEffect", 10);

	pEM->Add_Union_Effect("JumpImpack", 5);
	pEM->Add_Union_Effect("Jump_Effect_2", 2);

	pEM->Add_Union_Effect("Double_Jump", 2);

	pEM->Add_Union_Effect("Magnet_Spark", 10);

	pEM->Add_Union_Effect("Battle_Start", 2);

	pEM->Add_Union_Effect("Enemy_Boom", 5);

	pEM->Add_Union_Effect("Gimmick_Valve_Smoke", 2);
	pEM->Add_Union_Effect("Gimmick_Valve_Smoke_2", 2);
	pEM->Add_Union_Effect("Gimmick_Valve_Roll", 2);

	pEM->Add_Union_Effect("Pipe_Smoke_Explode_2_1", 10);
	pEM->Add_Union_Effect("Pipe_Smoke_Explode_1_1", 10);

	pEM->Add_Union_Effect("Pipe_Smoke_Explode_Up_1", 5);
	pEM->Add_Union_Effect("Pipe_Smoke_Explode_Up_2", 5);

	pEM->Add_Union_Effect("Pipe_Smoke_Explode_Loop_1", 20);
	pEM->Add_Union_Effect("Pipe_Smoke_Explode_Loop_2", 20);

	pEM->Add_Union_Effect("em0100_warningsign", 5);
	pEM->Add_Union_Effect("em0100_attack_000", 5);
	pEM->Add_Union_Effect("em0100_attack_001", 5);

	pEM->Add_Union_Effect("em2000_atk_56_warningSign", 2);
	pEM->Add_Union_Effect("em2000_atk_56", 2);
	pEM->Add_Union_Effect("em2000_atk_100_warningSign", 2);
	pEM->Add_Union_Effect("em2000_atk_100", 2);

	pEM->Add_Union_Effect("em0200_atk-gun_002", 5);
	pEM->Add_Union_Effect("em0200_atk_001", 5);

	pEM->Add_Union_Effect("em0400_shield_flare", 5);
	pEM->Add_Union_Effect("em0400_a_020_pre", 5);
	pEM->Add_Union_Effect("em0400_a_020_land", 5);
	pEM->Add_Union_Effect("em0400_a_000_temp", 5);
	pEM->Add_Union_Effect("em0400_a_050_pre", 5);
	pEM->Add_Union_Effect("em_0400_a_050_warning", 5);
	pEM->Add_Union_Effect("em_0400_a_050_land", 5);

	pEM->Add_Union_Effect("em_0400_a_040_land", 5);
	pEM->Add_Union_Effect("em_0400_a_041_land", 5);

	pEM->Add_Union_Effect("em0400_a_030_land", 5);
	pEM->Add_Union_Effect("em0400_a_030_post", 5);

	pEM->Add_Union_Effect("em0400_a_010", 5);

	pEM->Add_Union_Effect("Chai_Parry", 5);
	pEM->Add_Union_Effect("Parry_Spark", 3);

	pEM->Add_Union_Effect("LandingSmoke", 100);
	pEM->Add_Union_Effect("LargeLandingSmoke", 3);

	pEM->Add_Union_Effect("Spark_Fountain", 8);

	pEM->Add_Union_Effect("Chai_Special_Slide_1", 2);
	pEM->Add_Union_Effect("Chai_Special_Slide_2", 2);
	pEM->Add_Union_Effect("Chai_Special_Slide_2_F", 2);

	pEM->Add_Union_Effect("Enemy_Twitch", 2);
	pEM->Add_Union_Effect("Spawn_Effect", 8);

	pEM->Add_Union_Effect("808_Just_Pep", 2);
	pEM->Add_Union_Effect("808_Just_Mac", 2);
	pEM->Add_Union_Effect("808_Just_Kor", 2);

	pEM->Add_Union_Effect("O_D_Slash", 2);
	pEM->Add_Union_Effect("Chai_EventMimosa_010_0", 8);
	pEM->Add_Union_Effect("Chai_EventMimosa_020_0", 8);
	pEM->Add_Union_Effect("Chai_EventMimosa_030_0", 8);
	pEM->Add_Union_Effect("Chai_EventMimosa_040_0", 8);
	pEM->Add_Union_Effect("Chai_EventMimosa_Spark", 16);
	pEM->Add_Union_Effect("Chai_EventMimosa_Fail", 8);

	pEM->Add_Union_Effect("Unlimited_Void", 3);
	pEM->Add_Union_Effect("808Scratch_Kale", 1);

	pEM->Add_Union_Effect("Tower_Elec", 16);
	pEM->Add_Union_Effect("Tower_Elec1", 16);

	pEM->Add_Union_Effect("RadialBlur_Up", 4);
	pEM->Add_Union_Effect("RadialBlur_1Sec_In", 12);
	pEM->Add_Union_Effect("RadialBlur_1Sec_Out", 12);
	pEM->Add_Union_Effect("RadialBlur_1000Sec", 12);
#pragma region Rekka_Effect
	pEM->Add_Union_Effect("Rekka_Dash_Smoke", 2);
	pEM->Add_Union_Effect("Rekka_Dash_Smoke_End", 2);
	pEM->Add_Union_Effect("Rekka_Dash_Left", 2);
	pEM->Add_Union_Effect("Rekka_Dash_Right", 2);
	pEM->Add_Union_Effect("Rekka_Dash_Front", 2);
	pEM->Add_Union_Effect("Rekka_Dash_Back", 2);
	pEM->Add_Union_Effect("Rekka_Land_Electric", 2);
	pEM->Add_Union_Effect("Rekka_Charged", 2);

	pEM->Add_Union_Effect("Rekka_atk_400_1", 2);
	pEM->Add_Union_Effect("Rekka_atk_400_2", 2);
	pEM->Add_Union_Effect("Rekka_atk_400_3", 2);
	pEM->Add_Union_Effect("Rekka_atk_400_4", 2);
	pEM->Add_Union_Effect("Rekka_atk_400_5", 2);

	pEM->Add_Union_Effect("Rekka_atk_110_1", 2);
	pEM->Add_Union_Effect("Rekka_atk_110_2", 2);
	pEM->Add_Union_Effect("Rekka_atk_110_3", 2);
	pEM->Add_Union_Effect("Rekka_atk_110_4", 2);

	pEM->Add_Union_Effect("Rekka_atk_330_main", 2);
	pEM->Add_Union_Effect("Rekka_atk_330_boom", 2);
	pEM->Add_Union_Effect("Rekka_atk_330_warningSign", 2);
	pEM->Add_Union_Effect("Rekka_atk_330_4", 2);

	pEM->Add_Union_Effect("Rekka_atk_pipe_020_1", 2);
	pEM->Add_Union_Effect("Rekka_atk_pipe_020_2", 2);

	pEM->Add_Union_Effect("Rekka_Door_Breaching", 2);
	pEM->Add_Union_Effect("Door_Shock_Effect", 2);

	pEM->Add_Union_Effect("Rekka_atk_pipe_101", 4);
	pEM->Add_Union_Effect("Rekka_atk_pipe_102", 1);
	pEM->Add_Union_Effect("em2105_atk_300", 2);
	pEM->Add_Union_Effect("em2106_atk_300", 2);
	pEM->Add_Union_Effect("Rekka_atk_pipe_201_swing", 2);
	pEM->Add_Union_Effect("Rekka_atk_pipe_201_decal", 6);
	pEM->Add_Union_Effect("Rekka_atk_pipe_201_jump", 2);

	pEM->Add_Union_Effect("Rekka_Body_Spark", 50);
	pEM->Add_Union_Effect("Rekka_Pipe_Spark", 50);
	pEM->Add_Union_Effect("Rekka_Grab_Ready", 3);
	pEM->Add_Union_Effect("Rekka_Grab_Start", 3);
	pEM->Add_Union_Effect("Rekka_Grap_Roll", 3);
	pEM->Add_Union_Effect("Rekka_Chai_Grab_Air_1", 2);
	pEM->Add_Union_Effect("Rekka_Chai_Grab_Air_2", 2);
	pEM->Add_Union_Effect("Grab_Success", 2);

	pEM->Add_Union_Effect("Rekka_atk_170_1", 2);

	pEM->Add_Union_Effect("Platform_Cylinder_Effect", 4);
	pEM->Add_Union_Effect("Slate_Platform_Close", 3);
	pEM->Add_Union_Effect("Slate_Platform_Open", 3);

	pEM->Add_Union_Effect("FireWork", 6);

	pEM->Add_Union_Effect("Gunner_LockOn_1", 5);
	pEM->Add_Union_Effect("Gunner_Fire_1", 5);
	pEM->Add_Union_Effect("Gunner_Fire_2", 5);
	pEM->Add_Union_Effect("Chai_Rolling", 2);
	pEM->Add_Union_Effect("Chai_Suffer", 4);
	pEM->Add_Union_Effect("FireWall", 12);

	pEM->Add_Union_Effect("Tracking_Laser", 10);
	pEM->Add_Union_Effect("LightMagic", 2);
#pragma endregion

#pragma region PepperMint Effect

	pEM->Add_Union_Effect("PepperMint_200_1", 2);
	pEM->Add_Union_Effect("PepperMint_200_2", 2);

	pEM->Add_Union_Effect("PepperMint_201_1", 2);
	pEM->Add_Union_Effect("PepperMint_202_1", 2);

	pEM->Add_Union_Effect("Double_Riff_0", 5);

	pEM->Add_Union_Effect("DoubleRiff_Chai_Start", 2);
	pEM->Add_Union_Effect("DoubleRiff_Chai_Start_0", 2);

	pEM->Add_Union_Effect("Pep_Bullet", 20);

#pragma endregion
#pragma region Macaron

	pEM->Add_Union_Effect("Macaron_100", 2);
	pEM->Add_Union_Effect("Macaron_101_1", 2);
	pEM->Add_Union_Effect("Macaron_101_2", 2);

	
#pragma endregion
#pragma region Korsica_Partner


	pEM->Add_Union_Effect("Krck_atk_000", 2);
	pEM->Add_Union_Effect("Krck_atk_002", 2);


	pEM->Add_Union_Effect("Krck_Cyclone", 2);

	pEM->Add_Union_Effect("Twin_Assault_000", 2);
	pEM->Add_Union_Effect("Twin_Assault_001", 2);
	pEM->Add_Union_Effect("Twin_Assault_002", 2);
	pEM->Add_Union_Effect("Twin_Assault_003", 2);

	pEM->Add_Union_Effect("Twin_Assault_Krck_0", 2);
	pEM->Add_Union_Effect("Twin_Assault_Krck_1", 2);
	pEM->Add_Union_Effect("Twin_Assault_Krck_2", 2);
	pEM->Add_Union_Effect("Twin_Assault_Krck_3", 2);
	pEM->Add_Union_Effect("Twin_Assault_Krck_4", 2);
	pEM->Add_Union_Effect("Twin_Assault_Krck_5", 2);
	pEM->Add_Union_Effect("Twin_Assault_Krck_6", 2);

	pEM->Add_Union_Effect("Twin_Assault_Land_0", 2);
	pEM->Add_Union_Effect("Twin_Assault_Land_1", 2);

	pEM->Add_Union_Effect("Twin_Assault_Jump_0", 2);
	pEM->Add_Union_Effect("Twin_Assault_Jump_1", 2);
	pEM->Add_Union_Effect("Twin_Assault_Jump_2", 2);
	pEM->Add_Union_Effect("Twin_Assault_Jump_3", 2);

	pEM->Add_Union_Effect("Twin_Assault_End", 2);
	

#pragma endregion

#pragma region Korsica

	pEM->Add_Union_Effect("Korsica_Boss_Cyclone", 2);
	pEM->Add_Union_Effect("Korsica_220_Attack", 2);
	pEM->Add_Union_Effect("Korsica_210_Attack", 2);
	pEM->Add_Union_Effect("Korsica_110_Attack", 2);
	pEM->Add_Union_Effect("Korsica_310_Attack", 2);
	pEM->Add_Union_Effect("Korsica_410_Attack", 2);
	pEM->Add_Union_Effect("Korsica_420_Attack", 2);
	pEM->Add_Union_Effect("Korsica_430_Attack", 2);
	pEM->Add_Union_Effect("Korsica_431_Attack", 2);
	pEM->Add_Union_Effect("Korsica_610_Attack", 2);
	pEM->Add_Union_Effect("Korsica_620_Attack", 2);
	pEM->Add_Union_Effect("Korsica_621_Attack", 2);
	pEM->Add_Union_Effect("Korsica_622_Attack", 2);
	pEM->Add_Union_Effect("Korsica_630_Attack", 2);
	pEM->Add_Union_Effect("Korsica_631_Attack", 2);
	pEM->Add_Union_Effect("Korsica_632_Attack", 2);

	pEM->Add_Union_Effect("JumpImpack_Extend_220", 2);
	pEM->Add_Union_Effect("Korsica_Dash", 2);
	pEM->Add_Union_Effect("Korsica_Return", 2);
	pEM->Add_Union_Effect("Korsica_Jump", 2);
	pEM->Add_Union_Effect("Korsica_Land", 2);

	pEM->Add_Union_Effect("Korsica_Font_Zing", 2);
	pEM->Add_Union_Effect("Korsica_Font_Phaa", 2);
	pEM->Add_Union_Effect("Korsica_Font_Phaa_Extend", 2);
	pEM->Add_Union_Effect("Korsica_Font_Shwaa", 2);

	pEM->Add_Union_Effect("Font_On_Effect", 2);
	pEM->Add_Union_Effect("Chai_Electric_Shock", 10);
	pEM->Add_Union_Effect("Rekka_Rasengan", 2);
	pEM->Add_Union_Effect("Rekka_Spark_Impact", 2);
	pEM->Add_Union_Effect("Rekka_Groggy_1", 10);
	pEM->Add_Union_Effect("Rekka_Groggy_2", 10);

	pEM->Add_Union_Effect("Lift_Spark_LB", 3);
	pEM->Add_Union_Effect("Lift_Spark_RB", 3);
	pEM->Add_Union_Effect("Lift_Spark_LF", 3);
	pEM->Add_Union_Effect("Lift_Spark_RF", 3);

	pEM->Add_Union_Effect("Lift_Smoke_LB", 3);
	pEM->Add_Union_Effect("Lift_Smoke_RB", 3);
	pEM->Add_Union_Effect("Lift_Smoke_LF", 3);
	pEM->Add_Union_Effect("Lift_Smoke_RF", 3);
#pragma endregion

#pragma region KALE
	pEM->Add_Union_Effect("kale_counter_slash", 2);
	pEM->Add_Union_Effect("kale_atk_000_1", 2);
	pEM->Add_Union_Effect("kale_atk_000_2", 2);
	pEM->Add_Union_Effect("kale_atk_010", 2);
	pEM->Add_Union_Effect("kale_atk_030_1", 2);
	pEM->Add_Union_Effect("kale_atk_030_2", 2);

	pEM->Add_Union_Effect("kale_atk_040_warn", 2);

	pEM->Add_Union_Effect("kale_atk_080_1", 2);
	pEM->Add_Union_Effect("kale_atk_080_2", 2);
	pEM->Add_Union_Effect("kale_atk_080_3", 2);


	pEM->Add_Union_Effect("Exkale_counter_slash", 2);
	pEM->Add_Union_Effect("Exkale_atk_000_1", 2);
	pEM->Add_Union_Effect("Exkale_atk_000_2", 2);
	pEM->Add_Union_Effect("Exkale_atk_010", 2);

	pEM->Add_Union_Effect("Exkale_atk_030_1", 2);
	pEM->Add_Union_Effect("Exkale_atk_030_2", 2);

	pEM->Add_Union_Effect("Exkale_atk_080_1", 2);
	pEM->Add_Union_Effect("Exkale_atk_080_2", 2);
	pEM->Add_Union_Effect("Exkale_atk_080_3", 2);

	pEM->Add_Union_Effect("funnel_fire", 4);
	pEM->Add_Union_Effect("funnel_fire2", 4);
	pEM->Add_Union_Effect("funnel_warning", 4);

	pEM->Add_Union_Effect("kale_arm_000", 2);
	pEM->Add_Union_Effect("kale_arm_000_2", 2);

	pEM->Add_Union_Effect("kale_sarmchance_1", 2);

	pEM->Add_Union_Effect("kale_rp_1_1", 1);
	pEM->Add_Union_Effect("kale_rp_1_atk_0", 1);
	pEM->Add_Union_Effect("kale_rp_1_atk_1", 1);
	pEM->Add_Union_Effect("kale_rp_1_atk_2", 1);
	pEM->Add_Union_Effect("kale_rp_1_atk_3", 1);

	pEM->Add_Union_Effect("kale_rp_2_atk_0", 1);
	pEM->Add_Union_Effect("kale_rp_2_atk_1", 1);
	pEM->Add_Union_Effect("kale_rp_2_atk_2", 1);
	pEM->Add_Union_Effect("kale_rp_2_atk_3", 1);

	pEM->Add_Union_Effect("kale_rp_2_atk_3", 1);

	pEM->Add_Union_Effect("Kale_Lighting_Lod_0", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_1", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_2", 10);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_3", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_4", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_5", 10);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_6", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_7", 2);

	pEM->Add_Union_Effect("Kale_Judgement", 5);
	pEM->Add_Union_Effect("Kale_Judgement_Extend", 5);
	pEM->Add_Union_Effect("Kale_Judgement_Warning", 5);
	pEM->Add_Union_Effect("Kale_Judgement_Fire", 5);

	pEM->Add_Union_Effect("Kale_Barrier", 2);

	pEM->Add_Union_Effect("Kale_Lighting_Lod_1_Extend", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_2_Extend", 10);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_4_Extend", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_5_Extend", 10);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_6_Extend", 2);
	pEM->Add_Union_Effect("Kale_Lighting_Lod_7_Extend", 2);

	pEM->Add_Union_Effect("Flame_Throw_10Sec", 2);
	pEM->Add_Union_Effect("Flame_Hold", 2);
	pEM->Add_Union_Effect("Chai_Burn", 4);
#pragma endregion

#pragma region MIMOSA
	pEM->Add_Union_Effect("Mimosa_Doremi_Do", 3);
	pEM->Add_Union_Effect("Mimosa_Doremi_Re", 3);
	pEM->Add_Union_Effect("Mimosa_Doremi_Mi", 3);
	pEM->Add_Union_Effect("Mimosa_Doremi_Fa", 3);
	pEM->Add_Union_Effect("Mimosa_Doremi_So", 3);
	pEM->Add_Union_Effect("Mimosa_Doremi_La", 3);
	pEM->Add_Union_Effect("Mimosa_Doremi_Ti", 3);

	pEM->Add_Union_Effect("Mimosa_Barrier_Parry", 4);

	pEM->Add_Union_Effect("Mimosa_Dance_LeftPoint", 14);
	pEM->Add_Union_Effect("Mimosa_Dance_RightPoint", 14);
	pEM->Add_Union_Effect("Mimosa_Dance_Clap", 14);
	pEM->Add_Union_Effect("Mimosa_Dance_Spin", 14);
	pEM->Add_Union_Effect("Mimosa_Dance_Pose", 14);

	pEM->Add_Union_Effect("Mimosa_Laser_Shoot", 5);
	pEM->Add_Union_Effect("Mimosa_Laser_Ready", 5);
	pEM->Add_Union_Effect("Mimosa_Laser_Fire_1", 5);
	pEM->Add_Union_Effect("Mimosa_Laser_Fire_2", 5);
	pEM->Add_Union_Effect("Mimosa_Kick", 2);
	pEM->Add_Union_Effect("Mimosa_Sweep_1", 2);
	pEM->Add_Union_Effect("Mimosa_Sweep_2", 2);

	pEM->Add_Union_Effect("Mimosa_Dash_Right", 2);
	pEM->Add_Union_Effect("Mimosa_Dash_Left", 2);

	pEM->Add_Union_Effect("Mimosa_Chai_Izime", 3);

	pEM->Add_Union_Effect("Chai_Mimosa_Map_Decel", 3);

	pEM->Add_Union_Effect("Mimosa_Doremi_Boom", 2);
	pEM->Add_Union_Effect("SpotLight_Enemy", 10);
	pEM->Add_Union_Effect("SpotLight_Chai", 2);

	pEM->Add_Union_Effect("Mimosa_Ground_Map_Decel", 3);
	pEM->Add_Union_Effect("Mimosa_Map_Decel", 3);
	pEM->Add_Union_Effect("Mimosa_Walk_Shine", 3);

	pEM->Add_Union_Effect("Laser_Explode", 50);
	pEM->Add_Union_Effect("Mimosa_BG", 2);

	pEM->Add_Union_Effect("FireCracker_30", 3);
	pEM->Add_Union_Effect("FireCracker_20_Miku", 3);
	pEM->Add_Union_Effect("FireCracker_20_Purple", 3);
	pEM->Add_Union_Effect("FireCracker_20_Orange", 3);

	pEM->Add_Union_Effect("Mimosa_VSS", 2);
#pragma endregion
#pragma region CS_REKKA

	pEM->Add_Union_Effect("CS0230_00_018_em2200_0", 2);
	pEM->Add_Union_Effect("CS0230_00_018_em2200_1", 2);
	pEM->Add_Union_Effect("CS0230_00_020_em2200_0", 2);
	pEM->Add_Union_Effect("CS0230_00_020_em2200_1", 2);
	pEM->Add_Union_Effect("CS0230_00_020_Click", 2);
	pEM->Add_Union_Effect("CS0230_00_021_Click", 2);
	

	pEM->Add_Union_Effect("CS0230_00_023_em2000_00", 1);
	pEM->Add_Union_Effect("CS0230_00_025_em2000_00", 1);
	pEM->Add_Union_Effect("CS0230_00_027_em2000_00", 1);
	pEM->Add_Union_Effect("CS0230_00_029_em2000_00", 2);
	pEM->Add_Union_Effect("CS0235_00_001_em2000_00", 2);
	pEM->Add_Union_Effect("CS0235_00_004_em2000_00", 2);
	pEM->Add_Union_Effect("CS0235_00_008_em2000_00", 2);
	pEM->Add_Union_Effect("CS0235_00_010_em2000_00", 2);
	pEM->Add_Union_Effect("CS0235_00_010_em2000_00", 2);
	pEM->Add_Union_Effect("CS0235_00_013_em2000_00", 2);
	pEM->Add_Union_Effect("CS0235_00_019_em2000_00", 2);
	pEM->Add_Union_Effect("em0235_00_020_smoke", 2);
	pEM->Add_Union_Effect("CS0235_00_020_Impact", 2);

	pEM->Add_Union_Effect("CS0240_00_006_Click", 2);
	pEM->Add_Union_Effect("CS0240_00_006_Smoke", 2);
	pEM->Add_Union_Effect("Rekka_Dying", 2);
	pEM->Add_Union_Effect("Rekka_Dying_Bil", 2);
	pEM->Add_Union_Effect("Rekka_Explode", 2);

	pEM->Add_Union_Effect("Foot_Smoke", 50);
	pEM->Add_Union_Effect("LandingSmoke_Rekka", 5);
	pEM->Add_Union_Effect("Rekka_Arm_Smoke", 5);

	pEM->Add_Union_Effect("Base_FireCracker_Emerald", 10);
	pEM->Add_Union_Effect("Base_FireCracker_Purple", 10);
	pEM->Add_Union_Effect("Base_FireCracker_Yellow", 50);

	pEM->Add_Union_Effect("CS0230_00_024_Chai", 1);
	pEM->Add_Union_Effect("CS0230_00_030_Chai", 1); 
	pEM->Add_Union_Effect("CS0245_00_011_Chai", 1);
	pEM->Add_Union_Effect("CS0245_00_011_Chai_1", 1);

	pEM->Add_Union_Effect("CS0230_00_031", 1);
	pEM->Add_Union_Effect("CS0230_00_031_0", 1);
	pEM->Add_Union_Effect("CS0230_00_031_1", 1);
	pEM->Add_Union_Effect("CS0230_00_031_2", 1);
	pEM->Add_Union_Effect("CS0230_00_031_3", 1);
	pEM->Add_Union_Effect("CS0230_00_031_4", 1);
	pEM->Add_Union_Effect("CS0230_00_031_5", 1);

	pEM->Add_Union_Effect("CS0235_00_013_Plate", 1);
	pEM->Add_Union_Effect("CS0235_00_015_Plate", 1);
	pEM->Add_Union_Effect("CS0235_00_016_Plate", 1);

	pEM->Add_Union_Effect("Container_Donk", 1);
	pEM->Add_Union_Effect("Cnmn_Thick", 1);
	pEM->Add_Union_Effect("Container_Blur", 1);
	pEM->Add_Union_Effect("Laser_Charge_RobotCharge", 2);
	pEM->Add_Union_Effect("Laser_Charge_Robot_Cancle", 2);
	pEM->Add_Union_Effect("Robot_DeathBeam", 20);
#pragma endregion

#pragma endregion

#pragma region EVKALESTAGE
	pEM->Add_Union_Effect("Big_Laser_Charge", 24);
	pEM->Add_Union_Effect("Big_Laser_Body_2", 8);
	pEM->Add_Union_Effect("Stage_Shine", 3);
	pEM->Add_Union_Effect("Big_Laser_CNMN", 16);
	pEM->Add_Union_Effect("Big_Laser_CNMN_Long", 8);
	pEM->Add_Union_Effect("Big_Laser_CNMN_Long_2", 16);


	pEM->Add_Union_Effect("CS1130_000_em8100_Laser", 2);
	pEM->Add_Union_Effect("CS1130_RHE_0", 100);
	pEM->Add_Union_Effect("CS1130_RHE_1", 100);
	pEM->Add_Union_Effect("CS1130_RHE_2", 100);
	pEM->Add_Union_Effect("CS1130_008", 100);
	pEM->Add_Union_Effect("CS1130_CNMN_EXP", 1);

	pEM->Add_Union_Effect("EV1120_00_em0700", 1);
	pEM->Add_Union_Effect("EV1150_5_ch1000", 1);
	pEM->Add_Union_Effect("EV1150_02_em8100", 1);
	pEM->Add_Union_Effect("EV1150_06_em8100", 1);

	pEM->Add_Union_Effect("EV1150_Spark_0", 100);
	pEM->Add_Union_Effect("EV1150_Spark_1", 100);
	pEM->Add_Union_Effect("EV1150_Spark_2", 100);

	pEM->Add_Union_Effect("EV1160_00_em8100", 1);
	pEM->Add_Union_Effect("EV1170_00_em8100_Out", 1);
	pEM->Add_Union_Effect("EV1170_00_em8100_In", 1);


	pEM->Add_Union_Effect("EV1180_Spark_0", 50);
	pEM->Add_Union_Effect("EV1180_Spark_1", 50);
	pEM->Add_Union_Effect("EV1180_Spark_2", 50);
	pEM->Add_Union_Effect("EV1180_Spark_3", 50);
	pEM->Add_Union_Effect("EV1180_Spark_4", 50);

	pEM->Add_Union_Effect("EV1180_03_em8200", 1);


	pEM->Add_Union_Effect("EV1190_RHE_0", 100);
	pEM->Add_Union_Effect("EV1190_RHE_1", 100);
	pEM->Add_Union_Effect("EV1190_RHE_2", 100);
	pEM->Add_Union_Effect("EV1190_RHE_3", 100);
	pEM->Add_Union_Effect("EV1190_RHE_4", 100);
	pEM->Add_Union_Effect("EV1190_RHE_5", 100);
	pEM->Add_Union_Effect("EV1190_RHE_6", 100);
	pEM->Add_Union_Effect("EV1190_RHE_7", 100);
	pEM->Add_Union_Effect("EV1190_RHE_8", 100);

	pEM->Add_Union_Effect("EV_1122_Krck_Cyclone", 5);
	pEM->Add_Union_Effect("EV1105_Pep_Cont", 1);
	pEM->Add_Union_Effect("EV1105_bg1510_Line", 1);


	pEM->Add_Union_Effect("Steel_Impact_0", 1);
	pEM->Add_Union_Effect("Steel_Impact_1", 1);
	pEM->Add_Union_Effect("Steel_Impact_2", 1);
	pEM->Add_Union_Effect("Steel_Impact_3", 1);
	
#pragma endregion

#pragma region SWORDMACHINE_AND_BLAZE
	pEM->Add_Union_Effect("me0700_atk_020_0", 3);
	pEM->Add_Union_Effect("me0700_atk_020_1", 3);
	pEM->Add_Union_Effect("me0700_sword_a_100", 3);
	pEM->Add_Union_Effect("me0700_sword_a_101", 3);

	pEM->Add_Union_Effect("em740_WS_000", 3);
	pEM->Add_Union_Effect("em0740_atk_000_0", 3);
	pEM->Add_Union_Effect("em0740_atk_000_1", 3);

	pEM->Add_Union_Effect("em740_atk_010_0", 3);
	pEM->Add_Union_Effect("em0740_010_Fire_L", 3);
	pEM->Add_Union_Effect("em0740_010_Fire_R", 3);

	pEM->Add_Union_Effect("em0740_060_Fire_L", 3);
	pEM->Add_Union_Effect("em0740_060_Fire_R", 3);
	pEM->Add_Union_Effect("em0740_060_Wind", 3);

	pEM->Add_Union_Effect("em0740_070_down", 3);
	pEM->Add_Union_Effect("em0740_070_Up", 3);
	
#pragma endregion

#pragma region EVKALEBOSS
	pEM->Add_Union_Effect("CS1205_CoatOff", 1);
	pEM->Add_Union_Effect("CS1205_CoatCamSpin", 3);
	pEM->Add_Union_Effect("CS1205_SpectraHand", 1);
	pEM->Add_Union_Effect("CS1205_SpectraProj", 1);
	pEM->Add_Union_Effect("CS1205_SpFlare", 1);
	pEM->Add_Union_Effect("CS1205_ChaiSpark", 1);
	pEM->Add_Union_Effect("CS1205_Chai_ChestSpark", 1);


	pEM->Add_Union_Effect("CS1205_Chai_Throw", 1);
	pEM->Add_Union_Effect("CS1205_ChaiHit", 1);

	pEM->Add_Union_Effect("CS1205_KaleHeadHit", 1);
	pEM->Add_Union_Effect("CS1205_808HeadHit0", 3);
	pEM->Add_Union_Effect("CS1205_808HeadHit1", 3);

	pEM->Add_Union_Effect("CS1205_KaleCatThrow", 1);
	pEM->Add_Union_Effect("CS1205_KaleChaiThrow", 1);
	pEM->Add_Union_Effect("CS1205_KaleSward", 1);
	pEM->Add_Union_Effect("CS1205_Pep", 1);

	pEM->Add_Union_Effect("CS1210_ChaiStand", 1);
	pEM->Add_Union_Effect("CS1210_ChaiGuitar", 1);
	pEM->Add_Union_Effect("CS1210_ChaiChestPulse", 1);
	pEM->Add_Union_Effect("CS1210_TowerElec", 1);

	pEM->Add_Union_Effect("CS1215_Smoke0", 1);

	pEM->Add_Union_Effect("CS1215_ArmSpin", 1);
	pEM->Add_Union_Effect("CS1215_ArmSpin1", 1);
	pEM->Add_Union_Effect("CS1215_ArmSpin2", 1);
	pEM->Add_Union_Effect("CS1215_ArmSpin3", 1);

	pEM->Add_Union_Effect("CS1220_KaleHitGround", 4);

	pEM->Add_Union_Effect("CS1220_BL_5_0", 2);
	pEM->Add_Union_Effect("CS1220_BL_5_1", 2);
	pEM->Add_Union_Effect("CS1220_BL_5_2", 2);
	pEM->Add_Union_Effect("CS1220_BL_5_3", 2);
	pEM->Add_Union_Effect("CS1220_BL_5_4", 2);
	pEM->Add_Union_Effect("CS1220_BL_5_5", 2);
	pEM->Add_Union_Effect("CS1220_BL_5_6", 2);
	pEM->Add_Union_Effect("CS1220_BL_5_7", 2);


	pEM->Add_Union_Effect("CS1220_BL_Spark0", 2);
	pEM->Add_Union_Effect("CS1220_BL_Spark1", 2);
	pEM->Add_Union_Effect("CS1220_BL_Spark2", 2);
	pEM->Add_Union_Effect("CS1220_BL_Spark3", 2);
	pEM->Add_Union_Effect("CS1220_BL_Spark4", 2);
	pEM->Add_Union_Effect("CS1220_BL_Spark5", 2);
	pEM->Add_Union_Effect("CS1220_BL_Spark6", 2);
	pEM->Add_Union_Effect("CS1220_BL_Spark7", 2);
	pEM->Add_Union_Effect("CS1220_BL_Cent", 2);



	pEM->Add_Union_Effect("CS1220_Thunder", 2);
	pEM->Add_Union_Effect("CS1220_Spark", 2);


	pEM->Add_Union_Effect("CS1220_Light0", 2);
	pEM->Add_Union_Effect("CS1220_Light1", 2);
	pEM->Add_Union_Effect("CS1220_Light2", 2);
	pEM->Add_Union_Effect("CS1220_Light3", 2);
	pEM->Add_Union_Effect("CS1220_Light4", 2);
	pEM->Add_Union_Effect("CS1220_Light5", 2);
	pEM->Add_Union_Effect("CS1220_Light6", 2);
	pEM->Add_Union_Effect("CS1220_Light7", 2);

	pEM->Add_Union_Effect("CS1220_BodyLight0", 2);
	pEM->Add_Union_Effect("CS1220_ArmLight0", 2);
	pEM->Add_Union_Effect("CS1220_ArmLight1", 2);

	pEM->Add_Union_Effect("EV1250_Hit0", 2);
	pEM->Add_Union_Effect("EV1250_Hit1", 2);
	pEM->Add_Union_Effect("EV1250_Hit2", 2);
	pEM->Add_Union_Effect("EV1250_Hit3", 2);

	pEM->Add_Union_Effect("EV1250_K_Hit0", 2);
	pEM->Add_Union_Effect("EV1250_K_Hit1", 2);
	pEM->Add_Union_Effect("EV1250_K_Hit2", 2);

	pEM->Add_Union_Effect("EV1250_P_Hit0", 2);
	pEM->Add_Union_Effect("EV1250_P_Hit1", 2);
	pEM->Add_Union_Effect("EV1250_P_Hit2", 2);
	pEM->Add_Union_Effect("EV1250_P_Hit3", 2);

	pEM->Add_Union_Effect("EV1250_KaleLand0", 2);
	pEM->Add_Union_Effect("EV1250_KaleLand1", 2);
	pEM->Add_Union_Effect("EV1250_KaleSlash0", 2);
	pEM->Add_Union_Effect("EV1250_KaleSlash1", 2);

	pEM->Add_Union_Effect("EV1250_ChaiLine", 2);
	pEM->Add_Union_Effect("EV1250_ChaiSmoke0", 40);

	pEM->Add_Union_Effect("EV1250_C_Swing0", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing1", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing2", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing3", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing4", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing5", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing6", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing7", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing8", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing9", 2);
	pEM->Add_Union_Effect("EV1250_C_Swing10", 2);

	pEM->Add_Union_Effect("EV1250_P_Bul0", 2);
	pEM->Add_Union_Effect("EV1250_P_Bul1", 2);
	pEM->Add_Union_Effect("EV1250_P_Bul2", 2);

	pEM->Add_Union_Effect("EV1250_P_Jump", 2);
	pEM->Add_Union_Effect("EV1250_P_Jump1", 2);
	pEM->Add_Union_Effect("EV1250_P_Jump2", 2);

	pEM->Add_Union_Effect("EV1250_M_Swing0", 2);
	pEM->Add_Union_Effect("EV1250_M_Swing1", 2);

	pEM->Add_Union_Effect("EV1250_K_Swing0", 2);
	pEM->Add_Union_Effect("EV1250_K_Swing1", 2);
	
	pEM->Add_Union_Effect("Blaze_Oil_Start", 50);
	pEM->Add_Union_Effect("Blaze_Fire_Loop", 50);
	pEM->Add_Union_Effect("Blaze_Fire_Loop_1", 50);
	pEM->Add_Union_Effect("Blaze_Fire_End", 50);

	pEM->Add_Union_Effect("Kale_Cs1225_GroundHit", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_Start", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_GroundSpark", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_Dash", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_Clash", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_Clash2", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_Clash3", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_HitEffect", 3);
	pEM->Add_Union_Effect("Peppermint_Cs1225_Mp", 3);
	pEM->Add_Union_Effect("Korsica_Cs1225_Effect", 3);
	pEM->Add_Union_Effect("Korsica_Upper", 3);
	pEM->Add_Union_Effect("Mcr_Cs1225_Punch", 3);
	pEM->Add_Union_Effect("Guitar_Cs1225", 3); 
	pEM->Add_Union_Effect("Kale_Cs1225_Shock", 3);
	pEM->Add_Union_Effect("Kale_Suffer", 3);

	pEM->Add_Union_Effect("Kale_Cs1225_Finale_1", 3);
	pEM->Add_Union_Effect("Kale_Cs1225_Finale_2", 3); 
	pEM->Add_Union_Effect("Chai_Cs0820_Fire_Tg", 3);
	pEM->Add_Union_Effect("Chai_Cs0820_Turn", 3);
	pEM->Add_Union_Effect("Chai_Cs0820_HoodOut", 3);
	pEM->Add_Union_Effect("Mimosa_Cs0820_Turn", 3);
	pEM->Add_Union_Effect("Mimosa_Cs0820_Blur", 3);
	pEM->Add_Union_Effect("Mimosa_Cs0820_Change", 3);
	pEM->Add_Union_Effect("Mimosa_Cs0820_Jump", 3);
	
	pEM->Add_Union_Effect("Kale_Cs1225_Finale_2", 3);
	pEM->Add_Union_Effect("JudgementCut_End", 3);
#pragma endregion

#pragma region EVMIMOSABOSS
	pEM->Add_Union_Effect("CS0840_M_Hit", 3);
	pEM->Add_Union_Effect("CS0840_HitGround0", 3);

	pEM->Add_Union_Effect("CS0840_M_BigExp", 3);
	pEM->Add_Union_Effect("CS0840_M_Exp0", 3);
	pEM->Add_Union_Effect("CS0840_M_Exp1", 3);
	pEM->Add_Union_Effect("CS0840_M_Exp2", 3);
	pEM->Add_Union_Effect("CS0840_M_Exp3", 3);
	pEM->Add_Union_Effect("CS0840_M_Exp4", 3);
	pEM->Add_Union_Effect("CS0840_M_Exp5", 3);
	pEM->Add_Union_Effect("CS0840_Line", 1);

	pEM->Add_Union_Effect("CS0840_ChaiSwing", 1);
	pEM->Add_Union_Effect("CS0840_GroundSpark", 1);
	pEM->Add_Union_Effect("CS0840_GuitarChange", 1);
	pEM->Add_Union_Effect("CS0840_G_Assemble", 1);
	pEM->Add_Union_Effect("CS0840_FireWork", 1);
	pEM->Add_Union_Effect("CS0840_GuitarSpin", 1);

	pEM->Add_Union_Effect("CSC0835_HitWall", 1);
	pEM->Add_Union_Effect("CS0835_Blur0", 1);
	pEM->Add_Union_Effect("CS0835_Blur1", 1);

	pEM->Add_Union_Effect("CS0835_BraceUp", 1);
	pEM->Add_Union_Effect("CS0835_RiseUp", 1);
	pEM->Add_Union_Effect("CS0835_Spark", 1);

	pEM->Add_Union_Effect("CS0830_Smoke0", 1);
	pEM->Add_Union_Effect("CS0830_SmallSpark", 1);
	pEM->Add_Union_Effect("CS0830_Guitar", 1);
	pEM->Add_Union_Effect("CS0830_M_Posing", 1);

	pEM->Add_Union_Effect("CS0825_LandingSpark0", 1);
	pEM->Add_Union_Effect("CS0825_LandingSpark1", 1);
	pEM->Add_Union_Effect("CS0825_LandingSpark2", 1);

	pEM->Add_Union_Effect("CS0825_Snap", 1);
	pEM->Add_Union_Effect("CS0825_Spark0", 1);
	pEM->Add_Union_Effect("CS0825_Spin", 1);

	pEM->Add_Union_Effect("CS0825_MimSpot", 1);
	pEM->Add_Union_Effect("CS0825_ChaiSpot", 1);

	pEM->Add_Union_Effect("CS0825_SaberSpot0", 1);
	pEM->Add_Union_Effect("CS0825_SaberSpot1", 1);
	pEM->Add_Union_Effect("CS0825_SaberSpot2", 1);
	pEM->Add_Union_Effect("CS0825_SaberSpot3", 1);
	pEM->Add_Union_Effect("CS0825_SaberSpot4", 1);

	pEM->Add_Union_Effect("ShakeHalfSecond", 5);
	pEM->Add_Union_Effect("Rekka_Map_Spark_Ctr", 3);
#pragma endregion

#pragma region EVINTRO
	pEM->Add_Union_Effect("CS0106_ChaiHit", 3);
	pEM->Add_Union_Effect("CS0106_ChaiOoch", 3);
	pEM->Add_Union_Effect("CS0106_GrabLod", 3);
	pEM->Add_Union_Effect("CS0106_OpenHand", 3);
	pEM->Add_Union_Effect("CS0106_SpinHand", 3);
	pEM->Add_Union_Effect("CS0106_SpinHand1", 3);
	pEM->Add_Union_Effect("CS0106_ChestPulse", 10);
	pEM->Add_Union_Effect("CS0106_SlideSmoke", 3);
	pEM->Add_Union_Effect("CS0106_BG7Sec", 1);

	pEM->Add_Union_Effect("CS0106_GutarMagnet", 1);
	pEM->Add_Union_Effect("CS0106_GutarMagnet1", 1);
	pEM->Add_Union_Effect("CS0106_GutarMagnet2", 1);
	pEM->Add_Union_Effect("CS0106_GutarMagnet3", 1);
	pEM->Add_Union_Effect("CS0106_GutarMagnet4", 1);


	pEM->Add_Union_Effect("OverDrive_Start", 3);
	pEM->Add_Union_Effect("OverDrive_Loop", 3);
	pEM->Add_Union_Effect("OverDrive_End", 3);

	pEM->Add_Union_Effect("FirePunch", 20);
	pEM->Add_Union_Effect("CS0106_11_Tablet", 1);
	pEM->Add_Union_Effect("CS0106_12_Tablet", 1);
	pEM->Add_Union_Effect("CS0106_13_Tablet", 1);

#pragma endregion

#pragma region EVKRCK
	pEM->Add_Union_Effect("CS0620_22_BG", 1);
	pEM->Add_Union_Effect("CS0620_22_Spin0", 1);
	pEM->Add_Union_Effect("CS0620_22_Spin1", 1);

#pragma endregion
	pEM->Add_Union_Effect("Blaze_rp_1", 3);
	pEM->Add_Union_Effect("Blaze_rp_2", 3);
	pEM->Add_Union_Effect("Blaze_rp_3", 3);
	pEM->Add_Union_Effect("Blaze_rp_4", 3);
	pEM->Add_Union_Effect("Blaze_rp_5", 3);
	pEM->Add_Union_Effect("Blaze_rp_bg",3);
	
	pEM->Add_Union_Effect("Blaze_BackLoop", 5);
	pEM->Add_Union_Effect("ArmRobot_Spark", 5);

	pEM->Add_Union_Effect("Damage_TwinAss0", 25);

	pEM->Add_Union_Effect("ODS_Swing_0", 3);
	pEM->Add_Union_Effect("ODS_Swing_1", 3);
	pEM->Add_Union_Effect("ODS_Swing_2", 3);
	pEM->Add_Union_Effect("ODS_Swing_3", 3);
	pEM->Add_Union_Effect("ODS_Swing_4", 3);
	pEM->Add_Union_Effect("ODS_Swing_5", 3);
	
	return S_OK;
}

shared_ptr<CMainApp> CMainApp::Create()
{
	MAKE_SHARED_ENABLER(CMainApp);
	shared_ptr<CMainApp> pInstance = make_shared<MakeSharedEnabler>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		assert(false);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	m_pGameManager->Destroy_Instance();
	m_pGameManager = nullptr;

	m_pPlayerManager->Destroy_Instance();
	m_pPlayerManager = nullptr;

	m_pImguiManager->Destroy_Instance();
	m_pImguiManager = nullptr;

	m_pCUiMgr->Destroy_Instance();
	m_pCUiMgr = nullptr;

	CMapManager::Get_Instance()->Destroy_Instance();

	m_pGameInstance->Destroy_Instance();
	m_pGameInstance = nullptr;
	m_pBeat_Manager->Destroy_Instance();
	m_pBeat_Manager = nullptr;

	m_pCombat_Manager->Destroy_Instance();
	m_pCombat_Manager = nullptr;

	m_pEffect_Manager->Destroy_Instance();
	m_pEffect_Manager = nullptr;

	CGimmickManager::Destroy_Instance();
}