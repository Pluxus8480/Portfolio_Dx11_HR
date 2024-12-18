#include "LaserCircuit.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "PhysxCollider.h"
#include "Bounding_OBB.h"
#include "Mesh.h"
#include "Chai.h"
#include "CharacterController.h"
#include "Texture.h"
#include "Beat_Manager.h"
#include "LaserPod.h"

wstring CLaserCircuit::ObjID = TEXT("CLaserCircuit");
CLaserCircuit::CLaserCircuit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGimmickBase{ pDevice, pContext }
{
}

CLaserCircuit::CLaserCircuit(const CLaserCircuit& rhs)
	: CGimmickBase{ rhs }
{
}

CLaserCircuit::~CLaserCircuit()
{
	Free();
}

void CLaserCircuit::Execute(_bool bExecute)
{
	__super::Execute(bExecute);
	if(m_bExecuted)
		Shuffle_Route(2);
	else
		m_pGameInstance.lock()->Stop_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Loop");
}

HRESULT CLaserCircuit::Initialize_Prototype()
{
	_float4x4 fMat = {};
	XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_LaserCircuit"), TEXT("../Bin/Resources/Models/Map/Gimmick/LaserCircuit.fbx"),
			MODEL_TYPE::NONANIM, fMat)
	);

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Texture_Emit_Circuit"), TEXT("../Bin/Resources/Textures/Emit/t_comemisimplea_e_%d.dds"), 6));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_Shoot"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_9.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_LoopSound_Laser_Loop"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_27.ogg")));
	m_List_ResourceInfo.push_back(make_shared<RES_SOUND_3D>(TEXT("Sound_Laser_Signal"), TEXT("../Bin/Resources/Sounds/SFX/Laser/Object_Common_285.ogg")));




	return S_OK;
}

