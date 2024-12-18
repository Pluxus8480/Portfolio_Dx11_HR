#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"
#include "Boss_InteractObj.h"


class CRobotPunchStaticWall :  public CJeongGimmicBase, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CRobotPunchStaticWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobotPunchStaticWall(const CRobotPunchStaticWall& rhs);


public:
	~CRobotPunchStaticWall();

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
	shared_ptr<CModel> m_pBreakBeforeModelCom = { nullptr };
	shared_ptr<CModel> m_pBreakAfterModelCom = { nullptr };
	_uint m_iWallIndex = { 0 };
	_bool m_bRender = { true };


public:
	static shared_ptr<CRobotPunchStaticWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

