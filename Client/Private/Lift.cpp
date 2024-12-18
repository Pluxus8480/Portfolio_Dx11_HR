#include "Lift.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "PhysxCollider.h"
#include "Brake_Part.h"
#include "Beat_Manager.h"
#include "CUi_TramTimer.h"
#include "CUi_Mgr.h"
#include "Effect_Manager.h"

wstring CLift::ObjID = TEXT("CLift");

CLift::CLift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLift::CLift(const CLift& rhs)
	: CGameObject{ rhs }
{
}

CLift::~CLift()
{
	Free();
}

HRESULT CLift::Set_ModelComponent(const string& strPrototypeTag)
{
	_tchar szPrototypeTag[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);

	m_pModelCom->Set_AnimIndex(0, true);

	return S_OK;
}

HRESULT CLift::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_float4x4 PreTransform;
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Lift"), TEXT("../Bin/Resources/Models/Map/Gimmick/Lift.fbx"), MODEL_TYPE::ANIM, PreTransform)
	);
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Lift_FloorCol"), TEXT("../Bin/Resources/Models/Map/Gimmick/Lift_FloorCol.fbx"), MODEL_TYPE::NONANIM, PreTransform)
	);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Lift_Down"), TEXT("../Bin/Resources/Sounds/SFX/Lift/Lift_Down.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Brake_On"), TEXT("../Bin/Resources/Sounds/SFX/Lift/Brake_On.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("Brake_Break"), TEXT("../Bin/Resources/Sounds/SFX/Lift/Brake_Break.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("LiftFriction"), TEXT("../Bin/Resources/Sounds/SFX/Lift/LiftFriction.ogg")));

	return S_OK;
}

HRESULT CLift::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ANIMOBJ_DESC* pDesc = static_cast<ANIMOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	//_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	//m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	if (pDesc)
		m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);


	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	// patrollampbase01~04
	// break_bl, break_br, break_fl, break_fr

	vector<vector<string>> Split;
	Split.resize(5);
	Split[0].push_back("patrollampbase01");
	Split[0].push_back("patrollampbase02");
	Split[0].push_back("patrollampbase03");
	Split[0].push_back("patrollampbase04");
	Split[1].push_back("brake_fl");
	Split[1].push_back("brake_attach_fl");

	Split[2].push_back("brake_fr");
	Split[2].push_back("brake_attach_fr");

	Split[3].push_back("brake_bl");
	Split[3].push_back("brake_attach_bl");

	Split[4].push_back("brake_br");
	Split[4].push_back("brake_attach_br");

	m_pModelCom->SplitBones(6, Split);
	
	m_mapPartState.emplace("patrollampbase01", 24);
	m_mapPartState.emplace("brake_fl", 20);
	m_mapPartState.emplace("brake_fr", 20);
	m_mapPartState.emplace("brake_bl", 20);
	m_mapPartState.emplace("brake_br", 20);
	m_mapPartState.emplace("Rest", 21);


	strPartBone[0] = "patrollampbase01";
	strPartBone[1] = "brake_fl";
	strPartBone[2] = "brake_fr";
	strPartBone[3] = "brake_bl";
	strPartBone[4] = "brake_br";
	strPartBone[5] = "Rest";

	for (auto& Part : m_mapPartState)
	{
		m_pModelCom->Set_AnimIndex_Split(Part.second, true, Part.first);
	}

	m_vecBrakePart.resize(BRAKE_END);

	for (_uint i = 0; i < BRAKE_END; ++i)
	{
		_bool bRight = false;
		if (i == FRONT_RIGHT || i == BACK_RIGHT)
			bRight = true;
		m_vecBrakePart[i] = static_pointer_cast<CBrake_Part>(m_pGameInstance.lock()->Clone_Object(CBrake_Part::ObjID, &bRight));
		m_vecBrakePart[i]->Create_Ui(CBrake_Part::BRAKE(i), m_pModelCom->Get_BoneMatrixPtr(strPartBone[i+1].c_str()));
	}

	Initialize_Ui();

	return S_OK;
}