HRESULT CLaserCircuit::Initialize(void* pArg)
{
	//COMMONOBJ_DESC* pDesc = static_cast<COMMONOBJ_DESC*>(pArg);
	//pDesc->fSpeedPerSec = 80.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vecEmitColor.resize(83);
	for (_uint i = 0; i < 83; ++i)
	{
		m_vecEmitColor[i] = COLOR_END;
	}

	m_vecRouteA.resize(6);

	m_vecRouteA[0] = {67, 72, 73, 54, 55, 58, 57, 82};
	m_vecRouteA[1] = {43, 44, 47, 48, 51, 37, 36, 38};
	m_vecRouteA[2] = {1, 2, 42, 46, 60, 29, 30, 31, 34, 35, 24, 39};
	m_vecRouteA[3] = {40, 41, 3, 4, 7, 27, 28, 61, 22, 23, 80, 78, 79};
	m_vecRouteA[4] = {25, 26, 6, 5, 8, 9, 20, 62, 63, 64, 14, 13, 15};
	m_vecRouteA[5] = {16, 17, 18, 19, 10, 11, 12, 65, 66};
	m_vecRouteAPosition.resize(6);
	{
		_uint iRouteIdx = 0;
		for (auto& Route : m_vecRouteA)
		{
			_uint iPosIdx = 0;
			for (auto& Idx : Route)
			{
				const auto& Vertices = m_pModelCom->Get_Mesh(Idx - 1)->Get_Vertices();
				_float fXMax = -10000.f, fXMin = 10000.f;// , fZMax = -10000.f, fZMin = 10000.f;
				_uint iMaxIdx = 0;
				_uint iMinIdx = 0;
				_uint iVertexIdx = 0;
				for (auto& Vertex : Vertices)
				{
					if (fXMax < Vertex.x)
					{
						fXMax = Vertex.x;
						iMaxIdx = iVertexIdx;
					}
					if (fXMin > Vertex.x)
					{
						fXMin = Vertex.x;
						iMinIdx = iVertexIdx;
					}
					//if (fZMax < Vertex.z)
					//{
					//	fZMax = Vertex.z;
					//}
					//if (fZMin > Vertex.z)
					//{
					//	fZMin = Vertex.z;
					//}
					++iVertexIdx;
				}
				m_vecRouteAPosition[iRouteIdx].push_back({ fXMin, m_fPodYPos, Vertices[iMinIdx].z, 1.f });
				if (iPosIdx == Route.size() - 1)
				{
					m_vecRouteAPosition[iRouteIdx].push_back({ fXMax, m_fPodYPos, Vertices[iMaxIdx].z, 1.f });
				}

				++iPosIdx;
			}
			++iRouteIdx;
		}
	}

	m_vecRouteB.resize(5);
	m_vecRouteB[0] = {67, 68, 69, 70, 75, 74, 76, 53, 55, 58, 57, 82};
	m_vecRouteB[1] = {43, 44, 47, 33, 31, 34, 36, 38};
	m_vecRouteB[2] = {1, 2, 42, 45, 28, 29, 30, 32, 23, 21, 24, 39};
	m_vecRouteB[3] = {40, 41, 3, 4, 7, 27, 50, 59, 9, 10, 11, 12, 14, 13, 15};
	m_vecRouteB[4] = {16, 17, 18, 19, 20, 22, 83, 77, 78, 79 };
	m_vecRouteBPosition.resize(5);
	{
		_uint iRouteIdx = 0;
		for (auto& Route : m_vecRouteB)
		{
			_uint iPosIdx = 0;
			for (auto& Idx : Route)
			{
				const auto& Vertices = m_pModelCom->Get_Mesh(Idx - 1)->Get_Vertices();
				_float fXMax = -10000.f, fXMin = 10000.f;// , fZMax = -10000.f, fZMin = 10000.f;
				_uint iMaxIdx = 0;
				_uint iMinIdx = 0;
				_uint iVertexIdx = 0;
				for (auto& Vertex : Vertices)
				{
					if (fXMax < Vertex.x)
					{
						fXMax = Vertex.x;
						iMaxIdx = iVertexIdx;
					}
					if (fXMin > Vertex.x)
					{
						fXMin = Vertex.x;
						iMinIdx = iVertexIdx;
					}
					//if (fZMax < Vertex.z)
					//{
					//	fZMax = Vertex.z;
					//}
					//if (fZMin > Vertex.z)
					//{
					//	fZMin = Vertex.z;
					//}
					++iVertexIdx;
				}
				m_vecRouteBPosition[iRouteIdx].push_back({ fXMin, m_fPodYPos, Vertices[iMinIdx].z, 1.f });
				if (iPosIdx == Route.size() - 1)
				{
					m_vecRouteBPosition[iRouteIdx].push_back({ fXMax, m_fPodYPos, Vertices[iMaxIdx].z, 1.f });
				}

				++iPosIdx;
			}
			++iRouteIdx;
		}
	}



	m_iPrevBeatCnt = CBeat_Manager::Get_Instance()->Get_BeatCount();
	m_LaserPods.resize(2);
	m_LaserPods[0] = static_pointer_cast<CLaserPod>(GAMEINSTANCE->Clone_Object(CLaserPod::ObjID, nullptr));
	m_LaserPods[1] = static_pointer_cast<CLaserPod>(GAMEINSTANCE->Clone_Object(CLaserPod::ObjID, nullptr));
	m_LaserPods[0]->Set_Parent(m_pTransformCom->Get_WorldMatrix_Ptr());
	m_LaserPods[1]->Set_Parent(m_pTransformCom->Get_WorldMatrix_Ptr());


	m_vecPodPos.resize(2);
	m_vecPodPos[0] = { 0.f,12.f,0.f, 1.f };
	m_vecPodPos[1] = { 0.f,12.f,0.f, 1.f };
	m_vecPodInterpolTime.resize(2);
	m_vecPodCurrRoutePos.resize(2);
	m_vecPodCurrRouteIdx.resize(2);

	return S_OK;
}

void CLaserCircuit::PriorityTick(_float fTimeDelta)
{
	for (auto& LaserPod : m_LaserPods)
	{
		LaserPod->PriorityTick(fTimeDelta);
	}
}

