#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)


class CUi_EnemySpecialAttack : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_EnemySpecialAttackInfo : public CUi_2DInfo
	{
		_uint iParryCount;
		vector<_float> fBeatGap;
		vector<_bool> bParry;
		shared_ptr<CTransform> TargetTransform = { nullptr };
		_float2 fDistance = { 0, 0 };
		_float RealAttackGapBeat = { 8 };
	};


protected:
	CUi_EnemySpecialAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_EnemySpecialAttack(const CUi_EnemySpecialAttack& rhs);


public:
	virtual ~CUi_EnemySpecialAttack();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	void Initialize_Board();
	void Initialize_Parry(vector<_bool>& ParryVec);
	void Initialize_CreateBeatTime(vector<_float>& ParryGapVec);
	void Initialize_String();
	void Chase_Target();
	void Adjust_Pos();
	void Adjust_ParryActive();
	void Adjust_StringSize(_float fTimeDelta);
	void Increase_ParrySize(_float fTimeDelta);
	void Decrease_ParrySize(_float fTimeDelta);
	void Create_Part(_float RealAttackBeatGap);
	virtual void Dying() override;
	

private:
	shared_ptr<CTransform> m_pTargetTransform;
	shared_ptr<class CUi_Default> m_pBoard;
	shared_ptr<class CUi_Default> m_pString;
	vector<shared_ptr<class CUi_Default>> m_pParryVec;
	_uint m_iParryCount = { 0 };
	_float2 m_iOriginParrySize = { 70, 70 };
	_float2 m_iOriginNonParrySize = { 40, 40 };
	vector<_float> m_iParryCreateTimeVec;
	_bool m_bIncrese[10] = { true, true, true, true, true, true, true, true, true, true };
	_float2 m_fDistance = { 0, 0 };
	_bool m_bStringSize = { false };
	_float2 m_StringOriginSize = { 250, 100 }; 


public:							   
	static shared_ptr<CUi_EnemySpecialAttack> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
