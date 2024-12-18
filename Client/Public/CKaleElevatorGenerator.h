#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"


class CKaleElevatorGenerator :  public CJeongGimmicBase
{
public:
	static wstring ObjID;

private:
	CKaleElevatorGenerator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKaleElevatorGenerator(const CKaleElevatorGenerator& rhs);


public:
	~CKaleElevatorGenerator();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	_bool m_bTest = { false };


public:
	static shared_ptr<CKaleElevatorGenerator> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