void CLaserCircuit::Tick(_float fTimeDelta)
{
	//if (GAMEINSTANCE->Get_KeyDown(DIK_O))
	//	Execute();
	auto pGameInstance = m_pGameInstance.lock();
	_int iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if(m_bExecuted)
	{
		if (m_iPrevBeatCnt != iCurrBeat)
		{
			m_iPrevBeatCnt = iCurrBeat;
			{
				if (m_iWarnCount > 0)
				{
					m_iWarnFrequency = 4;
					--m_iWarnCount;
				}
				else
				{
					if(m_iWarnCount == 0)
					{
						if (!pGameInstance->Is_Sound_3D_Playing(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot"))
							pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Shoot", m_pTransformCom->Get_Position(), XMVectorZero(), false);
					}
					--m_iWarnCount;
					
					--m_iMoveCount;
					//pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Loop", m_pTransformCom->Get_Position());
					if (m_iMoveCount == 0)
					{
						m_bExecuted = false;
						//pGameInstance->Stop_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Loop");
					}
				}
			}
		}
		if (m_iWarnFrequency > 0)
		{
			if (CBeat_Manager::Get_Instance()->Get_Timer() > CBeat_Manager::Get_Instance()->Get_Beat() * _float(4 - m_iWarnFrequency) * 0.25f)
			{
				_uint iRand1 = rand() % m_vecRouteColor.size();
				_uint iRand2 = rand() % m_vecRouteColor.size();
				Turn_Off();
				Turn_On(m_vecRouteColor.size() == 6, iRand1);
				Turn_On(m_vecRouteColor.size() == 6, iRand2);
				--m_iWarnFrequency;
				pGameInstance->Play_Sound_3D(SOUND_CHANNEL::SOUND_EFFECT, L"Sound_Laser_Signal", m_pTransformCom->Get_Position());
			}
		}
		if (m_iWarnCount >= 0)
		{
			_bool bA = m_vecRouteColor.size() == 6;
			for(_uint i = 0; i< 2;++i)
			{
				if (bA)
				{
					_vector vDst = XMLoadFloat4(&m_vecRouteAPosition[m_vecPodCurrRouteIdx[i]][i]);
					_vector vPos = m_LaserPods[i]->Get_Transform()->Get_Position();
					_vector vDir = (vDst - vPos);
					if (XMVectorGetX(XMVector3Length(vDir)) <= 0.1f * 0.016f / fTimeDelta)
					{
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vDst, m_fPodYPos));
					}
					else
					{
						vPos += XMVector3Normalize(vDir) * fTimeDelta * 15.f;
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vPos, m_fPodYPos));
					}
				}
				else
				{
					_vector vDst = XMLoadFloat4(&m_vecRouteBPosition[m_vecPodCurrRouteIdx[i]][i]);
					_vector vPos = m_LaserPods[i]->Get_Transform()->Get_Position();
					_vector vDir = (vDst - vPos);
					if (XMVectorGetX(XMVector3Length(vDir)) <= 0.1f * 0.016f / fTimeDelta)
					{
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vDst, m_fPodYPos));
					}
					else
					{
						vPos += XMVector3Normalize(vDir) * fTimeDelta * 15.f;
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vPos, m_fPodYPos));
					}

				}
			}

			m_LaserPods[0]->Stop();
			m_LaserPods[1]->Stop();
		}
		else
		{
			Turn_Off();
			for (_uint i = 0; i < 2; ++i)
			{
				_bool bA = m_vecRouteColor.size() == 6;
				_uint iRouteIdx = m_vecPodCurrRouteIdx[i];
				m_LaserPods[i]->Shoot();
				if (bA)
				{
					auto& RouteVector = m_vecRouteAPosition[iRouteIdx];
					_uint iSize = static_cast<_uint>(RouteVector.size());
					_uint iStart = (iSize * (4 - m_iMoveCount)) / 4;
					_uint iEnd = (iSize * (5 - m_iMoveCount)) / 4;
					_uint iFromTo = iEnd - iStart;
					_uint& iCurr = m_vecPodCurrRoutePos[i];
					_float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
					_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
					_float fPerOne = 1.f / (_float)iFromTo;

					_float fTimerPerBeat = fTimer / fBeat;
					_uint iFrom = 0;
					_float fLeftOver = 0.f;
					while (fTimerPerBeat > fPerOne)
					{
						fTimerPerBeat -= fPerOne;
						++iFrom;
					}
					iFrom += iStart;
					_float fInterpol = fTimerPerBeat / fPerOne;
					if (iFrom >= RouteVector.size() - 1)
					{
						_vector vPosFrom = XMLoadFloat4(&RouteVector.back());

						_vector vPodPosition = vPosFrom;
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vPodPosition, m_fPodYPos));
						for (_uint j = 0; j <= iFrom; ++j)
						{
							Turn_On(bA, iRouteIdx, j);
						}
					}
					else
					{
						_vector vPosFrom = XMLoadFloat4(&RouteVector[iFrom]);
						_vector vPosTo = XMLoadFloat4(&RouteVector[iFrom + 1]);

						_vector vPodPosition = vPosFrom + (vPosTo - vPosFrom) * fInterpol;
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vPodPosition, m_fPodYPos));
						for (_uint j = 0; j <= iFrom; ++j)
						{
							Turn_On(bA, iRouteIdx, j);
						}
					}


				}
				else
				{
					auto& RouteVector = m_vecRouteBPosition[iRouteIdx];
					_uint iSize = static_cast<_uint>(RouteVector.size());
					_uint iStart = (iSize * (4 - m_iMoveCount)) / 4;
					_uint iEnd = (iSize * (5 - m_iMoveCount)) / 4;
					_uint iFromTo = iEnd - iStart;
					_uint& iCurr = m_vecPodCurrRoutePos[i];
					_float fTimer = CBeat_Manager::Get_Instance()->Get_Timer();
					_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();
					_float fPerOne = 1.f / (_float)iFromTo;

					_float fTimerPerBeat = fTimer / fBeat;
					_uint iFrom = 0;
					_float fLeftOver = 0.f;
					while (fTimerPerBeat > fPerOne)
					{
						fTimerPerBeat -= fPerOne;
						++iFrom;
					}
					iFrom += iStart;
					_float fInterpol = fTimerPerBeat / fPerOne;
					if (iFrom >= RouteVector.size() - 1)
					{
						_vector vPosFrom = XMLoadFloat4(&RouteVector.back());

						_vector vPodPosition = vPosFrom;
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vPodPosition, m_fPodYPos));
						for (_uint j = 0; j < iFrom - 1; ++j)
						{
							Turn_On(bA, iRouteIdx, j);
						}
					}
					else
					{
						_vector vPosFrom = XMLoadFloat4(&RouteVector[iFrom]);
						_vector vPosTo = XMLoadFloat4(&RouteVector[iFrom + 1]);

						_vector vPodPosition = vPosFrom + (vPosTo - vPosFrom) * fInterpol;
						m_LaserPods[i]->Get_Transform()->Set_Position(XMVectorSetY(vPodPosition, m_fPodYPos));
						for (_uint j = 0; j < iFrom; ++j)
						{
							Turn_On(bA, iRouteIdx, j);
						}
					}


				}
			}
		}

	}
	else
	{
		m_LaserPods[0]->Stop();
		m_LaserPods[1]->Stop();
	}
	m_LaserPods[0]->Tick(fTimeDelta);
	m_LaserPods[1]->Tick(fTimeDelta);
}

