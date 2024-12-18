#include "Enemy_Lift.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "PhysxCollider.h"
#include "Bounding_AABB.h"
#include "Monster_Baboo.h"
#include "Monster_Saber.h"
#include "Monster_Gunner.h"
#include "CombatManager.h"

#include "Beat_Manager.h"

wstring CEnemy_Lift::ObjID = TEXT("CEnemy_Lift");

CEnemy_Lift::CEnemy_Lift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEnemy_Lift::CEnemy_Lift(const CEnemy_Lift& rhs)
	: CGameObject{ rhs }
{
}

CEnemy_Lift::~CEnemy_Lift()
{
	Free();
}


HRESULT CEnemy_Lift::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_float4x4 PreTransform;
	XMStoreFloat4x4(&PreTransform, XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV2, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Enemy_Lift"), TEXT("../Bin/Resources/Models/Map/Gimmick/Enemy_Lift.fbx"), MODEL_TYPE::ANIM, PreTransform)
	);

	return S_OK;
}

HRESULT CEnemy_Lift::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ANIMOBJ_DESC* pDesc = static_cast<ANIMOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (pDesc)
		m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
	m_pModelCom->Set_AnimIndex(0, true);

	m_iPrevBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();

	m_fZDiff = 500.f;

	m_vecPositions.resize(POSITION_END);
	m_vecPositions[LF] = _float3{ 32.f, 26.f,  12.f - m_fZDiff};
	m_vecPositions[LR] = _float3{ 32.f, 26.f, -12.f - m_fZDiff};
	m_vecPositions[RF] = _float3{-32.f, 26.f,  12.f - m_fZDiff};
	m_vecPositions[RR] = _float3{-32.f, 26.f, -12.f - m_fZDiff};
	m_vZPos.resize(POSITION_END);
	m_vZPos[LF] = 12.f;
	m_vZPos[LR] = -12.f;
	m_vZPos[RF] = 12.f;
	m_vZPos[RR] = -12.f;
	m_vecDirection.resize(POSITION_END);
	m_vecDirection[LF] = 0;
	m_vecDirection[LR] = 0;
	m_vecDirection[RF] = 0;
	m_vecDirection[RR] = 0;
	m_vecLeft.resize(POSITION_END);
	m_vecLeft[LF] = 0;
	m_vecLeft[LR] = 0;
	m_vecLeft[RF] = 0;
	m_vecLeft[RR] = 0;
	m_vecMonsterVector.resize(POSITION_END);

	return S_OK;
}

void CEnemy_Lift::PriorityTick(_float fTimeDelta)
{
}

