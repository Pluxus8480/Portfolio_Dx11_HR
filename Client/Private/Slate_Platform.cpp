#include "Slate_Platform.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "PhysxCollider.h"

#include "Beat_Manager.h"
#include "MapManager.h"

wstring CSlate_Platform::ObjID = TEXT("CSlate_Platform");

CSlate_Platform::CSlate_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CSlate_Platform::CSlate_Platform(const CSlate_Platform& rhs)
	: CGimmickBase{ rhs }
{
}

CSlate_Platform::~CSlate_Platform()
{
	Free();
}

HRESULT CSlate_Platform::Set_ModelComponent(const string& strPrototypeTag)
{
	_tchar szPrototypeTag[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, strPrototypeTag.c_str(), (_int)strPrototypeTag.length(), szPrototypeTag, MAX_PATH);

	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, wstring(szPrototypeTag), TEXT("Com_Model")));

	if (!m_pModelCom)
		assert(false);

	m_pModelCom->Set_AnimIndex(0, true);

	// For effect handling
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach)
			CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
		else
			CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
		}
	);

	return S_OK;
}

HRESULT CSlate_Platform::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Slate_Platform"), TEXT("../Bin/Resources/Models/Map/Gimmick/slide_panel.fbx"), MODEL_TYPE::ANIM, _float4x4{ 0.01f, 0.f,  0.f,  0.f,
																																											0.f,  0.01f, 0.f,  0.f,
																																											0.f,  0.f,  0.01f, 0.f,
																																											0.f,  0.f,  0.f,  1.f })
	);
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(L"Sound_DoorOpen", L"../Bin/Resources/Sounds/MapObject/Door_Open.ogg",
		SOUND_CHANNEL::SOUND_EFFECT));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_Loop"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_27.ogg")));

	return S_OK;
}

HRESULT CSlate_Platform::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(XMVectorSet(1000.f, 1000.f, 1000.f, 0.f));
	//_float4 vPos = _float4(36.46f, -3.07f, -93.f, 1.f);
	//m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));
	if(pDesc)
		m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (m_pModelCom != nullptr)
	{
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, true);


	}
	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	// For effect handling
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach)
			CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
		else
			CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, m_pTransformCom->Get_WorldMatrix_Ptr(), 1.f, isBillBoard);
		}
	);


	return S_OK;
}

void CSlate_Platform::PriorityTick(_float fTimeDelta)
{
}

void CSlate_Platform::Tick(_float fTimeDelta)
{
	m_bCulled = !GAMEINSTANCE->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 6.f);
	if (!m_bInit)
	{
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
		for (_uint i = 0; i < STATE_END; ++i)
		{
			m_fAnimSpeed[i] = m_pModelCom->Get_Animation_PlayTime(i) / fBeat;
		}
		m_bInit = true;
	}
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
		//m_bPlay = !m_bPlay;
		//m_pModelCom->Set_Finished(m_bPlay);
		++m_iAccBeat;
		if (m_iCurrAnimIndex == OPEN)
		{
			if (m_iAccBeat >= m_iOpenDuration)
				Close();
		}
		else if (m_iCurrAnimIndex == CLOSE)
		{
			if (m_iAccBeat >= m_iCloseDuration)
				Open();
		}
	}
	if (m_pModelCom->Get_Finished())
	{
		Anim_Finished();
	}
	if(m_iCurrAnimIndex != CLOSE)
	{
		_matrix Transpose = m_pTransformCom->Get_WorldMatrix_XMMat();
		Transpose = XMMatrixMultiply(XMMatrixTranslation(-2.0f, -1.f, 3.f), Transpose);
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
	}
	else
	{
		m_pPhysXCollider->Get_Static_RigidBody()->setGlobalPose(PxTransform{ PxVec3{-100000.f,-100000.f + rand() % 300,-100000.f}});
	}

	_float4x4 Mat;
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fAnimSpeed[m_iCurrAnimIndex], 0.1f, Mat);
	
	// Call all reserved effects
	m_pModelCom->PlayReservedCallbacks();
}

void CSlate_Platform::LateTick(_float fTimeDelta)
{
	if (m_bCulled) return;
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CSlate_Platform::Render()
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
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
		{
			bNoEmit = true;
		}
		if(bNoEmit)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
		//	return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CSlate_Platform::Edit_Attribute()
{
	ImGui::InputInt("BeatDiff", &m_iStartAccBeat);
	ImGui::InputInt("OpenDuration", &m_iOpenDuration);
	ImGui::InputInt("CloseDuration", &m_iCloseDuration);
}

void CSlate_Platform::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
	write_typed_data(os, m_iStartAccBeat);
	write_typed_data(os, m_iOpenDuration);
	write_typed_data(os, m_iCloseDuration);

}

void CSlate_Platform::Load_Data(ifstream& is)
{
	__super::Load_Data(is);
	read_typed_data(is, m_iAccBeat);
	read_typed_data(is, m_iOpenDuration);
	read_typed_data(is, m_iCloseDuration);
	m_iStartAccBeat = m_iAccBeat;
}

void CSlate_Platform::Open()
{
	if (m_iCurrAnimIndex == CLOSE)
	{
		m_iCurrAnimIndex = CLOSE_TO_OPEN_1;
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, false);
		m_iAccBeat = 0;
		if(!m_bCulled)
			m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_DoorOpen", m_pTransformCom->Get_Position());
	}
}

void CSlate_Platform::Close()
{
	if (m_iCurrAnimIndex == OPEN)
	{
		m_iCurrAnimIndex = OPEN_TO_CLOSE_1;
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, false);
		m_iAccBeat = 0;
		if (!m_bCulled)
			m_pGameInstance.lock()->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_DoorOpen", m_pTransformCom->Get_Position());
	}
}

void CSlate_Platform::Anim_Finished()
{
	if (m_iCurrAnimIndex == CLOSE_TO_OPEN_1)
	{
		m_iCurrAnimIndex = CLOSE_TO_OPEN_2;
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, false);
	}
	else if (m_iCurrAnimIndex == CLOSE_TO_OPEN_2)
	{
		m_iCurrAnimIndex = OPEN;
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, true);
	}
	else if (m_iCurrAnimIndex == OPEN_TO_CLOSE_1)
	{
		m_iCurrAnimIndex = OPEN_TO_CLOSE_2;
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, false);
	}
	else if (m_iCurrAnimIndex == OPEN_TO_CLOSE_2)
	{
		m_iCurrAnimIndex = CLOSE;
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex, true);
	}
}

HRESULT CSlate_Platform::Ready_Components(const COMMONOBJ_DESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slate_Platform"), TEXT("Com_Model")));
	
	m_pPhysXCollider = static_pointer_cast<CPhysXCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysXCollider"), TEXT("Com_PhsyXCollider")));

	PxVec3 BoxDimension{2.f,0.5f,3.f};
	PxBoxGeometry Box{ BoxDimension };
	m_pPhysXCollider->createStaticShapeActor(Box, "");
	if (!m_pModelCom)
		assert(false);

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);
	assert(m_pPhysXCollider);
	

	return S_OK;
}

shared_ptr<CSlate_Platform> CSlate_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CSlate_Platform
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSlate_Platform(pDevice, pContext)
		{}
	};


	shared_ptr<CSlate_Platform> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSlate_Platform"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CSlate_Platform::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CSlate_Platform
	{
		MakeSharedEnabler(const CSlate_Platform& rhs) : CSlate_Platform(rhs)
		{}
	};


	shared_ptr<CSlate_Platform> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSlate_Platform"));
		assert(false);
	}

	return pInstance;
}

void CSlate_Platform::Free()
{
}
