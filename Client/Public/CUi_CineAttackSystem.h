#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_CineAttackSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_CineAttackSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_CineAttackSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_CineAttackSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

private:
	void Macaron_ComboAttack();
	void Corsica_ComboAttack();
	void PepperMint_ComboAttack();
	void Cat808_ComboAttack();


public:
	static shared_ptr<CUi_CineAttackSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
