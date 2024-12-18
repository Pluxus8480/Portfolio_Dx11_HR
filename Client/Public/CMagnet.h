#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"


BEGIN(Client)

class CMagnet : public CGimmickBase
{
public:
	static wstring ObjID;

protected:
	CMagnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagnet(const CMagnet& rhs);


public:
	~CMagnet();


public:
	void Set_MagnetUiActive(_bool Active);
	_float Magnet_DistancePlayer();
	void Player_UseMagnet();
	_bool Get_UseMagnet() const { return m_bUseMagnet; }

protected:
	void Initialize_Ui();
	

protected:
	shared_ptr<class CUi_MapInterAction> m_pUi;
	_bool m_bUseMagnet = { false };
	_int m_iUiRenderIndex = { 0 };
};

END