#include "Effect_Union.h"
#include "GameInstance.h"
#include "Camera.h"
//#include "GameInstance.h"
//#include "PipeLine.h"
//#include "Shader.h"
//#include "Texture.h"
//#include "VIBuffer_Cube.h"
//#include "Model.h"

wstring CEffect_Union::ObjID = TEXT("CEffect_Union");

CEffect_Union::CEffect_Union(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect_Union::CEffect_Union(const CEffect_Union& rhs)
	: CGameObject(rhs)
{
}

CEffect_Union::~CEffect_Union()
{
	Free();
}

HRESULT CEffect_Union::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Union::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	EffectUnionDesc* pDesc = (EffectUnionDesc*)pArg;

	m_strEffectUnionKey = pDesc->m_strEffectUnionKey;

	UnionEffectCreate();

	return S_OK;
}

void CEffect_Union::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Union::Tick(_float fTimeDelta)
{
	State_Engine(fTimeDelta);
}

void CEffect_Union::LateTick(_float fTimeDelta)
{
}

void CEffect_Union::State_Engine(_float fTimeDelta)
{
	switch (m_eEffectUnionState)
	{
	case EFFECT_UNION_SLEEP:
		UnionEffectSleep();
		break;
	case EFFECT_UNION_START:
		UnionEffectStart();
		break;
	case EFFECT_UNION_PLAY:
		UnionEffectPlay(fTimeDelta);
		break;
	case EFFECT_UNION_END:
		UnionEffectEnd();
		break;
	default:
		break;
	}
}

HRESULT CEffect_Union::Render()
{
	return S_OK;
}

void CEffect_Union::UnionMove(_float fTimeDelta)
{
	if (m_fMoveSpeed == 0.f)
		return;

	m_pTransformCom->Set_Speed(m_fMoveSpeed);
	m_pTransformCom->Go_Vector(XMVectorSet(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z, 0.f), fTimeDelta);
}

void CEffect_Union::UnionEffectSleep()
{
}

void CEffect_Union::UnionEffectPlay(_float fTimeDelta)
{
	m_fEffectUnionTime += fTimeDelta * m_fMultiplier;

	if (m_pMatWorldPtr != nullptr)
		m_pTransformCom->Set_WorldMatrix(*m_pMatWorldPtr);

	if (m_bMoveEffect)
		UnionMove(fTimeDelta);

	if (m_fEffectUnionTime >= m_fEffectUnionMaxTime)
		m_eEffectUnionState = EFFECT_UNION_END;
}

void CEffect_Union::UnionEffectResume(_float fTimeDelta)
{
}

void CEffect_Union::UnionEffectStop()
{
	m_eEffectUnionState = EFFECT_UNION_END;
}

void CEffect_Union::UnionEffectStart()
{
	UnionEffectStartEvent();
	for (auto& pTempEffect : m_vecEffects)
	{
		//pTempEffect->EffectStart();
		pTempEffect->Set_PlayOnce(false);

		_float4x4 matWorld = m_pTransformCom->Get_WorldMatrix();
		_float4x4 matRoot = pTempEffect->GetRootMatrix();

		if (pTempEffect->Get_LoopObject()) pTempEffect->SetEffectLoop(true);

		pTempEffect->Set_ParentWorld(m_pTransformCom->Get_WorldMatrix_Ptr());

		pTempEffect->Get_Transform()->Set_WorldMatrix(XMMatrixMultiply(XMLoadFloat4x4(&matRoot), XMLoadFloat4x4(&matWorld)));

		GAMEINSTANCE->Push_Object(LEVEL_STATIC, TEXT("Layer_Effect"), pTempEffect);
		pTempEffect->Set_Dead(false);
	}
	m_fEffectUnionTime = 0.f;
	m_eEffectUnionState = EFFECT_UNION_PLAY;
}

void CEffect_Union::UnionEffectEnd()
{
	UnionEffectEndEvent();
	for (auto& pTempEffect : m_vecEffects)
	{
		pTempEffect->SetEffectLoop(false);
		pTempEffect->EffectEnd();
		pTempEffect->Set_PlayOnce(false);

		pTempEffect->Dead();
	}
	m_fEffectUnionTime = 0.f;

	m_fMultiplier = 1.f;
	m_pMatWorldPtr = nullptr;
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_fMoveSpeed = 0.f;
	m_vMoveDir = { 0.f,0.f,0.f };

	m_pTransformCom->Set_Speed(0.f);

	Dead();

	m_eEffectUnionState = EFFECT_UNION_SLEEP;
}

void CEffect_Union::UnionEffectStartEvent()
{
}

void CEffect_Union::UnionEffectEndEvent()
{
}

void CEffect_Union::PlayEffect(_float fMultiplier, _float4x4 matWorld)
{
	if (m_eEffectUnionState != EFFECT_UNION_SLEEP)
		return;

	m_fMultiplier = fMultiplier;

	m_matFixWorld = matWorld;

	m_pTransformCom->Set_WorldMatrix(m_matFixWorld);

	m_eEffectUnionState = EFFECT_UNION_START;
}

void CEffect_Union::PlayEffect(_float fMultiplier, _float4x4 matWorld, _float _fMoveSpeed, _float3 _vMoveDir)
{
	if (m_eEffectUnionState != EFFECT_UNION_SLEEP)
		return;

	m_fMultiplier = fMultiplier;

	m_matFixWorld = matWorld;

	m_pTransformCom->Set_WorldMatrix(m_matFixWorld);

	m_bMoveEffect = true;
	m_fMoveSpeed = _fMoveSpeed;
	m_vMoveDir = _vMoveDir;

	m_eEffectUnionState = EFFECT_UNION_START;
}