void CLaserCircuit::LateTick(_float fTimeDelta)
{
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	for (auto& LaserPod : m_LaserPods)
	{
		LaserPod->LateTick(fTimeDelta);
	}
	//GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_GIMMICK, m_pColliderCom);
}

HRESULT CLaserCircuit::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	//m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)0, aiTextureType_EMISSIVE);

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_bool bNoEmit = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;
		if (i < 83 && m_vecEmitColor[i] != COLOR_END)
		{
			m_pEmissionTexture->Bind_ShaderResource(m_pShaderCom, "g_Emission_Texture", m_vecEmitColor[i]);
			m_pShaderCom->Begin((_uint)MESH_PASS::Emission);
		}
		else
			m_pShaderCom->Begin((_uint)MESH_PASS::Default);
		m_pModelCom->Render((_uint)i);
	}



	return S_OK;
}

void CLaserCircuit::Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr<CCollider> pCaller)
{
}

void CLaserCircuit::Collision(shared_ptr< CCollider> pOther, shared_ptr<CCollider> pCaller)
{
}

void CLaserCircuit::Edit_Attribute()
{


}

void CLaserCircuit::Save_Data(ofstream& os)
{
	__super::Save_Data(os);

}

void CLaserCircuit::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

}

void CLaserCircuit::Turn_Off()
{
	for (auto& Color : m_vecEmitColor)
	{
		Color = COLOR_END;
	}
}

