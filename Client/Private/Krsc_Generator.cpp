#include "Krsc_Generator.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Collider.h"
#include "Bounding_AABB.h"

#include "Beat_Manager.h"
#include "MapManager.h"
#include "PlayerManager.h"
#include "Korsica.h"
#include "CUi_Mgr.h"
#include "CUi_TalkSystem.h"

wstring CKrsc_Generator::ObjID = TEXT("CKrsc_Generator");
CKrsc_Generator::CKrsc_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{pDevice, pContext}
{
}

CKrsc_Generator::CKrsc_Generator(const CKrsc_Generator& rhs)
	: CGimmickBase{rhs}
{
}

CKrsc_Generator::~CKrsc_Generator()
{
	Free();
}

void CKrsc_Generator::Execute(_bool bExecute)
{
	if (m_bExecuted == bExecute) return;

	m_bExecuted = bExecute;

	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	m_iFirstBeat = m_iPrevBeat;

	auto pKorsica = static_pointer_cast<CKorsica>(PLAYERMANAGER->Get_Assist(L"Korsica"));
	if (pKorsica)
	{
		pKorsica->Play_Animation("EV1122_90_000_ch4000_00", false);
		pKorsica->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
	}
}

HRESULT CKrsc_Generator::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-90.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Krsc_Generator"), TEXT("../Bin/Resources/Models/Map/Gimmick/Krsc_Generator.fbx"), MODEL_TYPE::ANIM, fMat)
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Green.png"))
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_EmiSquare_Red"), TEXT("../Bin/Resources/Models/Map/Gimmick/T_ComEmiSquareA_E_Red.png"))
	);


	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("KorsicaGenerator"), TEXT("../Bin/Resources/Sounds/MapObject/KorsicaGenerator.ogg")));

	
	return S_OK;
}

HRESULT CKrsc_Generator::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iAnimIndex = 4;
	m_pModelCom->Set_AnimIndex(m_iAnimIndex, true);

	return S_OK;
}

void CKrsc_Generator::PriorityTick(_float fTimeDelta)
{
	if (!m_bInit)
	{
		_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
		_float fIdlePlayTime = m_pModelCom->Get_Animation_PlayTime(4);
		m_fPlaySpeed[0] = fIdlePlayTime / fBeat;
		_float fActionPlayTime = m_pModelCom->Get_Animation_PlayTime(1);
		m_fPlaySpeed[1] = fActionPlayTime / (fBeat * 4);
	}
}

void CKrsc_Generator::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_KeyDown(DIK_L))
		Execute();
#endif

	if (m_bExecuted && !m_bDone)
	{
		auto pKorsica = static_pointer_cast<CKorsica>(PLAYERMANAGER->Get_Assist(L"Korsica"));
		_bool bEventFinished = static_pointer_cast<CModel>(pKorsica->Find_Component(TEXT("Com_Model")))->Get_Finished();
		if (bEventFinished)
		{
			_uint iCurBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
			if (iCurBeat != m_iPrevBeat)
			{
				m_iPrevBeat = iCurBeat;

				if (iCurBeat == m_iFirstBeat + 1)
				{
					for (size_t i = 0; i < 3; i++)
					{
						m_vecRelatives[i]->Execute();
					}

					m_iCurIndex = 3;
				}
				else if (iCurBeat >= m_iFirstBeat + 5)
				{
					if (m_iCurIndex == m_vecRelatives.size())
						Done();
					else
					{
						m_vecRelatives[m_iCurIndex]->Execute();
						++m_iCurIndex;
					}
				}
			}
		}
		else
		{
			if (m_iAnimIndex != 1)
			{
				m_fTime += fTimeDelta;

				if (m_fTime > 7.5f)
				{
					m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"KorsicaGenerator");
					m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, L"KorsicaGenerator", 4);
					m_iAnimIndex = 1;
					m_pModelCom->Set_AnimIndex(m_iAnimIndex, true);
				}

			}
		}
	}
	
	_float4x4 fMat{};
	_float fPlaySpeed = (m_iAnimIndex == 1) ? m_fPlaySpeed[1] : m_fPlaySpeed[0];
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * fPlaySpeed, 0.1f, fMat);

	m_pColliderCom->Update(XMLoadFloat4x4(&m_ColliderOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CKrsc_Generator::LateTick(_float fTimeDelta)
{
	//if (false == CMapManager::Get_Instance()->IsIn_RenderBoundary(m_iObjectID)) return;

	auto pGI = m_pGameInstance.lock();
	if (false == pGI->isIn_Frustum_WorldSpace(XMVectorSet(m_ModelMatrix._41, m_ModelMatrix._42, m_ModelMatrix._43, 1.f), 10.f)) return;

	pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());

	if (!m_bExecuted && !m_bDone)
		pGI->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CKrsc_Generator::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 2)
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		_bool bEmissive = { false };
		if (i == 3 || i == 6)
		{
			_uint iEmiTexture = (m_iAnimIndex == 1) ? 1 : 0;
			if (FAILED(m_pEmiTexture[iEmiTexture]->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture")))
				return E_FAIL;
			bEmissive = true;
		}
		else
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
				bEmissive = false;
			else
				bEmissive = true;
		}

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i);

		if (bEmissive)
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
		else
			m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Default);

		m_pModelCom->Render((_uint)i);
	}

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CKrsc_Generator::Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	m_vecRelatives.emplace_back(pRelativeObject);
}

