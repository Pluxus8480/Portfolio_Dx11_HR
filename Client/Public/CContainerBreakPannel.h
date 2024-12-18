#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"
#include "Boss_InteractObj.h"


class CContainerBreakPannel :  public CJeongGimmicBase, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CContainerBreakPannel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerBreakPannel(const CContainerBreakPannel& rhs);


public:
	~CContainerBreakPannel();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	shared_ptr<CModel> m_pAfterModelCom = { nullptr };
	_bool m_bChange = { false };


public:
	static shared_ptr<CContainerBreakPannel> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