void CLaserCircuit::Shuffle_Route(_uint iRedNum)
{
	if (iRedNum > 2)
		iRedNum = 2;
	_bool bA = rand() % 2;
	if (bA)
	{
		m_vecRouteColor.resize(6 - iRedNum);
	}
	else
	{
		m_vecRouteColor.resize(5 - iRedNum);
	}

	for (_uint i = 0; i < m_vecRouteColor.size(); ++i)
	{
		m_vecRouteColor[i] = (COLOR)(i + 1);
	}
	for (_uint i = 0; i < iRedNum; ++i)
	{
		m_vecRouteColor.push_back(RED);
	}
	for (_uint i = 0; i < 100; ++i)
	{
		_uint rhs = rand() % m_vecRouteColor.size();
		_uint lhs = rand() % m_vecRouteColor.size();
		COLOR Temp = m_vecRouteColor[rhs];
		m_vecRouteColor[rhs] = m_vecRouteColor[lhs];
		m_vecRouteColor[lhs] = Temp;
	}
	_uint iRedRoute = 0;
	_uint iPodIdx = 0;
	for (auto& Color : m_vecRouteColor)
	{
		if (Color == RED)
		{
			m_vecPodCurrRouteIdx[iPodIdx] = iRedRoute;

			_uint iRouteSize = 0;
			if (m_vecRouteColor.size() == 6)
			{
				iRouteSize = static_cast<_uint>(m_vecRouteAPosition[iRedRoute].size());
			}
			else
			{
				iRouteSize = static_cast<_uint>(m_vecRouteBPosition[iRedRoute].size());
			}
			m_vecPodCurrRoutePos[iPodIdx] = 0;
			++iPodIdx;
		}
		++iRedRoute;
	}
	m_iWarnCount = 4;
	m_iMoveCount = 5;
	m_iWarnFrequency = 4;



}

void CLaserCircuit::Turn_On(_bool bA)
{
	Turn_Off();
	if (bA)
	{
		for (_uint i = 0; i < 6; ++i)
		{
			for (_uint j = 0; j < m_vecRouteA[i].size(); ++j)
			{
				m_vecEmitColor[m_vecRouteA[i][j] - 1] = m_vecRouteColor[i];
			}
		}
	}
	else
	{
		for (_uint i = 0; i < 5; ++i)
		{
			for (_uint j = 0; j < m_vecRouteB[i].size(); ++j)
			{
				m_vecEmitColor[m_vecRouteB[i][j] - 1] = m_vecRouteColor[i];
			}
		}
	}
}

void CLaserCircuit::Turn_On(_bool bA, _uint iIdx)
{
	if (bA)
	{
		for (_uint j = 0; j < m_vecRouteA[iIdx].size(); ++j)
		{
			m_vecEmitColor[m_vecRouteA[iIdx][j] - 1] = m_vecRouteColor[iIdx];
		}
	}
	else
	{
		for (_uint j = 0; j < m_vecRouteB[iIdx].size(); ++j)
		{
			m_vecEmitColor[m_vecRouteB[iIdx][j] - 1] = m_vecRouteColor[iIdx];
		}
	}
}

void CLaserCircuit::Turn_On(_bool bA, _uint iRouteIdx, _uint iIdx)
{
	if (bA)
	{
		if (m_vecRouteA[iRouteIdx].size() <= iIdx)
			return;
		m_vecEmitColor[m_vecRouteA[iRouteIdx][iIdx] - 1] = m_vecRouteColor[iRouteIdx];
	}
	else
	{
		if (m_vecRouteB[iRouteIdx].size() <= iIdx)
			return;
		m_vecEmitColor[m_vecRouteB[iRouteIdx][iIdx] - 1] = m_vecRouteColor[iRouteIdx];
	}
}

HRESULT CLaserCircuit::Ready_Components()
{
	m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
	m_pModelCom = static_pointer_cast<CModel>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LaserCircuit"), TEXT("Model_Com")));
	m_pEmissionTexture = static_pointer_cast<CTexture>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Emit_Circuit"), TEXT("Texture_Com"))); 
	

	if (!m_pShaderCom || !m_pModelCom || !m_pEmissionTexture)
		assert(false);

	return S_OK;
}

HRESULT CLaserCircuit::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//    return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CLaserCircuit> CLaserCircuit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLaserCircuit
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLaserCircuit(pDevice, pContext)
		{}
	};

	shared_ptr<CLaserCircuit> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLaserCircuit"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CLaserCircuit::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLaserCircuit
	{
		MakeSharedEnabler(const CLaserCircuit& rhs) : CLaserCircuit(rhs)
		{}
	};

	shared_ptr<CLaserCircuit> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLaserCircuit"));
		assert(false);
	}

	return pInstance;
}

void CLaserCircuit::Free()
{
}