void CLift::PriorityTick(_float fTimeDelta)
{
	for (_uint i = 0; i < BRAKE_END; ++i)
	{
		m_vecBrakePart[i]->PriorityTick(fTimeDelta);
	}
	Brake_Broken();
}

void CLift::Tick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		for (auto& Part : m_vecBrakePart)
		{
			_float4x4 RotMat = m_pTransformCom->Get_WorldMatrix();

			XMStoreFloat4x4(&RotMat, XMMatrixMultiply(XMLoadFloat4x4(&RotMat), XMMatrixRotationY(XMConvertToRadians(90.f))));

			if (Part->Is_On() && !Part->Is_Broken()) {
				if (Part == m_vecBrakePart[FRONT_LEFT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Spark_LF", &RotMat, 1.f);
				if (Part == m_vecBrakePart[FRONT_RIGHT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Spark_RF", &RotMat, 1.f);
				if (Part == m_vecBrakePart[BACK_LEFT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Spark_LB", &RotMat, 1.f);
				if (Part == m_vecBrakePart[BACK_RIGHT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Spark_RB", &RotMat, 1.f);
			}

			if (Part->Is_On() && Part->Is_Broken()) {
				if (Part == m_vecBrakePart[FRONT_LEFT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Smoke_LF", &RotMat, 1.f);
				if (Part == m_vecBrakePart[FRONT_RIGHT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Smoke_RF", &RotMat, 1.f);
				if (Part == m_vecBrakePart[BACK_LEFT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Smoke_LB", &RotMat, 1.f);
				if (Part == m_vecBrakePart[BACK_RIGHT])
					EFFECTMANAGER->Call_Fixed_Union_Effect("Lift_Smoke_RB", &RotMat, 1.f);
			}
		}

		m_iPrevBeat = iCurrBeat;
	}
	//if (m_mapPartState["Rest"] == 0)
	//{
	//	_float4x4 Mat;
	//	m_pModelCom->Play_Animation_Split(fTimeDelta, fTimeDelta * 3.f, 0.1f, Mat);
	//}
	//else
	//{
		_float4x4 Mat;
		m_pModelCom->Play_Animation_Split(fTimeDelta, fTimeDelta / CBeat_Manager::Get_Instance()->Get_Beat() * 0.5f, 1.f, Mat);
	//}


	if (m_pModelCom->Get_Finished_Split("Rest"))
	{
		if (m_bDown)
		{
			m_mapPartState["Rest"] = 20;
			m_pModelCom->Set_AnimIndex_Split(20, true, "Rest", true);
		}
		else
		{
			m_mapPartState["Rest"] = 21;
			m_pModelCom->Set_AnimIndex_Split(21, true, "Rest", true);
		}
	}

	if (GAMEINSTANCE->Get_KeyDown((DIK_1)))
	{
		Down(!m_bDown);
	}
	if (GAMEINSTANCE->Get_KeyDown((DIK_2)))
	{
		Brake_On(FRONT_LEFT);
	}
	if (GAMEINSTANCE->Get_KeyDown((DIK_3)))
	{
		Brake_On(FRONT_RIGHT);
	}
	if (GAMEINSTANCE->Get_KeyDown((DIK_4)))
	{
		Brake_On(BACK_LEFT);
	}
	if (GAMEINSTANCE->Get_KeyDown((DIK_5)))
	{
		Brake_On(BACK_RIGHT);
	}

	if (GAMEINSTANCE->Get_KeyDown((DIK_6)))
	{
		Brake_Crush(FRONT_LEFT);
	}
	if (GAMEINSTANCE->Get_KeyDown((DIK_7)))
	{
		Brake_Crush(FRONT_RIGHT);
	}
	if (GAMEINSTANCE->Get_KeyDown((DIK_8)))
	{
		Brake_Crush(BACK_LEFT);
	}
	if (GAMEINSTANCE->Get_KeyDown((DIK_9)))
	{
		Brake_Crush(BACK_RIGHT);
	}

	for (_uint i = 0; i < BRAKE_END; ++i)
	{
		m_vecBrakePart[i]->Tick(fTimeDelta);
		const _float4x4* pBone;
		switch (i)
		{
		case FRONT_LEFT:
		{
			pBone = m_pModelCom->Get_BoneMatrixPtr("brake_attach_fl");
		}
		break;
		case FRONT_RIGHT:
		{
			pBone = m_pModelCom->Get_BoneMatrixPtr("brake_attach_fr");
			//bRight = true;
		}
		break;
		case BACK_LEFT:
		{
			pBone = m_pModelCom->Get_BoneMatrixPtr("brake_attach_bl");
		}
		break;
		case BACK_RIGHT:
		{
			pBone = m_pModelCom->Get_BoneMatrixPtr("brake_attach_br");
			//bRight = true;
		}
		break;
		default:
			return;
		}
		_matrix BoneXM = XMLoadFloat4x4(pBone);
		XMStoreFloat4x4(&m_BrakePartMat[i], BoneXM * m_pTransformCom->Get_WorldMatrix_XMMat());
		//m_vecBrakePart[i]->Get_Transform()->Set_WorldMatrix(XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationMatrix(BoneXM), BoneXM.r[3]));
		m_vecBrakePart[i]->Set_Parent(&m_BrakePartMat[i]);
	}
	_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
	if (!m_bDown)
	{
		if (m_fColliderOffset < 3.5f)
		{
			m_fColliderOffset += fTimeDelta * 6.f;
		}
		else
		{
			m_fColliderOffset = 3.5f;
		}
	}
	else
	{
		if (m_fColliderOffset > 0.f)
		{
			m_fColliderOffset -= fTimeDelta * 5.f;
		}
		else
		{
			m_fColliderOffset = 0.f;
		}
	}
	Transpose.r[3] += XMVectorSet(0.f, m_fColliderOffset, 0.f, 0.f);
	PxMat44 mat;
	mat.column0.x = Transpose.r[0].m128_f32[0];
	mat.column0.y = Transpose.r[0].m128_f32[1];
	mat.column0.z = Transpose.r[0].m128_f32[2];
	mat.column0.w = Transpose.r[0].m128_f32[3];
	mat.column1.x = Transpose.r[1].m128_f32[0];
	mat.column1.y = Transpose.r[1].m128_f32[1];
	mat.column1.z = Transpose.r[1].m128_f32[2];
	mat.column1.w = Transpose.r[1].m128_f32[3];
	mat.column2.x = Transpose.r[2].m128_f32[0];
	mat.column2.y = Transpose.r[2].m128_f32[1];
	mat.column2.z = Transpose.r[2].m128_f32[2];
	mat.column2.w = Transpose.r[2].m128_f32[3];
	mat.column3.x = Transpose.r[3].m128_f32[0];
	mat.column3.y = Transpose.r[3].m128_f32[1];
	mat.column3.z = Transpose.r[3].m128_f32[2];
	mat.column3.w = Transpose.r[3].m128_f32[3];

	PxTransform transform{ mat };
	m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(transform);

	Check_Ui();
}

void CLift::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_TERRAIN, shared_from_this());

	for (_uint i = 0; i < BRAKE_END; ++i)
	{
		m_vecBrakePart[i]->LateTick(fTimeDelta);
	}
}

HRESULT CLift::Render()
{
	if (!m_pModelCom)
		return S_OK;

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}
		if (bNoEmit)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if(m_bColliderDraw)
	{

		_float4x4 Transpose = m_pTransformCom->Get_WorldMatrix();
		Transpose._42 += m_fColliderOffset;
		m_pDebugShaderCom->Bind_Matrix("g_WorldMatrix", &Transpose);

		if (FAILED(m_pDebugShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
			return E_FAIL;
		if (FAILED(m_pDebugShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
			return E_FAIL;
		if (FAILED(m_pDebugShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
			return E_FAIL;
		_uint iNumMeshes = m_pModelColliderCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pDebugShaderCom, "g_Texture", 0, aiTextureType_DIFFUSE)))
				continue;
			m_pDebugShaderCom->Begin(0);

			//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			//	return E_FAIL;
			m_pModelColliderCom->Render((_uint)i);
		}

	}

#endif
	return S_OK;
}

_bool CLift::Is_Broken(BRAKE eBrake)
{
	return m_vecBrakePart[eBrake]->Is_Broken();
}

void CLift::Brake_On(BRAKE eBrake)
{
	m_vecBrakePart[eBrake]->Brake_On();
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Brake_On");
	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftFriction", true);
	switch (eBrake)
	{
	case Client::CLift::FRONT_LEFT:
	{
		m_mapPartState["brake_fl"] = 9;
		m_pModelCom->Set_AnimIndex_Split(9, false, "brake_fl", false);
	}
		break;
	case Client::CLift::FRONT_RIGHT:
	{
		m_mapPartState["brake_fr"] = 8;
		m_pModelCom->Set_AnimIndex_Split(8, false, "brake_fr", false);
	}
		break;
	case Client::CLift::BACK_LEFT:
	{
		m_mapPartState["brake_bl"] = 7;
		m_pModelCom->Set_AnimIndex_Split(7, false, "brake_bl", false);
	}
		break;
	case Client::CLift::BACK_RIGHT:
	{
		m_mapPartState["brake_br"] = 6;
		m_pModelCom->Set_AnimIndex_Split(6, false, "brake_br", false);
	}
		break;
	default:
		break;
	}
}

void CLift::Brake_Crush(BRAKE eBrake)
{
	m_vecBrakePart[eBrake]->Break();
	switch (eBrake)
	{
	case Client::CLift::FRONT_LEFT:
	{
		m_mapPartState["brake_fl"] = 17;
		m_pModelCom->Set_AnimIndex_Split(17, false, "brake_fl", false);
		m_mapPartState["Rest"] = 17;
		m_pModelCom->Set_AnimIndex_Split(17, false, "Rest", false);

	}
	break;
	case Client::CLift::FRONT_RIGHT:
	{
		m_mapPartState["brake_fr"] = 15;
		m_pModelCom->Set_AnimIndex_Split(15, false, "brake_fr", false);
		m_mapPartState["Rest"] = 15;
		m_pModelCom->Set_AnimIndex_Split(15, false, "Rest", false);
	}
	break;
	case Client::CLift::BACK_LEFT:
	{
		m_mapPartState["brake_bl"] = 18;
		m_pModelCom->Set_AnimIndex_Split(18, false, "brake_bl", false);
		m_mapPartState["Rest"] = 18;
		m_pModelCom->Set_AnimIndex_Split(18, false, "Rest", false);
	}
	break;
	case Client::CLift::BACK_RIGHT:
	{
		m_mapPartState["brake_br"] = 16;
		m_pModelCom->Set_AnimIndex_Split(16, false, "brake_br", false);
		m_mapPartState["Rest"] = 16;
		m_pModelCom->Set_AnimIndex_Split(16, false, "Rest", false);
	}
	break;
	default:
		break;
	}
}

void CLift::Brake_Broken()
{
	for (_uint i = 0; i < 4; ++i)
	{
		if (m_vecBrakePart[i]->Is_Broken() && !m_bBroken[i])
		{
			m_pTimerUi->Set_Active(false);
			m_pTimerUi->Add_Time(10);
			m_bBroken[i] = true;
			m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Brake_Break");
			m_pGameInstance.lock()->Stop_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"LiftFriction");
			BRAKE eBrake = (BRAKE)i;
			switch (eBrake)
			{
			case Client::CLift::FRONT_LEFT:
			{
				m_mapPartState["brake_fl"] = 17;
				m_pModelCom->Set_AnimIndex_Split(17, false, "brake_fl", false);
				m_mapPartState["Rest"] = 17;
				m_pModelCom->Set_AnimIndex_Split(17, false, "Rest", false);

			}
			break;
			case Client::CLift::FRONT_RIGHT:
			{
				m_mapPartState["brake_fr"] = 15;
				m_pModelCom->Set_AnimIndex_Split(15, false, "brake_fr", false);
				m_mapPartState["Rest"] = 15;
				m_pModelCom->Set_AnimIndex_Split(15, false, "Rest", false);
			}
			break;
			case Client::CLift::BACK_LEFT:
			{
				m_mapPartState["brake_bl"] = 18;
				m_pModelCom->Set_AnimIndex_Split(18, false, "brake_bl", false);
				m_mapPartState["Rest"] = 18;
				m_pModelCom->Set_AnimIndex_Split(18, false, "Rest", false);
			}
			break;
			case Client::CLift::BACK_RIGHT:
			{
				m_mapPartState["brake_br"] = 16;
				m_pModelCom->Set_AnimIndex_Split(16, false, "brake_br", false);
				m_mapPartState["Rest"] = 16;
				m_pModelCom->Set_AnimIndex_Split(16, false, "Rest", false);
			}
			break;
			default:
				break;
			}
		}
	}
}

void CLift::Down(_bool bToggle)
{
	m_bDown = bToggle;
	if(m_bDown)
	{
		m_mapPartState["Rest"] = 0;
		m_pModelCom->Set_AnimIndex_Split(0, false, "Rest", false);
		m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"Lift_Down");
	}
	else
	{
		m_mapPartState["Rest"] = 1;
		m_pModelCom->Set_AnimIndex_Split(1, false, "Rest", true);
	}
}

void CLift::Initialize_Ui()
{
	CUi_TramTimer::Desc TramDesc;
	TramDesc.fTime = { 90 };
	m_pTimerUi = static_pointer_cast<CUi_TramTimer>(CUi_Mgr::Get_Instance()->
		Add_Clone_Return(CUi_TramTimer::ObjID, m_iLevelIndex, L"CUi", CUi_TramTimer::ObjID, &TramDesc));
	m_pTimerUi->Set_Active(false);
}

void CLift::Check_Ui()
{
	for (auto& iter : m_vecBrakePart)
	{
		if (iter->Is_On() && !iter->Is_Broken())
		{
			m_pTimerUi->Set_Active(true);
		}
	}
}

HRESULT CLift::Ready_Components(const ANIMOBJ_DESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lift"), TEXT("Com_Model")));
	m_pModelColliderCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lift_FloorCol"), TEXT("Com_CollisionModel")));
	assert(m_pModelColliderCom);


	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));
	_uint    iNumMeshes = m_pModelColliderCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		auto pMesh = m_pModelColliderCom->Get_Mesh((_uint)i);
		pMesh->Cooking_TriangleMesh();
		auto pTriangleMesh = pMesh->Get_TriangleMesh();
		m_pPhysXCollider->createStaticMeshActor(pTriangleMesh, "Lift");
	}

#ifdef _DEBUG
	m_pDebugShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_DebugShader")));

#endif

	if (!m_pModelCom)
		assert(false);

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);


	return S_OK;
}

shared_ptr<CLift> CLift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLift
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLift(pDevice, pContext)
		{}
	};


	shared_ptr<CLift> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLift"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CLift::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLift
	{
		MakeSharedEnabler(const CLift& rhs) : CLift(rhs)
		{}
	};


	shared_ptr<CLift> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLift"));
		assert(false);
	}

	return pInstance;
}

void CLift::Free()
{
}
