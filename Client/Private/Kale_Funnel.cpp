
#include "Kale_Funnel.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "Beat_Manager.h"
#include "PlayerManager.h"
#include "Effect_Manager.h"

wstring CKale_Funnel::ObjID = TEXT("CKale_Funnel");


CKale_Funnel::CKale_Funnel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CKale_Funnel::CKale_Funnel(const CKale_Funnel& rhs)
	: CPartObject(rhs)
{

}

HRESULT CKale_Funnel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	_matrix		LocalTransformMatrix = XMMatrixIdentity();
	
	//LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	LocalTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationRollPitchYaw(0.f, -XM_PIDIV2, XM_PIDIV2);
	_float4x4 preMatrix = {};
	XMStoreFloat4x4(&preMatrix, LocalTransformMatrix);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Kale_Funnel_1"), TEXT("../Bin/Resources/Models/Monsters/Kale/Funnel_1.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Kale_Funnel_2"), TEXT("../Bin/Resources/Models/Monsters/Kale/Funnel_2.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Kale_Funnel_3"), TEXT("../Bin/Resources/Models/Monsters/Kale/Funnel_3.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);
	m_List_ResourceInfo.push_back(
		make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Kale_Funnel_4"), TEXT("../Bin/Resources/Models/Monsters/Kale/Funnel_4.fbx"), MODEL_TYPE::ANIM, preMatrix)
	);

	return S_OK;
}

HRESULT CKale_Funnel::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pBoneMatrix = pDesc->pBoneMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	m_strModelTag = _wstring(L"Prototype_Component_Model_Kale_Funnel_") + to_wstring(pDesc->iNumber);
	if (FAILED(Ready_Components()))
		return E_FAIL;

	//vector<shared_ptr<CAnimation>> animList = m_pModelCom->Get_Animation_List();
	//for (auto& iter : animList)
	//{
	//	m_vecAnimList.push_back(iter->Get_AnimName());
	//}
	m_pModelCom->SetEffectCallback([&](string strName, _float4x4 matPos, _bool isAttach, _bool isBillBoard) {
		if (isAttach)
		{
			CEffect_Manager::Get_Instance()->Call_Union_Effect(strName, &m_WorldMatrix, m_fAnimSpeed, isBillBoard);
		}
		else
		{
			CEffect_Manager::Get_Instance()->Call_Fixed_Union_Effect(strName, &m_WorldMatrix, m_fAnimSpeed, isBillBoard);
		}
		}
	);
	m_pModelCom->Set_AnimIndex(m_eState, false);
	//m_pTransformCom->Rotation(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(-90));
	return S_OK;
}

void CKale_Funnel::PriorityTick(_float fTimeDelta)
{
	_uint iCurrBeat = CBeat_Manager::Get_Instance()->Get_BeatCount();
	if (m_iPrevBeat != iCurrBeat)
	{
		m_iPrevBeat = iCurrBeat;
		if (m_iShootTimer > 0)
		{
			--m_iShootTimer;

		}
		
	}
}

void CKale_Funnel::Tick(_float fTimeDelta)
{
	_float fBeat = BEATMANAGER->Get_Beat();
	if (m_iShootTimer <= 0 && (m_eState == CHARGE || m_eState == TO_ATK) && m_fShootTimerLeft <= BEATMANAGER->Get_Timer())
	{
		m_eState = SHOOT;
		m_pModelCom->Set_AnimIndex(m_eState, false);
	}

	Select_Beat();
	_float4x4 mMat = {};
	m_pModelCom->Play_Animation(fTimeDelta, fTimeDelta * m_fAnimSpeed, 0.1f, mMat);

	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix_XMMat() * BoneMatrix);
	//if(m_eState == CHARGE || m_eState == TO_ATK)
	Look(PLAYERMANAGER->Get_PlayerPos(), fTimeDelta / fBeat * 0.5f);

	//m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
	if (m_pModelCom->Get_Finished())
	{
		Anim_End();
	}
	m_fFloatingPoint += fTimeDelta * XM_PIDIV2 / fBeat;
	m_pModelCom->PlayReservedCallbacks();
}

void CKale_Funnel::LateTick(_float fTimeDelta)
{
	m_iRenderCount = 0;
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
	//GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
}

HRESULT CKale_Funnel::Render()
{
	++m_iRenderCount;
	if (m_iRenderCount == 1)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;
		_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_bool bEmit = true;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
			{
				continue;
			}
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Emission_Texture", (_uint)i, aiTextureType_EMISSIVE)))
			{
				bEmit = false;
			}
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
				return E_FAIL;
			if (bEmit)
			{
				m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Emission);
			}
			else
			{
				m_pShaderCom->Begin(0);
			}

			//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			//   return E_FAIL;

			m_pModelCom->Render((_uint)i);
			//if(!bEmit)
			//{
			//	m_pShaderCom->Bind_Float("g_OutlineWidth", 1.0f);
			//	m_pShaderCom->Begin((_uint)ANIMMESH_PASS::Outline);

			//	m_pModelCom->Draw((_uint)i);
			//}
		}
	}
	//else
	//{
	//	if (FAILED(Bind_ShaderResources()))
	//		return E_FAIL;
	//	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	//	for (size_t i = 0; i < iNumMeshes; i++)
	//	{
	//		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", (_uint)i, aiTextureType_DIFFUSE)))
	//		{
	//			continue;
	//		}

	//		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrix", (_uint)i)))
	//			return E_FAIL;

	//		m_pShaderCom->Begin(0);

	//		//if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
	//		//   return E_FAIL;

	//		m_pModelCom->Render((_uint)i);
	//	}
	//}


	return S_OK;
}