void CKrsc_Generator::Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject)
{
	auto iter = find(m_vecRelatives.begin(), m_vecRelatives.end(), pRelativeObject);
	if (iter != m_vecRelatives.end())
		m_vecRelatives.erase(iter);
}

void CKrsc_Generator::Edit_Attribute()
{
	_float fPos[3] = {}, fRot[3] = {}, fScl[3] = {};
	ImGuizmo::DecomposeMatrixToComponents((_float*)&m_ModelOffset, fPos, fRot, fScl);

	if (ImGui::DragFloat3("Model Pos Offset", fPos))
		m_ModelOffset._41 = fPos[0]; m_ModelOffset._42 = fPos[1]; m_ModelOffset._43 = fPos[2];
	
	if (ImGui::DragFloat3("Model Rot Offset", fRot))
	{
		ImGuizmo::RecomposeMatrixFromComponents(fPos, fRot, fScl, (_float*)&m_ModelOffset);
	}

	_vector vPos{}, vRot{}, vScl{};
	XMMatrixDecompose(&vScl, &vRot, &vPos, XMLoadFloat4x4(&m_ColliderOffset));

	if(ImGui::DragFloat3("Collider Scale", vScl.m128_f32))
		XMStoreFloat4x4(&m_ColliderOffset, XMMatrixAffineTransformation(vScl, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vPos));
}

void CKrsc_Generator::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	Save_Relatives(os);

	write_typed_data(os, m_ModelOffset);
	write_typed_data(os, m_ColliderOffset);
}

void CKrsc_Generator::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	Load_Relatives(is);
	 
	read_typed_data(is, m_ModelOffset);
	read_typed_data(is, m_ColliderOffset);

	XMStoreFloat4x4(&m_ModelMatrix, XMLoadFloat4x4(&m_ModelOffset) * m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CKrsc_Generator::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	if (pOther->Get_ColliderTag() == TEXT("Player_Hit"))
	{
		//PLAYERMANAGER->Callback_Event("");
		Execute();

		if (!m_bTalk)
		{
			m_bTalk = true;
			m_isTalkOver = false;

			auto pTalkSystem = CUi_Mgr::Get_Instance()->Find_UI(CUi_TalkSystem::ObjID);
			if (pTalkSystem == nullptr)
			{
				CUi_TalkSystem::CUi_TalkSystemInfo TalkDesc{};
				TalkDesc.endCallback = [&]() {
					m_isTalkOver = true;
					};

				pTalkSystem = CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_TalkSystem::ObjID, LEVEL_TEST, L"CUi", CUi_TalkSystem::ObjID, &TalkDesc);
			}

			auto PRequiredTalkSystem = dynamic_pointer_cast<CUi_TalkSystem>(pTalkSystem);
			if (PRequiredTalkSystem != nullptr)
			{
				wstring strTalkTag = wstring(L"STKale Talk_File_5.csv");
				PRequiredTalkSystem->LoadTalkFile(strTalkTag);

				CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
			}
		}

	}
}

HRESULT CKrsc_Generator::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Krsc_Generator"), TEXT("Model_Com")));

	m_pEmiTexture[0] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiSquare_Green"), TEXT("Texture0_Com")));
	m_pEmiTexture[1] = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmiSquare_Red"), TEXT("Texture1_Com")));

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc{};
	aabbDesc.vExtents = { 1.f, 1.f, 1.f };
	aabbDesc.vCenter = { 0.f, 1.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Collider_Com"), &aabbDesc));

	if (!m_pShaderCom || !m_pModelCom || !m_pEmiTexture[0] || !m_pEmiTexture[1] || !m_pColliderCom)
		assert(false);

	return S_OK;
}

HRESULT CKrsc_Generator::Bind_ShaderResources()
{
	auto pGameInstance = m_pGameInstance.lock();

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CKrsc_Generator> CKrsc_Generator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CKrsc_Generator
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKrsc_Generator(pDevice, pContext)
		{}
	};

	shared_ptr<CKrsc_Generator> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKrsc_Generator"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CKrsc_Generator::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CKrsc_Generator
	{
		MakeSharedEnabler(const CKrsc_Generator& rhs) : CKrsc_Generator(rhs)
		{}
	};

	shared_ptr<CKrsc_Generator> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKrsc_Generator"));
		assert(false);
	}

	return pInstance;
}

void CKrsc_Generator::Free()
{
}
