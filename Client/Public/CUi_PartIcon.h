#pragma once

#include "Client_Defines.h"
#include "CUi_Default.h"

BEGIN(Client)

class CUi_PartIcon : public CUi_Default
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_PartIconInfo : public CUi_2DInfo
	{
		_bool bRedIcon = { false };
	};


protected:
	CUi_PartIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_PartIcon(const CUi_PartIcon& rhs);


public:
	virtual ~CUi_PartIcon();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() override;


private:
	_bool m_bRedIcon = { false };


public:
	static shared_ptr<CUi_PartIcon> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