void CEffect_Union::PlayEffect(_float fMultiplier, _float4x4* matWorld)
{
	if (m_eEffectUnionState != EFFECT_UNION_SLEEP)
		return;

	m_fMultiplier = fMultiplier;

	m_pMatWorldPtr = matWorld;

	m_pTransformCom->Set_WorldMatrix(*m_pMatWorldPtr);

	m_eEffectUnionState = EFFECT_UNION_START;
}

shared_ptr<CEffect_Union> CEffect_Union::PlayEffectReturn(_float fMultiplier, _float4x4 matWorld)
{
	if (m_eEffectUnionState != EFFECT_UNION_SLEEP)
		assert(false);

	m_fMultiplier = fMultiplier;

	m_matFixWorld = matWorld;

	m_pTransformCom->Set_WorldMatrix(m_matFixWorld);

	m_eEffectUnionState = EFFECT_UNION_START;

	return static_pointer_cast<CEffect_Union>(shared_from_this());
}

shared_ptr<CEffect_Union> CEffect_Union::PlayEffectReturn(_float fMultiplier, _float4x4* matWorld)
{
	if (m_eEffectUnionState != EFFECT_UNION_SLEEP)
		assert(false);

	m_fMultiplier = fMultiplier;

	m_pMatWorldPtr = matWorld;

	m_pTransformCom->Set_WorldMatrix(*m_pMatWorldPtr);

	m_eEffectUnionState = EFFECT_UNION_START;

	return static_pointer_cast<CEffect_Union>(shared_from_this());
}

shared_ptr<CEffect_Union> CEffect_Union::PlayEffectReturn(_float fMultiplier, _float4x4 matWorld, _float _fMoveSpeed, _float3 _vMoveDir)
{
	if (m_eEffectUnionState != EFFECT_UNION_SLEEP)
		assert(false);

	m_fMultiplier = fMultiplier;

	m_matFixWorld = matWorld;

	m_pTransformCom->Set_WorldMatrix(m_matFixWorld);

	m_fMoveSpeed = _fMoveSpeed;
	m_vMoveDir = _vMoveDir;

	m_eEffectUnionState = EFFECT_UNION_START;

	return static_pointer_cast<CEffect_Union>(shared_from_this());
}

void CEffect_Union::UnionEffectCreate()
{
	string szBinFilePath = "../../Binary/";
	szBinFilePath += m_strEffectUnionKey;
	szBinFilePath += ".bin";

	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	//Failed To File Open
	assert(is.is_open());
#endif // _DEBUG

	UNION_EFFECT_DATA sUnionData;
	sUnionData.Load_FromBinary(is);

	is.close();

	Make_UnionEffects(sUnionData);
}

void CEffect_Union::Make_UnionEffects(UNION_EFFECT_DATA _sUnionEffectDat)
{
	for (_uint i = 0; i < _sUnionEffectDat.iNumEffectDatas; ++i)
	{
		shared_ptr<CEffect_Base> pEffect = static_pointer_cast<CEffect_Base>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), wstring().assign(_sUnionEffectDat.vEffectDatas[i]->szPrototypeTag.begin(), _sUnionEffectDat.vEffectDatas[i]->szPrototypeTag.end())));
		_sUnionEffectDat.Make_UnionEffectData(pEffect, i);
		pEffect->Set_ParentUnion(dynamic_pointer_cast<CEffect_Union>(shared_from_this()));

		pEffect->Dead();

		m_vecEffects.push_back(pEffect);
	}

	_float fTimeMaxCheck = 0.f;

	for (auto& pTempEffect : m_vecEffects)
	{
		pTempEffect->Set_EffectLink(true);
		pTempEffect->Set_EffectLinkTime(&m_fEffectUnionTime);
		pTempEffect->EffectEnd();
		pTempEffect->Set_PlayOnce(false);

		if (pTempEffect->GetEffectTimeMax() > fTimeMaxCheck) {
			fTimeMaxCheck = pTempEffect->GetEffectTimeMax();
			m_fEffectUnionMaxTime = fTimeMaxCheck;
		}
	}
}

void CEffect_Union::SetColorAll(const _float4& vColor)
{
	for (auto& pEffect : m_vecEffects) 
	{
		pEffect->SetColorPresetRGB(vColor, vColor, vColor);
	}
}

void CEffect_Union::SetColorPalette(const _float4& vColor)
{
	for (auto& pEffect : m_vecEffects)
	{
		pEffect->SetEffectBaseColorToColor(vColor);
	}
}

void CEffect_Union::SetGlowOff()
{
	for (auto& pEffect : m_vecEffects)
	{
		pEffect->SetUseGlow(false);
	}

}


shared_ptr<CEffect_Union> CEffect_Union::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CEffect_Union
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CEffect_Union(pDevice, pContext) {}
	};

	shared_ptr<CEffect_Union> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Union"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CEffect_Union::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CEffect_Union
	{
		MakeSharedEnabler(const CEffect_Union& rhs) : CEffect_Union(rhs) {}
	};

	shared_ptr<CEffect_Union> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Union"));
		assert(false);
	}

	return pInstance;
}

void CEffect_Union::Free()
{
}