void CEnemy_Lift::Tick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	m_bBeatChanged = false;
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
		m_bBeatChanged = true;
		for (_uint i = 0; i < 4; ++i)
		{
			if (m_vecLeft[i] > 0)
			{
				--m_vecLeft[i];
			}
			else
			{
				m_vecDirection[i] = 0;
			}
		}
	}
	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_SPACE))
	//{
	//	m_bDebug = !m_bDebug;
	//}
	_float4x4 fMat;
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta, 0.f, fMat);

	_float fPerBeat = CBeat_Manager::Get_Instance()->Get_BeatRate();
	_float fPerBeatRoot = (sqrt(fPerBeat));
	_float fPerBeatSqr = (fPerBeat * fPerBeat);
	for (_uint i = 0; i < 4; ++i)
	{
		if (m_vecDirection[i] == 1)
		{
			if(m_vecLeft[i] == 5)
			{
				m_vecPositions[i].z = m_vZPos[i] * fPerBeatRoot + (m_vZPos[i] - m_fZDiff) * (1.f - fPerBeatRoot);
				for (_int j = 0; j < (_int)m_vecMonsterVector[i].size(); ++j)
				{
					_int iIdx = j;
					_int iPos = (iIdx - (_int)m_vecMonsterVector[i].size() / 2);
					_float fDiff = 8.f / (_float)m_vecMonsterVector[i].size();
					_vector vPosition = XMVectorSetW(XMLoadFloat3(&m_vecPositions[i]) + XMVectorSet(0.f, 0.f, iPos * fDiff, 0.f), 1.f);
					//pMon->Get_Transform()->Set_Position(vPosition);
					m_vecMonsterVector[i][j]->SetPos_Hard(vPosition);
				}

			}
			else if (m_vecLeft[i] == 1)
			{
				m_vecPositions[i].z = m_vZPos[i] * (1.f - fPerBeatSqr) + (m_vZPos[i] - m_fZDiff) * fPerBeatSqr;
			}
			else if (m_vecLeft[i] == 0)
			{
				m_vecPositions[i].z = (m_vZPos[i] - m_fZDiff);
			}
			else
			{
				if (m_bBeatChanged && m_vecLeft[i] == 4)
				{
					for (_int j = 0; j < (_int)m_vecMonsterVector[i].size(); ++j)
					{
						_int iIdx = j;
						_int iPos = (iIdx - (_int)m_vecMonsterVector[i].size() / 2);
						_float fDiff = 8.f / (_float)m_vecMonsterVector[i].size();
						_vector vPosition = XMVectorSetW(XMLoadFloat3(&m_vecPositions[i]) + XMVectorSet(0.f, 0.f, iPos * fDiff, 0.f), 1.f);
						//pMon->Get_Transform()->Set_Position(vPosition);
						m_vecMonsterVector[i][j]->SetPos_Hard(vPosition);
						m_vecMonsterVector[i][j]->MoveToPos(XMVectorSetX(vPosition,  m_pGameInstance.lock()->Random_Float(-3.f, 3.f)));
					}
				}
				m_vecPositions[i].z = m_vZPos[i];
			}
		}
		else if (m_vecDirection[i] == -1)
		{
			if (m_vecLeft[i] == 5)
			{
				m_vecPositions[i].z = m_vZPos[i] * fPerBeatRoot + (m_vZPos[i] + m_fZDiff) * (1.f - fPerBeatRoot);

				for (_int j = 0; j < (_int)m_vecMonsterVector[i].size(); ++j)
				{
					_int iIdx = j;
					_int iPos = (iIdx - (_int)m_vecMonsterVector[i].size() / 2);
					_float fDiff = 8.f / (_float)m_vecMonsterVector[i].size();
					_vector vPosition = XMVectorSetW(XMLoadFloat3(&m_vecPositions[i]) + XMVectorSet(0.f, 0.f, iPos * fDiff, 0.f), 1.f);
					//pMon->Get_Transform()->Set_Position(vPosition);
					m_vecMonsterVector[i][j]->SetPos_Hard(vPosition);
				}

			}
			else if (m_vecLeft[i] == 1)
			{
				m_vecPositions[i].z = m_vZPos[i] * (1.f - fPerBeatSqr) + (m_vZPos[i] + m_fZDiff) * fPerBeatSqr;
			}
			else if(m_vecLeft[i] == 0)
			{
				m_vecPositions[i].z = (m_vZPos[i] + m_fZDiff * 2.f);
			}
			else
			{
				if (m_bBeatChanged && m_vecLeft[i] == 4)
				{
					for (_int j = 0; j < (_int)m_vecMonsterVector[i].size(); ++j)
					{
						_int iIdx = j;
						_int iPos = (iIdx - (_int)m_vecMonsterVector[i].size() / 2);
						_float fDiff = 8.f / (_float)m_vecMonsterVector[i].size();
						_vector vPosition = XMVectorSetW(XMLoadFloat3(&m_vecPositions[i]) + XMVectorSet(0.f, 0.f, iPos * fDiff, 0.f), 1.f);
						//pMon->Get_Transform()->Set_Position(vPosition);
						m_vecMonsterVector[i][j]->SetPos_Hard(vPosition);
						m_vecMonsterVector[i][j]->MoveToPos(XMVectorSetX(vPosition, 0.f));// vPosition.m128_f32[0] * -0.4f));
					}
				}
				m_vecPositions[i].z = m_vZPos[i];
			}
		}
		else
		{
			m_vecPositions[i].z = (m_vZPos[i] + m_fZDiff);
		}
	}

	//if (m_pGameInstance.lock()->Get_KeyDown(DIK_O))
	//{
	//	vector<_uint> vec{0,1,2};

	//	Move(LF, 1, vec);
	//	Move(RR, -1, vec);
	//}

}

void CEnemy_Lift::LateTick(_float fTimeDelta)
{
	auto pGi = m_pGameInstance.lock();
	pGi->Add_RenderObjects(CRenderer::RENDER_TERRAIN, shared_from_this());
	for (_uint i = 0; i < 4; ++i)
	{
		_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();

		World.r[3] += XMLoadFloat3(&m_vecPositions[i]);

		m_vecColliders[i]->Update(World);
		pGi->Add_Collider(COLLIDER_GROUP::COLLISION_PROJECTILE, m_vecColliders[i]);
	}
}

HRESULT CEnemy_Lift::Render()
{
	if (!m_pModelCom)
		return S_OK;

	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));



	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for(_uint j = 0; j < 4; ++j)
	{
		_matrix World = m_pTransformCom->Get_WorldMatrix_XMMat();
		if (abs(m_vecPositions[j].z) > 490.f)
			continue;
		World.r[3] += XMLoadFloat3(&m_vecPositions[j]);
		//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
		_float4x4 World4x4;
		XMStoreFloat4x4(&World4x4, World);
		m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World4x4);
		for (_uint i = 0; i < iNumMeshes; i++)
		{
			_bool bNoEmit = false;
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", i, aiTextureType_EMISSIVE)))
			{
				bNoEmit = true;
			}
			if (bNoEmit)
				m_pShaderCom->Begin(0);
			else
				m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);

			if (FAILED(m_pModelCom->Bind_Buffers((_uint)(i))))
				return E_FAIL;


			m_pModelCom->Draw((_uint)i);

		}
	}
#ifdef _DEBUG
	for (auto& pCollider : m_vecColliders)
	{
		pCollider->Render();
	}
#endif
	
	return S_OK;
}

