#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"


class CScroll2DContainer :  public CJeongGimmicBase
{
public:
	static wstring ObjID;

private:
	CScroll2DContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScroll2DContainer(const CScroll2DContainer& rhs);


public:
	~CScroll2DContainer();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Set_Anim(_uint iAnimIndex);
	virtual void Execute(_bool bExecute = true) override;


private:
	_bool m_bPlay = { false };
	_uint m_iCurAnimIndex = { 0 };
	const _float4x4* m_BonePtr = {};


public:
	static shared_ptr<CScroll2DContainer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

