#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"
#include "Boss_InteractObj.h"

class CRailContainer : public CJeongGimmicBase, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CRailContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRailContainer(const CRailContainer& rhs);


public:
	~CRailContainer();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	virtual void Change_Phase(_uint iPhase);


private:
	shared_ptr<CModel> m_pAfterModelCom = { nullptr };
	_bool m_bBefore = { true };
	_bool m_bRender = { true };


public:
	static shared_ptr<CRailContainer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

