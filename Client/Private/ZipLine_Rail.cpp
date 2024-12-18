#include "ZipLine_Rail.h"
#include "GameInstance.h"
#include "ZipLine_RailPart.h"
#include "ZipLine_Magnet.h"
#include "ZipLine_Obstacle.h"

#include "MapManager.h"
#include "Imgui_Manager.h"
#include "PlayerManager.h"
#include "iostream"
#include "CUi_Mgr.h"
#include "CUi_TalkSystem.h"
#include "Cine_Manager.h"

wstring CZipLine_Rail::ObjID = TEXT("CZipLine_Rail");
CZipLine_Rail::CZipLine_Rail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{pDevice, pContext}
{
}

CZipLine_Rail::CZipLine_Rail(const CZipLine_Rail& rhs)
	: CGimmickBase{rhs}
{
}

CZipLine_Rail::~CZipLine_Rail()
{
	Free();
}

void CZipLine_Rail::Set_Dead(bool bDead)
{
	m_bIsDead = bDead;

	for (auto& pPart : m_vecRailParts)
		pPart->Set_Dead(bDead);

	for (auto& pObs : m_ObstacleList)
		pObs->Set_Dead(bDead);

	m_pMagnet->Set_Dead(bDead);
}

void CZipLine_Rail::Execute(_bool bExecute)
{
	m_bExecuted = bExecute;

	//m_pMagnet->Enter_Event();
	m_iCurRailIndex = 0;

	m_pGameInstance.lock()->Play_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"ZipLineRail", true);
	m_pGameInstance.lock()->Set_Sound_Volume(SOUND_CHANNEL::SOUND_EFFECT, L"ZipLineRail", 1);
	
	_float3 vRightPoint = m_vecRailParts[m_iCurRailIndex]->Get_LocalRightPoint();
	XMStoreFloat3(&m_vMagnetDirection, m_vecRailParts[m_iCurRailIndex]->Calc_WorldDirection());

	if (m_iObjectIndex == 0 && !m_bTalk)
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
			wstring strTalkTag = wstring(L"STKale Talk_File_21.csv");
			PRequiredTalkSystem->LoadTalkFile(strTalkTag);

			CUi_Mgr::Get_Instance()->NotifyUIEvent(UI_EVENT_PROCEED_TALK);
		}
	}

	if (m_iObjectIndex == 1)
	{
		CMapManager::Get_Instance()->Boss_ChangePhase(3);
	}
}

void CZipLine_Rail::Done()
{
	m_bExecuted = false; 
	m_bDone = true;

	m_pGameInstance.lock()->Stop_Sound(SOUND_CHANNEL::SOUND_EFFECT, L"ZipLineRail");
	m_pMagnet->Exit_Event();
}

void CZipLine_Rail::Dying()
{
	for (auto& pPart : m_vecRailParts)
		pPart->Dead();
	for (auto& pObs : m_ObstacleList)
		pObs->Dead();

	m_pMagnet->Dead();
}

HRESULT CZipLine_Rail::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CZipLine_RailPart::ObjID);
	m_List_Owning_ObjectID.push_back(CZipLine_Magnet::ObjID);
	m_List_Owning_ObjectID.push_back(CZipLine_Obstacle::ObjID);

	m_List_ResourceInfo.push_back(make_shared<RES_SOUND>(TEXT("ZipLineRail"), TEXT("../Bin/Resources/Sounds/MapObject/ZipLineRail.ogg")));

	return S_OK;
}

HRESULT CZipLine_Rail::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	Ready_Parts();

	CCine_Manager::Get_Instance()->AddStartCallback
	("EV1150", [&]()
		{
			RenderOff();
		}
	);

	return S_OK;
}

void CZipLine_Rail::PriorityTick(_float fTimeDelta)
{
}

