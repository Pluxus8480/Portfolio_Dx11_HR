#pragma once
#include "Effect_Manager.h"
#include "Gameinstance.h"
#include "Effect_Union.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{
}

CEffect_Manager::~CEffect_Manager()
{
	Free();
}

void CEffect_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CEffect_Manager::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Manager::Tick(_float fTimeDelta)
{
	DebugMode();
}

void CEffect_Manager::LateTick(_float fTimeDelta)
{
}

void CEffect_Manager::Tick_Engine(_float fTimeDelta)
{
	PriorityTick(fTimeDelta);
	Tick(fTimeDelta);
	LateTick(fTimeDelta);
}

void CEffect_Manager::Render()
{
}

void CEffect_Manager::Add_Union_Effect(const string& _strKey, _uint _iPoolNum)
{
	if (m_mapEffects.find(_strKey) == m_mapEffects.end())
	{
		for (_uint i = 0; i < _iPoolNum; ++i) {
			CEffect_Union::EffectUnionDesc desc = {};
			desc.m_strEffectUnionKey = _strKey;

			shared_ptr<CEffect_Union> pEffect = static_pointer_cast<CEffect_Union>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("CEffect_Union"), &desc));

			pEffect->Dead();

			m_mapEffects[_strKey].push_back(pEffect);
		}
	}
}

//shared_ptr<class CEffect_Union> CEffect_Manager::Call_Union_Effect(const string& _strKey, _float4x4 _worldMat, _float _multiplier)
//{
//	if (m_mapEffects.find(_strKey) != m_mapEffects.end())
//	{
//		for (auto& pEffect : m_mapEffects[_strKey])
//		{
//			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_SLEEP)
//			{
//				pEffect->PlayEffect(_multiplier, _worldMat);
//
//				return pEffect;
//			}
//		}
//	}
//
//	return nullptr;
//}

void CEffect_Manager::Call_Union_Effect(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard)
{
	_float _fmultiplier = _multiplier;

	if (0 == strcmp(_strKey.c_str(), "Mimosa_Chai_Izime")) _fmultiplier = 1.f;

	if (m_mapEffects.find(_strKey) != m_mapEffects.end())
	{
		for (auto& pEffect : m_mapEffects[_strKey])
		{
			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_SLEEP)
			{
				pEffect->PlayEffect(_fmultiplier, _worldMat);
				pEffect->SetBillboard(_bBillboard);

				GAMEINSTANCE->Push_Object(LEVEL_STATIC, TEXT("Layer_Effect"), pEffect);
				pEffect->Set_Dead(false);

				return;
			}
		}
	}
}

void CEffect_Manager::Call_Fixed_Union_Effect(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard)
{
	_float4x4 FixedworldMat = *(_worldMat);

	HRESULT result = ExceptionControler(_strKey);

	if (result == E_FAIL)
		return;

	if(result == E_ABORT)
		FixedworldMat._42 = -2.f;

	if (m_mapEffects.find(_strKey) != m_mapEffects.end())
	{
		for (auto& pEffect : m_mapEffects[_strKey])
		{
			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_SLEEP)
			{
				pEffect->PlayEffect(_multiplier, FixedworldMat);
				pEffect->SetBillboard(_bBillboard);

				GAMEINSTANCE->Push_Object(LEVEL_STATIC, TEXT("Layer_Effect"), pEffect);
				pEffect->Set_Dead(false);

				return;
			}
		}
	}
}

shared_ptr<class CEffect_Union> CEffect_Manager::Call_Union_Effect_Return(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard)
{
	if (m_mapEffects.find(_strKey) != m_mapEffects.end())
	{
		for (auto& pEffect : m_mapEffects[_strKey])
		{
			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_SLEEP)
			{
				pEffect->PlayEffect(_multiplier, _worldMat);
				pEffect->SetBillboard(_bBillboard);

				GAMEINSTANCE->Push_Object(LEVEL_STATIC, TEXT("Layer_Effect"), pEffect);
				pEffect->Set_Dead(false);

				return pEffect;
			}
		}
	}

	return nullptr;
}

shared_ptr<class CEffect_Union> CEffect_Manager::Call_Fixed_Union_Effect_Return(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard)
{
	_float4x4 FixedworldMat = *(_worldMat);

	if (m_mapEffects.find(_strKey) != m_mapEffects.end())
	{
		for (auto& pEffect : m_mapEffects[_strKey])
		{
			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_SLEEP)
			{
				pEffect->PlayEffect(_multiplier, FixedworldMat);
				pEffect->SetBillboard(_bBillboard);

				GAMEINSTANCE->Push_Object(LEVEL_STATIC, TEXT("Layer_Effect"), pEffect);
				pEffect->Set_Dead(false);

				return pEffect;
			}
		}
	}

	return nullptr;
}

shared_ptr<class CEffect_Union> CEffect_Manager::Call_Union_Effect_MoveObj(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _float _fMoveSpeed, _float3 _vMoveDir, _bool _bBillboard)
{
	_float4x4 FixedworldMat = *(_worldMat);

	if (m_mapEffects.find(_strKey) != m_mapEffects.end())
	{
		for (auto& pEffect : m_mapEffects[_strKey])
		{
			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_SLEEP)
			{
				pEffect->PlayEffect(_multiplier, FixedworldMat, _fMoveSpeed, _vMoveDir);
				pEffect->SetBillboard(_bBillboard);

				GAMEINSTANCE->Push_Object(LEVEL_STATIC, TEXT("Layer_Effect"), pEffect);
				pEffect->Set_Dead(false);

				return pEffect;
			}
		}
	}

	return nullptr;
}

void CEffect_Manager::Change_Union_Effect_Speed(const string& _strKey, _float _multiplier)
{
	if (m_mapEffects.find(_strKey) != m_mapEffects.end())
	{
		for (auto& pEffect : m_mapEffects[_strKey])
		{
			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_PLAY)
			{
				pEffect->SetUnionTimeMultiplier(_multiplier);
			}
		}
	}
}

void CEffect_Manager::Change_Union_Effect_Speed(_float _multiplier)
{
	for (auto& pair : m_mapEffects)
	{
		for (auto& pEffect : pair.second)
		{
			if (pEffect->GetEffectUnionState() == CEffect_Union::EFFECT_UNION_PLAY)
			{
				pEffect->SetUnionTimeMultiplier(_multiplier);
			}
		}
	}
}

void CEffect_Manager::Get_EffectKey(vector<string>& _vector)
{
	for (auto& pair : m_mapEffects)
	{
		_vector.push_back(pair.first);
	}
}

HRESULT CEffect_Manager::ExceptionControler(const string& _strKey)
{
	//Mimosa Map Exception Effect

	if (0 == strcmp(_strKey.c_str(), "Chai_Mimosa_Map_Decel") || 0 == strcmp(_strKey.c_str(), "Mimosa_Map_Decel") || 0 == strcmp(_strKey.c_str(), "Mimosa_Ground_Map_Decel")
	|| (0 == strcmp(_strKey.c_str(), "Mimosa_VSS")))
	{
		if(GAMEINSTANCE->Get_Current_LevelID() == LEVEL_MIMOSA || GAMEINSTANCE->Get_Current_LevelID() == LEVEL_EFFECTTOOL)
			return E_ABORT;

		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Manager::DebugMode()
{
}

void CEffect_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}