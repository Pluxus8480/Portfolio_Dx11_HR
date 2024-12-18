#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_MimosaBar : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_MimosaBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_MimosaBar(const CUi_2D& rhs);


public:
	virtual ~CUi_MimosaBar();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	void Set_Percent(_float Percent) { m_fPercent = Percent; }
	void Set_Damaged(_bool Dmged) { m_bDmged = Dmged; }


private:
	_float m_fPercent = { 0 };
	_bool m_bDmged = { false };


public:
	static shared_ptr<CUi_MimosaBar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