void CZipLine_Rail::Tick(_float fTimeDelta)
{
	//if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_L))
	//	Execute();
	//if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_LEFT))
	//	m_pMagnet->Set_AnimState(CZipLine_Magnet::ANIMSTATE::DODGE_LEFT);
	//if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_RIGHT))
	//	m_pMagnet->Set_AnimState(CZipLine_Magnet::ANIMSTATE::DODGE_RIGHT);
	//if (CGameInstance::Get_Instance()->Get_KeyDown(DIK_UP))
	//	m_pMagnet->Set_AnimState(CZipLine_Magnet::ANIMSTATE::DODGE_TURNOVER);

	if (!m_bExecuted) return;

	m_pMagnetTransform->Go_Vector(XMLoadFloat3(&m_vMagnetDirection), fTimeDelta * m_fMagnetSpeed);

	_vector vMagnetPos = m_pMagnetTransform->Get_Position();
	if (false == m_vecRailParts[m_iCurRailIndex]->IsIn_RailPart(vMagnetPos))
	{
		++m_iCurRailIndex;

		if (m_iCurRailIndex < m_vecRailParts.size())
		{
			XMStoreFloat3(&m_vMagnetDirection, m_vecRailParts[m_iCurRailIndex]->Calc_WorldDirection());
			_vector vNewPos = m_vecRailParts[m_iCurRailIndex]->Adjust_Position(vMagnetPos);
			m_pMagnetTransform->Set_Position(vNewPos);
			_vector vLookPos = m_vecRailParts[m_iCurRailIndex]->Get_WorldRightPoint();
			m_pMagnetTransform->LookAt(vLookPos);
			
			//m_bLerpRot = true;
			//XMStoreFloat3(&m_vBeforeLerp, m_pMagnetTransform->Get_Position() + m_pMagnetTransform->Get_NormLook());
			//XMStoreFloat3(&m_vAfterLerp, vLookPos);
		}
		else
		{
			_vector vFinalPos = m_vecRailParts.back()->Get_WorldRightPoint();
			m_pMagnetTransform->Set_Position(vFinalPos);
			Done();
			m_iCurRailIndex = (_uint)m_vecRailParts.size() - 1;
		}

		if (m_ObstacleList.size() > 0 && m_iCurRailIndex > m_ObstacleList.front()->Get_RailIndex())
		{
			m_ObstacleStoreList.emplace_back(m_ObstacleList.front());
			m_ObstacleList.pop_front();
		}

	}
	else
	{
		_vector vNewPos = m_vecRailParts[m_iCurRailIndex]->Adjust_Position(vMagnetPos);
		m_pMagnetTransform->Set_Position(vNewPos);

		//if (m_bLerpRot)
		//{
		//	m_fLerpTime += fTimeDelta;
		//	_float t = m_fLerpTime / 0.1f;
		//	_vector vLookPos = {};
		//	if (t > 0.9f)
		//	{
		//		vLookPos = XMLoadFloat3(&m_vAfterLerp);
		//		m_bLerpRot = false;
		//		m_fLerpTime = 0.f;
		//	}
		//	else
		//		vLookPos = XMVectorLerp(XMLoadFloat3(&m_vBeforeLerp), XMLoadFloat3(&m_vAfterLerp), t);
		//		
		//	m_pMagnetTransform->LookAt(vLookPos);
		//}
	}

	if (m_ObstacleList.size() > 0 && m_ObstacleList.front()->IsChecked() == false)
	{
		m_pMagnet->Set_CurObstacleInfo(m_ObstacleList.front()->Get_ObstacleInfo());
		m_ObstacleList.front()->Set_Checked(true);
	}
}

void CZipLine_Rail::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();

	if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), 1.f)) return;

	if (!m_bExecuted && !m_bDone && m_pMagnet->Get_EventOnGoing())
		Execute();
}

HRESULT CZipLine_Rail::Render()
{
	return S_OK;
}

