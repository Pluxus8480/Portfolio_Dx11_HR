#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_RekkaDecoEffect : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_RekkaDecoEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_RekkaDecoEffect(const CUi_RekkaDecoEffect& rhs);


public:
	virtual ~CUi_RekkaDecoEffect();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_EffectStartPos(_float fStartPos) { m_fStartPos = fStartPos; }


private:
	_float m_fStartPos = { 0 };


public:
	static shared_ptr<CUi_RekkaDecoEffect> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
