#pragma once
#include "Client_Defines.h"

BEGIN(Client)

class CEffect_Manager
{
	DECLARE_SINGLETON(CEffect_Manager)

private:
	CEffect_Manager();

public:
	~CEffect_Manager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);

	void Render();

public:
	void Add_Union_Effect(const string& _strKey, _uint _iPoolNum);
	//shared_ptr<class CEffect_Union> Call_Union_Effect(const string& _strKey, _float4x4 _worldMat, _float _multiplier);
	void Call_Union_Effect(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard = false);
	void Call_Fixed_Union_Effect(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard = false);

	shared_ptr<class CEffect_Union> Call_Union_Effect_Return(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard = false);
	shared_ptr<class CEffect_Union> Call_Fixed_Union_Effect_Return(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _bool _bBillboard = false);

	shared_ptr<class CEffect_Union> Call_Union_Effect_MoveObj(const string& _strKey, _float4x4* _worldMat, _float _multiplier, _float _fMoveSpeed, _float3 _vMoveDir, _bool _bBillboard = false);

	void Change_Union_Effect_Speed(const string& _strKey, _float _multiplier);
	void Change_Union_Effect_Speed(_float _multiplier);
	void Get_EffectKey(vector<string>& _vector);

private:
	HRESULT ExceptionControler(const string& _strKey);

public:
	void DebugMode();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	map<string, vector<shared_ptr<class CEffect_Union>>> m_mapEffects;

public:
	void Free();
};

END