void CZipLine_Rail::Edit_Attribute()
{
	if (m_bExecuted) return;

	ImGui::InputFloat("Magnet Speed", &m_fMagnetSpeed);

	ImGui::InputInt("Object Index", &m_iObjectIndex);
	ImGui::InputText("File Name", m_strFileName.data(), MAX_PATH);

	if (ImGui::Button("Load"))
		Load_Children();

	if (ImGui::BeginListBox("##Parts", ImVec2(0.f, 80.f)))
	{
		for (_uint i = 0; i < m_vecRailParts.size(); i++)
		{
			string strListName = "Rail_" + to_string(i);
			_bool bIsSelected = (m_iCurRailIndex == i);
			if (ImGui::Selectable(strListName.c_str(), bIsSelected))
				m_iCurRailIndex = i;

			if (bIsSelected)
				ImGui::SetItemDefaultFocus();			
		}

		ImGui::EndListBox();
	}

	if (ImGui::Button("Add Part"))
	{
		CZipLine_RailPart::RAILPART_DESC partDesc{};
		partDesc.pParentTransform = m_pTransformCom;
		partDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
		partDesc.vLocalMatrix = Identity;
		if (m_vecRailParts.size() > 0)
		{
			partDesc.vLocalMatrix = m_vecRailParts.back()->Get_Transform()->Get_WorldMatrix();
			_float3 vNewPos = m_vecRailParts.back()->Get_LocalRightPoint();
			_vector vLastPos = XMVector3TransformCoord(XMLoadFloat3(&vNewPos), XMLoadFloat4x4(&partDesc.vLocalMatrix));
			partDesc.vLocalMatrix._41 = XMVectorGetX(vLastPos);
			partDesc.vLocalMatrix._42 = XMVectorGetY(vLastPos);
			partDesc.vLocalMatrix._43 = XMVectorGetZ(vLastPos);
		}

		auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CZipLine_RailPart::ObjID, &partDesc);
		assert(pObject);

		m_vecRailParts.emplace_back(static_pointer_cast<CZipLine_RailPart>(pObject));
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove Part") && m_vecRailParts.size() > 0)
	{
		m_vecRailParts[m_iCurRailIndex]->Set_Dead(true);
		m_vecRailParts.erase(m_vecRailParts.begin() + m_iCurRailIndex);
		--m_iCurRailIndex;
		m_iCurRailIndex = max(m_iCurRailIndex, 0);
	}

	if (m_vecRailParts.size() > 0)
	{
		_bool bRapidCurve = m_vecRailParts[m_iCurRailIndex]->Is_RapidCurve();
		if (ImGui::Checkbox("Rapid Curve", &bRapidCurve))
			m_vecRailParts[m_iCurRailIndex]->Set_RapidCurve(bRapidCurve);
	}

	ImGui::SliderInt("Obstacle Dir", &m_iObstacleOption, 0, 2);
	
	if (ImGui::Button("Add Obstacle") && m_vecRailParts.size() > 0)
	{
		CZipLine_Obstacle::ZIPOBS_DESC obsDesc{};
		obsDesc.iRailIndex = m_iCurRailIndex;
		obsDesc.pParentTransform = m_pTransformCom;
		obsDesc.localMat = m_vecRailParts[m_iCurRailIndex]->Get_Transform()->Get_WorldMatrix();
		_vector vLastPos = XMVector3TransformCoord(XMVectorSet(0.f, 0.f, 4.f, 1.f), XMLoadFloat4x4(&obsDesc.localMat));
		obsDesc.localMat._41 = XMVectorGetX(vLastPos);
		obsDesc.localMat._42 = XMVectorGetY(vLastPos);
		obsDesc.localMat._43 = XMVectorGetZ(vLastPos);

		switch (m_iObstacleOption) {
		case 0:
			obsDesc.eDir = CZipLine_Obstacle::OBS_DIR::LEFT;
			break;
		case 1:
			obsDesc.eDir = CZipLine_Obstacle::OBS_DIR::RIGHT;
			break;
		case 2: 
			obsDesc.eDir = CZipLine_Obstacle::OBS_DIR::FRONT;
			break;
		}
		
		auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Map"), CZipLine_Obstacle::ObjID, &obsDesc);
		m_ObstacleList.emplace_back(static_pointer_cast<CZipLine_Obstacle>(pObject));

		OBSDATA tData;
		tData.iObsDir = (_uint)obsDesc.eDir;
		tData.iRailIndex = obsDesc.iRailIndex;
		tData.localMat = obsDesc.localMat;
		m_vecObsData.emplace_back(tData);
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove Obstacle") && m_ObstacleList.size() > 0)
	{
		m_ObstacleList.back()->Set_Dead();
		m_vecObsData.pop_back();
	}

	if (m_vecRailParts.size() > 0)
	{
		tagGizmoDesc m_tGizmoDesc;
		m_tGizmoDesc.CurrentGizmoMode = ImGuizmo::LOCAL;

		auto pPartTransform = m_vecRailParts[m_iCurRailIndex]->Get_Transform();

		_float4x4 WorldMat;
		_float4  vPos;
		_float3 vRot;
		_float3 vScale;

		WorldMat = pPartTransform->Get_WorldMatrix();
		
		_bool flag = false;

		ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &vRot.x, &vScale.x);
		ImGui::PushItemWidth(200.f);
		flag = ImGui::DragFloat3("Pos", &vPos.x, 0.1f);
		if (flag) {
			pPartTransform->Set_Position(XMVectorSet(vPos.x, vPos.y, vPos.z, vPos.w));
		}
		ImGui::PushItemWidth(200.f);

		_float3 vRadianRot = {};

		flag = ImGui::DragFloat3("Rot", &vRot.x);
		if (flag) {
			ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, &WorldMat._11);
			pPartTransform->Set_WorldMatrix(WorldMat);
		}
		ImGui::PushItemWidth(200.f);

		flag = ImGui::DragFloat3("Scl", &vScale.x);
		if (flag) {
			if (vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f) {
				pPartTransform->Set_Scale(vScale);
			}
		}

		_float4x4 ViewMat, ProjMat;
		ViewMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
		ProjMat = GAMEINSTANCE->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

		XMStoreFloat4x4(&WorldMat, XMLoadFloat4x4(&WorldMat)* m_pTransformCom->Get_WorldMatrix_XMMat());

		if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
			, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
			, m_tGizmoDesc.CurrentGizmoMode				// WORLD, LOCAL
			, (_float*)&WorldMat
			, NULL
			, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL
			, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
			, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
		{
			//pPartTransform->Set_WorldMatrix(WorldMat);
		}
	}
}

