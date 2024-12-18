#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"
#include "Boss_InteractObj.h"

class CRobotBeamContainer :  public CJeongGimmicBase, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CRobotBeamContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobotBeamContainer(const CRobotBeamContainer& rhs);


public:
	~CRobotBeamContainer();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	virtual void Change_Phase(_uint iPhase) override;


private:
	_bool m_bPlay = { false };
	_bool m_bAnimEnd = { false };

public:
	static shared_ptr<CRobotBeamContainer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

