#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "CUi_JudgeWord.h"

BEGIN(Client)

class CUi_EnemySpecialAttackPart : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_EnemySpecialAttackPartInfo : public CUi_2DInfo
	{
		_float ActiveBeat = { 0 };
		_uint CorrectBeat = { 2 };
		_float SafeCircleSize = { 300 };
		_float DangerCircleSize = { 100 };
	};
protected:
	CUi_EnemySpecialAttackPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_EnemySpecialAttackPart(const CUi_2D& rhs);


public:
	virtual ~CUi_EnemySpecialAttackPart();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

private:
	void Initialize_Icon(_float SafeCircleSize, _float DangerCircleSize);
	void Initialize_Speed();
	virtual void Dying() override;


private:
	shared_ptr<class CUi_Default> m_pDangerIcon;
	shared_ptr<class CUi_Default> m_pSafeIcon;
	_float m_fScaleUpSpeed = { 0 };
	_uint m_iActiveBeat = { 0 };

public:
	static shared_ptr<CUi_EnemySpecialAttackPart> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