void CKale_Funnel::Shoot(_int iTimer)
{
	m_eState = TO_ATK;
	m_pModelCom->Set_AnimIndex(m_eState, false);
	m_iShootTimer = iTimer;
	m_fShootTimerLeft = BEATMANAGER->Get_Timer();
}

void CKale_Funnel::Anim_End()
{
	switch (m_eState)
	{
	case Client::CKale_Funnel::TO_ATK:
		m_eState = CHARGE;
		m_pModelCom->Set_AnimIndex(m_eState, true);
		break;
	case Client::CKale_Funnel::CHARGE:
		m_eState = SHOOT;
		m_pModelCom->Set_AnimIndex(m_eState, false);
		break;
	case Client::CKale_Funnel::TO_IDLE:
		break;
	case Client::CKale_Funnel::SHOOT:
		m_eState = TO_IDLE;
		m_pModelCom->Set_AnimIndex(m_eState, false);
		break;
	case Client::CKale_Funnel::STATE_END:
		break;
	default:
		break;
	}
}

void CKale_Funnel::Select_Beat()
{
	_float fTotalBeat = m_pModelCom->Get_Duration() / 15.f;

	_float fPlayTime = m_pModelCom->Get_Animation_PlayTime();
	_float fBeat = CBeat_Manager::Get_Instance()->Get_Beat();

	m_fAnimSpeed = fPlayTime / fBeat / fTotalBeat;
}

void CKale_Funnel::Look(_fvector vAt, _float fTimeDelta)
{
	_matrix World = XMLoadFloat4x4(&m_WorldMatrix);

	//{
	//	_vector up = World.r[2];
	//	_vector look = World.r[1];
	//	_vector right = XMVector3Cross(up, look);
	//	World.r[0] = right;
	//	World.r[1] = up;
	//	World.r[2] = look;
	//}
	

	_float3 vScale = { XMVector3Length(World.r[0]).m128_f32[0],  XMVector3Length(World.r[1]).m128_f32[0],  XMVector3Length(World.r[2]).m128_f32[0] };
	_vector vLook = vAt - World.r[3];
	vLook.m128_f32[2] += 1.f;
	_vector vTargetLook = vLook;
	if (XMVectorGetX(XMVector3Length(vLook)) < 0.0001f)
		return;
	////World.r[2].m128_f32[1] = vLook.m128_f32[1];
	//_vector rotationAxis = XMVector3Normalize(XMVector3Cross(World.r[2], vLook));
	//
	//if (XMVectorGetX(XMVector3LengthSq(rotationAxis)) < 0.0001f)
	//	return;
	//
	//float dotProduct = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(World.r[2])));
	////float dotProduct2 = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(World.r[2])));
	//float fAngle = acos(dotProduct) * fTimeDelta;
	//XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(rotationAxis, fAngle);
	////_float y = vLook.m128_f32[1];
	//
	//vLook = XMVector3Normalize(XMVector3Rotate(World.r[2], rotationQuaternion));
	////vLook.m128_f32[1] = y;
	////vLook = XMVector3Normalize(vLook);
	//
	//if (XMVector4IsNaN(vLook) || XMVectorGetX(XMVector3LengthSq(vLook)) < FLT_MIN)
	//	return;

	_vector vRight = XMVector3Cross( XMVectorSet(0.f,1.f,0.f,0.f), vLook);

	//dotProduct = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(vTargetLook)));
	//fAngle = acos(dotProduct);
	vLook = XMVector3Rotate(vLook, XMQuaternionRotationAxis(vRight, XM_PIDIV2));
	_vector vUp = XMVector3Cross(vLook, vRight);

	World.r[0] = XMVector3Normalize(vRight) * vScale.x;
	World.r[1] = XMVector3Normalize(vUp) * vScale.y;
	World.r[2] = XMVector3Normalize(vLook) * vScale.z;

	XMStoreFloat4x4(&m_WorldMatrix, World);
	//m_WorldMatrix = m_pGameInstance.lock()->Matrix_BillBoard(m_WorldMatrix);
}


HRESULT CKale_Funnel::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));


	/* For.Com_Model */
	m_pModelCom = static_pointer_cast<CModel>(__super::Add_Component(LEVEL_KALEBOSS, m_strModelTag, TEXT("Com_Model")));

	if (!m_pShaderCom || !m_pModelCom)
		assert(false);

	return S_OK;
}

HRESULT CKale_Funnel::Bind_ShaderResources()
{
	_float4x4 ViewMat;
	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_float4x4 ProjMat;
	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	/* 부모의 행렬을 완전히 무시한 상황. */
	if (m_eState == TO_IDLE)
	{
		_float fYPos = cos(m_fFloatingPoint) * 0.1f;
		_float4x4 MatFloatingPoint = Identity;
		MatFloatingPoint._41 = fYPos;
		XMStoreFloat4x4(&MatFloatingPoint, XMLoadFloat4x4(&MatFloatingPoint) * XMLoadFloat4x4(& m_WorldMatrix));
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &MatFloatingPoint)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_ObjectID", &m_iObjectID, sizeof(_uint))))
	//   return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CKale_Funnel> CKale_Funnel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CKale_Funnel
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CKale_Funnel(pDevice, pContext)
		{}
	};


	shared_ptr<CKale_Funnel> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKale_Funnel"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CKale_Funnel::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CKale_Funnel
	{
		MakeSharedEnabler(const CKale_Funnel& rhs) : CKale_Funnel(rhs)
		{}
	};


	shared_ptr<CKale_Funnel> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKale_Funnel"));
		assert(false);
	}

	return pInstance;
}


void CKale_Funnel::Free()
{

}

CKale_Funnel::~CKale_Funnel()
{
	Free();
}