void CZipLine_Rail::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

	write_typed_data(os, m_iObjectIndex);

	string FilePath = "../Bin/Resources/DataFiles/Map/" + m_strFileName + ".bin";
	ofstream ObjectOs(FilePath, ios::binary);

#ifdef _DEBUG
	assert(ObjectOs.is_open());
#endif

	write_typed_data(ObjectOs, m_fMagnetSpeed);
	write_typed_data(ObjectOs, m_vecRailParts.size());
	for (auto& pPart : m_vecRailParts)
	{
		_float4x4 LocalMat = pPart->Get_Transform()->Get_WorldMatrix();
		write_typed_data(ObjectOs, LocalMat);
		write_typed_data(ObjectOs, pPart->Is_RapidCurve());
	}

	write_typed_data(ObjectOs, m_vecObsData.size());
	sort(m_vecObsData.begin(), m_vecObsData.end(), [&](OBSDATA left, OBSDATA right) {
		return left.iRailIndex < right.iRailIndex;
	});

	for (auto& tDesc : m_vecObsData)
		write_typed_data(ObjectOs, tDesc);

	ObjectOs.close();
}

void CZipLine_Rail::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	read_typed_data(is, m_iObjectIndex);

	if (m_iObjectIndex == 0)
		m_strFileName = "MagnetRailA";
	else if(m_iObjectIndex == 1)
		m_strFileName = "MagnetRailB";

	Load_Children();
}