void CEnemy_Lift::Move(POSITION eType, _int iDirection, const vector<_uint>& vecMonType)
{
	auto pGi = m_pGameInstance.lock();
	m_vecPositions[eType].z = (m_vZPos[eType] - m_fZDiff * _float(iDirection));
	m_vecDirection[eType] = iDirection;
	m_vecLeft[eType] = 5;
	_uint iNumMon = (_uint)vecMonType.size();
	//m_vecMonsterVector[eType].resize(iNumMon);
	vector<CMonsterBase::Monster_DESC> vecDesc;
	for (_uint i = 0; i < iNumMon; ++i)
	{
		_wstring strTag;
		if (vecMonType[i] == 1)
		{
			strTag = CMonster_Gunner::ObjID;
		}
		else if(vecMonType[i] == 2)
		{
			strTag = CMonster_Baboo::ObjID;
		}
		else
		{
			strTag = CMonster_Saber::ObjID;
		}
		CMonsterBase::Monster_DESC desc;
		XMStoreFloat4x4(&desc.matTransform, XMMatrixTranslation(-10000.f,-10000.f,-10000.f));
		desc.strDesc = "1";
		desc.strObjID = strTag;
		vecDesc.push_back(desc);

		//m_vecMonsterVector[eType][i] = static_pointer_cast<CMonsterBase>(pGi->Add_Clone_Return(pGi->Get_Current_LevelID(), L"Layer_Monster", strTag));
	}
	auto pCM = CCombat_Manager::Get_Instance();
	pCM->Push_Wave(vecDesc);
	pCM->Finish_Input_Wave(true);
	auto& List = pCM->Get_Enemy_List();
	for (auto& pMon : List)
	{
		_bool bAlready = false;
		for (auto& vec : m_vecMonsterVector)
		{
			for (auto& mon : vec)
			{
				if (mon == pMon)
				{
					bAlready = true;
					break;
				}
			}
			if (bAlready)
				break;
		}
		if (!bAlready)
		{
			m_vecMonsterVector[eType].push_back(static_pointer_cast<CMonsterBase>(pMon));
			_int iIdx = (_int)m_vecMonsterVector[eType].size() - 1;
			_int iPos =  (iIdx - (_int)iNumMon / 2);
			_float fDiff = 8.f / (_float)iNumMon;
			_vector vPosition =  XMVectorSetW(XMLoadFloat3(&m_vecPositions[eType]) + XMVectorSet(0.f, 0.f, iPos * fDiff , 0.f), 1.f);
			//pMon->Get_Transform()->Set_Position(vPosition);
			m_vecMonsterVector[eType].back()->SetPos_Hard(vPosition);
		}
	}
}

void CEnemy_Lift::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CEnemy_Lift::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
	//if (pOther->Get_Collider_Group() == COLLIDER_GROUP::COLLISION_MONSTER)
	//{
	//	auto pOthOwner = dynamic_pointer_cast<CMonsterBase>(pOther->Get_Owner());
	//	if (!pOthOwner)
	//		return;
	//	auto pOthTrans = pOthOwner->Get_Transform();
	//	auto vPos = pOthTrans->Get_Position();
	//	_wstring strTag = L"Com_Collider";
	//	for (_uint i = 0; i < 4; ++i)
	//	{
	//		if (pCaller == m_vecColliders[i])
	//		{
	//			if (vPos.m128_f32[1] < m_pTransformCom->Get_Position().m128_f32[1] + m_vecPositions[i].y)
	//			{
	//				vPos.m128_f32[1] = m_pTransformCom->Get_Position().m128_f32[1] + m_vecPositions[i].y;
	//			}
	//			vPos.m128_f32[2] = m_pTransformCom->Get_Position().m128_f32[2] + m_vecPositions[i].z;
	//		}
	//	}
	//	pOthOwner->SetPos_Hard(vPos);
	//	//pOthTrans->Set_Position(vPos);
	//}
}


HRESULT CEnemy_Lift::Ready_Components(const ANIMOBJ_DESC* pDesc)
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));

	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Enemy_Lift"), TEXT("Com_Model")));

	m_vecColliders.resize(POSITION_END);
	CBounding_AABB::BOUNDING_AABB_DESC desc{};
	desc.vCenter = { 0.f,1.f,0.f };
	desc.vExtents = { 8.f,4.f, 12.f };
	for (_uint i = 0; i < 4; ++i)
	{
		m_vecColliders[i] = static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), _wstring(L"Com_Collider") + to_wstring(i), &desc));
	}


	if (!m_pModelCom)
		assert(false);

	if (!m_pShaderCom /* || !m_pModelCom*/)
		assert(false);


	return S_OK;
}

shared_ptr<CEnemy_Lift> CEnemy_Lift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEnemy_Lift
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEnemy_Lift(pDevice, pContext)
		{}
	};


	shared_ptr<CEnemy_Lift> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEnemy_Lift"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CEnemy_Lift::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEnemy_Lift
	{
		MakeSharedEnabler(const CEnemy_Lift& rhs) : CEnemy_Lift(rhs)
		{}
	};


	shared_ptr<CEnemy_Lift> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEnemy_Lift"));
		assert(false);
	}

	return pInstance;
}

void CEnemy_Lift::Free()
{
}