void CZipLine_Rail::RenderOff()
{
	for (auto& iter : m_ObstacleStoreList)
	{
		iter->Set_RenderState(false);
	}
	for (auto& iter : m_vecRailParts)
	{
		iter->Set_RenderState(false);
	}

	m_pMagnet->Set_RenderState(false);
}

void CZipLine_Rail::Ready_Parts()
{
	_uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

	COMMONOBJ_DESC magnetDesc{};
	magnetDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
	magnetDesc.worldMatrix = m_pTransformCom->Get_WorldMatrix();

	auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(iLevelIndex, TEXT("Layer_Map"), CZipLine_Magnet::ObjID, &magnetDesc);
	assert(pObject);

	m_pMagnet = static_pointer_cast<CZipLine_Magnet>(pObject);
	m_pMagnetTransform = m_pMagnet->Get_Transform();

	m_pMagnetTransform->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CZipLine_Rail::Load_Children()
{
	_uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

	//m_iObjectIndex = 1;
	//m_strFileName = "MagnetRailB";

	string FilePath = "../Bin/Resources/DataFiles/Map/" + m_strFileName + ".bin";
	ifstream ObjectIs(FilePath, ios::binary);

#ifdef _DEBUG
	assert(ObjectIs.is_open());
#endif

	read_typed_data(ObjectIs, m_fMagnetSpeed);

	size_t size = {};
	read_typed_data(ObjectIs, size);
	for (size_t i = 0; i < size; i++)
	{
		_float4x4 LocalMat = Identity;
		read_typed_data(ObjectIs, LocalMat);
		_bool bRapidCurve = false;
		read_typed_data(ObjectIs, bRapidCurve);

		CZipLine_RailPart::RAILPART_DESC partDesc{};
		partDesc.pParentTransform = m_pTransformCom;
		partDesc.vPosition = { 0.f, 0.f, 0.f, 1.f };
		partDesc.vLocalMatrix = LocalMat;
		partDesc.bRapidCurve = bRapidCurve;

		auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(iLevelIndex, TEXT("Layer_Map"), CZipLine_RailPart::ObjID, &partDesc);
		assert(pObject);

		m_vecRailParts.emplace_back(static_pointer_cast<CZipLine_RailPart>(pObject));
	}

	read_typed_data(ObjectIs, size);
	for (size_t i = 0; i < size; i++)
	{
		OBSDATA tData{};
		read_typed_data(ObjectIs, tData);

		CZipLine_Obstacle::ZIPOBS_DESC tDesc{};
		tDesc.eDir = (CZipLine_Obstacle::OBS_DIR)tData.iObsDir;
		tDesc.iRailIndex = tData.iRailIndex;
		tDesc.localMat = tData.localMat;
		tDesc.pParentTransform = m_pTransformCom;

		m_vecObsData.emplace_back(tData);

		auto pObject = CGameInstance::Get_Instance()->Add_Clone_Return(iLevelIndex, TEXT("Layer_Map"), CZipLine_Obstacle::ObjID, &tDesc);
		assert(pObject);

		m_ObstacleList.emplace_back(static_pointer_cast<CZipLine_Obstacle>(pObject));
	}
}

shared_ptr<CZipLine_Rail> CZipLine_Rail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CZipLine_Rail
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CZipLine_Rail(pDevice, pContext)
		{}
	};


	shared_ptr<CZipLine_Rail> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CZipLine_Rail"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CZipLine_Rail::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CZipLine_Rail
	{
		MakeSharedEnabler(const CZipLine_Rail& rhs) : CZipLine_Rail(rhs)
		{}
	};

	shared_ptr<CZipLine_Rail> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CZipLine_Rail"));
		assert(false);
	}

	return pInstance;
}

void CZipLine_Rail::Free()
{
}
